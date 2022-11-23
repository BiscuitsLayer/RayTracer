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

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    Object* raw = obj.get();
    T* new_raw = dynamic_cast<T*>(raw);
    return new_raw != nullptr;
}

// OBJECT //

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) = 0;
    virtual ~Object() = default;
};

// OTHERS //

class Number : public Object {
public:
    Number(int value) : value_(value) {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> value = scope.GetVariableValue(name_);
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
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


class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (!GetFirst()) { // Empty list case
            return shared_from_this();
        }
        std::shared_ptr<Object> function = GetFirst();
        if (!Is<Symbol>(function)) {
            throw RuntimeError("Lists are not self evaliating, use \"quote\"");
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"IsBoolean\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);
        return Is<Boolean>(value) ? std::make_shared<Boolean>(true) : std::make_shared<Boolean>(false);
    }
};

class IsNumber : public Symbol {
public:
    IsNumber() : Symbol("number?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"IsNumber\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);
        return Is<Number>(value) ? std::make_shared<Boolean>(true) : std::make_shared<Boolean>(false);
    }
};

class IsPair : public Symbol {
public:
    IsPair() : Symbol("pair?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"IsPair\" function");
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
                return cell->GetFirst() ? std::make_shared<Boolean>(true) : std::make_shared<Boolean>(false);
            }
        }
    }
};

class IsNull : public Symbol {
public:
    IsNull() : Symbol("null?") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"IsNull\" function");
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"IsList\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);

        if (!Is<Cell>(value)) {
            return std::make_shared<Boolean>(false);
        }
        for (std::shared_ptr<Cell> cell = As<Cell>(value); cell; cell = As<Cell>(cell->GetSecond())) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.empty()) {
            return std::make_shared<Cell>(nullptr, nullptr);
        }
        if (arguments.size() > 1) {
            throw RuntimeError("Exactly 1 argument (list) required for \"Quote\" function");
        }
        return arguments[0];
    }
};

class Not : public Symbol {
public:
    Not() : Symbol("not") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 1) {
            throw RuntimeError("Exactly 1 argument required for \"Not\" function");
        }
        std::shared_ptr<Object> value = arguments[0]->Evaluate({}, scope);

        if (Is<Boolean>(value)) {
            return As<Boolean>(value)->GetValue() ? std::make_shared<Boolean>(false) : std::make_shared<Boolean>(true);
        }
        return std::make_shared<Boolean>(false);
    }
};

class And : public Symbol {
public:
    And() : Symbol("and") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);
        std::shared_ptr<Object> value = nullptr;
        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(false);
        std::shared_ptr<Object> value = nullptr;
        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Boolean>(true);

        std::shared_ptr<Object> last_value = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Number>(0);
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        std::shared_ptr<Object> ans = std::make_shared<Number>(1);
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() <= 1) {
            throw RuntimeError("More than 1 argument required for \"Subtract\" function");
        }
        std::shared_ptr<Object> ans = std::make_shared<Number>(0);
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() <= 1) {
            throw RuntimeError("More than 1 argument required for \"Divide\" function");
        }
        std::shared_ptr<Object> ans = std::make_shared<Number>(1);
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.empty()) {
            throw RuntimeError("At least 1 argument required for \"Max\" function");
        }
        std::shared_ptr<Object> ans = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.empty()) {
            throw RuntimeError("At least 1 argument required for \"Min\" function");
        }
        std::shared_ptr<Object> ans = nullptr;
        std::shared_ptr<Object> value = nullptr;

        for (int argument_idx = 0; argument_idx < arguments.size(); ++argument_idx) {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
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

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 2) {
            throw RuntimeError("Exactly 2 arguments required for \"Define\" function");
        }

        if (!Is<Symbol>(arguments[0])) {
            throw RuntimeError("\"Define\" error: first argument should be a variable name");
        }
        std::string name = As<Symbol>(arguments[0])->GetName();
        std::shared_ptr<Object> value = arguments[1];
        scope.SetVariableValue(name, value);
        return nullptr;
    }
};

class Set : public Symbol {
public:
    Set() : Symbol("set!") {
    }

    virtual std::shared_ptr<Object> Evaluate(const std::vector<std::shared_ptr<Object>>& arguments, Scope& scope) override {
        if (arguments.size() != 2) {
            throw RuntimeError("Exactly 2 arguments required for \"Set\" function");
        }

        if (!Is<Symbol>(arguments[0])) {
            throw RuntimeError("\"Set\" error: first argument should be a variable name");
        }
        std::string name = As<Symbol>(arguments[0])->GetName();
        std::shared_ptr<Object> old_value = scope.GetVariableValue(name); // to make sure the variable exists

        std::shared_ptr<Object> new_value = arguments[1];
        scope.SetVariableValue(name, new_value);
        return nullptr;
    }
};