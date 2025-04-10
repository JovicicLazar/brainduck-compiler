#include <iostream>
#include <string>
#include <vector>

#include "headers/compiler.h"
#include "headers/utils.h"

constexpr uint8_t NUMBER_OF_ARGV = 2;

int main(int argc, char* argv[]) {
    if(argc != NUMBER_OF_ARGV) {
        std::cout << "Usage: brain <file_name.bf>" << std::endl;
        return 0;
    }

    std::string file_name(argv[1]);

    Compiler compiler;

    if(!compiler.initialize(file_name)) {
        DIE << "Initialization error";
    }

    compiler.compile();
    
    return 0;
}
