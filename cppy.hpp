#ifndef CPPY_LIBRARY_H
#define CPPY_LIBRARY_H
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace cppy {

    namespace _concepts {

        template<typename T>
        concept IsSubscriptable = requires(T val, std::size_t i)
        {
            val[i];
        };

        template <typename T>
        concept IsNumber = (std::integral<T> || std::floating_point<T>)
               && !std::same_as<T, bool>
               && !std::same_as<T, char>;

    }

    class var {

        using Array = std::vector<var>;
        using var_type = std::variant<std::monostate, bool, double, std::string, Array>;

        var_type _data;


    public:

        var() { _data = std::monostate{}; }
        var(var_type data) { _data = std::move(data); }
        var(bool data) { _data = data; }

        template <typename T, typename = std::enable_if<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>>
        var(T data) { _data = static_cast<double>(data); }

        var(const char* data) { _data = std::string(data); }
        var(std::string data) { _data = std::move(data); }
        var(Array data) { _data = std::move(data); }
        var(const std::vector<var_type>& data) {
            Array result;
            for (const auto& item : data) {
                result.emplace_back(item);
            }
            _data = result;
        }
        var(const std::initializer_list<var> data) { _data = Array(data); }

        var operator[](int x) {
            return std::visit([x](auto& data) -> var {
                using T = std::decay_t<decltype(data)>;

                if constexpr(_concepts::IsSubscriptable<T>) {
                    return data[x];
                }

                throw std::runtime_error("Use of [] on non-subscriptable variable");
            }, _data);
        }

        operator std::string() const {
            return std::visit([](auto& data) -> std::string {
                using T = std::decay_t<decltype(data)>;

                if constexpr (_concepts::IsNumber<T>) {
                    return std::format("{}", data);
                }

                if constexpr (std::is_constructible_v<std::string, T>) {
                    return std::string(data);
                }

                if constexpr (std::is_constructible_v<bool, T>) {
                    return data? "true" : "false";
                }

                throw std::runtime_error("Cannot convert to string");
            }, _data);
        }

        operator double() const {
            return std::visit([](auto& val) -> double {
                using T = std::decay_t<decltype(val)>;

                if constexpr (_concepts::IsNumber<T>) {
                    return static_cast<double>(val);
                }

                if constexpr (std::is_constructible_v<std::string, T>) {
                    auto val_str = std::string(val);
                    return std::stod(val_str);
                }

                if constexpr (std::is_same_v<bool, T>) {
                    return val ? 1 : 0;
                }

                throw std::runtime_error("Cannot convert variable to number");

            }, _data);
        }

        friend std::ostream& operator<<(std::ostream& stream, const var& val) {
            return stream << std::string(val);
        }

        var operator+(const auto& right_op) {
            var right_var = right_op;
            return std::visit([this, right_var](auto& left, auto& right) -> var {
                using T_left = std::decay_t<decltype(left)>;
                using T_right = std::decay_t<decltype(right)>;

                if constexpr (std::is_constructible_v<double, T_left> && std::is_constructible_v<double, T_right>) {
                    return static_cast<double>(*this) + static_cast<double>(right_var);
                }

                if constexpr (std::is_constructible_v<std::string, T_left> || std::is_constructible_v<std::string, T_right>) {
                    return std::string(*this) + std::string(right_var);
                }

                throw std::runtime_error("Cannot add variables");

            }, _data, right_var._data);
        }

    };

}

#endif // CPPY_LIBRARY_H
