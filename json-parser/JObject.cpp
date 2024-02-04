#include "JObject.h"

using namespace json;

void *JObject::value() {
    // get pointer to current type of m_value
    switch (m_type)
    {
        case T_NULL:
            return get_if<str_t>(&m_value);
        case T_BOOL:
            return get_if<bool_t>(&m_value);
        case T_INT:
            return get_if<int_t>(&m_value);
        case T_DOUBLE:
            return get_if<double_t>(&m_value);
        case T_LIST:
            return get_if<list_t>(&m_value);
        case T_DICT:
            return get_if<dict_t>(&m_value);
        case T_STR:
            return std::get_if<str_t>(&m_value);
        default:
            return nullptr;
    }
}

// convert value pointer to corresponding type of pointer, then reference it
#define GET_VALUE(type) *((type*) value)

string JObject::to_string() {
    // convert JObject to json string format
    void *value = this->value();
    std::ostringstream os;
    switch (m_type) {
        case T_NULL:
            os << "null";
            break;
        case T_BOOL:          
            if (GET_VALUE(bool))
                os << "true";
            else os << "false";
            break;
        case T_INT:
            os << GET_VALUE(int);
            break;
        case T_DOUBLE:
            os << GET_VALUE(double);
            break;
        case T_STR:
            os << '\"' << GET_VALUE(string) << '\"';
            break;
        case T_LIST: {
            list_t &list = GET_VALUE(list_t);
            os << '[';
            for (auto i = 0; i < list.size(); i++) {
                if (i != list.size() - 1) {
                    // recursively call to_string on each object
                    // Problem: this could cause stack overflow
                    os << ((list[i]).to_string());
                    os << ',';
                } else os << ((list[i]).to_string());
            }
            os << ']';
            break;
        }
        case T_DICT: {
            dict_t &dict = GET_VALUE(dict_t);
            os << '{';
            for (auto it = dict.begin(); it != dict.end(); ++it) {
                if (it != dict.begin()) // separate json  object by comma
                    os << ',';
                os << '\"' << it->first << "\":" << it->second.to_string(); 
                // this will recursively call to_string() if the dict has nested JObject 
            }
            os << '}';
            break;
        }
        default:
            return "";
    }
    
    return os.str();
}