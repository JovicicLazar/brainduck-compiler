#include "utils.h"
#include <cassert>
#include <cstring>
#include <limits>
#include <iostream>
#include <sys/mman.h>

namespace internal {

    FatalLogMessage::~FatalLogMessage() {
      fprintf(stderr, "Fatal error: %s\n", str().c_str());
      exit(EXIT_FAILURE);
    }
    
    } // namespace internal

namespace {
    void* allocate_writable_memory(size_t size) {
        void* mem_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(mem_ptr == MAP_FAILED) {
            std::cerr << "Error allocating memory: mmap" << std::endl;
            return nullptr;
        }
        return mem_ptr;
    }   

    bool make_memory_executable(void* memory, size_t size) {
        if(mprotect(memory, size, PROT_READ | PROT_EXEC) == -1) {
            std::cerr << "Error making memory executable: mprotect" << std::endl;
            return false;
        }
        return true;
    }
} // namespace

// JitProgram
JitProgram::JitProgram(const std::vector<uint8_t>& code)
{
    m_program_size = code.size();
    m_program_memory = allocate_writable_memory(m_program_size);

    if(m_program_memory == nullptr) {
        std::cerr << "Couldn't allocate program memory" << std::endl;
        return;
    }

    memcpy(m_program_memory, code.data(), m_program_size);

    if(!make_memory_executable(m_program_memory, m_program_size)) {
        std::cerr << "Couldn't make memory executable" << std::endl;
        return;
    }

}

JitProgram::~JitProgram() {
    if(m_program_memory != nullptr) {
        if(munmap(m_program_memory, m_program_size) == -1) {
            std::cerr << "Error freeing the memory: munmap" << std::endl;
        }
    }
}

// CodeEmitter
void CodeEmitter::EmitByte(uint8_t byte) {
    m_code.push_back(byte);
}

void CodeEmitter::EmitBytes(std::initializer_list<uint8_t> bytes) {
    for(const uint8_t byte : bytes) {
        EmitByte(byte);
    }
}

void CodeEmitter::ReplaceByteAtOffset(size_t offset, uint8_t replacemet_byte) {
    assert(offset < m_code.size() && "replacement fits in code");
    m_code[offset] = replacemet_byte;
}

void CodeEmitter::ReplaceUint32AtOffset(size_t offset, uint32_t replacemet_value) {
    ReplaceByteAtOffset(offset, replacemet_value & 0xFF);
    ReplaceByteAtOffset(offset + 1, (replacemet_value >> 8) & 0xFF);
    ReplaceByteAtOffset(offset + 2, (replacemet_value >> 16) & 0xFF);
    ReplaceByteAtOffset(offset + 3, (replacemet_value >> 24) & 0xFF);
}

void CodeEmitter::EmitUint32(uint32_t value) {
    EmitByte(value & 0xFF);
    EmitByte((value >> 8) & 0xFF);
    EmitByte((value >> 16) & 0xFF);
    EmitByte((value >> 24) & 0xFF);
  }
  
void CodeEmitter::EmitUint64(uint64_t value) {
    EmitUint32(value & 0xFFFFFFFF);
    EmitUint32((value >> 32) & 0xFFFFFFFF);
}

uint32_t compute_relative_32bit_offset(size_t jump_from, size_t jump_to) {
    if (jump_to >= jump_from) {
        size_t diff = jump_to - jump_from;
        assert(diff < (1ull << 31));
        return diff;
    } else {
        // Here the diff is negative, so we need to encode it as 2s complement.
        size_t diff = jump_from - jump_to;
        assert(diff - 1 < (1ull << 31));
        uint32_t diff_unsigned = static_cast<uint32_t>(diff);
        return ~diff_unsigned + 1;
    }
}