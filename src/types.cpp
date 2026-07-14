#include "cppseed/types.hpp"

namespace cppseed {

std::string_view to_number(const CppStandard value) noexcept {
    switch (value) {
        case CppStandard::cpp17:
            return "17";
        case CppStandard::cpp20:
            return "20";
        case CppStandard::cpp23:
            return "23";
    }
    return "20";
}

std::string_view to_cmake_feature(const CppStandard value) noexcept {
    switch (value) {
        case CppStandard::cpp17:
            return "cxx_std_17";
        case CppStandard::cpp20:
            return "cxx_std_20";
        case CppStandard::cpp23:
            return "cxx_std_23";
    }
    return "cxx_std_20";
}

}  // namespace cppseed
