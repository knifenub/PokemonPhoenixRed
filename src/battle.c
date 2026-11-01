#include "battle.h"
#include "type_chart.h"
#include "ui.h"
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
static int displayedCharCount = 0;
static double lastCharTime = 0.0;
static double lineFinishedTime = 0.0;
static bool lineFullyTyped = false;

#define TYPEWRITER_CHAR_DELAY 0.025  // 25ms per character (smooth & readable)
#define TYPEWRITER_LINE_WAIT   1.2   // 1.2s auto-advance

void AddBattleMessage(const char *format, ...)
{
    if (messageCount >= MAX_MESSAGES)
        return;

    va_list arguments;
    va_start(arguments, format);
    vsnprintf(messageBuffer[messageCount], MESSAGE_SIZE, format, arguments);
    va_end(arguments);

    // Strip trailing whitespace/newlines so text wraps cleanly
    int len = (int)strlen(messageBuffer[messageCount]);
    while (len > 0 && (messageBuffer[messageCount][len - 1] == '\n' ||
                       messageBuffer[messageCount][len - 1] == '\r' ||
                       messageBuffer[messageCount][len - 1] == ' '))
    {
        messageBuffer[messageCount][len - 1] = '\0';
        len--;
    }

    messageCount++;
}

bool IsBattleMessageActive(void)
{
    return (currentMessage < messageCount);
}

void ClearBattleMessages(void)
{
    messageCount = 0;
    currentMessage = 0;
    displayedCharCount = 0;
    lineFullyTyped = false;
}

int UpdateBattleMessages(void)
{
    if (currentMessage >= messageCount) {
        messageCount = 0;
        currentMessage = 0;
        displayedCharCount = 0;
        lineFullyTyped = false;
        return 0;
    }

    const char *fullMsg = messageBuffer[currentMessage];
    int len = (int)strlen(fullMsg);
    if (len == 0) {
        currentMessage++;
        return (currentMessage < messageCount);
    }

    // Check if user requests skip / instant advance
    bool advanceKey = IsKeyPressed(KEY_SPACE) ||
                      IsKeyPressed(KEY_ENTER) ||
                      IsKeyPressed(KEY_Z) ||
                      IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    double now = GetTime();

    if (!lineFullyTyped) {
        if (advanceKey) {
            // Instant reveal of current line
            displayedCharCount = len;
            lineFullyTyped = true;
            lineFinishedTime = now;
        } else {
            if (now - lastCharTime >= TYPEWRITER_CHAR_DELAY) {
                displayedCharCount++;
                lastCharTime = now;
                if (displayedCharCount >= len) {
                    displayedCharCount = len;
                    lineFullyTyped = true;
                    lineFinishedTime = now;
                }
            }
        }
    } else {
        // Line is complete: wait for key press or auto-advance timeout
        if (advanceKey || (now - lineFinishedTime >= TYPEWRITER_LINE_WAIT)) {
            currentMessage++;
            displayedCharCount = 0;
            lineFullyTyped = false;
            lastCharTime = now;

            if (currentMessage >= messageCount) {
                messageCount = 0;
                currentMessage = 0;
                return 0; // Completed all queued messages
            }
        }
    }

    return 1; // Still active
}

void DrawWrappedText(
    const char *text,
    Rectangle area,
    int fontSize,
    int lineSpacing,
    Color color
) {
    if (!text || text[0] == '\0') return;

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

                if (MeasureCustomText(testLine, (float)fontSize) <= area.width) {
                    strncpy(line, testLine, sizeof(line) - 1);
                    line[sizeof(line) - 1] = '\0';
                }
                else {
                    DrawCustomText(line, (float)area.x, (float)y, (float)fontSize, color);
                    y += fontSize + lineSpacing;
                    strncpy(line, word, sizeof(line) - 1);
                    line[sizeof(line) - 1] = '\0';
                }
            }

            wordLength = 0;

            if (c == '\n') {
                DrawCustomText(line, (float)area.x, (float)y, (float)fontSize, color);
                line[0] = '\0';
                y += fontSize + lineSpacing;
            }

            if (c == '\0')
                break;
        }
    }

    if (line[0] != '\0') {
        DrawCustomText(line, (float)area.x, (float)y, (float)fontSize, color);
    }
}

void DrawBattleTextBox(void)
{
    if (currentMessage >= messageCount)
        return;

    // Fixed HD 1280x720 positioning for battle dialogue box
    Rectangle bar = {
        50,
        510,
        680,
        180
    };

    DrawRectangleRounded(bar, 0.05f, 6, (Color){250, 252, 255, 255});
    DrawRectangleRoundedLines(bar, 0.05f, 6, (Color){35, 45, 60, 255});

    Rectangle textArea = {
        bar.x + 22,
        bar.y + 20,
        bar.width - 44,
        bar.height - 40
    };

    char visibleText[MESSAGE_SIZE];
    int n = displayedCharCount;
    if (n > (int)sizeof(visibleText) - 1) n = (int)sizeof(visibleText) - 1;
    strncpy(visibleText, messageBuffer[currentMessage], n);
    visibleText[n] = '\0';

    DrawWrappedText(
        visibleText,
        textArea,
        22,
        6,
        (Color){25, 28, 32, 255}
    );

    // Prompt arrow when current line has finished typing
    if (lineFullyTyped) {
        if ((int)(GetTime() * 3.5) % 2 == 0) {
            DrawCustomText("v", bar.x + bar.width - 30, bar.y + bar.height - 32, 20, (Color){220, 50, 50, 255});
        }
    }
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

bool PrepareTurnAction(Pokemon *attacker, Pokemon *defender, move *Move, TurnAction *action)
{
    if (!attacker || !defender || !Move || !action)
        return false;

    action->attacker = attacker;
    action->defender = defender;
    action->usedMove = Move;
    action->damage = 0;
    action->effectiveness = 1.0;
    action->missed = false;
    action->isStatusOnly = false;

    if (Move->Current_PP <= 0) {
        return false;
    }

    Move->Current_PP--;

    if (defender->evasiveness <= 0) defender->evasiveness = 1;
    int accuracy = Move->Accuracy * attacker->accuracy / defender->evasiveness;
    if (accuracy > 100) accuracy = 100;

    if (rand() % 100 >= accuracy) {
        action->missed = true;
        return true;
    }

    double random_factor = ((rand() % 39) + 217) / 255.0;

    if (Move->category == CATEGORY_PHYSICAL) {
        action->damage = damage_calculator(attacker, defender, Move, random_factor);
        move_status_effect_processing(attacker, defender, Move);
    }
    else if (Move->category == CATEGORY_SPECIAL) {
        action->damage = special_damage_calculator(attacker, defender, Move, random_factor);
        move_status_effect_processing(attacker, defender, Move);
    }
    else {
        action->isStatusOnly = true;
        move_status_effect_processing(attacker, defender, Move);
        return true;
    }

    action->effectiveness =
        type_effectiveness(Move->type, defender->type1) *
        type_effectiveness(Move->type, defender->type2);

    return true;
}
