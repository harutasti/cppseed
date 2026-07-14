#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

#include "cppseed/types.hpp"

namespace cppseed {

void print_diagnostic(const Diagnostic& diagnostic, std::ostream& output);
[[nodiscard]] std::string quote_for_diagnostic(std::string_view value);

}  // namespace cppseed
