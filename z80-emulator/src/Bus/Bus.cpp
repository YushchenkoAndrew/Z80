#include "Bus.h"

namespace Bus {
  #define BIT(word, pos) ((word >> pos) & 0x01)

  #define TEMPLATE template <int32_t TypeT, int32_t SizeT>
  #define CLASS Memory<TypeT, SizeT>

  TEMPLATE void CLASS::Disassemble() {
    dasm = bus->Disassemble();
    lexer.scan(dasm.first); 

    for (auto& addr : dasm.second) LINES_SIZE = std::max(LINES_SIZE, addr.second);
  }


  template class Memory<MemoryT::W27C512, W27C512_SIZE>;
  template class Memory<MemoryT::IMS1423, IMS1423_SIZE>;

  #undef TEMPLATE
  #undef CLASS

  void Keyboard::Interrupt() { bus->interrupt->ResetFlag(InterruptVector::IRQ::KEYBOARD); }

  Bus::Bus(LuaScript& config):
    luaConfig(config),

    led(std::make_shared<Led>(this)),
    switches(std::make_shared<Switch>(this)),
    hexDisplay(std::make_shared<HexDisplay>(this)),
    lcd(std::make_shared<LCD>(this)),
    keyboard(std::make_shared<Keyboard>(this)),

    ppi(std::make_shared<PPI>(this)),
    rlt(std::make_shared<RLT>(this)),
    interrupt(std::make_shared<InterruptVector>(this)),

    Z80(std::make_shared<Z80::CPU>(this, config.GetTableValue<int32_t>(nullptr, "clock"))),
    W27C512(std::make_shared<Memory<MemoryT::W27C512, W27C512_SIZE>>(this)),
    IMS1423(std::make_shared<Memory<MemoryT::IMS1423, IMS1423_SIZE>>(this)) {}

  void Bus::Preinitialize() {
    led       ->Preinitialize();
    switches  ->Preinitialize();
    hexDisplay->Preinitialize();
    lcd       ->Preinitialize();
    keyboard  ->Preinitialize();
    ppi       ->Preinitialize();
    rlt       ->Preinitialize();
    interrupt ->Preinitialize();
    Z80       ->Preinitialize();
    W27C512   ->Preinitialize(); 
  }

  void Bus::Initialize(DimensionT dimensions) {
    olc::vi2d zero = olc::vi2d(0, 0);
    olc::vi2d offset = olc::vi2d(4, 8);
    grid.clear();

    // TODO: Design and draw bus as PCB
    led->Initialize(std::pair(olc::vi2d(10, 10), zero));
    switches->Initialize(std::pair(olc::vi2d(10, 25), zero));
    hexDisplay->Initialize(std::pair(olc::vi2d(10, 40), zero));
    lcd->Initialize(std::pair(olc::vi2d(10, 150), zero));

    olc::vi2d pos = olc::vi2d(dimensions.second.x * 8 / 10, dimensions.second.y);

    auto Init = [&pos, &dimensions, &offset, &grid = grid](std::shared_ptr<Window> window, int32_t y) {
      grid.push_back(std::pair(olc::vi2d(pos.x, y), olc::vi2d(dimensions.second.x - offset.x * 2, y)));
      window->Initialize(std::pair(olc::vi2d(pos.x, y) + offset, olc::vi2d(0, 0)));
    };

    Z80->Initialize(std::pair(olc::vi2d(pos.x, zero.y) + offset, zero));

    Init(interrupt, 140); Init(keyboard, 184); Init(ppi, 206); Init(rlt, 228); 

    grid.push_back(std::pair(olc::vi2d(pos.x - offset.x * 2, offset.y), olc::vi2d(pos.x - offset.x * 2, dimensions.second.y - offset.y)));
  }

  void Bus::Preprocess() {
    led       ->Preprocess();
    switches  ->Preprocess();
    hexDisplay->Preprocess();
    lcd       ->Preprocess();
    keyboard  ->Preprocess();
    ppi       ->Preprocess();
    rlt       ->Preprocess();
    interrupt ->Preprocess();
    Z80       ->Preprocess();
  }

  void Bus::Process(PixelGameEngine* GameEngine) {
    led       ->Process(GameEngine);
    switches  ->Process(GameEngine);
    hexDisplay->Process(GameEngine);
    lcd       ->Process(GameEngine);
    keyboard  ->Process(GameEngine);
    ppi       ->Process(GameEngine);
    rlt       ->Process(GameEngine);
    interrupt ->Process(GameEngine);
    Z80       ->Process(GameEngine);
  }

  void Bus::Draw(PixelGameEngine* GameEngine) {
    led       ->Draw(GameEngine);
    switches  ->Draw(GameEngine);
    hexDisplay->Draw(GameEngine);
    lcd       ->Draw(GameEngine);
    keyboard  ->Draw(GameEngine);
    ppi       ->Draw(GameEngine);
    rlt       ->Draw(GameEngine);
    interrupt ->Draw(GameEngine);
    Z80       ->Draw(GameEngine);

    for (auto& line : grid) GameEngine->DrawLine(line.first, line.second, *AnyType<DARK_GREY, ColorT>::GetValue());

    // TODO: Get data from lua
    // GameEngine->FillRect({ 300, 20 }, { 20, 50 }, *AnyType<VERY_DARK_GREY, ColorT>::GetValue());
  }

  void InterruptVector::Interrupt() { bus->Z80->Interrupt(); }
  inline DisassembleT Bus::Disassemble() { return Z80->Disassemble(); }


namespace Z80 {

  inline uint8_t CPU::Read() { return bus->Read(regPC()++, true); }

  inline uint8_t CPU::Read(uint32_t addr, bool mreq) { return bus->Read(addr, mreq); }
  inline uint8_t CPU::Write(uint32_t addr, uint8_t data, bool mreq) { return bus->Write(addr, data, mreq); }

  DisassembleT CPU::Disassemble() {
    DisassembleT dasm = std::pair("", std::unordered_map<int32_t, int32_t>());
  
    for (uint32_t i = 0, line = 0; i < 0xFFFF; line++, i++) {
      std::pair<std::string, int32_t> cmd;
      dasm.second[i] = line;
      std::string unknown = "";

      switch (uint32_t instruction = bus->Read(i, true)) {
        case Z80::MISC_INSTR: {
          AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          std::pair<MiscSTR, int32_t> res = foreach<MiscMInstructions, AnyType<-1, int32_t>>::Key2Value();
          if (res.second == -1) unknown = sprintf("db 0xED, 0x%02x", AnyType<-1, int32_t>::GetValue());

          cmd = std::pair(*res.first, res.second); break;
        }
        
        case Z80::BIT_INSTR: {
          AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          std::pair<BitSTR, int32_t> res = foreach<BitInstructions, AnyType<-1, int32_t>>::Key2Value();
          if (res.second == -1) unknown = sprintf("db 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

          cmd = std::pair(*res.first, res.second); break;
        }

        case Z80::IX_INSTR: {
          instruction = bus->Read(++i, true);

          if (instruction == Z80::IxBitInstr) {
            AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
            std::pair<IxBitSTR, int32_t> res = foreach<IxBitInstructions, AnyType<-1, int32_t>>::Key2Value();
            if (res.second == -1) unknown = sprintf("db 0xDD, 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

            cmd = std::pair(*res.first, res.second); break;
          } 

          AnyType<-1, int32_t>::GetValue() = instruction;
          std::pair<IxSTR, int32_t> res = foreach<IxInstructions, AnyType<-1, int32_t>>::Key2Value();
          if (res.second == -1) unknown = sprintf("db 0xDD, 0x%02x", AnyType<-1, int32_t>::GetValue());

          cmd = std::pair(*res.first, res.second); break;
        }

        case Z80::IY_INSTR: {
          instruction = bus->Read(++i, true);

          if (instruction == Z80::IyBitInstr) {
            AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
            std::pair<IyBitSTR, int32_t> res = foreach<IyBitInstructions, AnyType<-1, int32_t>>::Key2Value();
            if (res.second == -1) unknown = sprintf("db 0xFD, 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

            cmd = std::pair(*res.first, res.second); break;
          } 

          AnyType<-1, int32_t>::GetValue() = instruction;
          std::pair<IySTR, int32_t> res = foreach<IyInstructions, AnyType<-1, int32_t>>::Key2Value();
          if (res.second == -1) unknown = sprintf("db 0xFD, 0x%02x", AnyType<-1, int32_t>::GetValue());

          cmd = std::pair(*res.first, res.second); break;
        }

        default: {
          AnyType<-1, int32_t>::GetValue() = instruction;
          std::pair<InstrSTR, int32_t> res = foreach<Instructions, AnyType<-1, int32_t>>::Key2Value();
          if (res.second == -1) unknown = sprintf("db 0x%02x", AnyType<-1, int32_t>::GetValue());

          cmd = std::pair(*res.first, res.second); break;
        }
      }
      

      if (unknown.size()) { dasm.first += unknown + "\n"; continue; }

      std::string phrase = cmd.first;
      for (int32_t cnt = cmd.second & 0x0F, offset = 1; cnt != 1; offset++) {
        const int32_t bytes = (cmd.second >> (4 * offset)) & 0x0F;
        int32_t word = 0x00; cnt -= bytes;
        for (int32_t j = 0; j < bytes; j++) {
          word = word | (bus->Read(++i, true) << (j * 8));
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
