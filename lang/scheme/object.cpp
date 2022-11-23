#include <object.h>

std::vector<std::shared_ptr<Object>> ListToVector(std::shared_ptr<Object> init) {
    std::vector<std::shared_ptr<Object>> ans{};
    for (std::shared_ptr<Cell> cell = As<Cell>(init); cell; cell = As<Cell>(cell->GetSecond())) {
        std::shared_ptr<Object> obj = cell->GetFirst();
        if (!obj) {
            return ans;
        }
        ans.push_back(obj);
    }
    return ans;
}

std::string ListToString(std::shared_ptr<Object> init) {
    std::string ans = "(";
    for (std::shared_ptr<Cell> cell = As<Cell>(init); cell; cell = As<Cell>(cell->GetSecond())) {
        std::shared_ptr<Object> first = cell->GetFirst();
        if (!first) {
            ans += ")";
            return ans;
        }
        if (!Is<Number>(first)) {
            throw RuntimeError("ListToString: first element is not a number");
        }
        // it may be a symbol as well...
        std::string value = std::to_string(As<Number>(first)->GetValue());
        std::shared_ptr<Object> second = cell->GetSecond();
        if (!second) {
            ans += value + ")";
            return ans;
        } else {
            ans += value + " ";
            if (!Is<Cell>(second)) { // If second is not Cell, we should use Dot
                std::string value = std::to_string(As<Number>(second)->GetValue());
                ans += ". " + value + ")";
                return ans;
            }
        }
    }
    return ans;
}