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
    
    
    //g++ server.cpp -o bridge_interface_http -lpthread -DCROW_ENABLE_SSL -lssl -lcrypto -std=c++17

    #include<crow.h>
    #include<string>

    using namespace std;
    int port = 25025;
    
    int listener(){

        crow::SimpleApp Rest;

        CROW_ROUTE(Rest, "/")([](const crow::request &req) -> crow::response {

            // Render and return "index.html"
            auto page = crow::mustache::load("index.html");
            crow::mustache::context ctx;
            
            crow::response res;
            res.set_header("Content-Type", "text/html; charset=utf-8");
            
            crow::mustache::rendered_template rendered = page.render(ctx);
            std::string renderedString = rendered.dump();

            return crow::response(200, renderedString);
                
        });       

        CROW_ROUTE(Rest, "/css.css")([](const crow::request &req){
                

            crow::response res;

            // definicion del tipo de MIME . Mine le dice al navegador cual es el formato de la respuesta
            res.add_header("Content-Type", "text/css");
    
            // read local .css file
            std::ifstream file("templates/css.css");
            if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();

            // Escribe el contenido del archivo en la respuesta
            res.write(buffer.str());
            } else {
            // Maneja el caso en el que no se pueda abrir el archivo
            res.code = 500; // Código de estado de error interno del servidor
            res.write("Error al cargar el archivo JavaScript");
            }

            return res;

                

        });

        CROW_ROUTE(Rest, "/src/abi")([](const crow::request& req, crow::response& res) {
            
            res.add_header("Content-Type", "application/json");
            std::ifstream file("../brownie/build/contracts/CryptoDbSSBridge.json", std::ios::binary);

            if (file) {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                res.write(content);
                res.end();
            } else {
                res.code = 500;
                res.write("Error al leer el archivo");
                res.end();
            }

        });

        //app.loglevel(crow::LogLevel::Warning);
        auto _a = Rest.port(port).multithreaded().run_async();

        return 0;

    }


    int main(){
        listener();
        return 0;
    }
