-- WIDTH = 260 * 2; HIGHT = 300; PIXELS = 2
WIDTH = 700; HIGHT = 412; PIXELS = 2;
WINDOW = { width = WIDTH, height = HIGHT - 24 }

SOURCE = "assets/src/DoS/Main.asm";
-- src = "assets/test/Timer/Test.asm",
-- src = "assets/test/RTC/Test.asm",
-- src = "assets/test/LCD/Test.asm",
CLOCK = 250; -- nano seconds = 4MHz

-- step   = { x = 8, y = 12 };
-- WINDOW = {  }

PANELS = {
  {
    bindings = { x = 0,              y = 0,                     width = 0,                     height = 0 },
    editor   = { x = 0,              y = 0,                     width = WINDOW.width * 7 / 10, height = WINDOW.height },
    W27C512  = { x = WIDTH * 7 / 10, y = 0,                     width = WINDOW.width * 3 / 10, height = WINDOW.height * 3 / 4 },
    IMS1423  = { x = WIDTH * 7 / 10, y = WINDOW.height * 3 / 4, width = WINDOW.width * 3 / 10, height = WINDOW.height * 1 / 4 },
  },
  {
    bus      = { x = 0,              y = 0,                     width = WINDOW.width * 7 / 10, height = WINDOW.height },
    HY62256A = { x = WIDTH * 7 / 10, y = 0,                     width = WINDOW.width * 3 / 10, height = WINDOW.height },
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
