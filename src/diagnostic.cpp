#include "cppseed/diagnostic.hpp"

#include <array>
#include <ostream>

namespace cppseed {

void print_diagnostic(const Diagnostic& diagnostic, std::ostream& output) {
    output << "error: " << diagnostic.message << '\n';
    if (diagnostic.hint) {
        output << "hint: " << *diagnostic.hint << '\n';
    }
    if (diagnostic.warning) {
        output << "warning: " << *diagnostic.warning << '\n';
    }
}

std::string quote_for_diagnostic(const std::string_view value) {
    constexpr std::size_t maximum_bytes = 128;
    constexpr std::array<char, 16> hex{
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    std::string result{"`"};
    const auto count = value.size() < maximum_bytes ? value.size() : maximum_bytes;
    for (std::size_t index = 0; index < count; ++index) {
        const auto byte = static_cast<unsigned char>(value[index]);
        if (byte < 0x20U || byte == 0x7FU) {
            result += "\\x";
            result += hex[(byte >> 4U) & 0x0FU];
            result += hex[byte & 0x0FU];
        } else {
            result += static_cast<char>(byte);
        }
    }
    if (value.size() > maximum_bytes) {
        result += "...";
    }
    result += '`';
    return result;
}

}  // namespace cppseed
