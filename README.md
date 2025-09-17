# ASCII-Roguelike-Dungeon-Crawler
A small, console-based roguelike written in C++ that demonstrates procedural dungeon generation, simple enemy AI (BFS pathfinding with an aggression heuristic), and basic turn-based combat.

# How To Play

Procedural dungeon: multiple non-overlapping rectangular rooms connected by corridors.

Player character @ with hit points and melee attack.

Exit tile > — step on it to win.

Enemies E spawn in rooms and:

Wander randomly when the player is far.

Use BFS pathfinding to chase the player when within an aggression radius.

Turn-based, console-only (no GUI) — simple controls, single-file code for easy review.

# Controls

W / A / S / D — move Up / Left / Down / Right (case-insensitive)

Q — quit

Step onto > to win

Move into enemy to attack it

# Core data structures

Room: small struct with (x,y,w,h) and an intersects test to prevent overlap.

Entity: struct for player/enemies: position, glyph, HP.

Queue: std::queue used for BFS pathfinding.

Vectors: std::vector for rooms and entity lists.

# Algorithms & heuristics

Procedural room placement: random non-overlapping rectangle placement with simple corridor carving between room centers.

Corridor carving: horizontal-then-vertical or vertical-then-horizontal corridor connecting centers.

# Enemy AI:

Random wandering when player out of AGGRO_RANGE.

BFS pathfinding when player within AGGRO_RANGE — compute parent pointers, backtrack to the cell just after the enemy to move one step toward player.

Combat: deterministic melee damage on moving into an occupied tile.

Randomness: std::mt19937 seeded with system clock.
