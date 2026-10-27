-- WIDTH = 260 * 2; HIGHT = 300; PIXELS = 2
WIDTH = 700; HIGHT = 412; PIXELS = 2;
WINDOW = { width = WIDTH, height = HIGHT - 24 }

CLOCK = 250; -- nano seconds = 4MHz
SOURCE = "assets/src/DoS/Main.asm";
-- SOURCE = "assets/test/Test.asm";
-- src = "assets/test/Timer/Test.asm",
-- src = "assets/test/RTC/Test.asm",
-- src = "assets/test/LCD/Test.asm",

-- step   = { x = 8, y = 12 };
-- WINDOW = {  }

PANELS = {
  {
    editor   = { x = 0,               y = 0,                      width = WINDOW.width * 7 // 10, height = WINDOW.height },
    W27C512  = { x = WIDTH * 7 // 10, y = 0,                      width = WINDOW.width * 3 // 10, height = WINDOW.height * 3 // 4 },
    IMS1423  = { x = WIDTH * 7 // 10, y = WINDOW.height * 3 // 4, width = WINDOW.width * 3 // 10, height = WINDOW.height * 1 // 4 },
    bindings = { x = 0,               y = 0,                      width = 0,                      height = 0 },
  },
  {
    -- bus      = { x = 0,              y = 0,                     width = WINDOW.width * 7 / 10, height = WINDOW.height },
    led      = { x = 10,             y = 15,                      width = WINDOW.width * 7 // 10, height = 15 },
    switch   = { x = 10,             y = 30,                      width = WINDOW.width * 7 // 10, height = 15 },
    hex      = { x = 10,             y = 45,                      width = WINDOW.width * 7 // 10, height = 40 },
    lcd      = { x = 10,             y = 85,                      width = WINDOW.width * 5 // 10, height = WINDOW.height - 80 },
    Z80      = { x = WIDTH * 5 // 10, y = 15,                     width = WINDOW.width * 2 // 10, height = 140 },
    irq      = { x = WIDTH * 5 // 10, y = 155,                    width = WINDOW.width * 2 // 10, height = 39 },
    keyboard = { x = WIDTH * 5 // 10, y = 194,                    width = WINDOW.width * 2 // 10, height = 22 },
    KR580VV55= { x = WIDTH * 5 // 10, y = 216,                    width = WINDOW.width * 2 // 10, height = 22 },
    KR580VI53= { x = WIDTH * 5 // 10, y = 238,                    width = WINDOW.width * 2 // 10, height = 66 },
    MC146818 = { x = WIDTH * 5 // 10, y = 304,                    width = WINDOW.width * 2 // 10, height = WINDOW.height - 304 },
    HY62256A = { x = WIDTH * 7 // 10, y = 0,                      width = WINDOW.width * 3 // 10, height = WINDOW.height  },

  --     //   olc::vi2d zero = olc::vi2d(0, 0);
  -- //   olc::vi2d offset = olc::vi2d(4, 8);
  -- //   grid.clear();

  -- //   // TODO: Design and draw bus as PCB
  -- //   led->Select(std::pair(olc::vi2d(10, 10), zero));
  -- //   switches->Select(std::pair(olc::vi2d(10, 25), zero));
  -- //   hexDisplay->Select(std::pair(olc::vi2d(10, 40), zero));

  -- //   lcd->Select(std::pair(olc::vi2d(10, 150), zero));


  -- //   olc::vi2d pos = olc::vi2d(dimensions.second.x * 7 / 10, dimensions.second.y);
  -- //   olc::vi2d size = dimensions.second - pos;

  -- //   auto Init = [&pos, &dimensions, &offset, &grid = grid](std::shared_ptr<Window> window, int32_t y, olc::vi2d size = olc::vi2d()) {
  -- //     grid.push_back(std::pair(olc::vi2d(pos.x, y), olc::vi2d(dimensions.second.x - offset.x * 2, y)));
  -- //     window->Select(std::pair(olc::vi2d(pos.x, y) + offset, size));
  -- //   };

  -- //   Z80->Select(std::pair(olc::vi2d(pos.x, zero.y) + offset, zero));

  -- //   Init(interrupt, 140); Init(keyboard, 184); Init(KR580VV55, 206); Init(KR580VI53, 228); 
  -- //   Init(MC146818, 294, olc::vi2d(size.x, 88)); 

  -- //   grid.push_back(std::pair(olc::vi2d(pos.x - offset.x * 2, offset.y), olc::vi2d(pos.x - offset.x * 2, dimensions.second.y - offset.y)));
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
