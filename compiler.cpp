#include "compiler.h"
#include <iostream>
#include <sstream>
#include <stack>

bool Compiler::initialize(const std::string& path_to_bf_file) {
    if(!this->m_tokenizer.initialize(path_to_bf_file)) {
        std::cout << "Could not initialize tokenizer." << std::endl;
        return false;
    }

    this->m_tokens = this->m_tokenizer.tokenize();

    this->m_obj_file_name = path_to_bf_file.substr(0, path_to_bf_file.find(".bf"));

    return true;
}

void Compiler::convertToAssembly(const std::string& output) {
    std::ofstream out_file(this->m_obj_file_name + ".s");
    if (!out_file.is_open()) {
        std::cerr << "Error: Could not open " << output << " for writing\n";
        return;
    }

    std::stringstream asm_code;
    std::stack<int> loop_stack;
    int loop_counter = 0;

    // BSS section
    asm_code << ".bss\n";
    asm_code << ".align 16\n";
    asm_code << "tape:\n";
    asm_code << "    .space 30000\n";

    // Text section
    asm_code << ".text\n";
    asm_code << ".globl _start\n";
    asm_code << "_start:\n";
    asm_code << "    pushq %rbx\n";
    asm_code << "    movq $tape, %rbx\n";

    auto tokens = *(this->m_tokens);

    for (const Tokens& token : tokens) {
        switch (token) {
        case Tokens::MoveRight:
            asm_code << "    incq %rbx\n";
            break;
        case Tokens::MoveLeft:
            asm_code << "    decq %rbx\n";
            break;
        case Tokens::Increment:
            asm_code << "    incb (%rbx)\n";
            break;
        case Tokens::Decrement:
            asm_code << "    decb (%rbx)\n";
            break;
        case Tokens::Print:
            asm_code << "    movq $1, %rax\n";
            asm_code << "    movq $1, %rdi\n";
            asm_code << "    movq %rbx, %rsi\n";
            asm_code << "    movq $1, %rdx\n";
            asm_code << "    syscall\n";
            break;
        case Tokens::Input:
            asm_code << "    movq $0, %rax\n";
            asm_code << "    movq $0, %rdi\n";
            asm_code << "    movq %rbx, %rsi\n";
            asm_code << "    movq $1, %rdx\n";
            asm_code << "    syscall\n";
            break;
        case Tokens::OpenBracket:
            asm_code << "loop_start_" << loop_counter << ":\n";
            asm_code << "    cmpb $0, (%rbx)\n";
            asm_code << "    je loop_end_" << loop_counter << "\n";
            loop_stack.push(loop_counter);
            ++loop_counter;
            break;
        case Tokens::CloseBracket:
            if (loop_stack.empty()) {
                std::cerr << "Error: Unmatched ] in Brainfuck code\n";
                out_file.close();
                return;
            }
            asm_code << "    cmpb $0, (%rbx)\n";
            asm_code << "    jne loop_start_" << loop_stack.top() << "\n";
            asm_code << "loop_end_" << loop_stack.top() << ":\n";
            loop_stack.pop();
            break;
        case Tokens::Comment:
            break;
        }
    }

    if (!loop_stack.empty()) {
        std::cerr << "Error: Unmatched [ in Brainfuck code\n";
        out_file.close();
        return;
    }

    asm_code << "    popq %rbx\n";
    asm_code << "    movq $60, %rax\n";
    asm_code << "    xorq %rdi, %rdi\n";
    asm_code << "    syscall\n";

    out_file << asm_code.str();
    out_file.close();
}

void Compiler::compile(const std::string& output) {
    this->convertToAssembly(output);

    std::string assemby_cmd = "as " + this->m_obj_file_name + ".s -o " + this->m_obj_file_name + ".o";
    std::string link_cmd = "ld " + this->m_obj_file_name + ".o -o " + output;
    std::string remove_cmd = "rm -rf " + this->m_obj_file_name + ".s " + this->m_obj_file_name + ".o";

    int assembly_result = std::system(assemby_cmd.c_str());
    int link_result = std::system(link_cmd.c_str());
    int remove_result = std::system(remove_cmd.c_str());

    if (assembly_result != 0 || link_result != 0 || remove_result != 0) {
        std::cerr << "Error: Compilation of " << output << " failed\n";
        return;
    }
}

