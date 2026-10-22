#include "Bus.h"
#include "src/Bus/IRQ.h"
#include "src/Bus/PIT.h"
#include "src/Registry.h"

namespace Bus {
  #define BIT(word, pos) ((word >> pos) & 0x01)

  #define TEMPLATE template <int32_t SizeT>
  #define CLASS Memory<SizeT>

  TEMPLATE void CLASS::Disassemble() {
    dasm = Registry::Bus->Disassemble();
    lexer.scan(dasm.first); 

    for (auto& addr : dasm.second) LINES_SIZE = std::max(LINES_SIZE, addr.second);
  }


  template class Memory<W27C512_SIZE>;
  template class Memory<IMS1423_SIZE>;
  template class Memory<HY62256A_SIZE>;

  #undef TEMPLATE
  #undef CLASS

  void Keyboard::Interrupt() { Registry::IRQ->ResetFlag(IRQ::BIT::KEYBOARD); }
  void RTC::Interrupt() { IRQF(); Registry::IRQ->ResetFlag(IRQ::BIT::RTC); }
  void PIT::Interrupt(int32_t ct) { Registry::IRQ->ResetFlag(IRQ::BIT::CT1 - ct + 1); }

  uint8_t Bus::Read(uint32_t addr, bool mreq) {
    if (mreq) return mux(Int2Type<MREQ>(), addr);
    else return mux(Int2Type<IORQ>(), addr);
  }

  uint8_t Bus::Write(uint32_t addr, uint8_t data, bool mreq) {
    if (mreq) return mux(Int2Type<MREQ>(), addr, data);
    else return mux(Int2Type<IORQ>(), addr, data);
  }

  inline uint8_t Bus::mux(Int2Type<Bus::MREQ>, uint32_t addr16) {
    auto addr = addr16 & 0x3FFF;
    auto mmu = Registry::KR580VV55->regB(Int2Type<PPI::REG2PORT>());

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return Registry::W27C512->Read(addr | ((mmu & 0x03) << 14), true);
      case MUX::MREQ::HY62256A: return Registry::HY62256A->Read(addr, true);
      case MUX::MREQ::KM28C17: break;
      case MUX::MREQ::STACK: {
        switch((addr16 & 0x3000) >> 12) {
          case MUX::STACK::IMS1423_1: 
          case MUX::STACK::IMS1423_2: return Registry::IMS1423->Read(addr, true);
          case MUX::STACK::MC146818:  return Registry::MC146818->Read(addr, true);
        }
      }
    }

    return 0x00;
  }

  inline uint8_t Bus::mux(Int2Type<MREQ>, uint32_t addr16, uint8_t data) {
    auto addr = addr16 & 0x3FFF;
    auto mmu = Registry::KR580VV55->regB(Int2Type<PPI::REG2PORT>());

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return 0x00;
      case MUX::MREQ::HY62256A: return Registry::HY62256A->Write(addr, data, true);
      case MUX::MREQ::KM28C17: break;
      case MUX::MREQ::STACK: {
        switch((addr16 & 0x3000) >> 12) {
          case MUX::STACK::IMS1423_1: 
          case MUX::STACK::IMS1423_2: return Registry::IMS1423->Write(addr, data, true);
          case MUX::STACK::MC146818:  return Registry::MC146818->Write(addr, data, true);
        }
      }
    }

    return 0x00;
  }

  inline uint8_t Bus::mux(Int2Type<IORQ>, uint32_t addr) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT:   return Registry::Switch->Read(addr & 0x00FF, true);
      case MUX::IORQ::HEX_PORT:      return 0x00;
      case MUX::IORQ::PPI_PORT:      return Registry::KR580VV55->Read(addr & 0x00FF, true);
      case MUX::IORQ::UART_PORT:     break; // TODO
      case MUX::IORQ::KEYBOARD_PORT: return Registry::IRQ->SetFlag(IRQ::BIT::KEYBOARD), Registry::Keyboard->Read(addr, true);
      case MUX::IORQ::INT_PORT:      return Registry::IRQ->Read(addr, true);
      case MUX::IORQ::PIT_PORT: {
        Registry::IRQ->SetFlag(IRQ::BIT::CT1);
        Registry::IRQ->SetFlag(IRQ::BIT::CT2);

        return Registry::KR580VI53->Read(addr, true);
      }
    }

    return 0x00;
  }

  inline uint8_t Bus::mux(Int2Type<IORQ>, uint32_t addr, uint8_t data) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT:   return Registry::Led->Write(addr, data, true);
      case MUX::IORQ::HEX_PORT:      return Registry::Hex->Write(addr, data, true); 
      case MUX::IORQ::UART_PORT:     break; // TODO
      case MUX::IORQ::KEYBOARD_PORT: return 0x00;
      case MUX::IORQ::INT_PORT:      return Registry::IRQ->Write(addr, data, true);
      case MUX::IORQ::PIT_PORT: {
        Registry::IRQ->SetFlag(IRQ::BIT::CT1);
        Registry::IRQ->SetFlag(IRQ::BIT::CT2);

        return Registry::KR580VI53->Write(addr, data, true);
      }
      case MUX::IORQ::PPI_PORT: {
        Registry::KR580VV55->Write(addr, data, true);
        uint8_t ctrl = Registry::KR580VV55->regC(Int2Type<PPI::REG2PORT>());

        Registry::KR580VI53->SetGate(0, BIT(ctrl, MUX::PPI_CS::CS0));
        Registry::KR580VI53->SetGate(1, BIT(ctrl, MUX::PPI_CS::CS1));

        switch ((ctrl >> MUX::PPI_CS::CS3) & 7) {
          case MUX::PPI_IO::LCD_PORT:
            if (BIT(ctrl, MUX::PPI_CS::CSW)) Registry::Lcd->Read(addr, BIT(ctrl, MUX::PPI_CS::CSA));
            else Registry::Lcd->Write(addr, Registry::KR580VV55->regA(Int2Type<PPI::REG2PORT>()), BIT(ctrl, MUX::PPI_CS::CSA));
            break;
        }

        return data;
      }
    }

    return 0x00;
  }

  // Bus::Bus(PixelGameEngine* engine, LuaScript& lua):
  //   Window::Window(nullptr) {}
    // luaConfig(config),

    // led(std::make_shared<Led>(engine, this)),
    // switches(std::make_shared<Switch>(this)),
    // hexDisplay(std::make_shared<HexDisplay>(this)),
    // lcd(std::make_shared<LCD>(this)),
    // keyboard(std::make_shared<Keyboard>(this)),

    // KR580VV55(std::make_shared<PPI>(this)),
    // KR580VI53(std::make_shared<PIT>(this, config.GetTableValue<int32_t>(nullptr, "clock") * 8)),
    // MC146818(std::make_shared<RTC>(this)),
    // interrupt(std::make_shared<IRQ>(this)),

    // Z80(std::make_shared<Z80::CPU>(this, config.GetTableValue<int32_t>(nullptr, "clock"))),
    // W27C512(std::make_shared<Memory<MemoryT::W27C512, W27C512_SIZE>>(this)),
    // IMS1423(std::make_shared<Memory<MemoryT::IMS1423, IMS1423_SIZE>>(this)),
    // HY62256A(std::make_shared<Memory<MemoryT::HY62256A, HY62256A_SIZE>>(this)) {}

  // void Bus::Initialize() {
  //   led       ->Initialize();
  //   switches  ->Initialize();
  //   hexDisplay->Initialize();
  //   lcd       ->Initialize();
  //   keyboard  ->Initialize();
  //   KR580VV55 ->Initialize();
  //   // KR580VI53 ->Initialize();
  //   MC146818  ->Preinitialize();
  //   interrupt ->Initialize();
  //   // Z80       ->Initialize();
  //   W27C512   ->Initialize(); 
  // }

  // void Bus::Select(DimensionT dimensions) {
  //   olc::vi2d zero = olc::vi2d(0, 0);
  //   olc::vi2d offset = olc::vi2d(4, 8);
  //   grid.clear();

  //   // TODO: Design and draw bus as PCB
  //   led->Select(std::pair(olc::vi2d(10, 10), zero));
  //   switches->Select(std::pair(olc::vi2d(10, 25), zero));
  //   hexDisplay->Select(std::pair(olc::vi2d(10, 40), zero));
  //   lcd->Select(std::pair(olc::vi2d(10, 150), zero));

  //   olc::vi2d pos = olc::vi2d(dimensions.second.x * 7 / 10, dimensions.second.y);
  //   olc::vi2d size = dimensions.second - pos;

  //   auto Init = [&pos, &dimensions, &offset, &grid = grid](std::shared_ptr<Window> window, int32_t y, olc::vi2d size = olc::vi2d()) {
  //     grid.push_back(std::pair(olc::vi2d(pos.x, y), olc::vi2d(dimensions.second.x - offset.x * 2, y)));
  //     window->Select(std::pair(olc::vi2d(pos.x, y) + offset, size));
  //   };

  //   Z80->Select(std::pair(olc::vi2d(pos.x, zero.y) + offset, zero));

  //   Init(interrupt, 140); Init(keyboard, 184); Init(KR580VV55, 206); Init(KR580VI53, 228); 
  //   Init(MC146818, 294, olc::vi2d(size.x, 88)); 

  //   grid.push_back(std::pair(olc::vi2d(pos.x - offset.x * 2, offset.y), olc::vi2d(pos.x - offset.x * 2, dimensions.second.y - offset.y)));
  // }

  // void Bus::Process() {}
  // void Bus::Draw() {
  //   led       ->Draw();
  //   switches  ->Draw();
  //   hexDisplay->Draw();
  //   lcd       ->Draw();
  //   keyboard  ->Draw();
  //   KR580VV55 ->Draw();
  //   KR580VI53 ->Draw();
  //   MC146818  ->Draw();
  //   interrupt ->Draw();
  //   Z80       ->Draw();

  //   for (auto& line : grid) GameEngine->DrawLine(line.first, line.second, Color::DARK_GREY);

  //   // TODO: Get data from lua
  //   // GameEngine->FillRect({ 300, 20 }, { 20, 50 }, Color::VERY_DARK_GREY);
  // }

  void IRQ::Interrupt() { if ((uint8_t)(state ^ 0xff)) Registry::Z80->Interrupt(); }
  inline DisassembleT Bus::Disassemble() { return Registry::Z80->Disassemble(); }


namespace Z80 {

  inline uint8_t CPU::Read() { return Registry::Bus->Read(regPC()++, true); }

  inline uint8_t CPU::Read(uint32_t addr, bool mreq) { return Registry::Bus->Read(addr, mreq); }
  inline uint8_t CPU::Write(uint32_t addr, uint8_t data, bool mreq) { return Registry::Bus->Write(addr, data, mreq); }

  DisassembleT CPU::Disassemble() {
    DisassembleT dasm = std::pair("", std::unordered_map<int32_t, int32_t>());
  
    for (uint32_t i = 0, line = 0; i < 0xFFFF; line++, i++) {
      std::pair<std::string, int32_t> cmd;
      dasm.second[i] = line;
      std::string unknown = "";

      switch (uint32_t instruction = Registry::Bus->Read(i, true)) {
        case Z80::MISC_INSTR: {
          // FIXME: 
          // AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          // std::pair<MiscSTR, int32_t> res = foreach<MiscMInstructions, AnyType<-1, int32_t>>::Key2Value();
          // if (res.second == -1) unknown = sprintf("db 0xED, 0x%02x", AnyType<-1, int32_t>::GetValue());

          // cmd = std::pair(*res.first, res.second); break;
        }
        
        case Z80::BIT_INSTR: {
          // FIXME: 
          // AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          // std::pair<BitSTR, int32_t> res = foreach<BitInstructions, AnyType<-1, int32_t>>::Key2Value();
          // if (res.second == -1) unknown = sprintf("db 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

          // cmd = std::pair(*res.first, res.second); break;
        }

        case Z80::IX_INSTR: {
          // FIXME: 
          // instruction = bus->Read(++i, true);

          // if (instruction == Z80::IxBitInstr) {
          //   AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          //   std::pair<IxBitSTR, int32_t> res = foreach<IxBitInstructions, AnyType<-1, int32_t>>::Key2Value();
          //   if (res.second == -1) unknown = sprintf("db 0xDD, 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

          //   cmd = std::pair(*res.first, res.second); break;
          // } 

          // AnyType<-1, int32_t>::GetValue() = instruction;
          // std::pair<IxSTR, int32_t> res = foreach<IxInstructions, AnyType<-1, int32_t>>::Key2Value();
          // if (res.second == -1) unknown = sprintf("db 0xDD, 0x%02x", AnyType<-1, int32_t>::GetValue());

          // cmd = std::pair(*res.first, res.second); break;
        }

        case Z80::IY_INSTR: {
          // FIXME: 
          // instruction = bus->Read(++i, true);

          // if (instruction == Z80::IyBitInstr) {
          //   AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          //   std::pair<IyBitSTR, int32_t> res = foreach<IyBitInstructions, AnyType<-1, int32_t>>::Key2Value();
          //   if (res.second == -1) unknown = sprintf("db 0xFD, 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

          //   cmd = std::pair(*res.first, res.second); break;
          // } 

          // AnyType<-1, int32_t>::GetValue() = instruction;
          // std::pair<IySTR, int32_t> res = foreach<IyInstructions, AnyType<-1, int32_t>>::Key2Value();
          // if (res.second == -1) unknown = sprintf("db 0xFD, 0x%02x", AnyType<-1, int32_t>::GetValue());

          // cmd = std::pair(*res.first, res.second); break;
        }

        default: {
          // FIXME: 
          // AnyType<-1, int32_t>::GetValue() = instruction;
          // std::pair<InstrSTR, int32_t> res = foreach<Instructions, AnyType<-1, int32_t>>::Key2Value();
          // if (res.second == -1) unknown = sprintf("db 0x%02x", AnyType<-1, int32_t>::GetValue());

          // cmd = std::pair(*res.first, res.second); break;
        }
      }
      

      if (unknown.size()) { dasm.first += unknown + "\n"; continue; }

      std::string phrase = cmd.first;
      for (int32_t cnt = cmd.second & 0x0F, offset = 1; cnt != 1; offset++) {
        const int32_t bytes = (cmd.second >> (4 * offset)) & 0x0F;
        int32_t word = 0x00; cnt -= bytes;
        for (int32_t j = 0; j < bytes; j++) {
          word = word | (Registry::Bus->Read(++i, true) << (j * 8));
        }
      
        phrase = sprintf(phrase, word);
      }

      dasm.first += phrase + "\n";
    }

    return dasm;
  }

  std::string CPU::sprintf(std::string str, int32_t byte) {
    for (int32_t i = 0; i < str.size(); i++) {
      if (str[i] != '%' && i + 1 < str.size()) continue;
      std::stringstream ss;

      int32_t size = 2; int32_t startAt = i;

      if (str[i + 1] != 'd' || str[i + 1] != 'c' || str[i + 1] != 'x') {
        ss << std::setfill(str[++i]); size++;

        if (i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '9') {
          ss << std::setw(str[++i] - '0'); size++;
        }
      }

      switch (str[i + 1]) {
        case 'd': ss << byte; break;
        case 'c': ss << (char)byte; break;
        case 'x': ss << std::hex << std::uppercase << byte; break;
        default: return str;
      }
      
      str.replace(startAt, size, ss.str());
      return str;
    }

    return str;
  }
};

};
