#include "battle.h"
#include "type_chart.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static int minnn(int a, int b) {
    return (a < b) ? a : b;
}

static char messageBuffer[MAX_MESSAGES][MESSAGE_SIZE];
static int messageCount = 0;
static int currentMessage = 0;
static double messageStartTime = 0.0;
static int messageTimerStarted = 0;

void AddBattleMessage(const char *format, ...)
{
    if (messageCount >= MAX_MESSAGES)
        return;

    va_list arguments;
    va_start(arguments, format);
    vsnprintf(messageBuffer[messageCount], MESSAGE_SIZE, format, arguments);
    va_end(arguments);

    messageCount++;
}

int UpdateBattleMessages(void)
{
    /* No active messages */
    if (currentMessage >= messageCount) {
        messageCount = 0;
        currentMessage = 0;
        messageTimerStarted = 0;
        return 0;
    }

    /* Start timer for the current message */
    if (!messageTimerStarted) {
        messageStartTime = GetTime();
        messageTimerStarted = 1;
    }

    /* Move to the next message after one second */
    if (GetTime() - messageStartTime >= MESSAGE_DURATION) {
        currentMessage++;
        messageTimerStarted = 0;

        /* All messages are finished */
        if (currentMessage >= messageCount) {
            messageCount = 0;
            currentMessage = 0;
            return 0;
        }
    }

    return 1;
}

void DrawWrappedText(
    const char *text,
    Rectangle area,
    int fontSize,
    int lineSpacing,
    Color color
) {
    char line[1024] = "";
    char word[128] = "";

    int wordLength = 0;
    float y = area.y;

    for (int i = 0;; i++) {
        char c = text[i];

        if (c != ' ' && c != '\n' && c != '\0') {
            if (wordLength < 127)
                word[wordLength++] = c;
        }
        else {
            word[wordLength] = '\0';

            if (wordLength > 0) {
                char testLine[1280];

                if (line[0] == '\0')
                    snprintf(testLine, sizeof(testLine), "%s", word);
                else
                    snprintf(testLine, sizeof(testLine), "%s %s", line, word);

                if (MeasureText(testLine, fontSize) <= area.width) {
                    strncpy(line, testLine, sizeof(line) - 1);
                    line[sizeof(line) - 1] = '\0';
                }
                else {
                    DrawText(line, (int)area.x, (int)y, fontSize, color);
                    y += fontSize + lineSpacing;
                    strncpy(line, word, sizeof(line) - 1);
                    line[sizeof(line) - 1] = '\0';
                }
            }

            wordLength = 0;

            if (c == '\n') {
                DrawText(line, (int)area.x, (int)y, fontSize, color);
                line[0] = '\0';
                y += fontSize + lineSpacing;
            }

            if (c == '\0')
                break;
        }
    }

    if (line[0] != '\0') {
        DrawText(line, (int)area.x, (int)y, fontSize, color);
    }
}

void DrawBattleTextBox(void)
{
    if (currentMessage >= messageCount)
        return;

    float boxWidth = GetScreenWidth() / 2.0f - 40;
    float boxHeight = GetScreenHeight() / 2.0f - 40;

    Rectangle bar = {
        20,
        333,
        boxWidth,
        boxHeight
    };

    DrawRectangleRec(bar, (Color){245, 245, 235, 255});
    DrawRectangleLinesEx(bar, 4, BLACK);

    Rectangle textArea = {
        bar.x + 15,
        bar.y + 15,
        bar.width - 30,
        bar.height - 30
    };

    DrawWrappedText(
        messageBuffer[currentMessage],
        textArea,
        22,
        2,
        BLACK
    );
}

int damage_calculator(Pokemon *pokemon, Pokemon *pokemonB, move *Move, double random_factor)
{
    if (pokemonB->defense <= 0) pokemonB->defense = 1;
    int damage = (int)(
        (((((2 * pokemon->level) / 5.0 + 2)
        * Move->BP
        * pokemon->attack / pokemonB->defense) / 50.0 + 2)
        * ((pokemon->type1 == Move->type || pokemon->type2 == Move->type) ? 1.5 : 1.0)
        * type_effectiveness(Move->type, pokemonB->type1)
        * type_effectiveness(Move->type, pokemonB->type2)
        * random_factor)
    );
    return damage;
}

int special_damage_calculator(Pokemon *pokemon, Pokemon *pokemonB, move *Move, double random_factor)
{
    if (pokemonB->special_defense <= 0) pokemonB->special_defense = 1;
    int damage = (int)(
        (((((2 * pokemon->level) / 5.0 + 2)
        * Move->BP
        * pokemon->special_attack
        / pokemonB->special_defense) / 50.0 + 2)
        * ((pokemon->type1 == Move->type || pokemon->type2 == Move->type) ? 1.5 : 1.0)
        * type_effectiveness(Move->type, pokemonB->type1)
        * type_effectiveness(Move->type, pokemonB->type2)
        * random_factor)
    );
    return damage;
}

int get_move_weight(move Move, Pokemon target)
{
    int weight = 0;

    if (Move.Current_PP <= 0) {
        return 0;
    }

    if (Move.type == TYPE_NONE || target.type1 == TYPE_NONE) {
        return 0;
    }

    double effectiveness =
        type_effectiveness(Move.type, target.type1) *
        type_effectiveness(Move.type, target.type2);

    if (effectiveness > 1.0) {
        weight += 40;
    }
    else if (effectiveness == 1.0) {
        weight += 10;
    }
    else if (effectiveness > 0.0) {
        weight += 2;
    }
    else {
        weight += 0;
    }

    if (Move.effect != STATUS_NONE && target.effect == STATUS_NONE) {
        weight += 70;
        weight = (weight * Move.probability) / 100;
    }

    weight = (weight * (100 - (rand() % 25))) / 100;
    return weight;
}

void move_status_effect_processing(Pokemon *A, Pokemon *B, move *MoveA)
{
    int roll = rand() % 100;
    if (MoveA->effect != STATUS_NONE &&
        B->effect == STATUS_NONE &&
        roll < MoveA->probability)
    {
        B->effect = MoveA->effect;

        if (MoveA->effect == STATUS_POISON)
            B->status_conditions_rounds_left = 15;
        if (MoveA->effect == STATUS_SLEEP)
            B->status_conditions_rounds_left = 1 + rand() % 2;

        AddBattleMessage("Status effect applied\n");
    }
}

void status_effect_processing(Pokemon *A, Pokemon *B)
{
    (void)B; // Unused in single-target status processing
    if (A->effect == STATUS_BURN) {
        int burn_dmg = A->max_health / 16;
        if (burn_dmg <= 0) burn_dmg = 1;
        A->current_health -= burn_dmg;
        AddBattleMessage("%s: %d Damage taken from BURN effect\n", A->Name, burn_dmg);
    }
    if (A->effect == STATUS_POISON) {
        int dam = (16 - A->status_conditions_rounds_left) * A->max_health / 16;
        if (dam <= 0) dam = 1;
        if (A->status_conditions_rounds_left > 0)
            A->current_health -= dam;
        else
            A->current_health -= 15 * A->max_health / 16;
        AddBattleMessage("%s: %d Damage taken from POISON effect\n", A->Name, dam);
    }
    if (A->effect == STATUS_SLEEP) {
        AddBattleMessage("%s is still sleeping.\n", A->Name);
        if (A->status_conditions_rounds_left <= 0) {
            A->effect = STATUS_NONE;
            AddBattleMessage("%s woke up!\n", A->Name);
        }
    }
    if (A->effect == STATUS_PARALYSIS) {
        AddBattleMessage("%s is still paralyzed!\n", A->Name);
        A->speed = (75 * A->max_speed) / 100;
    }
    if (A->effect == STATUS_FREEZE) {
        int roll = rand() % 256;
        if (roll <= 25) {
            AddBattleMessage("%s thawed and can move again!\n", A->Name);
            A->effect = STATUS_NONE;
        } else {
            AddBattleMessage("%s is still frozen and can't do anything\n", A->Name);
        }
    }
    A->status_conditions_rounds_left -= 1;
}

void turn_processing(Pokemon *A, Pokemon *B, move *MoveA)
{
    if (!(A->effect == STATUS_SLEEP || (A->effect == STATUS_PARALYSIS && rand() % 100 < 25) || A->effect == STATUS_FREEZE)) {
        if (MoveA->Current_PP <= 0) {
            AddBattleMessage("%s has no PP left!\n", MoveA->Name);
            return;
        }

        AddBattleMessage("%s used %s!\n", A->Name, MoveA->Name);

        MoveA->Current_PP--;
        if (B->evasiveness <= 0) B->evasiveness = 1;
        int accuracy = MoveA->Accuracy * A->accuracy / B->evasiveness;

        if (accuracy > 100)
            accuracy = 100;

        if (rand() % 100 >= accuracy) {
            AddBattleMessage("%s's attack missed!\n", A->Name);
            return;
        }

        double random_factor = ((rand() % 39) + 217) / 255.0;
        int damage = 0;

        if (MoveA->category == CATEGORY_PHYSICAL) {
            damage = damage_calculator(A, B, MoveA, random_factor);
            move_status_effect_processing(A, B, MoveA);
        }
        else if (MoveA->category == CATEGORY_SPECIAL) {
            damage = special_damage_calculator(A, B, MoveA, random_factor);
            move_status_effect_processing(A, B, MoveA);
        }
        else {
            move_status_effect_processing(A, B, MoveA);
            return;
        }

        AddBattleMessage("%s took %d damage.\n", B->Name, minnn(B->current_health, damage));

        B->current_health -= damage;
        if (B->current_health < 0)
            B->current_health = 0;

        double effectiveness =
            type_effectiveness(MoveA->type, B->type1) *
            type_effectiveness(MoveA->type, B->type2);

        if (effectiveness > 1.0)
            AddBattleMessage("It was super effective!\n");
        else if (effectiveness == 0.0)
            AddBattleMessage("It had no effect!\n");
        else if (effectiveness < 1.0)
            AddBattleMessage("It was not very effective!\n");
    }
}
