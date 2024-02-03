#include "JObject.h"
#include "Parser.h"
#include <cctype>
#include <algorithm>

using namespace json;

JObject Parser::FromString(string_view content) {
    // accept json format string and convert to a JObject
    // use the same parser instance instead of initializing a duplicate parser
    static Parser instance;
    instance.init(content);
    return instance.parse();
}

void Parser::init(std::string_view src) {
    // initialize parser
    m_str = src;
    m_idx = 0;
    trim_right(); // delete trailing spaces
}

void Parser::trim_right() {
    // delete trailing spaces, to determine the end of line
    m_str.erase(std::find_if(m_str.rbegin(), m_str.rend(), [](char ch) {
        return !std::isspace(ch);
    }).base(), m_str.end());
}

void Parser::skip_comment() {
    // start of a comment
    if (m_str.compare(m_idx, 2, R"(//)") == 0) {
        while (true) {
            auto next_pos = m_str.find('\n', m_idx);
            if (next_pos == string::npos) {
                throw std::logic_error("invalid comment area!");
            }
            // skip to next line to check if it is still a comment
            m_idx = next_pos + 1;
            while (isspace(m_str[m_idx])) {
                m_idx++;
            }
            if (m_str.compare(m_idx, 2, R"(//)") != 0) { // end of comment
                return;
            }
        }
    }
}

char Parser::get_next_token() {
    // skip leading white space at each line
    while (std::isspace(m_str[m_idx])) m_idx++;
    if (m_idx >= m_str.size())
        throw std::logic_error("unexpected character in parse json");
    // current line is comment, skip
    skip_comment();
    return m_str[m_idx];
}

bool Parser::is_esc_consume(size_t pos) {
    // count number of escape character to determine if the last " is escaped or not
    size_t end_pos = pos;
    while (m_str[pos] == '\\') pos--;
    auto cnt = end_pos - pos;
    // if the number of  \ is even，" is not being escaped，otherwise it is escaped
    return cnt % 2 == 0;
}

JObject Parser::parse() {
    char token = get_next_token();
    if (token == 'n') {
        return parse_null();
    }
    if (token == 't' || token == 'f') {
        return parse_bool();
    }
    if (token == '-' || std::isdigit(token)) {
        return parse_number();
    }
    if (token == '\"') {
        return parse_string();
    }
    if (token == '[') {
        return parse_list();
    }
    if (token == '{') {
        return parse_dict();
    }

    throw std::logic_error("unexpected character in parse json");
}

JObject Parser::parse_null() {
    if (m_str.compare(m_idx, 4, "null") == 0) {
        m_idx += 4;
        return {};
    }
    throw std::logic_error("parse null error");
}

JObject Parser::parse_number() {
    auto pos = m_idx;
    // negative integer, skip '-' 

    // parsing integer part
    if (m_str[m_idx] == '-') {
        m_idx++;
    }
    if (isdigit(m_str[m_idx]))
        while (isdigit(m_str[m_idx]))
            m_idx++;
    else {
        throw std::logic_error("invalid character in number");
    }

    if (m_str[m_idx] != '.') {
        // convert string object to c-style string (char array), 
        // then convert it again into long type
        return (int) strtol(m_str.c_str() + pos, nullptr, 10);
    }

    // parsing decimal part
    if (m_str[m_idx] == '.') {
        m_idx++;
        if (!std::isdigit(m_str[m_idx])) {
            throw std::logic_error("at least one digit required in parse float part!");
        }
        while (std::isdigit(m_str[m_idx]))
            m_idx++;
    }
    return strtod(m_str.c_str() + pos, nullptr);
}

bool Parser::parse_bool() {
    if (m_str.compare(m_idx, 4, "true") == 0) {
        m_idx += 4;
        return true;
    }
    if (m_str.compare(m_idx, 5, "false") == 0) {
        m_idx += 5;
        return false;
    }
    throw std::logic_error("parse bool error");
}

string Parser::parse_string() {
    // start index of string
    auto pre_pos = ++m_idx;

    // find the next occurence of string
    // If no matches were found, the function returns string::npos.
    auto pos = m_str.find('"', m_idx);
    if (pos != string::npos) {
        // keep parsing to find the index of end of string
        while (true) {
            // get to the end of string, stop parsing
            if (m_str[pos - 1] != '\\')  {
                break;
            }

            // check if the " is escaped by "\"
            if (is_esc_consume(pos - 1)) {
                break;
            }

            // find next " and check if it is the end of string
            pos = m_str.find('"', pos + 1);
            if (pos == string::npos) {
                throw std::logic_error(R"(expected left '"' in parse string)");
            }
        }
        m_idx = pos + 1;
        return m_str.substr(pre_pos, pos - pre_pos);
    }
    throw std::logic_error("parse string error");
}

JObject Parser::parse_list() {
    JObject arr((list_t()));
    m_idx++;
    char ch = get_next_token();
    if (ch == ']') {
        m_idx++;
        return arr;
    }

    while (true) {
        // recursively parse string ( there could be nested object or list )
        arr.push_back(parse());
        ch = get_next_token();
        if (ch == ']') {
            m_idx++;
            break;
        }

        if (ch != ',') {  // if there is no comma at the end ot json object, this is a json syntax error 
            throw std::logic_error("expected ',' in parse list");
        }
        // skip comma
        m_idx++;
    }

    return arr;
}

JObject Parser::parse_dict() {
    JObject dict((dict_t()));
    m_idx++;
    char ch = get_next_token();
    if (ch == '}') {
        m_idx++;
        return dict;
    }
    while (true) {
        // parse object key and convert to string type
        string key = std::move(parse().Value<string>());
        ch = get_next_token();
        if (ch != ':') {
            throw std::logic_error("expected ':' in parse dict");
        }
        m_idx++;

        // recursively parse whole string, to return JObject
        dict[key] = parse();
        ch = get_next_token();
        
        if (ch == '}') {
            m_idx++;
            break; // end of Json string
        }
        if (ch != ',') { // end of an object in Json, it must be a comma to go to next object
            throw std::logic_error("expected ',' in parse dict");
        }
        // current character is a comma, skip
        m_idx++;
    }
    return dict;
}
