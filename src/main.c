#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "raylib.h"
#include "types.h"
#include "type_chart.h"
#include "moves.h"
#include "pokemon_database.h"
#include "party.h"
#include "battle.h"
#include "ui.h"

#define VIRTUAL_WIDTH  1280
#define VIRTUAL_HEIGHT 720

typedef enum {
    BATTLE_MENU_NONE = 0,
    BATTLE_MENU_MAIN,  // Options: [1. FIGHT] or [2. POKEMON / SWITCH]
    BATTLE_MENU_MOVES  // 4 Moves + [ESC: Back]
} BattleMenuState;

typedef enum {
    ACTION_STAGE_IDLE = 0,
    ACTION_STAGE_FIRST_ATTACK_MSG,
    ACTION_STAGE_FIRST_HIT_BLINK,
    ACTION_STAGE_FIRST_DRAIN,
    ACTION_STAGE_FIRST_RESULT_MSG,
    ACTION_STAGE_FIRST_FAINT_CHECK,
    ACTION_STAGE_SECOND_ATTACK_MSG,
    ACTION_STAGE_SECOND_HIT_BLINK,
    ACTION_STAGE_SECOND_DRAIN,
    ACTION_STAGE_SECOND_RESULT_MSG,
    ACTION_STAGE_SECOND_FAINT_CHECK,
    ACTION_STAGE_STATUS_DRAIN,
    ACTION_STAGE_ENEMY_SWITCH_MSG,
    ACTION_STAGE_ROUND_FINISH
} BattleActionStage;

// Helper to load sprite with fallback
static Texture2D LoadPokemonSprite(const char *name, bool isBack)
{
    char lowerName[64];
    int i = 0;
    while (name[i] && i < 63) {
        lowerName[i] = (char)tolower((unsigned char)name[i]);
        i++;
    }
    lowerName[i] = '\0';

    char filename[128];
    snprintf(filename, sizeof(filename), "images/%s_%s.png", lowerName, isBack ? "back" : "front");
    if (FileExists(filename)) {
        return LoadTexture(filename);
    }

    char cleanName[64];
    int k = 0;
    for (int j = 0; lowerName[j] && k < 63; j++) {
        if (isalnum((unsigned char)lowerName[j])) {
            cleanName[k++] = lowerName[j];
        } else {
            cleanName[k++] = '_';
        }
    }
    cleanName[k] = '\0';

    snprintf(filename, sizeof(filename), "images/%s_%s.png", cleanName, isBack ? "back" : "front");
    if (FileExists(filename)) {
        return LoadTexture(filename);
    }

    const char *defaultSprite = isBack ? "images/venusaur_back.png" : "images/charizard_front.png";
    if (FileExists(defaultSprite)) {
        return LoadTexture(defaultSprite);
    }

    Image img = GenImageColor(64, 64, isBack ? (Color){60, 120, 200, 255} : (Color){220, 70, 70, 255});
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

static void DrawPartyBalls(int x, int y, const Party *party)
{
    for (int i = 0; i < party->count; i++) {
        int cx = x + i * 20;
        bool isAlive = (party->members[i].current_health > 0);
        bool isActive = (i == party->activeIndex);

        Color ballColor = isAlive ? (Color){46, 204, 113, 255} : (Color){231, 76, 60, 255};
        DrawCircle(cx, y, 6, ballColor);
        DrawCircleLines(cx, y, 6, BLACK);
        if (isActive) {
            DrawCircle(cx, y, 2, WHITE);
        }
    }
}

int main(void)
{
    srand((unsigned int)time(NULL));

    // Enable Resizable and VSync for smooth Fullscreen / Minimized behavior
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "Pokemon Phoenix Red");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    // Initialize UI fonts and sprite cache
    InitUIAssets();

    // Virtual Render Texture for crisp 1280x720 scaling in any resolution/fullscreen
    RenderTexture2D screenTarget = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(screenTarget.texture, TEXTURE_FILTER_BILINEAR);

    GameState gameState = STATE_TEAM_SELECT;
    BattleMenuState battleMenu = BATTLE_MENU_MAIN;
    BattleActionStage actionStage = ACTION_STAGE_IDLE;

    Party playerParty;
    InitParty(&playerParty);

    Party enemyParty;
    InitParty(&enemyParty);

    BattleContext contxt;
    memset(&contxt, 0, sizeof(BattleContext));

    TurnAction currentAction;
    memset(&currentAction, 0, sizeof(TurnAction));

    Pokemon *firstPoke = NULL;
    Pokemon *secondPoke = NULL;
    move *firstMove = NULL;
    move *secondMove = NULL;

    int rosterScrollIndex = 0;

    Texture2D aSprite = { 0 };
    Texture2D bSprite = { 0 };
    bool spritesLoaded = false;

    int selectedMove = 0;
    int displayedPlayerHP = 0;
    int displayedEnemyHP = 0;

    double lastHpUpdate = 0.0;
    double hitBlinkStartTime = 0.0;

    bool showSwitchMenu = false;
    bool isForcedSwitch = false;

    while (!WindowShouldClose()) {

        // Toggle Fullscreen on F11 key
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Calculate aspect-ratio scaling to fit current window size
        float scale = fminf((float)GetScreenWidth() / (float)VIRTUAL_WIDTH,
                            (float)GetScreenHeight() / (float)VIRTUAL_HEIGHT);
        Rectangle destRec = {
            ((float)GetScreenWidth() - ((float)VIRTUAL_WIDTH * scale)) * 0.5f,
            ((float)GetScreenHeight() - ((float)VIRTUAL_HEIGHT * scale)) * 0.5f,
            (float)VIRTUAL_WIDTH * scale,
            (float)VIRTUAL_HEIGHT * scale
        };

        // Virtual Mouse mapped into the 1280x720 canvas
        Vector2 rawMouse = GetMousePosition();
        Vector2 mouse = {
            (rawMouse.x - destRec.x) / scale,
            (rawMouse.y - destRec.y) / scale
        };
        bool mouseClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // ==========================================
        // 1. TEAM SELECTION SCREEN
        // ==========================================
        if (gameState == STATE_TEAM_SELECT) {
            BeginTextureMode(screenTarget);
            bool startBattle = DrawTeamSelectionScreen(&playerParty, &rosterScrollIndex, mouse);

            // Fullscreen toggle button in header
            Rectangle fsBtn = { 1110, 13, 140, 30 };
            bool fsHover = CheckCollisionPointRec(mouse, fsBtn);
            DrawRectangleRec(fsBtn, fsHover ? (Color){52, 152, 219, 255} : (Color){31, 108, 165, 255});
            DrawCustomText(IsWindowFullscreen() ? "Windowed" : "Fullscreen", (float)fsBtn.x + 28, (float)fsBtn.y + 7, 14, WHITE);
            if (mouseClicked && fsHover) {
                ToggleFullscreen();
            }

            EndTextureMode();

            if (startBattle) {
                // Opponent always gets full team of 6
                GenerateRandomParty(&enemyParty, 6, 100);

                contxt.playerParty = playerParty;
                contxt.enemyParty = enemyParty;
                contxt.player_a_active_move = NULL;
                contxt.player_b_active_move = NULL;

                Pokemon *pA = GetActivePokemon(&contxt.playerParty);
                Pokemon *pB = GetActivePokemon(&contxt.enemyParty);

                if (spritesLoaded) {
                    UnloadTexture(aSprite);
                    UnloadTexture(bSprite);
                }
                aSprite = LoadPokemonSprite(pA->Name, true);
                bSprite = LoadPokemonSprite(pB->Name, false);
                spritesLoaded = true;

                displayedPlayerHP = pA->current_health;
                displayedEnemyHP = pB->current_health;

                ClearBattleMessages();

                battleMenu = BATTLE_MENU_MAIN;
                actionStage = ACTION_STAGE_IDLE;
                showSwitchMenu = false;
                isForcedSwitch = false;
                gameState = STATE_BATTLE;
            }
        }
        // ==========================================
        // 2. BATTLE STATE
        // ==========================================
        else {
            Pokemon *pA = GetActivePokemon(&contxt.playerParty);
            Pokemon *pB = GetActivePokemon(&contxt.enemyParty);

            // Advance typewriter animation
            int messageActive = UpdateBattleMessages();

            // Reset hover states for the frame
            int hoveredMoveIndex = -1;
            const Pokemon *hoveredPokemon = NULL;
            bool isHoveredEnemy = false;

            // 1. Detect Move inspection hover (when in move selection menu)
            if (battleMenu == BATTLE_MENU_MOVES && actionStage == ACTION_STAGE_IDLE && !showSwitchMenu && !messageActive && pA->current_health > 0) {
                Vector2 movePositions[4] = {
                    {760, 510},
                    {1010, 510},
                    {760, 592},
                    {1010, 592}
                };
                for (int m = 0; m < 4; m++) {
                    Rectangle btnRec = { movePositions[m].x, movePositions[m].y, 230, 72 };
                    if (CheckCollisionPointRec(mouse, btnRec)) {
                        hoveredMoveIndex = m;
                        break;
                    }
                }
            }

            // 2. Detect Pokemon inspection hover (only if no move is being hovered)
            if (hoveredMoveIndex == -1 && !showSwitchMenu && actionStage == ACTION_STAGE_IDLE) {
                Rectangle enemySpriteRec = { 860, 80, 220, 220 };
                Rectangle enemyHudRec = { 80, 50, 450, 115 };
                if (CheckCollisionPointRec(mouse, enemySpriteRec) || CheckCollisionPointRec(mouse, enemyHudRec)) {
                    hoveredPokemon = pB;
                    isHoveredEnemy = true;
                }

                Rectangle playerSpriteRec = { 180, 260, 250, 250 };
                Rectangle playerHudRec = { 760, 340, 470, 125 };
                if (CheckCollisionPointRec(mouse, playerSpriteRec) || CheckCollisionPointRec(mouse, playerHudRec)) {
                    hoveredPokemon = pA;
                    isHoveredEnemy = false;
                }
            }

            // Slower, dramatic HP Drain Animation Speed
            if (GetTime() - lastHpUpdate >= 0.025) {
                if (displayedPlayerHP > pA->current_health) {
                    int diff = displayedPlayerHP - pA->current_health;
                    int step = (diff > 120) ? 3 : (diff > 50 ? 2 : 1);
                    displayedPlayerHP -= step;
                    if (displayedPlayerHP < pA->current_health) displayedPlayerHP = pA->current_health;
                } else if (displayedPlayerHP < pA->current_health) {
                    displayedPlayerHP = pA->current_health;
                }

                if (displayedEnemyHP > pB->current_health) {
                    int diff = displayedEnemyHP - pB->current_health;
                    int step = (diff > 120) ? 3 : (diff > 50 ? 2 : 1);
                    displayedEnemyHP -= step;
                    if (displayedEnemyHP < pB->current_health) displayedEnemyHP = pB->current_health;
                } else if (displayedEnemyHP < pB->current_health) {
                    displayedEnemyHP = pB->current_health;
                }
                lastHpUpdate = GetTime();
            }

            // ==========================================
            // Turn Execution State Machine
            // ==========================================
            if (!messageActive) {
                switch (actionStage) {
                    case ACTION_STAGE_IDLE:
                        // Check match end conditions
                        if (IsPartyWiped(&contxt.playerParty)) {
                            AddBattleMessage("All your Pokemon fainted! You blacked out!\n");
                            gameState = STATE_GAME_OVER;
                        } else if (IsPartyWiped(&contxt.enemyParty)) {
                            AddBattleMessage("All enemy Pokemon fainted! You won!\n");
                            gameState = STATE_GAME_OVER;
                        }
                        break;

                    case ACTION_STAGE_FIRST_ATTACK_MSG:
                        if (currentAction.missed || currentAction.isStatusOnly || currentAction.damage <= 0) {
                            actionStage = ACTION_STAGE_FIRST_FAINT_CHECK;
                        } else {
                            actionStage = ACTION_STAGE_FIRST_HIT_BLINK;
                            hitBlinkStartTime = GetTime();
                        }
                        break;

                    case ACTION_STAGE_FIRST_HIT_BLINK:
                        if (GetTime() - hitBlinkStartTime >= 0.35) {
                            currentAction.defender->current_health -= currentAction.damage;
                            if (currentAction.defender->current_health < 0) currentAction.defender->current_health = 0;
                            actionStage = ACTION_STAGE_FIRST_DRAIN;
                        }
                        break;

                    case ACTION_STAGE_FIRST_DRAIN: {
                        int *dispHP = (currentAction.defender == pA) ? &displayedPlayerHP : &displayedEnemyHP;
                        if (*dispHP == currentAction.defender->current_health) {
                            if (currentAction.effectiveness > 1.0) {
                                AddBattleMessage("It was super effective!\n");
                            } else if (currentAction.effectiveness == 0.0) {
                                AddBattleMessage("It had no effect!\n");
                            } else if (currentAction.effectiveness < 1.0) {
                                AddBattleMessage("It was not very effective!\n");
                            }
                            actionStage = ACTION_STAGE_FIRST_RESULT_MSG;
                        }
                        break;
                    }

                    case ACTION_STAGE_FIRST_RESULT_MSG:
                        actionStage = ACTION_STAGE_FIRST_FAINT_CHECK;
                        break;

                    case ACTION_STAGE_FIRST_FAINT_CHECK:
                        if (currentAction.defender->current_health <= 0) {
                            AddBattleMessage("%s fainted!\n", currentAction.defender->Name);
                            if (currentAction.defender == pA) {
                                if (IsPartyWiped(&contxt.playerParty)) {
                                    AddBattleMessage("All your Pokemon fainted! You blacked out!\n");
                                    gameState = STATE_GAME_OVER;
                                    actionStage = ACTION_STAGE_IDLE;
                                } else {
                                    showSwitchMenu = true;
                                    isForcedSwitch = true;
                                    actionStage = ACTION_STAGE_IDLE;
                                }
                            } else {
                                if (IsPartyWiped(&contxt.enemyParty)) {
                                    AddBattleMessage("All enemy Pokemon fainted! You won!\n");
                                    gameState = STATE_GAME_OVER;
                                    actionStage = ACTION_STAGE_IDLE;
                                } else {
                                    int nextEnemy = ChooseBestOpponentSwitch(&contxt.enemyParty, pA);
                                    SwitchPokemon(&contxt.enemyParty, nextEnemy);
                                    pB = GetActivePokemon(&contxt.enemyParty);
                                    UnloadTexture(bSprite);
                                    bSprite = LoadPokemonSprite(pB->Name, false);
                                    displayedEnemyHP = pB->current_health;
                                    AddBattleMessage("Enemy sent out %s!\n", pB->Name);
                                    actionStage = ACTION_STAGE_ENEMY_SWITCH_MSG;
                                }
                            }
                        } else {
                            // Defender survived: Second attacker executes their turn!
                            if (secondPoke->effect == STATUS_SLEEP ||
                                secondPoke->effect == STATUS_FREEZE ||
                                (secondPoke->effect == STATUS_PARALYSIS && rand() % 100 < 25))
                            {
                                if (secondPoke->effect == STATUS_SLEEP) AddBattleMessage("%s is fast asleep!\n", secondPoke->Name);
                                else if (secondPoke->effect == STATUS_FREEZE) AddBattleMessage("%s is frozen solid!\n", secondPoke->Name);
                                else AddBattleMessage("%s is fully paralyzed and cannot move!\n", secondPoke->Name);

                                currentAction.damage = 0;
                                currentAction.missed = false;
                                currentAction.isStatusOnly = false;
                                currentAction.attacker = secondPoke;
                                currentAction.defender = firstPoke;
                                actionStage = ACTION_STAGE_SECOND_RESULT_MSG;
                            } else {
                                PrepareTurnAction(secondPoke, firstPoke, secondMove, &currentAction);
                                AddBattleMessage("%s used %s!\n", secondPoke->Name, secondMove->Name);
                                if (currentAction.missed) {
                                    AddBattleMessage("%s's attack missed!\n", secondPoke->Name);
                                }
                                actionStage = ACTION_STAGE_SECOND_ATTACK_MSG;
                            }
                        }
                        break;

                    case ACTION_STAGE_SECOND_ATTACK_MSG:
                        if (currentAction.missed || currentAction.isStatusOnly || currentAction.damage <= 0) {
                            actionStage = ACTION_STAGE_SECOND_FAINT_CHECK;
                        } else {
                            actionStage = ACTION_STAGE_SECOND_HIT_BLINK;
                            hitBlinkStartTime = GetTime();
                        }
                        break;

                    case ACTION_STAGE_SECOND_HIT_BLINK:
                        if (GetTime() - hitBlinkStartTime >= 0.35) {
                            currentAction.defender->current_health -= currentAction.damage;
                            if (currentAction.defender->current_health < 0) currentAction.defender->current_health = 0;
                            actionStage = ACTION_STAGE_SECOND_DRAIN;
                        }
                        break;

                    case ACTION_STAGE_SECOND_DRAIN: {
                        int *dispHP = (currentAction.defender == pA) ? &displayedPlayerHP : &displayedEnemyHP;
                        if (*dispHP == currentAction.defender->current_health) {
                            if (currentAction.effectiveness > 1.0) {
                                AddBattleMessage("It was super effective!\n");
                            } else if (currentAction.effectiveness == 0.0) {
                                AddBattleMessage("It had no effect!\n");
                            } else if (currentAction.effectiveness < 1.0) {
                                AddBattleMessage("It was not very effective!\n");
                            }
                            actionStage = ACTION_STAGE_SECOND_RESULT_MSG;
                        }
                        break;
                    }

                    case ACTION_STAGE_SECOND_RESULT_MSG:
                        actionStage = ACTION_STAGE_SECOND_FAINT_CHECK;
                        break;

                    case ACTION_STAGE_SECOND_FAINT_CHECK:
                        if (currentAction.defender->current_health <= 0) {
                            AddBattleMessage("%s fainted!\n", currentAction.defender->Name);
                            if (currentAction.defender == pA) {
                                if (IsPartyWiped(&contxt.playerParty)) {
                                    AddBattleMessage("All your Pokemon fainted! You blacked out!\n");
                                    gameState = STATE_GAME_OVER;
                                    actionStage = ACTION_STAGE_IDLE;
                                } else {
                                    showSwitchMenu = true;
                                    isForcedSwitch = true;
                                    actionStage = ACTION_STAGE_IDLE;
                                }
                            } else {
                                if (IsPartyWiped(&contxt.enemyParty)) {
                                    AddBattleMessage("All enemy Pokemon fainted! You won!\n");
                                    gameState = STATE_GAME_OVER;
                                    actionStage = ACTION_STAGE_IDLE;
                                } else {
                                    int nextEnemy = ChooseBestOpponentSwitch(&contxt.enemyParty, pA);
                                    SwitchPokemon(&contxt.enemyParty, nextEnemy);
                                    pB = GetActivePokemon(&contxt.enemyParty);
                                    UnloadTexture(bSprite);
                                    bSprite = LoadPokemonSprite(pB->Name, false);
                                    displayedEnemyHP = pB->current_health;
                                    AddBattleMessage("Enemy sent out %s!\n", pB->Name);
                                    actionStage = ACTION_STAGE_ENEMY_SWITCH_MSG;
                                }
                            }
                        } else {
                            // Apply end-of-turn status effects (Burn / Poison)
                            status_effect_processing(pA, pB);
                            status_effect_processing(pB, pA);
                            actionStage = ACTION_STAGE_STATUS_DRAIN;
                        }
                        break;

                    case ACTION_STAGE_STATUS_DRAIN:
                        if (displayedPlayerHP == pA->current_health && displayedEnemyHP == pB->current_health) {
                            if (pA->current_health <= 0) {
                                AddBattleMessage("%s fainted from status damage!\n", pA->Name);
                                if (IsPartyWiped(&contxt.playerParty)) {
                                    gameState = STATE_GAME_OVER;
                                } else {
                                    showSwitchMenu = true;
                                    isForcedSwitch = true;
                                }
                                actionStage = ACTION_STAGE_IDLE;
                            } else if (pB->current_health <= 0) {
                                AddBattleMessage("Enemy %s fainted from status damage!\n", pB->Name);
                                if (IsPartyWiped(&contxt.enemyParty)) {
                                    gameState = STATE_GAME_OVER;
                                    actionStage = ACTION_STAGE_IDLE;
                                } else {
                                    int nextEnemy = ChooseBestOpponentSwitch(&contxt.enemyParty, pA);
                                    SwitchPokemon(&contxt.enemyParty, nextEnemy);
                                    pB = GetActivePokemon(&contxt.enemyParty);
                                    UnloadTexture(bSprite);
                                    bSprite = LoadPokemonSprite(pB->Name, false);
                                    displayedEnemyHP = pB->current_health;
                                    AddBattleMessage("Enemy sent out %s!\n", pB->Name);
                                    actionStage = ACTION_STAGE_ENEMY_SWITCH_MSG;
                                }
                            } else {
                                actionStage = ACTION_STAGE_ROUND_FINISH;
                            }
                        }
                        break;

                    case ACTION_STAGE_ENEMY_SWITCH_MSG:
                        actionStage = ACTION_STAGE_ROUND_FINISH;
                        break;

                    case ACTION_STAGE_ROUND_FINISH:
                        actionStage = ACTION_STAGE_IDLE;
                        if (pA->current_health > 0 && pB->current_health > 0 && gameState == STATE_BATTLE) {
                            battleMenu = BATTLE_MENU_MAIN; // Fight menu reappears here AFTER all HP is drained!
                        }
                        break;
                }
            }

            // Keyboard Shortcuts for Menus (Only when idle)
            if (actionStage == ACTION_STAGE_IDLE && !showSwitchMenu && !messageActive && pA->current_health > 0 && pB->current_health > 0 && gameState == STATE_BATTLE) {
                if (battleMenu == BATTLE_MENU_MAIN) {
                    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_F)) {
                        battleMenu = BATTLE_MENU_MOVES;
                    }
                    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_P)) {
                        showSwitchMenu = true;
                        isForcedSwitch = false;
                    }
                }
                else if (battleMenu == BATTLE_MENU_MOVES) {
                    int chosenMove = -1;
                    if (IsKeyPressed(KEY_ONE))   chosenMove = 0;
                    if (IsKeyPressed(KEY_TWO))   chosenMove = 1;
                    if (IsKeyPressed(KEY_THREE)) chosenMove = 2;
                    if (IsKeyPressed(KEY_FOUR))  chosenMove = 3;

                    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_B)) {
                        battleMenu = BATTLE_MENU_MAIN;
                    }

                    if (chosenMove != -1 && pA->Moves[chosenMove].Current_PP > 0) {
                        selectedMove = chosenMove;
                        contxt.player_a_active_move = &pA->Moves[selectedMove];

                        // AI selects move
                        int bestEnemyMove = 0;
                        int bestWeight = get_move_weight(pB->Moves[0], *pA);
                        for (int m = 1; m < 4; m++) {
                            int weight = get_move_weight(pB->Moves[m], *pA);
                            if (weight > bestWeight) {
                                bestWeight = weight;
                                bestEnemyMove = m;
                            }
                        }
                        contxt.player_b_active_move = &pB->Moves[bestEnemyMove];

                        // Speed check
                        if (pA->speed >= pB->speed) {
                            firstPoke = pA;
                            secondPoke = pB;
                            firstMove = contxt.player_a_active_move;
                            secondMove = contxt.player_b_active_move;
                        } else {
                            firstPoke = pB;
                            secondPoke = pA;
                            firstMove = contxt.player_b_active_move;
                            secondMove = contxt.player_a_active_move;
                        }

                        // Hide fight menu immediately!
                        battleMenu = BATTLE_MENU_NONE;

                        // Start Action 1
                        if (firstPoke->effect == STATUS_SLEEP ||
                            firstPoke->effect == STATUS_FREEZE ||
                            (firstPoke->effect == STATUS_PARALYSIS && rand() % 100 < 25))
                        {
                            if (firstPoke->effect == STATUS_SLEEP) AddBattleMessage("%s is fast asleep!\n", firstPoke->Name);
                            else if (firstPoke->effect == STATUS_FREEZE) AddBattleMessage("%s is frozen solid!\n", firstPoke->Name);
                            else AddBattleMessage("%s is fully paralyzed and cannot move!\n", firstPoke->Name);

                            currentAction.damage = 0;
                            currentAction.missed = false;
                            currentAction.isStatusOnly = false;
                            currentAction.attacker = firstPoke;
                            currentAction.defender = secondPoke;
                            actionStage = ACTION_STAGE_FIRST_RESULT_MSG;
                        } else {
                            PrepareTurnAction(firstPoke, secondPoke, firstMove, &currentAction);
                            AddBattleMessage("%s used %s!\n", firstPoke->Name, firstMove->Name);
                            if (currentAction.missed) {
                                AddBattleMessage("%s's attack missed!\n", firstPoke->Name);
                            }
                            actionStage = ACTION_STAGE_FIRST_ATTACK_MSG;
                        }
                    }
                }
            }

            // ==========================================
            // Render Battle into Virtual 1280x720 Texture
            // ==========================================
            BeginTextureMode(screenTarget);
            ClearBackground((Color){245, 248, 252, 255});

            // Bottom dialogue text box area (fixed at { 50, 510, 680, 180 })
            Rectangle textBoxBar = { 50, 510, 680, 180 };
            if (IsBattleMessageActive()) {
                DrawBattleTextBox();
            } else if (hoveredMoveIndex >= 0 && hoveredMoveIndex < 4) {
                DrawMoveDetailsInTextBox(&pA->Moves[hoveredMoveIndex], pB);
            } else if (hoveredPokemon != NULL) {
                DrawPokemonStatsInTextBox(hoveredPokemon, isHoveredEnemy);
            } else {
                // Kept clear and clean when cursor is not placed on anything
                DrawRectangleRounded(textBoxBar, 0.05f, 6, (Color){250, 252, 255, 255});
                DrawRectangleRoundedLines(textBoxBar, 0.05f, 6, (Color){200, 208, 218, 255});
            }

            // Sprites with Hit-Blink Animation
            Color aTint = WHITE;
            Color bTint = WHITE;

            if (actionStage == ACTION_STAGE_FIRST_HIT_BLINK || actionStage == ACTION_STAGE_SECOND_HIT_BLINK) {
                double elapsed = GetTime() - hitBlinkStartTime;
                bool isFlickerOff = ((int)(elapsed / 0.05) % 2 == 1);
                if (currentAction.defender == pA) {
                    aTint = isFlickerOff ? (Color){255, 80, 80, 80} : (Color){255, 120, 120, 255};
                } else if (currentAction.defender == pB) {
                    bTint = isFlickerOff ? (Color){255, 80, 80, 80} : (Color){255, 120, 120, 255};
                }
            }

            DrawTextureEx(aSprite, (Vector2){180, 260}, 0.0f, 3.8f, aTint);
            DrawTextureEx(bSprite, (Vector2){860, 80}, 0.0f, 3.2f, bTint);

            // --- Enemy HUD (Top-Left) ---
            Rectangle enemyHud = { 80, 50, 450, 115 };
            DrawRectangleRounded(enemyHud, 0.12f, 6, (Color){255, 255, 255, 240});
            DrawRectangleRoundedLines(enemyHud, 0.12f, 6, (Color){200, 208, 218, 255});

            int bNameW = MeasureCustomText(TextFormat("%s  Lv.%d", pB->Name, pB->level), 24);
            DrawCustomText(TextFormat("%s  Lv.%d", pB->Name, pB->level), 95, 62, 24, (Color){20, 24, 28, 255});
            int bNextX = 95 + bNameW + 12;
            DrawTypeBadge(pB->type1, bNextX, 64, 58, 22, 11);
            bNextX += 64;
            if (pB->type2 != TYPE_NONE) {
                DrawTypeBadge(pB->type2, bNextX, 64, 58, 22, 11);
                bNextX += 64;
            }
            if (pB->effect != STATUS_NONE) {
                DrawStatusBadge(pB->effect, bNextX, 64, 50, 22, 11);
            }
            DrawHPBar(95, 100, 290, 20, displayedEnemyHP, pB->max_health, true);
            DrawPartyBalls(95, 134, &contxt.enemyParty);

            // --- Player HUD (Bottom-Right) ---
            Rectangle playerHud = { 760, 340, 470, 125 };
            DrawRectangleRounded(playerHud, 0.12f, 6, (Color){255, 255, 255, 240});
            DrawRectangleRoundedLines(playerHud, 0.12f, 6, (Color){200, 208, 218, 255});

            int aNameW = MeasureCustomText(TextFormat("%s  Lv.%d", pA->Name, pA->level), 24);
            DrawCustomText(TextFormat("%s  Lv.%d", pA->Name, pA->level), 780, 352, 24, (Color){20, 24, 28, 255});
            int aNextX = 780 + aNameW + 12;
            DrawTypeBadge(pA->type1, aNextX, 354, 58, 22, 11);
            aNextX += 64;
            if (pA->type2 != TYPE_NONE) {
                DrawTypeBadge(pA->type2, aNextX, 354, 58, 22, 11);
                aNextX += 64;
            }
            if (pA->effect != STATUS_NONE) {
                DrawStatusBadge(pA->effect, aNextX, 354, 50, 22, 11);
            }
            DrawHPBar(780, 392, 310, 22, displayedPlayerHP, pA->max_health, true);
            DrawPartyBalls(780, 430, &contxt.playerParty);

            // --- Menus (ONLY shown when actionStage == ACTION_STAGE_IDLE) ---
            if (battleMenu == BATTLE_MENU_MAIN && actionStage == ACTION_STAGE_IDLE && !showSwitchMenu && !messageActive && pA->current_health > 0) {
                // Fight Button
                Rectangle fightBtn = { 760, 510, 230, 180 };
                bool fightHover = CheckCollisionPointRec(mouse, fightBtn);
                DrawRectangleRounded(fightBtn, 0.08f, 6, fightHover ? (Color){235, 70, 55, 255} : (Color){220, 53, 40, 255});
                DrawRectangleRoundedLines(fightBtn, 0.08f, 6, (Color){160, 30, 20, 255});
                DrawCustomText("FIGHT", (float)fightBtn.x + 70, (float)fightBtn.y + 60, 30, WHITE);
                DrawCustomText("[1] Select Attack", (float)fightBtn.x + 55, (float)fightBtn.y + 110, 15, (Color){255, 230, 230, 255});

                if (mouseClicked && fightHover) {
                    battleMenu = BATTLE_MENU_MOVES;
                }

                // Pokemon / Switch Button
                Rectangle switchBtn = { 1010, 510, 230, 180 };
                bool switchHover = CheckCollisionPointRec(mouse, switchBtn);
                DrawRectangleRounded(switchBtn, 0.08f, 6, switchHover ? (Color){46, 204, 113, 255} : (Color){39, 174, 96, 255});
                DrawRectangleRoundedLines(switchBtn, 0.08f, 6, (Color){25, 120, 65, 255});
                DrawCustomText("POKEMON", (float)switchBtn.x + 46, (float)switchBtn.y + 60, 30, WHITE);
                DrawCustomText("[2] Switch Party", (float)switchBtn.x + 58, (float)switchBtn.y + 110, 15, (Color){230, 255, 230, 255});

                if (mouseClicked && switchHover) {
                    showSwitchMenu = true;
                    isForcedSwitch = false;
                }
            } else if (battleMenu == BATTLE_MENU_MOVES && actionStage == ACTION_STAGE_IDLE && !showSwitchMenu && !messageActive && pA->current_health > 0) {
                Vector2 positions[4] = {
                    {760, 510},
                    {1010, 510},
                    {760, 592},
                    {1010, 592}
                };

                for (int i = 0; i < 4; i++) {
                    int x = (int)positions[i].x;
                    int y = (int)positions[i].y;
                    Rectangle btnRec = { (float)x, (float)y, 230, 72 };
                    bool isHovered = (hoveredMoveIndex == i);

                    Color bg = (selectedMove == i) ? (Color){220, 238, 252, 255} : (isHovered ? (Color){245, 248, 252, 255} : WHITE);
                    DrawRectangleRounded(btnRec, 0.12f, 4, bg);
                    DrawRectangleRoundedLines(btnRec, 0.12f, 4, (selectedMove == i) ? (Color){41, 128, 185, 255} : (Color){200, 205, 215, 255});

                    DrawCustomText(TextFormat("%d. %s", i + 1, pA->Moves[i].Name), (float)x + 12, (float)y + 12, 18, BLACK);
                    DrawCustomText(TextFormat("PP: %d/%d", pA->Moves[i].Current_PP, pA->Moves[i].Max_PP), (float)x + 12, (float)y + 38, 14, DARKGRAY);

                    DrawTypeBadge(pA->Moves[i].type, x + 152, y + 22, 64, 26, 11);

                    if (mouseClicked && isHovered && pA->Moves[i].Current_PP > 0) {
                        selectedMove = i;
                        contxt.player_a_active_move = &pA->Moves[selectedMove];

                        int bestEnemyMove = 0;
                        int bestWeight = get_move_weight(pB->Moves[0], *pA);
                        for (int m = 1; m < 4; m++) {
                            int weight = get_move_weight(pB->Moves[m], *pA);
                            if (weight > bestWeight) {
                                bestWeight = weight;
                                bestEnemyMove = m;
                            }
                        }
                        contxt.player_b_active_move = &pB->Moves[bestEnemyMove];

                        if (pA->speed >= pB->speed) {
                            firstPoke = pA;
                            secondPoke = pB;
                            firstMove = contxt.player_a_active_move;
                            secondMove = contxt.player_b_active_move;
                        } else {
                            firstPoke = pB;
                            secondPoke = pA;
                            firstMove = contxt.player_b_active_move;
                            secondMove = contxt.player_a_active_move;
                        }

                        battleMenu = BATTLE_MENU_NONE;

                        if (firstPoke->effect == STATUS_SLEEP ||
                            firstPoke->effect == STATUS_FREEZE ||
                            (firstPoke->effect == STATUS_PARALYSIS && rand() % 100 < 25))
                        {
                            if (firstPoke->effect == STATUS_SLEEP) AddBattleMessage("%s is fast asleep!\n", firstPoke->Name);
                            else if (firstPoke->effect == STATUS_FREEZE) AddBattleMessage("%s is frozen solid!\n", firstPoke->Name);
                            else AddBattleMessage("%s is fully paralyzed and cannot move!\n", firstPoke->Name);

                            currentAction.damage = 0;
                            currentAction.missed = false;
                            currentAction.isStatusOnly = false;
                            currentAction.attacker = firstPoke;
                            currentAction.defender = secondPoke;
                            actionStage = ACTION_STAGE_FIRST_RESULT_MSG;
                        } else {
                            PrepareTurnAction(firstPoke, secondPoke, firstMove, &currentAction);
                            AddBattleMessage("%s used %s!\n", firstPoke->Name, firstMove->Name);
                            if (currentAction.missed) {
                                AddBattleMessage("%s's attack missed!\n", firstPoke->Name);
                            }
                            actionStage = ACTION_STAGE_FIRST_ATTACK_MSG;
                        }
                    }
                }

                Rectangle backBtn = { 760, 674, 480, 32 };
                bool backHover = CheckCollisionPointRec(mouse, backBtn);
                DrawRectangleRounded(backBtn, 0.15f, 4, backHover ? (Color){200, 205, 215, 255} : (Color){225, 230, 238, 255});
                DrawRectangleRoundedLines(backBtn, 0.15f, 4, (Color){180, 185, 195, 255});
                DrawCustomText("[ESC / Click] Back to Menu", (float)backBtn.x + 155, (float)backBtn.y + 7, 15, BLACK);

                if (mouseClicked && backHover) {
                    battleMenu = BATTLE_MENU_MAIN;
                }
            }

            // Party Switch Modal
            if (showSwitchMenu) {
                int switchChoice = DrawPartySwitchModal(&contxt.playerParty, isForcedSwitch, mouse);

                if (switchChoice == -2) {
                    showSwitchMenu = false;
                } else if (switchChoice >= 0 && switchChoice < contxt.playerParty.count) {
                    const char *prevName = pA->Name;
                    SwitchPokemon(&contxt.playerParty, switchChoice);
                    pA = GetActivePokemon(&contxt.playerParty);

                    UnloadTexture(aSprite);
                    aSprite = LoadPokemonSprite(pA->Name, true);
                    displayedPlayerHP = pA->current_health;

                    showSwitchMenu = false;

                    if (isForcedSwitch) {
                        AddBattleMessage("Go! %s!\n", pA->Name);
                        isForcedSwitch = false;
                        actionStage = ACTION_STAGE_ROUND_FINISH;
                    } else {
                        AddBattleMessage("Come back, %s! Go, %s!\n", prevName, pA->Name);

                        int bestEnemyMove = 0;
                        int bestWeight = get_move_weight(pB->Moves[0], *pA);
                        for (int m = 1; m < 4; m++) {
                            int weight = get_move_weight(pB->Moves[m], *pA);
                            if (weight > bestWeight) {
                                bestWeight = weight;
                                bestEnemyMove = m;
                            }
                        }
                        battleMenu = BATTLE_MENU_NONE;
                        PrepareTurnAction(pB, pA, &pB->Moves[bestEnemyMove], &currentAction);
                        AddBattleMessage("Enemy %s used %s!\n", pB->Name, pB->Moves[bestEnemyMove].Name);
                        if (currentAction.missed) {
                            AddBattleMessage("%s's attack missed!\n", pB->Name);
                        }
                        actionStage = ACTION_STAGE_FIRST_ATTACK_MSG;
                    }
                }
            }

            // Game Over Banner
            if (gameState == STATE_GAME_OVER) {
                DrawRectangle(0, 0, 1280, 720, (Color){0, 0, 0, 195});
                bool won = IsPartyWiped(&contxt.enemyParty);
                const char *resText = won ? "VICTORY!" : "DEFEAT!";
                Color resColor = won ? (Color){46, 204, 113, 255} : (Color){231, 76, 60, 255};

                DrawCustomText(resText, (float)(1280 - MeasureCustomText(resText, 56)) / 2, 230, 56, resColor);
                const char *subText = won ? "You defeated all opponent Pokemon!" : "All your Pokemon fainted!";
                DrawCustomText(subText, (float)(1280 - MeasureCustomText(subText, 22)) / 2, 310, 22, WHITE);

                Rectangle playAgainRec = { 500, 380, 280, 60 };
                bool paHover = CheckCollisionPointRec(mouse, playAgainRec);
                DrawRectangleRounded(playAgainRec, 0.15f, 6, paHover ? (Color){39, 174, 96, 255} : (Color){46, 204, 113, 255});
                DrawRectangleRoundedLines(playAgainRec, 0.15f, 6, WHITE);
                DrawCustomText("PLAY AGAIN [ENTER]", (float)playAgainRec.x + 40, (float)playAgainRec.y + 20, 20, WHITE);

                if (IsKeyPressed(KEY_ENTER) || (mouseClicked && paHover)) {
                    InitParty(&playerParty);
                    InitParty(&enemyParty);
                    battleMenu = BATTLE_MENU_MAIN;
                    actionStage = ACTION_STAGE_IDLE;
                    gameState = STATE_TEAM_SELECT;
                }
            }

            EndTextureMode();
        }

        // ==========================================
        // 4. PRESENTATION: SCALE VIRTUAL 1280x720 TO WINDOW
        // ==========================================
        BeginDrawing();
        ClearBackground(BLACK);

        Rectangle srcRec = { 0.0f, 0.0f, (float)screenTarget.texture.width, -(float)screenTarget.texture.height };
        DrawTexturePro(screenTarget.texture, srcRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);

        EndDrawing();
    }

    if (spritesLoaded) {
        UnloadTexture(aSprite);
        UnloadTexture(bSprite);
    }
    UnloadRenderTexture(screenTarget);
    UnloadUIAssets();
    CloseWindow();

    return 0;
}
