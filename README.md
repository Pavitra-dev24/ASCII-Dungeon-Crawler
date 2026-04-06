# ASCII Roguelike Dungeon Crawler

A turn-based dungeon crawler that runs entirely in the terminal, built in C++ using ncurses.

## Gameplay

Navigate a multi-room dungeon, fight enemies, and reach the exit `>` to advance to the next level.

| Symbol | Meaning |
|--------|---------|
| `@` | Player |
| `E` | Enemy |
| `>` | Exit |
| `#` | Wall |
| `.` | Floor |

**Controls:** `W A S D` to move, `Q` to quit. Bump into an enemy to attack it.

## Features

- Four-room map connected by corridors
- Bump-to-attack combat system
- Greedy enemy AI that pathfinds toward the player each turn
- Multi-level progression with scaling enemy HP and player attack
- ncurses color-paired HUD showing live HP, level and status messages

## Project Structure

```
roguelike.cpp   # Entire game in a single file
Makefile
```
