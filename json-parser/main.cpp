#include <iostream>
#include <fstream>
#include "JObject.h"
#include "Parser.h"
#include "../benchmark/Timer.hpp"

using namespace json;

struct Base {
    int pp;
    string qq;

    // provide interface as macro to covert struct Base into JObject
    START_TO_JSON       // _to_json
        to("pp") = pp;
        to("qq") = qq;
    END_TO_JSON

    // provide interface as macro to covert JObject into struct Base 
    START_FROM_JSON     // _from_json
        pp = from("pp", int);
        qq = from("qq", string);
    END_FROM_JSON

    // we need to provide START_TO_JSON and START_FROM_JSON method so that the compiler would know how
    // to perform conversion between class member and JObject
};

struct Mytest {
    int id;
    std::string name;
    Base q;

    START_TO_JSON
        to_struct("base", q);
        to("id") = id;
        to("name") = name;
    END_TO_JSON

    START_FROM_JSON
        id = from("id", int);
        name = from("name", string);
        from_struct("base", q);
    END_FROM_JSON
};

void test_class_serialization() {
    // Mytest test{.id = 32, .name = "fda"};
    auto item = Parser::FromJson<Mytest>(R"({"base":{"pp":0,"qq":""},"id":32,"name":"fda"} )");     // convert to user-defined class
    std::cout << Parser::ToJSON(item);                                                              // convert back to json string
}

void test_string_parser(std::string file) {
    std::ifstream fin(file);
    if (!fin) {
        std::cout << "read file error";
        return;
    }
    
    std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    {
        // start timer to count the time for coversion
        Timer t;
        auto object = json::Parser::FromString(text);

        // std::cout << (object["[css]"].to_string()) << "\n";
    }
}

int main() {
    // test_class_serialization();
    test_string_parser(R"(../test_source/test.json)");
    test_string_parser(R"(../test_source/test_out.json)");
    test_string_parser(R"(../test_source/test_out_nl.json)");
}