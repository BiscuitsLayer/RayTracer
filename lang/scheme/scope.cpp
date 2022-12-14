#include <scope.h>
#include <error.h>

void Scope::SetPreviousScope(std::shared_ptr<Scope> previous_scope) {
    previous_scope_ = previous_scope;
}

std::shared_ptr<Object> Scope::GetVariableValueLocal(std::string name) {
    std::unordered_map<std::string, std::shared_ptr<Object>>* cur_variables = &variables_;
    std::shared_ptr<Scope> cur_scope = shared_from_this();

    auto found = std::find_if(cur_variables->begin(), cur_variables->end(),
                              [&](const auto& p) { return p.first == name; });
    if (found != cur_variables->end()) {
        return found->second;
    }

    if (name == "lambda") {
        throw SyntaxError("Syntax Error: trying to get lambda as a variable");
    }
    throw NameError(name);
}

std::shared_ptr<Object> Scope::GetVariableValueRecursive(std::string name) {
    std::unordered_map<std::string, std::shared_ptr<Object>>* cur_variables = &variables_;
    std::shared_ptr<Scope> cur_scope = shared_from_this();

    while (cur_scope) {
        auto found = std::find_if(cur_variables->begin(), cur_variables->end(),
                                  [&](const auto& p) { return p.first == name; });
        if (found != cur_variables->end()) {
            return found->second;
        }
        cur_scope = cur_scope->previous_scope_;
        if (cur_scope) {
            cur_variables = &cur_scope->variables_;
        }
    }

    if (name == "lambda") {
        throw SyntaxError("Syntax Error: trying to get lambda as a variable");
    }
    throw NameError(name);
}

void Scope::SetVariableValue(std::string name, std::shared_ptr<Object> value) {
    variables_[name] = value;
}

std::unordered_map<std::string, std::shared_ptr<Object>> Scope::GetVariablesMap() {
    return variables_;
}