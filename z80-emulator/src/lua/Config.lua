WIDTH = 260 * 2; HIGHT = 300; PIXELS = 2

PanelEnum = {
  editor = "editor",
  memory = "memory",
  lines  = "lines",
  bus    = "bus",
}

Init = {
  nScreenWidth = WIDTH,
  nScreenHeight = HIGHT,
  nPixel = PIXELS,

  vPanels = {
    {
      { PanelEnum.editor, { 0, 0 }, { 1, 1 } },
      { PanelEnum.memory, { 0, 0 }, { 1, 1 } },
      { PanelEnum.lines,  { 0, 0 }, { 1, 1 } },
    }
  },

  sColors = {
    black = 0x0000,
    gray = 0x000000,
  }
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
