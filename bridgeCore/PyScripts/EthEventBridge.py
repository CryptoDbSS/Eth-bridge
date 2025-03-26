from PyScripts.ContractObj import Contract
from PyScripts.rpc_connection import W3

def latestEthBlockN():
    return W3.eth.get_block_number()

def eventsbuydw(fromBlock):

    events_filter = Contract.events.Purchase.create_filter(fromBlock=fromBlock, toBlock=fromBlock)
    events = events_filter.get_all_entries()  

    vector = []

    print("Event view : " )
    print("Event size  : " +str( len(events) ) )

    for event in events:

        block_number = event['blockNumber']
        transaction_hash = event['transactionHash']
        log_index = event['logIndex']

        print(f'Event at the block {block_number}, transaction hash: {transaction_hash}, log Index: {log_index}')

        event_data = event['args']
        buyer = event_data['buyer']
        product = event_data['product']
        amount = event_data['amount']
        amount_hex = format(amount, '016x')
        block_number_hex = format(block_number, '016x')
        log_index_hex = format(log_index, '016x')

        if len(buyer) == 42  and len(product) == 132 and len(amount_hex) == 16 and len(block_number_hex) == 16 and len(log_index_hex) == 16:
                    
            print("Buyer:", buyer, "acct:", product, "Amount:", amount)
            vector.append((str("00") +str(buyer[2:]) +str("00") + str(product[2:]) + str(amount_hex)+ str(block_number_hex)+ str(log_index_hex)).upper())
         
        else:

            print("No se cumple ninguna de las condiciones.")

    print("end event view")

    # manejar error en response de ETH para no saltar un bloque que no se pudo indexar

    if len(events)<1:
        vector.append(format(0, '016x'))

    return vector

