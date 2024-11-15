#pragma once
#include "src/Editor/Editor.h"

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

typedef TypeList<Instruction, std::string> InstrSTR;
typedef TypeList<BitInstruction, std::string> BitSTR;
typedef TypeList<IxInstruction, std::string> IxSTR;
typedef TypeList<IyInstruction, std::string> IySTR;
typedef TypeList<MiscInstruction, std::string> MiscSTR;
typedef TypeList<IxBitInstruction, std::string> IxBitSTR;
typedef TypeList<IyBitInstruction, std::string> IyBitSTR;

typedef TypeList<
  TypeList<AnyType<Instruction::NOP,       InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_BC_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<Instruction::DJNZ_D,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::LD_DE_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<Instruction::JR_NZ_D,   InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::LD_HL_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<Instruction::JR_NC_D,   InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::LD_SP_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<Instruction::LD_B_B,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_B_C,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_D_B,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_D_C,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_H_B,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_H_C,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_hl_B,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_hl_C,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADD_A_B,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADD_A_C,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SUB_B,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SUB_C,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::AND_B,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::AND_C,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::OR_B,      InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::OR_C,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_NZ,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::POP_BC,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_NC,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::POP_DE,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_PO,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::POP_HL,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_P,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::POP_AF,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_bc_A,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::INC_BC,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_de_A,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::INC_DE,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_nn_HL,  InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::INC_HL,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_nn_A,   InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::INC_SP,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_B_D,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_B_E,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_D_D,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_D_E,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_H_D,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_H_E,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_hl_D,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_hl_E,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADD_A_D,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADD_A_E,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SUB_D,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SUB_E,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::AND_D,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::AND_E,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::OR_D,      InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::OR_E,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JP_NZ_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::JP_NN,     InstrSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<Instruction::JP_NC_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::OUT_n_A,   InstrSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<Instruction::JP_PO_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::EX_sp_HL,  InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JP_P_NN,   InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::DI,        InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_B,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_B,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_D,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_D,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_H,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_H,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_hl,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_hl,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_B_H,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_B_L,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_D_H,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_D_L,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_H_H,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_H_L,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_hl_H,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_hl_L,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADD_A_H,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADD_A_L,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SUB_H,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SUB_L,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::AND_H,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::AND_L,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::OR_H,      InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::OR_L,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CALL_NZ_NN,InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::PUSH_BC,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CALL_NC_NN,InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::PUSH_DE,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CALL_PO_NN,InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::PUSH_HL,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CALL_P_NN, InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::PUSH_AF,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_B_N,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RLCA,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_D_N,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RLA,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_H_N,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::DAA,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_hl_n,   InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::SCF,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_B_hl,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_B_A,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_D_hl,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_D_A,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_H_hl,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_H_A,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::HALT,      InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_hl_A,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADD_A_hl,  InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADD_A_A,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SUB_hl,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SUB_A,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::AND_hl,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::AND_A,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::OR_hl,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::OR_A,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADD_A_N,   InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_00h,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SUB_N,     InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_10h,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::AND_N,     InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_20h,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::OR_N,      InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_30h,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::EX_AF_AF,  InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADD_HL_BC, InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JR_D,      InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::ADD_HL_DE, InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JR_Z_D,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::ADD_HL_HL, InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JR_C_D,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::ADD_HL_SP, InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_C_B,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_C_C,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_E_B,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_E_C,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_L_B,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_L_C,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_B,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_A_C,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADC_A_B,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADC_A_C,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SBC_A_B,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SBC_A_C,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::XOR_B,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::XOR_C,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CP_B,      InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::CP_C,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_Z,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::RET,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_C,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::EXX,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_PE,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::JP_hl,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::RET_M,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_SP_HL,  InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_bc,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_BC,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_de,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_DE,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_HL_nn,  InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::DEC_HL,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_nn,   InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::DEC_SP,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_C_D,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_C_E,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_E_D,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_E_E,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_L_D,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_L_E,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_D,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_A_E,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADC_A_D,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADC_A_E,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SBC_A_D,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SBC_A_E,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::XOR_D,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::XOR_E,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CP_D,      InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::CP_E,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JP_Z_NN,   InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::BIT_INSTR, InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JP_C_NN,   InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::IN_A_n,    InstrSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<Instruction::JP_PE_NN,  InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::EX_DE_HL,  InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::JP_M_NN,   InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::EI,        InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_C,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_C,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_E,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_E,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_L,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_L,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::INC_A,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::DEC_A,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_C_H,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_C_L,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_E_H,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_E_L,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_L_H,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_L_L,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_H,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_A_L,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADC_A_H,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADC_A_L,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SBC_A_H,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SBC_A_L,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::XOR_H,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::XOR_L,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CP_H,      InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::CP_L,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CALL_Z_NN, InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::CALL_NN,   InstrSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<Instruction::CALL_C_NN, InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::IX_INSTR,  InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CALL_PE_NN,InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::MISC_INSTR,InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CALL_M_NN, InstrSTR>, Int2Type<0x23>>, TypeList<TypeList<AnyType<Instruction::IY_INSTR,  InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_C_N,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RRCA,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_E_N,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RRA,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_L_N,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::CPL,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_N,    InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::CCF,       InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_C_hl,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_C_A,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_E_hl,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_E_A,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_L_hl,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_L_A,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::LD_A_hl,   InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::LD_A_A,    InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADC_A_hl,  InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::ADC_A_A,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SBC_A_hl,  InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::SBC_A_A,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::XOR_hl,    InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::XOR_A,     InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CP_hl,     InstrSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<Instruction::CP_A,      InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::ADC_A_N,   InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_08h,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::SBC_A_N,   InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_18h,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::XOR_N,     InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_28h,   InstrSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<Instruction::CP_N,      InstrSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<Instruction::RST_38h,   InstrSTR>, Int2Type<0x01>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    Instructions;


typedef TypeList<
  TypeList<AnyType<BitInstruction::RLC_B,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RLC_C,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RL_B,    BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RL_C,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLA_B,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLA_C,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLL_B,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLL_C,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_0_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_0_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_2_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_2_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_4_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_4_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_6_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_6_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_0_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_0_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_2_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_2_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_4_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_4_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_6_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_6_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_0_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_0_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_2_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_2_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_4_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_4_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_6_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_6_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RLC_D,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RLC_E,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RL_D,    BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RL_E,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLA_D,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLA_E,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLL_D,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLL_E,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_0_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_0_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_2_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_2_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_4_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_4_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_6_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_6_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_0_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_0_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_2_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_2_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_4_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_4_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_6_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_6_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_0_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_0_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_2_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_2_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_4_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_4_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_6_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_6_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RLC_H,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RLC_L,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RL_H,    BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RL_L,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLA_H,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLA_L,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLL_H,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLL_L,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_0_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_0_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_2_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_2_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_4_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_4_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_6_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_6_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_0_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_0_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_2_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_2_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_4_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_4_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_6_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_6_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_0_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_0_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_2_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_2_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_4_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_4_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_6_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_6_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RLC_hl,  BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RLC_A,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RL_hl,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RL_A,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLA_hl,  BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLA_A,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SLL_hl,  BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SLL_A,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_0_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_0_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_2_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_2_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_4_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_4_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_6_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_6_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_0_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_0_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_2_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_2_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_4_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_4_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_6_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_6_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_0_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_0_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_2_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_2_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_4_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_4_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_6_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_6_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RRC_B,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RRC_C,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RR_B,    BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RR_C,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRA_B,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRA_C,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRL_B,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRL_C,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_1_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_1_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_3_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_3_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_5_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_5_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_7_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_7_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_1_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_1_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_3_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_3_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_5_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_5_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_7_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_7_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_1_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_1_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_3_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_3_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_5_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_5_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_7_B, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_7_C, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RRC_D,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RRC_E,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RR_D,    BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RR_E,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRA_D,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRA_E,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRL_D,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRL_E,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_1_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_1_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_3_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_3_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_5_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_5_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_7_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_7_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_1_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_1_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_3_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_3_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_5_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_5_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_7_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_7_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_1_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_1_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_3_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_3_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_5_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_5_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_7_D, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_7_E, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RRC_H,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RRC_L,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RR_H,    BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RR_L,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRA_H,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRA_L,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRL_H,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRL_L,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_1_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_1_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_3_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_3_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_5_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_5_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_7_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_7_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_1_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_1_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_3_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_3_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_5_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_5_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_7_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_7_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_1_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_1_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_3_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_3_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_5_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_5_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_7_H, BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_7_L, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RRC_hl,  BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RRC_A,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RR_hl,   BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RR_A,    BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRA_hl,  BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRA_A,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SRL_hl,  BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SRL_A,   BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_1_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_1_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_3_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_3_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_5_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_5_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::BIT_7_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::BIT_7_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_1_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_1_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_3_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_3_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_5_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_5_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::RES_7_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::RES_7_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_1_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_1_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_3_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_3_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_5_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_5_A, BitSTR>, Int2Type<1>>, TypeList<
  TypeList<AnyType<BitInstruction::SET_7_hl,BitSTR>, Int2Type<1>>, TypeList<TypeList<AnyType<BitInstruction::SET_7_A, BitSTR>, Int2Type<1>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    BitInstructions;

typedef TypeList<
  TypeList<AnyType<IxInstruction::ADD_IX_BC, IxSTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IxInstruction::ADD_IX_DE, IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::INC_ixd,   IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::DEC_ixd,   IxSTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_ixd_B,  IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::LD_ixd_C,  IxSTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IxInstruction::ADD_A_ixd, IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::ADC_A_ixd, IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::POP_IX,    IxSTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IxInstruction::EX_sp_IX,  IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_IX_NN,  IxSTR>, Int2Type<0x023>>, TypeList<TypeList<AnyType<IxInstruction::LD_nn_IX,  IxSTR>, Int2Type<0x023>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_ixd_N,  IxSTR>, Int2Type<0x113>>, TypeList<TypeList<AnyType<IxInstruction::ADD_IX_SP, IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_ixd_D,  IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::LD_ixd_E,  IxSTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IxInstruction::SUB_ixd,   IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::SBC_A_ixd, IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::PUSH_IX,   IxSTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IxInstruction::JP_ix,     IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::INC_IX,    IxSTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IxInstruction::ADD_IX_IX, IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_B_ixd,  IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::LD_C_ixd,  IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_ixd_H,  IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::LD_ixd_L,  IxSTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IxInstruction::AND_ixd,   IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::XOR_ixd,   IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_IX_nn,  IxSTR>, Int2Type<0x023>>, TypeList<TypeList<AnyType<IxInstruction::DEC_IX,    IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_D_ixd,  IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::LD_E_ixd,  IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_H_ixd,  IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::LD_L_ixd,  IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_ixd_A,  IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::LD_A_ixd,  IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::OR_ixd,    IxSTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IxInstruction::CP_ixd,    IxSTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IxInstruction::LD_SP_IX,  IxSTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IxInstruction::IxBitInstr,IxSTR>, Int2Type<0x001>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IxInstructions;

typedef TypeList<
  TypeList<AnyType<IxBitInstruction::RLC_ixd,   IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::RRC_ixd,   IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::RL_ixd,    IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::RR_ixd,    IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::SLA_ixd,   IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::SRA_ixd,   IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::SLL_ixd,   IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::SRL_ixd,   IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::BIT_0_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::BIT_1_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::RES_0_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::RES_1_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::SET_0_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::SET_1_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::BIT_2_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::BIT_3_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::RES_2_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::RES_3_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::SET_2_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::SET_3_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::BIT_4_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::BIT_5_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::RES_4_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::RES_5_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::SET_4_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::SET_5_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::BIT_6_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::BIT_7_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::RES_6_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::RES_7_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IxBitInstruction::SET_6_ixd, IxBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IxBitInstruction::SET_7_ixd, IxBitSTR>, Int2Type<0x12>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IxBitInstructions;


typedef TypeList<
  TypeList<AnyType<IyInstruction::ADD_IY_BC, IySTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IyInstruction::ADD_IY_DE, IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::INC_iyd,   IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::DEC_iyd,   IySTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_iyd_B,  IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::LD_iyd_C,  IySTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IyInstruction::ADD_A_iyd, IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::ADC_A_iyd, IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::POP_IY,    IySTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IyInstruction::EX_sp_IY,  IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_IY_NN,  IySTR>, Int2Type<0x023>>, TypeList<TypeList<AnyType<IyInstruction::LD_nn_IY,  IySTR>, Int2Type<0x023>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_iyd_N,  IySTR>, Int2Type<0x113>>, TypeList<TypeList<AnyType<IyInstruction::ADD_IY_SP, IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_iyd_D,  IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::LD_iyd_E,  IySTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IyInstruction::SUB_iyd,   IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::SBC_A_iyd, IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::PUSH_IY,   IySTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IyInstruction::JP_iy,     IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::INC_IY,    IySTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IyInstruction::ADD_IY_IY, IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_B_iyd,  IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::LD_C_iyd,  IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_iyd_H,  IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::LD_iyd_L,  IySTR>, Int2Type<0x012>>, TypeList<
  TypeList<AnyType<IyInstruction::AND_iyd,   IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::XOR_iyd,   IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_IY_nn,  IySTR>, Int2Type<0x023>>, TypeList<TypeList<AnyType<IyInstruction::DEC_IY,    IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_D_iyd,  IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::LD_E_iyd,  IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_H_iyd,  IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::LD_L_iyd,  IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_iyd_A,  IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::LD_A_iyd,  IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::OR_iyd,    IySTR>, Int2Type<0x012>>, TypeList<TypeList<AnyType<IyInstruction::CP_iyd,    IySTR>, Int2Type<0x001>>, TypeList<
  TypeList<AnyType<IyInstruction::LD_SP_IY,  IySTR>, Int2Type<0x001>>, TypeList<TypeList<AnyType<IyInstruction::IyBitInstr,IySTR>, Int2Type<0x001>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IyInstructions;

typedef TypeList<
  TypeList<AnyType<IyBitInstruction::RLC_iyd,   IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::RRC_iyd,   IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::RL_iyd,    IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::RR_iyd,    IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::SLA_iyd,   IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::SRA_iyd,   IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::SLL_iyd,   IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::SRL_iyd,   IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::BIT_0_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::BIT_1_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::RES_0_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::RES_1_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::SET_0_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::SET_1_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::BIT_2_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::BIT_3_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::RES_2_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::RES_3_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::SET_2_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::SET_3_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::BIT_4_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::BIT_5_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::RES_4_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::RES_5_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::SET_4_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::SET_5_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::BIT_6_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::BIT_7_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::RES_6_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::RES_7_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<
  TypeList<AnyType<IyBitInstruction::SET_6_iyd, IyBitSTR>, Int2Type<0x12>>, TypeList<TypeList<AnyType<IyBitInstruction::SET_7_iyd, IyBitSTR>, Int2Type<0x12>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    IyBitInstructions;

typedef TypeList<
  TypeList<AnyType<MiscInstruction::IN_B_c,    MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUT_c_B,  MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IN_D_c,    MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUT_c_D,  MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IN_H_c,    MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUT_c_H,  MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::SBC_HL_SP, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_nn_DE, MiscSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<MiscInstruction::SBC_HL_BC, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_nn_BC, MiscSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<MiscInstruction::SBC_HL_DE, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_nn_SP, MiscSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<MiscInstruction::SBC_HL_HL, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::RRD,      MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IN_A_c,    MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUT_c_A,  MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::NEG,       MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::RETN,     MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IM_1,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_A_I,   MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IN_L_c,    MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUT_c_L,  MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::ADC_HL_SP, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_SP_nn, MiscSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<MiscInstruction::IM_0,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_I_A,   MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IN_E_c,    MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUT_c_E,  MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::ADC_HL_HL, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::RLD,      MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IN_C_c,    MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUT_c_C,  MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::ADC_HL_BC, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_BC_nn, MiscSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<MiscInstruction::RETI,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_R_A,   MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::ADC_HL_DE, MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_DE_nn, MiscSTR>, Int2Type<0x23>>, TypeList<
  TypeList<AnyType<MiscInstruction::IM_2,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::LD_A_R,   MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::LDI,       MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::CPI,      MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::LDIR,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::CPIR,     MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::INI,       MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUTI,     MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::INIR,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OTIR,     MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::LDD,       MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::CPD,      MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::LDDR,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::CPDR,     MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::IND,       MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OUTD,     MiscSTR>, Int2Type<0x01>>, TypeList<
  TypeList<AnyType<MiscInstruction::INDR,      MiscSTR>, Int2Type<0x01>>, TypeList<TypeList<AnyType<MiscInstruction::OTDR,     MiscSTR>, Int2Type<0x01>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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

enum MemoryT {
  W27C512, IMS1423
};

typedef TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_i>, Int2Type<'i'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_a>, Int2Type<'a'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_r>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_p>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_P>, Int2Type<'P'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_h>, Int2Type<'h'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_j>, Int2Type<'j'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_k>, Int2Type<'k'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_l>, Int2Type<'l'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_x>, Int2Type<'x'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_w>, Int2Type<'w'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_b>, Int2Type<'b'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_e>, Int2Type<'e'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_0>, Int2Type<'0'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_gg>,Int2Type<'g'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_G>, Int2Type<'G'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_C>, Int2Type<'C'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_D>, Int2Type<'D'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_n>, Int2Type<'n'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_N>, Int2Type<'N'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_f>, Int2Type<'f'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_F>, Int2Type<'F'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_y>,  Int2Type<'y'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_u>,  Int2Type<'u'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_U>,  Int2Type<'U'>>, TypeList<

  TypeList<Int2Type<Editor::VimT::CMD_DOLLAR>,     Int2Type<'$'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_SLASH>,      Int2Type<'/'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_QUESTION>,   Int2Type<'?'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_COMMA>,      Int2Type<','>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_SEMICOLON>,  Int2Type<';'>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   MemoryCommands; 
  
typedef TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_x>,  Int2Type<'x'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_r>, Int2Type<'r'>>, NullType>>
   SyncMemoryCommands; 

typedef TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_i>, Int2Type<'i'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_a>, Int2Type<'a'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_r>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_p>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_P>, Int2Type<'P'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_x>, Int2Type<'x'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_y>,  Int2Type<'y'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_u>,  Int2Type<'u'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_U>,  Int2Type<'U'>>, NullType>>>>>>>>>>>
   LockedMemoryCommands; 


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


class Device {
public:
  virtual uint8_t Read(uint32_t addr, bool) = 0;
  virtual uint8_t Write(uint32_t addr, uint8_t data, bool) = 0;
};


   

class Defs {

public:
  static void Init() {
    AnyType<Z80::NOP,     Z80::InstrSTR>::GetValue() = "NOP";                AnyType<Z80::LD_BC_NN, Z80::InstrSTR>::GetValue() = "LD BC, 0x%04x";
    AnyType<Z80::DJNZ_D,  Z80::InstrSTR>::GetValue() = "DJNZ 0x%02x";        AnyType<Z80::LD_DE_NN, Z80::InstrSTR>::GetValue() = "LD DE, 0x%04x";
    AnyType<Z80::JR_NZ_D, Z80::InstrSTR>::GetValue() = "JR NZ, 0x%02x";      AnyType<Z80::LD_HL_NN, Z80::InstrSTR>::GetValue() = "LD HL, 0x%04x";
    AnyType<Z80::JR_NC_D, Z80::InstrSTR>::GetValue() = "JR NZ, 0x%02x";      AnyType<Z80::LD_SP_NN, Z80::InstrSTR>::GetValue() = "LD SP, 0x%04x";
    AnyType<Z80::LD_B_B,  Z80::InstrSTR>::GetValue() = "LD B, B";            AnyType<Z80::LD_B_C,   Z80::InstrSTR>::GetValue() = "LD B, C";
    AnyType<Z80::LD_D_B,  Z80::InstrSTR>::GetValue() = "LD D, B";            AnyType<Z80::LD_D_C,   Z80::InstrSTR>::GetValue() = "LD D, C";
    AnyType<Z80::LD_H_B,  Z80::InstrSTR>::GetValue() = "LD H, B";            AnyType<Z80::LD_H_C,   Z80::InstrSTR>::GetValue() = "LD H, C";
    AnyType<Z80::LD_hl_B, Z80::InstrSTR>::GetValue() = "LD (HL), B";         AnyType<Z80::LD_hl_C,  Z80::InstrSTR>::GetValue() = "LD (HL), C";
    AnyType<Z80::ADD_A_B, Z80::InstrSTR>::GetValue() = "ADD A, B";           AnyType<Z80::ADD_A_C,  Z80::InstrSTR>::GetValue() = "ADD A, C";
    AnyType<Z80::SUB_B,   Z80::InstrSTR>::GetValue() = "SUB B";              AnyType<Z80::SUB_C,    Z80::InstrSTR>::GetValue() = "SUB C";
    AnyType<Z80::AND_B,   Z80::InstrSTR>::GetValue() = "AND B";              AnyType<Z80::AND_C,    Z80::InstrSTR>::GetValue() = "AND C";
    AnyType<Z80::OR_B,    Z80::InstrSTR>::GetValue() = "OR B";               AnyType<Z80::OR_C,     Z80::InstrSTR>::GetValue() = "OR C";
    AnyType<Z80::RET_NZ,  Z80::InstrSTR>::GetValue() = "RET NZ";             AnyType<Z80::POP_BC,   Z80::InstrSTR>::GetValue() = "POP BC";
    AnyType<Z80::RET_NC,  Z80::InstrSTR>::GetValue() = "RET NC";             AnyType<Z80::POP_DE,   Z80::InstrSTR>::GetValue() = "POP DE";
    AnyType<Z80::RET_PO,  Z80::InstrSTR>::GetValue() = "RET PO";             AnyType<Z80::POP_HL,   Z80::InstrSTR>::GetValue() = "POP HL";
    AnyType<Z80::RET_P,   Z80::InstrSTR>::GetValue() = "RET P";              AnyType<Z80::POP_AF,   Z80::InstrSTR>::GetValue() = "POP AF";

    
    AnyType<Z80::LD_bc_A,  Z80::InstrSTR>::GetValue() = "LD (BC), A";        AnyType<Z80::INC_BC,   Z80::InstrSTR>::GetValue() = "INC BC";
    AnyType<Z80::LD_de_A,  Z80::InstrSTR>::GetValue() = "LD (DE), A";        AnyType<Z80::INC_DE,   Z80::InstrSTR>::GetValue() = "INC DE";
    AnyType<Z80::LD_nn_HL, Z80::InstrSTR>::GetValue() = "LD (0x%04x), HL";   AnyType<Z80::INC_HL,   Z80::InstrSTR>::GetValue() = "INC HL";
    AnyType<Z80::LD_nn_A,  Z80::InstrSTR>::GetValue() = "LD (0x%04x), A";    AnyType<Z80::INC_SP,   Z80::InstrSTR>::GetValue() = "INC SP";
    AnyType<Z80::LD_B_D,   Z80::InstrSTR>::GetValue() = "LD B, D";           AnyType<Z80::LD_B_E,   Z80::InstrSTR>::GetValue() = "LD B, E";
    AnyType<Z80::LD_D_D,   Z80::InstrSTR>::GetValue() = "LD D, D";           AnyType<Z80::LD_D_E,   Z80::InstrSTR>::GetValue() = "LD D, E";
    AnyType<Z80::LD_H_D,   Z80::InstrSTR>::GetValue() = "LD H, D";           AnyType<Z80::LD_H_E,   Z80::InstrSTR>::GetValue() = "LD H, E";
    AnyType<Z80::LD_hl_D,  Z80::InstrSTR>::GetValue() = "LD (HL), D";        AnyType<Z80::LD_hl_E,  Z80::InstrSTR>::GetValue() = "LD (HL), E";
    AnyType<Z80::ADD_A_D,  Z80::InstrSTR>::GetValue() = "ADD A, D";          AnyType<Z80::ADD_A_E,  Z80::InstrSTR>::GetValue() = "ADD A, E";
    AnyType<Z80::SUB_D,    Z80::InstrSTR>::GetValue() = "SUB D";             AnyType<Z80::SUB_E,    Z80::InstrSTR>::GetValue() = "SUB E";
    AnyType<Z80::AND_D,    Z80::InstrSTR>::GetValue() = "AND D";             AnyType<Z80::AND_E,    Z80::InstrSTR>::GetValue() = "AND E";
    AnyType<Z80::OR_D,     Z80::InstrSTR>::GetValue() = "OR D";              AnyType<Z80::OR_E,     Z80::InstrSTR>::GetValue() = "OR E";
    AnyType<Z80::JP_NZ_NN, Z80::InstrSTR>::GetValue() = "JP NZ, 0x%04x";     AnyType<Z80::JP_NN,    Z80::InstrSTR>::GetValue() = "JP 0x%04x";
    AnyType<Z80::JP_NC_NN, Z80::InstrSTR>::GetValue() = "JP NC, 0x%04x";     AnyType<Z80::OUT_n_A,  Z80::InstrSTR>::GetValue() = "OUT (0x%02x), A";
    AnyType<Z80::JP_PO_NN, Z80::InstrSTR>::GetValue() = "JP PO, 0x%04x";     AnyType<Z80::EX_sp_HL, Z80::InstrSTR>::GetValue() = "EX (SP), HL";
    AnyType<Z80::JP_P_NN,  Z80::InstrSTR>::GetValue() = "JP P, 0x%04x";      AnyType<Z80::DI,       Z80::InstrSTR>::GetValue() = "DI";


    AnyType<Z80::INC_B,      Z80::InstrSTR>::GetValue() = "INC B";           AnyType<Z80::DEC_B,   Z80::InstrSTR>::GetValue() = "DEC B";
    AnyType<Z80::INC_D,      Z80::InstrSTR>::GetValue() = "INC D";           AnyType<Z80::DEC_D,   Z80::InstrSTR>::GetValue() = "DEC D";
    AnyType<Z80::INC_H,      Z80::InstrSTR>::GetValue() = "INC H";           AnyType<Z80::DEC_H,   Z80::InstrSTR>::GetValue() = "DEC H";
    AnyType<Z80::INC_hl,     Z80::InstrSTR>::GetValue() = "INC (HL)";        AnyType<Z80::DEC_hl,  Z80::InstrSTR>::GetValue() = "DEC (HL)";
    AnyType<Z80::LD_B_H,     Z80::InstrSTR>::GetValue() = "LD B, H";         AnyType<Z80::LD_B_L,  Z80::InstrSTR>::GetValue() = "LD B, L";
    AnyType<Z80::LD_D_H,     Z80::InstrSTR>::GetValue() = "LD D, H";         AnyType<Z80::LD_D_L,  Z80::InstrSTR>::GetValue() = "LD D, L";
    AnyType<Z80::LD_H_H,     Z80::InstrSTR>::GetValue() = "LD H, H";         AnyType<Z80::LD_H_L,  Z80::InstrSTR>::GetValue() = "LD H, L";
    AnyType<Z80::LD_hl_H,    Z80::InstrSTR>::GetValue() = "LD (HL), H";      AnyType<Z80::LD_hl_L, Z80::InstrSTR>::GetValue() = "LD (HL), L";
    AnyType<Z80::ADD_A_H,    Z80::InstrSTR>::GetValue() = "ADD A, H";        AnyType<Z80::ADD_A_L, Z80::InstrSTR>::GetValue() = "ADD A, L";
    AnyType<Z80::SUB_H,      Z80::InstrSTR>::GetValue() = "SUB H";           AnyType<Z80::SUB_L,   Z80::InstrSTR>::GetValue() = "SUB L";
    AnyType<Z80::AND_H,      Z80::InstrSTR>::GetValue() = "AND H";           AnyType<Z80::AND_L,   Z80::InstrSTR>::GetValue() = "AND L";
    AnyType<Z80::OR_H,       Z80::InstrSTR>::GetValue() = "OR H";            AnyType<Z80::OR_L,    Z80::InstrSTR>::GetValue() = "OR L";
    AnyType<Z80::CALL_NZ_NN, Z80::InstrSTR>::GetValue() = "CALL NZ, 0x%04x"; AnyType<Z80::PUSH_BC, Z80::InstrSTR>::GetValue() = "PUSH BC";
    AnyType<Z80::CALL_NC_NN, Z80::InstrSTR>::GetValue() = "CALL NC, 0x%04x"; AnyType<Z80::PUSH_DE, Z80::InstrSTR>::GetValue() = "PUSH DE";
    AnyType<Z80::CALL_PO_NN, Z80::InstrSTR>::GetValue() = "CALL PO, 0x%04x"; AnyType<Z80::PUSH_HL, Z80::InstrSTR>::GetValue() = "PUSH HL";
    AnyType<Z80::CALL_P_NN,  Z80::InstrSTR>::GetValue() = "CALL P, 0x%04x";  AnyType<Z80::PUSH_AF, Z80::InstrSTR>::GetValue() = "PUSH AF";


    AnyType<Z80::LD_B_N,   Z80::InstrSTR>::GetValue() = "LD B, 0x%02x";      AnyType<Z80::RLCA,    Z80::InstrSTR>::GetValue() = "RLCA";
    AnyType<Z80::LD_D_N,   Z80::InstrSTR>::GetValue() = "LD D, 0x%02x";      AnyType<Z80::RLA,     Z80::InstrSTR>::GetValue() = "RLA";
    AnyType<Z80::LD_H_N,   Z80::InstrSTR>::GetValue() = "LD H, 0x%02x";      AnyType<Z80::DAA,     Z80::InstrSTR>::GetValue() = "DAA";
    AnyType<Z80::LD_hl_n,  Z80::InstrSTR>::GetValue() = "LD (HL), 0x%02x";   AnyType<Z80::SCF,     Z80::InstrSTR>::GetValue() = "SCF";
    AnyType<Z80::LD_B_hl,  Z80::InstrSTR>::GetValue() = "LD B, (HL)";        AnyType<Z80::LD_B_A,  Z80::InstrSTR>::GetValue() = "LD B, A";
    AnyType<Z80::LD_D_hl,  Z80::InstrSTR>::GetValue() = "LD D, (HL)";        AnyType<Z80::LD_D_A,  Z80::InstrSTR>::GetValue() = "LD D, A";
    AnyType<Z80::LD_H_hl,  Z80::InstrSTR>::GetValue() = "LD H, (HL)";        AnyType<Z80::LD_H_A,  Z80::InstrSTR>::GetValue() = "LD H, A";
    AnyType<Z80::HALT,     Z80::InstrSTR>::GetValue() = "HALT";              AnyType<Z80::LD_hl_A, Z80::InstrSTR>::GetValue() = "LD (HL), A";
    AnyType<Z80::ADD_A_hl, Z80::InstrSTR>::GetValue() = "ADD A, (HL)";       AnyType<Z80::ADD_A_A, Z80::InstrSTR>::GetValue() = "ADD A, A";
    AnyType<Z80::SUB_hl,   Z80::InstrSTR>::GetValue() = "SUB (HL)";          AnyType<Z80::SUB_A,   Z80::InstrSTR>::GetValue() = "SUB A";
    AnyType<Z80::AND_hl,   Z80::InstrSTR>::GetValue() = "AND (HL)";          AnyType<Z80::AND_A,   Z80::InstrSTR>::GetValue() = "AND A";
    AnyType<Z80::OR_hl,    Z80::InstrSTR>::GetValue() = "OR (HL)";           AnyType<Z80::OR_A,    Z80::InstrSTR>::GetValue() = "OR A";
    AnyType<Z80::ADD_A_N,  Z80::InstrSTR>::GetValue() = "ADD A, 0x%02x";     AnyType<Z80::RST_00h, Z80::InstrSTR>::GetValue() = "RST 0x00";
    AnyType<Z80::SUB_N,    Z80::InstrSTR>::GetValue() = "SUB 0x%02x";        AnyType<Z80::RST_10h, Z80::InstrSTR>::GetValue() = "RST 0x10";
    AnyType<Z80::AND_N,    Z80::InstrSTR>::GetValue() = "AND 0x%02x";        AnyType<Z80::RST_20h, Z80::InstrSTR>::GetValue() = "RST 0x20";
    AnyType<Z80::OR_N,     Z80::InstrSTR>::GetValue() = "OR 0x%02x";         AnyType<Z80::RST_30h, Z80::InstrSTR>::GetValue() = "RST 0x30";


    AnyType<Z80::EX_AF_AF, Z80::InstrSTR>::GetValue() = "EX AF, AF'";        AnyType<Z80::ADD_HL_BC, Z80::InstrSTR>::GetValue() = "ADD HL, BC";
    AnyType<Z80::JR_D,     Z80::InstrSTR>::GetValue() = "JR 0x%02x";         AnyType<Z80::ADD_HL_DE, Z80::InstrSTR>::GetValue() = "ADD HL, DE";
    AnyType<Z80::JR_Z_D,   Z80::InstrSTR>::GetValue() = "JR Z, 0x%02x";      AnyType<Z80::ADD_HL_HL, Z80::InstrSTR>::GetValue() = "ADD HL, HL";
    AnyType<Z80::JR_C_D,   Z80::InstrSTR>::GetValue() = "JR C, 0x%02x";      AnyType<Z80::ADD_HL_SP, Z80::InstrSTR>::GetValue() = "ADD HL, SP";
    AnyType<Z80::LD_C_B,   Z80::InstrSTR>::GetValue() = "LD C, B";           AnyType<Z80::LD_C_C,    Z80::InstrSTR>::GetValue() = "LD C, C";
    AnyType<Z80::LD_E_B,   Z80::InstrSTR>::GetValue() = "LD E, B";           AnyType<Z80::LD_E_C,    Z80::InstrSTR>::GetValue() = "LD E, C";
    AnyType<Z80::LD_L_B,   Z80::InstrSTR>::GetValue() = "LD L, B";           AnyType<Z80::LD_L_C,    Z80::InstrSTR>::GetValue() = "LD L, C";
    AnyType<Z80::LD_A_B,   Z80::InstrSTR>::GetValue() = "LD A, B";           AnyType<Z80::LD_A_C,    Z80::InstrSTR>::GetValue() = "LD A, C";
    AnyType<Z80::ADC_A_B,  Z80::InstrSTR>::GetValue() = "ADC A, B";          AnyType<Z80::ADC_A_C,   Z80::InstrSTR>::GetValue() = "ADC A, C";
    AnyType<Z80::SBC_A_B,  Z80::InstrSTR>::GetValue() = "SBC A, B";          AnyType<Z80::SBC_A_C,   Z80::InstrSTR>::GetValue() = "SBC A, C";
    AnyType<Z80::XOR_B,    Z80::InstrSTR>::GetValue() = "XOR B";             AnyType<Z80::XOR_C,     Z80::InstrSTR>::GetValue() = "XOR C";
    AnyType<Z80::CP_B,     Z80::InstrSTR>::GetValue() = "CP B";              AnyType<Z80::CP_C,      Z80::InstrSTR>::GetValue() = "CP C";
    AnyType<Z80::RET_Z,    Z80::InstrSTR>::GetValue() = "RET Z";             AnyType<Z80::RET,       Z80::InstrSTR>::GetValue() = "RET";
    AnyType<Z80::RET_C,    Z80::InstrSTR>::GetValue() = "RET C";             AnyType<Z80::EXX,       Z80::InstrSTR>::GetValue() = "EXX";
    AnyType<Z80::RET_PE,   Z80::InstrSTR>::GetValue() = "RET PE";            AnyType<Z80::JP_hl,     Z80::InstrSTR>::GetValue() = "JP (HL)";
    AnyType<Z80::RET_M,    Z80::InstrSTR>::GetValue() = "RET M";             AnyType<Z80::LD_SP_HL,  Z80::InstrSTR>::GetValue() = "LD SP, HL";


    AnyType<Z80::LD_A_bc,  Z80::InstrSTR>::GetValue() = "LD A, (BC)";        AnyType<Z80::DEC_BC,    Z80::InstrSTR>::GetValue() = "DEC BC";
    AnyType<Z80::LD_A_de,  Z80::InstrSTR>::GetValue() = "LD A, (DE)";        AnyType<Z80::DEC_DE,    Z80::InstrSTR>::GetValue() = "DEC DE";
    AnyType<Z80::LD_HL_nn, Z80::InstrSTR>::GetValue() = "LD HL, (0x%04x)";   AnyType<Z80::DEC_HL,    Z80::InstrSTR>::GetValue() = "DEC HL";
    AnyType<Z80::LD_A_nn,  Z80::InstrSTR>::GetValue() = "LD A, (0x%04x)";    AnyType<Z80::DEC_SP,    Z80::InstrSTR>::GetValue() = "DEC SP";
    AnyType<Z80::LD_C_D,   Z80::InstrSTR>::GetValue() = "LD C, D";           AnyType<Z80::LD_C_E,    Z80::InstrSTR>::GetValue() = "LD C, E";
    AnyType<Z80::LD_E_D,   Z80::InstrSTR>::GetValue() = "LD E, D";           AnyType<Z80::LD_E_E,    Z80::InstrSTR>::GetValue() = "LD E, E";
    AnyType<Z80::LD_L_D,   Z80::InstrSTR>::GetValue() = "LD L, D";           AnyType<Z80::LD_L_E,    Z80::InstrSTR>::GetValue() = "LD L, E";
    AnyType<Z80::LD_A_D,   Z80::InstrSTR>::GetValue() = "LD A, D";           AnyType<Z80::LD_A_E,    Z80::InstrSTR>::GetValue() = "LD A, E";
    AnyType<Z80::ADC_A_D,  Z80::InstrSTR>::GetValue() = "ADC A, D";          AnyType<Z80::ADC_A_E,   Z80::InstrSTR>::GetValue() = "ADC A, E";
    AnyType<Z80::SBC_A_D,  Z80::InstrSTR>::GetValue() = "SBC A, D";          AnyType<Z80::SBC_A_E,   Z80::InstrSTR>::GetValue() = "SBC A, E";
    AnyType<Z80::XOR_D,    Z80::InstrSTR>::GetValue() = "XOR D";             AnyType<Z80::XOR_E,     Z80::InstrSTR>::GetValue() = "XOR E";
    AnyType<Z80::CP_D,     Z80::InstrSTR>::GetValue() = "CP D";              AnyType<Z80::CP_E,      Z80::InstrSTR>::GetValue() = "CP E";
    AnyType<Z80::JP_Z_NN,  Z80::InstrSTR>::GetValue() = "JP Z, 0x%04x";      AnyType<Z80::BIT_INSTR, Z80::InstrSTR>::GetValue() = "";
    AnyType<Z80::JP_C_NN,  Z80::InstrSTR>::GetValue() = "JP C, 0x%04x";      AnyType<Z80::IN_A_n,    Z80::InstrSTR>::GetValue() = "IN A, (0x%02x)";
    AnyType<Z80::JP_PE_NN, Z80::InstrSTR>::GetValue() = "JP PE, 0x%04x";     AnyType<Z80::EX_DE_HL,  Z80::InstrSTR>::GetValue() = "EX DE, HL";
    AnyType<Z80::JP_M_NN,  Z80::InstrSTR>::GetValue() = "JP M, 0x%04x";      AnyType<Z80::EI,        Z80::InstrSTR>::GetValue() = "EI";


    AnyType<Z80::INC_C,      Z80::InstrSTR>::GetValue() = "INC C";           AnyType<Z80::DEC_C,      Z80::InstrSTR>::GetValue() = "DEC C";
    AnyType<Z80::INC_E,      Z80::InstrSTR>::GetValue() = "INC E";           AnyType<Z80::DEC_E,      Z80::InstrSTR>::GetValue() = "DEC E";
    AnyType<Z80::INC_L,      Z80::InstrSTR>::GetValue() = "INC L";           AnyType<Z80::DEC_L,      Z80::InstrSTR>::GetValue() = "DEC L";
    AnyType<Z80::INC_A,      Z80::InstrSTR>::GetValue() = "INC A";           AnyType<Z80::DEC_A,      Z80::InstrSTR>::GetValue() = "DEC A";
    AnyType<Z80::LD_C_H,     Z80::InstrSTR>::GetValue() = "LD C, H";         AnyType<Z80::LD_C_L,     Z80::InstrSTR>::GetValue() = "LD C, L";
    AnyType<Z80::LD_E_H,     Z80::InstrSTR>::GetValue() = "LD E, H";         AnyType<Z80::LD_E_L,     Z80::InstrSTR>::GetValue() = "LD E, L";
    AnyType<Z80::LD_L_H,     Z80::InstrSTR>::GetValue() = "LD L, H";         AnyType<Z80::LD_L_L,     Z80::InstrSTR>::GetValue() = "LD L, L";
    AnyType<Z80::LD_A_H,     Z80::InstrSTR>::GetValue() = "LD A, H";         AnyType<Z80::LD_A_L,     Z80::InstrSTR>::GetValue() = "LD A, L";
    AnyType<Z80::ADC_A_H,    Z80::InstrSTR>::GetValue() = "ADC A, H";        AnyType<Z80::ADC_A_L,    Z80::InstrSTR>::GetValue() = "ADC A, L";
    AnyType<Z80::SBC_A_H,    Z80::InstrSTR>::GetValue() = "SBC A, H";        AnyType<Z80::SBC_A_L,    Z80::InstrSTR>::GetValue() = "SBC A, L";
    AnyType<Z80::XOR_H,      Z80::InstrSTR>::GetValue() = "XOR H";           AnyType<Z80::XOR_L,      Z80::InstrSTR>::GetValue() = "XOR L";
    AnyType<Z80::CP_H,       Z80::InstrSTR>::GetValue() = "CP H";            AnyType<Z80::CP_L,       Z80::InstrSTR>::GetValue() = "CP L";
    AnyType<Z80::CALL_Z_NN,  Z80::InstrSTR>::GetValue() = "CALL Z, 0x%04x";  AnyType<Z80::CALL_NN,    Z80::InstrSTR>::GetValue() = "CALL 0x%04x";
    AnyType<Z80::CALL_C_NN,  Z80::InstrSTR>::GetValue() = "CALL C, 0x%04x";  AnyType<Z80::IX_INSTR,   Z80::InstrSTR>::GetValue() = "";
    AnyType<Z80::CALL_PE_NN, Z80::InstrSTR>::GetValue() = "CALL PE, 0x%04x"; AnyType<Z80::MISC_INSTR, Z80::InstrSTR>::GetValue() = "";
    AnyType<Z80::CALL_M_NN,  Z80::InstrSTR>::GetValue() = "CALL M, 0x%04x";  AnyType<Z80::IY_INSTR,   Z80::InstrSTR>::GetValue() = "";


    AnyType<Z80::LD_C_N,   Z80::InstrSTR>::GetValue() = "LD C, 0x%02x";      AnyType<Z80::RRCA,    Z80::InstrSTR>::GetValue() = "RRCA";
    AnyType<Z80::LD_E_N,   Z80::InstrSTR>::GetValue() = "LD E, 0x%02x";      AnyType<Z80::RRA,     Z80::InstrSTR>::GetValue() = "RRA";
    AnyType<Z80::LD_L_N,   Z80::InstrSTR>::GetValue() = "LD L, 0x%02x";      AnyType<Z80::CPL,     Z80::InstrSTR>::GetValue() = "CPL";
    AnyType<Z80::LD_A_N,   Z80::InstrSTR>::GetValue() = "LD A, 0x%02x";      AnyType<Z80::CCF,     Z80::InstrSTR>::GetValue() = "CCF";
    AnyType<Z80::LD_C_hl,  Z80::InstrSTR>::GetValue() = "LD C, (HL)";        AnyType<Z80::LD_C_A,  Z80::InstrSTR>::GetValue() = "LD C, A";
    AnyType<Z80::LD_E_hl,  Z80::InstrSTR>::GetValue() = "LD E, (HL)";        AnyType<Z80::LD_E_A,  Z80::InstrSTR>::GetValue() = "LD E, A";
    AnyType<Z80::LD_L_hl,  Z80::InstrSTR>::GetValue() = "LD L, (HL)";        AnyType<Z80::LD_L_A,  Z80::InstrSTR>::GetValue() = "LD L, A";
    AnyType<Z80::LD_A_hl,  Z80::InstrSTR>::GetValue() = "LD A, (HL)";        AnyType<Z80::LD_A_A,  Z80::InstrSTR>::GetValue() = "LD A, A";
    AnyType<Z80::ADC_A_hl, Z80::InstrSTR>::GetValue() = "ADC A, (HL)";       AnyType<Z80::ADC_A_A, Z80::InstrSTR>::GetValue() = "ADC A, A";
    AnyType<Z80::SBC_A_hl, Z80::InstrSTR>::GetValue() = "SBC A, (HL)";       AnyType<Z80::SBC_A_A, Z80::InstrSTR>::GetValue() = "SBC A, A";
    AnyType<Z80::XOR_hl,   Z80::InstrSTR>::GetValue() = "XOR (HL)";          AnyType<Z80::XOR_A,   Z80::InstrSTR>::GetValue() = "XOR A";
    AnyType<Z80::CP_hl,    Z80::InstrSTR>::GetValue() = "CP (HL)";           AnyType<Z80::CP_A,    Z80::InstrSTR>::GetValue() = "CP A";
    AnyType<Z80::ADC_A_N,  Z80::InstrSTR>::GetValue() = "ADC A, 0x%02x";     AnyType<Z80::RST_08h, Z80::InstrSTR>::GetValue() = "RST 0x08";
    AnyType<Z80::SBC_A_N,  Z80::InstrSTR>::GetValue() = "SBC A, 0x%02x";     AnyType<Z80::RST_18h, Z80::InstrSTR>::GetValue() = "RST 0x18";
    AnyType<Z80::XOR_N,    Z80::InstrSTR>::GetValue() = "XOR 0x%02x";        AnyType<Z80::RST_28h, Z80::InstrSTR>::GetValue() = "RST 0x28";
    AnyType<Z80::CP_N,     Z80::InstrSTR>::GetValue() = "CP 0x%02x";         AnyType<Z80::RST_38h, Z80::InstrSTR>::GetValue() = "RST 0x38";



    // BitInstructions
    AnyType<Z80::RLC_B,   Z80::BitSTR>::GetValue() = "RLC B";        AnyType<Z80::RLC_C,   Z80::BitSTR>::GetValue() = "RLC C";
    AnyType<Z80::RL_B,    Z80::BitSTR>::GetValue() = "RL B";         AnyType<Z80::RL_C,    Z80::BitSTR>::GetValue() = "RL C";
    AnyType<Z80::SLA_B,   Z80::BitSTR>::GetValue() = "SLA B";        AnyType<Z80::SLA_C,   Z80::BitSTR>::GetValue() = "SLA C";
    AnyType<Z80::SLL_B,   Z80::BitSTR>::GetValue() = "SLL B";        AnyType<Z80::SLL_C,   Z80::BitSTR>::GetValue() = "SLL C";
    AnyType<Z80::BIT_0_B, Z80::BitSTR>::GetValue() = "BIT 0, B";     AnyType<Z80::BIT_0_C, Z80::BitSTR>::GetValue() = "BIT 0, C";
    AnyType<Z80::BIT_2_B, Z80::BitSTR>::GetValue() = "BIT 2, B";     AnyType<Z80::BIT_2_C, Z80::BitSTR>::GetValue() = "BIT 2, C";
    AnyType<Z80::BIT_4_B, Z80::BitSTR>::GetValue() = "BIT 4, B";     AnyType<Z80::BIT_4_C, Z80::BitSTR>::GetValue() = "BIT 4, C";
    AnyType<Z80::BIT_6_B, Z80::BitSTR>::GetValue() = "BIT 6, B";     AnyType<Z80::BIT_6_C, Z80::BitSTR>::GetValue() = "BIT 6, C";
    AnyType<Z80::RES_0_B, Z80::BitSTR>::GetValue() = "RES 0, B";     AnyType<Z80::RES_0_C, Z80::BitSTR>::GetValue() = "RES 0, C";
    AnyType<Z80::RES_2_B, Z80::BitSTR>::GetValue() = "RES 2, B";     AnyType<Z80::RES_2_C, Z80::BitSTR>::GetValue() = "RES 2, C";
    AnyType<Z80::RES_4_B, Z80::BitSTR>::GetValue() = "RES 4, B";     AnyType<Z80::RES_4_C, Z80::BitSTR>::GetValue() = "RES 4, C";
    AnyType<Z80::RES_6_B, Z80::BitSTR>::GetValue() = "RES 6, B";     AnyType<Z80::RES_6_C, Z80::BitSTR>::GetValue() = "RES 6, C";
    AnyType<Z80::SET_0_B, Z80::BitSTR>::GetValue() = "SET 0, B";     AnyType<Z80::SET_0_C, Z80::BitSTR>::GetValue() = "SET 0, C";
    AnyType<Z80::SET_2_B, Z80::BitSTR>::GetValue() = "SET 2, B";     AnyType<Z80::SET_2_C, Z80::BitSTR>::GetValue() = "SET 2, C";
    AnyType<Z80::SET_4_B, Z80::BitSTR>::GetValue() = "SET 4, B";     AnyType<Z80::SET_4_C, Z80::BitSTR>::GetValue() = "SET 4, C";
    AnyType<Z80::SET_6_B, Z80::BitSTR>::GetValue() = "SET 6, B";     AnyType<Z80::SET_6_C, Z80::BitSTR>::GetValue() = "SET 6, C";


    AnyType<Z80::RLC_D,   Z80::BitSTR>::GetValue() = "RLC D";          AnyType<Z80::RLC_E,   Z80::BitSTR>::GetValue() = "RLC E";
    AnyType<Z80::RL_D,    Z80::BitSTR>::GetValue() = "RL D";           AnyType<Z80::RL_E,    Z80::BitSTR>::GetValue() = "RL E";
    AnyType<Z80::SLA_D,   Z80::BitSTR>::GetValue() = "SLA D";          AnyType<Z80::SLA_E,   Z80::BitSTR>::GetValue() = "SLA E";
    AnyType<Z80::SLL_D,   Z80::BitSTR>::GetValue() = "SLL D";          AnyType<Z80::SLL_E,   Z80::BitSTR>::GetValue() = "SLL E";
    AnyType<Z80::BIT_0_D, Z80::BitSTR>::GetValue() = "BIT 0, D";       AnyType<Z80::BIT_0_E, Z80::BitSTR>::GetValue() = "BIT 0, E";
    AnyType<Z80::BIT_2_D, Z80::BitSTR>::GetValue() = "BIT 2, D";       AnyType<Z80::BIT_2_E, Z80::BitSTR>::GetValue() = "BIT 2, E";
    AnyType<Z80::BIT_4_D, Z80::BitSTR>::GetValue() = "BIT 4, D";       AnyType<Z80::BIT_4_E, Z80::BitSTR>::GetValue() = "BIT 4, E";
    AnyType<Z80::BIT_6_D, Z80::BitSTR>::GetValue() = "BIT 6, D";       AnyType<Z80::BIT_6_E, Z80::BitSTR>::GetValue() = "BIT 6, E";
    AnyType<Z80::RES_0_D, Z80::BitSTR>::GetValue() = "RES 0, D";       AnyType<Z80::RES_0_E, Z80::BitSTR>::GetValue() = "RES 0, E";
    AnyType<Z80::RES_2_D, Z80::BitSTR>::GetValue() = "RES 2, D";       AnyType<Z80::RES_2_E, Z80::BitSTR>::GetValue() = "RES 2, E";
    AnyType<Z80::RES_4_D, Z80::BitSTR>::GetValue() = "RES 4, D";       AnyType<Z80::RES_4_E, Z80::BitSTR>::GetValue() = "RES 4, E";
    AnyType<Z80::RES_6_D, Z80::BitSTR>::GetValue() = "RES 6, D";       AnyType<Z80::RES_6_E, Z80::BitSTR>::GetValue() = "RES 6, E";
    AnyType<Z80::SET_0_D, Z80::BitSTR>::GetValue() = "SET 0, D";       AnyType<Z80::SET_0_E, Z80::BitSTR>::GetValue() = "SET 0, E";
    AnyType<Z80::SET_2_D, Z80::BitSTR>::GetValue() = "SET 2, D";       AnyType<Z80::SET_2_E, Z80::BitSTR>::GetValue() = "SET 2, E";
    AnyType<Z80::SET_4_D, Z80::BitSTR>::GetValue() = "SET 4, D";       AnyType<Z80::SET_4_E, Z80::BitSTR>::GetValue() = "SET 4, E";
    AnyType<Z80::SET_6_D, Z80::BitSTR>::GetValue() = "SET 6, D";       AnyType<Z80::SET_6_E, Z80::BitSTR>::GetValue() = "SET 6, E";


    AnyType<Z80::RLC_H,   Z80::BitSTR>::GetValue() = "RLC H";          AnyType<Z80::RLC_L,   Z80::BitSTR>::GetValue() = "RLC L";
    AnyType<Z80::RL_H,    Z80::BitSTR>::GetValue() = "RL H";           AnyType<Z80::RL_L,    Z80::BitSTR>::GetValue() = "RL L";
    AnyType<Z80::SLA_H,   Z80::BitSTR>::GetValue() = "SLA H";          AnyType<Z80::SLA_L,   Z80::BitSTR>::GetValue() = "SLA L";
    AnyType<Z80::SLL_H,   Z80::BitSTR>::GetValue() = "SLL H";          AnyType<Z80::SLL_L,   Z80::BitSTR>::GetValue() = "SLL L";
    AnyType<Z80::BIT_0_H, Z80::BitSTR>::GetValue() = "BIT 0, H";       AnyType<Z80::BIT_0_L, Z80::BitSTR>::GetValue() = "BIT 0, L";
    AnyType<Z80::BIT_2_H, Z80::BitSTR>::GetValue() = "BIT 2, H";       AnyType<Z80::BIT_2_L, Z80::BitSTR>::GetValue() = "BIT 2, L";
    AnyType<Z80::BIT_4_H, Z80::BitSTR>::GetValue() = "BIT 4, H";       AnyType<Z80::BIT_4_L, Z80::BitSTR>::GetValue() = "BIT 4, L";
    AnyType<Z80::BIT_6_H, Z80::BitSTR>::GetValue() = "BIT 6, H";       AnyType<Z80::BIT_6_L, Z80::BitSTR>::GetValue() = "BIT 6, L";
    AnyType<Z80::RES_0_H, Z80::BitSTR>::GetValue() = "RES 0, H";       AnyType<Z80::RES_0_L, Z80::BitSTR>::GetValue() = "RES 0, L";
    AnyType<Z80::RES_2_H, Z80::BitSTR>::GetValue() = "RES 2, H";       AnyType<Z80::RES_2_L, Z80::BitSTR>::GetValue() = "RES 2, L";
    AnyType<Z80::RES_4_H, Z80::BitSTR>::GetValue() = "RES 4, H";       AnyType<Z80::RES_4_L, Z80::BitSTR>::GetValue() = "RES 4, L";
    AnyType<Z80::RES_6_H, Z80::BitSTR>::GetValue() = "RES 6, H";       AnyType<Z80::RES_6_L, Z80::BitSTR>::GetValue() = "RES 6, L";
    AnyType<Z80::SET_0_H, Z80::BitSTR>::GetValue() = "SET 0, H";       AnyType<Z80::SET_0_L, Z80::BitSTR>::GetValue() = "SET 0, L";
    AnyType<Z80::SET_2_H, Z80::BitSTR>::GetValue() = "SET 2, H";       AnyType<Z80::SET_2_L, Z80::BitSTR>::GetValue() = "SET 2, L";
    AnyType<Z80::SET_4_H, Z80::BitSTR>::GetValue() = "SET 4, H";       AnyType<Z80::SET_4_L, Z80::BitSTR>::GetValue() = "SET 4, L";
    AnyType<Z80::SET_6_H, Z80::BitSTR>::GetValue() = "SET 6, H";       AnyType<Z80::SET_6_L, Z80::BitSTR>::GetValue() = "SET 6, L";


    AnyType<Z80::RLC_hl,   Z80::BitSTR>::GetValue() = "RLC (HL)";      AnyType<Z80::RLC_A,   Z80::BitSTR>::GetValue() = "RLC A";
    AnyType<Z80::RL_hl,    Z80::BitSTR>::GetValue() = "RL (HL)";       AnyType<Z80::RL_A,    Z80::BitSTR>::GetValue() = "RL A";
    AnyType<Z80::SLA_hl,   Z80::BitSTR>::GetValue() = "SLA (HL)";      AnyType<Z80::SLA_A,   Z80::BitSTR>::GetValue() = "SLA A";
    AnyType<Z80::SLL_hl,   Z80::BitSTR>::GetValue() = "SLL (HL)";      AnyType<Z80::SLL_A,   Z80::BitSTR>::GetValue() = "SLL A";
    AnyType<Z80::BIT_0_hl, Z80::BitSTR>::GetValue() = "BIT 0, (HL)";   AnyType<Z80::BIT_0_A, Z80::BitSTR>::GetValue() = "BIT 0, A";
    AnyType<Z80::BIT_2_hl, Z80::BitSTR>::GetValue() = "BIT 2, (HL)";   AnyType<Z80::BIT_2_A, Z80::BitSTR>::GetValue() = "BIT 2, A";
    AnyType<Z80::BIT_4_hl, Z80::BitSTR>::GetValue() = "BIT 4, (HL)";   AnyType<Z80::BIT_4_A, Z80::BitSTR>::GetValue() = "BIT 4, A";
    AnyType<Z80::BIT_6_hl, Z80::BitSTR>::GetValue() = "BIT 6, (HL)";   AnyType<Z80::BIT_6_A, Z80::BitSTR>::GetValue() = "BIT 6, A";
    AnyType<Z80::RES_0_hl, Z80::BitSTR>::GetValue() = "RES 0, (HL)";   AnyType<Z80::RES_0_A, Z80::BitSTR>::GetValue() = "RES 0, A";
    AnyType<Z80::RES_2_hl, Z80::BitSTR>::GetValue() = "RES 2, (HL)";   AnyType<Z80::RES_2_A, Z80::BitSTR>::GetValue() = "RES 2, A";
    AnyType<Z80::RES_4_hl, Z80::BitSTR>::GetValue() = "RES 4, (HL)";   AnyType<Z80::RES_4_A, Z80::BitSTR>::GetValue() = "RES 4, A";
    AnyType<Z80::RES_6_hl, Z80::BitSTR>::GetValue() = "RES 6, (HL)";   AnyType<Z80::RES_6_A, Z80::BitSTR>::GetValue() = "RES 6, A";
    AnyType<Z80::SET_0_hl, Z80::BitSTR>::GetValue() = "SET 0, (HL)";   AnyType<Z80::SET_0_A, Z80::BitSTR>::GetValue() = "SET 0, A";
    AnyType<Z80::SET_2_hl, Z80::BitSTR>::GetValue() = "SET 2, (HL)";   AnyType<Z80::SET_2_A, Z80::BitSTR>::GetValue() = "SET 2, A";
    AnyType<Z80::SET_4_hl, Z80::BitSTR>::GetValue() = "SET 4, (HL)";   AnyType<Z80::SET_4_A, Z80::BitSTR>::GetValue() = "SET 4, A";
    AnyType<Z80::SET_6_hl, Z80::BitSTR>::GetValue() = "SET 6, (HL)";   AnyType<Z80::SET_6_A, Z80::BitSTR>::GetValue() = "SET 6, A";


    AnyType<Z80::RRC_B,   Z80::BitSTR>::GetValue() = "RRC B";          AnyType<Z80::RRC_C,   Z80::BitSTR>::GetValue() = "RRC C";
    AnyType<Z80::RR_B,    Z80::BitSTR>::GetValue() = "RR B";           AnyType<Z80::RR_C,    Z80::BitSTR>::GetValue() = "RR C";
    AnyType<Z80::SRA_B,   Z80::BitSTR>::GetValue() = "SRA B";          AnyType<Z80::SRA_C,   Z80::BitSTR>::GetValue() = "SRA C";
    AnyType<Z80::SRL_B,   Z80::BitSTR>::GetValue() = "SRL B";          AnyType<Z80::SRL_C,   Z80::BitSTR>::GetValue() = "SRL C";
    AnyType<Z80::BIT_1_B, Z80::BitSTR>::GetValue() = "BIT 1, B";       AnyType<Z80::BIT_1_C, Z80::BitSTR>::GetValue() = "BIT 1, C";
    AnyType<Z80::BIT_3_B, Z80::BitSTR>::GetValue() = "BIT 3, B";       AnyType<Z80::BIT_3_C, Z80::BitSTR>::GetValue() = "BIT 3, C";
    AnyType<Z80::BIT_5_B, Z80::BitSTR>::GetValue() = "BIT 5, B";       AnyType<Z80::BIT_5_C, Z80::BitSTR>::GetValue() = "BIT 5, C";
    AnyType<Z80::BIT_7_B, Z80::BitSTR>::GetValue() = "BIT 7, B";       AnyType<Z80::BIT_7_C, Z80::BitSTR>::GetValue() = "BIT 7, C";
    AnyType<Z80::RES_1_B, Z80::BitSTR>::GetValue() = "RES 1, B";       AnyType<Z80::RES_1_C, Z80::BitSTR>::GetValue() = "RES 1, C";
    AnyType<Z80::RES_3_B, Z80::BitSTR>::GetValue() = "RES 3, B";       AnyType<Z80::RES_3_C, Z80::BitSTR>::GetValue() = "RES 3, C";
    AnyType<Z80::RES_5_B, Z80::BitSTR>::GetValue() = "RES 5, B";       AnyType<Z80::RES_5_C, Z80::BitSTR>::GetValue() = "RES 5, C";
    AnyType<Z80::RES_7_B, Z80::BitSTR>::GetValue() = "RES 7, B";       AnyType<Z80::RES_7_C, Z80::BitSTR>::GetValue() = "RES 7, C";
    AnyType<Z80::SET_1_B, Z80::BitSTR>::GetValue() = "SET 1, B";       AnyType<Z80::SET_1_C, Z80::BitSTR>::GetValue() = "SET 1, C";
    AnyType<Z80::SET_3_B, Z80::BitSTR>::GetValue() = "SET 3, B";       AnyType<Z80::SET_3_C, Z80::BitSTR>::GetValue() = "SET 3, C";
    AnyType<Z80::SET_5_B, Z80::BitSTR>::GetValue() = "SET 5, B";       AnyType<Z80::SET_5_C, Z80::BitSTR>::GetValue() = "SET 5, C";
    AnyType<Z80::SET_7_B, Z80::BitSTR>::GetValue() = "SET 7, B";       AnyType<Z80::SET_7_C, Z80::BitSTR>::GetValue() = "SET 7, C";


    AnyType<Z80::RRC_D,   Z80::BitSTR>::GetValue() = "RRC D";          AnyType<Z80::RRC_E,   Z80::BitSTR>::GetValue() = "RRC E";
    AnyType<Z80::RR_D,    Z80::BitSTR>::GetValue() = "RR D";           AnyType<Z80::RR_E,    Z80::BitSTR>::GetValue() = "RR E";
    AnyType<Z80::SRA_D,   Z80::BitSTR>::GetValue() = "SRA D";          AnyType<Z80::SRA_E,   Z80::BitSTR>::GetValue() = "SRA E";
    AnyType<Z80::SRL_D,   Z80::BitSTR>::GetValue() = "SRL D";          AnyType<Z80::SRL_E,   Z80::BitSTR>::GetValue() = "SRL E";
    AnyType<Z80::BIT_1_D, Z80::BitSTR>::GetValue() = "BIT 1, D";       AnyType<Z80::BIT_1_E, Z80::BitSTR>::GetValue() = "BIT 1, E";
    AnyType<Z80::BIT_3_D, Z80::BitSTR>::GetValue() = "BIT 3, D";       AnyType<Z80::BIT_3_E, Z80::BitSTR>::GetValue() = "BIT 3, E";
    AnyType<Z80::BIT_5_D, Z80::BitSTR>::GetValue() = "BIT 5, D";       AnyType<Z80::BIT_5_E, Z80::BitSTR>::GetValue() = "BIT 5, E";
    AnyType<Z80::BIT_7_D, Z80::BitSTR>::GetValue() = "BIT 7, D";       AnyType<Z80::BIT_7_E, Z80::BitSTR>::GetValue() = "BIT 7, E";
    AnyType<Z80::RES_1_D, Z80::BitSTR>::GetValue() = "RES 1, D";       AnyType<Z80::RES_1_E, Z80::BitSTR>::GetValue() = "RES 1, E";
    AnyType<Z80::RES_3_D, Z80::BitSTR>::GetValue() = "RES 3, D";       AnyType<Z80::RES_3_E, Z80::BitSTR>::GetValue() = "RES 3, E";
    AnyType<Z80::RES_5_D, Z80::BitSTR>::GetValue() = "RES 5, D";       AnyType<Z80::RES_5_E, Z80::BitSTR>::GetValue() = "RES 5, E";
    AnyType<Z80::RES_7_D, Z80::BitSTR>::GetValue() = "RES 7, D";       AnyType<Z80::RES_7_E, Z80::BitSTR>::GetValue() = "RES 7, E";
    AnyType<Z80::SET_1_D, Z80::BitSTR>::GetValue() = "SET 1, D";       AnyType<Z80::SET_1_E, Z80::BitSTR>::GetValue() = "SET 1, E";
    AnyType<Z80::SET_3_D, Z80::BitSTR>::GetValue() = "SET 3, D";       AnyType<Z80::SET_3_E, Z80::BitSTR>::GetValue() = "SET 3, E";
    AnyType<Z80::SET_5_D, Z80::BitSTR>::GetValue() = "SET 5, D";       AnyType<Z80::SET_5_E, Z80::BitSTR>::GetValue() = "SET 5, E";
    AnyType<Z80::SET_7_D, Z80::BitSTR>::GetValue() = "SET 7, D";       AnyType<Z80::SET_7_E, Z80::BitSTR>::GetValue() = "SET 7, E";

    AnyType<Z80::RRC_H,   Z80::BitSTR>::GetValue() = "RRC H";          AnyType<Z80::RRC_L,   Z80::BitSTR>::GetValue() = "RRC L";
    AnyType<Z80::RR_H,    Z80::BitSTR>::GetValue() = "RR H";           AnyType<Z80::RR_L,    Z80::BitSTR>::GetValue() = "RR L";
    AnyType<Z80::SRA_H,   Z80::BitSTR>::GetValue() = "SRA H";          AnyType<Z80::SRA_L,   Z80::BitSTR>::GetValue() = "SRA L";
    AnyType<Z80::SRL_H,   Z80::BitSTR>::GetValue() = "SRL H";          AnyType<Z80::SRL_L,   Z80::BitSTR>::GetValue() = "SRL L";
    AnyType<Z80::BIT_1_H, Z80::BitSTR>::GetValue() = "BIT 1, H";       AnyType<Z80::BIT_1_L, Z80::BitSTR>::GetValue() = "BIT 1, L";
    AnyType<Z80::BIT_3_H, Z80::BitSTR>::GetValue() = "BIT 3, H";       AnyType<Z80::BIT_3_L, Z80::BitSTR>::GetValue() = "BIT 3, L";
    AnyType<Z80::BIT_5_H, Z80::BitSTR>::GetValue() = "BIT 5, H";       AnyType<Z80::BIT_5_L, Z80::BitSTR>::GetValue() = "BIT 5, L";
    AnyType<Z80::BIT_7_H, Z80::BitSTR>::GetValue() = "BIT 7, H";       AnyType<Z80::BIT_7_L, Z80::BitSTR>::GetValue() = "BIT 7, L";
    AnyType<Z80::RES_1_H, Z80::BitSTR>::GetValue() = "RES 1, H";       AnyType<Z80::RES_1_L, Z80::BitSTR>::GetValue() = "RES 1, L";
    AnyType<Z80::RES_3_H, Z80::BitSTR>::GetValue() = "RES 3, H";       AnyType<Z80::RES_3_L, Z80::BitSTR>::GetValue() = "RES 3, L";
    AnyType<Z80::RES_5_H, Z80::BitSTR>::GetValue() = "RES 5, H";       AnyType<Z80::RES_5_L, Z80::BitSTR>::GetValue() = "RES 5, L";
    AnyType<Z80::RES_7_H, Z80::BitSTR>::GetValue() = "RES 7, H";       AnyType<Z80::RES_7_L, Z80::BitSTR>::GetValue() = "RES 7, L";
    AnyType<Z80::SET_1_H, Z80::BitSTR>::GetValue() = "SET 1, H";       AnyType<Z80::SET_1_L, Z80::BitSTR>::GetValue() = "SET 1, L";
    AnyType<Z80::SET_3_H, Z80::BitSTR>::GetValue() = "SET 3, H";       AnyType<Z80::SET_3_L, Z80::BitSTR>::GetValue() = "SET 3, L";
    AnyType<Z80::SET_5_H, Z80::BitSTR>::GetValue() = "SET 5, H";       AnyType<Z80::SET_5_L, Z80::BitSTR>::GetValue() = "SET 5, L";
    AnyType<Z80::SET_7_H, Z80::BitSTR>::GetValue() = "SET 7, H";       AnyType<Z80::SET_7_L, Z80::BitSTR>::GetValue() = "SET 7, L";


    AnyType<Z80::RRC_hl,   Z80::BitSTR>::GetValue() = "RRC (HL)";      AnyType<Z80::RRC_A,   Z80::BitSTR>::GetValue() = "RRC A";
    AnyType<Z80::RR_hl,    Z80::BitSTR>::GetValue() = "RR (HL)";       AnyType<Z80::RR_A,    Z80::BitSTR>::GetValue() = "RR A";
    AnyType<Z80::SRA_hl,   Z80::BitSTR>::GetValue() = "SRA (HL)";      AnyType<Z80::SRA_A,   Z80::BitSTR>::GetValue() = "SRA A";
    AnyType<Z80::SRL_hl,   Z80::BitSTR>::GetValue() = "SRL (HL)";      AnyType<Z80::SRL_A,   Z80::BitSTR>::GetValue() = "SRL A";
    AnyType<Z80::BIT_1_hl, Z80::BitSTR>::GetValue() = "BIT 1, (HL)";   AnyType<Z80::BIT_1_A, Z80::BitSTR>::GetValue() = "BIT 1, A";
    AnyType<Z80::BIT_3_hl, Z80::BitSTR>::GetValue() = "BIT 3, (HL)";   AnyType<Z80::BIT_3_A, Z80::BitSTR>::GetValue() = "BIT 3, A";
    AnyType<Z80::BIT_5_hl, Z80::BitSTR>::GetValue() = "BIT 5, (HL)";   AnyType<Z80::BIT_5_A, Z80::BitSTR>::GetValue() = "BIT 5, A";
    AnyType<Z80::BIT_7_hl, Z80::BitSTR>::GetValue() = "BIT 7, (HL)";   AnyType<Z80::BIT_7_A, Z80::BitSTR>::GetValue() = "BIT 7, A";
    AnyType<Z80::RES_1_hl, Z80::BitSTR>::GetValue() = "RES 1, (HL)";   AnyType<Z80::RES_1_A, Z80::BitSTR>::GetValue() = "RES 1, A";
    AnyType<Z80::RES_3_hl, Z80::BitSTR>::GetValue() = "RES 3, (HL)";   AnyType<Z80::RES_3_A, Z80::BitSTR>::GetValue() = "RES 3, A";
    AnyType<Z80::RES_5_hl, Z80::BitSTR>::GetValue() = "RES 5, (HL)";   AnyType<Z80::RES_5_A, Z80::BitSTR>::GetValue() = "RES 5, A";
    AnyType<Z80::RES_7_hl, Z80::BitSTR>::GetValue() = "RES 7, (HL)";   AnyType<Z80::RES_7_A, Z80::BitSTR>::GetValue() = "RES 7, A";
    AnyType<Z80::SET_1_hl, Z80::BitSTR>::GetValue() = "SET 1, (HL)";   AnyType<Z80::SET_1_A, Z80::BitSTR>::GetValue() = "SET 1, A";
    AnyType<Z80::SET_3_hl, Z80::BitSTR>::GetValue() = "SET 3, (HL)";   AnyType<Z80::SET_3_A, Z80::BitSTR>::GetValue() = "SET 3, A";
    AnyType<Z80::SET_5_hl, Z80::BitSTR>::GetValue() = "SET 5, (HL)";   AnyType<Z80::SET_5_A, Z80::BitSTR>::GetValue() = "SET 5, A";
    AnyType<Z80::SET_7_hl, Z80::BitSTR>::GetValue() = "SET 7, (HL)";   AnyType<Z80::SET_7_A, Z80::BitSTR>::GetValue() = "SET 7, A";


    // IxInstructions
    AnyType<Z80::ADD_IX_BC, Z80::IxSTR>::GetValue() = "ADD IX, BC";             AnyType<Z80::ADD_IX_DE, Z80::IxSTR>::GetValue() = "ADD IX, DE";
    AnyType<Z80::INC_ixd,   Z80::IxSTR>::GetValue() = "INC (IX+0x%02x)";        AnyType<Z80::DEC_ixd,   Z80::IxSTR>::GetValue() = "DEC (IX+%02x)";
    AnyType<Z80::LD_ixd_B,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), B";      AnyType<Z80::LD_ixd_C,  Z80::IxSTR>::GetValue() = "LD (IX+%02x), C";
    AnyType<Z80::ADD_A_ixd, Z80::IxSTR>::GetValue() = "ADD A, (IX+0x%02x)";     AnyType<Z80::ADC_A_ixd, Z80::IxSTR>::GetValue() = "ADC A, (IX+%02x)";
    AnyType<Z80::POP_IX,    Z80::IxSTR>::GetValue() = "POP IX";                 AnyType<Z80::EX_sp_IX,  Z80::IxSTR>::GetValue() = "EX (SP), IX";
    AnyType<Z80::LD_IX_NN,  Z80::IxSTR>::GetValue() = "LD IX, 0x%04x";          AnyType<Z80::LD_nn_IX,  Z80::IxSTR>::GetValue() = "LD (0x%04x), IX";
    AnyType<Z80::LD_ixd_N,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), 0x%04x"; AnyType<Z80::ADD_IX_SP, Z80::IxSTR>::GetValue() = "ADD IX, SP";
    AnyType<Z80::LD_ixd_D,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), D";      AnyType<Z80::LD_ixd_E,  Z80::IxSTR>::GetValue() = "LD (IX+%02x), E";
    AnyType<Z80::SUB_ixd,   Z80::IxSTR>::GetValue() = "SUB (IX+0x%02x)";        AnyType<Z80::SBC_A_ixd, Z80::IxSTR>::GetValue() = "SBC A, (IX+%02x)";
    AnyType<Z80::PUSH_IX,   Z80::IxSTR>::GetValue() = "PUSH IX";                AnyType<Z80::JP_ix,     Z80::IxSTR>::GetValue() = "JP (IX)";
    AnyType<Z80::INC_IX,    Z80::IxSTR>::GetValue() = "INC IX";                 AnyType<Z80::ADD_IX_IX, Z80::IxSTR>::GetValue() = "ADD IX, IX";
    AnyType<Z80::LD_B_ixd,  Z80::IxSTR>::GetValue() = "LD B, (IX+0x%02x)";      AnyType<Z80::LD_C_ixd,  Z80::IxSTR>::GetValue() = "LD C, (IX+%02x)";
    AnyType<Z80::LD_ixd_H,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), H";      AnyType<Z80::LD_ixd_L,  Z80::IxSTR>::GetValue() = "LD (IX+%02x), L";
    AnyType<Z80::AND_ixd,   Z80::IxSTR>::GetValue() = "AND (IX+0x%02x)";        AnyType<Z80::XOR_ixd,   Z80::IxSTR>::GetValue() = "XOR (IX+%02x)";
    AnyType<Z80::LD_IX_nn,  Z80::IxSTR>::GetValue() = "LD IX, 0x%04x";          AnyType<Z80::DEC_IX,    Z80::IxSTR>::GetValue() = "DEC IX";
    AnyType<Z80::LD_D_ixd,  Z80::IxSTR>::GetValue() = "LD D, (IX+0x%02x)";      AnyType<Z80::LD_E_ixd,  Z80::IxSTR>::GetValue() = "LD E, (IX+%02x)";
    AnyType<Z80::LD_H_ixd,  Z80::IxSTR>::GetValue() = "LD H, (IX+0x%02x)";      AnyType<Z80::LD_L_ixd,  Z80::IxSTR>::GetValue() = "LD L, (IX+%02x)";
    AnyType<Z80::LD_ixd_A,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), A";      AnyType<Z80::LD_A_ixd,  Z80::IxSTR>::GetValue() = "LD A, (IX+%02x)";
    AnyType<Z80::OR_ixd,    Z80::IxSTR>::GetValue() = "OR (IX+0x%02x)";         AnyType<Z80::CP_ixd,    Z80::IxSTR>::GetValue() = "CP (IX+%02x)";
    AnyType<Z80::LD_SP_IX,  Z80::IxSTR>::GetValue() = "LD SP, IX";              AnyType<Z80::IxBitInstr,Z80::IxSTR>::GetValue() = "";


    AnyType<Z80::RLC_ixd,   Z80::IxBitSTR>::GetValue() = "RLC (IX+0x%02x)";     AnyType<Z80::RRC_ixd,   Z80::IxBitSTR>::GetValue() = "RRC (IX+%02x)";
    AnyType<Z80::RL_ixd,    Z80::IxBitSTR>::GetValue() = "RL (IX+0x%02x)";      AnyType<Z80::RR_ixd,    Z80::IxBitSTR>::GetValue() = "RR (IX+%02x)";
    AnyType<Z80::SLA_ixd,   Z80::IxBitSTR>::GetValue() = "SLA (IX+0x%02x)";     AnyType<Z80::SRA_ixd,   Z80::IxBitSTR>::GetValue() = "SRA (IX+%02x)";
    AnyType<Z80::SLL_ixd,   Z80::IxBitSTR>::GetValue() = "SLL (IX+0x%02x)";     AnyType<Z80::SRL_ixd,   Z80::IxBitSTR>::GetValue() = "SRL (IX+%02x)";
    AnyType<Z80::BIT_0_ixd, Z80::IxBitSTR>::GetValue() = "BIT 0, (IX+0x%02x)";  AnyType<Z80::BIT_1_ixd, Z80::IxBitSTR>::GetValue() = "BIT 1, (IX+%02x)";
    AnyType<Z80::RES_0_ixd, Z80::IxBitSTR>::GetValue() = "RES 0, (IX+0x%02x)";  AnyType<Z80::RES_1_ixd, Z80::IxBitSTR>::GetValue() = "RES 1, (IX+%02x)";
    AnyType<Z80::SET_0_ixd, Z80::IxBitSTR>::GetValue() = "SET 0, (IX+0x%02x)";  AnyType<Z80::SET_1_ixd, Z80::IxBitSTR>::GetValue() = "SET 1, (IX+%02x)";
    AnyType<Z80::BIT_2_ixd, Z80::IxBitSTR>::GetValue() = "BIT 2, (IX+0x%02x)";  AnyType<Z80::BIT_3_ixd, Z80::IxBitSTR>::GetValue() = "BIT 3, (IX+%02x)";
    AnyType<Z80::RES_2_ixd, Z80::IxBitSTR>::GetValue() = "RES 2, (IX+0x%02x)";  AnyType<Z80::RES_3_ixd, Z80::IxBitSTR>::GetValue() = "RES 3, (IX+%02x)";
    AnyType<Z80::SET_2_ixd, Z80::IxBitSTR>::GetValue() = "SET 2, (IX+0x%02x)";  AnyType<Z80::SET_3_ixd, Z80::IxBitSTR>::GetValue() = "SET 3, (IX+%02x)";
    AnyType<Z80::BIT_4_ixd, Z80::IxBitSTR>::GetValue() = "BIT 4, (IX+0x%02x)";  AnyType<Z80::BIT_5_ixd, Z80::IxBitSTR>::GetValue() = "BIT 5, (IX+%02x)";
    AnyType<Z80::RES_4_ixd, Z80::IxBitSTR>::GetValue() = "RES 4, (IX+0x%02x)";  AnyType<Z80::RES_5_ixd, Z80::IxBitSTR>::GetValue() = "RES 5, (IX+%02x)";
    AnyType<Z80::SET_4_ixd, Z80::IxBitSTR>::GetValue() = "SET 4, (IX+0x%02x)";  AnyType<Z80::SET_5_ixd, Z80::IxBitSTR>::GetValue() = "SET 5, (IX+%02x)";
    AnyType<Z80::BIT_6_ixd, Z80::IxBitSTR>::GetValue() = "BIT 6, (IX+0x%02x)";  AnyType<Z80::BIT_7_ixd, Z80::IxBitSTR>::GetValue() = "BIT 7, (IX+%02x)";
    AnyType<Z80::RES_6_ixd, Z80::IxBitSTR>::GetValue() = "RES 6, (IX+0x%02x)";  AnyType<Z80::RES_7_ixd, Z80::IxBitSTR>::GetValue() = "RES 7, (IX+%02x)";
    AnyType<Z80::SET_6_ixd, Z80::IxBitSTR>::GetValue() = "SET 6, (IX+0x%02x)";  AnyType<Z80::SET_7_ixd, Z80::IxBitSTR>::GetValue() = "SET 7, (IX+%02x)";


    // IyInstructions
    AnyType<Z80::ADD_IY_BC, Z80::IySTR>::GetValue() = "ADD IY, BC";             AnyType<Z80::ADD_IY_DE, Z80::IySTR>::GetValue() = "ADD IY, DE";
    AnyType<Z80::INC_iyd,   Z80::IySTR>::GetValue() = "INC (IY+0x%02x)";        AnyType<Z80::DEC_iyd,   Z80::IySTR>::GetValue() = "DEC (IY+%02x)";
    AnyType<Z80::LD_iyd_B,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), B";      AnyType<Z80::LD_iyd_C,  Z80::IySTR>::GetValue() = "LD (IY+%02x), C";
    AnyType<Z80::ADD_A_iyd, Z80::IySTR>::GetValue() = "ADD A, (IY+0x%02x)";     AnyType<Z80::ADC_A_iyd, Z80::IySTR>::GetValue() = "ADC A, (IY+%02x)";
    AnyType<Z80::POP_IY,    Z80::IySTR>::GetValue() = "POP IY";                 AnyType<Z80::EX_sp_IY,  Z80::IySTR>::GetValue() = "EX (SP), IY";
    AnyType<Z80::LD_IY_NN,  Z80::IySTR>::GetValue() = "LD IY, 0x%04x";          AnyType<Z80::LD_nn_IY,  Z80::IySTR>::GetValue() = "LD (0x%04x), IY";
    AnyType<Z80::LD_iyd_N,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), 0x%04x"; AnyType<Z80::ADD_IY_SP, Z80::IySTR>::GetValue() = "ADD IY, SP";
    AnyType<Z80::LD_iyd_D,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), D";      AnyType<Z80::LD_iyd_E,  Z80::IySTR>::GetValue() = "LD (IY+%02x), E";
    AnyType<Z80::SUB_iyd,   Z80::IySTR>::GetValue() = "SUB (IY+0x%02x)";        AnyType<Z80::SBC_A_iyd, Z80::IySTR>::GetValue() = "SBC A, (IY+%02x)";
    AnyType<Z80::PUSH_IY,   Z80::IySTR>::GetValue() = "PUSH IY";                AnyType<Z80::JP_iy,     Z80::IySTR>::GetValue() = "JP (IY)";
    AnyType<Z80::INC_IY,    Z80::IySTR>::GetValue() = "INC IY";                 AnyType<Z80::ADD_IY_IY, Z80::IySTR>::GetValue() = "ADD IY, IY";
    AnyType<Z80::LD_B_iyd,  Z80::IySTR>::GetValue() = "LD B, (IY+0x%02x)";      AnyType<Z80::LD_C_iyd,  Z80::IySTR>::GetValue() = "LD C, (IY+%02x)";
    AnyType<Z80::LD_iyd_H,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), H";      AnyType<Z80::LD_iyd_L,  Z80::IySTR>::GetValue() = "LD (IY+%02x), L";
    AnyType<Z80::AND_iyd,   Z80::IySTR>::GetValue() = "AND (IY+0x%02x)";        AnyType<Z80::XOR_iyd,   Z80::IySTR>::GetValue() = "XOR (IY+%02x)";
    AnyType<Z80::LD_IY_nn,  Z80::IySTR>::GetValue() = "LD IY, 0x%04x";          AnyType<Z80::DEC_IY,    Z80::IySTR>::GetValue() = "DEC IY";
    AnyType<Z80::LD_D_iyd,  Z80::IySTR>::GetValue() = "LD D, (IY+0x%02x)";      AnyType<Z80::LD_E_iyd,  Z80::IySTR>::GetValue() = "LD E, (IY+%02x)";
    AnyType<Z80::LD_H_iyd,  Z80::IySTR>::GetValue() = "LD H, (IY+0x%02x)";      AnyType<Z80::LD_L_iyd,  Z80::IySTR>::GetValue() = "LD L, (IY+%02x)";
    AnyType<Z80::LD_iyd_A,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), A";      AnyType<Z80::LD_A_iyd,  Z80::IySTR>::GetValue() = "LD A, (IY+%02x)";
    AnyType<Z80::OR_iyd,    Z80::IySTR>::GetValue() = "OR (IY+0x%02x)";         AnyType<Z80::CP_iyd,    Z80::IySTR>::GetValue() = "CP (IY+%02x)";
    AnyType<Z80::LD_SP_IY,  Z80::IySTR>::GetValue() = "LD SP, IY";              AnyType<Z80::IyBitInstr,Z80::IySTR>::GetValue() = "";

    AnyType<Z80::RLC_iyd,   Z80::IyBitSTR>::GetValue() = "RLC (IY+0x%02x)";     AnyType<Z80::RRC_iyd,   Z80::IyBitSTR>::GetValue() = "RRC (IY+%02x)";
    AnyType<Z80::RL_iyd,    Z80::IyBitSTR>::GetValue() = "RL (IY+0x%02x)";      AnyType<Z80::RR_iyd,    Z80::IyBitSTR>::GetValue() = "RR (IY+%02x)";
    AnyType<Z80::SLA_iyd,   Z80::IyBitSTR>::GetValue() = "SLA (IY+0x%02x)";     AnyType<Z80::SRA_iyd,   Z80::IyBitSTR>::GetValue() = "SRA (IY+%02x)";
    AnyType<Z80::SLL_iyd,   Z80::IyBitSTR>::GetValue() = "SLL (IY+0x%02x)";     AnyType<Z80::SRL_iyd,   Z80::IyBitSTR>::GetValue() = "SRL (IY+%02x)";
    AnyType<Z80::BIT_0_iyd, Z80::IyBitSTR>::GetValue() = "BIT 0, (IY+0x%02x)";  AnyType<Z80::BIT_1_iyd, Z80::IyBitSTR>::GetValue() = "BIT 1, (IY+%02x)";
    AnyType<Z80::RES_0_iyd, Z80::IyBitSTR>::GetValue() = "RES 0, (IY+0x%02x)";  AnyType<Z80::RES_1_iyd, Z80::IyBitSTR>::GetValue() = "RES 1, (IY+%02x)";
    AnyType<Z80::SET_0_iyd, Z80::IyBitSTR>::GetValue() = "SET 0, (IY+0x%02x)";  AnyType<Z80::SET_1_iyd, Z80::IyBitSTR>::GetValue() = "SET 1, (IY+%02x)";
    AnyType<Z80::BIT_2_iyd, Z80::IyBitSTR>::GetValue() = "BIT 2, (IY+0x%02x)";  AnyType<Z80::BIT_3_iyd, Z80::IyBitSTR>::GetValue() = "BIT 3, (IY+%02x)";
    AnyType<Z80::RES_2_iyd, Z80::IyBitSTR>::GetValue() = "RES 2, (IY+0x%02x)";  AnyType<Z80::RES_3_iyd, Z80::IyBitSTR>::GetValue() = "RES 3, (IY+%02x)";
    AnyType<Z80::SET_2_iyd, Z80::IyBitSTR>::GetValue() = "SET 2, (IY+0x%02x)";  AnyType<Z80::SET_3_iyd, Z80::IyBitSTR>::GetValue() = "SET 3, (IY+%02x)";
    AnyType<Z80::BIT_4_iyd, Z80::IyBitSTR>::GetValue() = "BIT 4, (IY+0x%02x)";  AnyType<Z80::BIT_5_iyd, Z80::IyBitSTR>::GetValue() = "BIT 5, (IY+%02x)";
    AnyType<Z80::RES_4_iyd, Z80::IyBitSTR>::GetValue() = "RES 4, (IY+0x%02x)";  AnyType<Z80::RES_5_iyd, Z80::IyBitSTR>::GetValue() = "RES 5, (IY+%02x)";
    AnyType<Z80::SET_4_iyd, Z80::IyBitSTR>::GetValue() = "SET 4, (IY+0x%02x)";  AnyType<Z80::SET_5_iyd, Z80::IyBitSTR>::GetValue() = "SET 5, (IY+%02x)";
    AnyType<Z80::BIT_6_iyd, Z80::IyBitSTR>::GetValue() = "BIT 6, (IY+0x%02x)";  AnyType<Z80::BIT_7_iyd, Z80::IyBitSTR>::GetValue() = "BIT 7, (IY+%02x)";
    AnyType<Z80::RES_6_iyd, Z80::IyBitSTR>::GetValue() = "RES 6, (IY+0x%02x)";  AnyType<Z80::RES_7_iyd, Z80::IyBitSTR>::GetValue() = "RES 7, (IY+%02x)";
    AnyType<Z80::SET_6_iyd, Z80::IyBitSTR>::GetValue() = "SET 6, (IY+0x%02x)";  AnyType<Z80::SET_7_iyd, Z80::IyBitSTR>::GetValue() = "SET 7, (IY+%02x)";

    

    // Misc Instruction
    AnyType<Z80::IN_B_c,    Z80::MiscSTR>::GetValue() = "IN B, (C)";  AnyType<Z80::OUT_c_B,  Z80::MiscSTR>::GetValue() = "OUT (C), B";
    AnyType<Z80::IN_D_c,    Z80::MiscSTR>::GetValue() = "IN D, (C)";  AnyType<Z80::OUT_c_D,  Z80::MiscSTR>::GetValue() = "OUT (C), D";
    AnyType<Z80::IN_H_c,    Z80::MiscSTR>::GetValue() = "IN H, (C)";  AnyType<Z80::OUT_c_H,  Z80::MiscSTR>::GetValue() = "OUT (C), H";
    AnyType<Z80::SBC_HL_SP, Z80::MiscSTR>::GetValue() = "SBC HL, SP"; AnyType<Z80::LD_nn_DE, Z80::MiscSTR>::GetValue() = "LD (0x%04x), DE";
    AnyType<Z80::SBC_HL_BC, Z80::MiscSTR>::GetValue() = "SBC HL, BC"; AnyType<Z80::LD_nn_BC, Z80::MiscSTR>::GetValue() = "LD (0x%04x), BC";
    AnyType<Z80::SBC_HL_DE, Z80::MiscSTR>::GetValue() = "SBC HL, DE"; AnyType<Z80::LD_nn_SP, Z80::MiscSTR>::GetValue() = "LD (0x%04x), SP";
    AnyType<Z80::SBC_HL_HL, Z80::MiscSTR>::GetValue() = "SBC HL, HL"; AnyType<Z80::RRD,      Z80::MiscSTR>::GetValue() = "RRD";
    AnyType<Z80::IN_A_c,    Z80::MiscSTR>::GetValue() = "IN A, (C)";  AnyType<Z80::OUT_c_A,  Z80::MiscSTR>::GetValue() = "OUT (C), A";
    AnyType<Z80::NEG,       Z80::MiscSTR>::GetValue() = "NEG";        AnyType<Z80::RETN,     Z80::MiscSTR>::GetValue() = "RETN";
    AnyType<Z80::IM_1,      Z80::MiscSTR>::GetValue() = "IM 1";       AnyType<Z80::LD_A_I,   Z80::MiscSTR>::GetValue() = "LD A, I";
    AnyType<Z80::IN_L_c,    Z80::MiscSTR>::GetValue() = "IN L, (C)";  AnyType<Z80::OUT_c_L,  Z80::MiscSTR>::GetValue() = "OUT (C), L";
    AnyType<Z80::ADC_HL_SP, Z80::MiscSTR>::GetValue() = "ADC HL, SP"; AnyType<Z80::LD_SP_nn, Z80::MiscSTR>::GetValue() = "LD SP, (0x%04x)";
    AnyType<Z80::IM_0,      Z80::MiscSTR>::GetValue() = "IM 0";       AnyType<Z80::LD_I_A,   Z80::MiscSTR>::GetValue() = "LD I, A";
    AnyType<Z80::IN_E_c,    Z80::MiscSTR>::GetValue() = "IN E, (C)";  AnyType<Z80::OUT_c_E,  Z80::MiscSTR>::GetValue() = "OUT (C), E";
    AnyType<Z80::ADC_HL_HL, Z80::MiscSTR>::GetValue() = "ADC HL, HL"; AnyType<Z80::RLD,      Z80::MiscSTR>::GetValue() = "RLD";
    AnyType<Z80::IN_C_c,    Z80::MiscSTR>::GetValue() = "IN C, (C)";  AnyType<Z80::OUT_c_C,  Z80::MiscSTR>::GetValue() = "OUT (C), C";
    AnyType<Z80::ADC_HL_BC, Z80::MiscSTR>::GetValue() = "ADC HL, BC"; AnyType<Z80::LD_BC_nn, Z80::MiscSTR>::GetValue() = "LD BC, (0x%04x)";
    AnyType<Z80::RETI,      Z80::MiscSTR>::GetValue() = "RETI";       AnyType<Z80::LD_R_A,   Z80::MiscSTR>::GetValue() = "LD R, A";
    AnyType<Z80::ADC_HL_DE, Z80::MiscSTR>::GetValue() = "ADC HL, DE"; AnyType<Z80::LD_DE_nn, Z80::MiscSTR>::GetValue() = "LD DE, (0x%04x)";
    AnyType<Z80::IM_2,      Z80::MiscSTR>::GetValue() = "IM 2";       AnyType<Z80::LD_A_R,   Z80::MiscSTR>::GetValue() = "LD A, R";
    AnyType<Z80::LDI,       Z80::MiscSTR>::GetValue() = "LDI";        AnyType<Z80::CPI,      Z80::MiscSTR>::GetValue() = "CPI";
    AnyType<Z80::LDIR,      Z80::MiscSTR>::GetValue() = "LDIR";       AnyType<Z80::CPIR,     Z80::MiscSTR>::GetValue() = "CPIR";
    AnyType<Z80::INI,       Z80::MiscSTR>::GetValue() = "INI";        AnyType<Z80::OUTI,     Z80::MiscSTR>::GetValue() = "OUTI";
    AnyType<Z80::INIR,      Z80::MiscSTR>::GetValue() = "INIR";       AnyType<Z80::OTIR,     Z80::MiscSTR>::GetValue() = "OTIR";
    AnyType<Z80::LDD,       Z80::MiscSTR>::GetValue() = "LDD";        AnyType<Z80::CPD,      Z80::MiscSTR>::GetValue() = "CPD";
    AnyType<Z80::LDDR,      Z80::MiscSTR>::GetValue() = "LDDR";       AnyType<Z80::CPDR,     Z80::MiscSTR>::GetValue() = "CPDR";
    AnyType<Z80::IND,       Z80::MiscSTR>::GetValue() = "IND";        AnyType<Z80::OUTD,     Z80::MiscSTR>::GetValue() = "OUTD";
    AnyType<Z80::INDR,      Z80::MiscSTR>::GetValue() = "INDR";       AnyType<Z80::OTDR,     Z80::MiscSTR>::GetValue() = "OTDR";


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