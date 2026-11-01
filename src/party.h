#ifndef PARTY_H
#define PARTY_H

#include "types.h"
#include <stdbool.h>

void InitParty(Party *party);
bool AddToParty(Party *party, Pokemon p);
bool RemoveFromParty(Party *party, int index);

Pokemon *GetActivePokemon(Party *party);
const Pokemon *GetActivePokemonConst(const Party *party);

bool IsPartyWiped(const Party *party);
int GetAlivePartyCount(const Party *party);
int GetNextAlivePokemonIndex(const Party *party);

bool SwitchPokemon(Party *party, int newIndex);

// AI logic: picks Pokemon with type advantage vs playerActive;
// If no advantage, picks the next alive Pokemon in party order.
int ChooseBestOpponentSwitch(const Party *enemyParty, const Pokemon *playerActive);

// Generates a random team from the database for the opponent
void GenerateRandomParty(Party *party, int count, int level);

#endif // PARTY_H
