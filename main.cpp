#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>
#include "Lexer.h"
#include "magic_enum.hpp"

using namespace std;

int main(){
    ifstream file("test.morse");

    if (!file.is_open()){
        cerr << "Could not open test.morse\n";
        return 1;
    }

    string source(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    Lexer lexer(source);
    vector<Token> tokens = lexer.getTokens();

    for (const Token& token : tokens){
    cout << "Type: " << magic_enum::enum_name(token.type)
         << " | Lexeme: " << token.lexeme
         << " | Line: " << token.line
         << " | Column: " << token.column << '\n';
    }
}