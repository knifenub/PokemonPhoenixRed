#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAX_MESSAGES 30
#define MESSAGE_SIZE 256
#define MESSAGE_DURATION 1.5

typedef enum {
    TYPE_NONE = 0,
    TYPE_NORMAL,
    TYPE_FIRE,
    TYPE_WATER,
    TYPE_GRASS,
    TYPE_ELECTRIC,
    TYPE_ICE,
    TYPE_FIGHTING,
    TYPE_POISON,
    TYPE_GROUND,
    TYPE_FLYING,
    TYPE_PSYCHIC,
    TYPE_BUG,
    TYPE_ROCK,
    TYPE_GHOST,
    TYPE_DRAGON,
    TYPE_DARK,
    TYPE_STEEL,
    TYPE_FAIRY,
    TYPE_COUNT
} ElementalType;

typedef enum {
    CATEGORY_PHYSICAL,
    CATEGORY_SPECIAL,
    CATEGORY_STATUS
} MoveCategory;

typedef enum {
    STATUS_NONE,
    STATUS_POISON,
    STATUS_BURN,
    STATUS_PARALYSIS,
    STATUS_SLEEP,
    STATUS_FREEZE
} StatusEffect;

typedef struct move {
    int BP;
    int Accuracy;
    int Max_PP;
    int Current_PP;
    char Name[64];
    ElementalType type;
    MoveCategory category;
    StatusEffect effect;
    int probability;
} move;

typedef struct Pokemon {
    char Name[64];
    int max_health;
    int current_health;
    int max_speed;
    int speed;
    int max_defense;
    int defense;
    int max_attack;
    int attack;
    int special_attack;
    int special_defense;
    ElementalType type1;
    ElementalType type2;
    // In-battle modifiers (100 = 100%)
    int evasiveness;
    int accuracy;
    int max_evasiveness;
    int max_accuracy;
    move Moves[4];
    int level;
    StatusEffect effect;
    int status_conditions_rounds_left;
} Pokemon;

typedef struct BattleContext {
    Pokemon a;
    Pokemon b;
    move *player_a_active_move;
    move *player_b_active_move;
} BattleContext;

// Species template used in database
typedef struct PokemonSpecies {
    int id;                      // Pokedex number (#1 - #151)
    char name[64];
    int baseHP;
    int baseAttack;
    int baseDefense;
    int baseSpAttack;
    int baseSpDefense;
    int baseSpeed;
    ElementalType type1;
    ElementalType type2;
    const char *defaultMoveNames[4];
} PokemonSpecies;

#endif // TYPES_H
