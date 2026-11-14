#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <stack>
#include <unordered_map>

#include "Token.h"

using namespace std;

class Lexer {
    private:
        string source;
        size_t position = 0;
        
        int const spacesPerTab = 4;
        int indentation = 0;

        stack<int> indentationStack;
        vector<Token> tokens;
        unordered_map<string, TokenType> keywords;
        unordered_map<string, char> morse;
        unordered_map<char, char> numShorthands;

        int line = 1;
        int column = 1;

        bool isAtEnd(int) const;
        char peek(int) const;
        char advance();
        bool handleSpaces();
        void handleIndentation();
        void handleIndentationChange();
        string scanMorseToken();
        string getEscapeSequence(char);
        Token identifyMorse(string);
        void scanTokens();
        void initializeKeywords();
        void initializeMorse();
        void throwError(string);

    public:
        Lexer(const string & input) : source(input) {
            indentationStack.push(0);
            initializeKeywords();
            initializeMorse();
            scanTokens();
        }

        vector<Token> getTokens();
};

vector<Token> Lexer::getTokens(){
    return tokens;
}

bool Lexer::isAtEnd(int lookahead = 0) const {
    return position + lookahead >= source.size();
}

char Lexer::peek(int lookahead = 0) const {
    if (isAtEnd(lookahead)){
        return '\0';
    }
    return source[position + lookahead];
}

char Lexer::advance() {
    if (!isAtEnd()){
        if (source[position] == '\n'){
            line ++;
            column = 1;
        }
        else {
            column ++;
        }
        position ++;
        return source[position - 1];
    }
    return '\0';
}

bool Lexer::handleSpaces(){
    int countSpaces = 0;
    while (peek() == ' '){
        advance();
        countSpaces++;
    }
    return countSpaces >= 2;
}

void Lexer::handleIndentation(){
    int numSpaces = 0;
    int numTabs = 0;
    while (peek() == ' ' || peek() == '\t'){
        if (peek() == ' '){
            numSpaces ++;
        }
        else {
            numTabs ++;
        }
        advance();
    }
    if (peek() == '\n' || peek() == '\0'){
        return;
    }
    if (numSpaces % 4 != 0){
        throwError("Invalid indentation");
    }
    indentation = numTabs + numSpaces / spacesPerTab;
    handleIndentationChange();
}

void Lexer::handleIndentationChange(){
    if (indentation > indentationStack.top()){
        indentationStack.push(indentation);
        tokens.push_back(Token{TokenType::INDENT, "", line, column});
    }
    while (indentation < indentationStack.top()){
        indentationStack.pop();
        tokens.push_back(Token{TokenType::DEDENT, "", line, column});
    }
    if (indentation != indentationStack.top()){
        throwError("Invalid indentation");
    }
}

string Lexer::scanMorseToken(){
    string token = "";
    while (peek() == '.' || peek() == '-' || peek() == ' '){
        if (peek() == ' ' && peek(1) == ' '){
            return token;
        }
        token += advance();
    }
    return token;
}

string Lexer::getEscapeSequence(char morseChar){
    switch (morseChar){
        case 'N':
            return "\n";
        case 'T':
            return "\t";
        case '@':
            return "@";
        default:
            throwError("Invalid escape sequence.");
    }
}

Token Lexer::identifyMorse(string morseSequence){
    istringstream read(morseSequence);
    string morseChar;
    string decodedMorse = "";
    string parseValue = "";
    
    while (read >> morseChar){
        auto it = morse.find(morseChar);
        if (it != morse.end()){
            decodedMorse += it -> second;
        }
        else {
            throwError("Invalid Morse Sequence");
        }
    }

    if (decodedMorse.empty()){
        throwError("Empty Morse token");
    }

    if (isdigit(decodedMorse[0])){
        bool isInt = true;
        for (int i = 0; i < decodedMorse.size(); i++){
            if (isalpha(decodedMorse[i])){
                auto it = numShorthands.find(decodedMorse[i]);
                if (it != numShorthands.end()){
                    decodedMorse[i] = it -> second;
                }
                else {
                    throwError("Invalid number");
                }
            }
            else if (isdigit(decodedMorse[i])){
                continue;
            }
            else if (decodedMorse[i] == '.' && isInt == true){
                isInt = false;
            }
            else {
                throwError("Invalid number");
            }
        }
        if (isInt){
            return Token{TokenType::INTEGER, decodedMorse, line, column};
        } else {
            return Token{TokenType::DECIMAL, decodedMorse, line, column};
        }
    }
    else if (isalpha(decodedMorse[0])){
        // Identifier
        for (int i = 1; i < decodedMorse.size(); i++){
            if (!isalnum(decodedMorse[i])){
                throwError("Invalid identifier");
            }
        }
        return Token{TokenType::IDENTIFIER, decodedMorse, line, column};
    }

    else {
        string morseString = "";
        switch(decodedMorse[0]){
            case '\'':
                if (decodedMorse.size() == 4 && decodedMorse[1] == '@' && decodedMorse[3] == '\''){
                    return Token{TokenType::CHARACTER, getEscapeSequence(decodedMorse[2]), line, column};
                } else if (decodedMorse.size() == 3 && decodedMorse[2] == '\'') {
                    return Token{TokenType::CHARACTER, string(1, decodedMorse[1]), line, column};
                } else {
                    throwError("Invalid character");
                }
                break;
            case '"': {
                string currentMorse = "";
                bool escaped = false;
                bool closed = false;

                auto decodeCurrentMorse = [&]() {
                    if (currentMorse.empty()){
                        return;
                    }
                    auto it = morse.find(currentMorse);
                    if (it == morse.end()){
                        throwError("Invalid Morse sequence in string");
                    }
                    char decoded = it->second;
                    currentMorse.clear();
                    if (escaped){
                        morseString += getEscapeSequence(decoded);
                        escaped = false;
                    }
                    else if (decoded == '@'){
                        escaped = true;
                    }
                    else {
                        morseString += decoded;
                    }
                };

                while (!isAtEnd()){
                    char current = peek();
                    if (current == '\n'){
                        throwError("Incomplete string");
                    }
                    else if (current == '.' || current == '-'){
                        currentMorse += advance();
                        if (currentMorse == ".-..-."){
                            if (escaped){
                                throwError("Invalid escape sequence");
                            }
                            closed = true;
                            break;
                        }
                    }
                    else if (current == ' '){
                        int count = 0;

                        while (peek() == ' '){
                            advance();
                            count++;
                        }
                        decodeCurrentMorse();
                        if (count >= 2){
                            if (escaped){
                                throwError("Invalid escape sequence");
                            }
                            morseString += ' ';
                        }
                    }

                    else if (current == '\t'){
                        advance();
                        decodeCurrentMorse();

                        if (escaped){
                            throwError("Invalid escape sequence");
                        }
                        morseString += ' ';
                    }

                    else {
                        throwError("Invalid character in string");
                    }
                }

                if (!closed){
                    throwError("Incomplete string");
                }

                return Token{TokenType::STRING, morseString, line, column};
            }
            default:
                throwError("Error reading morse token");
        }
    }
}

void Lexer::scanTokens(){
    while (!isAtEnd()){
        if (column == 1){
            handleIndentation();
        }
        if (peek() == ' '){
            handleSpaces();
        }
        else if (peek() == '\t'){
            advance();
        }
        else if (peek() == '\n'){
            tokens.push_back(Token{TokenType::NEWLINE, "", line, column});
            advance();
        }
        else if (peek() == '.' || peek () == '-'){
            string letter = scanMorseToken();
            auto it = keywords.find(letter);
            if (it != keywords.end()){
                tokens.push_back(Token{it -> second, "", line, column});
            }
            else {
                tokens.push_back(identifyMorse(letter));
            }
        }
        else {
            while (peek() != '\n' && !isAtEnd()){
                advance();
            }
        }
    }
    while (indentationStack.top() != 0){
        indentationStack.pop();
        tokens.push_back(Token{TokenType::DEDENT, "", line, column});
    }
    tokens.push_back(Token{TokenType::EOF_TOKEN, "", line, column});
}

void Lexer::initializeKeywords(){
    keywords["-.-. --.-"] = TokenType::FUNCTION_DEFINE;
    keywords["-.. ."] = TokenType::FUNCTION_NAME;
    keywords["-.-"] = TokenType::FUNCTION_END;
    keywords[".-. .--. -"] = TokenType::REPORT;
    keywords["-.-. .-.."] = TokenType::FUNCTION_TERMINATE;
    keywords["-.-...."] = TokenType::START_CALL;
    keywords["-.--."] = TokenType::END_CALL;
    keywords["-. .-."] = TokenType::DECLARE_INTEGER;
    keywords["-.. . -.-."] = TokenType::DECLARE_DECIMAL;
    keywords["-... --- --- .-.."] = TokenType::DECLARE_BOOLEAN;
    keywords["-.-. .... .- .-."] = TokenType::DECLARE_CHARACTER;
    keywords["- -..- -"] = TokenType::DECLARE_STRING;
    keywords["-. .. .-.."] = TokenType::DECLARE_VOID;
    keywords["-.-."] = TokenType::TRUE;
    keywords["-."] = TokenType::FALSE;
    keywords[".. ..-."] = TokenType::IF;
    keywords[". .-.. .. ..-."] = TokenType::ELIF;
    keywords[". .-.. ... ."] = TokenType::ELSE;
    keywords["-- .... .- .-.. ."] = TokenType::WHILE;
    keywords["--. ---"] = TokenType::CONTINUE;
    keywords["...-.-"] = TokenType::BREAK;
    keywords["..-. --- .-."] = TokenType::FOR;
    keywords[".. ..."] = TokenType::COMPARISON_START;
    keywords["..--.."] = TokenType::COMPARISON_END;
    keywords[". ..."] = TokenType::AND;
    keywords["--- .-."] = TokenType::OR;
    keywords["-. ---"] = TokenType::NOT;
    keywords["-. ---..."] = TokenType::GREATER;
    keywords["-. -...-"] = TokenType::GREATER_EQUAL;
    keywords["... ---..."] = TokenType::LESSER;
    keywords["... -...-"] = TokenType::LESSER_EQUAL;
    keywords["-. --- -...-"] = TokenType::INEQUALITY;
    keywords["-...-"] = TokenType::EQUALITY;
    keywords["-. .--"] = TokenType::ARITHMETIC_START;
    keywords[".-.-.-"] = TokenType::ARITHMETIC_END;
    keywords[".-.-."] = TokenType::ADD;
    keywords["-....-"] = TokenType::SUBTRACT;
    keywords[".-.-. .-.-."] = TokenType::INCREMENT;
    keywords["-....- -....-"] = TokenType::DECREMENT;
    keywords["-..-"] = TokenType::MULTIPLY;
    keywords["-..-."] = TokenType::DIVIDE;
    keywords[".-.- -...-"] = TokenType::COMPOUND_ADD;
    keywords["-....- -...-"] = TokenType::COMPOUND_SUBTRACT;
    keywords["-..- -...-"] = TokenType::COMPOUND_MULTIPLY;
    keywords["-..-. -...-"] = TokenType::COMPOUND_DIVIDE;
    keywords[". ... -...-"] = TokenType::COMPOUND_AND;
    keywords["--- .-. -...-"] = TokenType::COMPOUND_OR;
    keywords[".-..."] = TokenType::WAIT;
    keywords[".-. -..-"] = TokenType::INPUT;
    keywords["- -..-"] = TokenType::OUTPUT;
}

void Lexer::initializeMorse(){
    morse[".-"] = 'A';
    morse["-..."] = 'B';
    morse["-.-."] = 'C';
    morse["-.."] = 'D';
    morse["."] = 'E';
    morse["..-."] = 'F';
    morse["--."] = 'G';
    morse["...."] = 'H';
    morse[".."] = 'I';
    morse[".---"] = 'J';
    morse["-.-"] = 'K';
    morse[".-.."] = 'L';
    morse["--"] = 'M';
    morse["-."] = 'N';
    morse["---"] = 'O';
    morse[".--."] = 'P';
    morse["--.-"] = 'Q';
    morse[".-."] = 'R';
    morse["..."] = 'S';
    morse["-"] = 'T';
    morse["..-"] = 'U';
    morse["...-"] = 'V';
    morse[".--"] = 'W';
    morse["-..-"] = 'X';
    morse["-.--"] = 'Y';
    morse["--.."] = 'Z';
    morse[".----"] = '1';
    morse["..---"] = '2';
    morse["...--"] = '3';
    morse["....-"] = '4';
    morse["....."] = '5';
    morse["-...."] = '6';
    morse["--..."] = '7';
    morse["---.."] = '8';
    morse["----."] = '9';
    morse["-----"] = '0';

    morse[".-.-.-"] = '.';
    morse["--..--"] = ',';
    morse["---..."] = ':';
    morse["..--.."] = '?';
    morse["-....-"] = '-';
    morse["-..-."] = '/';
    morse["-.--."] = '(';
    morse["-.--.-"] = ')';
    morse["-...-"] = '=';
    morse[".-.-."] = '+';
    morse["-..-"] = '*';

    morse[".-..-."] = '"';
    morse[".----."] = '\'';
    morse[".--.-."] = '@';

    numShorthands['T'] = '0';
    numShorthands['A'] = '1';
    numShorthands['U'] = '2';
    numShorthands['V'] = '3';
    numShorthands['E'] = '5';
    numShorthands['B'] = '7';
    numShorthands['D'] = '8';
    numShorthands['N'] = '9';
}

void Lexer::throwError(string message){
    throw runtime_error(message + ": Line " + to_string(line) + ", Col " + to_string(column));
}