from ContractObj import Contract
from RPC_Address import W3

private_key = ''
new_owner_address = ''

account = W3.eth.account.from_key(private_key)

gas_estimate = Contract.functions.changeReceiverAssets(new_owner_address).estimate_gas({'from': account.address})

transaction = Contract.functions.changeReceiverAssets(new_owner_address).build_transaction({
    'from': account.address,
    'gas': gas_estimate,
    'nonce': W3.eth.get_transaction_count(account.address)
})

signed_txn = W3.eth.account.sign_transaction(transaction, private_key)
transaction_hash = W3.eth.send_raw_transaction(signed_txn.rawTransaction)