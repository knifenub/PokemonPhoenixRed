#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "types.h"

// Initialize font and thumbnail cache
void InitUIAssets(void);
void UnloadUIAssets(void);

// Typography helpers using clean, readable font
void DrawCustomText(const char *text, float x, float y, float fontSize, Color color);
int MeasureCustomText(const char *text, float fontSize);

// Colors for all 18 elemental types
Color GetTypeColor(ElementalType type);

// Cached front thumbnail loader (64x64 GBA sprite)
Texture2D GetCachedPokemonThumbnail(const char *name);

// Draws a proportional HP bar with dynamic health-based colors:
// Green (>50%), Yellow (20-50%), Red (<=20%)
void DrawHPBar(int x, int y, int width, int height, int currentHP, int maxHP, bool showText);

// Draws a compact elemental type badge
void DrawTypeBadge(ElementalType type, int x, int y, int width, int height, int fontSize);

// Draws a colored status badge: Purple (Poison), Red (Burn), Sky Blue (Frozen), Yellow (Paralysis), Gray (Sleep)
void DrawStatusBadge(StatusEffect effect, int x, int y, int width, int height, int fontSize);

// Team Selection Screen with front sprites, types, stats, and scroll
bool DrawTeamSelectionScreen(Party *playerParty, int *scrollIndex, Vector2 virtualMouse);

// In-Battle Party Switch Overlay
int DrawPartySwitchModal(const Party *party, bool isForced, Vector2 virtualMouse);

// In-Battle Inspection rendered fixed inside the bottom text box
void DrawPokemonStatsInTextBox(const Pokemon *p, bool isEnemy);
void DrawMoveDetailsInTextBox(const move *m, const Pokemon *target);
void DrawIdleBattlePromptInTextBox(const Pokemon *playerActive, bool inMoveMenu);

#endif // UI_H
