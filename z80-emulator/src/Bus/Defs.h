#pragma once
#include "src/Editor/Editor.h"

namespace Bus {

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
    RET_P,   POP_AF,     JP_P_NN,   DI,       CALL_P_NN,  PUSH_AF, OR_N,     RST_30h, RET_M,     JP_SP_HL,    JP_M_NN,   EI,        CALL_M_NN,  IY_INSTR,   CP_N,     RST_38h,
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
  // TODO: Misc Instruction
};

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
  TypeList<Int2Type<Editor::VimT::CMD_n>, Int2Type<'n'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_N>, Int2Type<'N'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_f>, Int2Type<'f'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_F>, Int2Type<'F'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_y>,  Int2Type<'y'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_u>,  Int2Type<'u'>>, TypeList<
  TypeList<Int2Type<Editor::VimT::CMD_U>,  Int2Type<'U'>>, TypeList<

  TypeList<Int2Type<Editor::VimT::CMD_SPACE>,      Int2Type<' '>>, TypeList<
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
   

class Defs {

public:
  static void Init() {
    // TODO: Add mnemonics for instruction
  }
};
};