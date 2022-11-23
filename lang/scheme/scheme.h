#pragma once

#include <string>

#include <scope.h>

class Scheme {
public:
    std::string Evaluate(const std::string& expression);

private:
    Scope global_scope_{};
};