--[[ ===========================================================================
  Z80 BOARD SCHEMATIC  -  wiring and placement for the emulator's schematic window
  ---------------------------------------------------------------------------
  Transcribed from the hand-drawn iPad schematic.  A "?" means I could not read
  that part of the drawing reliably - please check those against the paper.

  HOW IT WORKS
    * The little toolbox right below collects what is written in this file; SCHEMATIC.build() (called by C++ once the
      whole file and config.lua have run) turns it into the tables C++ reads. Nothing is checked here: C++ prints
      unknown part types, parts and pins, and pins that are in two nets, on stderr when it loads.
    * What a chip looks like and what pins it has comes from its datasheet (Bus::Datasheets, found by name).
    * part("AND", "D5", "D7")      WHICH chips are on the board: a datasheet name and the ids ("id=text" adds text to the title)
      at("D5", x, y)               WHERE a chip is (pixels)
      net("RD", { pins = {...}, routes = {...} })   WHAT IS CONNECTED: a net is its name + its pins + the routes drawn between them
      wire("RD", "D1.RD", "D2.OE", "D3.OE")         short form of net(): a route from the first pin to each of the others
                                                    (no name = automatic one; last argument { dx = 150 } = distance of the vertical leg)
      tags("D1", "RD WR A0..15 CS=IO2")             a net with a single pin (drawn as a stub with the name) on many pins of one chip;
                                                    "PIN=NET" renames, "A0..15" / "Q0..7=IE0..7" are ranges
      note(x, y, "text")           free text on the sheet
    * Pin names may be written with or without the datasheet marks: "D5.<A", "D5.A>" and "D5.A" are the same pin.
    * Net names are the signal names from the drawing (RD, IO8, INT7, CS1 ...).  The same names are
      used for live signals in C++:  schematic->Bind("INT7", ...).
    * Drag parts in the viewer, then paste the block printed by  Layout > Print moved parts  at the bottom
      of this file / into config.lua.  It overrides the at(...) positions.

  ADDRESS MAP (from the decoders below)
    memory   0000-3FFF ROM W27C512 (MEM0)      4000-5FFF SRAM IMS1423 (MEM10/MEM11)
             6000      RTC MC146818 (MEM12)    8000 SRAM HY62256A (MEM2)     C000 EEPROM KM28C17 (MEM3)
    ports    10 HEX   20-23 PPI (IO2)   30 UART (IO3)   40 KEYBOARD (IO4)   50 INT status / IE mask (IO5)
             80-83 PIT (IO8)

  STILL UNREAD ("?" in the drawing):  RTC ack path, INT1/INT0 sources, RTC wait-state machine details,
  PS/2 receiver gates, a few memory chip enables, polarity of INTx into the INT line gates.
=========================================================================== ]]

--==========================================================================
-- TOOLBOX
--==========================================================================

local parts, placed, notes, nets = {}, {}, {}, {}

local function part(kind, ...)                           -- part("AND", "D5", "D7=label")
  for _, token in ipairs({ ... }) do
    local id, label = token:match("^([^=]+)=?(.*)$")
    parts[#parts + 1] = { id = id, type = kind, label = label }
  end
end

local function at(id, x, y)   placed[id] = { x = x, y = y } end                        -- at("D1", 760, 60)
local function note(x, y, text) notes[#notes + 1] = { x = x, y = y, text = text } end -- note(x, y, "text")

local function net(name, spec)                           -- net("RD", { pins = { "D1.RD", "D2.OE" }, routes = { { from = "D1.RD", to = "D2.OE", dx = 150 } } })
  local n = nets[name] or { pins = {}, routes = {} }     -- the same name again adds to the net
  nets[name] = n
  for _, p in ipairs(spec.pins or {})   do n.pins[#n.pins + 1] = p end
  for _, r in ipairs(spec.routes or {}) do n.routes[#n.routes + 1] = r end
end

local auto = 0
local function expand(token)                             -- "A0..15" -> A0 ... A15, "D3" -> D3
  local base, lo, hi = token:match("^(.-)(%d+)%.%.(%d+)$")
  if not base then return { token } end
  local out = {}
  for i = tonumber(lo), tonumber(hi) do out[#out + 1] = base .. i end
  return out
end

local function isPin(s) return (s:gsub("%.%.%d+", "")):find(".", 1, true) ~= nil end   -- "D1.A0..15" is a pin range, "A0..15" is a net name

local function wire(...)
  local args = { ... }
  local opts = type(args[#args]) == "table" and table.remove(args) or {}
  local names                                            -- optional first argument: the net name ("A0..15" = one name per route)
  if args[1] and not args[1]:find("=", 1, true) and not isPin(args[1]) then names = expand(table.remove(args, 1)) end

  local k = 0
  local function connect(a, b)                           -- one route a -> b, in the named net or in a net of its own
    k = k + 1
    local name = names and (names[k] or names[1])
    if not name then repeat auto = auto + 1 until not nets["N" .. auto]; name = "N" .. auto end
    net(name, { pins = { a, b }, routes = { { from = a, to = b, dx = opts.dx } } })
  end

  local first                                            -- old form: the first pin to each of the others
  for _, arg in ipairs(args) do
    for token in arg:gmatch("%S+") do
      local l, r = token:match("^([^=]+)=(.+)$")
      if l then
        local from, to = expand(l), expand(r)
        for i = 1, math.max(#from, #to) do connect(from[i] or from[1], to[i] or to[1]) end
      elseif not first then first = token
      else connect(first, token) end
    end
  end
end

local function tags(chip, spec)                          -- tags("D7", "RES=RESET WR RD Q0..7=IE0..7")
  for token in spec:gmatch("%S+") do
    local pin, netname = token:match("^([^=]+)=(.+)$")
    local pins, names = expand(pin or token), expand(netname or pin or token)
    for i, p in ipairs(pins) do net(names[i] or names[1], { pins = { chip .. "." .. p } }) end
  end
end

--==========================================================================
-- 1. RESET + CLOCK
--==========================================================================
note(120, 20, "RESET / CLOCK")
part("NOT", "D13a", "D13b");  part("CLK", "X1=32MHz");  part("CNT", "D12=K555IE2 /8", "D27=K155IE5 /8")
at("D13a", 200, 80);  at("D13b", 380, 80)               -- Schmitt inverter pair: button + RC -> clean RESET
tags("D13a", "A=RESET_BTN"); wire("RESET_N", "D13a.Y", "D13b.A"); tags("D13b", "Y=RESET")
note(200, 112, "button + RC (27k / 220pF) on D13a.A")

at("X1", 200, 200);  at("D12", 420, 180);  at("D27", 420, 300)
wire("CLK32", "X1.OUT", "D12.A"); tags("D12", "QC=CLK"); note(120, 150, "32MHz crystal oscillator")
tags("D27", "A=CLK QD=CLK8")                            -- CLK8 = clock for the PIT counters
note(120, 400, "QC = 4MHz CLK;  CLK/8 = 500kHz  (which Q gives /8: ?)")

--==========================================================================
-- 2. CPU
--==========================================================================
note(760, 32, "^CPU")
part("Z80", "D1") at("D1", 760, 60) tags("D1", "RESET CLK RD WR MREQ IORQ WAIT INT NMI=VCC_pullup A0..15 D0..7")

--==========================================================================
-- 3. IO DECODE   D9 / D10 (K155ID4 as 3-to-8):  A4..A6 pick the 16-port block, A7 picks the half
--      ports 00-7F -> IO0..IO7        ports 80-FF -> IO8..IOF
--      enable (1G+2G) = OR(IORQ, A7)  for D9,   OR(IORQ, not A7)  for D10
--      outputs 0-3 are 2Y0..2Y3, outputs 4-7 are 1Y0..1Y3
--==========================================================================
-- note(120, 440, "IO DECODE (A4..A6 pick the 16-port block, A7 the half)")

part("OR", "D16a", "D16b");  part("NOT", "D20"); 

part("K155ID4", "D9");  at("D9",  400, 400); tags("D9",  "A=A4 B=A5 1C=A6 2C=A6 2Y0=IO0 2Y1=IO1 2Y2=IO2 2Y3=IO3 1Y0=IO4 1Y1=IO5 1Y2=IO6 1Y3=IO7")
part("K155ID4", "D10"); at("D10", 400, 570); tags("D10", "A=A4 B=A5 1C=A6 2C=A6 2Y0=IO8 2Y1=IO9 2Y2=IOA 2Y3=IOB 1Y0=IOC 1Y1=IOD 1Y2=IOE 1Y3=IOF")

at("D16a", 200, 480);  at("D20", 200, 580);  at("D16b", 200, 640)
tags("D16a", "A=IORQ B=A7"); tags("D20", "A=A7"); tags("D16b", "A=IORQ")
wire("A7_N", "D20.Y", "D16b.B", { dx = -150 })

wire("D9_EN", "D16a.Y", "D9.1G", { dx = 70 });   wire("D9_EN", "D16a.Y", "D9.2G", { dx = 90 })
wire("D10_EN", "D16b.Y", "D10.1G", { dx = 70 }); wire("D10_EN", "D16b.Y", "D10.2G", { dx = 90 })

--==========================================================================
-- 4. MEMORY DECODE   D11 (74LS139)
--      1 :  A15:A14 -> MEM0..MEM3   (enabled by MREQ)
--      2 :  A13:A12 -> MEM10..MEM13 (only inside MEM1 = 4000-7FFF)
--==========================================================================
note(1150, 32, "^ADDR=0000")
part("W27C512", "D2");    at("D2",    1150, 60); tags("D2", "A0..13 Q0..7=D0..7 OE=RD CE=MEM0 A14=AA0 A15=AA1"); 
part("74LS139", "D11");   at("D11",   1150,350); tags("D11", "1A=A14 1B=A15 1G=MREQ 1Y0=MEM0 1Y1=MEM1 1Y2=MEM2 1Y3=MEM3 2A=A12 2B=A13 2G=MEM1 2Y1=MEM11 2Y2=MEM12 2Y3=MEM13")

note(1450, 32, "^ADDR=4000")
part("IMS1423", "D5(1)"); at("D5(1)", 1450, 60); tags("D5(1)", "A0..11 IO0..3=D0..3 W=WR E=MEM11");
part("IMS1423", "D5(2)"); at("D5(2)", 1450,280); tags("D5(2)", "A0..11 IO0..3=D4..7 W=WR E=MEM11")

note(1750, 32, "^ADDR=5000")
part("IMS1423", "D12(1)");at("D12(1)",1750, 60); tags("D12(1)","A0..11 IO0..3=D0..3 W=WR E=MEM12");
part("IMS1423", "D12(2)");at("D12(2)",1750,280); tags("D12(2)","A0..11 IO0..3=D4..7 W=WR E=MEM12")

note(2050, 32, "^ADDR=8000")
part("HY62256A","D3");   at("D3",    2050, 60); tags("D3", "A0..11 IO1..8=D0..7 CS=MEM12 OE=RD WE=WR");

note(2050,318, "^ADDR=C000")
part("KM28C17", "D4");   at("D4",    2050, 350);tags("D4", "A0..10 IO0..7=D0..7 CE=MEM13 OE=RD WE=WR");

--==========================================================================
-- 5. PPI  D7 (KR580VV55A), port 20-23 = IO2.   Port C bits are the board's control lines:
--      C0 CS0 (PIT gate 0)   C1 CS1 (PIT gate 1+2)   C2 CSA   C3 CSW   C7 CSB
--      C4..C6 go through D14 (decoder) -> CE1..CE7   (C4 alone = LCD enable)
--==========================================================================
note(760, 32, "^CPU")
part("Z80", "D1") at("D1", 760, 60) tags("D1", "RESET CLK RD WR MREQ IORQ WAIT INT NMI=VCC_pullup A0..15 D0..7")

-- note(760, 32, "PPI  (port 0x20..0x23 = IO2)")

part("KR580VV55", "D7"); at("D7", 760, 480); tags("D7", "RES=RESET WR RD CS=IO2 A0 A1 PC0=CS0 PC1=CS1 PC2=CSA PC3=CSW PC7=CSB")

part("K155ID4", "D14")   at("D14",1000,700)

-- wire("C4", "D7.PC4", "D14.A", { dx = 30 });  wire("C5", "D7.PC5", "D14.B", { dx = 50 })
-- wire("C6", "D7.PC6", "D14.1C", { dx = 70 }); wire("C6", "D7.PC6", "D14.2C", { dx = 90 })
-- tags("D14", "1G=CE_EN? 2G=CE_EN? 2Y1=CE1 2Y2=CE2 2Y3=CE3 1Y0=CE4 1Y1=CE5 1Y2=CE6 1Y3=CE7")
-- note(2000, 200, "C4 alone = LCD enable (CE1)")

--==========================================================================
-- 6. PIT  D54 (KR580VI53), port 80-83 = IO8.  All three CLK inputs = CLK8 (500 kHz).
--      GATE0 = CS0,  GATE1 and GATE2 BOTH = CS1  (one gate for both timers!)
--      OUT1 / OUT2 are the timer interrupt requests (CT_IRQ1 / CT_IRQ2)
--==========================================================================
note(1650, 412, "PIT  (0x80 = IO8)")
part("KR580VI53", "D54=IO8")
at("D54", 1650, 440)
tags("D54", "CS=IO8 RD WR A0 A1 CLK0=CLK8 CLK1=CLK8 CLK2=CLK8 GATE0=CS0 GATE1=CS1 GATE2=CS1 OUT0=CT_OUT0 OUT1=CT_IRQ1 OUT2=CT_IRQ2")
note(1650, 628, "GATE1 and GATE2 share CS1 (one gate for both timers)")
note(1650, 640, "OUT0 = speaker? (drawn label unclear)")

--==========================================================================
-- 7. RTC  D42 (MC146818) at MEM12 = 6000, with a wait-state machine (only a sketch in the drawing)
--==========================================================================
-- note(1650, 672, "RTC  (MEM12 = 0x6000)")
-- part("MC146818", "D42=MEM12");  part("RTC_SM", "RTC_SM")
-- at("D42", 1650, 700);  at("RTC_SM", 2000, 700)
-- tags("D42", "CE=MEM12 DS=RLT_R RW=RLT_W AS AD0..7 RESET IRQ=IRLT")
-- tags("RTC_SM", "MEM12 RD WR CLK AS RLT_R RLT_W WAIT")
-- note(2000, 780, "? gate-level detail not transcribed (74HC00/32, K155IE5 counter,")
-- note(2000, 792, "? 74HC157 muxes for address/data) - 'useless, breaks the system' note on D45")

--==========================================================================
-- 8. KEYBOARD   PS/2 receiver (one byte, interrupt INT_PS2)  +  IN 0x40 strobe
--==========================================================================
-- note(2000, 272, "KEYBOARD  (PS/2, set 2)")
-- part("PS/2", "PS2");  part("NOR", "KB=IN 0x40")
-- at("PS2", 2000, 300);  at("KB", 2000, 460)              -- KB: RD + IO4 -> KEYBOARD (also the INT7 ack)
-- tags("PS2", "PS2C PS2D KEYBOARD INT_PS2 DATA=D[7:0]")
-- tags("KB", "A=RD B=IO4 Y=KEYBOARD")
-- note(2000, 520, "? 74HC595 + counter + SR-edge logic")
-- note(2000, 532, "  (CNT=11) collapsed into this block")

--==========================================================================
-- 9. INTERRUPT LATCHES  (74LS00 SR latches, one row per interrupt)
--
--      source --[NOT]--+                      +-----------+
--                      +--[NAND R]------------| R       Q |-- INTn
--      IEn (mask) -----+                      |           |
--                                             |           |
--      RESET ----------+                      |           |
--                      +--[NAND S]------------| S      /Q |-- nINTn
--      ack ------------+                      +-----------+
--
--      R gate: the MASK (IEn) gates the request BEFORE the latch
--      S gate: ack clears the request.  CT1 and CT2 share the same ack: IO8 = any access to 0x80-0x8F
--==========================================================================
note(2500, 20, "INTERRUPT LATCHES (74LS00 SR latches).  IEn = mask bit, ack = strobe that clears the request")

-- interrupt_channel(n, x, y, { title = text, source = net, ack = net, inverted = true if the source is active-low })
local function interrupt_channel(n, x, y, c)
  local sg, rg, lt = "SG" .. n, "RG" .. n, "L" .. n
  part("NAND", sg .. "=S", rg .. "=R"); part("SR", lt .. "=INT" .. n .. " " .. c.title)
  if c.inverted then part("NOT", "IV" .. n) end
  at(sg, x + 220, y); at(rg, x + 220, y + 60); at(lt, x + 460, y + 30)
  tags(sg, "A=RESET B=" .. c.ack)
  wire("S" .. n, sg .. ".Y", lt .. ".S")
  wire("R" .. n, rg .. ".Y", lt .. ".R")
  tags(rg, "B=IE" .. n)
  if c.inverted then
    at("IV" .. n, x, y + 60)
    tags("IV" .. n, "A=" .. c.source)
    wire(c.source .. "_N", "IV" .. n .. ".Y", rg .. ".A")
  else
    tags(rg, "A=" .. c.source)
  end
  tags(lt, "Q=INT" .. n .. " QN=nINT" .. n)
end

--                    n   x     y    source / ack net
interrupt_channel(7, 2500,  60, { title = "KEYBOARD", source = "INT_PS2", ack = "KEYBOARD" })
interrupt_channel(6, 2500, 190, { title = "RxRDY", source = "RxRDY",   ack = "RxRST" })
interrupt_channel(5, 2500, 320, { title = "TxRDY", source = "TxRDY",   ack = "TxRST" })
interrupt_channel(4, 2500, 450, { title = "RTC", source = "IRLT",    ack = "RTC_ACK?", inverted = true })
interrupt_channel(3, 3250,  60, { title = "CT1", source = "CT_IRQ1", ack = "IO8",      inverted = true })
interrupt_channel(2, 3250, 190, { title = "CT2", source = "CT_IRQ2", ack = "IO8",      inverted = true })
interrupt_channel(1, 3250, 320, { title = "?", source = "SRC1?",   ack = "ACK1?" })
interrupt_channel(0, 3250, 450, { title = "?", source = "SRC0?",   ack = "ACK0?" })
note(3250, 580, "INT3/INT2 share one S gate fed by IO8: any access to 0x80-0x8F acks BOTH timers")
note(2500, 580, "RTC ack path (RTC_ACK?) and INT1/INT0 sources were not legible")

--==========================================================================
-- 10. IE register D30 (OUT 0x50)  -  the interrupt MASK bits IE0..IE7
--==========================================================================
note(2500, 700, "IE REGISTER  (OUT 0x50)")
part("NOR", "IEW=WR/IO5");  part("74HC377", "D30=IE (OUT 0x50)")
at("IEW", 2500, 740);  at("D30", 2800, 720)
tags("IEW", "A=WR B=IO5"); wire("IE_CLK", "IEW.Y", "D30.CP")
tags("D30", "E=GND D0..7 Q0..7=IE0..7")

--==========================================================================
-- 11. INT LINE (to the CPU) and INT STATUS (IN 0x50)
--      The latch outputs INT7..INT0 are NOT gated by the mask here:
--      D37 (two NAND4) -> OR -> NOT -> INT       and       D31 (74HC244) puts INT7..INT0 on the data bus
--==========================================================================
note(4000, 20, "INT LINE  (D37 2xNAND4 -> OR -> NOT)  (polarity of INTx inputs not verified)")
part("K155LA6", "D37");  part("OR", "D44");  part("NOT", "D50")
at("D37", 4000, 60);  at("D44", 4250, 80);  at("D50", 4450, 80)
tags("D37", "1A=INT7 1B=INT6 1C=INT5 1D=INT4 2A=INT3 2B=INT2 2C=INT1 2D=INT0")
wire("N4A", "D37.1Y", "D44.A", { dx = 70 }); wire("N4B", "D37.2Y", "D44.B", { dx = 40 })
wire("INT_OR", "D44.Y", "D50.A"); tags("D50", "Y=INT")

note(4000, 260, "INT STATUS  (IN 0x50)")
part("NOR", "INTR=RD/IO5");  part("74HC244", "D31=IN 0x50")
at("INTR", 4000, 300);  at("D31", 4250, 290)            -- RD + IO5 -> output enable of D31
tags("INTR", "A=RD B=IO5")
wire("INT_RD", "INTR.Y", "D31.1OE", { dx = 40 }); wire("INT_RD", "INTR.Y", "D31.2OE", { dx = 40 })
tags("D31", "1A1=INT7 1A2=INT6 1A3=INT5 1A4=INT4 2A1=INT3 2A2=INT2 2A3=INT1 2A4=INT0 1Y1=D7 1Y2=D6 1Y3=D5 1Y4=D4 2Y1=D3 2Y2=D2 2Y3=D1 2Y4=D0")
note(4000, 440, "INT status and INT line are NOT gated by the mask")

-- the tables C++ reads (see Schematic.h):
--   SCHEMATIC.parts = { { id, type, title, x, y, bx, by } }   x,y = where the chip is now, bx,by = where this file put it (both nil: not placed)
--   SCHEMATIC.nets  = { RD = { pins = { "D1.RD", ... }, routes = { { from = "D1.RD", to = "D2.OE", dx = 150 }, ... } } }
--   SCHEMATIC.notes = { { x, y, text } }
-- positions: SCHEMATIC_LAYOUT (pasted from Layout > Print moved parts) over at(). A chip with no position at all has no x, y: C++ parks it
-- in a column to the right (it knows the sizes), and a chip that is only in SCHEMATIC_LAYOUT has x, y but no bx, by.
SCHEMATIC = { parts = {}, nets = nets, notes = notes }

for _, p in ipairs(parts) do
  local now = placed[p.id]
  SCHEMATIC.parts[#SCHEMATIC.parts + 1] = {
    id = p.id, type = p.type, title = p.id .. " " .. p.type .. (p.label ~= "" and " " .. p.label or ""),
    x = now and now.x, y = now and now.y, bx = b and b.x, by = b and b.y,
  }
end

--==========================================================================
-- Dragged parts: paste the block printed by  Layout > Print moved parts  here
--   SCHEMATIC_LAYOUT = {
--     D54 = { x = 1700, y = 440 },
--   }
--==========================================================================