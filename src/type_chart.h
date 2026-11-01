#ifndef TYPE_CHART_H
#define TYPE_CHART_H

#include "types.h"
#include "raylib.h"

double type_effectiveness(ElementalType typeA, ElementalType typeB);
const char *GetTypeName(ElementalType type);
const char *GetStatusText(StatusEffect effect);
const char *GetStatusFullText(StatusEffect effect);
Color GetStatusColor(StatusEffect effect);

#endif // TYPE_CHART_H
