#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "tokens.h"

class Tokenizer {
 public:
  Tokenizer() = default;

  bool initialize(const std::string &path_to_bf_file);

  std::unique_ptr<std::vector<Tokens>> tokenize();

 private:
  std::unique_ptr<std::vector<Tokens>> m_tokens;

  std::string m_code;
};
