# Pokemon Phoenix Red

A Pokémon battle game built in pure C using [Raylib](https://www.raylib.com/). Features all 151 Generation 1 Pokémon with Generation 3 GBA front & back sprites, full party management, 720p HD native resolution, sequential turn execution, typewriter dialogue animations, and real-time matchup inspection.

---

## Quick Start (How to Play)

### Windows (Instant Play — No Setup Required)
1. Clone or download this repository as a ZIP:
   ```bash
   git clone https://github.com/knifenub/PokemonPhoenixRed.git
   ```
2. Open the folder and double-click **`pokemon.exe`**.
3. Enjoy the battle!

---

### Linux
You have two easy ways to play on Linux:

#### Option A: Run via Wine (Zero Compiling)
If you have Wine installed, simply run:
```bash
wine pokemon.exe
```

#### Option B: Compile Natively
1. Install Raylib using your package manager:
   * **Ubuntu / Debian / Linux Mint:**
     ```bash
     sudo apt install libraylib-dev
     ```
   * **Arch Linux / Manjaro / SteamOS:**
     ```bash
     sudo pacman -S raylib
     ```
   * **Fedora:**
     ```bash
     sudo dnf install raylib-devel
     ```
2. Build and launch:
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

---

### macOS
1. Install Raylib using [Homebrew](https://brew.sh/):
   ```bash
   brew install raylib
   ```
2. Compile and run:
   ```bash
   clang src/*.c -o pokemon -Isrc $(pkg-config --cflags --libs raylib)
   ./pokemon
   ```

---

## Game Features

* **All 151 Gen 1 Pokémon:** Complete stats (HP, Attack, Defense, Sp. Atk, Sp. Def, Speed), typings, and authentic Gen 3 GBA front/back sprites.
* **Team Builder:** Draft a team of 1 to 6 Pokémon before battle or click "Random Team" for instant action.
* **720p HD Native Canvas:** Clean 16:9 HD resolution (1280×720) with crisp bilinear texture scaling and borderless Fullscreen support (`F11`).
* **Sequential Turn Execution:** Turn actions execute step-by-step: move announcements &rarr; hit-blink reaction &rarr; steady HP bar draining &rarr; effectiveness feedback &rarr; faint handling.
* **In-Place Inspection:**
  * **Moves:** Hovering over any move in the `FIGHT` menu displays its Base Power, Accuracy, PP, Category (`Physical`/`Special`/`Status`), secondary status chance, and live matchup effectiveness against the opponent (`SUPER EFFECTIVE`, `NOT VERY EFFECTIVE`, `NO EFFECT`).
  * **Pokémon:** Hovering over either Pokémon's sprite or HUD card displays their full 5-stat profile, current health percentage, and active ailments.
* **Redesigned Status Conditions:** Color-coded, high-contrast ailment badges:
  * **PSN (Poison):** Vivid Royal Purple
  * **BRN (Burn):** Bright Fiery Crimson
  * **FRZ (Frozen):** Glowing Sky Cyan
  * **PAR (Paralysis):** Vivid Electric Amber/Gold
  * **SLP (Sleep):** Cool Slate Gray
* **Smart AI:** Opponents dynamically counter-switch when their Pokémon faints based on offensive type advantages and defensive resistances.

---

## Controls

| Action | Control |
| :--- | :--- |
| **Select Attack Moves (1 - 4)** | Keys `[1]`, `[2]`, `[3]`, `[4]` or **Left Click** |
| **FIGHT / Main Menu** | Key `[1]` or `[F]` |
| **POKEMON (Party Switch)** | Key `[2]`, `[S]`, or `[P]` |
| **Back / Cancel** | Key `[ESC]` or `[B]` |
| **Advance / Skip Dialogue** | `[Space]`, `[Enter]`, `[Z]`, or **Left Click** |
| **Toggle Fullscreen** | Key `[F11]` or Fullscreen button |
| **Mouse Support** | Full point-and-click interface |

---

## Building from Source (Windows Developers)

If you modify the source code, rebuild on Windows using GCC with:
```powershell
.\build.bat
```
or directly with GCC:
```powershell
gcc src/*.c -o pokemon.exe -Isrc -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -mwindows
```

---

## Project Structure

```
Pokemon Phoenix Red/
├── images/                   # All 302 Gen 3 front & back sprites for 151 Pokemon
├── src/                      # Clean, modular C source code
│   ├── main.c                # Main game loop, states & presentation
│   ├── battle.c / .h         # Damage formulas, turn steps, typewriter text
│   ├── moves.c / .h          # Move definitions and database
│   ├── party.c / .h          # Party management and AI counter-switching
│   ├── pokemon_database.c/.h # 151 Pokemon species templates & stats
│   ├── type_chart.c / .h     # 18 elemental type matchups and status colors
│   ├── types.h               # Core data structures
│   └── ui.c / .h             # 720p HD UI rendering, badges, HUDs, and inspection
├── tools/                    # Asset helper scripts
│   └── download_sprites.py
├── build.bat                 # Windows build script
├── build.sh                  # Linux build script
├── pokemon.exe               # Ready-to-play Windows executable (Raylib statically linked)
├── glfw3.dll                 # Portable GLFW runtime (bundled for Windows plug-and-play)
├── .gitignore
└── README.md
```
