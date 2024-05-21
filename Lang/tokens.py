# File contains all tokentypes for the language
from enum import Enum


class Tokens(Enum):
    WHITESPACE = -1
    
    # Structuring tokens
    ENDL = 0        # End of line
    STARTID = 1     # Start identifier
    ENDID = 2       # End identifier
    STARTE = 3      # Start expression
    ENDE = 4        # End expression
    LSEP = 5        # List separator

    # Expression clarifiers
    LITEXP = 6      # Literal expression
    COMPXP = 7      # Compute expression 

    # Type tokens
    BYTE = 8
    INT = 9
    FLOAT = 10
    LIST = 11
    STRING = 12

    # Operator tokens
    PLUS = 13
    MINUS = 14
    MULT = 15
    DIV = 16
    BITDWN = 17
    BITUP = 18

    # Actions tokens
    FUNCCALL = 19
    DEF = 20
    RETURN = 21
    ASSIGN = 22

    # Unique types
    FUNC = 23
    INB = 24    #Inbuilt function
    CODE = 25
    

PRETTY_TOKENS = [
    "END", "IDA", "IDB", "EXA", "EXB", "SEP", "LIT", "CMP", 
    "byt", "int", "flt", "lst", "str",
    " + ", " - ", " * ", " / ", ">> ", "<<",
    "EXE", "DEF", "RET", "SET",
    "FUN", "INB", "COD"
]

# Seperator tokens
SEP_TOKENS = {
    ',' : Tokens.LSEP,
    '(' : Tokens.STARTE,
    ')' : Tokens.ENDE,
    '{' : Tokens.STARTE,
    '}' : Tokens.ENDE,
    ' ' : Tokens.WHITESPACE,
    '\n': Tokens.WHITESPACE,
    '\t': Tokens.WHITESPACE,
    ';' : Tokens.ENDL
}

LITERAL_CHARS = [
    '"', "'"
]

KEYWORD_TOKENS = {
    'var' : Tokens.DEF,
    '=' : Tokens.ASSIGN
}