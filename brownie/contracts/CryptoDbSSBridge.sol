// SPDX-License-Identifier: MIT

pragma solidity ^0.8.0;

contract CryptoDbSSBridge{

    string public about;
    address payable public AssetsReceiver;
    address private Admin;
    bool public working;
    uint256 public MinValTransaction;
    
    event Purchase (address indexed buyer, uint256 amount, string product);
    event changeAssetsReceiver (address  oldAssetsReceiver, address  newAssetsReceiver);
    event powerSwitch (bool OnOff);
    event AdminSet(address indexed oldAdmin, address indexed newAdmin);

    constructor(bool _working, string memory _about, uint256 _MinValTransaction) {
        about = _about;
        AssetsReceiver = payable(msg.sender);
        working = _working;
        MinValTransaction = _MinValTransaction;
        Admin = msg.sender; 
        emit AdminSet(address(0), Admin);
    }

    function checkStringLength(string memory product) internal returns (bool) {
        bytes memory productBytes = bytes(product);
        return productBytes.length == 132;
    }

    function isHexString(string memory _hexString) internal pure returns (bool) {

        bytes memory bstr = bytes(_hexString);
        if (bstr.length == 0) {
            return false; // Empty string is not considered valid hex
        }

        if (bstr[0] == "0" && bstr.length > 1) {
            if (bstr[1] != "x" && bstr[1] != "X") {
                return false; // Hex strings should start with "0x" or "0X"
            }
        } else { //if it doesn't start with 0x or 0X, it is not hex.
            return false;
        }

        for (uint256 i = 2; i < bstr.length; i++) {
            bytes1 char = bstr[i];
            if (
                (char >= "0" && char <= "9") ||
                (char >= "a" && char <= "f") ||
                (char >= "A" && char <= "F")
            ) {
                continue;
            } else {
                return false;
            }
        }
        return true;
    }

    function infoAbout() public view returns (string memory) {
        return about;
    }

    function uint2str(uint256 _i) internal pure returns (string memory _uintAsString) {

        if (_i == 0) {
            return "0";
        }
        uint256 j = _i;
        uint256 len;
        while (j != 0) {
            len++;
            j /= 10;
        }
        bytes memory bstr = new bytes(len);
        uint256 k = len;
        while (_i != 0) {
            k = k - 1;
            uint8 temp = (48 + uint8(_i % 10));
            bytes1 b1 = bytes1(temp);
            bstr[k] = b1;
            _i /= 10;
        }
        return string(bstr);

    }
    
    function buy(string memory CryptoDbSSAddress) external payable Working {
        require(checkStringLength(CryptoDbSSAddress) == true && isHexString(CryptoDbSSAddress) == true, unicode"Invalid Address Provided");
        require(msg.value >= MinValTransaction, string(
                abi.encodePacked(
                    "The transaction does not have the minimum value. Min value is ",
                    uint2str(MinValTransaction)
                    )
                )
        );
        AssetsReceiver.transfer(msg.value);
        emit Purchase(msg.sender, msg.value, CryptoDbSSAddress);
    }

    function changeReceiverAssets(address newAssetsReceiver) public onlyAdmin {
        address oldAssetsReceiver =AssetsReceiver;
        AssetsReceiver = payable(newAssetsReceiver);
        emit changeAssetsReceiver(oldAssetsReceiver, AssetsReceiver);
    }

    function switchSM(bool w) public onlyAdmin {
        emit powerSwitch(w);
        working = w;
    }

    function changeAdmin(address newAdmin) public onlyAdmin {
        Admin = newAdmin;
        emit AdminSet(Admin, newAdmin);
    }

    function ShowAdmin() external view returns (address) {
        return Admin;
    }

    function SetMinValTransaction(uint256 _MinValTransaction) public onlyAdmin {
        MinValTransaction = _MinValTransaction;
    }
    function ShowMinValTransaction() external view returns (uint256) {
        return MinValTransaction;
    }

    modifier onlyAdmin() {
        require(msg.sender == Admin, "You don't have privileges for this call");
        _;
    }

    modifier Working() {
        require(working == true, "this contract is off");
        _;
    }

}
