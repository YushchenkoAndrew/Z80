#pragma once

#include "include/Typelist.h"
#include "src/Defs.h"

namespace Bus {

#define W27C512_SIZE   65536
#define HY62256A_SIZE  32786
#define IMS1423_SIZE   4096 * 2
#define MC146818_SIZE  64
#define KM28C17_SIZE   2048

#define LCD_SEGMENT_SIZE 5

class Bus;


/**
 * This code was hardly expired by the next resource
 * Check out this link for more info: https://clrhome.org/table/
 * 
 * If any register written with lower case letter that means that instead of register
 * value it uses data that are located by the register ptr
 */
namespace Z80 {

enum Instruction {

//   0          1           2         3         4           5         6        7        8           9           A         B          C           D           E         F  
    NOP=0,   LD_BC_NN,   LD_bc_A,   INC_BC,   INC_B,      DEC_B,   LD_B_N,   RLCA,    EX_AF_AF,  ADD_HL_BC,   LD_A_bc,   DEC_BC,    INC_C,      DEC_C,      LD_C_N,   RRCA,
    DJNZ_D,  LD_DE_NN,   LD_de_A,   INC_DE,   INC_D,      DEC_D,   LD_D_N,   RLA,     JR_D,      ADD_HL_DE,   LD_A_de,   DEC_DE,    INC_E,      DEC_E,      LD_E_N,   RRA,
    JR_NZ_D, LD_HL_NN,   LD_nn_HL,  INC_HL,   INC_H,      DEC_H,   LD_H_N,   DAA,     JR_Z_D,    ADD_HL_HL,   LD_HL_nn,  DEC_HL,    INC_L,      DEC_L,      LD_L_N,   CPL,
    JR_NC_D, LD_SP_NN,   LD_nn_A,   INC_SP,   INC_hl,     DEC_hl,  LD_hl_n,  SCF,     JR_C_D,    ADD_HL_SP,   LD_A_nn,   DEC_SP,    INC_A,      DEC_A,      LD_A_N,   CCF,
    LD_B_B,  LD_B_C,     LD_B_D,    LD_B_E,   LD_B_H,     LD_B_L,  LD_B_hl,  LD_B_A,  LD_C_B,    LD_C_C,      LD_C_D,    LD_C_E,    LD_C_H,     LD_C_L,     LD_C_hl,  LD_C_A,
    LD_D_B,  LD_D_C,     LD_D_D,    LD_D_E,   LD_D_H,     LD_D_L,  LD_D_hl,  LD_D_A,  LD_E_B,    LD_E_C,      LD_E_D,    LD_E_E,    LD_E_H,     LD_E_L,     LD_E_hl,  LD_E_A,
    LD_H_B,  LD_H_C,     LD_H_D,    LD_H_E,   LD_H_H,     LD_H_L,  LD_H_hl,  LD_H_A,  LD_L_B,    LD_L_C,      LD_L_D,    LD_L_E,    LD_L_H,     LD_L_L,     LD_L_hl,  LD_L_A,
    LD_hl_B, LD_hl_C,    LD_hl_D,   LD_hl_E,  LD_hl_H,    LD_hl_L, HALT,     LD_hl_A, LD_A_B,    LD_A_C,      LD_A_D,    LD_A_E,    LD_A_H,     LD_A_L,     LD_A_hl,  LD_A_A,
    ADD_A_B, ADD_A_C,    ADD_A_D,   ADD_A_E,  ADD_A_H,    ADD_A_L, ADD_A_hl, ADD_A_A, ADC_A_B,   ADC_A_C,     ADC_A_D,   ADC_A_E,   ADC_A_H,    ADC_A_L,    ADC_A_hl, ADC_A_A,
    SUB_B,   SUB_C,      SUB_D,     SUB_E,    SUB_H,      SUB_L,   SUB_hl,   SUB_A,   SBC_A_B,   SBC_A_C,     SBC_A_D,   SBC_A_E,   SBC_A_H,    SBC_A_L,    SBC_A_hl, SBC_A_A,
    AND_B,   AND_C,      AND_D,     AND_E,    AND_H,      AND_L,   AND_hl,   AND_A,   XOR_B,     XOR_C,       XOR_D,     XOR_E,     XOR_H,      XOR_L,      XOR_hl,   XOR_A,
    OR_B,    OR_C,       OR_D,      OR_E,     OR_H,       OR_L,    OR_hl,    OR_A,    CP_B,      CP_C,        CP_D,      CP_E,      CP_H,       CP_L,       CP_hl,    CP_A,
    RET_NZ,  POP_BC,     JP_NZ_NN,  JP_NN,    CALL_NZ_NN, PUSH_BC, ADD_A_N,  RST_00h, RET_Z,     RET,         JP_Z_NN,   BIT_INSTR, CALL_Z_NN,  CALL_NN,    ADC_A_N,  RST_08h,
    RET_NC,  POP_DE,     JP_NC_NN,  OUT_n_A,  CALL_NC_NN, PUSH_DE, SUB_N,    RST_10h, RET_C,     EXX,         JP_C_NN,   IN_A_n,    CALL_C_NN,  IX_INSTR,   SBC_A_N,  RST_18h,
    RET_PO,  POP_HL,     JP_PO_NN,  EX_sp_HL, CALL_PO_NN, PUSH_HL, AND_N,    RST_20h, RET_PE,    JP_hl,       JP_PE_NN,  EX_DE_HL,  CALL_PE_NN, MISC_INSTR, XOR_N,    RST_28h,
    RET_P,   POP_AF,     JP_P_NN,   DI,       CALL_P_NN,  PUSH_AF, OR_N,     RST_30h, RET_M,     LD_SP_HL,    JP_M_NN,   EI,        CALL_M_NN,  IY_INSTR,   CP_N,     RST_38h,
};

/**
 * All of those instructions start with code 0xCB
 */
enum BitInstruction {

//   0          1         2         3          4         5          6         7         8         9         A         B         C         D         E          F  
    RLC_B=0,  RLC_C,    RLC_D,     RLC_E,    RLC_H,    RLC_L,    RLC_hl,    RLC_A,    RRC_B,    RRC_C,    RRC_D,    RRC_E,    RRC_H,    RRC_L,    RRC_hl,    RRC_A,
    RL_B,     RL_C,     RL_D,      RL_E,     RL_H,     RL_L,     RL_hl,     RL_A,     RR_B,     RR_C,     RR_D,     RR_E,     RR_H,     RR_L,     RR_hl,     RR_A,
    SLA_B,    SLA_C,    SLA_D,     SLA_E,    SLA_H,    SLA_L,    SLA_hl,    SLA_A,    SRA_B,    SRA_C,    SRA_D,    SRA_E,    SRA_H,    SRA_L,    SRA_hl,    SRA_A,
    SLL_B,    SLL_C,    SLL_D,     SLL_E,    SLL_H,    SLL_L,    SLL_hl,    SLL_A,    SRL_B,    SRL_C,    SRL_D,    SRL_E,    SRL_H,    SRL_L,    SRL_hl,    SRL_A,
    BIT_0_B,  BIT_0_C,  BIT_0_D,   BIT_0_E,  BIT_0_H,  BIT_0_L,  BIT_0_hl,  BIT_0_A,  BIT_1_B,  BIT_1_C,  BIT_1_D,  BIT_1_E,  BIT_1_H,  BIT_1_L,  BIT_1_hl,  BIT_1_A,
    BIT_2_B,  BIT_2_C,  BIT_2_D,   BIT_2_E,  BIT_2_H,  BIT_2_L,  BIT_2_hl,  BIT_2_A,  BIT_3_B,  BIT_3_C,  BIT_3_D,  BIT_3_E,  BIT_3_H,  BIT_3_L,  BIT_3_hl,  BIT_3_A,
    BIT_4_B,  BIT_4_C,  BIT_4_D,   BIT_4_E,  BIT_4_H,  BIT_4_L,  BIT_4_hl,  BIT_4_A,  BIT_5_B,  BIT_5_C,  BIT_5_D,  BIT_5_E,  BIT_5_H,  BIT_5_L,  BIT_5_hl,  BIT_5_A,
    BIT_6_B,  BIT_6_C,  BIT_6_D,   BIT_6_E,  BIT_6_H,  BIT_6_L,  BIT_6_hl,  BIT_6_A,  BIT_7_B,  BIT_7_C,  BIT_7_D,  BIT_7_E,  BIT_7_H,  BIT_7_L,  BIT_7_hl,  BIT_7_A,
    RES_0_B,  RES_0_C,  RES_0_D,   RES_0_E,  RES_0_H,  RES_0_L,  RES_0_hl,  RES_0_A,  RES_1_B,  RES_1_C,  RES_1_D,  RES_1_E,  RES_1_H,  RES_1_L,  RES_1_hl,  RES_1_A,
    RES_2_B,  RES_2_C,  RES_2_D,   RES_2_E,  RES_2_H,  RES_2_L,  RES_2_hl,  RES_2_A,  RES_3_B,  RES_3_C,  RES_3_D,  RES_3_E,  RES_3_H,  RES_3_L,  RES_3_hl,  RES_3_A,
    RES_4_B,  RES_4_C,  RES_4_D,   RES_4_E,  RES_4_H,  RES_4_L,  RES_4_hl,  RES_4_A,  RES_5_B,  RES_5_C,  RES_5_D,  RES_5_E,  RES_5_H,  RES_5_L,  RES_5_hl,  RES_5_A,
    RES_6_B,  RES_6_C,  RES_6_D,   RES_6_E,  RES_6_H,  RES_6_L,  RES_6_hl,  RES_6_A,  RES_7_B,  RES_7_C,  RES_7_D,  RES_7_E,  RES_7_H,  RES_7_L,  RES_7_hl,  RES_7_A,
    SET_0_B,  SET_0_C,  SET_0_D,   SET_0_E,  SET_0_H,  SET_0_L,  SET_0_hl,  SET_0_A,  SET_1_B,  SET_1_C,  SET_1_D,  SET_1_E,  SET_1_H,  SET_1_L,  SET_1_hl,  SET_1_A,
    SET_2_B,  SET_2_C,  SET_2_D,   SET_2_E,  SET_2_H,  SET_2_L,  SET_2_hl,  SET_2_A,  SET_3_B,  SET_3_C,  SET_3_D,  SET_3_E,  SET_3_H,  SET_3_L,  SET_3_hl,  SET_3_A,
    SET_4_B,  SET_4_C,  SET_4_D,   SET_4_E,  SET_4_H,  SET_4_L,  SET_4_hl,  SET_4_A,  SET_5_B,  SET_5_C,  SET_5_D,  SET_5_E,  SET_5_H,  SET_5_L,  SET_5_hl,  SET_5_A,
    SET_6_B,  SET_6_C,  SET_6_D,   SET_6_E,  SET_6_H,  SET_6_L,  SET_6_hl,  SET_6_A,  SET_7_B,  SET_7_C,  SET_7_D,  SET_7_E,  SET_7_H,  SET_7_L,  SET_7_hl,  SET_7_A,

};


/**
 * All of those instructions start with code 0xDD
 */
enum IxInstruction {
  ADD_IX_BC = 0x09, ADD_IX_DE = 0x19, LD_IX_NN = 0x21, LD_nn_IX = 0x22,  INC_IX = 0x23,   ADD_IX_IX = 0x29, LD_IX_nn = 0x2A,  DEC_IX = 0x2B, 
  INC_ixd = 0x34,   DEC_ixd = 0x35,   LD_ixd_N = 0x36, ADD_IX_SP = 0x39, LD_B_ixd = 0x46, LD_C_ixd = 0x4E,  LD_D_ixd = 0x56,  LD_E_ixd = 0x5E, LD_H_ixd = 0x66, LD_L_ixd = 0x6E,
  LD_ixd_B = 0x70,  LD_ixd_C = 0x71,  LD_ixd_D = 0x72, LD_ixd_E = 0x73,  LD_ixd_H = 0x74, LD_ixd_L = 0x75,  LD_ixd_A = 0x77,  LD_A_ixd = 0x7E, 
  ADD_A_ixd = 0x86, ADC_A_ixd = 0x8E, SUB_ixd = 0x96,  SBC_A_ixd = 0x9E, AND_ixd = 0xA6,  XOR_ixd = 0xAE,   OR_ixd = 0xB6,    CP_ixd = 0xBE,
  POP_IX = 0xE1,    EX_sp_IX = 0xE3,  PUSH_IX = 0xE5,  JP_ix = 0xE9,     LD_SP_IX = 0xF9, 

  IxBitInstr = Instruction::BIT_INSTR
};

/**
 * All of those instructions start with code 0xDDCB
 */
enum IxBitInstruction {
  RLC_ixd = 0x06,   RRC_ixd = 0x0E,   RL_ixd = 0x16,    RR_ixd = 0x1E,    SLA_ixd = 0x26,   SRA_ixd = 0x2E,   SLL_ixd = 0x36,   SRL_ixd = 0x3E,
  BIT_0_ixd = 0x46, BIT_1_ixd = 0x4E, BIT_2_ixd = 0x56, BIT_3_ixd = 0x5E, BIT_4_ixd = 0x66, BIT_5_ixd = 0x6E, BIT_6_ixd = 0x76, BIT_7_ixd = 0x7E,
  RES_0_ixd = 0x86, RES_1_ixd = 0x8E, RES_2_ixd = 0x96, RES_3_ixd = 0x9E, RES_4_ixd = 0xA6, RES_5_ixd = 0xAE, RES_6_ixd = 0xB6, RES_7_ixd = 0xBE,
  SET_0_ixd = 0xC6, SET_1_ixd = 0xCE, SET_2_ixd = 0xD6, SET_3_ixd = 0xDE, SET_4_ixd = 0xE6, SET_5_ixd = 0xEE, SET_6_ixd = 0xF6, SET_7_ixd = 0xFE,
};

/**
 * All of those instructions start with code 0xFD
 */
enum IyInstruction {
  ADD_IY_BC = 0x09, ADD_IY_DE = 0x19, LD_IY_NN = 0x21, LD_nn_IY = 0x22,  INC_IY = 0x23,   ADD_IY_IY = 0x29, LD_IY_nn = 0x2A,  DEC_IY = 0x2B, 
  INC_iyd = 0x34,   DEC_iyd = 0x35,   LD_iyd_N = 0x36, ADD_IY_SP = 0x39, LD_B_iyd = 0x46, LD_C_iyd = 0x4E,  LD_D_iyd = 0x56,  LD_E_iyd = 0x5E, LD_H_iyd = 0x66, LD_L_iyd = 0x6E,
  LD_iyd_B = 0x70,  LD_iyd_C = 0x71,  LD_iyd_D = 0x72, LD_iyd_E = 0x73,  LD_iyd_H = 0x74, LD_iyd_L = 0x75,  LD_iyd_A = 0x77,  LD_A_iyd = 0x7E, 
  ADD_A_iyd = 0x86, ADC_A_iyd = 0x8E, SUB_iyd = 0x96,  SBC_A_iyd = 0x9E, AND_iyd = 0xA6,  XOR_iyd = 0xAE,   OR_iyd = 0xB6,    CP_iyd = 0xBE,
  POP_IY = 0xE1,    EX_sp_IY = 0xE3,  PUSH_IY = 0xE5,  JP_iy = 0xE9,     LD_SP_IY = 0xF9, 

  IyBitInstr = Instruction::BIT_INSTR
};

/**
 * All of those instructions start with code 0xFDCB
 */
enum IyBitInstruction {
  RLC_iyd = 0x06,   RRC_iyd = 0x0E,   RL_iyd = 0x16,    RR_iyd = 0x1E,    SLA_iyd = 0x26,   SRA_iyd = 0x2E,   SLL_iyd = 0x36,   SRL_iyd = 0x3E,
  BIT_0_iyd = 0x46, BIT_1_iyd = 0x4E, BIT_2_iyd = 0x56, BIT_3_iyd = 0x5E, BIT_4_iyd = 0x66, BIT_5_iyd = 0x6E, BIT_6_iyd = 0x76, BIT_7_iyd = 0x7E,
  RES_0_iyd = 0x86, RES_1_iyd = 0x8E, RES_2_iyd = 0x96, RES_3_iyd = 0x9E, RES_4_iyd = 0xA6, RES_5_iyd = 0xAE, RES_6_iyd = 0xB6, RES_7_iyd = 0xBE,
  SET_0_iyd = 0xC6, SET_1_iyd = 0xCE, SET_2_iyd = 0xD6, SET_3_iyd = 0xDE, SET_4_iyd = 0xE6, SET_5_iyd = 0xEE, SET_6_iyd = 0xF6, SET_7_iyd = 0xFE,
};

/**
 * All of those instructions start with code 0xED
 */
enum MiscInstruction {
  IN_B_c = 0x40,    OUT_c_B = 0x41,  SBC_HL_BC = 0x42, LD_nn_BC = 0x43, NEG = 0x44,       RETN = 0x45,     IM_0 = 0x46,       LD_I_A = 0x47,  IN_C_c = 0x48,    OUT_c_C = 0x49,  ADC_HL_BC = 0x4A, LD_BC_nn = 0x4B, RETI = 0x4D, LD_R_A = 0x4F,
  IN_D_c = 0x50,    OUT_c_D = 0x51,  SBC_HL_DE = 0x52, LD_nn_DE = 0x53, IM_1 = 0x56,      LD_A_I = 0x57,   IN_E_c = 0x58,     OUT_c_E = 0x59, ADC_HL_DE = 0x5A, LD_DE_nn = 0x5B, IM_2 = 0x5E,      LD_A_R = 0x5F,
  IN_H_c = 0x60,    OUT_c_H = 0x61,  SBC_HL_HL = 0x62, RRD = 0x67,      IN_L_c = 0x68,    OUT_c_L = 0x69,  ADC_HL_HL = 0x6A, RLD = 0x6F,
  SBC_HL_SP = 0x72, LD_nn_SP = 0x73, IN_A_c = 0x78,    OUT_c_A = 0x79,  ADC_HL_SP = 0x7A, LD_SP_nn = 0x7B,

  LDI = 0xA0,  CPI = 0xA1,  INI = 0xA2,  OUTI = 0xA3,   LDD = 0xA8,  CPD = 0xA9,  IND = 0xAA,  OUTD = 0xAB,
  LDIR = 0xB0, CPIR = 0xB1, INIR = 0xB2, OTIR = 0xB3,   LDDR = 0xB8, CPDR = 0xB9, INDR = 0xBA, OTDR = 0xBB,
};

// template <class T>
// using WindowT = std::tuple<bool, std::shared_ptr<T>, DimensionT, int32_t>;

// typedef TypeList<Instruction, std::string> InstrSTR;
// typedef TypeList<BitInstruction, std::string> BitSTR;
// typedef TypeList<IxInstruction, std::string> IxSTR;
// typedef TypeList<IyInstruction, std::string> IySTR;
// typedef TypeList<MiscInstruction, std::string> MiscSTR;
// typedef TypeList<IxBitInstruction, std::string> IxBitSTR;
// typedef TypeList<IyBitInstruction, std::string> IyBitSTR;

typedef TypeList<
  Int2Type<Instruction::NOP>,        TypeList<Int2Type<Instruction::LD_BC_NN>,  TypeList<
  Int2Type<Instruction::DJNZ_D>,     TypeList<Int2Type<Instruction::LD_DE_NN>,  TypeList<
  Int2Type<Instruction::JR_NZ_D>,    TypeList<Int2Type<Instruction::LD_HL_NN>,  TypeList<
  Int2Type<Instruction::JR_NC_D>,    TypeList<Int2Type<Instruction::LD_SP_NN>,  TypeList<
  Int2Type<Instruction::LD_B_B>,     TypeList<Int2Type<Instruction::LD_B_C>,    TypeList<
  Int2Type<Instruction::LD_D_B>,     TypeList<Int2Type<Instruction::LD_D_C>,    TypeList<
  Int2Type<Instruction::LD_H_B>,     TypeList<Int2Type<Instruction::LD_H_C>,    TypeList<
  Int2Type<Instruction::LD_hl_B>,    TypeList<Int2Type<Instruction::LD_hl_C>,   TypeList<
  Int2Type<Instruction::ADD_A_B>,    TypeList<Int2Type<Instruction::ADD_A_C>,   TypeList<
  Int2Type<Instruction::SUB_B>,      TypeList<Int2Type<Instruction::SUB_C>,     TypeList<
  Int2Type<Instruction::AND_B>,      TypeList<Int2Type<Instruction::AND_C>,     TypeList<
  Int2Type<Instruction::OR_B>,       TypeList<Int2Type<Instruction::OR_C>,      TypeList<
  Int2Type<Instruction::RET_NZ>,     TypeList<Int2Type<Instruction::POP_BC>,    TypeList<
  Int2Type<Instruction::RET_NC>,     TypeList<Int2Type<Instruction::POP_DE>,    TypeList<
  Int2Type<Instruction::RET_PO>,     TypeList<Int2Type<Instruction::POP_HL>,    TypeList<
  Int2Type<Instruction::RET_P>,      TypeList<Int2Type<Instruction::POP_AF>,    TypeList<
  Int2Type<Instruction::LD_bc_A>,    TypeList<Int2Type<Instruction::INC_BC>,    TypeList<
  Int2Type<Instruction::LD_de_A>,    TypeList<Int2Type<Instruction::INC_DE>,    TypeList<
  Int2Type<Instruction::LD_nn_HL>,   TypeList<Int2Type<Instruction::INC_HL>,    TypeList<
  Int2Type<Instruction::LD_nn_A>,    TypeList<Int2Type<Instruction::INC_SP>,    TypeList<
  Int2Type<Instruction::LD_B_D>,     TypeList<Int2Type<Instruction::LD_B_E>,    TypeList<
  Int2Type<Instruction::LD_D_D>,     TypeList<Int2Type<Instruction::LD_D_E>,    TypeList<
  Int2Type<Instruction::LD_H_D>,     TypeList<Int2Type<Instruction::LD_H_E>,    TypeList<
  Int2Type<Instruction::LD_hl_D>,    TypeList<Int2Type<Instruction::LD_hl_E>,   TypeList<
  Int2Type<Instruction::ADD_A_D>,    TypeList<Int2Type<Instruction::ADD_A_E>,   TypeList<
  Int2Type<Instruction::SUB_D>,      TypeList<Int2Type<Instruction::SUB_E>,     TypeList<
  Int2Type<Instruction::AND_D>,      TypeList<Int2Type<Instruction::AND_E>,     TypeList<
  Int2Type<Instruction::OR_D>,       TypeList<Int2Type<Instruction::OR_E>,      TypeList<
  Int2Type<Instruction::JP_NZ_NN>,   TypeList<Int2Type<Instruction::JP_NN>,     TypeList<
  Int2Type<Instruction::JP_NC_NN>,   TypeList<Int2Type<Instruction::OUT_n_A>,   TypeList<
  Int2Type<Instruction::JP_PO_NN>,   TypeList<Int2Type<Instruction::EX_sp_HL>,  TypeList<
  Int2Type<Instruction::JP_P_NN>,    TypeList<Int2Type<Instruction::DI>,        TypeList<
  Int2Type<Instruction::INC_B>,      TypeList<Int2Type<Instruction::DEC_B>,     TypeList<
  Int2Type<Instruction::INC_D>,      TypeList<Int2Type<Instruction::DEC_D>,     TypeList<
  Int2Type<Instruction::INC_H>,      TypeList<Int2Type<Instruction::DEC_H>,     TypeList<
  Int2Type<Instruction::INC_hl>,     TypeList<Int2Type<Instruction::DEC_hl>,    TypeList<
  Int2Type<Instruction::LD_B_H>,     TypeList<Int2Type<Instruction::LD_B_L>,    TypeList<
  Int2Type<Instruction::LD_D_H>,     TypeList<Int2Type<Instruction::LD_D_L>,    TypeList<
  Int2Type<Instruction::LD_H_H>,     TypeList<Int2Type<Instruction::LD_H_L>,    TypeList<
  Int2Type<Instruction::LD_hl_H>,    TypeList<Int2Type<Instruction::LD_hl_L>,   TypeList<
  Int2Type<Instruction::ADD_A_H>,    TypeList<Int2Type<Instruction::ADD_A_L>,   TypeList<
  Int2Type<Instruction::SUB_H>,      TypeList<Int2Type<Instruction::SUB_L>,     TypeList<
  Int2Type<Instruction::AND_H>,      TypeList<Int2Type<Instruction::AND_L>,     TypeList<
  Int2Type<Instruction::OR_H>,       TypeList<Int2Type<Instruction::OR_L>,      TypeList<
  Int2Type<Instruction::CALL_NZ_NN>, TypeList<Int2Type<Instruction::PUSH_BC>,   TypeList<
  Int2Type<Instruction::CALL_NC_NN>, TypeList<Int2Type<Instruction::PUSH_DE>,   TypeList<
  Int2Type<Instruction::CALL_PO_NN>, TypeList<Int2Type<Instruction::PUSH_HL>,   TypeList<
  Int2Type<Instruction::CALL_P_NN>,  TypeList<Int2Type<Instruction::PUSH_AF>,   TypeList<
  Int2Type<Instruction::LD_B_N>,     TypeList<Int2Type<Instruction::RLCA>,      TypeList<
  Int2Type<Instruction::LD_D_N>,     TypeList<Int2Type<Instruction::RLA>,       TypeList<
  Int2Type<Instruction::LD_H_N>,     TypeList<Int2Type<Instruction::DAA>,       TypeList<
  Int2Type<Instruction::LD_hl_n>,    TypeList<Int2Type<Instruction::SCF>,       TypeList<
  Int2Type<Instruction::LD_B_hl>,    TypeList<Int2Type<Instruction::LD_B_A>,    TypeList<
  Int2Type<Instruction::LD_D_hl>,    TypeList<Int2Type<Instruction::LD_D_A>,    TypeList<
  Int2Type<Instruction::LD_H_hl>,    TypeList<Int2Type<Instruction::LD_H_A>,    TypeList<
  Int2Type<Instruction::HALT>,       TypeList<Int2Type<Instruction::LD_hl_A>,   TypeList<
  Int2Type<Instruction::ADD_A_hl>,   TypeList<Int2Type<Instruction::ADD_A_A>,   TypeList<
  Int2Type<Instruction::SUB_hl>,     TypeList<Int2Type<Instruction::SUB_A>,     TypeList<
  Int2Type<Instruction::AND_hl>,     TypeList<Int2Type<Instruction::AND_A>,     TypeList<
  Int2Type<Instruction::OR_hl>,      TypeList<Int2Type<Instruction::OR_A>,      TypeList<
  Int2Type<Instruction::ADD_A_N>,    TypeList<Int2Type<Instruction::RST_00h>,   TypeList<
  Int2Type<Instruction::SUB_N>,      TypeList<Int2Type<Instruction::RST_10h>,   TypeList<
  Int2Type<Instruction::AND_N>,      TypeList<Int2Type<Instruction::RST_20h>,   TypeList<
  Int2Type<Instruction::OR_N>,       TypeList<Int2Type<Instruction::RST_30h>,   TypeList<
  Int2Type<Instruction::EX_AF_AF>,   TypeList<Int2Type<Instruction::ADD_HL_BC>, TypeList<
  Int2Type<Instruction::JR_D>,       TypeList<Int2Type<Instruction::ADD_HL_DE>, TypeList<
  Int2Type<Instruction::JR_Z_D>,     TypeList<Int2Type<Instruction::ADD_HL_HL>, TypeList<
  Int2Type<Instruction::JR_C_D>,     TypeList<Int2Type<Instruction::ADD_HL_SP>, TypeList<
  Int2Type<Instruction::LD_C_B>,     TypeList<Int2Type<Instruction::LD_C_C>,    TypeList<
  Int2Type<Instruction::LD_E_B>,     TypeList<Int2Type<Instruction::LD_E_C>,    TypeList<
  Int2Type<Instruction::LD_L_B>,     TypeList<Int2Type<Instruction::LD_L_C>,    TypeList<
  Int2Type<Instruction::LD_A_B>,     TypeList<Int2Type<Instruction::LD_A_C>,    TypeList<
  Int2Type<Instruction::ADC_A_B>,    TypeList<Int2Type<Instruction::ADC_A_C>,   TypeList<
  Int2Type<Instruction::SBC_A_B>,    TypeList<Int2Type<Instruction::SBC_A_C>,   TypeList<
  Int2Type<Instruction::XOR_B>,      TypeList<Int2Type<Instruction::XOR_C>,     TypeList<
  Int2Type<Instruction::CP_B>,       TypeList<Int2Type<Instruction::CP_C>,      TypeList<
  Int2Type<Instruction::RET_Z>,      TypeList<Int2Type<Instruction::RET>,       TypeList<
  Int2Type<Instruction::RET_C>,      TypeList<Int2Type<Instruction::EXX>,       TypeList<
  Int2Type<Instruction::RET_PE>,     TypeList<Int2Type<Instruction::JP_hl>,     TypeList<
  Int2Type<Instruction::RET_M>,      TypeList<Int2Type<Instruction::LD_SP_HL>,  TypeList<
  Int2Type<Instruction::LD_A_bc>,    TypeList<Int2Type<Instruction::DEC_BC>,    TypeList<
  Int2Type<Instruction::LD_A_de>,    TypeList<Int2Type<Instruction::DEC_DE>,    TypeList<
  Int2Type<Instruction::LD_HL_nn>,   TypeList<Int2Type<Instruction::DEC_HL>,    TypeList<
  Int2Type<Instruction::LD_A_nn>,    TypeList<Int2Type<Instruction::DEC_SP>,    TypeList<
  Int2Type<Instruction::LD_C_D>,     TypeList<Int2Type<Instruction::LD_C_E>,    TypeList<
  Int2Type<Instruction::LD_E_D>,     TypeList<Int2Type<Instruction::LD_E_E>,    TypeList<
  Int2Type<Instruction::LD_L_D>,     TypeList<Int2Type<Instruction::LD_L_E>,    TypeList<
  Int2Type<Instruction::LD_A_D>,     TypeList<Int2Type<Instruction::LD_A_E>,    TypeList<
  Int2Type<Instruction::ADC_A_D>,    TypeList<Int2Type<Instruction::ADC_A_E>,   TypeList<
  Int2Type<Instruction::SBC_A_D>,    TypeList<Int2Type<Instruction::SBC_A_E>,   TypeList<
  Int2Type<Instruction::XOR_D>,      TypeList<Int2Type<Instruction::XOR_E>,     TypeList<
  Int2Type<Instruction::CP_D>,       TypeList<Int2Type<Instruction::CP_E>,      TypeList<
  Int2Type<Instruction::JP_Z_NN>,    TypeList<Int2Type<Instruction::BIT_INSTR>, TypeList<
  Int2Type<Instruction::JP_C_NN>,    TypeList<Int2Type<Instruction::IN_A_n>,    TypeList<
  Int2Type<Instruction::JP_PE_NN>,   TypeList<Int2Type<Instruction::EX_DE_HL>,  TypeList<
  Int2Type<Instruction::JP_M_NN>,    TypeList<Int2Type<Instruction::EI>,        TypeList<
  Int2Type<Instruction::INC_C>,      TypeList<Int2Type<Instruction::DEC_C>,     TypeList<
  Int2Type<Instruction::INC_E>,      TypeList<Int2Type<Instruction::DEC_E>,     TypeList<
  Int2Type<Instruction::INC_L>,      TypeList<Int2Type<Instruction::DEC_L>,     TypeList<
  Int2Type<Instruction::INC_A>,      TypeList<Int2Type<Instruction::DEC_A>,     TypeList<
  Int2Type<Instruction::LD_C_H>,     TypeList<Int2Type<Instruction::LD_C_L>,    TypeList<
  Int2Type<Instruction::LD_E_H>,     TypeList<Int2Type<Instruction::LD_E_L>,    TypeList<
  Int2Type<Instruction::LD_L_H>,     TypeList<Int2Type<Instruction::LD_L_L>,    TypeList<
  Int2Type<Instruction::LD_A_H>,     TypeList<Int2Type<Instruction::LD_A_L>,    TypeList<
  Int2Type<Instruction::ADC_A_H>,    TypeList<Int2Type<Instruction::ADC_A_L>,   TypeList<
  Int2Type<Instruction::SBC_A_H>,    TypeList<Int2Type<Instruction::SBC_A_L>,   TypeList<
  Int2Type<Instruction::XOR_H>,      TypeList<Int2Type<Instruction::XOR_L>,     TypeList<
  Int2Type<Instruction::CP_H>,       TypeList<Int2Type<Instruction::CP_L>,      TypeList<
  Int2Type<Instruction::CALL_Z_NN>,  TypeList<Int2Type<Instruction::CALL_NN>,   TypeList<
  Int2Type<Instruction::CALL_C_NN>,  TypeList<Int2Type<Instruction::IX_INSTR>,  TypeList<
  Int2Type<Instruction::CALL_PE_NN>, TypeList<Int2Type<Instruction::MISC_INSTR>,TypeList<
  Int2Type<Instruction::CALL_M_NN>,  TypeList<Int2Type<Instruction::IY_INSTR>,  TypeList<
  Int2Type<Instruction::LD_C_N>,     TypeList<Int2Type<Instruction::RRCA>,      TypeList<
  Int2Type<Instruction::LD_E_N>,     TypeList<Int2Type<Instruction::RRA>,       TypeList<
  Int2Type<Instruction::LD_L_N>,     TypeList<Int2Type<Instruction::CPL>,       TypeList<
  Int2Type<Instruction::LD_A_N>,     TypeList<Int2Type<Instruction::CCF>,       TypeList<
  Int2Type<Instruction::LD_C_hl>,    TypeList<Int2Type<Instruction::LD_C_A>,    TypeList<
  Int2Type<Instruction::LD_E_hl>,    TypeList<Int2Type<Instruction::LD_E_A>,    TypeList<
  Int2Type<Instruction::LD_L_hl>,    TypeList<Int2Type<Instruction::LD_L_A>,    TypeList<
  Int2Type<Instruction::LD_A_hl>,    TypeList<Int2Type<Instruction::LD_A_A>,    TypeList<
  Int2Type<Instruction::ADC_A_hl>,   TypeList<Int2Type<Instruction::ADC_A_A>,   TypeList<
  Int2Type<Instruction::SBC_A_hl>,   TypeList<Int2Type<Instruction::SBC_A_A>,   TypeList<
  Int2Type<Instruction::XOR_hl>,     TypeList<Int2Type<Instruction::XOR_A>,     TypeList<
  Int2Type<Instruction::CP_hl>,      TypeList<Int2Type<Instruction::CP_A>,      TypeList<
  Int2Type<Instruction::ADC_A_N>,    TypeList<Int2Type<Instruction::RST_08h>,   TypeList<
  Int2Type<Instruction::SBC_A_N>,    TypeList<Int2Type<Instruction::RST_18h>,   TypeList<
  Int2Type<Instruction::XOR_N>,      TypeList<Int2Type<Instruction::RST_28h>,   TypeList<
  Int2Type<Instruction::CP_N>,       TypeList<Int2Type<Instruction::RST_38h>,   NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    Instructions;


typedef TypeList<
  Int2Type<BitInstruction::RLC_B>,    TypeList<Int2Type<BitInstruction::RLC_C>,   TypeList<
  Int2Type<BitInstruction::RL_B>,     TypeList<Int2Type<BitInstruction::RL_C>,    TypeList<
  Int2Type<BitInstruction::SLA_B>,    TypeList<Int2Type<BitInstruction::SLA_C>,   TypeList<
  Int2Type<BitInstruction::SLL_B>,    TypeList<Int2Type<BitInstruction::SLL_C>,   TypeList<
  Int2Type<BitInstruction::BIT_0_B>,  TypeList<Int2Type<BitInstruction::BIT_0_C>, TypeList<
  Int2Type<BitInstruction::BIT_2_B>,  TypeList<Int2Type<BitInstruction::BIT_2_C>, TypeList<
  Int2Type<BitInstruction::BIT_4_B>,  TypeList<Int2Type<BitInstruction::BIT_4_C>, TypeList<
  Int2Type<BitInstruction::BIT_6_B>,  TypeList<Int2Type<BitInstruction::BIT_6_C>, TypeList<
  Int2Type<BitInstruction::RES_0_B>,  TypeList<Int2Type<BitInstruction::RES_0_C>, TypeList<
  Int2Type<BitInstruction::RES_2_B>,  TypeList<Int2Type<BitInstruction::RES_2_C>, TypeList<
  Int2Type<BitInstruction::RES_4_B>,  TypeList<Int2Type<BitInstruction::RES_4_C>, TypeList<
  Int2Type<BitInstruction::RES_6_B>,  TypeList<Int2Type<BitInstruction::RES_6_C>, TypeList<
  Int2Type<BitInstruction::SET_0_B>,  TypeList<Int2Type<BitInstruction::SET_0_C>, TypeList<
  Int2Type<BitInstruction::SET_2_B>,  TypeList<Int2Type<BitInstruction::SET_2_C>, TypeList<
  Int2Type<BitInstruction::SET_4_B>,  TypeList<Int2Type<BitInstruction::SET_4_C>, TypeList<
  Int2Type<BitInstruction::SET_6_B>,  TypeList<Int2Type<BitInstruction::SET_6_C>, TypeList<
  Int2Type<BitInstruction::RLC_D>,    TypeList<Int2Type<BitInstruction::RLC_E>,   TypeList<
  Int2Type<BitInstruction::RL_D>,     TypeList<Int2Type<BitInstruction::RL_E>,    TypeList<
  Int2Type<BitInstruction::SLA_D>,    TypeList<Int2Type<BitInstruction::SLA_E>,   TypeList<
  Int2Type<BitInstruction::SLL_D>,    TypeList<Int2Type<BitInstruction::SLL_E>,   TypeList<
  Int2Type<BitInstruction::BIT_0_D>,  TypeList<Int2Type<BitInstruction::BIT_0_E>, TypeList<
  Int2Type<BitInstruction::BIT_2_D>,  TypeList<Int2Type<BitInstruction::BIT_2_E>, TypeList<
  Int2Type<BitInstruction::BIT_4_D>,  TypeList<Int2Type<BitInstruction::BIT_4_E>, TypeList<
  Int2Type<BitInstruction::BIT_6_D>,  TypeList<Int2Type<BitInstruction::BIT_6_E>, TypeList<
  Int2Type<BitInstruction::RES_0_D>,  TypeList<Int2Type<BitInstruction::RES_0_E>, TypeList<
  Int2Type<BitInstruction::RES_2_D>,  TypeList<Int2Type<BitInstruction::RES_2_E>, TypeList<
  Int2Type<BitInstruction::RES_4_D>,  TypeList<Int2Type<BitInstruction::RES_4_E>, TypeList<
  Int2Type<BitInstruction::RES_6_D>,  TypeList<Int2Type<BitInstruction::RES_6_E>, TypeList<
  Int2Type<BitInstruction::SET_0_D>,  TypeList<Int2Type<BitInstruction::SET_0_E>, TypeList<
  Int2Type<BitInstruction::SET_2_D>,  TypeList<Int2Type<BitInstruction::SET_2_E>, TypeList<
  Int2Type<BitInstruction::SET_4_D>,  TypeList<Int2Type<BitInstruction::SET_4_E>, TypeList<
  Int2Type<BitInstruction::SET_6_D>,  TypeList<Int2Type<BitInstruction::SET_6_E>, TypeList<
  Int2Type<BitInstruction::RLC_H>,    TypeList<Int2Type<BitInstruction::RLC_L>,   TypeList<
  Int2Type<BitInstruction::RL_H>,     TypeList<Int2Type<BitInstruction::RL_L>,    TypeList<
  Int2Type<BitInstruction::SLA_H>,    TypeList<Int2Type<BitInstruction::SLA_L>,   TypeList<
  Int2Type<BitInstruction::SLL_H>,    TypeList<Int2Type<BitInstruction::SLL_L>,   TypeList<
  Int2Type<BitInstruction::BIT_0_H>,  TypeList<Int2Type<BitInstruction::BIT_0_L>, TypeList<
  Int2Type<BitInstruction::BIT_2_H>,  TypeList<Int2Type<BitInstruction::BIT_2_L>, TypeList<
  Int2Type<BitInstruction::BIT_4_H>,  TypeList<Int2Type<BitInstruction::BIT_4_L>, TypeList<
  Int2Type<BitInstruction::BIT_6_H>,  TypeList<Int2Type<BitInstruction::BIT_6_L>, TypeList<
  Int2Type<BitInstruction::RES_0_H>,  TypeList<Int2Type<BitInstruction::RES_0_L>, TypeList<
  Int2Type<BitInstruction::RES_2_H>,  TypeList<Int2Type<BitInstruction::RES_2_L>, TypeList<
  Int2Type<BitInstruction::RES_4_H>,  TypeList<Int2Type<BitInstruction::RES_4_L>, TypeList<
  Int2Type<BitInstruction::RES_6_H>,  TypeList<Int2Type<BitInstruction::RES_6_L>, TypeList<
  Int2Type<BitInstruction::SET_0_H>,  TypeList<Int2Type<BitInstruction::SET_0_L>, TypeList<
  Int2Type<BitInstruction::SET_2_H>,  TypeList<Int2Type<BitInstruction::SET_2_L>, TypeList<
  Int2Type<BitInstruction::SET_4_H>,  TypeList<Int2Type<BitInstruction::SET_4_L>, TypeList<
  Int2Type<BitInstruction::SET_6_H>,  TypeList<Int2Type<BitInstruction::SET_6_L>, TypeList<
  Int2Type<BitInstruction::RLC_hl>,   TypeList<Int2Type<BitInstruction::RLC_A>,   TypeList<
  Int2Type<BitInstruction::RL_hl>,    TypeList<Int2Type<BitInstruction::RL_A>,    TypeList<
  Int2Type<BitInstruction::SLA_hl>,   TypeList<Int2Type<BitInstruction::SLA_A>,   TypeList<
  Int2Type<BitInstruction::SLL_hl>,   TypeList<Int2Type<BitInstruction::SLL_A>,   TypeList<
  Int2Type<BitInstruction::BIT_0_hl>, TypeList<Int2Type<BitInstruction::BIT_0_A>, TypeList<
  Int2Type<BitInstruction::BIT_2_hl>, TypeList<Int2Type<BitInstruction::BIT_2_A>, TypeList<
  Int2Type<BitInstruction::BIT_4_hl>, TypeList<Int2Type<BitInstruction::BIT_4_A>, TypeList<
  Int2Type<BitInstruction::BIT_6_hl>, TypeList<Int2Type<BitInstruction::BIT_6_A>, TypeList<
  Int2Type<BitInstruction::RES_0_hl>, TypeList<Int2Type<BitInstruction::RES_0_A>, TypeList<
  Int2Type<BitInstruction::RES_2_hl>, TypeList<Int2Type<BitInstruction::RES_2_A>, TypeList<
  Int2Type<BitInstruction::RES_4_hl>, TypeList<Int2Type<BitInstruction::RES_4_A>, TypeList<
  Int2Type<BitInstruction::RES_6_hl>, TypeList<Int2Type<BitInstruction::RES_6_A>, TypeList<
  Int2Type<BitInstruction::SET_0_hl>, TypeList<Int2Type<BitInstruction::SET_0_A>, TypeList<
  Int2Type<BitInstruction::SET_2_hl>, TypeList<Int2Type<BitInstruction::SET_2_A>, TypeList<
  Int2Type<BitInstruction::SET_4_hl>, TypeList<Int2Type<BitInstruction::SET_4_A>, TypeList<
  Int2Type<BitInstruction::SET_6_hl>, TypeList<Int2Type<BitInstruction::SET_6_A>, TypeList<
  Int2Type<BitInstruction::RRC_B>,    TypeList<Int2Type<BitInstruction::RRC_C>,   TypeList<
  Int2Type<BitInstruction::RR_B>,     TypeList<Int2Type<BitInstruction::RR_C>,    TypeList<
  Int2Type<BitInstruction::SRA_B>,    TypeList<Int2Type<BitInstruction::SRA_C>,   TypeList<
  Int2Type<BitInstruction::SRL_B>,    TypeList<Int2Type<BitInstruction::SRL_C>,   TypeList<
  Int2Type<BitInstruction::BIT_1_B>,  TypeList<Int2Type<BitInstruction::BIT_1_C>, TypeList<
  Int2Type<BitInstruction::BIT_3_B>,  TypeList<Int2Type<BitInstruction::BIT_3_C>, TypeList<
  Int2Type<BitInstruction::BIT_5_B>,  TypeList<Int2Type<BitInstruction::BIT_5_C>, TypeList<
  Int2Type<BitInstruction::BIT_7_B>,  TypeList<Int2Type<BitInstruction::BIT_7_C>, TypeList<
  Int2Type<BitInstruction::RES_1_B>,  TypeList<Int2Type<BitInstruction::RES_1_C>, TypeList<
  Int2Type<BitInstruction::RES_3_B>,  TypeList<Int2Type<BitInstruction::RES_3_C>, TypeList<
  Int2Type<BitInstruction::RES_5_B>,  TypeList<Int2Type<BitInstruction::RES_5_C>, TypeList<
  Int2Type<BitInstruction::RES_7_B>,  TypeList<Int2Type<BitInstruction::RES_7_C>, TypeList<
  Int2Type<BitInstruction::SET_1_B>,  TypeList<Int2Type<BitInstruction::SET_1_C>, TypeList<
  Int2Type<BitInstruction::SET_3_B>,  TypeList<Int2Type<BitInstruction::SET_3_C>, TypeList<
  Int2Type<BitInstruction::SET_5_B>,  TypeList<Int2Type<BitInstruction::SET_5_C>, TypeList<
  Int2Type<BitInstruction::SET_7_B>,  TypeList<Int2Type<BitInstruction::SET_7_C>, TypeList<
  Int2Type<BitInstruction::RRC_D>,    TypeList<Int2Type<BitInstruction::RRC_E>,   TypeList<
  Int2Type<BitInstruction::RR_D>,     TypeList<Int2Type<BitInstruction::RR_E>,    TypeList<
  Int2Type<BitInstruction::SRA_D>,    TypeList<Int2Type<BitInstruction::SRA_E>,   TypeList<
  Int2Type<BitInstruction::SRL_D>,    TypeList<Int2Type<BitInstruction::SRL_E>,   TypeList<
  Int2Type<BitInstruction::BIT_1_D>,  TypeList<Int2Type<BitInstruction::BIT_1_E>, TypeList<
  Int2Type<BitInstruction::BIT_3_D>,  TypeList<Int2Type<BitInstruction::BIT_3_E>, TypeList<
  Int2Type<BitInstruction::BIT_5_D>,  TypeList<Int2Type<BitInstruction::BIT_5_E>, TypeList<
  Int2Type<BitInstruction::BIT_7_D>,  TypeList<Int2Type<BitInstruction::BIT_7_E>, TypeList<
  Int2Type<BitInstruction::RES_1_D>,  TypeList<Int2Type<BitInstruction::RES_1_E>, TypeList<
  Int2Type<BitInstruction::RES_3_D>,  TypeList<Int2Type<BitInstruction::RES_3_E>, TypeList<
  Int2Type<BitInstruction::RES_5_D>,  TypeList<Int2Type<BitInstruction::RES_5_E>, TypeList<
  Int2Type<BitInstruction::RES_7_D>,  TypeList<Int2Type<BitInstruction::RES_7_E>, TypeList<
  Int2Type<BitInstruction::SET_1_D>,  TypeList<Int2Type<BitInstruction::SET_1_E>, TypeList<
  Int2Type<BitInstruction::SET_3_D>,  TypeList<Int2Type<BitInstruction::SET_3_E>, TypeList<
  Int2Type<BitInstruction::SET_5_D>,  TypeList<Int2Type<BitInstruction::SET_5_E>, TypeList<
  Int2Type<BitInstruction::SET_7_D>,  TypeList<Int2Type<BitInstruction::SET_7_E>, TypeList<
  Int2Type<BitInstruction::RRC_H>,    TypeList<Int2Type<BitInstruction::RRC_L>,   TypeList<
  Int2Type<BitInstruction::RR_H>,     TypeList<Int2Type<BitInstruction::RR_L>,    TypeList<
  Int2Type<BitInstruction::SRA_H>,    TypeList<Int2Type<BitInstruction::SRA_L>,   TypeList<
  Int2Type<BitInstruction::SRL_H>,    TypeList<Int2Type<BitInstruction::SRL_L>,   TypeList<
  Int2Type<BitInstruction::BIT_1_H>,  TypeList<Int2Type<BitInstruction::BIT_1_L>, TypeList<
  Int2Type<BitInstruction::BIT_3_H>,  TypeList<Int2Type<BitInstruction::BIT_3_L>, TypeList<
  Int2Type<BitInstruction::BIT_5_H>,  TypeList<Int2Type<BitInstruction::BIT_5_L>, TypeList<
  Int2Type<BitInstruction::BIT_7_H>,  TypeList<Int2Type<BitInstruction::BIT_7_L>, TypeList<
  Int2Type<BitInstruction::RES_1_H>,  TypeList<Int2Type<BitInstruction::RES_1_L>, TypeList<
  Int2Type<BitInstruction::RES_3_H>,  TypeList<Int2Type<BitInstruction::RES_3_L>, TypeList<
  Int2Type<BitInstruction::RES_5_H>,  TypeList<Int2Type<BitInstruction::RES_5_L>, TypeList<
  Int2Type<BitInstruction::RES_7_H>,  TypeList<Int2Type<BitInstruction::RES_7_L>, TypeList<
  Int2Type<BitInstruction::SET_1_H>,  TypeList<Int2Type<BitInstruction::SET_1_L>, TypeList<
  Int2Type<BitInstruction::SET_3_H>,  TypeList<Int2Type<BitInstruction::SET_3_L>, TypeList<
  Int2Type<BitInstruction::SET_5_H>,  TypeList<Int2Type<BitInstruction::SET_5_L>, TypeList<
  Int2Type<BitInstruction::SET_7_H>,  TypeList<Int2Type<BitInstruction::SET_7_L>, TypeList<
  Int2Type<BitInstruction::RRC_hl>,   TypeList<Int2Type<BitInstruction::RRC_A>,   TypeList<
  Int2Type<BitInstruction::RR_hl>,    TypeList<Int2Type<BitInstruction::RR_A>,    TypeList<
  Int2Type<BitInstruction::SRA_hl>,   TypeList<Int2Type<BitInstruction::SRA_A>,   TypeList<
  Int2Type<BitInstruction::SRL_hl>,   TypeList<Int2Type<BitInstruction::SRL_A>,   TypeList<
  Int2Type<BitInstruction::BIT_1_hl>, TypeList<Int2Type<BitInstruction::BIT_1_A>, TypeList<
  Int2Type<BitInstruction::BIT_3_hl>, TypeList<Int2Type<BitInstruction::BIT_3_A>, TypeList<
  Int2Type<BitInstruction::BIT_5_hl>, TypeList<Int2Type<BitInstruction::BIT_5_A>, TypeList<
  Int2Type<BitInstruction::BIT_7_hl>, TypeList<Int2Type<BitInstruction::BIT_7_A>, TypeList<
  Int2Type<BitInstruction::RES_1_hl>, TypeList<Int2Type<BitInstruction::RES_1_A>, TypeList<
  Int2Type<BitInstruction::RES_3_hl>, TypeList<Int2Type<BitInstruction::RES_3_A>, TypeList<
  Int2Type<BitInstruction::RES_5_hl>, TypeList<Int2Type<BitInstruction::RES_5_A>, TypeList<
  Int2Type<BitInstruction::RES_7_hl>, TypeList<Int2Type<BitInstruction::RES_7_A>, TypeList<
  Int2Type<BitInstruction::SET_1_hl>, TypeList<Int2Type<BitInstruction::SET_1_A>, TypeList<
  Int2Type<BitInstruction::SET_3_hl>, TypeList<Int2Type<BitInstruction::SET_3_A>, TypeList<
  Int2Type<BitInstruction::SET_5_hl>, TypeList<Int2Type<BitInstruction::SET_5_A>, TypeList<
  Int2Type<BitInstruction::SET_7_hl>, TypeList<Int2Type<BitInstruction::SET_7_A>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    BitInstructions;

typedef TypeList<
  Int2Type<IxInstruction::ADD_IX_BC>, TypeList<Int2Type<IxInstruction::ADD_IX_DE>, TypeList<
  Int2Type<IxInstruction::INC_ixd>,   TypeList<Int2Type<IxInstruction::DEC_ixd>,   TypeList<
  Int2Type<IxInstruction::LD_ixd_B>,  TypeList<Int2Type<IxInstruction::LD_ixd_C>,  TypeList<
  Int2Type<IxInstruction::ADD_A_ixd>, TypeList<Int2Type<IxInstruction::ADC_A_ixd>, TypeList<
  Int2Type<IxInstruction::POP_IX>,    TypeList<Int2Type<IxInstruction::EX_sp_IX>,  TypeList<
  Int2Type<IxInstruction::LD_IX_NN>,  TypeList<Int2Type<IxInstruction::LD_nn_IX>,  TypeList<
  Int2Type<IxInstruction::LD_ixd_N>,  TypeList<Int2Type<IxInstruction::ADD_IX_SP>, TypeList<
  Int2Type<IxInstruction::LD_ixd_D>,  TypeList<Int2Type<IxInstruction::LD_ixd_E>,  TypeList<
  Int2Type<IxInstruction::SUB_ixd>,   TypeList<Int2Type<IxInstruction::SBC_A_ixd>, TypeList<
  Int2Type<IxInstruction::PUSH_IX>,   TypeList<Int2Type<IxInstruction::JP_ix>,     TypeList<
  Int2Type<IxInstruction::INC_IX>,    TypeList<Int2Type<IxInstruction::ADD_IX_IX>, TypeList<
  Int2Type<IxInstruction::LD_B_ixd>,  TypeList<Int2Type<IxInstruction::LD_C_ixd>,  TypeList<
  Int2Type<IxInstruction::LD_ixd_H>,  TypeList<Int2Type<IxInstruction::LD_ixd_L>,  TypeList<
  Int2Type<IxInstruction::AND_ixd>,   TypeList<Int2Type<IxInstruction::XOR_ixd>,   TypeList<
  Int2Type<IxInstruction::LD_IX_nn>,  TypeList<Int2Type<IxInstruction::DEC_IX>,    TypeList<
  Int2Type<IxInstruction::LD_D_ixd>,  TypeList<Int2Type<IxInstruction::LD_E_ixd>,  TypeList<
  Int2Type<IxInstruction::LD_H_ixd>,  TypeList<Int2Type<IxInstruction::LD_L_ixd>,  TypeList<
  Int2Type<IxInstruction::LD_ixd_A>,  TypeList<Int2Type<IxInstruction::LD_A_ixd>,  TypeList<
  Int2Type<IxInstruction::OR_ixd>,    TypeList<Int2Type<IxInstruction::CP_ixd>,    TypeList<
  Int2Type<IxInstruction::LD_SP_IX>,  TypeList<Int2Type<IxInstruction::IxBitInstr>,NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IxInstructions;

typedef TypeList<
  Int2Type<IxBitInstruction::RLC_ixd>,   TypeList<Int2Type<IxBitInstruction::RRC_ixd>,   TypeList<
  Int2Type<IxBitInstruction::RL_ixd>,    TypeList<Int2Type<IxBitInstruction::RR_ixd>,    TypeList<
  Int2Type<IxBitInstruction::SLA_ixd>,   TypeList<Int2Type<IxBitInstruction::SRA_ixd>,   TypeList<
  Int2Type<IxBitInstruction::SLL_ixd>,   TypeList<Int2Type<IxBitInstruction::SRL_ixd>,   TypeList<
  Int2Type<IxBitInstruction::BIT_0_ixd>, TypeList<Int2Type<IxBitInstruction::BIT_1_ixd>, TypeList<
  Int2Type<IxBitInstruction::RES_0_ixd>, TypeList<Int2Type<IxBitInstruction::RES_1_ixd>, TypeList<
  Int2Type<IxBitInstruction::SET_0_ixd>, TypeList<Int2Type<IxBitInstruction::SET_1_ixd>, TypeList<
  Int2Type<IxBitInstruction::BIT_2_ixd>, TypeList<Int2Type<IxBitInstruction::BIT_3_ixd>, TypeList<
  Int2Type<IxBitInstruction::RES_2_ixd>, TypeList<Int2Type<IxBitInstruction::RES_3_ixd>, TypeList<
  Int2Type<IxBitInstruction::SET_2_ixd>, TypeList<Int2Type<IxBitInstruction::SET_3_ixd>, TypeList<
  Int2Type<IxBitInstruction::BIT_4_ixd>, TypeList<Int2Type<IxBitInstruction::BIT_5_ixd>, TypeList<
  Int2Type<IxBitInstruction::RES_4_ixd>, TypeList<Int2Type<IxBitInstruction::RES_5_ixd>, TypeList<
  Int2Type<IxBitInstruction::SET_4_ixd>, TypeList<Int2Type<IxBitInstruction::SET_5_ixd>, TypeList<
  Int2Type<IxBitInstruction::BIT_6_ixd>, TypeList<Int2Type<IxBitInstruction::BIT_7_ixd>, TypeList<
  Int2Type<IxBitInstruction::RES_6_ixd>, TypeList<Int2Type<IxBitInstruction::RES_7_ixd>, TypeList<
  Int2Type<IxBitInstruction::SET_6_ixd>, TypeList<Int2Type<IxBitInstruction::SET_7_ixd>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IxBitInstructions;


typedef TypeList<
  Int2Type<IyInstruction::ADD_IY_BC>, TypeList<Int2Type<IyInstruction::ADD_IY_DE>,  TypeList<
  Int2Type<IyInstruction::INC_iyd>,   TypeList<Int2Type<IyInstruction::DEC_iyd>,    TypeList<
  Int2Type<IyInstruction::LD_iyd_B>,  TypeList<Int2Type<IyInstruction::LD_iyd_C>,   TypeList<
  Int2Type<IyInstruction::ADD_A_iyd>, TypeList<Int2Type<IyInstruction::ADC_A_iyd>,  TypeList<
  Int2Type<IyInstruction::POP_IY>,    TypeList<Int2Type<IyInstruction::EX_sp_IY>,   TypeList<
  Int2Type<IyInstruction::LD_IY_NN>,  TypeList<Int2Type<IyInstruction::LD_nn_IY>,   TypeList<
  Int2Type<IyInstruction::LD_iyd_N>,  TypeList<Int2Type<IyInstruction::ADD_IY_SP>,  TypeList<
  Int2Type<IyInstruction::LD_iyd_D>,  TypeList<Int2Type<IyInstruction::LD_iyd_E>,   TypeList<
  Int2Type<IyInstruction::SUB_iyd>,   TypeList<Int2Type<IyInstruction::SBC_A_iyd>,  TypeList<
  Int2Type<IyInstruction::PUSH_IY>,   TypeList<Int2Type<IyInstruction::JP_iy>,      TypeList<
  Int2Type<IyInstruction::INC_IY>,    TypeList<Int2Type<IyInstruction::ADD_IY_IY>,  TypeList<
  Int2Type<IyInstruction::LD_B_iyd>,  TypeList<Int2Type<IyInstruction::LD_C_iyd>,   TypeList<
  Int2Type<IyInstruction::LD_iyd_H>,  TypeList<Int2Type<IyInstruction::LD_iyd_L>,   TypeList<
  Int2Type<IyInstruction::AND_iyd>,   TypeList<Int2Type<IyInstruction::XOR_iyd>,    TypeList<
  Int2Type<IyInstruction::LD_IY_nn>,  TypeList<Int2Type<IyInstruction::DEC_IY>,     TypeList<
  Int2Type<IyInstruction::LD_D_iyd>,  TypeList<Int2Type<IyInstruction::LD_E_iyd>,   TypeList<
  Int2Type<IyInstruction::LD_H_iyd>,  TypeList<Int2Type<IyInstruction::LD_L_iyd>,   TypeList<
  Int2Type<IyInstruction::LD_iyd_A>,  TypeList<Int2Type<IyInstruction::LD_A_iyd>,   TypeList<
  Int2Type<IyInstruction::OR_iyd>,    TypeList<Int2Type<IyInstruction::CP_iyd>,     TypeList<
  Int2Type<IyInstruction::LD_SP_IY>,  TypeList<Int2Type<IyInstruction::IyBitInstr>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IyInstructions;

typedef TypeList<
  Int2Type<IyBitInstruction::RLC_iyd>,   TypeList<Int2Type<IyBitInstruction::RRC_iyd>,   TypeList<
  Int2Type<IyBitInstruction::RL_iyd>,    TypeList<Int2Type<IyBitInstruction::RR_iyd>,    TypeList<
  Int2Type<IyBitInstruction::SLA_iyd>,   TypeList<Int2Type<IyBitInstruction::SRA_iyd>,   TypeList<
  Int2Type<IyBitInstruction::SLL_iyd>,   TypeList<Int2Type<IyBitInstruction::SRL_iyd>,   TypeList<
  Int2Type<IyBitInstruction::BIT_0_iyd>, TypeList<Int2Type<IyBitInstruction::BIT_1_iyd>, TypeList<
  Int2Type<IyBitInstruction::RES_0_iyd>, TypeList<Int2Type<IyBitInstruction::RES_1_iyd>, TypeList<
  Int2Type<IyBitInstruction::SET_0_iyd>, TypeList<Int2Type<IyBitInstruction::SET_1_iyd>, TypeList<
  Int2Type<IyBitInstruction::BIT_2_iyd>, TypeList<Int2Type<IyBitInstruction::BIT_3_iyd>, TypeList<
  Int2Type<IyBitInstruction::RES_2_iyd>, TypeList<Int2Type<IyBitInstruction::RES_3_iyd>, TypeList<
  Int2Type<IyBitInstruction::SET_2_iyd>, TypeList<Int2Type<IyBitInstruction::SET_3_iyd>, TypeList<
  Int2Type<IyBitInstruction::BIT_4_iyd>, TypeList<Int2Type<IyBitInstruction::BIT_5_iyd>, TypeList<
  Int2Type<IyBitInstruction::RES_4_iyd>, TypeList<Int2Type<IyBitInstruction::RES_5_iyd>, TypeList<
  Int2Type<IyBitInstruction::SET_4_iyd>, TypeList<Int2Type<IyBitInstruction::SET_5_iyd>, TypeList<
  Int2Type<IyBitInstruction::BIT_6_iyd>, TypeList<Int2Type<IyBitInstruction::BIT_7_iyd>, TypeList<
  Int2Type<IyBitInstruction::RES_6_iyd>, TypeList<Int2Type<IyBitInstruction::RES_7_iyd>, TypeList<
  Int2Type<IyBitInstruction::SET_6_iyd>, TypeList<Int2Type<IyBitInstruction::SET_7_iyd>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IyBitInstructions;

typedef TypeList<
  Int2Type<MiscInstruction::IN_B_c>,    TypeList<Int2Type<MiscInstruction::OUT_c_B>,  TypeList<
  Int2Type<MiscInstruction::IN_D_c>,    TypeList<Int2Type<MiscInstruction::OUT_c_D>,  TypeList<
  Int2Type<MiscInstruction::IN_H_c>,    TypeList<Int2Type<MiscInstruction::OUT_c_H>,  TypeList<
  Int2Type<MiscInstruction::SBC_HL_SP>, TypeList<Int2Type<MiscInstruction::LD_nn_DE>, TypeList<
  Int2Type<MiscInstruction::SBC_HL_BC>, TypeList<Int2Type<MiscInstruction::LD_nn_BC>, TypeList<
  Int2Type<MiscInstruction::SBC_HL_DE>, TypeList<Int2Type<MiscInstruction::LD_nn_SP>, TypeList<
  Int2Type<MiscInstruction::SBC_HL_HL>, TypeList<Int2Type<MiscInstruction::RRD>,      TypeList<
  Int2Type<MiscInstruction::IN_A_c>,    TypeList<Int2Type<MiscInstruction::OUT_c_A>,  TypeList<
  Int2Type<MiscInstruction::NEG>,       TypeList<Int2Type<MiscInstruction::RETN>,     TypeList<
  Int2Type<MiscInstruction::IM_1>,      TypeList<Int2Type<MiscInstruction::LD_A_I>,   TypeList<
  Int2Type<MiscInstruction::IN_L_c>,    TypeList<Int2Type<MiscInstruction::OUT_c_L>,  TypeList<
  Int2Type<MiscInstruction::ADC_HL_SP>, TypeList<Int2Type<MiscInstruction::LD_SP_nn>, TypeList<
  Int2Type<MiscInstruction::IM_0>,      TypeList<Int2Type<MiscInstruction::LD_I_A>,   TypeList<
  Int2Type<MiscInstruction::IN_E_c>,    TypeList<Int2Type<MiscInstruction::OUT_c_E>,  TypeList<
  Int2Type<MiscInstruction::ADC_HL_HL>, TypeList<Int2Type<MiscInstruction::RLD>,      TypeList<
  Int2Type<MiscInstruction::IN_C_c>,    TypeList<Int2Type<MiscInstruction::OUT_c_C>,  TypeList<
  Int2Type<MiscInstruction::ADC_HL_BC>, TypeList<Int2Type<MiscInstruction::LD_BC_nn>, TypeList<
  Int2Type<MiscInstruction::RETI>,      TypeList<Int2Type<MiscInstruction::LD_R_A>,   TypeList<
  Int2Type<MiscInstruction::ADC_HL_DE>, TypeList<Int2Type<MiscInstruction::LD_DE_nn>, TypeList<
  Int2Type<MiscInstruction::IM_2>,      TypeList<Int2Type<MiscInstruction::LD_A_R>,   TypeList<
  Int2Type<MiscInstruction::LDI>,       TypeList<Int2Type<MiscInstruction::CPI>,      TypeList<
  Int2Type<MiscInstruction::LDIR>,      TypeList<Int2Type<MiscInstruction::CPIR>,     TypeList<
  Int2Type<MiscInstruction::INI>,       TypeList<Int2Type<MiscInstruction::OUTI>,     TypeList<
  Int2Type<MiscInstruction::INIR>,      TypeList<Int2Type<MiscInstruction::OTIR>,     TypeList<
  Int2Type<MiscInstruction::LDD>,       TypeList<Int2Type<MiscInstruction::CPD>,      TypeList<
  Int2Type<MiscInstruction::LDDR>,      TypeList<Int2Type<MiscInstruction::CPDR>,     TypeList<
  Int2Type<MiscInstruction::IND>,       TypeList<Int2Type<MiscInstruction::OUTD>,     TypeList<
  Int2Type<MiscInstruction::INDR>,      TypeList<Int2Type<MiscInstruction::OTDR>,     NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    MiscMInstructions;

class Lock {
public:
  Lock(PixelGameEngine* g): GameEngine(g) { GameEngine->Event(Int2Type<NEW_DEBUG_MODE_CALLBACK>()); }
  ~Lock() { GameEngine->Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>()); }

private:
  PixelGameEngine* GameEngine;
};

};


typedef std::pair<std::string, std::unordered_map<int32_t, int32_t>> DisassembleT;

// enum MemoryT {
//   W27C512, IMS1423, HY62256A, MC146818
// };

// typedef TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_i>, Int2Type<'i'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_a>, Int2Type<'a'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_r>, Int2Type<'r'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_p>, Int2Type<'p'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_P>, Int2Type<'P'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_h>, Int2Type<'h'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_j>, Int2Type<'j'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_k>, Int2Type<'k'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_l>, Int2Type<'l'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_x>, Int2Type<'x'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_w>, Int2Type<'w'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_b>, Int2Type<'b'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_e>, Int2Type<'e'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_0>, Int2Type<'0'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_gg>,Int2Type<'g'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_G>, Int2Type<'G'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_C>, Int2Type<'C'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_D>, Int2Type<'D'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_n>, Int2Type<'n'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_N>, Int2Type<'N'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_f>, Int2Type<'f'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_F>, Int2Type<'F'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_y>,  Int2Type<'y'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_u>,  Int2Type<'u'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_U>,  Int2Type<'U'>>, TypeList<

//   TypeList<Int2Type<Editor::VimT::CMD_DOLLAR>,     Int2Type<'$'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_SLASH>,      Int2Type<'/'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_QUESTION>,   Int2Type<'?'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_COMMA>,      Int2Type<','>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_SEMICOLON>,  Int2Type<';'>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//    MemoryCommands; 
  
// typedef TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_x>,  Int2Type<'x'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_r>, Int2Type<'r'>>, NullType>>
//    SyncMemoryCommands; 

// typedef TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_i>, Int2Type<'i'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_a>, Int2Type<'a'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_r>, Int2Type<'r'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_p>, Int2Type<'p'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_P>, Int2Type<'P'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_x>, Int2Type<'x'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_y>,  Int2Type<'y'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_u>,  Int2Type<'u'>>, TypeList<
//   TypeList<Int2Type<Editor::VimT::CMD_U>,  Int2Type<'U'>>, NullType>>>>>>>>>>>
//    LockedMemoryCommands; 


enum LcdROM {};
typedef std::array<uint8_t, LCD_SEGMENT_SIZE> SegmentT;
typedef TypeList<TypeList<LcdROM, Int2Type<LCD_SEGMENT_SIZE>>, SegmentT> LcdCHR;

typedef TypeList<
  AnyType<0x10,LcdCHR>, TypeList<AnyType<0x11,LcdCHR>, TypeList<
  AnyType<0x12,LcdCHR>, TypeList<AnyType<0x13,LcdCHR>, TypeList<AnyType<0x14,LcdCHR>, TypeList<AnyType<0x15,LcdCHR>, TypeList<
  AnyType<0x16,LcdCHR>, TypeList<AnyType<0x17,LcdCHR>, TypeList<AnyType<0x18,LcdCHR>, TypeList<AnyType<0x19,LcdCHR>, TypeList<
  AnyType<0x1A,LcdCHR>, TypeList<AnyType<0x1B,LcdCHR>, TypeList<AnyType<0x1C,LcdCHR>, TypeList<AnyType<0x1D,LcdCHR>, TypeList<
  AnyType<0x1E,LcdCHR>, TypeList<AnyType<0x1F,LcdCHR>, TypeList<AnyType<'!', LcdCHR>, TypeList<AnyType<'"', LcdCHR>, TypeList<
  AnyType<'#', LcdCHR>, TypeList<AnyType<'$', LcdCHR>, TypeList<AnyType<'%', LcdCHR>, TypeList<AnyType<'&', LcdCHR>, TypeList<
  AnyType<'\'',LcdCHR>, TypeList<AnyType<'(', LcdCHR>, TypeList<AnyType<')', LcdCHR>, TypeList<AnyType<'*', LcdCHR>, TypeList<
  AnyType<'+', LcdCHR>, TypeList<AnyType<',', LcdCHR>, TypeList<AnyType<'-', LcdCHR>, TypeList<AnyType<'.', LcdCHR>, TypeList<
  AnyType<'/', LcdCHR>, TypeList<AnyType<'0', LcdCHR>, TypeList<AnyType<'1', LcdCHR>, TypeList<AnyType<'2', LcdCHR>, TypeList<
  AnyType<'3', LcdCHR>, TypeList<AnyType<'4', LcdCHR>, TypeList<AnyType<'5', LcdCHR>, TypeList<AnyType<'6', LcdCHR>, TypeList<
  AnyType<'7', LcdCHR>, TypeList<AnyType<'8', LcdCHR>, TypeList<AnyType<'9', LcdCHR>, TypeList<AnyType<':', LcdCHR>, TypeList<
  AnyType<';', LcdCHR>, TypeList<AnyType<'<', LcdCHR>, TypeList<AnyType<'=', LcdCHR>, TypeList<AnyType<'>', LcdCHR>, TypeList<
  AnyType<'?', LcdCHR>, TypeList<AnyType<'@', LcdCHR>, TypeList<AnyType<'A', LcdCHR>, TypeList<AnyType<'B', LcdCHR>, TypeList<
  AnyType<'C', LcdCHR>, TypeList<AnyType<'D', LcdCHR>, TypeList<AnyType<'E', LcdCHR>, TypeList<AnyType<'F', LcdCHR>, TypeList<
  AnyType<'G', LcdCHR>, TypeList<AnyType<'H', LcdCHR>, TypeList<AnyType<'I', LcdCHR>, TypeList<AnyType<'J', LcdCHR>, TypeList<
  AnyType<'K', LcdCHR>, TypeList<AnyType<'L', LcdCHR>, TypeList<AnyType<'M', LcdCHR>, TypeList<AnyType<'N', LcdCHR>, TypeList<
  AnyType<'O', LcdCHR>, TypeList<AnyType<'P', LcdCHR>, TypeList<AnyType<'Q', LcdCHR>, TypeList<AnyType<'R', LcdCHR>, TypeList<
  AnyType<'S', LcdCHR>, TypeList<AnyType<'T', LcdCHR>, TypeList<AnyType<'U', LcdCHR>, TypeList<AnyType<'V', LcdCHR>, TypeList<
  AnyType<'W', LcdCHR>, TypeList<AnyType<'X', LcdCHR>, TypeList<AnyType<'Y', LcdCHR>, TypeList<AnyType<'Z', LcdCHR>, TypeList<
  AnyType<'[', LcdCHR>, TypeList<AnyType<'\\',LcdCHR>, TypeList<AnyType<']', LcdCHR>, TypeList<AnyType<'^', LcdCHR>, TypeList<
  AnyType<'_', LcdCHR>, TypeList<AnyType<'`', LcdCHR>, TypeList<AnyType<'a', LcdCHR>, TypeList<AnyType<'b', LcdCHR>, TypeList<
  AnyType<'c', LcdCHR>, TypeList<AnyType<'d', LcdCHR>, TypeList<AnyType<'e', LcdCHR>, TypeList<AnyType<'f', LcdCHR>, TypeList<
  AnyType<'g', LcdCHR>, TypeList<AnyType<'h', LcdCHR>, TypeList<AnyType<'i', LcdCHR>, TypeList<AnyType<'j', LcdCHR>, TypeList<
  AnyType<'k', LcdCHR>, TypeList<AnyType<'l', LcdCHR>, TypeList<AnyType<'m', LcdCHR>, TypeList<AnyType<'n', LcdCHR>, TypeList<
  AnyType<'o', LcdCHR>, TypeList<AnyType<'p', LcdCHR>, TypeList<AnyType<'q', LcdCHR>, TypeList<AnyType<'r', LcdCHR>, TypeList<
  AnyType<'s', LcdCHR>, TypeList<AnyType<'t', LcdCHR>, TypeList<AnyType<'u', LcdCHR>, TypeList<AnyType<'v', LcdCHR>, TypeList<
  AnyType<'w', LcdCHR>, TypeList<AnyType<'x', LcdCHR>, TypeList<AnyType<'y', LcdCHR>, TypeList<AnyType<'z', LcdCHR>, TypeList<
  AnyType<'{', LcdCHR>, TypeList<AnyType<'|', LcdCHR>, TypeList<AnyType<'}', LcdCHR>, TypeList<AnyType<'~', LcdCHR>, TypeList<
  AnyType<0x7F,LcdCHR>, TypeList<AnyType<0x80,LcdCHR>, TypeList<AnyType<0x81,LcdCHR>, TypeList<AnyType<0x82,LcdCHR>, TypeList<
  AnyType<0x83,LcdCHR>, TypeList<AnyType<0x84,LcdCHR>, TypeList<AnyType<0x85,LcdCHR>, TypeList<AnyType<0x86,LcdCHR>, TypeList<
  AnyType<0x87,LcdCHR>, TypeList<AnyType<0x88,LcdCHR>, TypeList<AnyType<0x89,LcdCHR>, TypeList<AnyType<0x8A,LcdCHR>, TypeList<
  AnyType<0x8B,LcdCHR>, TypeList<AnyType<0x8C,LcdCHR>, TypeList<AnyType<0x8D,LcdCHR>, TypeList<AnyType<0x8E,LcdCHR>, TypeList<
  AnyType<0x8F,LcdCHR>, TypeList<AnyType<0x90,LcdCHR>, TypeList<AnyType<0x91,LcdCHR>, TypeList<AnyType<0x92,LcdCHR>, TypeList<
  AnyType<0x93,LcdCHR>, TypeList<AnyType<0x94,LcdCHR>, TypeList<AnyType<0x95,LcdCHR>, TypeList<AnyType<0x96,LcdCHR>, TypeList<
  AnyType<0x97,LcdCHR>, TypeList<AnyType<0x97,LcdCHR>, TypeList<AnyType<0x98,LcdCHR>, TypeList<AnyType<0x98,LcdCHR>, TypeList<
  AnyType<0x99,LcdCHR>, TypeList<AnyType<0x9A,LcdCHR>, TypeList<AnyType<0x9B,LcdCHR>, TypeList<AnyType<0x9C,LcdCHR>, TypeList<
  AnyType<0x9D,LcdCHR>, TypeList<AnyType<0x9E,LcdCHR>, TypeList<AnyType<0x9F,LcdCHR>, TypeList<AnyType<0xA0,LcdCHR>, TypeList<
  AnyType<0xA1,LcdCHR>, TypeList<AnyType<0xA2,LcdCHR>, TypeList<AnyType<0xA3,LcdCHR>, TypeList<AnyType<0xA4,LcdCHR>, TypeList<
  AnyType<0xA5,LcdCHR>, TypeList<AnyType<0xA6,LcdCHR>, TypeList<AnyType<0xA7,LcdCHR>, TypeList<AnyType<0xA8,LcdCHR>, TypeList<
  AnyType<0xA9,LcdCHR>, TypeList<AnyType<0xAA,LcdCHR>, TypeList<AnyType<0xAB,LcdCHR>, TypeList<AnyType<0xAC,LcdCHR>, TypeList<
  AnyType<0xAD,LcdCHR>, TypeList<AnyType<0xAE,LcdCHR>, TypeList<AnyType<0xAF,LcdCHR>, TypeList<AnyType<0xB0,LcdCHR>, TypeList<
  AnyType<0xB1,LcdCHR>, TypeList<AnyType<0xB2,LcdCHR>, TypeList<AnyType<0xB3,LcdCHR>, TypeList<AnyType<0xB4,LcdCHR>, TypeList<
  AnyType<0xB5,LcdCHR>, TypeList<AnyType<0xB6,LcdCHR>, TypeList<AnyType<0xB7,LcdCHR>, TypeList<AnyType<0xB8,LcdCHR>, TypeList<
  AnyType<0xB9,LcdCHR>, TypeList<AnyType<0xBA,LcdCHR>, TypeList<AnyType<0xBB,LcdCHR>, TypeList<AnyType<0xBC,LcdCHR>, TypeList<
  AnyType<0xBD,LcdCHR>, TypeList<AnyType<0xBE,LcdCHR>, TypeList<AnyType<0xBF,LcdCHR>, TypeList<AnyType<0xC0,LcdCHR>, TypeList<
  AnyType<0xC1,LcdCHR>, TypeList<AnyType<0xC2,LcdCHR>, TypeList<AnyType<0xC3,LcdCHR>, TypeList<AnyType<0xC4,LcdCHR>, TypeList<
  AnyType<0xC5,LcdCHR>, TypeList<AnyType<0xC6,LcdCHR>, TypeList<AnyType<0xC7,LcdCHR>, TypeList<AnyType<0xC8,LcdCHR>, TypeList<
  AnyType<0xC9,LcdCHR>, TypeList<AnyType<0xCA,LcdCHR>, TypeList<AnyType<0xCB,LcdCHR>, TypeList<AnyType<0xCC,LcdCHR>, TypeList<
  AnyType<0xCD,LcdCHR>, TypeList<AnyType<0xCE,LcdCHR>, TypeList<AnyType<0xCF,LcdCHR>, TypeList<AnyType<0xD0,LcdCHR>, TypeList<
  AnyType<0xD1,LcdCHR>, TypeList<AnyType<0xD2,LcdCHR>, TypeList<AnyType<0xD3,LcdCHR>, TypeList<AnyType<0xD4,LcdCHR>, TypeList<
  AnyType<0xD5,LcdCHR>, TypeList<AnyType<0xD6,LcdCHR>, TypeList<AnyType<0xD7,LcdCHR>, TypeList<AnyType<0xD8,LcdCHR>, TypeList<
  AnyType<0xD9,LcdCHR>, TypeList<AnyType<0xDA,LcdCHR>, TypeList<AnyType<0xDB,LcdCHR>, TypeList<AnyType<0xDC,LcdCHR>, TypeList<
  AnyType<0xDD,LcdCHR>, TypeList<AnyType<0xDE,LcdCHR>, TypeList<AnyType<0xDF,LcdCHR>, TypeList<AnyType<0xE0,LcdCHR>, TypeList<
  AnyType<0xE1,LcdCHR>, TypeList<AnyType<0xE2,LcdCHR>, TypeList<AnyType<0xE3,LcdCHR>, TypeList<AnyType<0xE4,LcdCHR>, TypeList<
  AnyType<0xE5,LcdCHR>, TypeList<AnyType<0xE6,LcdCHR>, TypeList<AnyType<0xE7,LcdCHR>, TypeList<AnyType<0xE8,LcdCHR>, TypeList<
  AnyType<0xE9,LcdCHR>, TypeList<AnyType<0xEA,LcdCHR>, TypeList<AnyType<0xEB,LcdCHR>, TypeList<AnyType<0xEC,LcdCHR>, TypeList<
  AnyType<0xED,LcdCHR>, TypeList<AnyType<0xEE,LcdCHR>, TypeList<AnyType<0xEF,LcdCHR>, TypeList<AnyType<0xF0,LcdCHR>, TypeList<
  AnyType<0xF1,LcdCHR>, TypeList<AnyType<0xF2,LcdCHR>, TypeList<AnyType<0xF3,LcdCHR>, TypeList<AnyType<0xF4,LcdCHR>, TypeList<
  AnyType<0xF5,LcdCHR>, TypeList<AnyType<0xF6,LcdCHR>, TypeList<AnyType<0xF7,LcdCHR>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   LcdChars; 


typedef TypeList<
  TypeList<Int2Type<olc::Key::Q>, Int2Type<0x15>>, TypeList<
  TypeList<Int2Type<olc::Key::W>, Int2Type<0x1D>>, TypeList<
  TypeList<Int2Type<olc::Key::E>, Int2Type<0x24>>, TypeList<
  TypeList<Int2Type<olc::Key::R>, Int2Type<0x2D>>, TypeList<
  TypeList<Int2Type<olc::Key::T>, Int2Type<0x2C>>, TypeList<
  TypeList<Int2Type<olc::Key::Y>, Int2Type<0x35>>, TypeList<
  TypeList<Int2Type<olc::Key::U>, Int2Type<0x3C>>, TypeList<
  TypeList<Int2Type<olc::Key::I>, Int2Type<0x43>>, TypeList<
  TypeList<Int2Type<olc::Key::O>, Int2Type<0x44>>, TypeList<
  TypeList<Int2Type<olc::Key::P>, Int2Type<0x4D>>, TypeList<
  TypeList<Int2Type<olc::Key::A>, Int2Type<0x1C>>, TypeList<
  TypeList<Int2Type<olc::Key::S>, Int2Type<0x1B>>, TypeList<
  TypeList<Int2Type<olc::Key::D>, Int2Type<0x23>>, TypeList<
  TypeList<Int2Type<olc::Key::F>, Int2Type<0x2B>>, TypeList<
  TypeList<Int2Type<olc::Key::G>, Int2Type<0x34>>, TypeList<
  TypeList<Int2Type<olc::Key::H>, Int2Type<0x33>>, TypeList<
  TypeList<Int2Type<olc::Key::J>, Int2Type<0x3B>>, TypeList<
  TypeList<Int2Type<olc::Key::K>, Int2Type<0x42>>, TypeList<
  TypeList<Int2Type<olc::Key::L>, Int2Type<0x4B>>, TypeList<
  TypeList<Int2Type<olc::Key::Z>, Int2Type<0x1A>>, TypeList<
  TypeList<Int2Type<olc::Key::X>, Int2Type<0x22>>, TypeList<
  TypeList<Int2Type<olc::Key::C>, Int2Type<0x21>>, TypeList<
  TypeList<Int2Type<olc::Key::V>, Int2Type<0x2A>>, TypeList<
  TypeList<Int2Type<olc::Key::B>, Int2Type<0x32>>, TypeList<
  TypeList<Int2Type<olc::Key::N>, Int2Type<0x31>>, TypeList<
  TypeList<Int2Type<olc::Key::M>, Int2Type<0x3A>>, TypeList<

  TypeList<Int2Type<olc::Key::K1>, Int2Type<0x16>>, TypeList<
  TypeList<Int2Type<olc::Key::K2>, Int2Type<0x1E>>, TypeList<
  TypeList<Int2Type<olc::Key::K3>, Int2Type<0x26>>, TypeList<
  TypeList<Int2Type<olc::Key::K4>, Int2Type<0x25>>, TypeList<
  TypeList<Int2Type<olc::Key::K5>, Int2Type<0x2E>>, TypeList<
  TypeList<Int2Type<olc::Key::K6>, Int2Type<0x36>>, TypeList<
  TypeList<Int2Type<olc::Key::K7>, Int2Type<0x3D>>, TypeList<
  TypeList<Int2Type<olc::Key::K8>, Int2Type<0x3E>>, TypeList<
  TypeList<Int2Type<olc::Key::K9>, Int2Type<0x46>>, TypeList<
  TypeList<Int2Type<olc::Key::K0>, Int2Type<0x45>>, TypeList<

  TypeList<Int2Type<olc::Key::NP0>, Int2Type<'0'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP1>, Int2Type<'1'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP2>, Int2Type<'2'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP3>, Int2Type<'3'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP4>, Int2Type<'4'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP5>, Int2Type<'5'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP6>, Int2Type<'6'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP7>, Int2Type<'7'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP8>, Int2Type<'8'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP9>, Int2Type<'9'>>, TypeList<

  TypeList<Int2Type<olc::Key::TAB>,   Int2Type<0x0D>>, TypeList<
  TypeList<Int2Type<olc::Key::BACK>,  Int2Type<0x66>>, TypeList<
  TypeList<Int2Type<olc::Key::DEL>,   Int2Type<'\0'>>, TypeList<
  TypeList<Int2Type<olc::Key::SPACE>, Int2Type<0x29>>, TypeList<
  TypeList<Int2Type<olc::Key::ENTER>, Int2Type<0x5A>>, TypeList<
  TypeList<Int2Type<olc::Key::ESCAPE>,Int2Type<0x76>>, TypeList<

  TypeList<Int2Type<olc::Key::OEM_1>, Int2Type<0x4C>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_2>, Int2Type<0x4A>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_3>, Int2Type<0x0E>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_4>, Int2Type<0x54>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_5>, Int2Type<0x5D>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_6>, Int2Type<0x5B>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_7>, Int2Type<0x52>>, TypeList<

  TypeList<Int2Type<olc::Key::CAPS_LOCK>, Int2Type<0x58>>, TypeList<
  TypeList<Int2Type<olc::Key::SHIFT>,     Int2Type<0x12>>, TypeList<
  TypeList<Int2Type<olc::Key::CTRL>,      Int2Type<0x14>>, TypeList<

  TypeList<Int2Type<olc::Key::F1>, Int2Type<0x05>>, TypeList<
  TypeList<Int2Type<olc::Key::F2>, Int2Type<0x06>>, TypeList<
  TypeList<Int2Type<olc::Key::F3>, Int2Type<0x04>>, TypeList<
  TypeList<Int2Type<olc::Key::F4>, Int2Type<0x0C>>, TypeList<
  TypeList<Int2Type<olc::Key::F5>, Int2Type<0x03>>, TypeList<
  TypeList<Int2Type<olc::Key::F6>, Int2Type<0x0B>>, TypeList<
  TypeList<Int2Type<olc::Key::F7>, Int2Type<0x83>>, TypeList<
  TypeList<Int2Type<olc::Key::F8>, Int2Type<0x0A>>, TypeList<
  TypeList<Int2Type<olc::Key::F9>, Int2Type<0x01>>, TypeList<
  TypeList<Int2Type<olc::Key::F10>,Int2Type<0x09>>, TypeList<
  TypeList<Int2Type<olc::Key::F11>,Int2Type<0x78>>, TypeList<
  TypeList<Int2Type<olc::Key::F12>,Int2Type<0x07>>, TypeList<

  TypeList<Int2Type<olc::Key::EQUALS>, Int2Type<0x55>>, TypeList<
  TypeList<Int2Type<olc::Key::PERIOD>, Int2Type<0x49>>, TypeList<
  TypeList<Int2Type<olc::Key::COMMA>, Int2Type<0x41>>, TypeList<
  TypeList<Int2Type<olc::Key::MINUS>, Int2Type<0x4E>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   KeyboardScanCodes; 

   

class Defs {

public:
  static void Init() {


    AnyType<0x10,LcdCHR>::GetValue() = { 0b00000000, 0b11111110, 0b01111100, 0b00111000, 0b00010000 };
    AnyType<0x11,LcdCHR>::GetValue() = { 0b00010000, 0b00111000, 0b01111100, 0b11111110, 0b00000000 };
    AnyType<0x12,LcdCHR>::GetValue() = { 0b01100000, 0b10100000, 0b00000000, 0b01100000, 0b10100000 };
    AnyType<0x13,LcdCHR>::GetValue() = { 0b10100000, 0b11000000, 0b00000000, 0b10100000, 0b11000000 };
    AnyType<0x14,LcdCHR>::GetValue() = { 0b00100010, 0b01100110, 0b11101110, 0b01100110, 0b00100010 };
    AnyType<0x15,LcdCHR>::GetValue() = { 0b10001000, 0b11001100, 0b11101110, 0b11001100, 0b10001000 };
    AnyType<0x16,LcdCHR>::GetValue() = { 0b00111000, 0b01111100, 0b01111100, 0b01111100, 0b00111000 };
    AnyType<0x17,LcdCHR>::GetValue() = { 0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b11111000 };
    AnyType<0x18,LcdCHR>::GetValue() = { 0b00100000, 0b01000000, 0b11111110, 0b01000000, 0b00100000 };
    AnyType<0x19,LcdCHR>::GetValue() = { 0b00001000, 0b00000100, 0b11111110, 0b00000100, 0b00001000 };
    AnyType<0x1A,LcdCHR>::GetValue() = { 0b00010000, 0b00010000, 0b01010100, 0b00111000, 0b00010000 };
    AnyType<0x1B,LcdCHR>::GetValue() = { 0b00010000, 0b00111000, 0b01010100, 0b00010000, 0b00010000 };
    AnyType<0x1C,LcdCHR>::GetValue() = { 0b00000010, 0b00100010, 0b01010010, 0b10001010, 0b00000010 };
    AnyType<0x1D,LcdCHR>::GetValue() = { 0b00000010, 0b10001010, 0b01010010, 0b00100010, 0b00000010 };
    AnyType<0x1E,LcdCHR>::GetValue() = { 0b00000100, 0b00011100, 0b01111100, 0b00011100, 0b00000100 };
    AnyType<0x1F,LcdCHR>::GetValue() = { 0b01000000, 0b01110000, 0b01111100, 0b01110000, 0b01000000 };

    AnyType<'!', LcdCHR>::GetValue() = { 0b00000000, 0b00000000, 0b11110010, 0b00000000, 0b00000000 };
    AnyType<'"', LcdCHR>::GetValue() = { 0b00000000, 0b11100000, 0b00000000, 0b11100000, 0b00000000 };
    AnyType<'#', LcdCHR>::GetValue() = { 0b00101000, 0b11111110, 0b00101000, 0b11111110, 0b00101000 };
    AnyType<'$', LcdCHR>::GetValue() = { 0b00100100, 0b01010100, 0b11111110, 0b01010100, 0b01001000 };
    AnyType<'%', LcdCHR>::GetValue() = { 0b11000100, 0b11001000, 0b00010000, 0b00100110, 0b01000110 };
    AnyType<'&', LcdCHR>::GetValue() = { 0b01101100, 0b10010010, 0b10101010, 0b01000100, 0b00001010 };
    AnyType<'\'',LcdCHR>::GetValue() = { 0b00000000, 0b10100000, 0b11000000, 0b00000000, 0b00000000 };
    AnyType<'(', LcdCHR>::GetValue() = { 0b00000000, 0b00111000, 0b01000100, 0b10000010, 0b00000000 };
    AnyType<')', LcdCHR>::GetValue() = { 0b00000000, 0b10000010, 0b01000100, 0b00111000, 0b00000000 };
    AnyType<'*', LcdCHR>::GetValue() = { 0b00101000, 0b00010000, 0b01111100, 0b00010000, 0b00101000 };
    AnyType<'+', LcdCHR>::GetValue() = { 0b00010000, 0b00010000, 0b01111100, 0b00010000, 0b00010000 };
    AnyType<',', LcdCHR>::GetValue() = { 0b00000000, 0b00001010, 0b00001100, 0b00000000, 0b00000000 };
    AnyType<'-', LcdCHR>::GetValue() = { 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000 };
    AnyType<'.', LcdCHR>::GetValue() = { 0b00000000, 0b00000110, 0b00000110, 0b00000000, 0b00000000 };
    AnyType<'/', LcdCHR>::GetValue() = { 0b00000100, 0b00001000, 0b00010000, 0b00010000, 0b00100000 };
    
    AnyType<'0', LcdCHR>::GetValue() = { 0b01111100, 0b10001010, 0b10010010, 0b10100010, 0b01111100 };
    AnyType<'1', LcdCHR>::GetValue() = { 0b00000000, 0b01000010, 0b11111110, 0b00000010, 0b00000000 };
    AnyType<'2', LcdCHR>::GetValue() = { 0b01000010, 0b10000110, 0b10001010, 0b10010010, 0b01100010 };
    AnyType<'3', LcdCHR>::GetValue() = { 0b10000100, 0b10000010, 0b10100010, 0b11010010, 0b10001100 };
    AnyType<'4', LcdCHR>::GetValue() = { 0b00011000, 0b00101000, 0b01001000, 0b11111110, 0b00001000 };
    AnyType<'5', LcdCHR>::GetValue() = { 0b11100100, 0b10100010, 0b10100010, 0b10100010, 0b10011100 };
    AnyType<'6', LcdCHR>::GetValue() = { 0b00111100, 0b01010010, 0b10010010, 0b10010010, 0b00001100 };
    AnyType<'7', LcdCHR>::GetValue() = { 0b11000000, 0b10000000, 0b10001110, 0b10010000, 0b11100000 };
    AnyType<'8', LcdCHR>::GetValue() = { 0b01101100, 0b10010010, 0b10010010, 0b10010010, 0b01101100 };
    AnyType<'9', LcdCHR>::GetValue() = { 0b01100000, 0b10010010, 0b10010010, 0b10010100, 0b01111000 };
    AnyType<':', LcdCHR>::GetValue() = { 0b00000000, 0b01101100, 0b01101100, 0b00000000, 0b00000000 };
    AnyType<';', LcdCHR>::GetValue() = { 0b00000000, 0b01101010, 0b01101100, 0b00000000, 0b00000000 };
    AnyType<'<', LcdCHR>::GetValue() = { 0b00010000, 0b00101000, 0b01000100, 0b10000010, 0b00000000 };
    AnyType<'=', LcdCHR>::GetValue() = { 0b00101000, 0b00101000, 0b00101000, 0b00101000, 0b00101000 };
    AnyType<'>', LcdCHR>::GetValue() = { 0b00000000, 0b10000010, 0b01000100, 0b00101000, 0b00010000 };
    AnyType<'?', LcdCHR>::GetValue() = { 0b01000000, 0b10000000, 0b10001010, 0b10010000, 0b01100000 };

    AnyType<'@', LcdCHR>::GetValue() = { 0b01001100, 0b10010010, 0b10011110, 0b10000010, 0b01111100 };
    AnyType<'A', LcdCHR>::GetValue() = { 0b01111110, 0b10001000, 0b10001000, 0b10001000, 0b01111110 };
    AnyType<'B', LcdCHR>::GetValue() = { 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b01101100 };
    AnyType<'C', LcdCHR>::GetValue() = { 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01000100 };
    AnyType<'D', LcdCHR>::GetValue() = { 0b11111110, 0b10000010, 0b10000010, 0b01000100, 0b00111000 };
    AnyType<'E', LcdCHR>::GetValue() = { 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b10000010 };
    AnyType<'F', LcdCHR>::GetValue() = { 0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b10000000 };
    AnyType<'G', LcdCHR>::GetValue() = { 0b01111100, 0b10000010, 0b10010010, 0b10010010, 0b01011110 };
    AnyType<'H', LcdCHR>::GetValue() = { 0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b11111110 };
    AnyType<'I', LcdCHR>::GetValue() = { 0b00000000, 0b10000010, 0b11111110, 0b10000010, 0b00000000 };
    AnyType<'J', LcdCHR>::GetValue() = { 0b00000100, 0b00000010, 0b10000010, 0b11111100, 0b10000000 };
    AnyType<'K', LcdCHR>::GetValue() = { 0b11111110, 0b00010000, 0b00101000, 0b01000100, 0b10000010 };
    AnyType<'L', LcdCHR>::GetValue() = { 0b11111110, 0b00000010, 0b00000010, 0b00000010, 0b00000010 };
    AnyType<'M', LcdCHR>::GetValue() = { 0b11111110, 0b01000000, 0b00110000, 0b01000000, 0b11111110 };
    AnyType<'N', LcdCHR>::GetValue() = { 0b11111110, 0b00100000, 0b00010000, 0b00001000, 0b11111110 };
    AnyType<'O', LcdCHR>::GetValue() = { 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01111100 };

    AnyType<'P', LcdCHR>::GetValue() = { 0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b01100000 };
    AnyType<'Q', LcdCHR>::GetValue() = { 0b01111100, 0b10000010, 0b10001010, 0b10000100, 0b01111000 };
    AnyType<'R', LcdCHR>::GetValue() = { 0b11111110, 0b10010000, 0b10011000, 0b10010100, 0b01100010 };
    AnyType<'S', LcdCHR>::GetValue() = { 0b01100010, 0b10010010, 0b10010010, 0b10010010, 0b10001100 };
    AnyType<'T', LcdCHR>::GetValue() = { 0b10000000, 0b10000000, 0b11111110, 0b10000000, 0b10000000 };
    AnyType<'U', LcdCHR>::GetValue() = { 0b11111100, 0b00000010, 0b00000010, 0b00000010, 0b11111100 };
    AnyType<'V', LcdCHR>::GetValue() = { 0b11111000, 0b00000100, 0b00000010, 0b00000100, 0b11111000 };
    AnyType<'W', LcdCHR>::GetValue() = { 0b11111100, 0b00000010, 0b00011100, 0b00000010, 0b11111100 };
    AnyType<'X', LcdCHR>::GetValue() = { 0b11000110, 0b00101000, 0b00010000, 0b00101000, 0b11000110 };
    AnyType<'Y', LcdCHR>::GetValue() = { 0b11100000, 0b00010000, 0b00001110, 0b00010000, 0b11100000 };
    AnyType<'Z', LcdCHR>::GetValue() = { 0b10000110, 0b10001010, 0b10010010, 0b10100010, 0b11000010 };
    AnyType<'[', LcdCHR>::GetValue() = { 0b11111110, 0b10000010, 0b10000010, 0b00000000, 0b00000000 };
    AnyType<'\\',LcdCHR>::GetValue() = { 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100 };
    AnyType<']', LcdCHR>::GetValue() = { 0b00000000, 0b00000000, 0b10000010, 0b10000010, 0b11111110 };
    AnyType<'^', LcdCHR>::GetValue() = { 0b00010000, 0b01000000, 0b10000000, 0b01000000, 0b00010000 };
    AnyType<'_', LcdCHR>::GetValue() = { 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010 };

    AnyType<'`', LcdCHR>::GetValue() = { 0b00000000, 0b01000000, 0b00100000, 0b00010000, 0b00000000 };
    AnyType<'a', LcdCHR>::GetValue() = { 0b00000100, 0b00101010, 0b00101010, 0b00101010, 0b00011110 };
    AnyType<'b', LcdCHR>::GetValue() = { 0b11111110, 0b00010010, 0b00100010, 0b00100010, 0b00001110 };
    AnyType<'c', LcdCHR>::GetValue() = { 0b00011100, 0b00100010, 0b00100010, 0b00100010, 0b00000100 };
    AnyType<'d', LcdCHR>::GetValue() = { 0b00011100, 0b00100010, 0b00100010, 0b00010010, 0b11111110 };
    AnyType<'e', LcdCHR>::GetValue() = { 0b00011100, 0b00101010, 0b00101010, 0b00101010, 0b00011000 };
    AnyType<'f', LcdCHR>::GetValue() = { 0b00010000, 0b01111110, 0b10010000, 0b10000000, 0b01000000 };
    AnyType<'g', LcdCHR>::GetValue() = { 0b00110000, 0b01001010, 0b01001010, 0b01001010, 0b01111100 };
    AnyType<'h', LcdCHR>::GetValue() = { 0b11111110, 0b00010000, 0b00100000, 0b00100000, 0b00011110 };
    AnyType<'i', LcdCHR>::GetValue() = { 0b00000000, 0b00100010, 0b10111110, 0b00000010, 0b00000000 };
    AnyType<'j', LcdCHR>::GetValue() = { 0b00000100, 0b00000010, 0b00100010, 0b10111100, 0b00000000 };
    AnyType<'k', LcdCHR>::GetValue() = { 0b11111110, 0b00001000, 0b00010100, 0b00100010, 0b00000000 };
    AnyType<'l', LcdCHR>::GetValue() = { 0b00000000, 0b10000010, 0b11111110, 0b00000010, 0b00000000 };
    AnyType<'m', LcdCHR>::GetValue() = { 0b00111110, 0b00100000, 0b00011100, 0b00100000, 0b00011110 };
    AnyType<'n', LcdCHR>::GetValue() = { 0b00111110, 0b00010000, 0b00100000, 0b00100000, 0b00011110 };
    AnyType<'o', LcdCHR>::GetValue() = { 0b00011100, 0b00100010, 0b00100010, 0b00100010, 0b00011100 };

    AnyType<'p', LcdCHR>::GetValue() = { 0b00111110, 0b00101000, 0b00101000, 0b00101000, 0b00010000 };
    AnyType<'q', LcdCHR>::GetValue() = { 0b00010000, 0b00101000, 0b00101000, 0b00011000, 0b00111110 };
    AnyType<'r', LcdCHR>::GetValue() = { 0b00111110, 0b00010000, 0b00100000, 0b00100000, 0b00010000 };
    AnyType<'s', LcdCHR>::GetValue() = { 0b00010010, 0b00101010, 0b00101010, 0b00101010, 0b00000100 };
    AnyType<'t', LcdCHR>::GetValue() = { 0b00100000, 0b11111100, 0b00100010, 0b00000010, 0b00000100 };
    AnyType<'u', LcdCHR>::GetValue() = { 0b00111100, 0b00000010, 0b00000010, 0b00000100, 0b00111110 };
    AnyType<'v', LcdCHR>::GetValue() = { 0b00111000, 0b00000100, 0b00000010, 0b00000100, 0b00111000 };
    AnyType<'w', LcdCHR>::GetValue() = { 0b00111100, 0b00000010, 0b00001100, 0b00000010, 0b00111000 };
    AnyType<'x', LcdCHR>::GetValue() = { 0b00100010, 0b00010100, 0b00001000, 0b00010100, 0b00100010 };
    AnyType<'y', LcdCHR>::GetValue() = { 0b00110000, 0b00001010, 0b00001010, 0b00001010, 0b00111100 };
    AnyType<'z', LcdCHR>::GetValue() = { 0b00100010, 0b00100110, 0b00101010, 0b00110010, 0b00100010 };
    AnyType<'{', LcdCHR>::GetValue() = { 0b00000000, 0b00010000, 0b01101100, 0b10000010, 0b00000000 };
    AnyType<'|', LcdCHR>::GetValue() = { 0b00000000, 0b00000000, 0b11111110, 0b00000000, 0b00000000 };
    AnyType<'}', LcdCHR>::GetValue() = { 0b00000000, 0b10000010, 0b01101100, 0b00010000, 0b00000000 };
    AnyType<'~', LcdCHR>::GetValue() = { 0b00001000, 0b00010000, 0b00010000, 0b00001000, 0b00010000 };
    AnyType<0x7F,LcdCHR>::GetValue() = { 0b00111100, 0b01000100, 0b10000100, 0b01000100, 0b00111100 };

    AnyType<0x80,LcdCHR>::GetValue() = { 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b11001100 };
    AnyType<0x81,LcdCHR>::GetValue() = { 0b00001111, 0b10010100, 0b11100100, 0b10000100, 0b11111111 };
    AnyType<0x82,LcdCHR>::GetValue() = { 0b11101110, 0b00010000, 0b11111110, 0b00010000, 0b11101110 };
    AnyType<0x83,LcdCHR>::GetValue() = { 0b10000010, 0b10000010, 0b10010010, 0b10010010, 0b01101100 };
    AnyType<0x84,LcdCHR>::GetValue() = { 0b11111110, 0b00001000, 0b00010000, 0b00100000, 0b11111110 };
    AnyType<0x85,LcdCHR>::GetValue() = { 0b00111111, 0b10000010, 0b01000100, 0b10001000, 0b00111111 };
    AnyType<0x86,LcdCHR>::GetValue() = { 0b00000100, 0b10000010, 0b11111100, 0b10000000, 0b11111110 };
    AnyType<0x87,LcdCHR>::GetValue() = { 0b11111110, 0b10000000, 0b10000000, 0b10000000, 0b11111110 };
    AnyType<0x88,LcdCHR>::GetValue() = { 0b11100010, 0b00010100, 0b00001000, 0b00010000, 0b11100000 };
    AnyType<0x89,LcdCHR>::GetValue() = { 0b11111100, 0b00000100, 0b00000100, 0b00000100, 0b11111110 };
    AnyType<0x8A,LcdCHR>::GetValue() = { 0b11100000, 0b00010000, 0b00010000, 0b00010000, 0b11111110 };
    AnyType<0x8B,LcdCHR>::GetValue() = { 0b01111110, 0b00000010, 0b01111110, 0b00000010, 0b01111110 };
    AnyType<0x8C,LcdCHR>::GetValue() = { 0b11111100, 0b00000100, 0b11111100, 0b00000100, 0b11111110 };
    AnyType<0x8D,LcdCHR>::GetValue() = { 0b10000000, 0b11111110, 0b00010010, 0b00010010, 0b00001100 };
    AnyType<0x8E,LcdCHR>::GetValue() = { 0b11111110, 0b00010010, 0b00001100, 0b00000000, 0b11111110 };
    AnyType<0x8F,LcdCHR>::GetValue() = { 0b01000100, 0b10010010, 0b10100010, 0b10010010, 0b01111100 };

    AnyType<0x90,LcdCHR>::GetValue() = { 0b00011100, 0b00100010, 0b00010010, 0b00001100, 0b00110010 };
    AnyType<0x91,LcdCHR>::GetValue() = { 0b00000110, 0b00000110, 0b11111110, 0b01000000, 0b00110000 };
    AnyType<0x92,LcdCHR>::GetValue() = { 0b11111110, 0b10000000, 0b10000000, 0b10000000, 0b11000000 };
    AnyType<0x93,LcdCHR>::GetValue() = { 0b00100010, 0b00111100, 0b00100000, 0b00111110, 0b00100010 };
    AnyType<0x94,LcdCHR>::GetValue() = { 0b11000110, 0b10101010, 0b10010010, 0b10000010, 0b10000010 };
    AnyType<0x95,LcdCHR>::GetValue() = { 0b00011100, 0b00100010, 0b00100010, 0b00111100, 0b00100000 };
    AnyType<0x96,LcdCHR>::GetValue() = { 0b00000110, 0b00000110, 0b11111100, 0b10100011, 0b01111111 };
    AnyType<0x97,LcdCHR>::GetValue() = { 0b00010000, 0b00100000, 0b00111100, 0b00100010, 0b01000000 };
    AnyType<0x97,LcdCHR>::GetValue() = { 0b00010000, 0b00100000, 0b00111100, 0b00100010, 0b01000000 };
    AnyType<0x98,LcdCHR>::GetValue() = { 0b00001000, 0b01111000, 0b11111100, 0b01111000, 0b00001000 };
    AnyType<0x98,LcdCHR>::GetValue() = { 0b00001000, 0b01111000, 0b11111100, 0b01111000, 0b00001000 };
    AnyType<0x99,LcdCHR>::GetValue() = { 0b01111100, 0b10010010, 0b10010010, 0b10010010, 0b01111100 };
    AnyType<0x9A,LcdCHR>::GetValue() = { 0b00111010, 0b01000110, 0b01000000, 0b01000110, 0b00111010 };
    AnyType<0x9B,LcdCHR>::GetValue() = { 0b00001100, 0b01010010, 0b10100010, 0b10010010, 0b01001100 };
    AnyType<0x9C,LcdCHR>::GetValue() = { 0b00011000, 0b00101000, 0b00010000, 0b00101000, 0b00110000 };
    AnyType<0x9D,LcdCHR>::GetValue() = { 0b00111000, 0b01111100, 0b00111110, 0b01111100, 0b00111000 };
    AnyType<0x9E,LcdCHR>::GetValue() = { 0b00010100, 0b00101010, 0b00101010, 0b00100010, 0b00000100 };
    AnyType<0x9F,LcdCHR>::GetValue() = { 0b01111110, 0b10000000, 0b10000000, 0b10000000, 0b01111110 };

    AnyType<0xA0,LcdCHR>::GetValue() = { 0b11111110, 0b11111110, 0b00000000, 0b11111110, 0b11111110 };
    AnyType<0xA1,LcdCHR>::GetValue() = { 0b00000000, 0b00000000, 0b10011110, 0b00000000, 0b00000000 };
    AnyType<0xA2,LcdCHR>::GetValue() = { 0b00111000, 0b01000100, 0b11111110, 0b01000100, 0b00001000 };
    AnyType<0xA3,LcdCHR>::GetValue() = { 0b00010010, 0b01111100, 0b10010010, 0b10000010, 0b00000100 };
    AnyType<0xA4,LcdCHR>::GetValue() = { 0b01000100, 0b00111000, 0b00111000, 0b00111000, 0b01000100 };
    AnyType<0xA5,LcdCHR>::GetValue() = { 0b10101000, 0b01101000, 0b00111110, 0b01101000, 0b10101000 };
    AnyType<0xA6,LcdCHR>::GetValue() = { 0b00000000, 0b00000000, 0b11101110, 0b00000000, 0b00000000 };
    AnyType<0xA7,LcdCHR>::GetValue() = { 0b00000100, 0b01010010, 0b10101010, 0b10010100, 0b01000000 };
    AnyType<0xA8,LcdCHR>::GetValue() = { 0b00010100, 0b00010010, 0b01111100, 0b10010000, 0b01010000 };
    AnyType<0xA9,LcdCHR>::GetValue() = { 0b11111110, 0b10000010, 0b10111010, 0b10010010, 0b11111110 };
    AnyType<0xAA,LcdCHR>::GetValue() = { 0b00010010, 0b10101010, 0b10101010, 0b10101010, 0b01111010 };
    AnyType<0xAB,LcdCHR>::GetValue() = { 0b00010000, 0b00101000, 0b01010100, 0b00101000, 0b01000100 };
    AnyType<0xAC,LcdCHR>::GetValue() = { 0b11111110, 0b00010000, 0b01111100, 0b10000010, 0b01111100 };
    AnyType<0xAD,LcdCHR>::GetValue() = { 0b01100010, 0b10010100, 0b10011000, 0b10010000, 0b11111110 };
    AnyType<0xAE,LcdCHR>::GetValue() = { 0b11111110, 0b10000010, 0b10100110, 0b10001010, 0b11111110 };
    AnyType<0xAF,LcdCHR>::GetValue() = { 0b00000000, 0b01100000, 0b10100000, 0b00000000, 0b00000000 };

    AnyType<0xB0,LcdCHR>::GetValue() = { 0b01110000, 0b10001000, 0b10001000, 0b10001000, 0b01110000 };
    AnyType<0xB1,LcdCHR>::GetValue() = { 0b00100010, 0b00100010, 0b11111010, 0b00100010, 0b00100010 };
    AnyType<0xB2,LcdCHR>::GetValue() = { 0b01001000, 0b10011000, 0b10101000, 0b01001000, 0b00000000 };
    AnyType<0xB3,LcdCHR>::GetValue() = { 0b10001000, 0b10101000, 0b10101000, 0b01010000, 0b00000000 };
    AnyType<0xB4,LcdCHR>::GetValue() = { 0b11111110, 0b10100000, 0b10100100, 0b01001111, 0b00000100 };
    AnyType<0xB5,LcdCHR>::GetValue() = { 0b11111110, 0b00001000, 0b00001000, 0b00010000, 0b11111000 };
    AnyType<0xB6,LcdCHR>::GetValue() = { 0b01100000, 0b10010000, 0b10010000, 0b11111110, 0b11111110 };
    AnyType<0xB7,LcdCHR>::GetValue() = { 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000 };
    AnyType<0xB8,LcdCHR>::GetValue() = { 0b00011100, 0b00100010, 0b00001100, 0b00100010, 0b00011100 };
    AnyType<0xB9,LcdCHR>::GetValue() = { 0b01001000, 0b11111000, 0b00001000, 0b00000000, 0b00000000 };
    AnyType<0xBA,LcdCHR>::GetValue() = { 0b01110010, 0b10001010, 0b10001010, 0b10001010, 0b01110010 };
    AnyType<0xBB,LcdCHR>::GetValue() = { 0b01000100, 0b00101000, 0b01010100, 0b00101000, 0b00010000 };
    AnyType<0xBC,LcdCHR>::GetValue() = { 0b11101000, 0b00010110, 0b00101010, 0b01011111, 0b10000010 };
    AnyType<0xBD,LcdCHR>::GetValue() = { 0b11101000, 0b00010000, 0b00101001, 0b01010011, 0b10001101 };
    AnyType<0xBE,LcdCHR>::GetValue() = { 0b10101000, 0b11111000, 0b00000110, 0b00001010, 0b00011111 };
    AnyType<0xBF,LcdCHR>::GetValue() = { 0b00001100, 0b00010010, 0b10100010, 0b00000010, 0b00000100 };

    AnyType<0xC0,LcdCHR>::GetValue() = { 0b00001111, 0b10010100, 0b01100100, 0b00010100, 0b00001111 };
    AnyType<0xC1,LcdCHR>::GetValue() = { 0b00001111, 0b00010100, 0b01100100, 0b10010100, 0b00001111 };
    AnyType<0xC2,LcdCHR>::GetValue() = { 0b00001111, 0b01010100, 0b10010100, 0b01010100, 0b00001111 };
    AnyType<0xC3,LcdCHR>::GetValue() = { 0b01001111, 0b10010100, 0b10010100, 0b01010100, 0b10001111 };
    AnyType<0xC4,LcdCHR>::GetValue() = { 0b00001111, 0b10010100, 0b00010100, 0b10010100, 0b00001111 };
    AnyType<0xC5,LcdCHR>::GetValue() = { 0b00001111, 0b01010100, 0b10110100, 0b01010100, 0b00001111 };
    AnyType<0xC6,LcdCHR>::GetValue() = { 0b00111110, 0b01001000, 0b11111110, 0b10010010, 0b10010010 };
    AnyType<0xC7,LcdCHR>::GetValue() = { 0b01111000, 0b10000100, 0b10000101, 0b10000111, 0b01001000 };
    AnyType<0xC8,LcdCHR>::GetValue() = { 0b00011111, 0b10010101, 0b01010101, 0b00010101, 0b00010001 };
    AnyType<0xC9,LcdCHR>::GetValue() = { 0b00011111, 0b01010101, 0b10010101, 0b00010101, 0b00010001 };
    AnyType<0xCA,LcdCHR>::GetValue() = { 0b00011111, 0b01010101, 0b10010101, 0b01010101, 0b00010001 };
    AnyType<0xCB,LcdCHR>::GetValue() = { 0b00011111, 0b01010101, 0b00010101, 0b01010101, 0b00010001 };
    AnyType<0xCC,LcdCHR>::GetValue() = { 0b00000000, 0b10010001, 0b01011111, 0b00010001, 0b00000000 };
    AnyType<0xCD,LcdCHR>::GetValue() = { 0b00000000, 0b00010001, 0b01011111, 0b10010001, 0b00000000 };
    AnyType<0xCE,LcdCHR>::GetValue() = { 0b00000000, 0b01010001, 0b10011111, 0b01010001, 0b00000000 };
    AnyType<0xCF,LcdCHR>::GetValue() = { 0b00000000, 0b01010001, 0b00011111, 0b01010001, 0b00000000 };

    AnyType<0xD0,LcdCHR>::GetValue() = { 0b00010000, 0b11111110, 0b10010010, 0b10000010, 0b01111100 };
    AnyType<0xD1,LcdCHR>::GetValue() = { 0b01011111, 0b10001000, 0b10000100, 0b01000010, 0b10011111 };
    AnyType<0xD2,LcdCHR>::GetValue() = { 0b00011110, 0b10100001, 0b01100001, 0b00100001, 0b00011110 };
    AnyType<0xD3,LcdCHR>::GetValue() = { 0b00011110, 0b00100001, 0b01100001, 0b10100001, 0b00011110 };
    AnyType<0xD4,LcdCHR>::GetValue() = { 0b00001110, 0b01010001, 0b10010001, 0b01010001, 0b00001110 };
    AnyType<0xD5,LcdCHR>::GetValue() = { 0b01001110, 0b10010001, 0b10010001, 0b01010001, 0b10001110 };
    AnyType<0xD6,LcdCHR>::GetValue() = { 0b00011110, 0b10100001, 0b00100001, 0b10100001, 0b00011110 };
    AnyType<0xD7,LcdCHR>::GetValue() = { 0b00100010, 0b00010100, 0b00001000, 0b00010100, 0b00100010 };
    AnyType<0xD8,LcdCHR>::GetValue() = { 0b00010000, 0b10101010, 0b11111110, 0b10101010, 0b00010000 };
    AnyType<0xD9,LcdCHR>::GetValue() = { 0b00111110, 0b10000001, 0b01000001, 0b00000001, 0b00111110 };
    AnyType<0xDA,LcdCHR>::GetValue() = { 0b00111110, 0b00000001, 0b01000001, 0b10000001, 0b00111110 };
    AnyType<0xDB,LcdCHR>::GetValue() = { 0b00111110, 0b01000001, 0b10000001, 0b01000001, 0b00111110 };
    AnyType<0xDC,LcdCHR>::GetValue() = { 0b00111110, 0b10000001, 0b00000001, 0b10000001, 0b00111110 };
    AnyType<0xDD,LcdCHR>::GetValue() = { 0b00100000, 0b00010000, 0b01001111, 0b10010000, 0b00100000 };
    AnyType<0xDE,LcdCHR>::GetValue() = { 0b10000001, 0b11111111, 0b00100101, 0b00100100, 0b00011000 };
    AnyType<0xDF,LcdCHR>::GetValue() = { 0b00000010, 0b01111100, 0b10010010, 0b10010010, 0b01101100 };

    AnyType<0xE0,LcdCHR>::GetValue() = { 0b00000010, 0b10010101, 0b01010101, 0b00010101, 0b00001111 };
    AnyType<0xE1,LcdCHR>::GetValue() = { 0b00000010, 0b00010101, 0b01010101, 0b10010101, 0b00001111 };
    AnyType<0xE2,LcdCHR>::GetValue() = { 0b00000010, 0b01010101, 0b10010101, 0b01010101, 0b00001111 };
    AnyType<0xE3,LcdCHR>::GetValue() = { 0b01000010, 0b10010101, 0b10010101, 0b01010101, 0b10001111 };
    AnyType<0xE4,LcdCHR>::GetValue() = { 0b00000010, 0b01010101, 0b00010101, 0b01010101, 0b00001111 };
    AnyType<0xE5,LcdCHR>::GetValue() = { 0b00000010, 0b01010101, 0b10110101, 0b01010101, 0b00001111 };
    AnyType<0xE6,LcdCHR>::GetValue() = { 0b00000010, 0b01010101, 0b10110101, 0b01010101, 0b00001111 };
    AnyType<0xE7,LcdCHR>::GetValue() = { 0b01110000, 0b10001010, 0b10001110, 0b10001010, 0b00010000 };
    AnyType<0xE8,LcdCHR>::GetValue() = { 0b00001110, 0b10010101, 0b01010101, 0b00010101, 0b00001100 };
    AnyType<0xE9,LcdCHR>::GetValue() = { 0b00001110, 0b00010101, 0b01010101, 0b10010101, 0b00001100 };
    AnyType<0xEA,LcdCHR>::GetValue() = { 0b00001110, 0b01010101, 0b10010101, 0b01010101, 0b00001100 };
    AnyType<0xEB,LcdCHR>::GetValue() = { 0b00001110, 0b01010101, 0b00010101, 0b01010101, 0b00001100 };
    AnyType<0xEC,LcdCHR>::GetValue() = { 0b00000000, 0b10001001, 0b01011111, 0b00000001, 0b00000000 };
    AnyType<0xED,LcdCHR>::GetValue() = { 0b00000000, 0b00001001, 0b01011111, 0b10000001, 0b00000000 };
    AnyType<0xEE,LcdCHR>::GetValue() = { 0b00000000, 0b01001001, 0b10011111, 0b01000001, 0b00000000 };
    AnyType<0xEF,LcdCHR>::GetValue() = { 0b00000000, 0b01001001, 0b00011111, 0b01000001, 0b00000000 };

    AnyType<0xF0,LcdCHR>::GetValue() = { 0b01010010, 0b00100101, 0b01010101, 0b00001101, 0b00000110 };
    AnyType<0xF1,LcdCHR>::GetValue() = { 0b01011111, 0b10001000, 0b10010000, 0b01010000, 0b10001111 };
    AnyType<0xF2,LcdCHR>::GetValue() = { 0b00001110, 0b10010001, 0b01010001, 0b00010001, 0b00001110 };
    AnyType<0xF3,LcdCHR>::GetValue() = { 0b00001110, 0b00010001, 0b01010001, 0b10010001, 0b00001110 };
    AnyType<0xF4,LcdCHR>::GetValue() = { 0b00000110, 0b00101001, 0b01001001, 0b00101001, 0b00000110 };
    AnyType<0xF5,LcdCHR>::GetValue() = { 0b00100110, 0b01001001, 0b01001001, 0b00101001, 0b01000110 };
    AnyType<0xF6,LcdCHR>::GetValue() = { 0b00001110, 0b01010001, 0b00010001, 0b01010001, 0b00001110 };
    AnyType<0xF7,LcdCHR>::GetValue() = { 0b00010000, 0b00010000, 0b01010100, 0b00010000, 0b00010000 };
    AnyType<0xFF,LcdCHR>::GetValue() = { 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111 };
  }
};
};