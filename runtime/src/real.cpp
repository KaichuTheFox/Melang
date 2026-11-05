#include <Windows.h>

#include <bit>
#include <charconv>
#include <cstdint>
#include <limits>

extern "C" void melang_print(const char* text, int length);
extern "C" [[noreturn]] void melang_runtime_error(const char* message, int length);

namespace {
template <std::size_t size>
[[noreturn]] void numericError(const char (&message)[size]) {
    melang_runtime_error(message, static_cast<int>(size - 1));
}

template<class Real>
void printReal(Real value) {
    char buffer[32];
    const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
    if (result.ec != std::errc{}) numericError("Melang runtime: cannot format a real value.\r\n");
    melang_print(buffer, static_cast<int>(result.ptr - buffer));
}
}

extern "C" void melang_print_float(float value) {
    printReal(value);
}

extern "C" void melang_print_double(double value) {
    printReal(value);
}

extern "C" float melang_narrow_real(double value) {
    constexpr std::uint64_t doubleInfinity = 0x7ff0000000000000;
    constexpr auto floatLimit =
        std::bit_cast<std::uint64_t>(static_cast<double>((std::numeric_limits<float>::max)()));
    const auto magnitude = std::bit_cast<std::uint64_t>(value) & 0x7fffffffffffffff;
    if (magnitude < doubleInfinity && magnitude > floatLimit) {
        numericError("Melang runtime: real value is outside the float range.\r\n");
    }
    return static_cast<float>(value);
}

extern "C" std::uint32_t melang_real_to_int(double value) {
    constexpr int fractionBits = 52;
    constexpr std::uint64_t leadingBit = std::uint64_t{1} << fractionBits;
    const auto raw = std::bit_cast<std::uint64_t>(value);
    const int biasedExponent = static_cast<int>((raw >> fractionBits) & 0x7ff);
    if (biasedExponent == 0x7ff) {
        numericError("Melang runtime: cannot convert a non-finite real to an integer.\r\n");
    }
    const int exponent = biasedExponent - 1023;
    if (exponent < -1 || exponent >= fractionBits + 32) return 0;

    const auto significand = leadingBit | (raw & (leadingBit - 1));
    std::uint32_t lowBits;
    if (exponent >= fractionBits) {
        lowBits = static_cast<std::uint32_t>(significand) << (exponent - fractionBits);
    } else {
        const int shift = fractionBits - exponent;
        const auto whole = significand >> shift;
        const auto halfway = std::uint64_t{1} << (shift - 1);
        const auto fraction = significand & ((halfway << 1) - 1);
        const bool roundUp = fraction > halfway || (fraction == halfway && (whole & 1) != 0);
        lowBits = static_cast<std::uint32_t>(whole + roundUp);
    }
    return (raw >> 63) ? 0u - lowBits : lowBits;
}
