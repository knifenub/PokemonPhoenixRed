#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "raylib.h"
#include "types.h"
#include "type_chart.h"
#include "moves.h"
#include "pokemon_database.h"
#include "battle.h"

// Helper to load sprite with graceful fallback if the image file doesn't exist yet
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

    // Fallback to existing default sprites if available
    const char *defaultSprite = isBack ? "images/venusaur_back.png" : "images/charizard_front.png";
    if (FileExists(defaultSprite)) {
        return LoadTexture(defaultSprite);
    }

    // Safety fallback: generate colored placeholder texture
    Image img = GenImageColor(64, 64, isBack ? (Color){60, 120, 200, 255} : (Color){220, 70, 70, 255});
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    // Initialize battle with Pokémon generated directly from the database!
    Pokemon playerPokemon = CreatePokemonByName("Venusaur", 100);
    Pokemon enemyPokemon = CreatePokemonByName("Charizard", 100);

    BattleContext contxt;
    contxt.a = playerPokemon;
    contxt.b = enemyPokemon;
    contxt.player_a_active_move = NULL;
    contxt.player_b_active_move = NULL;

    AddBattleMessage("Wild %s appeared!\n", contxt.b.Name);
    AddBattleMessage("Go! %s!\n", contxt.a.Name);

    InitWindow(800, 450, "Pokemon Phoenix Red");
    SetTargetFPS(60);

    Texture2D aSprite = LoadPokemonSprite(contxt.a.Name, true);
    Texture2D bSprite = LoadPokemonSprite(contxt.b.Name, false);

    int selectedMove = 0;
    int displayedPlayerHP = contxt.a.current_health;
    int displayedEnemyHP = contxt.b.current_health;

    double lastHpUpdate = 0.0;
    int hpAnimationStage = 0;
    int nextHpAnimationStage = 0;

    while (!WindowShouldClose()) {
        int messageActive = UpdateBattleMessages();

        if (!messageActive) {
            int hpAnimating = (displayedPlayerHP != contxt.a.current_health) ||
                              (displayedEnemyHP != contxt.b.current_health);
            int chosenMove = -1;

            if (!hpAnimating && contxt.a.current_health > 0 && contxt.b.current_health > 0) {
                if (IsKeyPressed(KEY_ONE)) chosenMove = 0;
                if (IsKeyPressed(KEY_TWO)) chosenMove = 1;
                if (IsKeyPressed(KEY_THREE)) chosenMove = 2;
                if (IsKeyPressed(KEY_FOUR)) chosenMove = 3;
            }

            if (chosenMove != -1 && contxt.a.Moves[chosenMove].Current_PP > 0) {
                selectedMove = chosenMove;
                contxt.player_a_active_move = &contxt.a.Moves[selectedMove];

                // AI move selection
                int bestEnemyMove = 0;
                int bestWeight = get_move_weight(contxt.b.Moves[0], contxt.a);
                for (int i = 1; i < 4; i++) {
                    int weight = get_move_weight(contxt.b.Moves[i], contxt.a);
                    if (weight > bestWeight) {
                        bestWeight = weight;
                        bestEnemyMove = i;
                    }
                }
                contxt.player_b_active_move = &contxt.b.Moves[bestEnemyMove];

                // Turn processing according to speed
                if (contxt.a.speed >= contxt.b.speed) {
                    turn_processing(&contxt.a, &contxt.b, contxt.player_a_active_move);
                    if (contxt.b.current_health > 0) {
                        turn_processing(&contxt.b, &contxt.a, contxt.player_b_active_move);
                    }
                }
                else {
                    turn_processing(&contxt.b, &contxt.a, contxt.player_b_active_move);
                    if (contxt.a.current_health > 0) {
                        turn_processing(&contxt.a, &contxt.b, contxt.player_a_active_move);
                    }
                }

                // Status effects end of turn
                if (contxt.a.current_health > 0 && contxt.b.current_health > 0) {
                    status_effect_processing(&contxt.a, &contxt.b);
                    status_effect_processing(&contxt.b, &contxt.a);
                }

                if (contxt.a.current_health < 0) contxt.a.current_health = 0;
                if (contxt.b.current_health < 0) contxt.b.current_health = 0;

                if (contxt.a.speed >= contxt.b.speed) {
                    hpAnimationStage = 1;     // enemy HP first
                    nextHpAnimationStage = 2; // then player HP
                }
                else {
                    hpAnimationStage = 2;     // player HP first
                    nextHpAnimationStage = 1; // then enemy HP
                }
            }

            // HP animation tick
            if (GetTime() - lastHpUpdate >= 0.04) {
                if (hpAnimationStage == 1) {
                    if (displayedEnemyHP > contxt.b.current_health) {
                        displayedEnemyHP--;
                    }
                    else {
                        hpAnimationStage = nextHpAnimationStage;
                        nextHpAnimationStage = 0;
                    }
                }
                else if (hpAnimationStage == 2) {
                    if (displayedPlayerHP > contxt.a.current_health) {
                        displayedPlayerHP--;
                    }
                    else {
                        hpAnimationStage = nextHpAnimationStage;
                        nextHpAnimationStage = 0;
                    }
                }
                lastHpUpdate = GetTime();
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Battle text box
        DrawBattleTextBox();

        // Sprites
        DrawTextureEx(aSprite, (Vector2){90, 190}, 0.0f, 2.0f, WHITE);
        DrawTextureEx(bSprite, (Vector2){560, 40}, 0.0f, 2.0f, WHITE);

        // Enemy stats
        DrawText(TextFormat("%s  Lv.%d", contxt.b.Name, contxt.b.level), 50, 40, 25, BLACK);
        if (contxt.b.effect != STATUS_NONE) {
            DrawText(GetStatusText(contxt.b.effect), 150 + MeasureText(contxt.b.Name, 25) + 10, 43, 18, GRAY);
        }
        DrawText(TextFormat("HP: %d/%d", displayedEnemyHP, contxt.b.max_health), 50, 70, 22, BLACK);

        // Player stats
        DrawText(TextFormat("%s  Lv.%d", contxt.a.Name, contxt.a.level), 500, 240, 25, BLACK);
        if (contxt.a.effect != STATUS_NONE) {
            DrawText(GetStatusText(contxt.a.effect), 600 + MeasureText(contxt.a.Name, 25) + 10, 243, 18, GRAY);
        }
        DrawText(TextFormat("HP: %d/%d", displayedPlayerHP, contxt.a.max_health), 500, 270, 22, BLACK);

        // Move buttons
        Vector2 positions[4] = {
            {400, 330},
            {590, 330},
            {400, 385},
            {590, 385}
        };

        for (int i = 0; i < 4; i++) {
            int x = (int)positions[i].x;
            int y = (int)positions[i].y;

            if (selectedMove == i)
                DrawRectangle(x, y, 180, 45, LIGHTGRAY);

            DrawRectangleLines(x, y, 180, 45, BLACK);

            DrawText(
                TextFormat("%d. %s", i + 1, contxt.a.Moves[i].Name),
                x + 10, y + 5, 18, BLACK
            );

            DrawText(
                TextFormat("PP: %d/%d", contxt.a.Moves[i].Current_PP, contxt.a.Moves[i].Max_PP),
                x + 10, y + 27, 12, DARKGRAY
            );
        }

        // Faint messages
        if (contxt.a.current_health <= 0 && displayedPlayerHP <= 0) {
            DrawText(
                TextFormat("%s fainted!", contxt.a.Name),
                280, 180, 30, RED
            );
        }
        else if (contxt.b.current_health <= 0 && displayedEnemyHP <= 0) {
            DrawText(
                TextFormat("%s fainted!", contxt.b.Name),
                280, 180, 30, RED
            );
        }

        EndDrawing();
    }

    UnloadTexture(aSprite);
    UnloadTexture(bSprite);
    CloseWindow();

    return 0;
}
