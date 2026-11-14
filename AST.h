
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

#include "Token.h"

using namespace std;

enum class DataType {
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

struct UnaryExpression : Expression {
    TokenType op;
    unique_ptr<Expression> operand;
};

struct FunctionCall : Expression {
    string name;
    vector<unique_ptr<Expression>> arguments;
};

struct Statement {
    virtual ~Statement() = default;
};

struct FunctionCallStatement : Statement {
    unique_ptr<FunctionCall> call;
};

struct VariableDeclaration : Statement {
    DataType type;
    string name;
    unique_ptr<Expression> initializer = nullptr;
};

struct Assignment : Statement {
    string name;
    unique_ptr<Expression> value;
};

struct CompoundAddAssignment : Statement {
    string name;
    unique_ptr<Expression> value;
};

struct CompoundSubtractAssignment : Statement {
    string name;
    unique_ptr<Expression> value;
};

struct CompoundMultiplyAssignment : Statement {
    string name;
    unique_ptr<Expression> value;
};

struct CompoundDivideAssignment : Statement {
    string name;
    unique_ptr<Expression> value;
};

struct CompoundAndAssignment : Statement {
    string name;
    unique_ptr<Expression> value;
};

struct CompoundOrAssignment : Statement {
    string name;
    unique_ptr<Expression> value;
};

struct OutputStatement : Statement {
    vector<unique_ptr<Expression>> expressions;
};

struct IncrementStatement : Statement {
    string name;
};

struct DecrementStatement : Statement {
    string name;
};

struct InputStatement : Statement {
    vector<string> names;
};

struct WaitStatement : Statement {
    unique_ptr<Expression> duration;
};

struct ConditionalBranch {
    unique_ptr<Expression> condition;
    vector<unique_ptr<Statement>> body;
};

struct IfStatement : Statement {
    ConditionalBranch ifBranch;
    vector<ConditionalBranch> elifBranches;
    optional<vector<unique_ptr<Statement>>> elseBody;
};

struct WhileStatement : Statement {
    unique_ptr<Expression> condition;
    vector<unique_ptr<Statement>> body;
};

struct ForStatement : Statement {
    unique_ptr<Statement> initializer;
    unique_ptr<Expression> condition;
    unique_ptr<Statement> increment;
    vector<unique_ptr<Statement>> body;
};

struct BreakStatement : Statement {};

struct ContinueStatement : Statement {};

struct Parameter {
    DataType type;
    string name;
};

struct FunctionDeclaration : Statement {
    optional<DataType> returnType;
    string name;
    vector<Parameter> parameters;
    vector<unique_ptr<Statement>> body;
};

struct FunctionTerminate : Statement {};

struct Program {
    vector<unique_ptr<Statement>> statements;
};