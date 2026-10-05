# DOOM on TINY-32

Run real DOOM on **TINY-32**, a 32-bit RISC-V computer built entirely from logic gates inside [Tiny Logic](https://whoismuzzy.itch.io/tinylogic).

TINY-32 is a full RV32IM CPU (93,338 transistors) with a program ROM, 32 MB of RAM, a 320x200 palette screen, a 1 kHz timer and 27 keys. It passes the official RISC-V test suite. Nothing is emulated: the circuit you see on the board is the computer, and Tiny Logic's engine compiles it into machine code so it can run DOOM at full speed.

## What's in here

| File | What it is |
|---|---|
| `TINY-32.tlc` | The TINY-32 computer, as a Tiny Logic project |
| `doom.bin` | DOOM compiled for TINY-32 (RV32IM, no operating system), ready to load |
| `source/` | Everything needed to build `doom.bin` yourself |
| `LICENSE` | GPL-2.0, the license of the DOOM source code |

## How to run it

You need [Tiny Logic](https://whoismuzzy.itch.io/tinylogic) installed.

1. **Download this repo.** Click the green **Code** button, then **Download ZIP**, and unzip it.
2. **Open the computer.** Drag `TINY-32.tlc` onto the Tiny Logic window (or onto the main menu). The TINY-32 board opens.
3. **Load DOOM.** Drag `doom.bin` onto the **program ROM**, then drag it onto the **RAM** (the big 32 MB memory). The same file goes into both: the CPU reads its instructions from the ROM and its data from the RAM. You can also click a memory and use **load...** on its card.
4. **Turn it on.** Click the **POWER** switch. DOOM boots on the screen part.
5. **Take control.** Press **K** to hand your keyboard to the circuit, so your key presses go to TINY-32 instead of the editor. Press **K** again to take it back.

The first run takes a few seconds to reach full speed while the engine compiles the board. After that, Tiny Logic keeps the compiled code, so it starts at full speed next time.

## Controls

| Key | In DOOM |
|---|---|
| Arrow keys | Move and turn |
| W / S | Move forward / back |
| A / D | Strafe left / right |
| Ctrl | Fire |
| Space / E | Use (open doors, switches) |
| Shift | Run |
| Alt | Strafe |
| 1 to 7 | Weapons |
| Tab | Map |
| Enter | Confirm in menus |
| ` (the key left of 1) | Menu |
| Y / N | Yes / No |
| - / = | Screen size |

## Build it yourself

`source/` holds the port and its build. You need LLVM's clang and lld with RISC-V support.

```sh
cd source
./fetch.sh      # gets doomgeneric, LLVM's soft-float helpers and the shareware DOOM1.WAD
make PAL=1      # builds build-pal/doom.bin for TINY-32's palette screen
```

- `port/` is the TINY-32 port: startup code, linker script, a small C library, timer and keys (`doomgeneric_tiny32.c`), drawing straight into the palette screen's memory (`i_video_tiny32.c`), and DOOM's two hottest drawing loops tuned for TINY-32 (`r_draw_tiny32.c`).
- `doomgeneric/` is the portable DOOM source, unmodified, from [ozkl/doomgeneric](https://github.com/ozkl/doomgeneric).
- `compiler-rt/` holds seven soft-float helpers from LLVM's compiler-rt, since TINY-32 has no floating-point unit.
- `make XLEN=64` builds the same port for TINY-64, the 64-bit sibling.

## Credits and licenses

- **DOOM** by id Software. The DOOM source code is released under the GNU General Public License v2 (see `LICENSE`). This port and `doom.bin` are distributed under the same license.
- **doomgeneric** by ozkl, GPL-2.0.
- **compiler-rt** helpers from the LLVM Project, Apache License 2.0 with LLVM Exceptions.
- **`doom.bin` contains the shareware episode of DOOM (DOOM1.WAD)**, © id Software. The shareware version is freely distributable and is included here free of charge. It is not sold, and it is not part of the paid Tiny Logic game. If you own the full DOOM, you can build the port with your own WAD.
- **TINY-32** and the TINY-32 port by Mazell Smack-Dubose.

Questions? Add me on Discord at **plangomc**, or DM me on Instagram at [@whoismuzzy](https://www.instagram.com/whoismuzzy/).
