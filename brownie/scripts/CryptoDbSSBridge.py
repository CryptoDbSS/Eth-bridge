from brownie import CryptoDbSSBridge, accounts

def main():

    Private_key = ""

    acct = accounts.add(Private_key)
    address = acct.address
    balance = acct.balance()

    print("Publc Address Ethereum:", address)
    print("Account balance:", balance)

    gas_estimate = CryptoDbSSBridge.deploy.estimate_gas( 1, "CryptoDbSS Interoperability Smart Contract Ethererum. Steeven Javier Salazar - steevenjavier@gmail.com, All rights reserved.", 100000 ,{"from": acct})
    print(f"Estimated gas: {gas_estimate}")

    constructor_params = {
        'gas_limit': gas_estimate 
    }

    contract = CryptoDbSSBridge.deploy( 1, "CryptoDbSS Interoperability Smart Contract Ethererum. Steeven Javier Salazar - steevenjavier@gmail.com, All rights reserved.", 100000 ,{'from': acct, **constructor_params})