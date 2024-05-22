import sys
import tokens

from enum import Enum
from tokens import Tokens

def tokenize(text):
    tokenList = []
    lastSep = 0 # Last enclosing seperator token
    currentAction = None
    currentToken = ""
    enclosedChar = None

    # Iterate through the text
    for char in text:
        # Check for literal
        if char in tokens.LITERAL_CHARS:
            if enclosedChar is None:
                tokenList.append(Tokens.LITEXP)
                if char == '"':
                    tokenList.append(Tokens.STRING)
                if char == "'":
                    tokenList.append(Tokens.BYTE)
                tokenList.append(Tokens.STARTE)
                enclosedChar = char
                continue

            # Check if end expression
            if char == enclosedChar:
                tokenList.append(Tokens.ENDE)
                enclosedChar = None
                continue

            # Bail if missing seperator
            if currentToken != "":
                print("ERROR: Missing seperator before literal expression.")
                sys.exit(1)

        if enclosedChar is not None:
            tokenList.append(char)
            continue

        # Check if code encloser
        if char == "{":
            tokenList.append(Tokens.LITEXP)
            tokenList.append(Tokens.CODE)
        if char == "}":
            tokenList.append(0xff)

        # Check if list encloser
        if char == '[':
            tokenList.append(Tokens.LITEXP)
            tokenList.append(Tokens.LIST)
        
        # If the current character is a seperator
        if char in tokens.SEP_TOKENS:
            # If the current token is not empty
            if currentToken != "":
                # Check if is keyword
                if currentToken in tokens.KEYWORD_TOKENS:
                    tokenList.append(tokens.KEYWORD_TOKENS[currentToken])
                    currentToken = ""
                    continue

                # Check if decimal literal
                literal = currentToken
                if literal.startswith("-"):
                    literal = currentToken[1:]
                
                if literal.isdecimal():
                    # Real value
                    val = None

                    # Add tokens
                    tokenList.append(Tokens.LITEXP)
                    if literal.isdigit():
                        tokenList.append(Tokens.INT)
                        val = int(currentToken)
                    else:
                        tokenList.append(Tokens.FLOAT)
                        val = float(currentToken)
                    tokenList.append(Tokens.STARTE)
                    
                    # Add bytes
                    bytes = val.to_bytes(4, 'little', signed = True)
                    for byte in bytes:
                        tokenList.append(byte)

                    tokenList.append(Tokens.ENDE)
                else:
                    tokenList.append(Tokens.STARTID)
                    for token in currentToken:
                        tokenList.append(token)
                    tokenList.append(Tokens.ENDID)

                currentToken = ""
                
            # Check if function call (if last token is Tokens.ENDID)
            if char == "(" and len(tokenList) > 0 and tokenList[~0] == Tokens.ENDID:
                # Find latest Tokens.STARTID
                startId = None
                for i in range(len(tokenList) - 1, -1, -1):
                    if tokenList[i] == Tokens.STARTID:
                        startId = i
                        break
                    if(tokenList[i] == Tokens.ENDL):
                        print("Error: No valid function to call")
                        exit(1)
                
                # Insert Tokens.FUNCCALL before corresponding Tokens.STARTID
                if startId is not None:
                    tokenList.insert(startId, Tokens.FUNCCALL)
                else:
                    print("ERROR: Missing STARTID before FUNCCALL.")

            # Add the seperator to the list if not whitespace
            if tokens.SEP_TOKENS[char] != Tokens.WHITESPACE:
                lastSep = tokens.SEP_TOKENS[char]
                tokenList.append(tokens.SEP_TOKENS[char])
                
        # If the current character is not a seperator
        else:
            # Add the character to the current token
            currentToken += char
    
    tokenList.append(0xFF)
    return tokenList

def byteifyTokens(tokenList):
    bytes = bytearray()
    for t in tokenList:
        # Convert enum
        if type(t) == Tokens:
            bytes.append(t.value)
        # Convert string
        elif isinstance(t, str):
            bytes.extend(bytearray(t, "utf-8"))
        # Convert byte
        else:
            bytes.append(t)
    return bytes

def printTokens(tokenList):
    for t in tokenList:
        if type(t) == Tokens:
            print(tokens.PRETTY_TOKENS[t.value], end=" ")
            if t == Tokens.ENDL:
                print("")
        elif type(t) == int:
            print(str(t).zfill(3), end=" ")
        else:
            print(" " + t + " ", end=" ")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python main.py <filename>")
    else:
        filename = sys.argv[1]

        # Tokenize file
        with open(filename, "r") as file:
            raw = file.read()

            # Pre compile
            text = ""
            for line in raw.split("\n"):
                if not line.startswith("//"):
                    text += line

            tokenList = tokenize(text)
            printTokens(tokenList)

            # Get filename before split at last '.'
            filename = "".join(filename.split(".")[:-1])
            with open(filename + ".mb", "wb") as outFile:
                bytes = byteifyTokens(tokenList)
                outFile.write(bytes)
                print("\n")

                print("Code str: ")
                print("\t" + "".join("%02x" % b for b in bytes))

                print("\nC formatted list: ")
                print("\t{", end="\n\t\t")
                for i, b in enumerate(bytes):
                    print("0x%02x" % b, end="")
                    if i != len(bytes) - 1:
                        print(", ", end="")
                    if i % 4 == 3:
                        print("\n", end="\t\t")
                    i += 1
                print("\n\t}")