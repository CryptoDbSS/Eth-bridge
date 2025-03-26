import json
from RPC_Address import W3

compiled_contract_path = '../brownie/build/contracts/CryptoDbSSBridge.json'
# Deployed contract address (see `migrate` command output: `contract address`)
deployed_contract_address = 'enter the contract address'

with open(compiled_contract_path) as file:
    contract_json = json.load(file)  # load contract info as JSON
    contract_abi = contract_json['abi']  # fetch contract's abi - necessary to call its functions

Contract = W3.eth.contract(address=deployed_contract_address, abi = contract_abi)
