-- WIDTH = 260 * 2; HIGHT = 300; PIXELS = 2
WIDTH = 700; HIGHT = 412; PIXELS = 2

Init = {
  width = WIDTH,
  height = HIGHT,
  pixel = PIXELS,

  -- src = "assets/src/DoS/Main.asm",
  src = "assets/test/Timer/Test.asm",
  -- src = "assets/test/LCD/Test.asm",
  clock = 250, -- nano seconds = 4MHz

  -- TODO: Impl this
  schematic = {},
}

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
