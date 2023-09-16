#pragma once
#include "src/Editor/Editor.h"

namespace Bus {

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
   

};