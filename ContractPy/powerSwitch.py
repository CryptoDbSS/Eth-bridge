from RPC_Address import W3
from ContractObj import Contract

private_key = ''
switch = True
account = W3.eth.account.from_key(private_key)
print("calling transact from "+account.address)
gas_estimate = Contract.functions.switchSM(switch).estimate_gas({'from': account.address})

transaction = Contract.functions.switchSM(switch).build_transaction({
    'from': account.address,
    'gas': gas_estimate,
    'nonce': W3.eth.get_transaction_count(account.address)
})

signed_txn = W3.eth.account.sign_transaction(transaction, private_key)
transaction_hash = W3.eth.send_raw_transaction(signed_txn.rawTransaction)

print (transaction_hash)