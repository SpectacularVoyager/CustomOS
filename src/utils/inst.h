#include <stdint.h>
typedef enum {
    INST_INT      = 0xCD,
    INST_HALT     = 0xF4,
    INST_NOP      = 0x90,
    INST_MOV      = 0x89,
    INST_ADD      = 0x01,
    INST_SUB      = 0x29,
    INST_MUL      = 0xF7,
    INST_DIV      = 0xF6,
    INST_PUSH     = 0x50,
    INST_POP      = 0x58,
    INST_CALL     = 0xE8,
    INST_RET      = 0xC3,
    INST_JMP      = 0xE9,
    INST_JZ       = 0x74,
    INST_JNZ      = 0x75,
    INST_CMP      = 0x39,
    INST_TEST     = 0xF6,
    INST_XOR      = 0x31,
    INST_AND      = 0x21,
    INST_OR       = 0x09,
    INST_LEA      = 0x8D,
    INST_SHL      = 0xD1,
    INST_SHR      = 0xD1,
    INST_SAR      = 0xD1,
    INST_PUSHF    = 0x9C,
    INST_POPF     = 0x9D,
    INST_IRET     = 0xCF,
    INST_MOVZX    = 0x0F, // Part of the MOVZX instruction
    INST_MOVSX    = 0x0F, // Part of the MOVSX instruction
} INSTRUCTIONS;

 int INST_READ(uint8_t* data);
