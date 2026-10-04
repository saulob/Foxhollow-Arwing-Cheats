# Foxhollow Cheats - Arwing

A native mod that adds Arwing cheats to Star Fox Adventures running through Foxhollow.

## Controls

| Key | Action |
| --- | --- |
| 6 | Toggle Arwing God Mode |
| 7 | Toggle Rapid Fire |
| 8 | Toggle Infinite Bombs |
| 9 | Toggle Complete Rings |

- Keys 6-9 work on both the number row and the numeric keypad. On Windows, the numpad keys need Num Lock on. On Linux and macOS, the keys are recognized by their position on the keyboard, so numpad 6-9 work with Num Lock on or off.
- Press a key once to enable its cheat and press it again to disable it. Holding a key does not toggle repeatedly; release it and press it again to toggle again. The number-row key and the numpad key for the same number count as one: pressing one while the other is held does not toggle again.
- The hotkeys are only active while flying the Arwing, so they do not interfere with normal player gameplay.
- Keys only work while the Foxhollow window has keyboard focus. A key pressed while the window is in the background is ignored, not saved for later.
- All cheats start off and reset when you leave the current save (returning to the title screen, the save select or a soft reset). Leaving the Arwing, warps and loading screens keep them enabled.
- There is no on-screen display. Every change is written to the Foxhollow log, for example `[Arwing Cheats] God Mode enabled`.

## Features

- **God Mode**: prevents the Arwing from being destroyed by enemy fire, collisions and terrain, including instantly lethal surfaces. Hits still flash and shake the Arwing, but its health is refilled before the game checks for destruction.
- **Rapid Fire**: allows continuous rapid firing by holding the fire button, while preserving the game's native firing cadence.
- **Infinite Bombs**: keeps the Arwing bomb supply at 3. The button press that launches a bomb never detonates it, and a new bomb is not set off by the previous bomb's explosion.
- **Complete Rings**: keeps the Arwing ring count at 10.


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
      linux-amd64/
        mod.so
      linux-arm64/
        mod.so
      macos-x86_64/
        mod.so
      macos-arm64/
        mod.so
```

Foxhollow only loads the library in the folder that matches your system and ignores the others, so you only need the folder for your platform.

Restart the game after installing.

## Platform support

| Platform | Folder | Status |
| --- | --- | --- |
| Windows x64 | `windows-amd64` | Tested in game |
| Linux x86_64 | `linux-amd64` | Build validated by GitHub Actions, in-game testing pending |
| Linux ARM64 | `linux-arm64` | Build validated by GitHub Actions, in-game testing pending |
| macOS Apple Silicon | `macos-arm64` | Build validated by GitHub Actions, in-game testing pending |
| macOS Intel | `macos-x86_64` | Build validated by GitHub Actions, in-game testing pending |

Official Foxhollow builds are currently published for Windows x64, Linux x86_64 and macOS Apple Silicon. The Linux ARM64 and macOS Intel libraries are for Foxhollow builds you compile yourself. Windows on ARM is not supported.

On Linux and macOS, keys 6-9 are read from the keyboard state of Foxhollow's own SDL3 runtime, so the mod needs no extra libraries and behaves the same under X11 and Wayland. If the Foxhollow log shows `[Arwing Cheats] disabled: ...`, the mod could not find the game functions or keyboard input it needs and left the game unchanged. If only some game functions are missing, the log names the affected cheats as unavailable and the others keep working.

## Repository

https://github.com/saulob/Foxhollow-Arwing-Cheats

## License

[MIT](LICENSE)
