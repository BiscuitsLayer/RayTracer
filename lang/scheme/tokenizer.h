#pragma once

#include <error.h>

#include <variant>
#include <istream>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

enum class DummyToken { DUMMY };

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, DummyToken>;

// Интерфейс позволяющий читать токены по одному из потока.
class Tokenizer {
public:
    Tokenizer(std::istream* in) : in_{in}, cur_token_{DummyToken{}}, is_end_(false) {
        Next();
    }

    bool IsEnd() {
        return is_end_;
    }

    // Three cool facts:
    // 1. (*in_ >> std::ws).peek() <- allows to skip whitespaces and peek next non-whitespace symbol
    // 2. in_->peek() sets eof_bit, reading the last symbol in stream doesn't do that, so
    // in_->peek() should be called before checking in_->eof()
    // 3. while (...) { ... return; } <=> if (...) { ... return; } <- acting the same way
    // but in the while-case we can use "break" to get out
    void Next() {
        if (is_end_) {
            throw SyntaxError("Expected Token, got EOF");
        }

        // End Token
        char symbol = 0;
        if (!(*in_ >> symbol)) {
            is_end_ = true;
            return;
        }
        is_end_ = false;

        // Constant Token
        while (isdigit(symbol) || (isdigit(in_->peek()) && ((symbol == '-') || (symbol == '+')))) {
            int number = 0;
            int sign = 0;
            if (isdigit(symbol)) {
                number += symbol - '0';
                sign = +1;
            } else if (isdigit(in_->peek())) {
                number = 0;
                if (symbol == '-') {
                    sign = -1;
                } else if (symbol == '+') {
                    sign = +1;
                }
            }
            while (in_->peek(), !in_->eof() && isdigit(in_->peek())) {
                number *= 10;
                *in_ >> symbol;
                number += (symbol - '0');
            }
            cur_token_ = ConstantToken{sign * number};
            return;
        }

        // Service Tokens
        if (!isdigit(symbol)) {
            std::string str{};
            switch (symbol) {
                case '\'': {
                    cur_token_ = QuoteToken{};
                    return;
                }
                case '.': {
                    cur_token_ = DotToken{};
                    return;
                }
                case '(': {
                    cur_token_ = BracketToken::OPEN;
                    return;
                }
                case ')': {
                    cur_token_ = BracketToken::CLOSE;
                    return;
                }
            }
            std::string service{".()' "};
            str += symbol;
            while (in_->peek(),
                   !in_->eof() && !iswspace(in_->peek()) && !isdigit(in_->peek()) &&
                       (std::find(service.begin(), service.end(), in_->peek()) == service.end())) {
                *in_ >> symbol;
                str += symbol;
            }
            cur_token_ = SymbolToken{str};
            return;
        }
    }

    Token GetToken() {
        return cur_token_;
    }

private:
    std::istream* in_ = nullptr;
    Token cur_token_{};
    bool is_end_ = false;
};
