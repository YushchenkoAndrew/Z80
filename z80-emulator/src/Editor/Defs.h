#pragma once
#include "src/Interpreter/Interpreter.h"

namespace Editor {

enum VimT {
  // phrase
  CMD_i, CMD_I, CMD_a, CMD_A, CMD_o, CMD_O, CMD_C, CMD_D, CMD_r, CMD_R, CMD_u, CMD_U,

  // noun
  CMD_h, CMD_j, CMD_k, CMD_l, CMD_w, CMD_W, CMD_b, CMD_B, CMD_e, CMD_0, CMD_gg, CMD_gd, CMD_G, CMD_n, CMD_N, CMD_f, CMD_F,
  CMD_CARET, CMD_DOLLAR, CMD_UNDERLINE, CMD_SLASH, CMD_QUESTION, CMD_COMMA, CMD_SEMICOLON, CMD_SQUIGGLE,

  CMD_SPACE,

  // verb
  CMD_c, CMD_d, CMD_y,

  // adverb
  CMD_cc, CMD_dd, CMD_yy, CMD_p, CMD_P, CMD_x,
  
  // Special commands
  CMD_CTRL_d, CMD_CTRL_u,
  
  
  // Undefined CMD
  CMD_NONE, CMD_z, CMD_q, CMD_NUMBER, CMD_AND
};

typedef TypeList<
  TypeList<Int2Type<VimT::CMD_i>, Int2Type<'i'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_I>, Int2Type<'I'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_a>, Int2Type<'a'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_A>, Int2Type<'A'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_o>, Int2Type<'o'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_O>, Int2Type<'O'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_r>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_R>, Int2Type<'R'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_p>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_P>, Int2Type<'P'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_C>, Int2Type<'C'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_D>, Int2Type<'D'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_h>, Int2Type<'h'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_j>, Int2Type<'j'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_k>, Int2Type<'k'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_l>, Int2Type<'l'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_x>, Int2Type<'x'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_w>, Int2Type<'w'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_W>, Int2Type<'W'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_b>, Int2Type<'b'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_B>, Int2Type<'B'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_e>, Int2Type<'e'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_0>, Int2Type<'0'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_G>, Int2Type<'G'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_n>, Int2Type<'n'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_N>, Int2Type<'N'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_f>, Int2Type<'f'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_F>, Int2Type<'F'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_y>,  Int2Type<'y'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_u>,  Int2Type<'u'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_U>,  Int2Type<'U'>>, TypeList<

  TypeList<Int2Type<VimT::CMD_SQUIGGLE>,   Int2Type<'~'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_DOLLAR>,     Int2Type<'$'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_CARET>,      Int2Type<'^'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_UNDERLINE>,  Int2Type<'_'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_SLASH>,      Int2Type<'/'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_QUESTION>,   Int2Type<'?'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_COMMA>,      Int2Type<','>>, TypeList<
  TypeList<Int2Type<VimT::CMD_SEMICOLON>,  Int2Type<';'>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   VimCommands; 
  
typedef TypeList<
  TypeList<Int2Type<VimT::CMD_o>,  Int2Type<'o'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_O>,  Int2Type<'O'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_C>,  Int2Type<'C'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_D>,  Int2Type<'D'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_x>,  Int2Type<'x'>>, TypeList<

  TypeList<Int2Type<VimT::CMD_SQUIGGLE>,  Int2Type<'~'>>, TypeList<

  TypeList<Int2Type<VimT::CMD_r>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_p>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_P>, Int2Type<'P'>>, NullType>>>>>>>>>>>
   SyncVimCommands; 

typedef TypeList<
  TypeList<Int2Type<VimT::CMD_i>, Int2Type<'i'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_I>, Int2Type<'I'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_a>, Int2Type<'a'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_A>, Int2Type<'A'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_o>, Int2Type<'o'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_O>, Int2Type<'O'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_r>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_R>, Int2Type<'R'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_p>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_P>, Int2Type<'P'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_C>, Int2Type<'C'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_D>, Int2Type<'D'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_x>, Int2Type<'x'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_y>,  Int2Type<'y'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_u>,  Int2Type<'u'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_U>,  Int2Type<'U'>>, TypeList<

  TypeList<Int2Type<VimT::CMD_SQUIGGLE>,   Int2Type<'~'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_UNDERLINE>,  Int2Type<'_'>>, NullType>>>>>>>>>>>>>>>>>>>>
   LockedVimCommands; 
   

};
