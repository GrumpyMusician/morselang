#include <string>
#include <cstddef>

using namespace std;

enum class TokenType{
    // STRUCTURE
    INDENT,
    DEDENT,
    NEWLINE,
    EOF_TOKEN,

    IDENTIFIER,

    FUNCTION_DEFINE,
    FUNCTION_NAME,
    FUNCTION_END,
    REPORT,
    FUNCTION_TERMINATE,
    START_CALL,
    END_CALL,

    // DATA TYPES
    DECLARE_INTEGER,
    DECLARE_DECIMAL,
    DECLARE_BOOLEAN,
    DECLARE_CHARACTER,
    DECLARE_STRING,
    DECLARE_VOID,
    INTEGER,
    DECIMAL,
    BOOLEAN,
    CHARACTER,
    STRING,
    VOID,

    TRUE,
    FALSE,

    // CONTROL FLOW
    IF,
    ELIF,
    ELSE,
    WHILE,
    FOR,
    CONTINUE,
    BREAK,

    // LOGIC AND COMPARISON
    COMPARISON_START,
    COMPARISON_END,
    AND,
    OR,
    NOT,
    GREATER,
    GREATER_EQUAL,
    LESSER,
    LESSER_EQUAL,
    INEQUALITY,
    EQUALITY,

    // MATH
    ARITHMETIC_START,
    ARITHMETIC_END,
    ADD,
    SUBTRACT,
    INCREMENT,
    DECREMENT,
    MULTIPLY,
    DIVIDE,
    COMPOUND_ADD,
    COMPOUND_SUBTRACT,
    COMPOUND_MULTIPLY,
    COMPOUND_DIVIDE,
    COMPOUND_AND,
    COMPOUND_OR,

    // BUILT-IN OPERATIONS AND ITS MODIFIERS
    WAIT,
    INPUT,
    OUTPUT,
    IO_CONVERT

};

struct Token {
    TokenType type;
    string lexeme;
    int line;
    int column;
};