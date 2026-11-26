#pragma once

#include "src/Bus/Datasheet.h"
#include <string>
#include <unordered_map>

// Every datasheet there is, found by name: the name written in Lua  part("AND", "D5", "D7").
// Library::Initialize() registers them (Registry::Initialize calls it first, before the Schematic is created, because that loads in its constructor).
// A new datasheet = define it below and add it to Initialize().  Library::Z80 etc. can also be returned by a device's GetDatasheet().
class Library {
public:
  static void Initialize() {
    Add(Z80); Add(KR580VV55); Add(KR580VI53); Add(MC146818); Add(W27C512); Add(IMS1423);
    Add(HY62256A); Add(KM28C17); Add(LS139); Add(K155ID4); Add(HC377); Add(HC244);
    Add(K155LA6); Add(CNT); Add(XTAL); Add(SR); Add(PS2RX); Add(RTC_SM);
    Add(AND); Add(NAND); Add(OR); Add(NOR); Add(NOT); Add(SCN2661AC1N28);
  }

  static const Bus::Datasheet* Find(const std::string& name) {
    const auto it = sheets.find(name);
    return it == sheets.end() ? nullptr : it->second;
  }

private:
  inline static void Add(const Bus::Datasheet& d) { sheets.emplace(d.name, &d); }    // the first one wins if a name is used twice

private:
  inline static std::unordered_map<std::string, const Bus::Datasheet*> sheets;

public:
  inline static const Bus::Datasheet Z80 = Bus::Datasheet("Z80")
    .L("</M1",    27).L("_").L("</MREQ", 19)  .L("</IORQ", 20) .L("</RD", 21)  .L("</WR", 22) .L("_")  .L("</RFSH", 28) .L("_")  .L("</HALT", 18)  .L("_") 
    .L("/WAIT>",  24).L("_").L("/INT>", 16)   .L("/NMI>",  17) .L("_").L("/RESET>", 26)       .L("_")
    .L("/BUSREQ>",25).L("</BUSACK", 23).L("_").L("/CLK>",  6)
    .R("A0>", 30)  .R("A1>", 31)  .R("A2>", 32)  .R("A3>", 33)  .R("A4>", 34)  .R("A5>", 35)  .R("A6>", 36)  .R("A7>", 37)
    .R("A8>", 38)  .R("A9>", 39)  .R("A10>", 40) .R("A11>", 1)  .R("A12>", 2)  .R("A13>", 3)  .R("A14>", 4)  .R("A15>", 5) .R("_")
    .R("<D0>", 14) .R("<D1>", 15) .R("<D2>", 12) .R("<D3>", 8) .R("<D4>", 7) .R("<D5>", 9) .R("<D6>", 10) .R("<D7>", 13);

  inline static const Bus::Datasheet SCN2661AC1N28 = Bus::Datasheet("SCN2661AC1N28").W(12)
    .L("<D0>",27) .L("<D1>",28)  .L("<D2>", 1) .L("<D3>", 2).L("<D4>", 5) .L("<D5>", 6) .L("<D6>", 7) .L("<D7>", 8).L("_")
    .L("A1>", 12) .L("A2>", 10)  .L("_")
    .L("/RD",13)  .L("/CS",11)   .L("SR",21)   .L("CLK", 20) 
    .R("RxD", 3)  .R("/RxRDY",14).R("TxD",29)  .R("/TxRDY", 15).R("_")
    .R("RxC", 9)  .R("TxC",25)   .R("/DTR",24) .R("/RTS",23)  .R("/DSR",22) .R("/CTS",17).R("/DCD",16);

  inline static const Bus::Datasheet KR580VV55 = Bus::Datasheet("KR580VV55").W(12)
    .L("RES>",35) .L("_")
    .L("/RD>", 5) .L("/WR>", 36).L("_")
    .L("/CS>", 6) .L("_")
    .L("A0>",  8) .L("A1>", 9) .L("_")
    .L("<D0>", 34).L("<D1>", 33).L("<D2>", 32).L("<D3>", 31).L("<D4>", 30).L("<D5>", 29).L("<D6>", 28).L("<D7>", 27)  .L("_") .L("_") .L("_") .L("_")
    .R("<PA0>", 4).R("<PA1>", 3).R("<PA2>", 2).R("<PA3>", 1).R("<PA4>",40).R("<PA5>",39).R("<PA6>",38).R("<PA7>",37).R("_")
    .R("<PB0>",18).R("<PB1>",19).R("<PB2>",20).R("<PB3>",21).R("<PB4>",22).R("<PB5>",23).R("<PB6>",24).R("<PB7>",25).R("_")
    .R("<PC0>",14).R("<PC1>",15).R("<PC2>",16).R("<PC3>",17).R("<PC4>",13).R("<PC5>",12).R("<PC6>",11).R("<PC7>",10);

  inline static const Bus::Datasheet KR580VI53 = Bus::Datasheet("KR580VI53").W(12)
    .L("<D0>").L("<D1>").L("<D2>").L("<D3>").L("<D4>").L("<D5>").L("<D6>").L("<D7>").L("_")
    .L("/RD>").L("/WR>").L("_")
    .L("A0>") .L("A1>") .L("_")
    .L("/CS>")
    .R("<CLK0").R("<GATE0").R("OUT0>").R("_")
    .R("<CLK1").R("<GATE1").R("OUT1>").R("_")
    .R("<CLK2").R("<GATE2").R("OUT2>");

  inline static const Bus::Datasheet MC146818 = Bus::Datasheet("MC146818").W(12)
    // left: oscillator, power, bus interface
    .L("OSC1>").L("<OSC2").L("_")
    .L("/CE>") .L("DS>")  .L("RW>")  .L("AS>")  .L("_")
    .L("<AD0>").L("<AD1>").L("<AD2>").L("<AD3>").L("<AD4>").L("<AD5>").L("<AD6>").L("<AD7>")
    // right: clock out, square wave, interrupt, reset
    .R("CKOUT>") .R("<CKFS").R("_")
    .R("SQW>")   .R("_")
    .R("/IRQ>")  .R("_")
    .R("</RESET").R("<PS");

  // ---------------------------------------------------------------- memories
  // 64K x 8 EEPROM
  inline static const Bus::Datasheet W27C512 = Bus::Datasheet("W27C512").W(12)
    .L("A0>", 10) .L("A1>",  9) .L("A2>",   8).L("A3>", 7)  .L("A4>", 6)  .L("A5>",   5).L("A6>",   4) .L("A7>", 3)
    .L("A8>", 25) .L("A9>", 24) .L("A10>", 21).L("A11>", 23).L("A12>", 2) .L("A13>", 26).L("A14>", 27).L("A15>", 1).L("_")
    .R("<Q0>",11) .R("<Q1>",12) .R("<Q2>", 13).R("<Q3>", 15).R("<Q4>", 16).R("<Q5>", 17).R("<Q6>", 18).R("<Q7>", 19).R("_")
    .R("/CE>",20) .R("/OE>",22) .R("_").R("_").R("_").R("_").R("_").R("_");

  // 4K x 4 SRAM (one chip = half a byte)
  inline static const Bus::Datasheet IMS1423 = Bus::Datasheet("IMS1423").W(12)
    .L("A0>") .L("A1>") .L("A2>") .L("A3>") .L("A4>") .L("A5>")
    .L("A6>") .L("A7>") .L("A8>") .L("A9>") .L("A10>").L("A11>").L("_")
    .R("<IO0>").R("<IO1>").R("<IO2>").R("<IO3>").R("_")
    .R("/E>")  .R("/W>")  .R("_")    .R("_")    .R("_")
    .R("_")    .R("_");

  // 32K x 8 SRAM
  inline static const Bus::Datasheet HY62256A = Bus::Datasheet("HY62256A").W(12)
    .L("A0>") . L("A1>")  .L("A2>") . L("A3>")  .L("A4>")  .L("A5>")  .L("A6>")  .L("A7>")
    .L("A8>") . L("A9>")  .L("A10>"). L("A11>") .L("A12>") .L("A13>") .L("A14>") .L("_")
    .R("<IO1>").R("<IO2>").R("<IO3>").R("<IO4>").R("<IO5>").R("<IO6>").R("<IO7>").R("<IO8>")
    .R("/CS>"). R("/WE>") .R("/OE>") .R("_")    .R("_")    .R("_");

  // 2K x 8 EEPROM (no window yet, so the schematic uses this one directly)
  inline static const Bus::Datasheet KM28C17 = Bus::Datasheet("KM28C17").W(12)
    .L("A0>")  .L("A1>")  .L("A2>")  .L("A3>") .L("A4>") .L("A5>")
    .L("A6>")  .L("A7>")  .L("A8>")  .L("A9>") .L("A10>").L("_")
    .L("/CE>") .L("/WE>") .L("/OE>")
    .R("<IO0>").R("<IO1>").R("<IO2>").R("<IO3>").R("<IO4>").R("<IO5>").R("<IO6>").R("<IO7>")
    .R("RDY/BSY>");

  // ---------------------------------------------------------------- logic chips
  // dual 2-to-4 decoder
  inline static const Bus::Datasheet LS139 = Bus::Datasheet("74LS139")
    .L("/1G>") .L("1A>")  .L("1B>").L("_")
    .L("/2G>") .L("2A>")  .L("2B>")
    .R("/1Y0>").R("/1Y1>").R("/1Y2>").R("/1Y3>").R("_")
    .R("/2Y0>").R("/2Y1>").R("/2Y2>").R("/2Y3>");

  // dual 2-to-4 decoder with individual strobes (the SN74155 equivalent)
  inline static const Bus::Datasheet K155ID4 = Bus::Datasheet("K155ID4")
    .L("A>")   .L("B>")   .L("_")
    .L("1C>")  .L("/1G>") .L("_")
    .L("/2C>") .L("/2G>")
    .R("/2Y0>").R("/2Y1>").R("/2Y2>").R("/2Y3>").R("_")
    .R("/1Y0>").R("/1Y1>").R("/1Y2>").R("/1Y3>");

  // octal D flip-flop with enable
  inline static const Bus::Datasheet HC377 = Bus::Datasheet("74HC377")
    .L("/E>").L("CP>").L("_")
    .L("D0>").L("D1>").L("D2>").L("D3>").L("D4>").L("D5>").L("D6>").L("D7>")
    .R("Q0>").R("Q1>").R("Q2>").R("Q3>").R("Q4>").R("Q5>").R("Q6>").R("Q7>");

  // octal buffer, 3-state
  inline static const Bus::Datasheet HC244 = Bus::Datasheet("74HC244")
    .L("/1OE>").L("1A1>").L("1A2>").L("1A3>").L("1A4>").L("_")
    .L("/2OE>").L("2A1>").L("2A2>").L("2A3>").L("2A4>")
    .R("1Y1>") .R("1Y2>").R("1Y3>").R("1Y4>").R("_")
    .R("2Y1>") .R("2Y2>").R("2Y3>").R("2Y4>");

  // dual 4-input NAND (the SN7440 equivalent)
  inline static const Bus::Datasheet K155LA6 = Bus::Datasheet("K155LA6")
    .L("1A>").L("1B>").L("1C>").L("1D>")
    .L("2A>").L("2B>").L("2C>").L("2D>")
    .R("/1Y>").R("/2Y>");

  // ---------------------------------------------------------------- board-specific blocks
  inline static const Bus::Datasheet XTAL   = Bus::Datasheet("CLK")   .R("OUT>");
  inline static const Bus::Datasheet SR     = Bus::Datasheet("SR")    .L("/S>")   .L("/R>")  .R("Q>")       .R("/QN>");
  inline static const Bus::Datasheet PS2RX  = Bus::Datasheet("PS/2")  .L("PS2C>") .L("PS2D>").L("KEYBOARD>").R("INT_PS2>").R("DATA>");
  inline static const Bus::Datasheet CNT    = Bus::Datasheet("CNT")   .L("A>")    .L("_")    .L("_")        .L("_")       .R("QA>")  .R("QB>")   .R("QC>")   .R("QD>");
  inline static const Bus::Datasheet RTC_SM = Bus::Datasheet("RTC_SM").L("MEM12>").L("RD>")  .L("WR>")      .L("CLK>")    .R("AS>")  .R("RLT_R>").R("RLT_W>").R("WAIT>");

  // ---------------------------------------------------------------- common logic
  inline static const Bus::Datasheet AND  = Bus::Datasheet("AND") .L("A>").L("B>").R("Y>");
  inline static const Bus::Datasheet NAND = Bus::Datasheet("NAND").L("A>").L("B>").R("/Y>");
  inline static const Bus::Datasheet OR   = Bus::Datasheet("OR")  .L("A>").L("B>").R("Y>");
  inline static const Bus::Datasheet NOR  = Bus::Datasheet("NOR") .L("A>").L("B>").R("/Y>");
  inline static const Bus::Datasheet NOT  = Bus::Datasheet("NOT") .L("A>").R("/Y>");
};