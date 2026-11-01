#include "type_chart.h"

// 18 x 18 Type Effectiveness Table (Multipliers are multiplied by 10)
// Indices: 0: Normal, 1: Fire, 2: Water, 3: Grass, 4: Electric, 5: Ice,
//          6: Fighting, 7: Poison, 8: Ground, 9: Flying, 10: Psychic, 11: Bug,
//          12: Rock, 13: Ghost, 14: Dragon, 15: Dark, 16: Steel, 17: Fairy
static const int type_chart[18][18] = {
/*NORMAL  */ { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  5,  0, 10, 10,  5, 10 },
/*FIRE    */ { 10,  5,  5, 20, 10, 20, 10, 10, 10, 10, 10, 20,  5, 10,  5, 10, 20, 10 },
/*WATER   */ { 10, 20,  5,  5, 10, 10, 10, 10, 20, 10, 10, 10, 20, 10,  5, 10, 10, 10 },
/*GRASS   */ { 10,  5, 20,  5, 10, 10, 10,  5, 20,  5, 10,  5, 20, 10,  5, 10,  5, 10 },
/*ELECTRIC*/ { 10, 10, 20,  5,  5, 10, 10, 10,  0, 20, 10, 10, 10, 10,  5, 10, 10, 10 },
/*ICE     */ { 10,  5,  5, 20, 10,  5, 10, 10, 20, 20, 10, 10, 10, 10, 20, 10,  5, 10 },
/*FIGHTING*/ { 20, 10, 10, 10, 10, 20, 10,  5, 10,  5,  5,  5, 20,  0, 10, 20, 20,  5 },
/*POISON  */ { 10, 10, 10, 20, 10, 10, 10,  5,  5, 10, 10, 10,  5,  5, 10, 10,  0, 20 },
/*GROUND  */ { 10, 20, 10,  5, 20, 10, 10, 20, 10,  0, 10,  5, 20, 10, 10, 10, 20, 10 },
/*FLYING  */ { 10, 10, 10, 20,  5, 10, 20, 10, 10, 10, 10, 20,  5, 10, 10, 10,  5, 10 },
/*PSYCHIC */ { 10, 10, 10, 10, 10, 10, 20, 20, 10, 10,  5, 10, 10, 10, 10,  0,  5, 10 },
/*BUG     */ { 10,  5, 10, 20, 10, 10,  5,  5, 10,  5, 20, 10, 10,  5, 10, 20,  5,  5 },
/*ROCK    */ { 10, 20, 10, 10, 10, 20,  5, 10,  5, 20, 10, 20, 10, 10, 10, 10,  5, 10 },
/*GHOST   */ {  0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 20, 10, 10, 20, 10,  5, 10, 10 },
/*DRAGON  */ { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 20, 10,  5,  0 },
/*DARK    */ { 10, 10, 10, 10, 10, 10,  5, 10, 10, 10, 20, 10, 10, 20, 10,  5, 10,  5 },
/*STEEL   */ { 10,  5,  5, 10,  5, 20, 10, 10, 10, 10, 10, 10, 20, 10, 10, 10,  5, 20 },
/*FAIRY   */ { 10,  5, 10, 10, 10, 10, 20,  5, 10, 10, 10, 10, 10, 10, 20, 20,  5, 10 },
};

double type_effectiveness(ElementalType typeA, ElementalType typeB)
{
    if (typeA <= TYPE_NONE || typeA >= TYPE_COUNT)
        return 0.0;

    if (typeB <= TYPE_NONE || typeB >= TYPE_COUNT)
        return 1.0;

    return type_chart[typeA - 1][typeB - 1] / 10.0;
}

const char *GetTypeName(ElementalType type)
{
    switch (type) {
        case TYPE_NORMAL:   return "Normal";
        case TYPE_FIRE:     return "Fire";
        case TYPE_WATER:    return "Water";
        case TYPE_GRASS:    return "Grass";
        case TYPE_ELECTRIC: return "Electric";
        case TYPE_ICE:      return "Ice";
        case TYPE_FIGHTING: return "Fighting";
        case TYPE_POISON:   return "Poison";
        case TYPE_GROUND:   return "Ground";
        case TYPE_FLYING:   return "Flying";
        case TYPE_PSYCHIC:  return "Psychic";
        case TYPE_BUG:      return "Bug";
        case TYPE_ROCK:     return "Rock";
        case TYPE_GHOST:    return "Ghost";
        case TYPE_DRAGON:   return "Dragon";
        case TYPE_DARK:     return "Dark";
        case TYPE_STEEL:    return "Steel";
        case TYPE_FAIRY:    return "Fairy";
        default:            return "None";
    }
}

const char *GetStatusText(StatusEffect effect)
{
    switch (effect) {
        case STATUS_POISON:    return "PSN";
        case STATUS_BURN:      return "BRN";
        case STATUS_PARALYSIS: return "PAR";
        case STATUS_SLEEP:     return "SLP";
        case STATUS_FREEZE:    return "FRZ";
        default:               return "";
    }
}
