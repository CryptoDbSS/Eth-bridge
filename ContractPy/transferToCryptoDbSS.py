

from ContractObj import Contract
from RPC_Address import W3

value = 1000000
private_key = ''
CryptoDbSSPublicAddress = ''
account = W3.eth.account.from_key(private_key)

print(W3.from_wei(value, 'wei'))
valuetransaction = W3.to_wei(value, 'wei')

nonce = W3.eth.get_transaction_count(account.address)

gas_estimate = Contract.functions.buy(CryptoDbSSPublicAddress).estimate_gas({'from': account.address,
    'value': valuetransaction})

transaction = Contract.functions.buy(CryptoDbSSPublicAddress).build_transaction({
    'from': account.address,
    'gas': gas_estimate,
    'nonce': nonce,
    'value': valuetransaction
})

signed_txn = W3.eth.account.sign_transaction(transaction, private_key)
transaction_hash = W3.eth.send_raw_transaction(signed_txn.rawTransaction)

print("hash transaction ", transaction_hash.hex())