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

#include <filesystem>
#include <vector>


#ifndef DB_H
#define DB_H

inline uint64_t LatestIndexedEthbl;

void exit_call(){
    cout<<"Error, Exit";
    exit(0);
} 

string readdatatransac(string patch){
    ifstream fileData("DB/checkTransac/"+patch, std::ios::binary | std::ios::ate);

    vector<unsigned char> bl2;
    
    if (!fileData.is_open()) {
        throw std::runtime_error("the file could not be opened");
    }

    std::streamsize size = fileData.tellg();
    fileData.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    
    if (!fileData.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("the file could not be read");
    }

    bl2.clear();
    bl2 = buffer;

    fileData.close();

    string bl = bytesToHexStr(bl2);

    if(bl.length() != 414 && HexCheck(bl) ){
        cout<<endl<<"error - readdatatransac() bl.length() != 414 && HexCheck(bl) "<<endl;
        return "error - Error reading db readDataEth ";
    }
    
    return bl;
}

string readDataEth(string patch){

    ifstream fileData("DB/ethEvent/"+patch, std::ios::binary | std::ios::ate);
    string pr;

    vector<unsigned char> bl2;
    
    if (!fileData.is_open()) {
         cout<<endl << "f2"<<endl;
        // Manejo de error si no se pudo abrir el archivo
        throw std::runtime_error("No se pudo abrir el archivo");
    }

    std::streamsize size = fileData.tellg();
    fileData.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    
    if (!fileData.read(reinterpret_cast<char*>(buffer.data()), size)) {
        // Manejo de error si no se pudo leer el archivo
        throw std::runtime_error("No se pudo leer el archivo");
    }

    bl2.clear();
    bl2 = buffer;

    fileData.close();

    string bl = bytesToHexStr(bl2);

    if(bl.length() != 222 && HexCheck(bl) ){
        cout<<endl<<"error - readDataEth() pr.length() != 254 && HexCheck(pr) "<<endl;
        return "error - Error reading db readDataEth ";
    }
    
    return bl;
}

string readlatestEthBlockNumber(){

    ifstream fileData("DB/latestEthBlockNumber", std::ios::binary | std::ios::ate);
    string pr;

    vector<unsigned char> bl2;
    
    if (!fileData.is_open()) {
        return ullToHex(LatestIndexedEthbl);
        throw std::runtime_error("No se pudo abrir el archivo");

    }

    std::streamsize size = fileData.tellg();
    fileData.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    
    if (!fileData.read(reinterpret_cast<char*>(buffer.data()), size)) {
        // Manejo de error si no se pudo leer el archivo
        throw std::runtime_error("No se pudo leer el archivo");
    }

    bl2.clear();
    bl2 = buffer;
    fileData.close();
    string bl = bytesToHexStr(bl2);
    if(bl.length() != 16 && HexCheck(bl) ){
        cout<<endl<<"error - readlatestEthBlockNumber() bl.length() != 16 && HexCheck(bl) "<<endl;
        return "error - Error reading db readlatestEthBlockNumber ";
    }
    
    return bl;
}

void loadDataPrevTransactions(){

    extern vector<string> PendingTransacs;
    filesystem::path directory = "DB/checkTransac/";
    vector<string> ethEvent;
    string filedata;

    LatestIndexedEthbl = hexToULL(readlatestEthBlockNumber());

    for (const auto &file : filesystem::directory_iterator(directory)) {
        filedata =readdatatransac(file.path().filename().string());
        if(filedata.length()== 414 && hexToULL(filedata.substr(190,16)) < LatestIndexedEthbl){
            PendingTransacs.push_back(filedata);
        } else { 
            cout<<endl<<"error loadDataTransac filedata.length()!= 414 "<<endl;
            exit_call();
        }
    }

    directory = "DB/ethEvent/";
    
    for (const auto &file : filesystem::directory_iterator(directory)) {
        string fullpatch = "DB/ethEvent/" + file.path().filename().string();
        filedata =readDataEth(file.path().filename().string());
        if(filedata.length()== 222 && hexToULL(filedata.substr(190,16)) < LatestIndexedEthbl){
            ethEvent.push_back(filedata);
        } else { 
            cout<<endl<<"error loadDataTransac filedata.length()!= 222  && hexToULL(filedata.substr(190,16)) < LatestIndexedEthbl "+filedata+" "<<hexToULL(filedata.substr(190,16))<<" "<< LatestIndexedEthbl<<endl;
            if (std::remove(fullpatch.c_str()) != 0) {
                std::perror("the file could not be removed");
                exit_call();
                return;
            }
        }
    }

    for(int i = 0; i < ethEvent.size(); i++){
        for (int u= 0; u < PendingTransacs.size(); u++){
            if (ethEvent[i]==PendingTransacs[u].substr(0,222)){
                ethEvent.erase(ethEvent.begin() + i);
                i--; 
                break;
            }
        }
    }
    for (int u= 0; u < ethEvent.size(); u++){
        PendingTransacs.push_back(ethEvent[u]);
    }
    ethEvent.clear();
}

bool Erasedatatransac(string Data) {

    string archivo = "DB/checkTransac/"+ Data.substr(190 , 16 )+"-"+Data.substr(206 , 16 );
    if (std::remove(archivo.c_str()) != 0) {
        cout<<endl<<"Error deleting checktransac " + archivo<<endl;
        return false;
    }
    cout<<endl<<"checktransac deleted: " + archivo<<endl;
    return true;
}

bool EraseDataETH(string Data) {

    string archivo = "DB/ethEvent/"+ Data.substr(190 , 16 )+"-"+Data.substr(206 , 16 );
    if (std::remove(archivo.c_str()) != 0) {
        std::perror("Error al eliminar el archivo");
        return false;
    }
    std::puts("Archivo eliminado exitosamente");
    return true;
}

bool SaveDataTransac(string Data) {

    if(Data.length() != 414 ){return false;}

    string filename = Data.substr(190 , 16 )+"-"+Data.substr(206 , 16 );

    vector<unsigned char> byteArray;
    addHexStringInVector(byteArray, Data);
    ofstream filedata("DB/checkTransac/"+filename, ios::binary | ios::out);
    if (!filedata) {return false;}
    for (unsigned int i = 0; i < byteArray.size(); i++){
        filedata.seekp(i);
        filedata.put(byteArray[i]);
    }
    filedata.close();

    if(readdatatransac(filename) != Data ){
        return false;}

    cout<<endl<<"savedatatransac writed: "<<filename<<" "<<readdatatransac(filename)<<endl;

    return true;
}

bool SaveDataETH(string Data){

    if(Data.length() != 222 ){
        cout<<endl<<"error = SaveDataETH() Data.length() != 222 "<<endl;
        return false;
    }

    string filename = Data.substr(190 , 16 )+"-"+Data.substr(206 , 16 );
    vector<unsigned char> byteArray;
    addHexStringInVector(byteArray, Data);

    ofstream filedata("DB/ethEvent/"+filename, ios::binary | ios::out);
    if (!filedata) {return false;}
    for (unsigned int i = 0; i < byteArray.size(); i++){
        filedata.seekp(i);
        filedata.put(byteArray[i]);
    }
    filedata.close();

    cout<<endl<<"readDataEth(filename) "<<readDataEth(filename)<<endl;

    if(readDataEth(filename) != Data ){
        cout<<endl<<"error = SaveDataETH() readdatatransac(filename) != Data "<<endl;
        return false;
    }
    return true;

}

bool SaveLatestIndexedEthBlIndex(string Data){

    if(Data.length() != 16 ){
        cout<<endl<<"error = SaveLatestIndexedEthBlIndex() Data.length() != 16 "<<endl;
        return false;
    }

    vector<unsigned char> byteArray;
    addHexStringInVector(byteArray, Data);

    ofstream filedata("DB/latestEthBlockNumber", ios::binary | ios::out);
    if (!filedata) {return false;}
    for (unsigned int i = 0; i < byteArray.size(); i++){
        filedata.seekp(i);
        filedata.put(byteArray[i]);
    }
    filedata.close();

    if(readlatestEthBlockNumber() != Data ){
        cout<<endl<<"error = SaveLatestIndexedEthBlIndex() readlatestEthBlockNumber() != Data "<<endl;
        return false;
    }
    return true;
}

#endif