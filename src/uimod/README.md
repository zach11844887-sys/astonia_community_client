# Modern UI Mod for Astonia Community Client

A modular UI overlay system providing modern, customizable UI panels.

## Features

- **Semi-transparent Chat Panel** - Scrollable chat history with color-coded messages
- **Modern Inventory Grid** - 5x8 scrollable grid with item tooltips
- **Paperdoll Equipment View** - Visual equipment layout with stat bars
- **World Clock Widget** - In-game time display with day/night indicator

## Files

```
src/uimod/
├── README.md           # This file
├── WIREFRAMES.md       # UI design mockups
├── Makefile            # Standalone build
├── uimod.h             # Main header with all imports/exports
├── uimod.c             # Mod lifecycle and command handling
├── ui_chat.c           # Chat panel component
├── ui_inventory.c      # Inventory panel component
├── ui_equipment.c      # Equipment panel component
└── ui_clock.c          # Clock widget component

patches/
├── export_ui_state.md      # Client export patch documentation
└── makefile_uimod_rules.txt # Rules to add to main Makefile
```

## Building

### Option 1: Add to main build

1. Add uimod rules from `patches/makefile_uimod_rules.txt` to `build/make/Makefile.windows`
2. Run: `make uimod`

### Option 2: Standalone build

```bash
cd src/uimod
make -f Makefile
```

Output: `bin/uimod.dll`

## Installation

1. Build the mod (creates `bin/uimod.dll`)
2. The client loads mods alphabetically: amod, bmod, cmod, etc.
3. Rename `uimod.dll` to your preferred slot (e.g., `bmod.dll`)
4. Launch the client

## Usage

In-game commands:

| Command | Description |
|---------|-------------|
| `#uihelp` | Show all UI mod commands |
| `#uichat [on\|off]` | Toggle chat panel |
| `#uiinv [on\|off]` | Toggle inventory panel |
| `#uieq [on\|off]` | Toggle equipment panel |
| `#uiclock [on\|off]` | Toggle clock widget |
| `#uialpha <0-255>` | Set panel transparency |
| `#uipos <panel> <x> <y>` | Move panel position |
| `#uireset` | Reset to default layout |

## Client Export Patch (Optional)

For full functionality, apply the exports documented in `patches/export_ui_state.md`.

Without the patch:
- Chat panel uses local buffer (can't read existing chat history)
- Selection highlighting won't sync with default UI

With the patch:
- Full chat history access
- Selection state synchronization
- Scroll position access

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Astonia Client                          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Mod Loader (modder.c)                   │   │
│  │  Loads: amod.dll, bmod.dll, ... up to 6 mods        │   │
│  └─────────────────────────────────────────────────────┘   │
│                           │                                 │
│                           ▼                                 │
│  ┌─────────────────────────────────────────────────────┐   │
│  │               uimod.dll (this mod)                   │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐    │   │
│  │  │ ui_chat │ │ ui_inv  │ │ ui_equip│ │ui_clock │    │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘    │   │
│  └─────────────────────────────────────────────────────┘   │
│                           │                                 │
│                           ▼                                 │
│  ┌─────────────────────────────────────────────────────┐   │
│  │           SDL3 Rendering Layer                       │   │
│  │  render_text(), render_rect(), render_sprite()       │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## Mod Callbacks Used

| Callback | Purpose |
|----------|---------|
| `amod_init()` | Initialize UI components |
| `amod_exit()` | Cleanup resources |
| `amod_gamestart()` | Show welcome message |
| `amod_frame()` | Render all UI panels |
| `amod_tick()` | Update animations |
| `amod_mouse_click()` | Handle panel interactions |
| `amod_client_cmd()` | Process #ui commands |

## Customization

### Adding a new panel

1. Create `ui_newpanel.c` with init/exit/frame/click functions
2. Add declarations to `uimod.h`
3. Call from `uimod.c` lifecycle functions
4. Add enable flag and position variables
5. Update Makefile

### Changing colors

Edit these variables in `uimod.c`:

```c
unsigned short ui_panel_color = 0x0000;      // Background
unsigned short ui_border_color = 0x4210;     // Border
unsigned short ui_highlight_color = 0x7C00;  // Highlights
```

## License

Part of Astonia Community Client. See license.txt.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes with tests
4. Submit a pull request

For the export patch, consider submitting upstream so all mods benefit.
