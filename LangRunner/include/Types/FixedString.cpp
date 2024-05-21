#ifndef FIXED_STRING_SIZE
    #define FIXED_STRING_SIZE 20
#endif

#include <iostream>
#include <cstring>

using namespace std;

namespace Muck {
    class FixedString {
        public:
        char content[FIXED_STRING_SIZE];
        FixedString(string str) {
            fill(content, content + 20, 0);
            memcpy(content, str.c_str(), str.length());
        }
        FixedString(const char * str) {
            fill(content, content + 20, 0);
            memcpy(content, str, strlen(str));
        }
        FixedString() {
            fill(content, content + 20, 0);
        }

        operator string() {
            return string(content);
        }
        operator char*() {
            return content;
        }
        operator const char*() {
            return content;
        }
    };

    struct FixedStringHasher {
        size_t operator()(const FixedString & rhs) {
            return hash<string>{}(rhs.content);
        }
    };
};