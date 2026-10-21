#ifndef CPPY_LIBRARY_H
#define CPPY_LIBRARY_H
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

        using Object = std::unordered_map<std::string, var>;
        using Array = std::vector<var>;
        using var_type = std::variant<std::monostate, bool, double, std::string, Array, Object>;

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
        var(Object data) { _data = std::move(data); }

        var operator[](int x) {
            return std::visit([x](auto& data) -> var {
                using T = std::decay_t<decltype(data)>;
                if constexpr(_concepts::IsSubscriptable<T>) {
                    return data[x];
                }
                throw std::runtime_error("Use of [] on non-subscriptable variable");
            }, _data);
        }

        operator std::string() {
            std::string str;
            std::visit([&str](auto& data) {
                using T = std::decay_t<decltype(data)>;
                if constexpr (_concepts::IsNumber<T>) {
                    str = std::to_string(data);
                    return;
                }
                if constexpr (std::is_assignable_v<std::string, T>) {
                    str = data;
                    return;
                }
                throw std::runtime_error("Cannot convert to string");
            }, _data);
            return str;
        }

        friend std::ostream& operator<<(std::ostream& stream, var val) {
            return stream << std::string(val);
        }

    };

}

#endif // CPPY_LIBRARY_H
