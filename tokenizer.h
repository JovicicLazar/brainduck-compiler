#pragma once
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include "tokens.h"

class Tokenizer {
    public:
        Tokenizer() = default;

        bool initialize(const std::string& path_to_bf_file);

        std::unique_ptr<std::vector<Tokens>> tokenize();

    private:
        std::unique_ptr<std::vector<Tokens>> m_tokens;

        std::string m_code;
};