# Introduction

This program only cover two classes: JObject and Parser.

## JObject

An intermediate class which holds Json data, to support converstion between user defined class and JObject, we provide a set of macro for user and user should add these into their class declaration.

```C++
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
```

**START_TO_JSON** : Macro to start coverting each class member into JObject, it would pass a JObject would hold a map<string, JObject>

**to("pp") = pp;** : This would add a key-value: {"pp", pp} into the map of JObject

**END_TO_JSON**: Macro to end conversion

**START_FROM_JSON**: Macro to start conversion from JObject to user defined class

**pp = from("pp", int);**: This would retrive value of an JObject and store into the member of user defined class

**END_FROM_JSON**: Macro to end conversion

## Json Parser

An intermediate component which support three functionalities

### Parser::FromString

Accept an Json format string (From file stream) and convert it into JObject

### Parser::ToJSON

Accept any type and convert into Json format string (From file stream)

### Parser::FromJson

accept a Json raw string (set by user) and convert into user defined class

## Benchmark

We provide a timer for performance testing, to use the timer, only thing you need to do is enclose the parsing code with a timer in a same scope. The timer would start counting when it is initialized.

## Problem

Our implementation relys on recursively calling parse() method in the parser, to parse the whole Json string. This means, once the structure of Json string is too complicated(too many nested levels), the program may crashs.
