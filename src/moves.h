#ifndef MOVES_H
#define MOVES_H

#include "types.h"

move CreateMove(const char *name, int bp, int accuracy, int max_pp,
                ElementalType type, MoveCategory category,
                StatusEffect effect, int probability);

move GetMoveByName(const char *name);
int GetTotalMoveCount(void);
const move *GetMasterMoveList(void);

#endif // MOVES_H
