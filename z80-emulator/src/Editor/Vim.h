#pragma once
#include "Cursor.h"


namespace Editor {

class Vim {
public:
  void Process(olc::PixelGameEngine* GameEngine) {
    // TODO:
    if (GameEngine->GetKey(olc::Key::I).bPressed) {
      AnyType<-1, ModeT>::GetValue() = INSERT;
    }

    if (GameEngine->GetKey(olc::Key::A).bPressed) {
      AnyType<-1, ModeT>::GetValue() = INSERT;
    }

    if (GameEngine->GetKey(olc::Key::ESCAPE).bPressed) {
      AnyType<-1, ModeT>::GetValue() = NORMAL;
    }


    cursor.Process(GameEngine);
  }


protected:
  Cursor cursor;
};

};