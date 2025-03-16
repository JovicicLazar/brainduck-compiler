#include <iostream>
#include <string>
#include <vector>
#include "compiler.h"
#include "tokens.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.bf> [-o output]\n";
        std::cerr << "Example: " << argv[0] << " code.bf -o code\n";
        return 1;
    }

    std::string input_file;
    std::string output_file = "a.out";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-o") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                std::cerr << "Error: -o requires an output filename\n";
                return 1;
            }
        } else if (input_file.empty()) {
            input_file = arg;
        } else {
            std::cerr << "Error: Unexpected argument '" << arg << "'\n";
            return 1;
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: No input file specified\n";
        return 1;
    }

    if(input_file.find(".bf") == -1) {
        std::cerr << "Error: No .bf file specified\n";
        return 1;
    }

    Compiler compiler;
    if (!compiler.initialize(input_file)) {
        std::cerr << "Error: Failed to initialize with " << input_file << "\n";
        return 1;
    }

    compiler.compile(output_file);

    return 0;
}