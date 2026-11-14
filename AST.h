#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

#include "Token.h"

using namespace std;

enum class DataType{
    INTEGER,
    DECIMAL,
    BOOLEAN,
    CHARACTER,
    STRING
};

struct Expression {
    virtual ~Expression() = default;
};

struct IntegerLiteral : Expression {
    int value;
};

struct DecimalLiteral : Expression {
    double value;
};

struct BooleanLiteral : Expression {
    bool value;
};

struct CharacterLiteral : Expression {
    char value;
};

struct StringLiteral : Expression {
    string value;
};

struct VariableExpression : Expression {
    string name;
};

struct BinaryExpression : Expression {
    unique_ptr<Expression> left;
    TokenType op;
    unique_ptr<Expression> right;
};

struct Statement {
    virtual ~Statement() = default;
};

struct VariableDeclaration : Statement{ 
    DataType type;
    string name;
    unique_ptr<Expression> initializer = nullptr;
};

struct Assignment : Statement { 
    string name;
    unique_ptr<Expression> value;
};

struct OutputStatement : Statement {
    unique_ptr<Expression> expression;
};

struct InputStatement : Statement {
    string name;
};

struct ConditionalBranch {
    unique_ptr<Expression> condition;
    vector<unique_ptr<Statement>> body;
};

struct Program {
    vector<unique_ptr<Statement>> statements;
};