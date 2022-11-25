#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>

#include <scheme.h>
#include <tokenizer.h>
#include <parser.h>

std::string Scheme::Evaluate(const std::string& expression) {
    std::stringstream ss{expression};
    Tokenizer tokenizer{&ss};
    std::shared_ptr<Object> current = Read(&tokenizer);

    if (!current) {  // Empty list, but not evaliated
        throw RuntimeError("Empty list given, lists are not self evaliating, use \"quote\"");
    }

    // "Is" methods
    global_scope_->SetVariableValue("boolean?", std::make_shared<IsBoolean>());
    global_scope_->SetVariableValue("number?", std::make_shared<IsNumber>());
    global_scope_->SetVariableValue("symbol?", std::make_shared<IsSymbol>());
    global_scope_->SetVariableValue("pair?", std::make_shared<IsPair>());
    global_scope_->SetVariableValue("null?", std::make_shared<IsNull>());
    global_scope_->SetVariableValue("list?", std::make_shared<IsList>());

    // Logical operators
    global_scope_->SetVariableValue("not", std::make_shared<Not>());
    global_scope_->SetVariableValue("and", std::make_shared<And>());
    global_scope_->SetVariableValue("or", std::make_shared<Or>());

    // Comparisons
    global_scope_->SetVariableValue("=", std::make_shared<Equal>());
    global_scope_->SetVariableValue(">", std::make_shared<Greater>());
    global_scope_->SetVariableValue(">=", std::make_shared<GreaterEqual>());
    global_scope_->SetVariableValue("<", std::make_shared<Less>());
    global_scope_->SetVariableValue("<=", std::make_shared<LessEqual>());

    // Arithmetic
    global_scope_->SetVariableValue("+", std::make_shared<Add>());
    global_scope_->SetVariableValue("*", std::make_shared<Multiply>());
    global_scope_->SetVariableValue("-", std::make_shared<Subtract>());
    global_scope_->SetVariableValue("/", std::make_shared<Divide>());
    global_scope_->SetVariableValue("max", std::make_shared<Max>());
    global_scope_->SetVariableValue("min", std::make_shared<Min>());
    global_scope_->SetVariableValue("abs", std::make_shared<Abs>());

    // Variable
    global_scope_->SetVariableValue("define", std::make_shared<Define>());
    global_scope_->SetVariableValue("set!", std::make_shared<Set>());

    // Control-flow
    global_scope_->SetVariableValue("if", std::make_shared<If>());

    // Quote
    global_scope_->SetVariableValue("quote", std::make_shared<Quote>());

    // List operations
    global_scope_->SetVariableValue("cons", std::make_shared<Cons>());
    global_scope_->SetVariableValue("car", std::make_shared<Car>());
    global_scope_->SetVariableValue("cdr", std::make_shared<Cdr>());
    global_scope_->SetVariableValue("set-car!", std::make_shared<SetCar>());
    global_scope_->SetVariableValue("set-cdr!", std::make_shared<SetCdr>());
    global_scope_->SetVariableValue("list", std::make_shared<List>());
    global_scope_->SetVariableValue("list-ref", std::make_shared<ListRef>());
    global_scope_->SetVariableValue("list-tail", std::make_shared<ListTail>());

    std::shared_ptr<Object> ans = current->Evaluate({}, global_scope_);

    if (!ans) {
        return "()";
    } else if (Is<Number>(ans)) {
        int value = As<Number>(ans)->GetValue();
        return std::to_string(value);
    } else if (Is<Boolean>(ans)) {
        bool value = As<Boolean>(ans)->GetValue();
        return value ? "#t" : "#f";
    } else if (Is<Cell>(ans)) {
        return ListToString(ans);
    } else if (Is<Symbol>(ans)) {
        return As<Symbol>(ans)->GetName();
    } else {
        throw std::runtime_error("Unimplemented");
    }
}