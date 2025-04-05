#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <sstream>

#define DIE internal::FatalLogMessage()

namespace internal {
    class FatalLogMessage : public std::basic_ostringstream<char> {
    public:
        ~FatalLogMessage();
    };   
} // namespace internal

class JitProgram {
    public:
        JitProgram(const std::vector<uint8_t>& code);
        ~JitProgram();

        void* get_program_memory() const {
            return this->m_program_memory;
        }

        size_t get_program_size() const {
            return m_program_size;
        }

    private:
        void* m_program_memory = nullptr;
        size_t m_program_size  = 0;
};

class CodeEmitter {
    public:
        CodeEmitter() = default;
    
        void EmitByte(uint8_t byte);
    
        void EmitBytes(std::initializer_list<uint8_t> bytes);
    
        void EmitUint32(uint32_t value);

        void EmitUint64(uint64_t value);
    
        void ReplaceByteAtOffset(size_t offset, uint8_t replacemet_byte);
    
        void ReplaceUint32AtOffset(size_t offset, uint32_t replacemet_value);
    
        size_t get_size() const {
            return m_code.size();
        }
    
        const std::vector<uint8_t>& get_code() const {
            return m_code;
        }
    
    private:
        std::vector<uint8_t> m_code;
};

uint32_t compute_relative_32bit_offset(size_t jump_from, size_t jump_to);