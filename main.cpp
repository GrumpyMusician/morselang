#pragma once

#include <string>
#include <stdexcept>
#include <cstddef>

using namespace std;

class Lexer {
    private:
        string source;
        size_t position = 0;
        int const spacesPerTab = 4;
        int line = 1;
        int column = 1;

        bool isAtEnd() const {
            return position >= source.size();
        }

        char peek() const {
            if (isAtEnd()){
                return '\0';
            }
            return source[position];
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

    public:
        Lexer(const string & input) : source(input) {}
};