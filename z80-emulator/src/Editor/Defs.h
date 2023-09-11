#pragma once
#include "src/Interpreter/Interpreter.h"

namespace Editor {

// NOTE: Think about this in a future !!!
// class Cursor;

// typedef TypeList<
//   Cursor, NullType>
//   // AnyType<TokenT::CMD_SRL,  std::string>, TypeList<
//   // AnyType<TokenT::CMD_SUB,  std::string>, TypeList<
//   // AnyType<TokenT::CMD_XOR,  std::string>, NullType>>>>
//    CommandList2;


enum VimT {
  CMD_NONE,

  // phrase
  CMD_i, CMD_I, CMD_a, CMD_A, CMD_o, CMD_O, CMD_C, CMD_D, CMD_r, CMD_R,

  // noun
  CMD_h, CMD_j, CMD_k, CMD_l, CMD_w, CMD_W, CMD_b, CMD_B, CMD_e, CMD_0, CMD_gg, CMD_G, CMD_n, CMD_N, CMD_f, CMD_F,
  CMD_CARET, CMD_DOLLAR, CMD_UNDERLINE, CMD_SLASH, CMD_QUESTION, CMD_COMMA, CMD_SEMICOLON, CMD_SQUIGGLE,

  // verb
  CMD_c, CMD_d, CMD_y,

  // adverb
  CMD_cc, CMD_dd, CMD_yy, CMD_p, CMD_P, CMD_x
};

typedef TypeList<
  TypeList<Int2Type<olc::Key::Q>, Int2Type<'q'>>, TypeList<
  TypeList<Int2Type<olc::Key::W>, Int2Type<'w'>>, TypeList<
  TypeList<Int2Type<olc::Key::E>, Int2Type<'e'>>, TypeList<
  TypeList<Int2Type<olc::Key::R>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<olc::Key::T>, Int2Type<'t'>>, TypeList<
  TypeList<Int2Type<olc::Key::Y>, Int2Type<'y'>>, TypeList<
  TypeList<Int2Type<olc::Key::U>, Int2Type<'u'>>, TypeList<
  TypeList<Int2Type<olc::Key::I>, Int2Type<'i'>>, TypeList<
  TypeList<Int2Type<olc::Key::O>, Int2Type<'o'>>, TypeList<
  TypeList<Int2Type<olc::Key::P>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<olc::Key::A>, Int2Type<'a'>>, TypeList<
  TypeList<Int2Type<olc::Key::S>, Int2Type<'s'>>, TypeList<
  TypeList<Int2Type<olc::Key::D>, Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<olc::Key::F>, Int2Type<'f'>>, TypeList<
  TypeList<Int2Type<olc::Key::G>, Int2Type<'g'>>, TypeList<
  TypeList<Int2Type<olc::Key::H>, Int2Type<'h'>>, TypeList<
  TypeList<Int2Type<olc::Key::J>, Int2Type<'j'>>, TypeList<
  TypeList<Int2Type<olc::Key::K>, Int2Type<'k'>>, TypeList<
  TypeList<Int2Type<olc::Key::L>, Int2Type<'l'>>, TypeList<
  TypeList<Int2Type<olc::Key::Z>, Int2Type<'z'>>, TypeList<
  TypeList<Int2Type<olc::Key::X>, Int2Type<'x'>>, TypeList<
  TypeList<Int2Type<olc::Key::C>, Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<olc::Key::V>, Int2Type<'v'>>, TypeList<
  TypeList<Int2Type<olc::Key::B>, Int2Type<'b'>>, TypeList<
  TypeList<Int2Type<olc::Key::N>, Int2Type<'n'>>, TypeList<
  TypeList<Int2Type<olc::Key::M>, Int2Type<'m'>>, TypeList<

  TypeList<Int2Type<olc::Key::K0>, Int2Type<'0'>>, TypeList<
  TypeList<Int2Type<olc::Key::K1>, Int2Type<'1'>>, TypeList<
  TypeList<Int2Type<olc::Key::K2>, Int2Type<'2'>>, TypeList<
  TypeList<Int2Type<olc::Key::K3>, Int2Type<'3'>>, TypeList<
  TypeList<Int2Type<olc::Key::K4>, Int2Type<'4'>>, TypeList<
  TypeList<Int2Type<olc::Key::K5>, Int2Type<'5'>>, TypeList<
  TypeList<Int2Type<olc::Key::K6>, Int2Type<'6'>>, TypeList<
  TypeList<Int2Type<olc::Key::K7>, Int2Type<'7'>>, TypeList<
  TypeList<Int2Type<olc::Key::K8>, Int2Type<'8'>>, TypeList<
  TypeList<Int2Type<olc::Key::K9>, Int2Type<'9'>>, TypeList<

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

  TypeList<Int2Type<olc::Key::TAB>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::BACK>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::DEL>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::SPACE>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::ENTER>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<' '>>, TypeList<

  TypeList<Int2Type<olc::Key::UP>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::DOWN>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::RIGHT>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::LEFT>, Int2Type<' '>>, TypeList<

  TypeList<Int2Type<olc::Key::OEM_1>, Int2Type<';'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_2>, Int2Type<'/'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_3>, Int2Type<'\''>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_4>, Int2Type<'['>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_5>, Int2Type<'\\'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_6>, Int2Type<']'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_7>, Int2Type<'\''>>, TypeList<

  TypeList<Int2Type<olc::Key::EQUALS>, Int2Type<'='>>, TypeList<
  TypeList<Int2Type<olc::Key::PERIOD>, Int2Type<'.'>>, TypeList<
  TypeList<Int2Type<olc::Key::COMMA>, Int2Type<','>>, TypeList<
  TypeList<Int2Type<olc::Key::MINUS>, Int2Type<'-'>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   KeyEvent; 


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
  TypeList<Int2Type<VimT::CMD_gg>,Int2Type<'g'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_G>, Int2Type<'G'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_n>, Int2Type<'n'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_N>, Int2Type<'N'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_f>, Int2Type<'f'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_F>, Int2Type<'F'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_y>,  Int2Type<'y'>>, TypeList<

  TypeList<Int2Type<VimT::CMD_SQUIGGLE>,   Int2Type<'~'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_DOLLAR>,     Int2Type<'$'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_CARET>,      Int2Type<'^'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_UNDERLINE>,  Int2Type<'_'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_SLASH>,      Int2Type<'/'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_QUESTION>,   Int2Type<'?'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_COMMA>,      Int2Type<','>>, TypeList<
  TypeList<Int2Type<VimT::CMD_SEMICOLON>,  Int2Type<';'>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   VimCommands; 
  
typedef TypeList<
  TypeList<Int2Type<VimT::CMD_o>,  Int2Type<'o'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_O>,  Int2Type<'O'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_C>,  Int2Type<'C'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_D>,  Int2Type<'D'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_d>,  Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_c>,  Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_x>,  Int2Type<'x'>>, TypeList<

  TypeList<Int2Type<VimT::CMD_SQUIGGLE>,  Int2Type<'x'>>, TypeList<

  TypeList<Int2Type<VimT::CMD_r>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_p>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<VimT::CMD_P>, Int2Type<'P'>>, NullType>>>>>>>>>>>
   SyncVimCommands; 
   

};
