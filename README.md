# Foxhollow Cheats - Arwing

A native mod that adds optional Arwing cheats to Star Fox Adventures running through Foxhollow.

## Controls

| Key | Action |
| --- | --- |
| 1 | Toggle Arwing God Mode |
| 2 | Toggle Rapid Fire |
| 3 | Toggle Infinite Bombs |
| 4 | Toggle Complete Rings |

- Use the number-row keys. Press a key once to enable the cheat and press it again to disable it. Holding a key does not toggle it repeatedly.
- The hotkeys are only active while flying the Arwing, so they do not interfere with normal player gameplay or the separate Player Cheats mod, which uses the same keys on foot.
- Keys only work while the game window is focused.
- All cheats start off and reset when you leave the current save (returning to the title screen, the save select or a soft reset). Leaving the Arwing, warps and loading screens keep them enabled.
- There is no on-screen display. Every change is written to the Foxhollow log, for example `[Arwing Cheats] God Mode enabled`.

## Features

- **God Mode**: prevents the Arwing from being destroyed by enemy fire, collisions and terrain, including instantly lethal surfaces. Hits still flash and shake the Arwing, but its health is refilled before the game checks for destruction.
- **Rapid Fire**: allows continuous rapid firing by holding the fire button, while preserving the game's native firing cadence.
- **Infinite Bombs**: keeps the Arwing bomb supply at 3. The button press that launches a bomb never detonates it, and a new bomb is not set off by the previous bomb's explosion.
- **Complete Rings**: keeps the Arwing ring count at 10.

## Related cheat mod

This mod can be installed alongside [Cheats - Player](https://github.com/saulob/Foxhollow-Player-Cheats). Both mods use keys 1-4, but their hotkeys do not overlap: Cheats - Player handles them during normal player gameplay, while Cheats - Arwing handles them only while flying the Arwing.

## Installation

**Recommended:** install through the Foxhollow Launcher once the mod is published there.

**Manual:** place the extracted mod folder in the Foxhollow Launcher's `mods` folder, so it looks like this:

```
mods/
  com.saulob.cheats-arwing/
    mod.json
    lib/
      windows-amd64/
        mod.dll
```

Restart the game after installing.

## Platform support

- Windows x64

Other Foxhollow platforms are not supported by this mod yet.

## Repository

https://github.com/saulob/Foxhollow-Arwing-Cheats

## License

[MIT](LICENSE)
