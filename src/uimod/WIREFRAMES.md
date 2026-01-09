# Modern UI Mod - Wireframes

Text-based wireframes showing the layout of each UI component.
Default screen resolution: 800x600

## Screen Layout Overview

```
+------------------------------------------------------------------------+
|  [DEFAULT CLIENT TOP BAR - Equipment/Clock]                            |
+------------------------------------------------------------------------+
|                                                                        |
|                                              +------------------+      |
|                                              | CLOCK            |      |
|          GAME WORLD VIEW                     | 14:35            |      |
|                                              | Afternoon        |      |
|                                              | Light: ========  |      |
|                                              +------------------+      |
|                                                                        |
|                                              +------------------+      |
|                                              | INVENTORY        |      |
|                                              | [5x8 grid]       |      |
|                                              |                  |      |
|                                              +------------------+      |
|                                                                        |
|                                              +------------------+      |
|                                              | EQUIPMENT        |      |
|                                              | [Paperdoll]      |      |
|                                              |                  |      |
| +-------------------+                        | HP: ====         |      |
| | CHAT              |                        | MP: ====         |      |
| | ----------------- |                        +------------------+      |
| | [Message 1]       |                                                  |
| | [Message 2]       |                                                  |
| | [Message 3]       |                                                  |
| +-------------------+                                                  |
+------------------------------------------------------------------------+
|  [DEFAULT CLIENT BOTTOM BAR - Skills/Chat Input]                       |
+------------------------------------------------------------------------+
```

## Chat Panel (ui_chat.c)

Position: Bottom-left (10, 400)
Size: 300x150 pixels

```
+--[ Chat ]--------------------------------+
| --------------------------------------   |  <- Header separator
| [13:42] Player1: Hello everyone!         |
| [13:43] Player2: Hi there!               |
| [13:44] System: You gained 50 exp        |
| [13:45] Player1: Anyone want to party?   |
| [13:46] You: Sure, invite me             |
| [13:47] Player2: On my way               |
| [13:48] System: Player2 invited you      |
| [13:49] You: Accepted                    |
|                                          |
|                                          |
|                                       |  |  <- Scrollbar
|                                       |##|
|                                       |  |
+------------------------------------------+

Features:
- Semi-transparent background (alpha: 180)
- Auto-scroll to newest message
- Manual scroll with mouse wheel
- Color-coded messages:
  - White: Normal chat
  - Green: Party chat
  - Blue: Whispers
  - Orange: System messages
  - Red: Warnings/Errors
```

## Inventory Panel (ui_inventory.c)

Position: Right side (520, 100)
Size: ~200x320 pixels (5 cols x 8 rows + header)

```
+--[ Inventory ]--------[ 1,234g ]--+
| -----------------------------------
| +----+----+----+----+----+        |
| |    |    |    |    |    |        |
| | 01 | 02 | 03 | 04 | 05 |   ^    |  <- Scroll up
| +----+----+----+----+----+   |    |
| +----+----+----+----+----+   |    |
| |*   |    |    |    |    |   #    |  <- Scroll thumb
| | 06 | 07 | 08 | 09 | 10 |   #    |
| +----+----+----+----+----+   |    |
| +----+----+----+----+----+   |    |
| |    |    |    |    |    |   v    |  <- Scroll down
| | 11 | 12 | 13 | 14 | 15 |        |
| +----+----+----+----+----+        |
| ...                               |
| 45/110 items              [scroll]|
+-----------------------------------+

Slot indicators:
  * = Selected
  . = Usable (green dot)
  ^ = Equippable (colored dot by type)

Hover tooltip:
+------------------+
| Slot 7           |
| Sprite: 12345    |
| Usable Equippable|
+------------------+
```

## Equipment Panel (ui_equipment.c)

Position: Right side, below inventory (520, 300)
Size: ~155x240 pixels

```
+--[ Equipment ]----------------------+
| ----------------------------------- |
|                                     |
|           +------+                  |  <- HEAD
|           | HEAD |                  |
|           +------+                  |
|    +------+      +------+           |  <- CLOAK / NECK
|    |CLOAK |      | NECK |           |
|    +------+      +------+           |
| +----+  +--------+  +----+          |  <- R.HAND / BODY / L.HAND
| |R.HD|  |  BODY  |  |L.HD|          |
| +----+  +--------+  +----+          |
| +----+  +--------+  +----+          |  <- R.RING / BELT / L.RING
| |R.RG|  |  BELT  |  |L.RG|          |
| +----+  +--------+  +----+          |
|         +--------+                  |  <- ARMS
|         |  ARMS  |                  |
|         +--------+                  |
|         +--------+                  |  <- LEGS
|         |  LEGS  |                  |
|         +--------+                  |
|         +--------+                  |  <- FEET
|         |  FEET  |                  |
|         +--------+                  |
| ----------------------------------- |
| HP [========    ] MP [======      ] |  <- Status bars
| EN [==========  ] SH [====        ] |
+-------------------------------------+

Two-handed weapon handling:
- When R.HAND has 2H weapon, L.HAND shows "2H" grayed out
```

## Clock Widget (ui_clock.c)

Position: Top-right (700, 10)
Size: ~85x50 pixels

```
+--[ CLOCK ]----------+
|                     |
|  ☀  14:35          |  <- Sun/Moon icon + Time
|     Afternoon       |  <- Time period
|                     |
| Light [==========]  |  <- Brightness bar
| t:847               |  <- Tick counter (debug)
+---------------------+

Time periods:
  05:00-07:00  Dawn       (orange)
  07:00-12:00  Morning    (white)
  12:00-14:00  Midday     (white)
  14:00-17:00  Afternoon  (white)
  17:00-20:00  Evening    (orange)
  20:00-22:00  Dusk       (dark orange)
  22:00-05:00  Night      (blue)

Icons:
  ☀ = Sun (daytime: 06:00-20:00)
  ☾ = Moon with stars (nighttime: 20:00-06:00)
```

## Color Scheme

```
Default color palette (customizable via #ui commands):

Panel Background:  0x0000 (black) @ alpha 180
Panel Border:      0x4210 (dark gray)
Highlight:         0x7C00 (blue)
Text:              varies by context

Stat bar colors (from client):
  Health:     healthcolor (red)
  Mana:       manacolor (blue)
  Endurance:  endurancecolor (green)
  Shield:     shieldcolor (cyan)
```

## Commands

```
#uihelp              - Show all commands
#uichat [on|off]     - Toggle chat panel
#uiinv [on|off]      - Toggle inventory panel
#uieq [on|off]       - Toggle equipment panel
#uiclock [on|off]    - Toggle clock widget
#uialpha <0-255>     - Set panel transparency
#uipos <panel> <x> <y> - Move panel (chat, inv, eq, clock)
#uireset             - Reset all UI to defaults
```

## Future Enhancements

1. **Draggable panels** - Click and drag title bar to reposition
2. **Resizable panels** - Drag edges to resize
3. **Custom color themes** - #uitheme <name> to load preset colors
4. **Save/load layout** - Persist positions to config file
5. **Minimap overlay** - Semi-transparent minimap
6. **Buff/debuff bar** - Visual timers for active effects
7. **Target info panel** - Show selected mob/player stats
8. **Quick-cast bar** - Customizable spell shortcuts
