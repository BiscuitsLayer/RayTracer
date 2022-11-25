#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <scheme.h>
#include <error.h>

// Forward declaration
class Object;
class Cell;
class Quote;

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj) != nullptr;
}

// OBJECT //

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) = 0;
    virtual ~Object() = default;
};

// OTHERS //

class Number : public Object {
public:
    Number(int value) : value_(value) {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>&,
                                             std::shared_ptr<Scope>) override {

        return shared_from_this();
    }

    int GetValue() const {
        return value_;
    }

private:
    int value_ = 0;
};

class Symbol : public Object {
public:
    Symbol(const std::string& name) : name_(name) {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>&,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> value = scope->GetVariableValueRecursive(name_);
        return value;
    }

    const std::string& GetName() const {
        return name_;
    }

private:
    std::string name_{};
};

class Boolean : public Object {
public:
    Boolean(bool value) : value_(value) {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>&,
                                             std::shared_ptr<Scope>) override {
        return shared_from_this();
    }

    bool GetValue() const {
        return value_;
    }

private:
    bool value_ = 0;
};

// CELL //

std::vector<std::shared_ptr<Object>> ListToVector(std::shared_ptr<Object> init);
std::string ListToString(std::shared_ptr<Object> init);
std::shared_ptr<Object> BuildLambda(std::shared_ptr<Object> init, std::shared_ptr<Scope> scope);
std::pair<std::string, std::shared_ptr<Object>> BuildLambdaSugar(
    std::vector<std::shared_ptr<Object>> parts, std::shared_ptr<Scope> scope);

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>&,
                                             std::shared_ptr<Scope> scope) override {
        if (!GetFirst()) {  // Empty list case
            return shared_from_this();
        }

        std::shared_ptr<Object> function = GetFirst();
        std::shared_ptr<Object> maybe_lambda_keyword = function;
        if (Is<Symbol>(maybe_lambda_keyword) &&
            (As<Symbol>(maybe_lambda_keyword)->GetName() == "lambda")) {
            return BuildLambda(GetSecond(), scope);
        } else if (!Is<Quote>(function)) {
            if (Is<Symbol>(function) || Is<Cell>(function)) {
                function =
                    function->Evaluate({}, scope);  // Get function object from scope variables
            } else {
                throw RuntimeError("Lists are not self evaliating, use \"quote\"");
            }
        }

        std::shared_ptr<Object> arguments_start = GetSecond();

        std::vector<std::shared_ptr<Object>> function_arguments = ListToVector(arguments_start);
        return function->Evaluate(function_arguments, scope);
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    void SetFirst(std::shared_ptr<Object> first) {
        first_ = first;
    }
    void SetSecond(std::shared_ptr<Object> second) {
        second_ = second;
    }

private:
    std::shared_ptr<Object> first_{};
    std::shared_ptr<Object> second_{};
};

// FUNCTIONS //

class IsBoolean : public Symbol {
public:
    IsBoolean() : Symbol("boolean?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"IsBoolean\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);
        return Is<Boolean>(value) ? std::make_shared<Boolean>(true)
                                  : std::make_shared<Boolean>(false);
    }
};

class IsNumber : public Symbol {
public:
    IsNumber() : Symbol("number?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"IsNumber\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);
        return Is<Number>(value) ? std::make_shared<Boolean>(true)
                                 : std::make_shared<Boolean>(false);
    }
};

class IsSymbol : public Symbol {
public:
    IsSymbol() : Symbol("symbol?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"IsSymbol\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);
        return Is<Symbol>(value) ? std::make_shared<Boolean>(true)
                                 : std::make_shared<Boolean>(false);
    }
};

class IsPair : public Symbol {
public:
    IsPair() : Symbol("pair?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"IsPair\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);

        std::shared_ptr<Boolean> ans = std::make_shared<Boolean>(true);
        if (!Is<Cell>(value)) {
            return std::make_shared<Boolean>(false);
        } else {
            std::shared_ptr<Cell> cell = As<Cell>(value);
            if (!cell->GetFirst()) {
                return std::make_shared<Boolean>(false);
            } else {
                if (!Is<Cell>(cell->GetSecond())) {
                    return std::make_shared<Boolean>(true);
                }
                cell = As<Cell>(cell->GetSecond());
                if (cell->GetSecond()) {
                    return std::make_shared<Boolean>(false);
                }
                return cell->GetFirst() ? std::make_shared<Boolean>(true)
                                        : std::make_shared<Boolean>(false);
            }
        }
    }
};

class IsNull : public Symbol {
public:
    IsNull() : Symbol("null?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"IsNull\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);

        if (!Is<Cell>(value)) {
            return std::make_shared<Boolean>(false);
        }
        std::shared_ptr<Cell> cell = As<Cell>(value);
        if (!cell->GetFirst() && !cell->GetSecond()) {
            return std::make_shared<Boolean>(true);
        }
        return std::make_shared<Boolean>(false);
    }
};

class IsList : public Symbol {
public:
    IsList() : Symbol("list?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"IsList\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);

        if (!Is<Cell>(value)) {
            return std::make_shared<Boolean>(false);
        }
        for (std::shared_ptr<Cell> cell = As<Cell>(value); cell;
             cell = As<Cell>(cell->GetSecond())) {
            if (cell->GetSecond() && !Is<Cell>(cell->GetSecond())) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
};

class Quote : public Symbol {
public:
    Quote() : Symbol("quote") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope>) override {
        if (arguments.empty()) {
            return std::make_shared<Cell>(nullptr, nullptr);
        }
        if (arguments.size() > 1) {
            throw SyntaxError("Exactly 1 argument (list) required for \"Quote\" function");
        }
        return arguments[0];
    }
};

class Not : public Symbol {
public:
    Not() : Symbol("not") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"Not\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);

        if (Is<Boolean>(value)) {
            return As<Boolean>(value)->GetValue() ? std::make_shared<Boolean>(false)
                                                  : std::make_shared<Boolean>(true);
        }
        return std::make_shared<Boolean>(false);
    }
};

class And : public Symbol {
public:
    And() : Symbol("and") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);
        std::shared_ptr<Object> value = nullptr;
        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (Is<Boolean>(value)) {
                if (!As<Boolean>(value)->GetValue()) {
                    ans = std::make_shared<Boolean>(false);
                    return ans;
                }
            } else {
                ans = value;
            }
        }
        return ans;
    }
};

class Or : public Symbol {
public:
    Or() : Symbol("or") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(false);
        std::shared_ptr<Object> value = nullptr;
        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (Is<Boolean>(value)) {
                if (As<Boolean>(value)->GetValue()) {
                    ans = std::make_shared<Boolean>(true);
                    return ans;
                }
            } else {
                ans = value;
            }
        }
        return ans;
    }
};

class Equal : public Symbol {
public:
    Equal() : Symbol("=") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            last_value = value;
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Equal\" error: not a number given");
            }
            if (argument_idx != 0) {
                if (As<Number>(last_value)->GetValue() != As<Number>(value)->GetValue()) {
                    ans = std::make_shared<Boolean>(false);
                }
            }
        }
        return ans;
    }
};

class Greater : public Symbol {
public:
    Greater() : Symbol(">") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            last_value = value;
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Greater\" error: not a number given");
            }
            if (argument_idx != 0) {
                if (As<Number>(last_value)->GetValue() <= As<Number>(value)->GetValue()) {
                    ans = std::make_shared<Boolean>(false);
                }
            }
        }
        return ans;
    }
};

class GreaterEqual : public Symbol {
public:
    GreaterEqual() : Symbol(">=") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            last_value = value;
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"GreaterEqual\" error: not a number given");
            }
            if (argument_idx != 0) {
                if (As<Number>(last_value)->GetValue() < As<Number>(value)->GetValue()) {
                    ans = std::make_shared<Boolean>(false);
                }
            }
        }
        return ans;
    }
};

class Less : public Symbol {
public:
    Less() : Symbol("<") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            last_value = value;
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Less\" error: not a number given");
            }
            if (argument_idx != 0) {
                if (As<Number>(last_value)->GetValue() >= As<Number>(value)->GetValue()) {
                    ans = std::make_shared<Boolean>(false);
                }
            }
        }
        return ans;
    }
};

class LessEqual : public Symbol {
public:
    LessEqual() : Symbol("<=") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            last_value = value;
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"LessEqual\" error: not a number given");
            }
            if (argument_idx != 0) {
                if (As<Number>(last_value)->GetValue() > As<Number>(value)->GetValue()) {
                    ans = std::make_shared<Boolean>(false);
                }
            }
        }
        return ans;
    }
};

class Add : public Symbol {
public:
    Add() : Symbol("+") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Number>(0);
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Add\" error: not a number given");
            } else {
                int old_number = As<Number>(ans)->GetValue();
                int update_number = As<Number>(value)->GetValue();

                ans = std::make_shared<Number>(old_number + update_number);
            }
        }
        return ans;
    }
};

class Multiply : public Symbol {
public:
    Multiply() : Symbol("*") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Number>(1);
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Multiply\" error: not a number given");
            } else {
                int old_number = As<Number>(ans)->GetValue();
                int update_number = As<Number>(value)->GetValue();

                ans = std::make_shared<Number>(old_number * update_number);
            }
        }
        return ans;
    }
};

class Subtract : public Symbol {
public:
    Subtract() : Symbol("-") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() <= 1) {
            throw RuntimeError("More than 1 argument required for \"Subtract\" function");
        }
        std::shared_ptr<Object> ans = std::make_shared<Number>(0);
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Subtract\" error: not a number given");
            } else {
                if (argument_idx == 0) {
                    int update_number = As<Number>(value)->GetValue();

                    ans = std::make_shared<Number>(update_number);
                } else {
                    int old_number = As<Number>(ans)->GetValue();
                    int update_number = As<Number>(value)->GetValue();

                    ans = std::make_shared<Number>(old_number - update_number);
                }
            }
        }
        return ans;
    }
};

class Divide : public Symbol {
public:
    Divide() : Symbol("/") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() <= 1) {
            throw RuntimeError("More than 1 argument required for \"Divide\" function");
        }
        std::shared_ptr<Object> ans = std::make_shared<Number>(1);
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Divide\" error: not a number given");
            } else {
                if (argument_idx == 0) {
                    int update_number = As<Number>(value)->GetValue();

                    ans = std::make_shared<Number>(update_number);
                } else {
                    int old_number = As<Number>(ans)->GetValue();
                    int update_number = As<Number>(value)->GetValue();

                    ans = std::make_shared<Number>(old_number / update_number);
                }
            }
        }
        return ans;
    }
};

class Max : public Symbol {
public:
    Max() : Symbol("max") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            throw RuntimeError("At least 1 argument required for \"Max\" function");
        }
        std::shared_ptr<Object> ans = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Max\" error: not a number given");
            } else {
                if (argument_idx == 0) {
                    int new_number = As<Number>(value)->GetValue();

                    ans = std::make_shared<Number>(new_number);
                } else {
                    int old_number = As<Number>(ans)->GetValue();
                    int new_number = As<Number>(value)->GetValue();

                    if (new_number > old_number) {
                        ans = std::make_shared<Number>(new_number);
                    }
                }
            }
        }
        return ans;
    }
};

class Min : public Symbol {
public:
    Min() : Symbol("min") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            throw RuntimeError("At least 1 argument required for \"Min\" function");
        }
        std::shared_ptr<Object> ans = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            value = arguments[argument_idx]->Evaluate({}, scope);
            if (!Is<Number>(value)) {
                throw RuntimeError("\"Min\" error: not a number given");
            } else {
                if (argument_idx == 0) {
                    int new_number = As<Number>(value)->GetValue();

                    ans = std::make_shared<Number>(new_number);
                } else {
                    int old_number = As<Number>(ans)->GetValue();
                    int new_number = As<Number>(value)->GetValue();

                    if (new_number < old_number) {
                        ans = std::make_shared<Number>(new_number);
                    }
                }
            }
        }
        return ans;
    }
};

class Abs : public Symbol {
public:
    Abs() : Symbol("abs") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"Abs\" function");
        }
        std::shared_ptr<Object> ans = nullptr;
        std::shared_ptr<Object> value = nullptr;

        value = arguments[0]->Evaluate({}, scope);
        if (!Is<Number>(value)) {
            throw RuntimeError("\"Abs\" error: not a number given");
        }

        int new_number = As<Number>(value)->GetValue();
        new_number = abs(new_number);
        ans = std::make_shared<Number>(new_number);

        return ans;
    }
};

class Define : public Symbol {
public:
    Define() : Symbol("define") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 2) {
            throw SyntaxError("Exactly 2 arguments required for \"Define\" function");
        }

        if (Is<Cell>(arguments[0])) {
            auto name_and_value = BuildLambdaSugar(arguments, scope);
            scope->SetVariableValue(name_and_value.first, name_and_value.second);
            return nullptr;
        }
        if (!Is<Symbol>(arguments[0])) {
            throw RuntimeError(
                "\"Define\" error: first argument should be a variable name or sugar like \"(f x "
                "y) (x + y)\"");
        }
        std::string name = As<Symbol>(arguments[0])->GetName();
        std::shared_ptr<Object> value = nullptr;

        if (Is<Cell>(arguments[1])) {
            std::shared_ptr<Object> maybe_lambda_keyword = As<Cell>(arguments[1])->GetFirst();
            if (Is<Symbol>(maybe_lambda_keyword) &&
                As<Symbol>(maybe_lambda_keyword)->GetName() == "lambda") {
                value = BuildLambda(As<Cell>(arguments[1])->GetSecond(), scope);
            } else {
                value = arguments[1]->Evaluate({}, scope);
            }
        } else {
            value = arguments[1]->Evaluate({}, scope);
        }
        scope->SetVariableValue(name, value);
        return nullptr;
    }
};

class Set : public Symbol {
public:
    Set() : Symbol("set!") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 2) {
            throw SyntaxError("Exactly 2 arguments required for \"Set\" function");
        }

        if (!Is<Symbol>(arguments[0])) {
            throw RuntimeError("\"Set\" error: first argument should be a variable name");
        }
        std::string name = As<Symbol>(arguments[0])->GetName();
        std::shared_ptr<Object> old_value =
            scope->GetVariableValueRecursive(name);  // to make sure the variable exists

        std::shared_ptr<Object> new_value = arguments[1]->Evaluate({}, scope);
        scope->SetVariableValue(name, new_value);
        return nullptr;
    }
};

class If : public Symbol {
public:
    If() : Symbol("if") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if ((arguments.size() < 2) || (arguments.size() > 3)) {
            throw SyntaxError("Exactly 2 or 3 arguments required for \"If\" function");
        }

        std::shared_ptr<Object> condition = arguments[0]->Evaluate({}, scope);
        if (!Is<Boolean>(condition)) {
            throw RuntimeError("\"If\" error: condition is not Boolean");
        }

        if (As<Boolean>(condition)->GetValue()) {
            return arguments[1]->Evaluate({}, scope);
        } else {
            if (arguments.size() == 3) {
                return arguments[2]->Evaluate({}, scope);
            }
            return nullptr;
        }
        return nullptr;
    }
};

class Cons : public Symbol {
public:
    Cons() : Symbol("cons") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope>) override {
        if (arguments.size() != 2) {
            throw SyntaxError("Exactly 2 arguments required for \"Cons\" function");
        }

        std::shared_ptr<Object> ans = std::make_shared<Cell>(nullptr, nullptr);
        As<Cell>(ans)->SetFirst(arguments[0]);
        As<Cell>(ans)->SetSecond(arguments[1]);
        return ans;
    }
};

class Car : public Symbol {
public:
    Car() : Symbol("car") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"Car\" function");
        }

        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);
        if (!Is<Cell>(value)) {
            throw RuntimeError("\"Car\" error: not a pair or list given");
        }
        std::shared_ptr<Object> ans = As<Cell>(value)->GetFirst();
        return ans;
    }
};

class Cdr : public Symbol {
public:
    Cdr() : Symbol("cdr") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 1) {
            throw SyntaxError("Exactly 1 argument required for \"Cdr\" function");
        }

        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);
        if (!Is<Cell>(value)) {
            throw RuntimeError("\"Cdr\" error: not a pair or list given");
        }
        std::shared_ptr<Object> ans = As<Cell>(value)->GetSecond();
        return ans;
    }
};

class SetCar : public Symbol {
public:
    SetCar() : Symbol("set-car!") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 2) {
            throw SyntaxError("Exactly 2 arguments required for \"SetCar\" function");
        }

        std::shared_ptr<Object> source = arguments[0]->Evaluate({}, scope);
        if (!Is<Cell>(source)) {
            throw RuntimeError("\"SetCar\" error: not a pair or list given");
        }
        std::shared_ptr<Object> value = arguments[1]->Evaluate({}, scope);
        As<Cell>(source)->SetFirst(value);
        return nullptr;
    }
};

class SetCdr : public Symbol {
public:
    SetCdr() : Symbol("set-cdr!") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 2) {
            throw SyntaxError("Exactly 2 arguments required for \"SetCdr\" function");
        }

        std::shared_ptr<Object> source = arguments[0]->Evaluate({}, scope);
        if (!Is<Cell>(source)) {
            throw RuntimeError("\"SetCdr\" error: not a pair or list given");
        }
        std::shared_ptr<Object> value = arguments[1]->Evaluate({}, scope);
        As<Cell>(source)->SetSecond(value);
        return nullptr;
    }
};

class List : public Symbol {
public:
    List() : Symbol("list") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope>) override {
        std::shared_ptr<Object> previous = nullptr;
        std::shared_ptr<Object> current = std::make_shared<Cell>(nullptr, nullptr);
        std::shared_ptr<Object> ans = current;
        std::shared_ptr<Object> next = nullptr;

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            As<Cell>(current)->SetFirst(arguments[argument_idx]);
            next = std::make_shared<Cell>(nullptr, nullptr);
            As<Cell>(current)->SetSecond(next);

            previous = current;
            current = next;
        }
        if (previous) {
            As<Cell>(previous)->SetSecond(nullptr);
        }

        return ans;
    }
};

class ListRef : public Symbol {
public:
    ListRef() : Symbol("list-ref") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 2) {
            throw SyntaxError("Exactly 2 arguments required for \"ListRef\" function");
        }

        std::shared_ptr<Object> init = arguments[0]->Evaluate({}, scope);
        std::shared_ptr<Object> idx = arguments[1]->Evaluate({}, scope);
        if (!Is<Number>(idx)) {
            throw RuntimeError("\"ListRef\" error: idx is not a number");
        }
        int idx_number = As<Number>(idx)->GetValue();

        for (std::shared_ptr<Cell> cell = As<Cell>(init); cell;
             cell = As<Cell>(cell->GetSecond())) {
            if (idx_number == 0) {
                return cell->GetFirst();
            }
            --idx_number;
        }
        throw RuntimeError("\"ListRef\" error: idx out of bounds");
    }
};

class ListTail : public Symbol {
public:
    ListTail() : Symbol("list-tail") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        if (arguments.size() != 2) {
            throw SyntaxError("Exactly 2 arguments required for \"ListTail\" function");
        }
        std::shared_ptr<Object> init = arguments[0]->Evaluate({}, scope);
        std::shared_ptr<Object> idx = arguments[1]->Evaluate({}, scope);
        if (!Is<Number>(idx)) {
            throw RuntimeError("\"ListTail\" error: idx is not a number");
        }
        int idx_number = As<Number>(idx)->GetValue();

        for (std::shared_ptr<Cell> cell = As<Cell>(init); cell;
             cell = As<Cell>(cell->GetSecond())) {
            if (idx_number == 0) {
                return cell;
            }
            --idx_number;
        }
        if (idx_number == 0) {
            return nullptr;
        }
        throw RuntimeError("\"ListTail\" error: idx out of bounds");
    }
};

class Lambda : public Object {
public:
    Lambda(std::vector<std::shared_ptr<Object>>& commands,
           std::vector<std::string>& arguments_idx_to_name, std::shared_ptr<Scope> self_scope)
        : commands_(commands),
          arguments_idx_to_name_(arguments_idx_to_name),
          self_scope_(self_scope) {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments,
                                             std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Scope> cur_scope = std::make_shared<Scope>();
        cur_scope->SetPreviousScope(scope);

        for (size_t argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
            std::string name = arguments_idx_to_name_[argument_idx];
            std::shared_ptr<Object> value = arguments[argument_idx]->Evaluate({}, scope);
            cur_scope->SetVariableValue(name, value);
        }

        // Set variables before entering the function
        auto self_scope_variables = self_scope_->GetVariablesMap();
        for (auto& [name, value] : self_scope_variables) {
            try {
                cur_scope->GetVariableValueLocal(name);
            } catch (NameError& error) {
                cur_scope->SetVariableValue(name, value);
            }
        }

        std::shared_ptr<Object> ans = nullptr;
        for (size_t command_idx = 0; command_idx < commands_.size(); ++command_idx) {
            ans = commands_[command_idx]->Evaluate({}, cur_scope);
        }

        // Update variables after finishing the function
        auto cur_scope_variables = cur_scope->GetVariablesMap();
        for (auto& [name, value] : cur_scope_variables) {
            self_scope_->SetVariableValue(name, value);
        }
        return ans;
    }

private:
    std::vector<std::shared_ptr<Object>> commands_{};
    std::vector<std::string> arguments_idx_to_name_{};
    std::shared_ptr<Scope> self_scope_{};
};