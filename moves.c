#include "moves.h"
#include <string.h>
#include <strings.h>

move CreateMove(const char *name, int bp, int accuracy, int max_pp,
                ElementalType type, MoveCategory category,
                StatusEffect effect, int probability)
{
    move m;
    strncpy(m.Name, name, sizeof(m.Name) - 1);
    m.Name[sizeof(m.Name) - 1] = '\0';
    m.BP = bp;
    m.Accuracy = accuracy;
    m.Max_PP = max_pp;
    m.Current_PP = max_pp;
    m.type = type;
    m.category = category;
    m.effect = effect;
    m.probability = probability;
    return m;
}

static const move master_moves[] = {
    // Normal Moves
    { .Name = "Tackle",        .BP = 40,  .Accuracy = 100, .Max_PP = 35, .Current_PP = 35, .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Quick Attack",  .BP = 40,  .Accuracy = 100, .Max_PP = 30, .Current_PP = 30, .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Headbutt",      .BP = 70,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Slash",         .BP = 70,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Body Slam",     .BP = 85,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_PARALYSIS, .probability = 30 },
    { .Name = "Tri Attack",    .BP = 80,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_NORMAL,   .category = CATEGORY_SPECIAL,  .effect = STATUS_PARALYSIS, .probability = 20 },
    { .Name = "Hyper Beam",    .BP = 150, .Accuracy = 90,  .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_NORMAL,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Double-Edge",   .BP = 120, .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Extreme Speed", .BP = 80,  .Accuracy = 100, .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Swift",         .BP = 60,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_NORMAL,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Mega Punch",    .BP = 80,  .Accuracy = 85,  .Max_PP = 20, .Current_PP = 20, .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Mega Kick",     .BP = 120, .Accuracy = 75,  .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_NORMAL,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },

    // Fire Moves
    { .Name = "Ember",         .BP = 40,  .Accuracy = 100, .Max_PP = 25, .Current_PP = 25, .type = TYPE_FIRE,     .category = CATEGORY_SPECIAL,  .effect = STATUS_BURN,      .probability = 10 },
    { .Name = "Fire Punch",    .BP = 75,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_FIRE,     .category = CATEGORY_PHYSICAL, .effect = STATUS_BURN,      .probability = 10 },
    { .Name = "Flamethrower",  .BP = 90,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_FIRE,     .category = CATEGORY_SPECIAL,  .effect = STATUS_BURN,      .probability = 10 },
    { .Name = "Fire Blast",    .BP = 110, .Accuracy = 85,  .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_FIRE,     .category = CATEGORY_SPECIAL,  .effect = STATUS_BURN,      .probability = 20 },
    { .Name = "Flare Blitz",   .BP = 120, .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_FIRE,     .category = CATEGORY_PHYSICAL, .effect = STATUS_BURN,      .probability = 10 },

    // Water Moves
    { .Name = "Water Gun",     .BP = 40,  .Accuracy = 100, .Max_PP = 25, .Current_PP = 25, .type = TYPE_WATER,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Bubble Beam",   .BP = 65,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_WATER,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Waterfall",     .BP = 80,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_WATER,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Surf",          .BP = 90,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_WATER,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Hydro Pump",    .BP = 110, .Accuracy = 80,  .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_WATER,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Crabhammer",    .BP = 100, .Accuracy = 90,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_WATER,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },

    // Grass Moves
    { .Name = "Vine Whip",     .BP = 45,  .Accuracy = 100, .Max_PP = 25, .Current_PP = 25, .type = TYPE_GRASS,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Razor Leaf",    .BP = 55,  .Accuracy = 95,  .Max_PP = 25, .Current_PP = 25, .type = TYPE_GRASS,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Giga Drain",    .BP = 75,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_GRASS,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Solar Beam",    .BP = 120, .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_GRASS,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Petal Dance",   .BP = 120, .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_GRASS,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Sleep Powder",  .BP = 0,   .Accuracy = 75,  .Max_PP = 15, .Current_PP = 15, .type = TYPE_GRASS,    .category = CATEGORY_STATUS,   .effect = STATUS_SLEEP,     .probability = 100 },

    // Electric Moves
    { .Name = "Spark",         .BP = 65,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_ELECTRIC, .category = CATEGORY_PHYSICAL, .effect = STATUS_PARALYSIS, .probability = 30 },
    { .Name = "Thunder Punch", .BP = 75,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_ELECTRIC, .category = CATEGORY_PHYSICAL, .effect = STATUS_PARALYSIS, .probability = 10 },
    { .Name = "Thunderbolt",   .BP = 90,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_ELECTRIC, .category = CATEGORY_SPECIAL,  .effect = STATUS_PARALYSIS, .probability = 10 },
    { .Name = "Thunder",       .BP = 110, .Accuracy = 70,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_ELECTRIC, .category = CATEGORY_SPECIAL,  .effect = STATUS_PARALYSIS, .probability = 30 },
    { .Name = "Thunder Wave",  .BP = 0,   .Accuracy = 90,  .Max_PP = 20, .Current_PP = 20, .type = TYPE_ELECTRIC, .category = CATEGORY_STATUS,   .effect = STATUS_PARALYSIS, .probability = 100 },

    // Ice Moves
    { .Name = "Aurora Beam",   .BP = 65,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_ICE,      .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Ice Punch",     .BP = 75,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_ICE,      .category = CATEGORY_PHYSICAL, .effect = STATUS_FREEZE,    .probability = 10 },
    { .Name = "Ice Beam",      .BP = 90,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_ICE,      .category = CATEGORY_SPECIAL,  .effect = STATUS_FREEZE,    .probability = 10 },
    { .Name = "Blizzard",      .BP = 110, .Accuracy = 70,  .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_ICE,      .category = CATEGORY_SPECIAL,  .effect = STATUS_FREEZE,    .probability = 10 },

    // Fighting Moves
    { .Name = "Karate Chop",   .BP = 50,  .Accuracy = 100, .Max_PP = 25, .Current_PP = 25, .type = TYPE_FIGHTING, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Brick Break",   .BP = 75,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_FIGHTING, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Submission",    .BP = 80,  .Accuracy = 80,  .Max_PP = 20, .Current_PP = 20, .type = TYPE_FIGHTING, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Cross Chop",    .BP = 100, .Accuracy = 80,  .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_FIGHTING, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "High Jump Kick",.BP = 130, .Accuracy = 90,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_FIGHTING, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Close Combat",  .BP = 120, .Accuracy = 100, .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_FIGHTING, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },

    // Poison Moves
    { .Name = "Acid",          .BP = 40,  .Accuracy = 100, .Max_PP = 30, .Current_PP = 30, .type = TYPE_POISON,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Poison Powder", .BP = 0,   .Accuracy = 75,  .Max_PP = 35, .Current_PP = 35, .type = TYPE_POISON,   .category = CATEGORY_STATUS,   .effect = STATUS_POISON,    .probability = 100 },
    { .Name = "Poison Jab",    .BP = 80,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_POISON,   .category = CATEGORY_PHYSICAL, .effect = STATUS_POISON,    .probability = 30 },
    { .Name = "Sludge Bomb",   .BP = 90,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_POISON,   .category = CATEGORY_SPECIAL,  .effect = STATUS_POISON,    .probability = 30 },
    { .Name = "Toxic",         .BP = 0,   .Accuracy = 90,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_POISON,   .category = CATEGORY_STATUS,   .effect = STATUS_POISON,    .probability = 100 },

    // Ground Moves
    { .Name = "Mud-Slap",      .BP = 20,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_GROUND,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Bone Club",     .BP = 65,  .Accuracy = 85,  .Max_PP = 20, .Current_PP = 20, .type = TYPE_GROUND,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Dig",           .BP = 80,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_GROUND,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Earthquake",    .BP = 100, .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_GROUND,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Earth Power",   .BP = 90,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_GROUND,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },

    // Flying Moves
    { .Name = "Wing Attack",   .BP = 60,  .Accuracy = 100, .Max_PP = 35, .Current_PP = 35, .type = TYPE_FLYING,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Aerial Ace",    .BP = 60,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_FLYING,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Air Slash",     .BP = 75,  .Accuracy = 95,  .Max_PP = 15, .Current_PP = 15, .type = TYPE_FLYING,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Drill Peck",    .BP = 80,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_FLYING,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Fly",           .BP = 90,  .Accuracy = 95,  .Max_PP = 15, .Current_PP = 15, .type = TYPE_FLYING,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Hurricane",     .BP = 110, .Accuracy = 70,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_FLYING,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },

    // Psychic Moves
    { .Name = "Confusion",     .BP = 50,  .Accuracy = 100, .Max_PP = 25, .Current_PP = 25, .type = TYPE_PSYCHIC,  .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Psybeam",       .BP = 65,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_PSYCHIC,  .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Psychic",       .BP = 90,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_PSYCHIC,  .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Hypnosis",      .BP = 0,   .Accuracy = 60,  .Max_PP = 20, .Current_PP = 20, .type = TYPE_PSYCHIC,  .category = CATEGORY_STATUS,   .effect = STATUS_SLEEP,     .probability = 100 },
    { .Name = "Dream Eater",   .BP = 100, .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_PSYCHIC,  .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },

    // Bug Moves
    { .Name = "Twineedle",     .BP = 50,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_BUG,      .category = CATEGORY_PHYSICAL, .effect = STATUS_POISON,    .probability = 20 },
    { .Name = "Pin Missile",   .BP = 50,  .Accuracy = 95,  .Max_PP = 20, .Current_PP = 20, .type = TYPE_BUG,      .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "X-Scissor",     .BP = 80,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_BUG,      .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Bug Buzz",      .BP = 90,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_BUG,      .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Megahorn",      .BP = 120, .Accuracy = 85,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_BUG,      .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },

    // Rock Moves
    { .Name = "Rock Throw",    .BP = 50,  .Accuracy = 90,  .Max_PP = 15, .Current_PP = 15, .type = TYPE_ROCK,     .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Ancient Power", .BP = 60,  .Accuracy = 100, .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_ROCK,     .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Rock Slide",    .BP = 75,  .Accuracy = 90,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_ROCK,     .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Stone Edge",    .BP = 100, .Accuracy = 80,  .Max_PP = 5,  .Current_PP = 5,  .type = TYPE_ROCK,     .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },

    // Ghost Moves
    { .Name = "Lick",          .BP = 30,  .Accuracy = 100, .Max_PP = 30, .Current_PP = 30, .type = TYPE_GHOST,    .category = CATEGORY_PHYSICAL, .effect = STATUS_PARALYSIS, .probability = 30 },
    { .Name = "Shadow Claw",   .BP = 70,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_GHOST,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Shadow Ball",   .BP = 80,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_GHOST,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },

    // Dragon Moves
    { .Name = "Dragon Breath", .BP = 60,  .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_DRAGON,   .category = CATEGORY_SPECIAL,  .effect = STATUS_PARALYSIS, .probability = 30 },
    { .Name = "Dragon Claw",   .BP = 80,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_DRAGON,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Dragon Pulse",  .BP = 85,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_DRAGON,   .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Outrage",       .BP = 120, .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_DRAGON,   .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },

    // Dark Moves
    { .Name = "Bite",          .BP = 60,  .Accuracy = 100, .Max_PP = 25, .Current_PP = 25, .type = TYPE_DARK,     .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Night Slash",   .BP = 70,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_DARK,     .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Crunch",        .BP = 80,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_DARK,     .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Dark Pulse",    .BP = 80,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_DARK,     .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },

    // Steel Moves
    { .Name = "Metal Claw",    .BP = 50,  .Accuracy = 95,  .Max_PP = 35, .Current_PP = 35, .type = TYPE_STEEL,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Steel Wing",    .BP = 70,  .Accuracy = 90,  .Max_PP = 25, .Current_PP = 25, .type = TYPE_STEEL,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Iron Head",     .BP = 80,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_STEEL,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Flash Cannon",  .BP = 80,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_STEEL,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },

    // Fairy Moves
    { .Name = "Dazzling Gleam",.BP = 80,  .Accuracy = 100, .Max_PP = 10, .Current_PP = 10, .type = TYPE_FAIRY,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE },
    { .Name = "Play Rough",    .BP = 90,  .Accuracy = 90,  .Max_PP = 10, .Current_PP = 10, .type = TYPE_FAIRY,    .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE },
    { .Name = "Moonblast",     .BP = 95,  .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_FAIRY,    .category = CATEGORY_SPECIAL,  .effect = STATUS_NONE }
};

static const int master_moves_count = sizeof(master_moves) / sizeof(master_moves[0]);

int GetTotalMoveCount(void)
{
    return master_moves_count;
}

const move *GetMasterMoveList(void)
{
    return master_moves;
}

move GetMoveByName(const char *name)
{
    if (!name || name[0] == '\0') {
        return master_moves[0]; // Tackle default
    }

    for (int i = 0; i < master_moves_count; i++) {
        if (strcasecmp(master_moves[i].Name, name) == 0) {
            return master_moves[i];
        }
    }

    // If not found in master list, return Tackle
    return master_moves[0];
}
