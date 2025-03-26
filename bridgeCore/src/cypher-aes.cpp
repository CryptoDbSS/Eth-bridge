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


#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include<cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <iostream>
#include <string>
#include "codec.h"

using namespace CryptoPP;

std::string randomprng(){

    CryptoPP::AutoSeededRandomPool prng;
    std::string key;
    const int keySize = 16; // Tamaño de la clave en bytes

    // Generar clave aleatoria
    key.resize(keySize);
    prng.GenerateBlock(reinterpret_cast<CryptoPP::byte*>(&key[0]), keySize);

    // Imprimir clave en hexadecimal
    std::string encodedKey;
    CryptoPP::StringSource(key, true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(encodedKey)));

    return encodedKey;
}

bool EncryptAES(const std::string& plain, const std::string keyStr, const std::string ivStr, std::string& cipher) {
    try {
        SecByteBlock key((const CryptoPP::byte*)keyStr.data(), keyStr.size() / 2);
        SecByteBlock iv((const CryptoPP::byte*)ivStr.data(), ivStr.size() / 2);
        CBC_Mode<AES>::Encryption e;
        e.SetKeyWithIV(key, key.size(), iv);

        StringSource s(plain, true,
            new StreamTransformationFilter(e,
                new HexEncoder(
                    new StringSink(cipher)
                )
            )
        );

        return true;
    }
    catch (const Exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return false;
}

void DecryptAES(const std::string& cipher, const std::string keyStr, const std::string ivStr, std::string& recovered) {
    try {
        SecByteBlock key((const CryptoPP::byte*)keyStr.data(), keyStr.size() / 2);
        SecByteBlock iv((const CryptoPP::byte*)ivStr.data(), ivStr.size() / 2);
        CBC_Mode<AES>::Decryption d;
        d.SetKeyWithIV(key, key.size(), iv);

        std::string decodedCipher;
        
        CryptoPP::StringSource(cipher, true,
            new CryptoPP::HexDecoder(
                new CryptoPP::StringSink(decodedCipher)
            )
        );

        CryptoPP::StringSource(decodedCipher, true,
            new CryptoPP::StreamTransformationFilter(d,
                new CryptoPP::StringSink(recovered)
            )
        );
    }
    catch (const CryptoPP::Exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

std::string readData(string patch){

    std::ifstream fileData(patch, std::ios::binary | std::ios::ate);
    std::vector<unsigned char> bl2;
    
    if (!fileData.is_open()) {throw std::runtime_error("error reading data");}
    std::streamsize size = fileData.tellg();
    fileData.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    
    if (!fileData.read(reinterpret_cast<char*>(buffer.data()), size)) {throw std::runtime_error("error reading data");}

    bl2.clear();
    bl2 = buffer;

    fileData.close();

    std::string bl = bytesToHexStr(bl2);

/*
    if( HexCheck(bl) ){
        cout<<endl<<"error -  HexCheck "<<bl<<endl;
        return "error - Error reading db readDataEth ";
    }

    */
    
    return bl;
}

bool SaveEncrypted(std::string Data){

    std::vector<unsigned char> byteArray;
    cout<<endl<<"data encrypted to save : "<<Data<<endl;

    addHexStringInVector(byteArray, Data);

    std::string filename= "encrypted";

    ofstream filedata(filename, ios::binary | ios::out);
    if (!filedata) {return false;}
    for (unsigned int i = 0; i < byteArray.size(); i++){
        filedata.seekp(i);
        filedata.put(byteArray[i]);
    }
    filedata.close();

    if(readData(filename) != Data ){
        cout<<endl<<"error = SaveDataETH() readdatatransac(filename) != Data "<<readData(filename)<<endl;
        return false;
    }
    return true;

}