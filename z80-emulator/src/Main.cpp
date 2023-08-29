#include <stdio.h>
#include <fstream>
#include "Emulator.h"
#include "include/LuaScript.h"

// Check Memory Leaking
//#define MEM_TRACK
//#include "include/MemTrack.h"


class App : public olc::PixelGameEngine {
public:
    App(LuaScript& config): luaConfig(config) {
        sAppName = "Z80 Emulator";
    }

    bool OnUserCreate() override {
        std::ifstream t("../RTC_Test/Test.asm");
        std::stringstream buffer;
        buffer << t.rdbuf();

        // mMinecraft.Init(ScreenHeight(), ScreenWidth(), luaConfig);
        Lexer sLexer = Lexer(buffer.str());
        sLexer.scan();

        printf("%s\n", sLexer.err.c_str());

        for (auto token : sLexer.vTokens) { token.print(); }


        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        // mMinecraft.Update(*this, fElapsedTime);


	    Clear(olc::BLACK);
        // mMinecraft.Draw(*this, fElapsedTime);
        // return mMinecraft.IsFinished();
        return false;
    }

private:
    // Minecraft mMinecraft;
    LuaScript& luaConfig;
};

int main() {
    LuaScript luaConfig;
    if (!luaConfig.Init("src/lua/Config.lua")) return 0;
    if (!luaConfig.GetTable("Init")) return 0;

    const int32_t nScreenWidth = luaConfig.GetTableValue<int32_t>(nullptr, "nScreenWidth");
    const int32_t nScreenHeight = luaConfig.GetTableValue<int32_t>(nullptr, "nScreenHeight");
    const int32_t nPixel = luaConfig.GetTableValue<int32_t>(nullptr, "nPixel");

    App app(luaConfig);
    if (app.Construct(nScreenWidth, nScreenHeight, nPixel, nPixel)) app.Start();
    
    return 0;
}
