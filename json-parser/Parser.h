#ifndef MYUTIL_PARSER_H
#define MYUTIL_PARSER_H

#include <string>
#include <string_view>
#include <sstream>

namespace json
{

// a set of macro as an interface for user to 

// convert class member to JObject    
#define FUNC_TO_NAME _to_json

// convert JObject to class member
#define FUNC_FROM_NAME _from_json

#define START_TO_JSON void FUNC_TO_NAME(json::JObject & obj) const{
#define to(key) obj[key]
// macro to add a customized struct (recursively call)
#define to_struct(key, struct_member) json::JObject tmp((json::dict_t())); struct_member.FUNC_TO_NAME(tmp); obj[key] = tmp
#define END_TO_JSON  }

#define START_FROM_JSON void FUNC_FROM_NAME(json::JObject& obj) {
#define from(key, type) obj[key].Value<type>()
// macro to get a customized struct from JObject (recursively call)
#define from_struct(key, struct_member) struct_member.FUNC_FROM_NAME(obj[key])
#define END_FROM_JSON }

    using std::string;
    using std::string_view;
    using std::stringstream;

    class JObject;

    class Parser
    {
    public:
        Parser() = default;

        // accept json format string and convert to a JObject
        static JObject FromString(string_view content);

        // accept any type and convert to Json format string
        template<class T>
        static string ToJSON(T const &src)
        {
            // basic type in json
            if constexpr(IS_TYPE(T, int_t))
            {
                JObject object(src);
                return object.to_string();
            } else if constexpr(IS_TYPE(T, bool_t))
            {
                JObject object(src);
                return object.to_string();
            } else if constexpr(IS_TYPE(T, double_t))
            {
                JObject object(src);
                return object.to_string();
            } else if constexpr(IS_TYPE(T, str_t))
            {
                JObject object(src);
                return object.to_string();
            }
            // if this is a user-defined class, it needs to have a customized method to assign its member
            // to JObject, then call to_string
            json::JObject obj((json::dict_t()));

            src.FUNC_TO_NAME(obj);
            return obj.to_string();
        }

        // accept a Json format string and convert to a variable of type T 
        template<class T>
        static T FromJson(string_view src)
        {
            JObject object = FromString(src);
            // basic type in Json 
            if constexpr(is_basic_type<T>())
            {
                return object.template Value<T>();
            }

            if (object.Type() != T_DICT)throw std::logic_error("not dict type fromjson");
            T ret;
            
            // call member method of customized class T to assign JObject member to T member variable
            ret.FUNC_FROM_NAME(object);
            return ret;
        }

        void init(string_view src);

        void trim_right();

        void skip_comment();

        bool is_esc_consume(size_t pos);

        char get_next_token();

        JObject parse();

        JObject parse_null();

        JObject parse_number();

        bool parse_bool();

        string parse_string();

        JObject parse_list();

        JObject parse_dict();

    private:
        // current json string
        string m_str;

        // index of current position
        size_t m_idx{};
    };
}


#endif //MYUTIL_PARSER_H
