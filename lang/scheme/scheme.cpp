#include <sstream>
#include <stdexcept>
#include <cassert>
#include <vector>
#include <map>

#include <scheme.h>
#include <tokenizer.h>
#include <parser.h>

std::string Scheme::Evaluate(const std::string& expression) {
    std::stringstream ss{expression};
    Tokenizer tokenizer{&ss};

    std::shared_ptr<Object> current = Read(&tokenizer);
    assert(tokenizer.IsEnd());

    if (!current) { // Empty list, but not evaliated
        throw RuntimeError("Empty list given, lists are not self evaliating, use \"quote\"");
    }
    std::shared_ptr<Object> ans = current->Evaluate({}, global_scope_);

    if (!ans) {
        return "";
    } else if (Is<Number>(ans)) {
        int value = As<Number>(ans)->GetValue();
        return std::to_string(value);
    } else if (Is<Boolean>(ans)) {
        bool value = As<Boolean>(ans)->GetValue();
        return value ? "#t" : "#f";
    } else if (Is<Cell>(ans)) {
        return ListToString(ans);
    } else {
        throw std::runtime_error("Unimplemented");
    }
}