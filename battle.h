#ifndef BATTLE_H
#define BATTLE_H

#include "types.h"
#include "raylib.h"

// Battle Messaging
void AddBattleMessage(const char *format, ...);
int UpdateBattleMessages(void);
void DrawBattleTextBox(void);
void DrawWrappedText(const char *text, Rectangle area, int fontSize, int lineSpacing, Color color);

// Calculations & AI
int damage_calculator(Pokemon *pokemon, Pokemon *pokemonB, move *Move, double random_factor);
int special_damage_calculator(Pokemon *pokemon, Pokemon *pokemonB, move *Move, double random_factor);
int get_move_weight(move Move, Pokemon target);

// Status Effects & Turn Processing
void move_status_effect_processing(Pokemon *A, Pokemon *B, move *MoveA);
void status_effect_processing(Pokemon *A, Pokemon *B);
void turn_processing(Pokemon *A, Pokemon *B, move *MoveA);

#endif // BATTLE_H
