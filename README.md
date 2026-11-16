<p align="center">
  <img src="assets/icon.png" alt="New Nation: Portugal" width="320">
</p>

<h1 align="center">New Nation: Portugal</h1>

<p align="center">
  A mod for <b>Caribbean Legend: Age of Pirates</b> that adds Portugal as a full, playable sixth nation.
</p>

<p align="center">
  <a href="STEAM_WORKSHOP_LINK">Steam Workshop</a> ·
  <a href="#installation">Installation</a> ·
  <a href="#changelog">Changelog</a>
</p>

---

## About

It is 1660, and the Portuguese Crown has a foothold in the Caribbean. **Cumaná** and **Caracas** are now Portuguese colonies, ruled by a Governor-General in Caracas. You can play as a Portuguese captain, earn a Portuguese letter of marque, rise through the Portuguese ranks and follow a questline that ends in the Governor-General's palace.

## Features

### A new nation
- Portugal as a sixth nation, with relations to every other nation.
- Cumaná and Caracas as Portuguese colonies.
- Interface texts and nation names in all 8 game languages.

### Visuals
- Portuguese uniforms for soldiers and officers.
- The Portuguese flag on ships, forts, the interface and the world map: colonies, ships and the player panel.
- Portuguese ship models on the world map.
- Portraits for Portuguese naval officers.
- Portugal icons for nation selection, the flag ability and reputation notifications.

### Gameplay
- Portugal in character creation.
- **Flag of Portugal** ability to raise Portuguese colours on your ship.
- Portuguese letter of marque, from a governor or the diplomat.
- Portuguese ranks: **Corsair → Captain → Capitão-Mor → Admiral → General → Governor**.
- Portuguese names for characters and ships.

### Questline: *For the Glory of the Crown*
Seven missions, unlocked by talking to the Governor-General in Caracas with a Portuguese letter of marque. A pirate network is bleeding Portuguese trade in the Caribbean and Brazil, and someone is paying for it. The questline includes:
- sea battles and boarding actions, with dialogue in the enemy captain's cabin;
- tavern investigations, where informants don't talk for free;
- choices: what evidence to hand over, whom to lie to, and who to spare;
- a promotion at every stage, and an ending where you find out who was using you.

### Governor-General
After the questline you become the Governor-General of the Portuguese colonies:
- Portuguese mayors recognise your office.
- Each month a tenth of every Portuguese colony's profit goes to your treasury, where it accumulates until you collect it.
- A secretary keeps the treasury, and you can move the seat of government to another Portuguese colony.

## Installation

### Steam Workshop (recommended)
1. Subscribe on the [Steam Workshop page](STEAM_WORKSHOP_LINK).
2. Start a **new game**. Old saves don't include Portugal.

### Manual (local overlay)
The game loads mods as overlay folders, so you don't need to overwrite any game file.

1. Download this repository or the latest release.
2. In the game folder, create a folder named `overlay_portugal` and copy the `PROGRAM` and `RESOURCE` folders into it.
3. In the game folder, create or edit `overlays.txt` and add the line:
   ```
   overlay_portugal
   ```
4. Start a **new game**.

If you are subscribed to the Workshop version, unsubscribe first: the same files in two mods count as a conflict.

## Compatibility

- **New game required.**
- The mod replaces 111 game files: scripts for nations, colonies, characters, dialogues and items, plus interface textures. It will conflict with other mods that replace the same files. When that happens, the game shows a conflict error and disables the mods involved.
- The questline is written in **English** and **Portuguese**. In the other languages it shows the English text.
- Music and voices for Portugal still use the Spanish ones.

## Reporting problems

Please open an issue and include:
- `system.log` and `error.log` from the game folder;
- the list of other mods you use;
- what you were doing when the problem happened (e.g. which mission or which dialogue).

## Repository structure

```
PROGRAM/        Game scripts (replaced and new files)
RESOURCE/       Textures, models, interface files and texts
_ferramentas/   Python tools used to build the mod (not needed to play)
```

### Tools
The scripts in `_ferramentas/` generate the mod's assets and the questline:

| Script | Purpose |
|---|---|
| `txtool.py` | Reads and writes the game's `.tx` texture format |
| `portugal_arms.py` | Draws the Portuguese royal arms used in flags and icons |
| `build_portraits.py`, `portrait_xform.py` | Build the Portuguese officer portraits |
| `build_portugal_ui.py`, `build_portugal_ui2.py` | Interface textures: banners, perks, nation icons |
| `build_portugal_wm.py` | World-map flags and icons |
| `recolor.py` | Recolours uniforms |
| `porline_texts.py` | All questline texts (English and Portuguese) |
| `porline_code.py`, `porline_line.py` | Questline script templates |
| `build_porline.py` | Generates the questline scripts, dialogue files and quest-journal entries |
| `build_icon.py` | Generates the mod icon |

Requires Python 3 with `numpy`, `Pillow` and `scipy`.

## Changelog

### v1.0 — 2026-11-16
- First release: the Portuguese nation, the Cumaná and Caracas colonies, visuals, character creation, the flag ability, the letter of marque and ranks.
- *For the Glory of the Crown* questline (7 missions).
- Governor-General income, treasury and movable seat of government.
- Fixed: the Portugal reputation notification showed "England" and had no icon.
- Shortened character names and rank titles so they fit menus and the HUD.

## Credits

- **Caribbean Legend: Age of Pirates** by BlackMark Studio and Valkyrie Initiative. This mod requires the game, and all original game content belongs to its owners.
- Mod by **YOUR_NAME**.
