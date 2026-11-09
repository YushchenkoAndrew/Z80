#pragma once

#include "include/Typelist.h"
#include "lib/olcPixelGameEngine.h"

#define HISTORY_SIZE 100

namespace Color {
static const olc::Pixel
    GREY(0xAC, 0xB6, 0xC7),   DARK_GREY(0x3A, 0x45, 0x56),   VERY_DARK_GREY(0x18, 0x23, 0x2D),
    RED(0xFF, 0x86, 0x63),    DARK_RED(0xB8, 0x5A, 0x45),    VERY_DARK_RED(0x5C, 0x2D, 0x25),
    ORANGE(0xE0, 0x9C, 0x5F), DARK_ORANGE(0xD3, 0x7D, 0x09), VERY_DARK_ORANGE(0x9D, 0x02, 0x08),
    YELLOW(0xEC, 0xCE, 0x8A), DARK_YELLOW(0xD7, 0xAD, 0x5D), VERY_DARK_YELLOW(0x65, 0x57, 0x39),
    GREEN(0x00, 0xC9, 0x5A),  DARK_GREEN(0x2A, 0xAE, 0x4F),  VERY_DARK_GREEN(0x0D, 0x4B, 0x2C),
    CYAN(0x2E, 0xBF, 0xD5),   DARK_CYAN(0x23, 0x8A, 0x9A),   VERY_DARK_CYAN(0x17, 0x4A, 0x53),
    BLUE(0x4C, 0xB8, 0xFE),   DARK_BLUE(0x2D, 0x75, 0xA8),   VERY_DARK_BLUE(0x1B, 0x3F, 0x5A),
    MAGENTA(0xCC, 0x75, 0xEC),DARK_MAGENTA(0x91, 0x52, 0xA8),VERY_DARK_MAGENT(0x4B, 0x2B, 0x57),
    WHITE(0xDB, 0xE5, 0xD5),  BLACK(0x14, 0x1A, 0x1F),       BLANK(0, 0, 0, 0);
};

typedef TypeList<
  Int2Type<olc::Key::SPACE>,TypeList<
  Int2Type<olc::Key::TAB>,  TypeList<
  Int2Type<olc::Key::SHIFT>,TypeList<
  Int2Type<olc::Key::CTRL>, TypeList<
  Int2Type<olc::Key::INS>,  TypeList<
  Int2Type<olc::Key::DEL>,  TypeList<
  Int2Type<olc::Key::HOME>, TypeList<
  Int2Type<olc::Key::END>,  TypeList<
  Int2Type<olc::Key::PGUP>, TypeList<
  Int2Type<olc::Key::PGDN>, TypeList<

  Int2Type<olc::Key::BACK>,  TypeList<
  Int2Type<olc::Key::ESCAPE>,TypeList<
  Int2Type<olc::Key::RETURN>,TypeList<
  Int2Type<olc::Key::ENTER>, TypeList<
  Int2Type<olc::Key::PAUSE>, TypeList<
  Int2Type<olc::Key::SCROLL>,TypeList<

  Int2Type<olc::Key::A>, TypeList<
  Int2Type<olc::Key::B>, TypeList<
  Int2Type<olc::Key::C>, TypeList<
  Int2Type<olc::Key::D>, TypeList<
  Int2Type<olc::Key::E>, TypeList<
  Int2Type<olc::Key::F>, TypeList<
  Int2Type<olc::Key::G>, TypeList<
  Int2Type<olc::Key::H>, TypeList<
  Int2Type<olc::Key::I>, TypeList<
  Int2Type<olc::Key::J>, TypeList<
  Int2Type<olc::Key::K>, TypeList<
  Int2Type<olc::Key::L>, TypeList<
  Int2Type<olc::Key::M>, TypeList<
  Int2Type<olc::Key::N>, TypeList<
  Int2Type<olc::Key::O>, TypeList<
  Int2Type<olc::Key::P>, TypeList<
  Int2Type<olc::Key::Q>, TypeList<
  Int2Type<olc::Key::R>, TypeList<
  Int2Type<olc::Key::S>, TypeList<
  Int2Type<olc::Key::T>, TypeList<
  Int2Type<olc::Key::U>, TypeList<
  Int2Type<olc::Key::V>, TypeList<
  Int2Type<olc::Key::W>, TypeList<
  Int2Type<olc::Key::X>, TypeList<
  Int2Type<olc::Key::Y>, TypeList<
  Int2Type<olc::Key::Z>, TypeList<


  Int2Type<olc::Key::K0>, TypeList<
  Int2Type<olc::Key::K1>, TypeList<
  Int2Type<olc::Key::K2>, TypeList<
  Int2Type<olc::Key::K3>, TypeList<
  Int2Type<olc::Key::K4>, TypeList<
  Int2Type<olc::Key::K5>, TypeList<
  Int2Type<olc::Key::K6>, TypeList<
  Int2Type<olc::Key::K7>, TypeList<
  Int2Type<olc::Key::K8>, TypeList<
  Int2Type<olc::Key::K9>, TypeList<

  Int2Type<olc::Key::NP0>, TypeList<
  Int2Type<olc::Key::NP1>, TypeList<
  Int2Type<olc::Key::NP2>, TypeList<
  Int2Type<olc::Key::NP3>, TypeList<
  Int2Type<olc::Key::NP4>, TypeList<
  Int2Type<olc::Key::NP5>, TypeList<
  Int2Type<olc::Key::NP6>, TypeList<
  Int2Type<olc::Key::NP7>, TypeList<
  Int2Type<olc::Key::NP8>, TypeList<
  Int2Type<olc::Key::NP9>, TypeList<

  Int2Type<olc::Key::OEM_1>, TypeList<
  Int2Type<olc::Key::OEM_2>, TypeList<
  Int2Type<olc::Key::OEM_3>, TypeList<
  Int2Type<olc::Key::OEM_4>, TypeList<
  Int2Type<olc::Key::OEM_5>, TypeList<
  Int2Type<olc::Key::OEM_6>, TypeList<
  Int2Type<olc::Key::OEM_7>, TypeList<
  Int2Type<olc::Key::OEM_8>, TypeList<

  Int2Type<olc::Key::UP>,   TypeList<
  Int2Type<olc::Key::DOWN>, TypeList<
  Int2Type<olc::Key::RIGHT>,TypeList<
  Int2Type<olc::Key::LEFT>, TypeList<

  Int2Type<olc::Key::NP_MUL>, TypeList<
  Int2Type<olc::Key::NP_DIV>, TypeList<
  Int2Type<olc::Key::NP_ADD>, TypeList<
  Int2Type<olc::Key::NP_SUB>, TypeList<

  Int2Type<olc::Key::EQUALS>,TypeList<
  Int2Type<olc::Key::COMMA>, TypeList<
  Int2Type<olc::Key::PERIOD>,TypeList<
  Int2Type<olc::Key::MINUS>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   KeyEvent; 


enum Events {
  NEW_DEBUG_MODE_CALLBACK, DETACH_DEBUG_MODE_CALLBACK,
  NEXT_DEBUG_STEP_CALLBACK, DEBUG_RESET_CALLBACK,

  KEYBOARD_EVENT_SUBSCRIBE, KEYBOARD_EVENT_UNSUBSCRIBE,

  EDITOR_DEFINE_CALLBACK, EDITOR_GOTO_CALLBACK, EDITOR_BREAKPOINT_CALLBACK,
  MEMORY_GOTO_CALLBACK, PANEL_SELECT_CALLBACK,
  
  CMD_UPDATE_CALLBACK, CMD_EXEC_CALLBACK, PROGRAM_EXIT
};

struct LocationT { std::filesystem::path file; olc::vi2d pos; };

class PixelGameEngine : public olc::PixelGameEngine {
  public:
  virtual void Event(Int2Type<NEW_DEBUG_MODE_CALLBACK>) = 0;
  virtual void Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>) = 0;

  virtual void Event(Int2Type<DEBUG_RESET_CALLBACK>) = 0;
  virtual void Event(Int2Type<NEXT_DEBUG_STEP_CALLBACK>) = 0;

  virtual void Event(Int2Type<KEYBOARD_EVENT_SUBSCRIBE>, void*) = 0;
  virtual void Event(Int2Type<KEYBOARD_EVENT_UNSUBSCRIBE>, void*) = 0;

  virtual void Event(Int2Type<EDITOR_DEFINE_CALLBACK>, LocationT) = 0;
  virtual void Event(Int2Type<EDITOR_GOTO_CALLBACK>, olc::vi2d) = 0;
  virtual void Event(Int2Type<EDITOR_BREAKPOINT_CALLBACK>, olc::vi2d) = 0;

  virtual void Event(Int2Type<PANEL_SELECT_CALLBACK>, int32_t) = 0;
  virtual void Event(Int2Type<MEMORY_GOTO_CALLBACK>, int32_t) = 0;

  virtual void Event(Int2Type<CMD_EXEC_CALLBACK>) = 0;
  virtual void Event(Int2Type<CMD_UPDATE_CALLBACK>, std::string) = 0;
  virtual void Event(Int2Type<PROGRAM_EXIT>) = 0;

  virtual uint32_t GetMode() = 0;

  void FillDither(olc::vi2d pos, olc::vi2d size, olc::Pixel color = olc::WHITE, int32_t scale = 1, int32_t flag = 0) {
    for (int32_t i = 0, k = flag; i < size.y; i += scale, k ^= 1) {
      for (int32_t j = k * scale; j < size.x; j += scale << 1) {
        FillRect(pos + olc::vi2d(j, i), olc::vi2d(scale, scale), color);
      }
    }
  }
};


