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

#include <curl/curl.h>
#include <string>
#include <iostream>
#include <future>

using namespace std;

struct ResponseData {
    string data;
    long responseCode;
};

// Función para manejar la respuesta de la solicitud
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    string* response = reinterpret_cast<string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

string curlpost(string url, string postfield, int timeout_ms) {
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  string s;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); //verificar que las peticiones esten encriptadas
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfield.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      cerr <<endl<< "req fail: " << curl_easy_strerror(res);
      s = "00";
    }

    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();
  return s;
}

future<string>async_post_data(const string& url, const string& data, const int& timeout_ms) {
    return async(launch::async, curlpost, url, data, timeout_ms);
}

future<ResponseData> convert_future(future<string>&& future_string) {
    return async(launch::deferred, [](string str) -> ResponseData {
        ResponseData data;
        data.data = str;
        return data;
    }, move(future_string.get()));
}

