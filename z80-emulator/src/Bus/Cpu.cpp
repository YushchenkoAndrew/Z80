#include "Bus.h"

namespace Bus {
namespace Z80 {

  inline uint8_t CPU::Read() { return bus->Read(regPC()++, true); }

  inline uint8_t CPU::Read(uint32_t addr, bool mreq) { return bus->Read(addr, mreq); }
  inline uint8_t CPU::Write(uint32_t addr, uint8_t data, bool mreq) { return bus->Write(addr, data, mreq); }

  DisassembleT CPU::Disassemble() {
    DisassembleT dasm = std::pair("", std::unordered_map<int32_t, int32_t>());
  
    for (uint32_t i = 0, line = 0; i < 0xFFFF; line++, i++) {
      std::pair<std::string, int32_t> cmd;
      dasm.second[i] = line;

      switch (auto instruction = bus->Read(i, true)) {
        case Z80::MISC_INSTR: {
          AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          auto res = foreach<MiscMInstructions, AnyType<-1, int32_t>>::Key2Value();

          cmd = std::pair(~res.first, res.second); break;
        }
        
        case Z80::BIT_INSTR: {
          AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
          auto res = foreach<BitInstructions, AnyType<-1, int32_t>>::Key2Value();

          cmd = std::pair(~res.first, res.second); break;
        }

        case Z80::IX_INSTR: {
          instruction = bus->Read(++i, true);

          if (instruction == Z80::IxBitInstr) {
            AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
            auto res = foreach<IxBitInstructions, AnyType<-1, int32_t>>::Key2Value();

            cmd = std::pair(~res.first, res.second); break;
          } 

          AnyType<-1, int32_t>::GetValue() = instruction;
          auto res = foreach<IxInstructions, AnyType<-1, int32_t>>::Key2Value();

          cmd = std::pair(~res.first, res.second); break;
        }

        case Z80::IY_INSTR: {
          instruction = bus->Read(++i, true);

          if (instruction == Z80::IyBitInstr) {
            AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
            auto res = foreach<IyBitInstructions, AnyType<-1, int32_t>>::Key2Value();

            cmd = std::pair(~res.first, res.second); break;
          } 

          AnyType<-1, int32_t>::GetValue() = instruction;
          auto res = foreach<IyInstructions, AnyType<-1, int32_t>>::Key2Value();

          cmd = std::pair(~res.first, res.second); break;
        }

        default: {
          AnyType<-1, int32_t>::GetValue() = instruction;
          auto res = foreach<Instructions, AnyType<-1, int32_t>>::Key2Value();
          cmd = std::pair(~res.first, res.second); break;
        }
      }
      

      if (cmd.second == 1) { dasm.first += cmd.first + "\n"; continue; }

      int32_t word = 0x00;
      for (int32_t j = 0; j < cmd.second - 1; j++) {
        word = word | (bus->Read(++i, true) << (j * 8));
      }
      
      dasm.first += sprintf(cmd.first, word) + "\n";
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