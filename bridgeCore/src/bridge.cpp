/*******************************************************************************

 * This notice, including the copyright notice and permission notice, 
 * must be retained in all copies or substantial portions of the Software and 
 * in all derivative works.
 *
 * Software Name: CryptoDbSS / Ethereum bridge integration.
 * Copyright (C) 2025 Steeven J Salazar.
 * License: CryptoDbSS: Software Review and Audit License
 * 
 * https://github.com/CryptoDbSS/Eth-bridge
 *
 * IMPORTANT: Before using, compiling, or doing anything with this software,
 * you must read and accept the terms of the License provided with this software.
 *
 * If you do not have a copy of the License, you can obtain it at the following link:
 * https://github.com/CryptoDbSS/Eth-bridge/blob/main/LICENSE.md
 *
 * By using, compiling, or modifying this software, you implicitly accept
 * the terms of the License. If you do not agree with the terms,
 * do not use, compile, or modify this software.
 * 
 * This software is provided "as is," without warranty of any kind.
 * For more details, see the LICENSE file.

********************************************************************************/

#include "curlpostAsync.h"
#include "codec.h"
#include "key.h"
#include "pythonw3.h"
#include "hasher.h"
#include "cypher-aes.cpp"
#include "setting.h"
#include "DB.h"
#include <vector>
#include <forward_list>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdlib>

using namespace std;

//  g++ src/bridge.cpp -o bridge -I/usr/include/python3.11 -lboost_python311 -lpython3.11  -lssl -lcrypto -lcryptopp -lcurl -std=c++17

string NodeDir;
std::forward_list<string> hashTransaction;
vector<string> PendingTransacs;
string localaccount;
inline string recovered;


void signalHandler(int signum) {
    std::cout << "exit call (" << signum << ") .\n";
    recovered="";
    exit(signum);
}

void addStringInVector(vector<string> &vec, string datatocodify){
    for (unsigned int i = 0; i < datatocodify.length(); i += 2){
        vec.push_back(datatocodify.substr(i, 2));
    }
}

string vectorstring(vector<unsigned char> &vec){
    string msg = "";
    for (unsigned int i = 0; i < vec.size(); i++){
        msg += byteToHex(vec[i]);
    }
    return msg;
}

string SHAstg(string stg){

    vector<uint8_t> hashed = sha3_256(stg);
    string data66 =  vectorstring(hashed);
    for (auto &c:data66){c=toupper(c);}
    return data66;
}

string transacComfirmedCheck(){

    string hashblindex = curlpost("https://"+NodeDir+"/shaLBB", "", 2000);

    if(hashblindex.length() == 64 && HexCheck(hashblindex)){

        for(int i = 0 ; i<PendingTransacs.size(); i++){
            cout<<" 3 ) transacComfirmedCheck PendingTransacs[i].length() "<<PendingTransacs[i].length()<<endl;
            if(PendingTransacs[i].length() == 414 ){

                if(PendingTransacs[i].substr(350, 64) != hashblindex){

                    string typeindex = "2";
                    string jsonval = "{\"typeIndex\": \"" + typeindex + "\", \"valueC\": \"" + PendingTransacs[i].substr(286 , 64) + "\"}"; 

                    string response = curlpost("https://"+NodeDir+"/IndexTransaction", jsonval, 2000);

                    cout<<endl<<"transacComfirmedCheck response "<<response.length()<<" "<<byteVectorToHexStr(sha3_256v(HexStrToBytes(response)))<<" "<<PendingTransacs[i].substr(286 , 64)<<endl;
                    if (response.length()==430 && byteVectorToHexStr(sha3_256v(HexStrToBytes(response))) == PendingTransacs[i].substr(286 , 64)  ){

                        if(!Erasedatatransac(PendingTransacs[i])){
                            cout<< endl<< " ERROR transacComfirmedCheck() !Erasedatatransac(PendingTransacs[i]) PendingTransacs[i]) "+PendingTransacs[i]<<endl;
                            exit_call();
                        }
                        PendingTransacs.erase(PendingTransacs.begin() + i);
                        i--;
                        cout<<endl<<"Succefully bridge";
                        
                    } else if (response == "Not Found in DB"){

                        cout<<endl<<"check Transac is not found - it is assumed to have failed"<<endl;

                        PendingTransacs[i]= PendingTransacs[i].substr(0 , 222);
                        if(!SaveDataETH(PendingTransacs[i])){
                            exit_call();
                        }

                        if(!Erasedatatransac(PendingTransacs[i])){
                            exit_call();
                        }
                    }
                }
            }
        }
    }
    return "ok";
}

string CheckAndFirm(){

    extern vector<string> PendingTransacs;
    int intsize = PendingTransacs.size();
    string datatransac;


    for(int i = 0 ; i<intsize; i++){
    
        if(PendingTransacs[i].length()  == 286 ) {

            string response = curlpost("https://"+NodeDir+"/queuetransacs", PendingTransacs[i].substr(222 , 64), 2000);
            for (auto &s : response){s = toupper(s);}
            cout<<endl<<"CheckAndFirm response queuetransacs "<< response.length()<< " " << response<<endl;

            if(response.length() == 494){

                string sign = Signer(recovered, response);
                datatransac = response + sign;
                string hashbl = response.substr(366 , 64 );

                 string lowerTransaction = datatransac;
                for (auto &c : lowerTransaction) {
                    c = tolower(c);
                }

                cout<<endl<<"lowerTransaction.substr(0,392)+sign"<< lowerTransaction.substr(0,302)+sign<<endl<<"SHAstg(datatransac) "<<SHAstg(datatransac)<<endl<<" PendingTransacs[i].length() "<<PendingTransacs[i].length()<<endl ;



                PendingTransacs[i] += byteVectorToHexStr(sha3_256v(HexStrToBytes(lowerTransaction.substr(0,302)+sign)))+hashbl;

                cout<<endl<<" Check and sign "<<PendingTransacs[i]<<endl;

                if(!SaveDataTransac(PendingTransacs[i])){
                    exit_call();
                }

        /////////////////////////////////////////////////////////////////////

                if(!EraseDataETH(PendingTransacs[i])){
                    exit_call();
                }

                string jsonval = "{\"x\": \"" + response+sign + "\"}";

                //transaccion enviada al nodo
                response = curlpost("https://"+NodeDir+ "/TransacSignedPost", jsonval, 2000);
                cout << endl<< "transacsignespost response"<< response<<endl;


            } else {
                if(response == "PROCESSING"){
                    continue;
                } else {
                    PendingTransacs[i] = PendingTransacs[i].substr(0 , 222);
                }
            }
  
            //
  
        }
    }
    return "something";
}

string getDataTransac(string toDir, string value, int indexPending){

    cout<<endl<<"getDataTransac()"<<endl;

    string response = curlpost("https://"+NodeDir+ "/shaLBB", "", 2000);

    string shaLBB = response;
    
    if(response.length()== 64){

        string shaLBB = response;
        string bltype = "0x00";

        string stg1= shaLBB+bltype+localaccount+toDir+value;
        string signature = Signer(recovered, stg1);
        string jsonval = "{\"v\": \"" + bltype + "\", \"w\": \"" + localaccount + "\", \"x\": \"" + toDir + "\", \"y\": \"" + value + "\", \"z\": \"" + signature + "\"}"; 
        response = curlpost("https://"+NodeDir+ "/GetDataTransac", jsonval, 2000);
        cout<<endl<<"response getdatatransac "<<response<<endl;

        if(response.length()== 64){
            PendingTransacs[indexPending]+=response;
            return "transfer ok";
        } else {
            return "request  getdatatransac fail "; 
        }


    } else {  
        return "request shalbb getdatatransac fail "; 
    }

    return "transfer ok";

    exit_call();
    return "error db";

}

void  main_thread(){

    vector<string> pythonResult;
    uint64_t LatestEthBl = 0;
    
    while(true){ 

        pythonResult.clear();

        if(LatestEthBl > LatestIndexedEthbl){

            pythonResult = EthEventBridge(LatestIndexedEthbl);

            for (int i = 0; i<pythonResult.size();i++){
                
                cout<<endl<<"event: "<<i<<" "+pythonResult[i];

                if (pythonResult[i].length() == 222 ){

                    PendingTransacs.push_back(pythonResult[i]);
                    if(!SaveDataETH(PendingTransacs[PendingTransacs.size()-1])){
                        exit_call();
                    }

                    cout <<endl<<"debug event contrac "<<hexToULL(pythonResult[i].substr(190, 16))<<endl;
                } else {
                    if (pythonResult[0].length() == 16 && pythonResult.size() == 1 ){ 

                    
                    } else {
                        cout<<endl<<"error main ! pythonResult[0].length() == 16 && pythonResult.size() == 1"<<endl;
                        exit_call();
                    }
                }

                if(i == pythonResult.size()-1 ){
                    LatestIndexedEthbl++;
                    SaveLatestIndexedEthBlIndex(ullToHex(LatestIndexedEthbl));
                }

            }

        }

        for (int i = 0; i<PendingTransacs.size();i++){
            if (PendingTransacs[i].length() == 222 ){
                getDataTransac(PendingTransacs[i].substr(44,130), PendingTransacs[i].substr( 174 , 16 ), i );
            }
        } 

        CheckAndFirm();
        transacComfirmedCheck();

        LatestEthBl = latestEthBl();

        cout<<" LatestIndexedEthbl "<<LatestIndexedEthbl<< " LatestEthBl "<<LatestEthBl<<endl;

        if(LatestIndexedEthbl ==  LatestEthBl || LatestEthBl == 0){
            std::this_thread::sleep_for(std::chrono::seconds(15));
        }

    }

}





int main(int argc, char* argv[]){

    signal(SIGINT, signalHandler);

    cout<<endl<<"  CryptoDbSS / Ethereum bridge  "<<endl;
    cout<<endl<<" =  Software is developed by:  =";
    cout<<endl<<"     #####################    ";                            
    cout<<endl<<"                              ";             
    cout<<endl<<"    Steevenjavier@gmail.com   ";                  
    cout<<endl<<"                              ";                              
    cout<<endl<<"        CriptoDbSS   ";                            
    cout<<endl;                                                               
    cout<<endl; 

    if(argc>1){

        string option = argv[1];
        if(SetOptions(option, argc, argv)){
            cout<<endl<<"set option successfully"<<endl;
        } else{
            cout<<endl<<"error setting parameter"<<endl;
        }

        return 0;
    }

    NodeDir = LoadNodeAddress();

    decryptLocalAccount();

    //Load the request for previous transactions before the bridge application was closed.
    loadDataPrevTransactions();
    LatestIndexedEthbl = hexToULL(readlatestEthBlockNumber());

    cout<<endl<<"latest indexed eth block number: "<<LatestIndexedEthbl<<endl;

    thread thread1(main_thread);
    thread1.join();

    return 0;

}
