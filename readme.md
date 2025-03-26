## CryptoDbSS / Ethereum bridge integration.

integrates a Ethereum based Blockchain to CryptoDbSS based one, reading  the smart contract bridge events and then the equivalent founds are transfer to CryptoDBSS 

Dependencies: 

        c++: crowcpp, openssl, cryptopp, lcurl, boost.

        python: web3.py, brownie.



this package contain 4 modules

        brownie: eth-brownie folder with the bridge smart contract and script to deploy.

        bridge-core: reads the  bridge smart contract and performs the correspondent transctions to CryptoDbSS.

        Contractpy: implementations to interact individually with the smart contract and some examples.

        server interface user gui: serve the gui via http to interact with the bridge from a wallet.


Instructions to buil

        bridgeCore :

                - cd ./bridgeCore
                - g++ src/bridge.cpp -o bridge -I/usr/include/python3.11 -lboost_python311 -lpython3.11  -lssl -lcrypto -lcryptopp -lcurl -std=c++17
                - cd ..

        server interface user gui:

                - cd ./server_interface_dapp
                - g++ server.cpp -o bridge_interface_http -lpthread -DCROW_ENABLE_SSL -lssl -lcrypto -std=c++17
                - cd ..

setting up

        deploying bridge smart contract:

                - open and edit Private_key definition in "/brownie/scripts/CryptoDbSSBridge.py" with the private key of the account that performs the deployment
                - cd ./brownie
                - brownie run CryptoDbSSBridge --network "choose the network provider for brownie deployment"
                - copy the addresse of deployment
                - open and edit deployed_contract_address definition in "/bridgeCore/PyScripts/ContractObj.py" with the address of the deployed contract

        setting the BridgeCore Parameters:

                private key: this is the address that will perform the transactions to CryptoDbSS, for settup run:

                        ./bridge setPrivateKey

                        enter the private key, next  the private key will be encrypted for secure storage, enter a pass for it, This will be required every time the app is run.

                CryptoDbSS node address: 

                        ./bridge setNodeAddress "Your CryptoDbSS Node Address"

                ethereum rpc: 

                        open and edit Provider definition in"/bridgeCore/PyScripts/rpc_connection.py" with the private key of the account that performs the deployment


                number from which ETH blocks will begin to be indexed
                
                        ./bridge setEthIndexBlFrom "number"


run 

        BridgeCore:

                cd ./BridgeCore
                ./bridge

        User GUI:

                cd ./server_interface_dapp
                ./bridge_interface_http

                default access : http://0.0.0.0:25025

# Get more Info And Updates

  </br>
<p align="center">Follow the blog <a href="https://cryptodbss.blogspot.com" > cryptodbss.blogspot.com </a> </p>

<p align="center">questions, suggestions or contact : Steevenjavier@gmail.com
</br></br>
 Copyright (C) 2025 Steeven J Salazar.
</p>








