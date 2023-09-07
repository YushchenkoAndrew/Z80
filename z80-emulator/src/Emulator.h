#pragma once
#include "lib/olcPixelGameEngine.h"
#include "Interpreter/Interpreter.h"
#include "Bus/Rom.h"

class Emulator {
public:


public:
  Interpreter::Interpreter interpreter;
  Bus::Rom ROM;

};