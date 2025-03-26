from ContractObj import Contract

contract = Contract
result = contract.functions.infoAbout().call()

print (result)