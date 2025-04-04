#include "tokenizer.h"
#include <iostream>
#include <fstream>

bool Tokenizer::initialize(const std::string& path_to_bf_file) {
    this->m_tokens = std::make_unique<std::vector<Tokens>>();

    std::ifstream input_file(path_to_bf_file);

    if(!input_file.is_open()) {
        std::cout << "Failed to load .bf file";
        return false;
    }

    this->m_code = std::string((std::istreambuf_iterator<char>(input_file)),std::istreambuf_iterator<char>());

    return true;
}

std::unique_ptr<std::vector<Tokens>> Tokenizer::tokenize() {
    auto& tokens = *(this->m_tokens);
    for(const char& ch : m_code) {
        switch (ch)
        {
            case '>':
                tokens.push_back(Tokens::MoveRight);
                break;
            case '<':
                tokens.push_back(Tokens::MoveLeft);
                break;
            case '+':
                tokens.push_back(Tokens::Increment);
                break;
            case '-':
                tokens.push_back(Tokens::Decrement);
                break;
            case '.':
                tokens.push_back(Tokens::Print);
                break;
            case ',':
                tokens.push_back(Tokens::Input);
                break;
            case '[':
                tokens.push_back(Tokens::OpenBracket);
                break;
            case ']':
                tokens.push_back(Tokens::CloseBracket);
                break;
            default:
                tokens.push_back(Tokens::Comment);
                break;
        }
    }
    
    return std::move(this->m_tokens);
}