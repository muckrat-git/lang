#include <iostream>
#include <vector>

#include "mucktools.hpp"

//#define DEBUG

using namespace Muck;

#define byte unsigned char

using namespace std;

enum tokens {
    WHITESPACE = -1,
    ENDL = 0,
    STARTID = 1,
    ENDID = 2,
    STARTE = 3,
    ENDE = 4,
    LSEP = 5,
    LITEXP = 6,
    COMPXP = 7,
    BYTE = 8,
    INT = 9,
    FLOAT = 10,
    LIST = 11,
    STRING = 12,
    PLUS = 13,
    MINUS = 14,
    MULT = 15,
    DIV = 16,
    BITDWN = 17,
    BITUP = 18,
    FUNCCALL = 19,
    DEF = 20,
    RETURN = 21,
    ASSIGN = 22,
    FUNC = 23,
    INB = 24,
    CODE = 25
};

const byte script[] = {
    0x14, 0x01, 0x65, 0x78, 0x65, 0x02, 0x16, 0x06, 0x19, 0x03, 0x13, 0x01, 0x70, 0x72, 0x69, 0x6e, 0x74, 0x02, 0x03, 0x06, 0x0c, 0x03, 0x54, 0x68, 0x69, 0x73, 0x20, 0x73, 0x68, 0x6f, 0x75, 0x6c, 0x64, 0x6e, 0x74, 0x20, 0x72, 0x75, 0x6e, 0x04, 0x04, 0x00, 0xff, 0x04, 0x03, 0x04, 0x00, 0xff
};

const string PRETTY_TOKENS[] = {
    "END", "IDA", "IDB", "EXA", "EXB", "SEP", "LIT", "CMP", 
    "byt", "int", "flt", "lst", "str",
    " + ", " - ", " * ", " / ", ">> ", "<<",
    "EXE", "DEF", "RET", "SET",
    "FUN", "INB", "COD"
};

// Conversion union for int
union iUnion {
    byte bytes[4];
    int32_t val;
};

// Runner variable
class Variable {
    public:
    byte * bytes;
    u_int64_t length = 0;
    byte type = 0;

    Variable Execute(vector<Variable> args);
} NULLVAR;

// - Inbuilt functions -
Variable _inb_print(vector<Variable> args) {
    if(args[0].type == INT) {
        iUnion iu;
        for(int i = 0; i < 4; ++i) iu.bytes[i] = args[0].bytes[i];
        cout << iu.val << endl;
        return NULLVAR;
    }
    if(args[0].type == CODE) {
        for(int i = 0; i < args[0].length; ++i) {
            cout << hex << int(args[0].bytes[i]) << " ";
        }
        return NULLVAR;
    }
    if(args[0].type == STRING) {
        char msg[args[0].length + 1];
        memcpy(msg, args[0].bytes, args[0].length);
        msg[args[0].length] = 0;
        cout << msg;
        return NULLVAR;
    }

    cout << args[0].bytes << endl;
    return NULLVAR;
}

void Debug(string msg) {
    #ifdef DEBUG
        cout << msg;
    #endif
}

// Inbuilt function array
Variable (*inbuiltFunctions[])(vector<Variable> args) = {
    &_inb_print
};

// Variable storage
Dict<FixedString, Variable, FixedStringHasher> variables;

class Scope {
    public:
    Variable DoExpression(byte *& exp) {
        byte expType = *(exp++);
        byte litType;
        if(expType != STARTE) {
            Debug(PRETTY_TOKENS[*exp] + " ");
            litType = *(exp++);
            ++exp;
        }

        Variable ret;

        // If literal
        if(expType == LITEXP) {
            ret.type = litType;
            ret.length = 0;
            // Get literal size
            int depth = 1;
            while(depth != 0) {
                Debug(" " + to_string(int(*exp)) + " ");

                if(*exp == STARTE) ++depth;
                if(*exp == ENDE) --depth;

                ++ret.length;
                ++exp;
            }
            --exp;
            --ret.length;

            // Allocate and set bytes
            ret.bytes = new byte[ret.length];
            for(u_int64_t i = 0; i < ret.length; i++) {
                ret.bytes[ret.length - i - 1] = *(exp - i - 1);
            }
            return ret;
        }

        return NULLVAR;
    }

    void ExecuteLine(byte *& line) {
        Variable * host;
        byte action;
        string currentId = "";
        bool inID = false;
        bool isSubject = false;

        vector<Variable> subjects;

        while(*line != ENDL) {
            // Check if assembling id
            if(inID) {
                // If should exit id 
                if(*line == ENDID) {
                    Debug(PRETTY_TOKENS[*line] + " ");
                    // Reset
                    inID = false;

                    // Check if subject
                    if(isSubject) {
                        // Get id
                        if(variables.Get(currentId) == nullptr) {
                            // Error out
                            cout << "ERROR: variable '" + currentId + "' does not exist :(\n";
                            exit(EXIT_FAILURE);
                            return;
                        }
                        subjects.push_back(variables[currentId]);
                        currentId = "";
                        ++line;
                        continue;
                    }

                    // Check if is definition
                    if(action == DEF) {
                        // Check if already exists
                        if(variables.Get(currentId) != nullptr) {
                            // Error
                            cout << "ERROR: variable '" + currentId + "' already exists\n";
                            exit(EXIT_FAILURE);
                            return;
                        }

                        // Create
                        variables.Add(currentId, Variable());
                    }

                    // Get id
                    if(variables.Get(currentId) == nullptr) {
                        // Error out
                        cout << "ERROR: variable '" + currentId + "' does not exist :(\n";
                        exit(EXIT_FAILURE);
                        return;
                    }

                    // Get host and reset id
                    host = &variables[currentId];
                    currentId = "";
                }
                else {
                    Debug(" " + to_string(*line) + "  ");
                    currentId += *line;
                }
                ++line;
                continue;
            }

            Debug(PRETTY_TOKENS[*line] + " ");

            switch(*line) {
                case STARTID:
                    inID = true;
                    break;
                case COMPXP:
                case LITEXP:
                    subjects.push_back(DoExpression(line));
                    break;
                case STARTE:
                    subjects.clear();
                    isSubject = true;
                    break;
                case ENDE:
                    isSubject = false;
                    break;
                default:
                    action = *line;
                    break;
            }

            ++line;

            if(*line == ENDL) {
                // Act on action
                if(action == FUNCCALL)
                    host->Execute(subjects);
                if(action == ASSIGN) {
                    delete host->bytes;
                    (*host) = subjects[0];
                }  
            }
        }
    }
};

Scope scope;

Variable Variable::Execute(vector<Variable> args) {
    if(type == INB) {
        Variable (*func)(vector<Variable> args) = inbuiltFunctions[bytes[0]];
        return func(args);
    }
    if(type == CODE) {
        byte * scriptStream = bytes;
        while(*scriptStream != 0xFF) {
            scope.ExecuteLine(scriptStream);
            Debug(PRETTY_TOKENS[*scriptStream] + " ");
            ++scriptStream;
        }
        return NULLVAR;
    }

    cout << "Error: Cannot execute variable\n";
    return NULLVAR;
}

int main() {
    variables.Add("print", {.bytes = new byte [1]{0}, .length = 0, .type = INB});
    byte * scriptStream = (byte *)script;
    
    while(*scriptStream != 0xFF) {
        scope.ExecuteLine(scriptStream);
        Debug(PRETTY_TOKENS[*scriptStream] + " ");
        ++scriptStream;
    }
}