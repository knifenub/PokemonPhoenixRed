#ifndef POKEMON_DATABASE_H
#define POKEMON_DATABASE_H

#include "types.h"

int GetDatabasePokemonCount(void);
const PokemonSpecies *GetPokemonSpeciesByIndex(int index);
const PokemonSpecies *GetPokemonSpeciesByID(int pokedexId);
const PokemonSpecies *FindPokemonSpeciesByName(const char *name);

Pokemon CreatePokemon(const PokemonSpecies *species, int level);
Pokemon CreatePokemonByName(const char *name, int level);

#endif // POKEMON_DATABASE_H
