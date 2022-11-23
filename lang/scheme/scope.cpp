#include <scope.h>
#include <error.h>

void Scope::SetPreviousScope(Scope* previous_scope) {
    previous_scope_ = previous_scope;
}

std::shared_ptr<Object> Scope::GetVariableValue(std::string name) {
    std::unordered_map<std::string, std::shared_ptr<Object>>* cur_variables = &variables_;
    Scope* cur_scope = this;

    while (cur_scope) {
        auto found = std::find_if( cur_variables->begin(), cur_variables->end(), [&](const auto &p) { return p.first == name; });
        if (found != cur_variables->end()) {
            return found->second;
        }
        cur_scope = cur_scope->previous_scope_;
        if (cur_scope) {
            cur_variables = &cur_scope->variables_;
        }
    }

    throw NameError(name);
}

void Scope::SetVariableValue(std::string name, std::shared_ptr<Object> value) {
    variables_[name] = value;
}