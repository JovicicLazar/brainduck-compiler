#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../headers/tokens.h"

class Compiler {
 public:
  Compiler() = default;

  bool initialize(const std::string& path_to_bf_file);

  void compile();

 private:
  std::unique_ptr<std::vector<Tokens>> m_tokens;
};
