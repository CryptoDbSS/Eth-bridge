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

#include <iostream>
#include <boost/python.hpp>
#include <vector>

// g++ callpython.cpp -o callpython -I/usr/include/python3.10 -lboost_python310 -lpython3.10

using namespace std;

vector<string> convert_python_list_to_vector(boost::python::object py_list) {

    vector<string> result;
    boost::python::ssize_t length = boost::python::len(py_list);
  
    for (boost::python::ssize_t i = 0; i < length; ++i) {
        string element = boost::python::extract<string>(py_list[i]);
        result.push_back(element);
    }

    return result;
}

vector<string> EthEventBridge(int arg){

    namespace py = boost::python;
    Py_Initialize();
    py::object main_module = py::import("__main__");
    py::object main_namespace = main_module.attr("__dict__");

    std::vector<string> cpp_vector;

    try {

        py::object sys = py::import("sys");
        py::object sys_path = sys.attr("path");
        sys_path.attr("insert")(0, "");
        py::exec_file("PyScripts/EthEventBridge.py", main_namespace);
        py::object result = main_namespace["eventsbuydw"](arg);

        cpp_vector = convert_python_list_to_vector(result);

        return cpp_vector;

    } catch (const py::error_already_set& e) {
            PyErr_Print();
            return cpp_vector;
    }

    return cpp_vector;
}

uint64_t latestEthBl(){

    uint64_t bl= 0;
    namespace py = boost::python;
    Py_Initialize();
    py::object main_module = py::import("__main__");
    py::object main_namespace = main_module.attr("__dict__");

    try {

        py::object sys = py::import("sys");
        py::object sys_path = sys.attr("path");
        sys_path.attr("insert")(0, "");
        py::exec_file("PyScripts/EthEventBridge.py", main_namespace);
        py::object result = main_namespace["latestEthBlockN"]();
        bl  = py::extract<uint64_t>(result);

        return bl;

    } catch (const py::error_already_set& e) {
            PyErr_Print();
            return bl;
    }

    return bl;

}


