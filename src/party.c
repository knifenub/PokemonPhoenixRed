#include "party.h"
#include "pokemon_database.h"
#include "type_chart.h"
#include <string.h>
#include <stdlib.h>

void InitParty(Party *party)
{
    if (!party) return;
    memset(party, 0, sizeof(Party));
    party->count = 0;
    party->activeIndex = 0;
}

bool AddToParty(Party *party, Pokemon p)
{
    if (!party || party->count >= MAX_PARTY_SIZE)
        return false;

    party->members[party->count] = p;
    party->count++;
    return true;
}

bool RemoveFromParty(Party *party, int index)
{
    if (!party || index < 0 || index >= party->count)
        return false;

    for (int i = index; i < party->count - 1; i++) {
        party->members[i] = party->members[i + 1];
    }
    party->count--;
    if (party->activeIndex >= party->count && party->count > 0) {
        party->activeIndex = party->count - 1;
    }
    return true;
}

Pokemon *GetActivePokemon(Party *party)
{
    if (!party || party->count == 0)
        return NULL;
    if (party->activeIndex < 0 || party->activeIndex >= party->count)
        party->activeIndex = 0;
    return &party->members[party->activeIndex];
}

const Pokemon *GetActivePokemonConst(const Party *party)
{
    if (!party || party->count == 0)
        return NULL;
    if (party->activeIndex < 0 || party->activeIndex >= party->count)
        return &party->members[0];
    return &party->members[party->activeIndex];
}

bool IsPartyWiped(const Party *party)
{
    if (!party || party->count == 0)
        return true;

    for (int i = 0; i < party->count; i++) {
        if (party->members[i].current_health > 0)
            return false;
    }
    return true;
}

int GetAlivePartyCount(const Party *party)
{
    if (!party) return 0;
    int alive = 0;
    for (int i = 0; i < party->count; i++) {
        if (party->members[i].current_health > 0)
            alive++;
    }
    return alive;
}

int GetNextAlivePokemonIndex(const Party *party)
{
    if (!party) return -1;
    for (int i = 0; i < party->count; i++) {
        if (i != party->activeIndex && party->members[i].current_health > 0) {
            return i;
        }
    }
    return -1;
}

bool SwitchPokemon(Party *party, int newIndex)
{
    if (!party) return false;
    if (newIndex < 0 || newIndex >= party->count)
        return false;
    if (newIndex == party->activeIndex)
        return false;
    if (party->members[newIndex].current_health <= 0)
        return false;

    party->activeIndex = newIndex;
    return true;
}

// Chooses opponent Pokémon based on type advantage. If none has advantage, picks next alive.
int ChooseBestOpponentSwitch(const Party *enemyParty, const Pokemon *playerActive)
{
    if (!enemyParty || !playerActive || enemyParty->count == 0)
        return -1;

    int bestIndex = -1;
    double bestAdvantage = 1.0; // Must be strictly greater than 1.0 (neutral) to count as an advantage

    for (int i = 0; i < enemyParty->count; i++) {
        if (i == enemyParty->activeIndex)
            continue;
        if (enemyParty->members[i].current_health <= 0)
            continue;

        const Pokemon *cand = &enemyParty->members[i];

        // Offensive effectiveness against player's active Pokémon
        double eff1 = type_effectiveness(cand->type1, playerActive->type1) *
                      type_effectiveness(cand->type1, playerActive->type2);
        double eff2 = 1.0;
        if (cand->type2 != TYPE_NONE) {
            eff2 = type_effectiveness(cand->type2, playerActive->type1) *
                   type_effectiveness(cand->type2, playerActive->type2);
        }
        double offensiveScore = (eff1 > eff2) ? eff1 : eff2;

        // Defensive resistance bonus (player attacks are not very effective against candidate)
        double incomingEff = type_effectiveness(playerActive->type1, cand->type1) *
                             type_effectiveness(playerActive->type1, cand->type2);
        double totalAdvantage = offensiveScore;
        if (incomingEff < 1.0) {
            totalAdvantage += (1.0 - incomingEff) * 0.5;
        }

        if (totalAdvantage > bestAdvantage) {
            bestAdvantage = totalAdvantage;
            bestIndex = i;
        }
    }

    // If no candidate has a type advantage, pick the next alive Pokémon in party order
    if (bestIndex == -1) {
        bestIndex = GetNextAlivePokemonIndex(enemyParty);
    }

    return bestIndex;
}

void GenerateRandomParty(Party *party, int count, int level)
{
    if (!party) return;
    InitParty(party);

    int dbCount = GetDatabasePokemonCount();
    if (dbCount <= 0) return;

    if (count > MAX_PARTY_SIZE) count = MAX_PARTY_SIZE;
    if (count <= 0) count = 1;

    int pickedIDs[MAX_PARTY_SIZE];
    int pickedCount = 0;

    while (pickedCount < count) {
        int r = rand() % dbCount;
        const PokemonSpecies *sp = GetPokemonSpeciesByIndex(r);
        if (!sp) continue;

        // Avoid duplicate species in the same random party
        bool duplicate = false;
        for (int i = 0; i < pickedCount; i++) {
            if (pickedIDs[i] == sp->id) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;

        pickedIDs[pickedCount] = sp->id;
        AddToParty(party, CreatePokemon(sp, level));
        pickedCount++;
    }
}
