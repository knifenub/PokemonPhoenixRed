#include "pokemon_database.h"
#include "moves.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>

// Full Gen 1 fully-evolved & non-evolving Pokémon database (81 Pokémon)
static const PokemonSpecies gen1_database[] = {
    // 1
    {
        .id = 3, .name = "Venusaur",
        .baseHP = 80, .baseAttack = 82, .baseDefense = 83, .baseSpAttack = 100, .baseSpDefense = 100, .baseSpeed = 80,
        .type1 = TYPE_GRASS, .type2 = TYPE_POISON,
        .defaultMoveNames = {"Vine Whip", "Razor Leaf", "Sleep Powder", "Sludge Bomb"}
    },
    // 2
    {
        .id = 6, .name = "Charizard",
        .baseHP = 78, .baseAttack = 84, .baseDefense = 78, .baseSpAttack = 109, .baseSpDefense = 85, .baseSpeed = 100,
        .type1 = TYPE_FIRE, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Flamethrower", "Wing Attack", "Fire Blast", "Slash"}
    },
    // 3
    {
        .id = 9, .name = "Blastoise",
        .baseHP = 79, .baseAttack = 83, .baseDefense = 100, .baseSpAttack = 85, .baseSpDefense = 105, .baseSpeed = 78,
        .type1 = TYPE_WATER, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Hydro Pump", "Surf", "Ice Beam", "Bite"}
    },
    // 4
    {
        .id = 12, .name = "Butterfree",
        .baseHP = 60, .baseAttack = 45, .baseDefense = 50, .baseSpAttack = 90, .baseSpDefense = 80, .baseSpeed = 70,
        .type1 = TYPE_BUG, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Bug Buzz", "Air Slash", "Sleep Powder", "Psybeam"}
    },
    // 5
    {
        .id = 15, .name = "Beedrill",
        .baseHP = 65, .baseAttack = 90, .baseDefense = 40, .baseSpAttack = 45, .baseSpDefense = 80, .baseSpeed = 75,
        .type1 = TYPE_BUG, .type2 = TYPE_POISON,
        .defaultMoveNames = {"Twineedle", "Poison Jab", "X-Scissor", "Aerial Ace"}
    },
    // 6
    {
        .id = 18, .name = "Pidgeot",
        .baseHP = 83, .baseAttack = 80, .baseDefense = 75, .baseSpAttack = 70, .baseSpDefense = 70, .baseSpeed = 101,
        .type1 = TYPE_NORMAL, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Wing Attack", "Air Slash", "Quick Attack", "Hurricane"}
    },
    // 7
    {
        .id = 20, .name = "Raticate",
        .baseHP = 55, .baseAttack = 81, .baseDefense = 60, .baseSpAttack = 50, .baseSpDefense = 70, .baseSpeed = 97,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Hyper Beam", "Crunch", "Double-Edge", "Quick Attack"}
    },
    // 8
    {
        .id = 22, .name = "Fearow",
        .baseHP = 65, .baseAttack = 90, .baseDefense = 65, .baseSpAttack = 61, .baseSpDefense = 61, .baseSpeed = 100,
        .type1 = TYPE_NORMAL, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Drill Peck", "Aerial Ace", "Double-Edge", "Quick Attack"}
    },
    // 9
    {
        .id = 24, .name = "Arbok",
        .baseHP = 60, .baseAttack = 95, .baseDefense = 69, .baseSpAttack = 65, .baseSpDefense = 79, .baseSpeed = 80,
        .type1 = TYPE_POISON, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Poison Jab", "Sludge Bomb", "Crunch", "Earthquake"}
    },
    // 10
    {
        .id = 26, .name = "Raichu",
        .baseHP = 60, .baseAttack = 90, .baseDefense = 55, .baseSpAttack = 90, .baseSpDefense = 80, .baseSpeed = 110,
        .type1 = TYPE_ELECTRIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Thunderbolt", "Thunder Wave", "Quick Attack", "Brick Break"}
    },
    // 11
    {
        .id = 28, .name = "Sandslash",
        .baseHP = 75, .baseAttack = 100, .baseDefense = 110, .baseSpAttack = 45, .baseSpDefense = 55, .baseSpeed = 65,
        .type1 = TYPE_GROUND, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Earthquake", "Slash", "Rock Slide", "Poison Jab"}
    },
    // 12
    {
        .id = 31, .name = "Nidoqueen",
        .baseHP = 90, .baseAttack = 92, .baseDefense = 87, .baseSpAttack = 75, .baseSpDefense = 85, .baseSpeed = 76,
        .type1 = TYPE_POISON, .type2 = TYPE_GROUND,
        .defaultMoveNames = {"Earthquake", "Sludge Bomb", "Body Slam", "Ice Beam"}
    },
    // 13
    {
        .id = 34, .name = "Nidoking",
        .baseHP = 81, .baseAttack = 102, .baseDefense = 77, .baseSpAttack = 85, .baseSpDefense = 75, .baseSpeed = 85,
        .type1 = TYPE_POISON, .type2 = TYPE_GROUND,
        .defaultMoveNames = {"Earthquake", "Sludge Bomb", "Megahorn", "Thunderbolt"}
    },
    // 14
    {
        .id = 36, .name = "Clefable",
        .baseHP = 95, .baseAttack = 70, .baseDefense = 73, .baseSpAttack = 95, .baseSpDefense = 90, .baseSpeed = 60,
        .type1 = TYPE_FAIRY, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Moonblast", "Dazzling Gleam", "Psychic", "Body Slam"}
    },
    // 15
    {
        .id = 38, .name = "Ninetales",
        .baseHP = 73, .baseAttack = 76, .baseDefense = 75, .baseSpAttack = 81, .baseSpDefense = 100, .baseSpeed = 100,
        .type1 = TYPE_FIRE, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Flamethrower", "Fire Blast", "Quick Attack", "Dark Pulse"}
    },
    // 16
    {
        .id = 40, .name = "Wigglytuff",
        .baseHP = 140, .baseAttack = 70, .baseDefense = 45, .baseSpAttack = 85, .baseSpDefense = 50, .baseSpeed = 45,
        .type1 = TYPE_NORMAL, .type2 = TYPE_FAIRY,
        .defaultMoveNames = {"Dazzling Gleam", "Body Slam", "Hyper Beam", "Play Rough"}
    },
    // 17
    {
        .id = 42, .name = "Golbat",
        .baseHP = 75, .baseAttack = 80, .baseDefense = 70, .baseSpAttack = 65, .baseSpDefense = 75, .baseSpeed = 90,
        .type1 = TYPE_POISON, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Wing Attack", "Poison Jab", "Bite", "Air Slash"}
    },
    // 18
    {
        .id = 45, .name = "Vileplume",
        .baseHP = 75, .baseAttack = 80, .baseDefense = 85, .baseSpAttack = 110, .baseSpDefense = 90, .baseSpeed = 50,
        .type1 = TYPE_GRASS, .type2 = TYPE_POISON,
        .defaultMoveNames = {"Petal Dance", "Sludge Bomb", "Sleep Powder", "Giga Drain"}
    },
    // 19
    {
        .id = 47, .name = "Parasect",
        .baseHP = 60, .baseAttack = 95, .baseDefense = 80, .baseSpAttack = 60, .baseSpDefense = 80, .baseSpeed = 30,
        .type1 = TYPE_BUG, .type2 = TYPE_GRASS,
        .defaultMoveNames = {"X-Scissor", "Giga Drain", "Sleep Powder", "Slash"}
    },
    // 20
    {
        .id = 49, .name = "Venomoth",
        .baseHP = 70, .baseAttack = 65, .baseDefense = 60, .baseSpAttack = 90, .baseSpDefense = 75, .baseSpeed = 90,
        .type1 = TYPE_BUG, .type2 = TYPE_POISON,
        .defaultMoveNames = {"Bug Buzz", "Sludge Bomb", "Psybeam", "Sleep Powder"}
    },
    // 21
    {
        .id = 51, .name = "Dugtrio",
        .baseHP = 35, .baseAttack = 100, .baseDefense = 50, .baseSpAttack = 50, .baseSpDefense = 70, .baseSpeed = 120,
        .type1 = TYPE_GROUND, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Earthquake", "Slash", "Rock Slide", "Night Slash"}
    },
    // 22
    {
        .id = 53, .name = "Persian",
        .baseHP = 65, .baseAttack = 70, .baseDefense = 60, .baseSpAttack = 65, .baseSpDefense = 65, .baseSpeed = 115,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Slash", "Bite", "Swift", "Play Rough"}
    },
    // 23
    {
        .id = 55, .name = "Golduck",
        .baseHP = 80, .baseAttack = 82, .baseDefense = 78, .baseSpAttack = 95, .baseSpDefense = 80, .baseSpeed = 85,
        .type1 = TYPE_WATER, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Surf", "Hydro Pump", "Psychic", "Ice Beam"}
    },
    // 24
    {
        .id = 57, .name = "Primeape",
        .baseHP = 65, .baseAttack = 105, .baseDefense = 60, .baseSpAttack = 60, .baseSpDefense = 70, .baseSpeed = 95,
        .type1 = TYPE_FIGHTING, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Cross Chop", "Close Combat", "Karate Chop", "Rock Slide"}
    },
    // 25
    {
        .id = 59, .name = "Arcanine",
        .baseHP = 90, .baseAttack = 110, .baseDefense = 80, .baseSpAttack = 100, .baseSpDefense = 80, .baseSpeed = 95,
        .type1 = TYPE_FIRE, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Flare Blitz", "Flamethrower", "Extreme Speed", "Crunch"}
    },
    // 26
    {
        .id = 62, .name = "Poliwrath",
        .baseHP = 90, .baseAttack = 95, .baseDefense = 95, .baseSpAttack = 70, .baseSpDefense = 90, .baseSpeed = 70,
        .type1 = TYPE_WATER, .type2 = TYPE_FIGHTING,
        .defaultMoveNames = {"Hydro Pump", "Surf", "Submission", "Brick Break"}
    },
    // 27
    {
        .id = 65, .name = "Alakazam",
        .baseHP = 55, .baseAttack = 50, .baseDefense = 45, .baseSpAttack = 135, .baseSpDefense = 95, .baseSpeed = 120,
        .type1 = TYPE_PSYCHIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Psychic", "Psybeam", "Shadow Ball", "Dazzling Gleam"}
    },
    // 28
    {
        .id = 68, .name = "Machamp",
        .baseHP = 90, .baseAttack = 130, .baseDefense = 80, .baseSpAttack = 65, .baseSpDefense = 85, .baseSpeed = 55,
        .type1 = TYPE_FIGHTING, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Close Combat", "Cross Chop", "Stone Edge", "Fire Punch"}
    },
    // 29
    {
        .id = 71, .name = "Victreebel",
        .baseHP = 80, .baseAttack = 105, .baseDefense = 65, .baseSpAttack = 100, .baseSpDefense = 70, .baseSpeed = 70,
        .type1 = TYPE_GRASS, .type2 = TYPE_POISON,
        .defaultMoveNames = {"Razor Leaf", "Sludge Bomb", "Sleep Powder", "Solar Beam"}
    },
    // 30
    {
        .id = 73, .name = "Tentacruel",
        .baseHP = 80, .baseAttack = 70, .baseDefense = 65, .baseSpAttack = 80, .baseSpDefense = 120, .baseSpeed = 100,
        .type1 = TYPE_WATER, .type2 = TYPE_POISON,
        .defaultMoveNames = {"Surf", "Hydro Pump", "Sludge Bomb", "Ice Beam"}
    },
    // 31
    {
        .id = 76, .name = "Golem",
        .baseHP = 80, .baseAttack = 120, .baseDefense = 130, .baseSpAttack = 55, .baseSpDefense = 65, .baseSpeed = 45,
        .type1 = TYPE_ROCK, .type2 = TYPE_GROUND,
        .defaultMoveNames = {"Earthquake", "Stone Edge", "Rock Slide", "Double-Edge"}
    },
    // 32
    {
        .id = 78, .name = "Rapidash",
        .baseHP = 65, .baseAttack = 100, .baseDefense = 70, .baseSpAttack = 80, .baseSpDefense = 80, .baseSpeed = 105,
        .type1 = TYPE_FIRE, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Flare Blitz", "Flamethrower", "Megahorn", "Quick Attack"}
    },
    // 33
    {
        .id = 80, .name = "Slowbro",
        .baseHP = 95, .baseAttack = 75, .baseDefense = 110, .baseSpAttack = 100, .baseSpDefense = 80, .baseSpeed = 30,
        .type1 = TYPE_WATER, .type2 = TYPE_PSYCHIC,
        .defaultMoveNames = {"Surf", "Psychic", "Ice Beam", "Flamethrower"}
    },
    // 34
    {
        .id = 82, .name = "Magneton",
        .baseHP = 50, .baseAttack = 60, .baseDefense = 95, .baseSpAttack = 120, .baseSpDefense = 70, .baseSpeed = 70,
        .type1 = TYPE_ELECTRIC, .type2 = TYPE_STEEL,
        .defaultMoveNames = {"Thunderbolt", "Flash Cannon", "Tri Attack", "Thunder Wave"}
    },
    // 35
    {
        .id = 83, .name = "Farfetch'd",
        .baseHP = 52, .baseAttack = 90, .baseDefense = 55, .baseSpAttack = 58, .baseSpDefense = 62, .baseSpeed = 60,
        .type1 = TYPE_NORMAL, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Slash", "Aerial Ace", "Night Slash", "Quick Attack"}
    },
    // 36
    {
        .id = 85, .name = "Dodrio",
        .baseHP = 60, .baseAttack = 110, .baseDefense = 70, .baseSpAttack = 60, .baseSpDefense = 60, .baseSpeed = 110,
        .type1 = TYPE_NORMAL, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Drill Peck", "Tri Attack", "Quick Attack", "Steel Wing"}
    },
    // 37
    {
        .id = 87, .name = "Dewgong",
        .baseHP = 90, .baseAttack = 70, .baseDefense = 80, .baseSpAttack = 70, .baseSpDefense = 95, .baseSpeed = 70,
        .type1 = TYPE_WATER, .type2 = TYPE_ICE,
        .defaultMoveNames = {"Surf", "Ice Beam", "Blizzard", "Headbutt"}
    },
    // 38
    {
        .id = 89, .name = "Muk",
        .baseHP = 105, .baseAttack = 105, .baseDefense = 75, .baseSpAttack = 65, .baseSpDefense = 100, .baseSpeed = 50,
        .type1 = TYPE_POISON, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Sludge Bomb", "Poison Jab", "Dark Pulse", "Fire Blast"}
    },
    // 39
    {
        .id = 91, .name = "Cloyster",
        .baseHP = 50, .baseAttack = 95, .baseDefense = 180, .baseSpAttack = 85, .baseSpDefense = 45, .baseSpeed = 70,
        .type1 = TYPE_WATER, .type2 = TYPE_ICE,
        .defaultMoveNames = {"Hydro Pump", "Ice Beam", "Blizzard", "Surf"}
    },
    // 40
    {
        .id = 94, .name = "Gengar",
        .baseHP = 60, .baseAttack = 65, .baseDefense = 60, .baseSpAttack = 130, .baseSpDefense = 75, .baseSpeed = 110,
        .type1 = TYPE_GHOST, .type2 = TYPE_POISON,
        .defaultMoveNames = {"Shadow Ball", "Sludge Bomb", "Dark Pulse", "Thunderbolt"}
    },
    // 41
    {
        .id = 95, .name = "Onix",
        .baseHP = 35, .baseAttack = 45, .baseDefense = 160, .baseSpAttack = 30, .baseSpDefense = 45, .baseSpeed = 70,
        .type1 = TYPE_ROCK, .type2 = TYPE_GROUND,
        .defaultMoveNames = {"Stone Edge", "Earthquake", "Rock Throw", "Iron Head"}
    },
    // 42
    {
        .id = 97, .name = "Hypno",
        .baseHP = 85, .baseAttack = 73, .baseDefense = 70, .baseSpAttack = 73, .baseSpDefense = 115, .baseSpeed = 67,
        .type1 = TYPE_PSYCHIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Psychic", "Hypnosis", "Dream Eater", "Shadow Ball"}
    },
    // 43
    {
        .id = 99, .name = "Kingler",
        .baseHP = 55, .baseAttack = 130, .baseDefense = 115, .baseSpAttack = 50, .baseSpDefense = 50, .baseSpeed = 75,
        .type1 = TYPE_WATER, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Crabhammer", "X-Scissor", "Body Slam", "Rock Slide"}
    },
    // 44
    {
        .id = 101, .name = "Electrode",
        .baseHP = 60, .baseAttack = 50, .baseDefense = 70, .baseSpAttack = 80, .baseSpDefense = 80, .baseSpeed = 150,
        .type1 = TYPE_ELECTRIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Thunderbolt", "Thunder", "Swift", "Thunder Wave"}
    },
    // 45
    {
        .id = 103, .name = "Exeggutor",
        .baseHP = 95, .baseAttack = 95, .baseDefense = 85, .baseSpAttack = 125, .baseSpDefense = 75, .baseSpeed = 55,
        .type1 = TYPE_GRASS, .type2 = TYPE_PSYCHIC,
        .defaultMoveNames = {"Solar Beam", "Psychic", "Giga Drain", "Sleep Powder"}
    },
    // 46
    {
        .id = 105, .name = "Marowak",
        .baseHP = 60, .baseAttack = 80, .baseDefense = 110, .baseSpAttack = 50, .baseSpDefense = 80, .baseSpeed = 45,
        .type1 = TYPE_GROUND, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Earthquake", "Bone Club", "Stone Edge", "Fire Punch"}
    },
    // 47
    {
        .id = 106, .name = "Hitmonlee",
        .baseHP = 50, .baseAttack = 120, .baseDefense = 53, .baseSpAttack = 35, .baseSpDefense = 110, .baseSpeed = 87,
        .type1 = TYPE_FIGHTING, .type2 = TYPE_NONE,
        .defaultMoveNames = {"High Jump Kick", "Close Combat", "Brick Break", "Mega Kick"}
    },
    // 48
    {
        .id = 107, .name = "Hitmonchan",
        .baseHP = 50, .baseAttack = 105, .baseDefense = 79, .baseSpAttack = 35, .baseSpDefense = 110, .baseSpeed = 76,
        .type1 = TYPE_FIGHTING, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Close Combat", "Fire Punch", "Ice Punch", "Thunder Punch"}
    },
    // 49
    {
        .id = 108, .name = "Lickitung",
        .baseHP = 90, .baseAttack = 55, .baseDefense = 75, .baseSpAttack = 60, .baseSpDefense = 75, .baseSpeed = 30,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Body Slam", "Lick", "Surf", "Mega Punch"}
    },
    // 50
    {
        .id = 110, .name = "Weezing",
        .baseHP = 65, .baseAttack = 90, .baseDefense = 120, .baseSpAttack = 85, .baseSpDefense = 70, .baseSpeed = 60,
        .type1 = TYPE_POISON, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Sludge Bomb", "Flamethrower", "Dark Pulse", "Thunderbolt"}
    },
    // 51
    {
        .id = 112, .name = "Rhydon",
        .baseHP = 105, .baseAttack = 130, .baseDefense = 120, .baseSpAttack = 45, .baseSpDefense = 45, .baseSpeed = 40,
        .type1 = TYPE_GROUND, .type2 = TYPE_ROCK,
        .defaultMoveNames = {"Earthquake", "Stone Edge", "Megahorn", "Rock Slide"}
    },
    // 52
    {
        .id = 113, .name = "Chansey",
        .baseHP = 250, .baseAttack = 5, .baseDefense = 5, .baseSpAttack = 35, .baseSpDefense = 105, .baseSpeed = 50,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Body Slam", "Tri Attack", "Psychic", "Ice Beam"}
    },
    // 53
    {
        .id = 114, .name = "Tangela",
        .baseHP = 65, .baseAttack = 55, .baseDefense = 115, .baseSpAttack = 100, .baseSpDefense = 40, .baseSpeed = 60,
        .type1 = TYPE_GRASS, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Giga Drain", "Solar Beam", "Sleep Powder", "Ancient Power"}
    },
    // 54
    {
        .id = 115, .name = "Kangaskhan",
        .baseHP = 105, .baseAttack = 95, .baseDefense = 80, .baseSpAttack = 40, .baseSpDefense = 80, .baseSpeed = 90,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Mega Punch", "Crunch", "Earthquake", "Outrage"}
    },
    // 55
    {
        .id = 117, .name = "Seadra",
        .baseHP = 55, .baseAttack = 65, .baseDefense = 95, .baseSpAttack = 95, .baseSpDefense = 45, .baseSpeed = 85,
        .type1 = TYPE_WATER, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Hydro Pump", "Surf", "Dragon Pulse", "Ice Beam"}
    },
    // 56
    {
        .id = 119, .name = "Seaking",
        .baseHP = 80, .baseAttack = 92, .baseDefense = 65, .baseSpAttack = 65, .baseSpDefense = 80, .baseSpeed = 68,
        .type1 = TYPE_WATER, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Waterfall", "Megahorn", "Surf", "Ice Beam"}
    },
    // 57
    {
        .id = 121, .name = "Starmie",
        .baseHP = 60, .baseAttack = 75, .baseDefense = 85, .baseSpAttack = 100, .baseSpDefense = 85, .baseSpeed = 115,
        .type1 = TYPE_WATER, .type2 = TYPE_PSYCHIC,
        .defaultMoveNames = {"Surf", "Psychic", "Thunderbolt", "Ice Beam"}
    },
    // 58
    {
        .id = 122, .name = "Mr. Mime",
        .baseHP = 40, .baseAttack = 45, .baseDefense = 65, .baseSpAttack = 100, .baseSpDefense = 120, .baseSpeed = 90,
        .type1 = TYPE_PSYCHIC, .type2 = TYPE_FAIRY,
        .defaultMoveNames = {"Psychic", "Dazzling Gleam", "Thunderbolt", "Psybeam"}
    },
    // 59
    {
        .id = 123, .name = "Scyther",
        .baseHP = 70, .baseAttack = 110, .baseDefense = 80, .baseSpAttack = 55, .baseSpDefense = 80, .baseSpeed = 105,
        .type1 = TYPE_BUG, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"X-Scissor", "Wing Attack", "Slash", "Night Slash"}
    },
    // 60
    {
        .id = 124, .name = "Jynx",
        .baseHP = 65, .baseAttack = 50, .baseDefense = 35, .baseSpAttack = 115, .baseSpDefense = 95, .baseSpeed = 95,
        .type1 = TYPE_ICE, .type2 = TYPE_PSYCHIC,
        .defaultMoveNames = {"Ice Beam", "Blizzard", "Psychic", "Sleep Powder"}
    },
    // 61
    {
        .id = 125, .name = "Electabuzz",
        .baseHP = 65, .baseAttack = 83, .baseDefense = 57, .baseSpAttack = 95, .baseSpDefense = 85, .baseSpeed = 105,
        .type1 = TYPE_ELECTRIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Thunderbolt", "Thunder Punch", "Psychic", "Cross Chop"}
    },
    // 62
    {
        .id = 126, .name = "Magmar",
        .baseHP = 65, .baseAttack = 95, .baseDefense = 57, .baseSpAttack = 100, .baseSpDefense = 85, .baseSpeed = 93,
        .type1 = TYPE_FIRE, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Flamethrower", "Fire Blast", "Fire Punch", "Psychic"}
    },
    // 63
    {
        .id = 127, .name = "Pinsir",
        .baseHP = 65, .baseAttack = 125, .baseDefense = 100, .baseSpAttack = 55, .baseSpDefense = 70, .baseSpeed = 85,
        .type1 = TYPE_BUG, .type2 = TYPE_NONE,
        .defaultMoveNames = {"X-Scissor", "Submission", "Close Combat", "Stone Edge"}
    },
    // 64
    {
        .id = 128, .name = "Tauros",
        .baseHP = 75, .baseAttack = 100, .baseDefense = 95, .baseSpAttack = 40, .baseSpDefense = 70, .baseSpeed = 110,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Body Slam", "Earthquake", "Stone Edge", "Iron Head"}
    },
    // 65
    {
        .id = 130, .name = "Gyarados",
        .baseHP = 95, .baseAttack = 125, .baseDefense = 79, .baseSpAttack = 60, .baseSpDefense = 100, .baseSpeed = 81,
        .type1 = TYPE_WATER, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Waterfall", "Crunch", "Outrage", "Ice Beam"}
    },
    // 66
    {
        .id = 131, .name = "Lapras",
        .baseHP = 130, .baseAttack = 85, .baseDefense = 80, .baseSpAttack = 85, .baseSpDefense = 95, .baseSpeed = 60,
        .type1 = TYPE_WATER, .type2 = TYPE_ICE,
        .defaultMoveNames = {"Surf", "Hydro Pump", "Ice Beam", "Thunderbolt"}
    },
    // 67
    {
        .id = 132, .name = "Ditto",
        .baseHP = 48, .baseAttack = 48, .baseDefense = 48, .baseSpAttack = 48, .baseSpDefense = 48, .baseSpeed = 48,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Tackle", "Quick Attack", "Swift", "Double-Edge"}
    },
    // 68
    {
        .id = 134, .name = "Vaporeon",
        .baseHP = 130, .baseAttack = 65, .baseDefense = 60, .baseSpAttack = 110, .baseSpDefense = 95, .baseSpeed = 65,
        .type1 = TYPE_WATER, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Surf", "Hydro Pump", "Ice Beam", "Quick Attack"}
    },
    // 69
    {
        .id = 135, .name = "Jolteon",
        .baseHP = 65, .baseAttack = 65, .baseDefense = 60, .baseSpAttack = 110, .baseSpDefense = 95, .baseSpeed = 130,
        .type1 = TYPE_ELECTRIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Thunderbolt", "Thunder", "Pin Missile", "Quick Attack"}
    },
    // 70
    {
        .id = 136, .name = "Flareon",
        .baseHP = 65, .baseAttack = 130, .baseDefense = 60, .baseSpAttack = 95, .baseSpDefense = 110, .baseSpeed = 65,
        .type1 = TYPE_FIRE, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Flare Blitz", "Flamethrower", "Fire Blast", "Quick Attack"}
    },
    // 71
    {
        .id = 137, .name = "Porygon",
        .baseHP = 65, .baseAttack = 60, .baseDefense = 70, .baseSpAttack = 85, .baseSpDefense = 75, .baseSpeed = 40,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Tri Attack", "Psybeam", "Thunderbolt", "Ice Beam"}
    },
    // 72
    {
        .id = 139, .name = "Omastar",
        .baseHP = 70, .baseAttack = 60, .baseDefense = 125, .baseSpAttack = 115, .baseSpDefense = 70, .baseSpeed = 55,
        .type1 = TYPE_ROCK, .type2 = TYPE_WATER,
        .defaultMoveNames = {"Hydro Pump", "Surf", "Ancient Power", "Ice Beam"}
    },
    // 73
    {
        .id = 141, .name = "Kabutops",
        .baseHP = 60, .baseAttack = 115, .baseDefense = 105, .baseSpAttack = 65, .baseSpDefense = 70, .baseSpeed = 80,
        .type1 = TYPE_ROCK, .type2 = TYPE_WATER,
        .defaultMoveNames = {"Stone Edge", "Waterfall", "Slash", "X-Scissor"}
    },
    // 74
    {
        .id = 142, .name = "Aerodactyl",
        .baseHP = 80, .baseAttack = 105, .baseDefense = 65, .baseSpAttack = 60, .baseSpDefense = 75, .baseSpeed = 130,
        .type1 = TYPE_ROCK, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Stone Edge", "Wing Attack", "Crunch", "Earthquake"}
    },
    // 75
    {
        .id = 143, .name = "Snorlax",
        .baseHP = 160, .baseAttack = 110, .baseDefense = 65, .baseSpAttack = 65, .baseSpDefense = 110, .baseSpeed = 30,
        .type1 = TYPE_NORMAL, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Body Slam", "Crunch", "Earthquake", "Hyper Beam"}
    },
    // 76
    {
        .id = 144, .name = "Articuno",
        .baseHP = 90, .baseAttack = 85, .baseDefense = 100, .baseSpAttack = 95, .baseSpDefense = 125, .baseSpeed = 85,
        .type1 = TYPE_ICE, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Ice Beam", "Blizzard", "Hurricane", "Air Slash"}
    },
    // 77
    {
        .id = 145, .name = "Zapdos",
        .baseHP = 90, .baseAttack = 90, .baseDefense = 85, .baseSpAttack = 125, .baseSpDefense = 90, .baseSpeed = 100,
        .type1 = TYPE_ELECTRIC, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Thunderbolt", "Thunder", "Drill Peck", "Hurricane"}
    },
    // 78
    {
        .id = 146, .name = "Moltres",
        .baseHP = 90, .baseAttack = 100, .baseDefense = 90, .baseSpAttack = 125, .baseSpDefense = 85, .baseSpeed = 90,
        .type1 = TYPE_FIRE, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Flamethrower", "Fire Blast", "Air Slash", "Hurricane"}
    },
    // 79
    {
        .id = 149, .name = "Dragonite",
        .baseHP = 91, .baseAttack = 134, .baseDefense = 95, .baseSpAttack = 100, .baseSpDefense = 100, .baseSpeed = 80,
        .type1 = TYPE_DRAGON, .type2 = TYPE_FLYING,
        .defaultMoveNames = {"Outrage", "Dragon Claw", "Wing Attack", "Extreme Speed"}
    },
    // 80
    {
        .id = 150, .name = "Mewtwo",
        .baseHP = 106, .baseAttack = 110, .baseDefense = 90, .baseSpAttack = 154, .baseSpDefense = 90, .baseSpeed = 130,
        .type1 = TYPE_PSYCHIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Psychic", "Shadow Ball", "Flamethrower", "Thunderbolt"}
    },
    // 81
    {
        .id = 151, .name = "Mew",
        .baseHP = 100, .baseAttack = 100, .baseDefense = 100, .baseSpAttack = 100, .baseSpDefense = 100, .baseSpeed = 100,
        .type1 = TYPE_PSYCHIC, .type2 = TYPE_NONE,
        .defaultMoveNames = {"Psychic", "Ancient Power", "Solar Beam", "Thunderbolt"}
    }
};

static const int gen1_database_count = sizeof(gen1_database) / sizeof(gen1_database[0]);

int GetDatabasePokemonCount(void)
{
    return gen1_database_count;
}

const PokemonSpecies *GetPokemonSpeciesByIndex(int index)
{
    if (index < 0 || index >= gen1_database_count)
        return NULL;
    return &gen1_database[index];
}

const PokemonSpecies *GetPokemonSpeciesByID(int pokedexId)
{
    for (int i = 0; i < gen1_database_count; i++) {
        if (gen1_database[i].id == pokedexId)
            return &gen1_database[i];
    }
    return NULL;
}

const PokemonSpecies *FindPokemonSpeciesByName(const char *name)
{
    if (!name || name[0] == '\0')
        return NULL;

    for (int i = 0; i < gen1_database_count; i++) {
        if (strcasecmp(gen1_database[i].name, name) == 0)
            return &gen1_database[i];
    }
    return NULL;
}

// Level-based stat calculations with competitive EVs (31 IV, 252 EV)
static int CalcHP(int base, int level)
{
    if (level <= 0) level = 100;
    return (((2 * base + 31 + 63) * level) / 100) + level + 10;
}

static int CalcStat(int base, int level)
{
    if (level <= 0) level = 100;
    return (((2 * base + 31 + 63) * level) / 100) + 5;
}

Pokemon CreatePokemon(const PokemonSpecies *species, int level)
{
    Pokemon p;
    memset(&p, 0, sizeof(Pokemon));

    if (!species)
        return p;

    if (level <= 0)
        level = 100;

    strncpy(p.Name, species->name, sizeof(p.Name) - 1);
    p.Name[sizeof(p.Name) - 1] = '\0';

    p.level = level;
    p.type1 = species->type1;
    p.type2 = species->type2;

    p.max_health = CalcHP(species->baseHP, level);
    p.current_health = p.max_health;

    p.max_attack = CalcStat(species->baseAttack, level);
    p.attack = p.max_attack;

    p.max_defense = CalcStat(species->baseDefense, level);
    p.defense = p.max_defense;

    p.special_attack = CalcStat(species->baseSpAttack, level);
    p.special_defense = CalcStat(species->baseSpDefense, level);

    p.max_speed = CalcStat(species->baseSpeed, level);
    p.speed = p.max_speed;

    p.accuracy = 100;
    p.max_accuracy = 100;
    p.evasiveness = 100;
    p.max_evasiveness = 100;

    p.effect = STATUS_NONE;
    p.status_conditions_rounds_left = 0;

    for (int i = 0; i < 4; i++) {
        p.Moves[i] = GetMoveByName(species->defaultMoveNames[i]);
    }

    return p;
}

Pokemon CreatePokemonByName(const char *name, int level)
{
    const PokemonSpecies *species = FindPokemonSpeciesByName(name);
    if (!species) {
        // Fallback to first Pokémon (Venusaur)
        species = &gen1_database[0];
    }
    return CreatePokemon(species, level);
}
