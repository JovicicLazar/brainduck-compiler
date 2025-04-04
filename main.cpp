#include <iostream>
#include <string>
#include <vector>
#include "compiler.h"
#include "tokens.h"

int main(int argc, char* argv[]) {
    Compiler compiler;
    compiler.initialize("test.bf");
    compiler.compile();

    return 0;
}