#include <parser.h>
#include <error.h>

#include <stdexcept>

std::shared_ptr<Object> GetSymbolQuoteConstant(Tokenizer* tokenizer, Token& token) {
    std::shared_ptr<Object> first = nullptr;

    if (SymbolToken* symbol_token = std::get_if<SymbolToken>(&token); symbol_token) {
        if (symbol_token->name == "#t") {
            first = std::make_shared<Boolean>(true);
        } else if (symbol_token->name == "#f") {
            first = std::make_shared<Boolean>(false);
        } else {
            first = std::make_shared<Symbol>(symbol_token->name);
        }
    } else if (QuoteToken* quote_token = std::get_if<QuoteToken>(&token); quote_token) {
        std::shared_ptr<Object> quote_cell = std::make_shared<Cell>(nullptr, nullptr);
        As<Cell>(quote_cell)->SetFirst(std::make_shared<Quote>());
        token = tokenizer->GetToken();
        if (BracketToken* open_bracket = std::get_if<BracketToken>(&token);
            open_bracket && *open_bracket == BracketToken::OPEN) {
            tokenizer->Next();
            std::shared_ptr<Object> argument = std::make_shared<Cell>(nullptr, nullptr);
            As<Cell>(argument)->SetFirst(ReadList(tokenizer));
            As<Cell>(quote_cell)->SetSecond(argument);
            first = quote_cell;
        } else if (SymbolToken* symbol_token = std::get_if<SymbolToken>(&token); symbol_token) {
            std::shared_ptr<Object> argument = std::make_shared<Cell>(nullptr, nullptr);
            std::shared_ptr<Object> symbol = std::make_shared<Symbol>(symbol_token->name);
            tokenizer->Next();
            As<Cell>(argument)->SetFirst(symbol);
            As<Cell>(quote_cell)->SetSecond(argument);
            first = quote_cell;
        } else {
            throw SyntaxError(
                "Invalid token after quote: expected BracketToken::OPEN or SymbolToken");
        }
    } else if (ConstantToken* constant_token = std::get_if<ConstantToken>(&token); constant_token) {
        first = std::make_shared<Number>(constant_token->value);
    } else {
        throw SyntaxError("Invalid token");
    }

    return first;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    std::shared_ptr<Object> first = nullptr;
    std::shared_ptr<Object> second = nullptr;

    std::shared_ptr<Object> previous = nullptr;
    std::shared_ptr<Object> current = std::make_shared<Cell>(nullptr, nullptr);
    std::shared_ptr<Object> ans = current;
    std::shared_ptr<Object> next = nullptr;

    Token token{};

    while (!tokenizer->IsEnd()) {
        token = tokenizer->GetToken();
        tokenizer->Next();

        if (BracketToken* open_bracket = std::get_if<BracketToken>(&token);
            open_bracket && *open_bracket == BracketToken::OPEN) {
            first = ReadList(tokenizer);
        } else {
            first = GetSymbolQuoteConstant(tokenizer, token);
        }

        As<Cell>(current)->SetFirst(first);
        next = std::make_shared<Cell>(nullptr, nullptr);
        As<Cell>(current)->SetSecond(next);

        previous = current;
        current = next;
    }
    As<Cell>(previous)->SetSecond(nullptr);

    // If ans consists of a single cell, set ans = ans->first
    if (!As<Cell>(ans)->GetSecond()) {
        ans = As<Cell>(ans)->GetFirst();
    }
    return ans;
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    std::shared_ptr<Object> first = nullptr;
    std::shared_ptr<Object> second = nullptr;

    std::shared_ptr<Object> previous = nullptr;
    std::shared_ptr<Object> current = std::make_shared<Cell>(nullptr, nullptr);
    std::shared_ptr<Object> ans = current;
    std::shared_ptr<Object> next = nullptr;

    Token token{};
    if (tokenizer->IsEnd()) {
        throw SyntaxError("No tokens after BracketToken::OPEN");
    }
    while (!tokenizer->IsEnd()) {
        token = tokenizer->GetToken();
        tokenizer->Next();

        if (BracketToken* open_bracket = std::get_if<BracketToken>(&token);
            open_bracket && *open_bracket == BracketToken::OPEN) {
            first = ReadList(tokenizer);
        } else if (BracketToken* close_bracket = std::get_if<BracketToken>(&token);
                   close_bracket && *close_bracket == BracketToken::CLOSE) {
            if (current == ans) {
                // Empty list <=> nullptr
                return nullptr;
            }
            break;
        } else if (DotToken* dot_token = std::get_if<DotToken>(&token); dot_token) {
            if (current == ans) {
                throw SyntaxError("DotToken after BracketToken::OPEN");
            }
            break;
        } else {
            first = GetSymbolQuoteConstant(tokenizer, token);
        }

        As<Cell>(current)->SetFirst(first);
        next = std::make_shared<Cell>(nullptr, nullptr);
        As<Cell>(current)->SetSecond(next);

        previous = current;
        current = next;
    }
    As<Cell>(previous)->SetSecond(nullptr);

    // Handle DotToken
    if (DotToken* dot_token = std::get_if<DotToken>(&token); dot_token) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError("No token after DotToken");
        }
        token = tokenizer->GetToken();
        tokenizer->Next();

        if (BracketToken* open_bracket = std::get_if<BracketToken>(&token);
            open_bracket && *open_bracket == BracketToken::OPEN) {
            second = ReadList(tokenizer);
        } else if (ConstantToken* constant_token = std::get_if<ConstantToken>(&token);
                   constant_token) {
            second = std::make_shared<Number>(constant_token->value);
        } else {
            throw SyntaxError("Invalid token after DotToken");
        }
        As<Cell>(previous)->SetSecond(second);

        token = tokenizer->GetToken();
        tokenizer->Next();
    }

    if (BracketToken* close_bracket = std::get_if<BracketToken>(&token);
        close_bracket && *close_bracket == BracketToken::CLOSE) {
        return ans;
    } else {
        throw SyntaxError("Close bracket error");
    }
}