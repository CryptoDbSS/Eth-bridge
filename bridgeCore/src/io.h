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

#ifndef IO_H
#define IO_H

bool saveFile(string patch, vector<unsigned char> &Data){

    ofstream filew(patch , ios::binary | ios::out);
    if (!filew) { return "error de escritura"; }
    
    for (unsigned int i = 0; i < Data.size(); i++){
        filew.seekp(i);
        filew.put(Data[i]);
    }
    filew.close();

    return true;
}    


vector<unsigned char> readFile(const std::string& filename) {

    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    try {

        if (!file.is_open()) {
            // Manejo de error si no se pudo abrir el archivo
            throw std::runtime_error("Could not open file "+filename);    
        }

    } catch (const std::exception& e) {
    
        file.close();
        std::cerr << "Error: " << e.what() << std::endl;
        vector<unsigned char> FileRead;
        return FileRead;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);

    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        file.close();
        // Manejo de error si no se pudo leer el archivo
        throw std::runtime_error("No se pudo leer el archivo");
    }

    file.close();
    return buffer;
}

time_t timing(){
    return time(nullptr);
}

#endif