-- WIDTH = 260 * 2; HIGHT = 300; PIXELS = 2
-- WIDTH = 700; HEIGHT = 412; PIXELS = 2;
WIDTH = 840; HEIGHT = 514; PIXELS = 1;
-- WINDOW = { width = WIDTH, height = HEIGHT - 24 }

CLOCK = 0; -- Max
SCHEMATIC = "src/lua/Schematic.lua";
-- CLOCK = 4000000; -- nano seconds = 4MHz
SOURCE = "assets/src/DoS/Main.z80";
-- SOURCE = "assets/test/Test.z80";
-- src = "assets/test/Timer/Test.z80",
-- src = "assets/test/RTC/Test.z80",
-- SOURCE = "assets/test/LCD/Test.z80";

-- step   = { x = 8, y = 12 };
-- WINDOW = {  }

PANELS = {
  {
    filesystem = { x = 0,           y = 0,             width = 140,   height = HEIGHT - 35 },
    editor     = { x = WIDTH - 700, y = 0,             width = 490,   height = HEIGHT - 35, selected = true },
    W27C512    = { x = WIDTH - 210, y = 12,            width = 210,   height = 374 },
    IMS1423    = { x = WIDTH - 210, y = 392,           width = 210,   height = 84 },
  },
  {
    led        = { x = 10,          y = 15,            width = 100,   height = 15 },
    switch     = { x = 10,          y = 30,            width = 100,   height = 15 },
    hex        = { x = 10,          y = 45,            width = 100,   height = 40 },
    lcd        = { x = 10,          y = 85,            width = 350,   height = HEIGHT - 104 },
    Z80        = { x = WIDTH - 350, y = 12,            width = 140,   height = 140, selected = true  },
    irq        = { x = WIDTH - 350, y = 165,           width = 140,   height = 39 },
    keyboard   = { x = WIDTH - 350, y = 205,           width = 140,   height = 33 },
    KR580VV55  = { x = WIDTH - 350, y = 237,           width = 140,   height = 22 },
    KR580VI53  = { x = WIDTH - 350, y = 259,           width = 140,   height = 66 },

    HY62256A   = { x = WIDTH - 210, y = 12,            width = 210,   height = 374 },
    MC146818   = { x = WIDTH - 210, y = 392,           width = 210,   height = 84 },
  },
  {
    schematic  = { x = 0,           y = 0,             width = WIDTH, height = HEIGHT - 35, selected = true },
  }
};
-- }

-- sMenuConfig: assets/Menu.json
-- sMenuAnimated: assets/AnimatedSprite.json
-- sMenuSprite: assets/Sprite-0002.png
-- sMenuSpriteScale: 1
-- nMapSize: 6
-- nMapLoadRange: 2
-- sNoise:
--   nSize: 100
--   fStep: 0.2
-- vLightSource: { x: 10, y: 30, z: 0 }
-- vCamera: { x: 0, y: 50, z: 0 }
-- nCameraStep: 8
-- nMouseSpeed: 0.5
-- MenuSprites:
--   MENU: [8, 2]
