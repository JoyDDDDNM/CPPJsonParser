#include <iostream>
#include <fstream>
#include "JObject.h"
#include "Parser.h"
#include "../benchmark/Timer.hpp"

using namespace json;

struct Base {
    int pp;
    string qq;

    START_TO_JSON
        to("pp") = pp;
        to("qq") = qq;
    END_TO_JSON

    START_FROM_JSON
        pp = from("pp", int);
        qq = from("qq", string);
    END_FROM_JSON
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
    Mytest test{.id = 32, .name = "fda"};
    auto item = Parser::FromJson<Mytest>(R"({"base":{"pp":0,"qq":""},"id":32,"name":"fda"} )");     // convert to user-defined class
    std::cout << Parser::ToJSON(item);                                                              // convert back to json string
}

void test_string_parser() {
    std::ifstream fin(R"(../test_source/test_out_nl.json)");
    if (!fin) {
        std::cout << "read file error";
        return;
    }
    
    std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    
    {
        // start timer to count the time for coversion
        Timer t;
        auto object = json::Parser::FromString(text);

        std::cout << (object["[css]"].to_string()) << "\n";
    }
}

int main() {
    test_class_serialization();
    // test_string_parser();
}