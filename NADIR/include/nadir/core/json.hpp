#pragma once
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace nadir::json {

struct Value;
using Array = std::vector<Value>;
using Object = std::map<std::string, Value, std::less<>>;

struct Value {
    using Storage = std::variant<std::nullptr_t, bool, double, std::string, Array, Object>;
    Storage data{};

    bool is_null() const;
    bool is_bool() const;
    bool is_number() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;
    bool as_bool(bool fallback=false) const;
    double as_number(double fallback=0.0) const;
    std::string as_string(std::string fallback={}) const;
    const Array* as_array() const;
    const Object* as_object() const;
    const Value* get(std::string_view key) const;
};

struct ParseResult {
    bool ok{};
    Value value;
    std::size_t offset{};
    std::string error;
};

ParseResult parse(std::string_view text);
std::optional<std::string> read_text_file(const std::string& path);

}
