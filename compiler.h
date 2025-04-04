#pragma once

#include "tokenizer.h"
#include <memory>
#include <vector>

class Compiler {
    public:
        Compiler() = default;

        bool initialize(const std::string& path_to_bf_file);

        void compile();

    private:
        Tokenizer m_tokenizer;   

        std::unique_ptr<std::vector<Tokens>> m_tokens;    

        std::string m_obj_file_name;
};