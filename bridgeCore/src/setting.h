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

#include "io.h"
#include "DB.h"
#include "hasher.h"
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <filesystem>

#ifndef SETTINGS_H
#define SETTINGS_H

std::string retrievePrivFromTerminal(string msg) {

    std::string password;
    termios oldt, newt;

    // Obtener la configuración actual de la terminal
    tcgetattr(STDIN_FILENO, &oldt);

    // Desactivar la visualización de caracteres
    newt = oldt;
    newt.c_lflag &= ~ECHO;

    // Aplicar la nueva configuración
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::cout << msg;

    std::getline(std::cin, password);

    // Restaurar la configuración original de la terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return password;
}

bool setKey(){

    std::string PvData = retrievePrivFromTerminal("enter privKey Of wallet:");
    cout<<endl;

    std::string encryptionKey = retrievePrivFromTerminal("enter a pass for encryption");
    cout<<endl;
    encryptionKey = hasher(encryptionKey);

    string cypherdata;

    string strIv= randomprng(); 

    if(!EncryptAES(PvData, encryptionKey, strIv, cypherdata )){
        return false;
    }

    string todecrypt;

    DecryptAES(cypherdata, encryptionKey, strIv, todecrypt);

    cypherdata = strIv+cypherdata;

    cout<<endl<<"cypherdata "<< cypherdata;

    std::vector<unsigned char> Data=  HexStrToBytes(cypherdata);

    return saveFile("./sets/private", Data);

}

void decryptLocalAccount(){

    string pass = retrievePrivFromTerminal("enter pass key");
    pass = hasher(pass);
    string readed = readData("./sets/private");
    extern string recovered;
    extern string localaccount;
    cout<<endl<<" data to decrypt "<< readed<<endl<<" iv "<<  readed.substr(32 , readed.length());
    DecryptAES( readed.substr(32 , readed.length()), pass, readed.substr(0,32), recovered);
    localaccount = derivate(recovered);

}

bool SetNodeAddress(string RPC_Address){

    vector<unsigned char> Data(RPC_Address.begin(), RPC_Address.end());
    if(!saveFile("./sets/NodeAddress", Data)){
        cout<<endl<<"Error SetNodeAddress !saveFile";
        return false;
    }
    cout<<endl<<"Privider Setted"<<endl;
    return true;

}

std::string LoadNodeAddress(){

    vector<unsigned char> dataRead;
    dataRead = readFile("./sets/NodeAddress");
    return std::string(dataRead.begin() , dataRead.end());
    
}

void displayInfo(){
    cout<<endl<<"invalid arguments call";
}

bool SetOptions(string &option, int argc, char* argv[]){

    if(option == "setPrivateKey"){
        return setKey();
    }

    if(option == "setEthIndexBlFrom"){
        string EthBlNumber = argv[2];

        if(!HexCheck(EthBlNumber)){
            cout<<endl<<"Data Format is invalid ";
            return false;
        }

        return SaveLatestIndexedEthBlIndex(ullToHex(stoull(EthBlNumber)));
    }

    if(option == "setNodeAddress"){
        if(argc == 3 ){
            return SetNodeAddress(argv[2]);
        }
        displayInfo();
    }

    return false;
}

#endif