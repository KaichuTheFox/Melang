#include "moulang/lexer.hpp"

#include <utility>

namespace moulang {
namespace {
bool identifierStart(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool identifierPart(char c) {
    return identifierStart(c) || (c >= '0' && c <= '9');
}

bool digit(char c) {
    return c >= '0' && c <= '9';
}
}

char Lexer::advance() {
    const char c = source_[offset_++];
    if (c == '\r' || c == '\n') {
        if (c == '\r' && offset_ < source_.size() && source_[offset_] == '\n') {
            ++offset_;
        }
        ++location_.line;
        location_.column = 1;
    } else if ((static_cast<unsigned char>(c) & 0xC0) != 0x80) {
        ++location_.column;
    }
    return c;
}

Token Lexer::readString(Location start) {
    std::string value;
    while (offset_ < source_.size()) {
        const char c = advance();
        if (c == '"') {
            return {TokenKind::string, start, std::move(value)};
        }
        if (c == '\r' || c == '\n') {
            break;
        }
        if (c == '\0') {
            throw Diagnostic(start, "ML1001", "Unexpected NUL in string literal.");
        }
        value.push_back(c);
    }
    throw Diagnostic(start, "ML1002", "Unterminated string literal.");
}

Token Lexer::next() {
    while (offset_ < source_.size()) {
        const auto start = location_;
        const auto begin = offset_;
        const char c = advance();
        switch (c) {
        case ' ':
        case '\t':
            continue;
        case '\n':
        case '\r':
            return {TokenKind::newline, start, {}};
        case '(':
            return {TokenKind::leftParen, start, {}};
        case ')':
            return {TokenKind::rightParen, start, {}};
        case '{':
            return {TokenKind::leftBrace, start, {}};
        case '}':
            return {TokenKind::rightBrace, start, {}};
        case '[':
            return {TokenKind::leftBracket, start, {}};
        case ']':
            return {TokenKind::rightBracket, start, {}};
        case ',':
            return {TokenKind::comma, start, {}};
        case '.':
            return {TokenKind::dot, start, {}};
        case ':':
            return {TokenKind::colon, start, {}};
        case '"':
            return readString(start);
        case '/':
            if (offset_ < source_.size() && source_[offset_] == '/') {
                while (offset_ < source_.size() && source_[offset_] != '\r' && source_[offset_] != '\n') {
                    advance();
                }
                continue;
            }
            break;
        }
        if (digit(c)) {
            const auto peek = [&](std::size_t distance = 0) {
                return distance < source_.size() - offset_ ? source_[offset_ + distance] : '\0';
            };
            while (digit(peek())) advance();
            auto kind = TokenKind::integer;
            if (peek() == '.' && digit(peek(1))) {
                kind = TokenKind::real;
                advance();
                while (digit(peek())) advance();
            }
            if (peek() == 'e' || peek() == 'E') {
                kind = TokenKind::real;
                advance();
                if (peek() == '+' || peek() == '-') advance();
                if (!digit(peek())) throw Diagnostic(start, "ML1003", "Expected digits in the exponent.");
                while (digit(peek())) advance();
            }
            return {kind, start, std::string(source_.substr(begin, offset_ - begin))};
        }
        if (std::string_view("+-*/=<>!&|?^~").find(c) != std::string_view::npos) {
            const char next = offset_ < source_.size() ? source_[offset_] : '\0';
            if (c == '?' && next == '.') {
                advance();
                return {TokenKind::safeDot, start, {}};
            }
            if ((std::string_view("=!<>+-*/").find(c) != std::string_view::npos && next == '=') ||
                (std::string_view("+-&|?<>").find(c) != std::string_view::npos && next == c)) {
                advance();
            }
            return {TokenKind::operation, start, std::string(source_.substr(begin, offset_ - begin))};
        }
        if (identifierStart(c)) {
            while (offset_ < source_.size() && identifierPart(source_[offset_])) {
                advance();
            }
            return {TokenKind::identifier, start, std::string(source_.substr(begin, offset_ - begin))};
        }
        throw Diagnostic(start, "ML1001", "Unexpected character.");
    }
    return {TokenKind::end, location_, {}};
}
}
