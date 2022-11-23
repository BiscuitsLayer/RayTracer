#include <memory>
#include <algorithm>
#include <unordered_map>

// Forward declaration
class Object;

class Scope : public std::enable_shared_from_this<Scope> {
public:
    Scope() : previous_scope_(nullptr), variables_({}) {
    }

    void SetPreviousScope(Scope* previous_scope);
    std::shared_ptr<Object> GetVariableValue(std::string name);
    void SetVariableValue(std::string name, std::shared_ptr<Object> value);

private:
    Scope* previous_scope_ = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Object>> variables_{};
};