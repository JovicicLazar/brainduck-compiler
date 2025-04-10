#include "../headers/compiler.h"

#include <iostream>
#include <sstream>
#include <stack>
#include <iomanip>

#include "../headers/utils.h"
#include "../headers/tokenizer.h"

using JittedFunction = void (*)(void);

namespace {
    constexpr int MEMORY_SIZE = 30000;
}

bool Compiler::initialize(const std::string& path_to_bf_file) {
    Tokenizer tokenizer;

    if(!tokenizer.initialize(path_to_bf_file)) {
        std::cerr << "Could not initialize tokenizer." << std::endl;
        return false;
    }

    this->m_tokens = tokenizer.tokenize();

    return true;
}

void Compiler::compile() {
    CodeEmitter emitter;
    
    std::stack<size_t> open_bracket_stack;

    std::vector<uint8_t> memory(MEMORY_SIZE, 0);

    emitter.EmitBytes({0x49, 0xBD});
    emitter.EmitUint64((uint64_t)memory.data());

    for(size_t pc = 0; pc < (*m_tokens).size(); ++pc) {
        auto token = (*m_tokens)[pc];
        switch (token) {
        case Tokens::MoveRight:
            emitter.EmitBytes({0x49, 0xFF, 0xC5});
            break;
        case Tokens::MoveLeft:
            emitter.EmitBytes({0x49, 0xFF, 0xCD});
            break;
        case Tokens::Increment:
            emitter.EmitBytes({0x41, 0x80, 0x45, 0x00, 0x01});
            break;
        case Tokens::Decrement:
            emitter.EmitBytes({0x41, 0x80, 0x6D, 0x00, 0x01});
            break;
        case Tokens::Print:
            emitter.EmitBytes({0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00});
            emitter.EmitBytes({0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00});
            emitter.EmitBytes({0x4C, 0x89, 0xEE});
            emitter.EmitBytes({0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00});
            emitter.EmitBytes({0x0F, 0x05});
            break;
        case Tokens::Input:
            emitter.EmitBytes({0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00});
            emitter.EmitBytes({0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00});
            emitter.EmitBytes({0x4C, 0x89, 0xEE});
            emitter.EmitBytes({0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00});
            emitter.EmitBytes({0x0F, 0x05});
            break;
        case Tokens::OpenBracket:
            emitter.EmitBytes({0x41, 0x80, 0x7d, 0x00, 0x00});

            open_bracket_stack.push(emitter.get_size());

            emitter.EmitBytes({0x0F, 0x84});
            emitter.EmitUint32(0);
            break;
        case Tokens::CloseBracket: {
            if (open_bracket_stack.empty()) {
                DIE << "unmatched closing ']' at pc=" << pc;
            }

            size_t open_bracket_offset = open_bracket_stack.top();
            open_bracket_stack.pop();

            emitter.EmitBytes({0x41, 0x80, 0x7d, 0x00, 0x00});

            // open_bracket_offset points to the JZ that jumps to this closing
            // bracket. We'll need to fix up the offset for that JZ, as well as emit a
            // JNZ with a correct offset back. Note that both [ and ] jump to the
            // instruction *after* the matching bracket if their condition is
            // fulfilled.

            // Compute the offset for this jump. The jump start is computed from after
            // the jump instruction, and the target is the instruction after the one
            // saved on the stack.
            size_t jump_back_from      = emitter.get_size() + 6;
            size_t jump_back_to        = open_bracket_offset + 6;
            uint32_t pcrel_offset_back = compute_relative_32bit_offset(jump_back_from, jump_back_to);

            emitter.EmitBytes({0x0F, 0x85});
            emitter.EmitUint32(pcrel_offset_back);

            size_t jump_forward_from = open_bracket_offset + 6;
            size_t jump_forward_to = emitter.get_size();
            uint32_t pcrel_offset_forward = compute_relative_32bit_offset(jump_forward_from, jump_forward_to);
            emitter.ReplaceUint32AtOffset(open_bracket_offset + 2, pcrel_offset_forward);

            break;
        }
        default:
            break;
        }
    }

    if(!open_bracket_stack.empty()) {
        DIE << "Unmatched [ bracket at pc=" << open_bracket_stack.top();
    }

    // The emitted code will be called as a function from C++; therefore it has to
    // use the proper calling convention. Emit a 'ret' for orderly return to the
    // caller.
    emitter.EmitByte(0xC3);

    std::vector<uint8_t> emitted_code = emitter.get_code();

    JitProgram jit_program(emitted_code);
    JittedFunction func = (JittedFunction)jit_program.get_program_memory();

    func();
}

