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
        // std::string s = buffer.str();

        // mMinecraft.Init(ScreenHeight(), ScreenWidth(), luaConfig);

        // Interpreter::Lexer lexer = Interpreter::Lexer(buffer.str());
        Interpreter::Interpreter interpreter = Interpreter::Interpreter();

        bool err = interpreter.scan(buffer.str());
        // for (auto token : interpreter.parser.lexer.vTokens) { token->print(); }

        if (err) {
            for (auto err : interpreter.parser.errors) {
                printf("%s", err.c_str());
            }
        }


        printf("%d\n", interpreter.env.memory.size());
        printf("%d\n", interpreter.parser.stmt.size());
        interpreter.env.save("out.bin");

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
	    Clear(olc::BLACK);
        return false;

        // TODO: Text editor
        // std::ifstream t("../RTC_Test/Test.asm");
        // std::stringstream buffer;
        // buffer << t.rdbuf();

        // // mMinecraft.Update(*this, fElapsedTime);
        // Interpreter::Lexer lexer = Interpreter::Lexer(buffer.str());
        // if (lexer.scan()) printf("%s\n", lexer.error().c_str());


        // // this->DrawString(10, 10, buffer.str());
        // for (auto t : lexer.vDst) {
        //     this->DrawString(t->col * 8, t->line * 12, t->lexeme, t->color);
        // }


        // // mMinecraft.Draw(*this, fElapsedTime);
        // // return mMinecraft.IsFinished();
        // return true;
    }

private:
    // Minecraft mMinecraft;
    LuaScript& luaConfig;
};

int main() {
    Interpreter::Defs::Init(); // Initialize types

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
