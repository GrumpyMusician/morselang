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

        int line = 1;
        int column = 1;

        bool isAtEnd(int lookahead = 0) const {
            return position + lookahead >= source.size();
        }

        char peek(int lookahead = 0) const {
            if (isAtEnd(lookahead)){
                return '\0';
            }
            return source[position + lookahead];
        }

        char advance() {
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

        bool handleSpaces(){
            int countSpaces = 0;
            while (peek() == ' '){
                advance();
                countSpaces++;
            }
            return countSpaces >= 2;
        }

        void handleIndentation(){
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

        void handleIndentationChange(){
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

        string scanMorseToken(){
            string token = "";
            while (peek() == '.' || peek() == '-' || peek() == ' '){
                if (peek() == ' ' && peek(1) == ' '){
                    return token;
                }
                token += advance();
            }
            return token;
        }

        string scanIdentifier(string identifier){
            string result = "";
            string morseLetter;

            istringstream stream(identifier);
            while (stream >> morseLetter){
                auto it = morse.find(morseLetter);
                if (it != morse.end()){
                    result += it -> second;
                }
                else {
                    throwError("Invalid Morse character in identifier");
                }
            }
            return result;
        }

        void scanTokens(){
            while (!isAtEnd()){
                if (column == 1){
                    handleIndentation();
                }
                if (peek() == ' '){
                    bool spaceType = handleSpaces(); // False = Space, True = Token Seperator (Tab)
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
                        tokens.push_back(Token{it->second, letter, line, column});
                    }
                    else {
                        tokens.push_back(Token{TokenType::IDENTIFIER, scanIdentifier(letter), line, column});
                    }
                }
                else {
                    advance();
                }
            }
            while (indentationStack.top() != 0){
                indentationStack.pop();
                tokens.push_back(Token{TokenType::DEDENT, "", line, column});
            }
            tokens.push_back(Token{TokenType::EOF_TOKEN, "", line, column});
        }

        void initializeKeywords(){
            keywords["-.-. --.-"] = TokenType::FUNCTION_DEFINE;
            keywords["-.. ."] = TokenType::FUNCTION_NAME;
            keywords["-.-"] = TokenType::FUNCTION_END;
            keywords[".-. .--. -"] = TokenType::REPORT;
            keywords["-.-. .-.."] = TokenType::FUNCTION_TERMINATE;
            keywords["-.-...."] = TokenType::START_CALL;
            keywords["-.--."] = TokenType::END_CALL;
            keywords["-. .-."] = TokenType::INTEGER;
            keywords["-.. . -.-."] = TokenType::DECIMAL;
            keywords["-... --- --- .-.."] = TokenType::BOOLEAN;
            keywords["-.-. .... .- .-."] = TokenType::CHARACTER;
            keywords["- -..- -"] = TokenType::STRING;
            keywords["-. .. .-.."] = TokenType::VOID;
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
            keywords["-. ..--.."] = TokenType::GREATER;
            keywords["-. -...-"] = TokenType::GREATER_EQUAL;
            keywords["... ..--.."] = TokenType::LESSER;
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
            keywords["-- ... --."] = TokenType::IO_CONVERT;
        }

        void initializeMorse(){
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
            morse["---"] = 'O';
            morse[".--."] = 'P';
            morse["--.-"] = 'Q';
            morse[".-."] = 'R';
            morse["..."] = 'S';
            morse["---"] = 'O';
            morse["-"] = 'T';
            morse["..-"] = 'U';
            morse["...-"] = 'V';
            morse[".--"] = 'W';
            morse["-..-"] = 'X';
            morse["-.--"] = 'Y';
            morse["--.."] = 'Z';
        }


        void throwError(string message){
            throw runtime_error(message + ": Line " + to_string(line) + ", Col " + to_string(column));
        }

    public:
        Lexer(const string & input) : source(input) {
            indentationStack.push(0);
            initializeKeywords();
            initializeMorse();
            scanTokens();
        }
        
        vector<Token> getTokens(){
            return tokens;
        }
};