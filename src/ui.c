#include "ui.h"
#include "pokemon_database.h"
#include "type_chart.h"
#include "party.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static Font g_GameFont = { 0 };
static bool g_FontLoaded = false;

#define MAX_THUMB_CACHE 160
typedef struct {
    char name[64];
    Texture2D texture;
} ThumbnailEntry;

static ThumbnailEntry g_ThumbCache[MAX_THUMB_CACHE];
static int g_ThumbCount = 0;

void InitUIAssets(void)
{
    // High-legibility, bold, visible system font (no pixel font)
    const char *fontPaths[] = {
        "C:/Windows/Fonts/segoeuib.ttf", // Segoe UI Bold
        "C:/Windows/Fonts/arialbd.ttf",  // Arial Bold
        "C:/Windows/Fonts/calibrib.ttf", // Calibri Bold
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (int i = 0; i < 5; i++) {
        if (FileExists(fontPaths[i])) {
            g_GameFont = LoadFontEx(fontPaths[i], 48, NULL, 0);
            SetTextureFilter(g_GameFont.texture, TEXTURE_FILTER_BILINEAR);
            g_FontLoaded = true;
            break;
        }
    }
}

void UnloadUIAssets(void)
{
    if (g_FontLoaded) {
        UnloadFont(g_GameFont);
        g_FontLoaded = false;
    }

    for (int i = 0; i < g_ThumbCount; i++) {
        UnloadTexture(g_ThumbCache[i].texture);
    }
    g_ThumbCount = 0;
}

void DrawCustomText(const char *text, float x, float y, float fontSize, Color color)
{
    if (!text) return;
    if (g_FontLoaded) {
        DrawTextEx(g_GameFont, text, (Vector2){x, y}, fontSize, 1.0f, color);
    } else {
        DrawText(text, (int)x, (int)y, (int)fontSize, color);
    }
}

int MeasureCustomText(const char *text, float fontSize)
{
    if (!text) return 0;
    if (g_FontLoaded) {
        Vector2 sz = MeasureTextEx(g_GameFont, text, fontSize, 1.0f);
        return (int)sz.x;
    }
    return MeasureText(text, (int)fontSize);
}

Color GetTypeColor(ElementalType type)
{
    switch (type) {
        case TYPE_NORMAL:   return (Color){168, 168, 120, 255};
        case TYPE_FIRE:     return (Color){240, 128,  48, 255};
        case TYPE_WATER:    return (Color){104, 144, 240, 255};
        case TYPE_GRASS:    return (Color){120, 200,  80, 255};
        case TYPE_ELECTRIC: return (Color){248, 208,  48, 255};
        case TYPE_ICE:      return (Color){152, 216, 216, 255};
        case TYPE_FIGHTING: return (Color){192,  48,  40, 255};
        case TYPE_POISON:   return (Color){160,  64, 160, 255};
        case TYPE_GROUND:   return (Color){224, 192, 104, 255};
        case TYPE_FLYING:   return (Color){168, 144, 240, 255};
        case TYPE_PSYCHIC:  return (Color){248,  88, 136, 255};
        case TYPE_BUG:      return (Color){168, 184,  32, 255};
        case TYPE_ROCK:     return (Color){184, 160,  56, 255};
        case TYPE_GHOST:    return (Color){112,  88, 152, 255};
        case TYPE_DRAGON:   return (Color){112,  56, 248, 255};
        case TYPE_DARK:     return (Color){112,  88,  72, 255};
        case TYPE_STEEL:    return (Color){184, 184, 208, 255};
        case TYPE_FAIRY:    return (Color){238, 153, 172, 255};
        default:            return (Color){160, 160, 160, 255};
    }
}

Texture2D GetCachedPokemonThumbnail(const char *name)
{
    if (!name || name[0] == '\0') {
        Image img = GenImageColor(48, 48, BLANK);
        Texture2D t = LoadTextureFromImage(img);
        UnloadImage(img);
        return t;
    }

    // Check cache
    for (int i = 0; i < g_ThumbCount; i++) {
        if (strcasecmp(g_ThumbCache[i].name, name) == 0) {
            return g_ThumbCache[i].texture;
        }
    }

    // Not in cache: load file
    char lowerName[64];
    int idx = 0;
    while (name[idx] && idx < 63) {
        lowerName[idx] = (char)tolower((unsigned char)name[idx]);
        idx++;
    }
    lowerName[idx] = '\0';

    char filename[128];
    snprintf(filename, sizeof(filename), "images/%s_front.png", lowerName);

    Texture2D tex;
    if (FileExists(filename)) {
        tex = LoadTexture(filename);
    } else {
        // Try sanitized
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
        snprintf(filename, sizeof(filename), "images/%s_front.png", cleanName);
        if (FileExists(filename)) {
            tex = LoadTexture(filename);
        } else {
            Image img = GenImageColor(48, 48, (Color){200, 100, 100, 255});
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
        }
    }

    // Add to cache
    if (g_ThumbCount < MAX_THUMB_CACHE) {
        strncpy(g_ThumbCache[g_ThumbCount].name, name, 63);
        g_ThumbCache[g_ThumbCount].name[63] = '\0';
        g_ThumbCache[g_ThumbCount].texture = tex;
        g_ThumbCount++;
    }

    return tex;
}

void DrawTypeBadge(ElementalType type, int x, int y, int width, int height, int fontSize)
{
    if (type <= TYPE_NONE || type >= TYPE_COUNT)
        return;

    Color c = GetTypeColor(type);
    Rectangle rec = { (float)x, (float)y, (float)width, (float)height };
    DrawRectangleRounded(rec, 0.35f, 4, c);
    DrawRectangleRoundedLines(rec, 0.35f, 4, (Color){0, 0, 0, 180});

    const char *name = GetTypeName(type);
    int textWidth = MeasureCustomText(name, (float)fontSize);
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - fontSize) / 2;
    DrawCustomText(name, (float)textX, (float)textY, (float)fontSize, WHITE);
}

void DrawStatusBadge(StatusEffect effect, int x, int y, int width, int height, int fontSize)
{
    if (effect <= STATUS_NONE)
        return;

    Color c = GetStatusColor(effect);
    Rectangle rec = { (float)x, (float)y, (float)width, (float)height };

    // High-contrast, dark ailment badge to clearly distinguish from elemental type badges
    DrawRectangleRounded(rec, 0.35f, 4, (Color){22, 25, 32, 255});
    DrawRectangleRoundedLines(rec, 0.35f, 4, c);

    // Glowing status indicator dot
    DrawCircle(x + 9, y + height / 2, 3, c);

    const char *name = GetStatusText(effect);
    int textWidth = MeasureCustomText(name, (float)fontSize);
    int textX = x + 15 + (width - 15 - textWidth) / 2;
    int textY = y + (height - fontSize) / 2;
    DrawCustomText(name, (float)textX, (float)textY, (float)fontSize, c);
}

void DrawHPBar(int x, int y, int width, int height, int currentHP, int maxHP, bool showText)
{
    if (maxHP <= 0) maxHP = 1;
    if (currentHP < 0) currentHP = 0;
    if (currentHP > maxHP) currentHP = maxHP;

    float pct = (float)currentHP / (float)maxHP;

    // Dark background frame
    DrawRectangle(x, y, width, height, (Color){35, 39, 42, 255});
    DrawRectangleLines(x, y, width, height, BLACK);

    // Color based on HP percentage
    Color barColor;
    if (pct > 0.50f) {
        barColor = (Color){46, 204, 113, 255}; // Green
    } else if (pct > 0.20f) {
        barColor = (Color){241, 196, 15, 255}; // Yellow
    } else {
        barColor = (Color){231, 76, 60, 255};  // Red
    }

    int fillWidth = (int)((width - 4) * pct);
    if (fillWidth > 0) {
        DrawRectangle(x + 2, y + 2, fillWidth, height - 4, barColor);
    }

    if (showText) {
        char hpStr[32];
        snprintf(hpStr, sizeof(hpStr), "%d/%d", currentHP, maxHP);
        DrawCustomText(hpStr, (float)(x + width + 8), (float)(y + (height - 12) / 2), 13, (Color){40, 40, 40, 255});
    }
}

bool DrawTeamSelectionScreen(Party *playerParty, int *scrollIndex, Vector2 mouse)
{
    int dbCount = GetDatabasePokemonCount();
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // Scrolling controls
    float wheel = GetMouseWheelMove();
    if (wheel > 0) (*scrollIndex) -= 2;
    if (wheel < 0) (*scrollIndex) += 2;
    if (IsKeyPressed(KEY_UP)) (*scrollIndex)--;
    if (IsKeyPressed(KEY_DOWN)) (*scrollIndex)++;
    if (IsKeyPressed(KEY_PAGE_UP)) (*scrollIndex) -= 5;
    if (IsKeyPressed(KEY_PAGE_DOWN)) (*scrollIndex) += 5;

    if (*scrollIndex > dbCount - 5) *scrollIndex = dbCount - 5;
    if (*scrollIndex < 0) *scrollIndex = 0;

    // Background
    ClearBackground((Color){244, 246, 249, 255});

    // Top Header (1280x56)
    DrawRectangle(0, 0, 1280, 56, (Color){41, 128, 185, 255});
    DrawCustomText("BUILD YOUR TEAM (1 - 6 POKEMON)", 25, 14, 24, WHITE);
    DrawCustomText(TextFormat("Team: %d/6", playerParty->count), 950, 17, 18, (playerParty->count >= 1) ? (Color){46, 204, 113, 255} : (Color){241, 196, 15, 255});

    // Left Area: Roster List (x = 25, y = 72, width = 780, height = 550)
    int listX = 25;
    int listY = 72;
    int listW = 780;
    int listH = 550;
    int cardH = 98;

    DrawRectangle(listX, listY, listW, listH, WHITE);
    DrawRectangleLines(listX, listY, listW, listH, (Color){210, 215, 220, 255});

    for (int i = 0; i < 5; i++) {
        int idx = *scrollIndex + i;
        if (idx >= dbCount) break;

        const PokemonSpecies *sp = GetPokemonSpeciesByIndex(idx);
        if (!sp) continue;

        int rowY = listY + i * 108 + 8;
        Rectangle rowRec = { (float)listX + 8, (float)rowY, (float)listW - 16, (float)cardH };
        bool isHovered = CheckCollisionPointRec(mouse, rowRec);

        // Check if already in party
        bool inParty = false;
        for (int p = 0; p < playerParty->count; p++) {
            if (strcasecmp(playerParty->members[p].Name, sp->name) == 0) {
                inParty = true;
                break;
            }
        }

        Color bg = inParty ? (Color){238, 250, 242, 255} : (isHovered ? (Color){242, 248, 254, 255} : WHITE);
        DrawRectangleRec(rowRec, bg);
        DrawRectangleLinesEx(rowRec, 1, inParty ? (Color){46, 204, 113, 255} : (Color){220, 225, 230, 255});

        // 1. FRONT SPRITE THUMBNAIL
        Texture2D thumb = GetCachedPokemonThumbnail(sp->name);
        Rectangle srcRec = { 0, 0, (float)thumb.width, (float)thumb.height };
        Rectangle dstRec = { (float)listX + 16, (float)rowY + 11, 76, 76 };
        DrawTexturePro(thumb, srcRec, dstRec, (Vector2){0, 0}, 0.0f, WHITE);

        // 2. NAME & POKEDEX #
        DrawCustomText(TextFormat("#%03d %s", sp->id, sp->name), (float)listX + 104, (float)rowY + 14, 22, (Color){30, 30, 30, 255});

        // 3. TYPES
        DrawTypeBadge(sp->type1, listX + 104, rowY + 50, 68, 26, 12);
        if (sp->type2 != TYPE_NONE) {
            DrawTypeBadge(sp->type2, listX + 178, rowY + 50, 68, 26, 12);
        }

        // 4. STATS (HP, ATK, DEF, SPE)
        DrawCustomText(TextFormat("HP %d   Atk %d   Def %d   Spe %d", sp->baseHP, sp->baseAttack, sp->baseDefense, sp->baseSpeed),
                       (float)listX + 265, (float)rowY + 20, 16, (Color){70, 70, 70, 255});
        DrawCustomText(TextFormat("SpA %d   SpD %d", sp->baseSpAttack, sp->baseSpDefense),
                       (float)listX + 265, (float)rowY + 52, 15, DARKGRAY);

        // 5. ADD / ADDED BUTTON
        Rectangle btnRec = { (float)listX + listW - 125, (float)rowY + 28, 105, 42 };
        bool btnHover = CheckCollisionPointRec(mouse, btnRec);

        if (inParty) {
            DrawRectangleRec(btnRec, (Color){46, 204, 113, 255});
            DrawCustomText("IN TEAM", (float)btnRec.x + 18, (float)btnRec.y + 11, 15, WHITE);
        } else if (playerParty->count < MAX_PARTY_SIZE) {
            DrawRectangleRec(btnRec, btnHover ? (Color){52, 152, 219, 255} : (Color){41, 128, 185, 255});
            DrawCustomText("+ ADD", (float)btnRec.x + 24, (float)btnRec.y + 11, 15, WHITE);
            if ((clicked && isHovered) || (clicked && btnHover)) {
                AddToParty(playerParty, CreatePokemon(sp, 100));
            }
        } else {
            DrawRectangleRec(btnRec, (Color){189, 195, 199, 255});
            DrawCustomText("FULL", (float)btnRec.x + 30, (float)btnRec.y + 11, 15, WHITE);
        }
    }

    // Scroll Indicator / Navigation
    DrawCustomText(TextFormat("Pokemon %d - %d of %d", *scrollIndex + 1, (*scrollIndex + 5 > dbCount ? dbCount : *scrollIndex + 5), dbCount),
                   (float)listX + 10, 636, 16, DARKGRAY);

    Rectangle prevRec = { (float)listX + 540, 632, 90, 32 };
    Rectangle nextRec = { (float)listX + 645, 632, 90, 32 };
    bool prevHover = CheckCollisionPointRec(mouse, prevRec);
    bool nextHover = CheckCollisionPointRec(mouse, nextRec);

    DrawRectangleRec(prevRec, prevHover ? (Color){200, 205, 210, 255} : (Color){225, 230, 235, 255});
    DrawCustomText("< Prev", (float)prevRec.x + 18, (float)prevRec.y + 6, 14, BLACK);
    if (clicked && prevHover) (*scrollIndex) -= 5;

    DrawRectangleRec(nextRec, nextHover ? (Color){200, 205, 210, 255} : (Color){225, 230, 235, 255});
    DrawCustomText("Next >", (float)nextRec.x + 18, (float)nextRec.y + 6, 14, BLACK);
    if (clicked && nextHover) (*scrollIndex) += 5;

    // Bottom Action Controls
    Rectangle randRec = { (float)listX, 674, 160, 36 };
    bool randHover = CheckCollisionPointRec(mouse, randRec);
    DrawRectangleRec(randRec, randHover ? (Color){142, 68, 173, 255} : (Color){155, 89, 182, 255});
    DrawCustomText("Random Team", (float)randRec.x + 22, (float)randRec.y + 8, 15, WHITE);
    if (clicked && randHover) {
        GenerateRandomParty(playerParty, 6, 100);
    }

    Rectangle clearRec = { (float)listX + 180, 674, 150, 36 };
    bool clearHover = CheckCollisionPointRec(mouse, clearRec);
    DrawRectangleRec(clearRec, clearHover ? (Color){192, 57, 43, 255} : (Color){231, 76, 60, 255});
    DrawCustomText("Clear Party", (float)clearRec.x + 25, (float)clearRec.y + 8, 15, WHITE);
    if (clicked && clearHover) {
        InitParty(playerParty);
    }

    // Right Area: Selected Party Slots (x = 830, y = 72, width = 425, height = 550)
    int partyX = 830;
    int partyY = 72;
    int partyW = 425;
    int partyH = 550;

    DrawRectangle(partyX, partyY, partyW, partyH, WHITE);
    DrawRectangleLines(partyX, partyY, partyW, partyH, (Color){210, 215, 220, 255});
    DrawCustomText("YOUR PARTY", (float)partyX + 20, (float)partyY + 12, 20, (Color){30, 30, 30, 255});

    for (int p = 0; p < MAX_PARTY_SIZE; p++) {
        int slotY = partyY + 44 + p * 82;
        Rectangle slotRec = { (float)partyX + 12, (float)slotY, (float)partyW - 24, 74 };

        if (p < playerParty->count) {
            Pokemon *m = &playerParty->members[p];
            bool slotHover = CheckCollisionPointRec(mouse, slotRec);

            DrawRectangleRec(slotRec, slotHover ? (Color){254, 242, 242, 255} : (Color){248, 250, 252, 255});
            DrawRectangleLinesEx(slotRec, 1, (Color){46, 204, 113, 255});

            // Member Front Sprite Thumbnail
            Texture2D pThumb = GetCachedPokemonThumbnail(m->Name);
            Rectangle pSrc = { 0, 0, (float)pThumb.width, (float)pThumb.height };
            Rectangle pDst = { (float)partyX + 20, (float)slotY + 7, 60, 60 };
            DrawTexturePro(pThumb, pSrc, pDst, (Vector2){0, 0}, 0.0f, WHITE);

            DrawCustomText(TextFormat("%d. %s", p + 1, m->Name), (float)partyX + 90, (float)slotY + 10, 18, BLACK);
            DrawTypeBadge(m->type1, partyX + 90, slotY + 38, 56, 22, 10);
            if (m->type2 != TYPE_NONE) {
                DrawTypeBadge(m->type2, partyX + 152, slotY + 38, 56, 22, 10);
            }

            // Remove button [X]
            Rectangle remRec = { (float)partyX + partyW - 52, (float)slotY + 20, 32, 32 };
            bool remHover = CheckCollisionPointRec(mouse, remRec);
            DrawRectangleRec(remRec, remHover ? RED : (Color){225, 228, 232, 255});
            DrawCustomText("X", (float)remRec.x + 10, (float)remRec.y + 6, 16, remHover ? WHITE : DARKGRAY);

            if (clicked && (remHover || slotHover)) {
                RemoveFromParty(playerParty, p);
                break;
            }
        } else {
            DrawRectangleRec(slotRec, (Color){247, 249, 250, 255});
            DrawRectangleLinesEx(slotRec, 1, (Color){220, 225, 230, 255});
            DrawCustomText(TextFormat("[ Slot %d Empty ]", p + 1), (float)partyX + 130, (float)slotY + 26, 16, (Color){170, 175, 180, 255});
        }
    }

    // START BATTLE Button (Large, high-contrast)
    Rectangle startRec = { (float)partyX, 640, (float)partyW, 68 };
    bool startHover = CheckCollisionPointRec(mouse, startRec);
    bool canStart = (playerParty->count >= 1);

    Color startBg = canStart ? (startHover ? (Color){39, 174, 96, 255} : (Color){46, 204, 113, 255})
                             : (Color){189, 195, 199, 255};
    DrawRectangleRec(startRec, startBg);
    DrawRectangleLinesEx(startRec, 2, canStart ? (Color){30, 130, 70, 255} : (Color){150, 150, 150, 255});

    const char *startText = canStart ? "START BATTLE! [ENTER]" : "PICK AT LEAST 1 POKEMON";
    int startTextW = MeasureCustomText(startText, 20);
    DrawCustomText(startText, (float)startRec.x + (partyW - startTextW) / 2, (float)startRec.y + 22, 20, WHITE);

    if (canStart && (IsKeyPressed(KEY_ENTER) || (clicked && startHover))) {
        return true;
    }

    return false;
}

int DrawPartySwitchModal(const Party *party, bool isForced, Vector2 mouse)
{
    if (!party) return -1;
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // Dark semi-transparent background overlay covering 1280x720 canvas
    DrawRectangle(0, 0, 1280, 720, (Color){0, 0, 0, 185});

    // Modal Box (960x600 centered)
    int mw = 960;
    int mh = 600;
    int mx = (1280 - mw) / 2;
    int my = (720 - mh) / 2;

    DrawRectangle(mx, my, mw, mh, (Color){250, 252, 255, 255});
    DrawRectangleLinesEx((Rectangle){(float)mx, (float)my, (float)mw, (float)mh}, 3, (Color){41, 128, 185, 255});

    // Header
    const char *title = isForced ? "YOUR POKEMON FAINTED! CHOOSE NEXT POKEMON:" : "SWITCH POKEMON";
    DrawCustomText(title, (float)mx + 25, (float)my + 18, 22, (Color){30, 30, 30, 255});
    if (!isForced) {
        DrawCustomText("[ESC] Cancel", (float)mx + mw - 140, (float)my + 20, 16, DARKGRAY);
    }

    int chosenIndex = -1;

    for (int i = 0; i < party->count; i++) {
        const Pokemon *p = &party->members[i];
        int slotY = my + 60 + i * 86;
        Rectangle slotRec = { (float)mx + 20, (float)slotY, (float)mw - 40, 78 };

        bool isActive = (i == party->activeIndex);
        bool isFainted = (p->current_health <= 0);
        bool isHovered = CheckCollisionPointRec(mouse, slotRec);

        Color slotBg = isActive ? (Color){225, 240, 252, 255} :
                       isFainted ? (Color){248, 228, 228, 255} :
                       (isHovered ? (Color){238, 248, 238, 255} : WHITE);

        DrawRectangleRec(slotRec, slotBg);
        DrawRectangleLinesEx(slotRec, 1, isActive ? (Color){41, 128, 185, 255} : (Color){210, 215, 220, 255});

        // 1. Front Sprite Thumbnail
        Texture2D thumb = GetCachedPokemonThumbnail(p->Name);
        Rectangle sSrc = { 0, 0, (float)thumb.width, (float)thumb.height };
        Rectangle sDst = { (float)mx + 30, (float)slotY + 8, 62, 62 };
        DrawTexturePro(thumb, sSrc, sDst, (Vector2){0, 0}, 0.0f, WHITE);

        // 2. Slot number & Name
        DrawCustomText(TextFormat("%d.", i + 1), (float)mx + 104, (float)slotY + 18, 20, BLACK);
        DrawCustomText(TextFormat("%s  Lv.%d", p->Name, p->level), (float)mx + 132, (float)slotY + 18, 20, isFainted ? GRAY : BLACK);

        // 3. Types
        DrawTypeBadge(p->type1, mx + 380, slotY + 22, 68, 26, 12);
        if (p->type2 != TYPE_NONE) {
            DrawTypeBadge(p->type2, mx + 456, slotY + 22, 68, 26, 12);
        }

        // 4. Status Badge if afflicted
        if (p->effect != STATUS_NONE) {
            DrawStatusBadge(p->effect, mx + 536, slotY + 22, 56, 26, 11);
        }

        // 5. HP Bar (without overlapping text)
        DrawHPBar(mx + 606, slotY + 26, 140, 20, p->current_health, p->max_health, false);
        // Dedicated, non-overlapping HP text
        DrawCustomText(TextFormat("%d/%d", p->current_health, p->max_health), (float)mx + 756, (float)slotY + 26, 16, (Color){40, 40, 40, 255});

        // 6. Status Tag at right margin
        if (isActive) {
            DrawCustomText("[ACTIVE]", (float)mx + mw - 120, (float)slotY + 26, 16, (Color){41, 128, 185, 255});
        } else if (isFainted) {
            DrawCustomText("[FAINTED]", (float)mx + mw - 120, (float)slotY + 26, 16, RED);
        } else {
            DrawCustomText("[READY]", (float)mx + mw - 120, (float)slotY + 26, 16, (Color){46, 204, 113, 255});
            if (clicked && isHovered) {
                chosenIndex = i;
            }
        }

        // Numeric shortcuts (1 - 6)
        if (!isActive && !isFainted) {
            if (i == 0 && IsKeyPressed(KEY_ONE))   chosenIndex = 0;
            if (i == 1 && IsKeyPressed(KEY_TWO))   chosenIndex = 1;
            if (i == 2 && IsKeyPressed(KEY_THREE)) chosenIndex = 2;
            if (i == 3 && IsKeyPressed(KEY_FOUR))  chosenIndex = 3;
            if (i == 4 && IsKeyPressed(KEY_FIVE))  chosenIndex = 4;
            if (i == 5 && IsKeyPressed(KEY_SIX))   chosenIndex = 5;
        }
    }

    if (!isForced && IsKeyPressed(KEY_ESCAPE)) {
        return -2;
    }

    return chosenIndex;
}

void DrawPokemonStatsInTextBox(const Pokemon *p, bool isEnemy)
{
    if (!p) return;

    Rectangle bar = { 50, 510, 680, 180 };

    // Outer card
    DrawRectangleRounded(bar, 0.05f, 6, (Color){250, 252, 255, 255});
    Color borderCol = isEnemy ? (Color){231, 76, 60, 255} : (Color){41, 128, 185, 255};
    DrawRectangleRoundedLines(bar, 0.05f, 6, borderCol);

    // 1. Top Header Row
    Texture2D thumb = GetCachedPokemonThumbnail(p->Name);
    Rectangle srcRec = { 0, 0, (float)thumb.width, (float)thumb.height };
    Rectangle dstRec = { bar.x + 16, bar.y + 10, 44, 44 };
    DrawTexturePro(thumb, srcRec, dstRec, (Vector2){0, 0}, 0.0f, WHITE);

    DrawCustomText(TextFormat("%s  Lv.%d", p->Name, p->level), bar.x + 68, bar.y + 14, 22, (Color){25, 28, 35, 255});

    int nameW = MeasureCustomText(TextFormat("%s  Lv.%d", p->Name, p->level), 22);
    int typeX = (int)bar.x + 68 + nameW + 12;
    DrawTypeBadge(p->type1, typeX, (int)bar.y + 14, 60, 24, 11);
    if (p->type2 != TYPE_NONE) {
        DrawTypeBadge(p->type2, typeX + 66, (int)bar.y + 14, 60, 24, 11);
    }

    const char *roleText = isEnemy ? "[OPPONENT POKEMON]" : "[YOUR ACTIVE POKEMON]";
    Color roleCol = isEnemy ? (Color){231, 76, 60, 255} : (Color){41, 128, 185, 255};
    int roleW = MeasureCustomText(roleText, 14);
    DrawCustomText(roleText, (float)bar.x + bar.width - roleW - 20, (float)bar.y + 18, 14, roleCol);

    // Divider line
    DrawRectangle((int)bar.x + 16, (int)bar.y + 54, (int)bar.width - 32, 1, (Color){225, 230, 238, 255});

    // 2. HP & Status Row
    float hpPct = (p->max_health > 0) ? ((float)p->current_health / (float)p->max_health) * 100.0f : 0.0f;
    DrawCustomText(TextFormat("HP: %d / %d (%.0f%%)", p->current_health, p->max_health, hpPct), (float)bar.x + 20, (float)bar.y + 62, 16, (Color){35, 40, 50, 255});

    DrawCustomText("Status Condition:", (float)bar.x + 260, (float)bar.y + 62, 15, (Color){100, 105, 115, 255});
    if (p->effect != STATUS_NONE) {
        DrawStatusBadge(p->effect, (int)bar.x + 395, (int)bar.y + 60, 58, 22, 11);
    } else {
        DrawCustomText("[Healthy - No Ailment]", (float)bar.x + 395, (float)bar.y + 62, 15, (Color){46, 204, 113, 255});
    }

    // Divider line
    DrawRectangle((int)bar.x + 16, (int)bar.y + 88, (int)bar.width - 32, 1, (Color){225, 230, 238, 255});

    // 3. Stats Breakdown Row (Attack, Defense, Sp. Atk, Sp. Def, Speed)
    int colW = (int)(bar.width - 40) / 5;

    DrawCustomText("Attack", (float)bar.x + 20 + 0 * colW, (float)bar.y + 96, 13, (Color){110, 115, 125, 255});
    DrawCustomText(TextFormat("%d", p->attack), (float)bar.x + 20 + 0 * colW, (float)bar.y + 114, 18, (Color){25, 30, 40, 255});

    DrawCustomText("Defense", (float)bar.x + 20 + 1 * colW, (float)bar.y + 96, 13, (Color){110, 115, 125, 255});
    DrawCustomText(TextFormat("%d", p->defense), (float)bar.x + 20 + 1 * colW, (float)bar.y + 114, 18, (Color){25, 30, 40, 255});

    DrawCustomText("Sp. Atk", (float)bar.x + 20 + 2 * colW, (float)bar.y + 96, 13, (Color){110, 115, 125, 255});
    DrawCustomText(TextFormat("%d", p->special_attack), (float)bar.x + 20 + 2 * colW, (float)bar.y + 114, 18, (Color){25, 30, 40, 255});

    DrawCustomText("Sp. Def", (float)bar.x + 20 + 3 * colW, (float)bar.y + 96, 13, (Color){110, 115, 125, 255});
    DrawCustomText(TextFormat("%d", p->special_defense), (float)bar.x + 20 + 3 * colW, (float)bar.y + 114, 18, (Color){25, 30, 40, 255});

    DrawCustomText("Speed", (float)bar.x + 20 + 4 * colW, (float)bar.y + 96, 13, (Color){110, 115, 125, 255});
    DrawCustomText(TextFormat("%d", p->speed), (float)bar.x + 20 + 4 * colW, (float)bar.y + 114, 18, (Color){25, 30, 40, 255});

    // 4. Footer Hint
    DrawRectangle((int)bar.x + 16, (int)bar.y + 142, (int)bar.width - 32, 1, (Color){225, 230, 238, 255});
    const char *hintStr = isEnemy ? "Inspect enemy stats to strategize type matchups and speed tiers." : "Your active battler. Use [2] Switch to swap party members anytime.";
    DrawCustomText(hintStr, (float)bar.x + 20, (float)bar.y + 150, 13, DARKGRAY);
}

void DrawMoveDetailsInTextBox(const move *m, const Pokemon *target)
{
    if (!m) return;

    Rectangle bar = { 50, 510, 680, 180 };

    DrawRectangleRounded(bar, 0.05f, 6, (Color){250, 252, 255, 255});
    DrawRectangleRoundedLines(bar, 0.05f, 6, (Color){41, 128, 185, 255});

    // 1. Move Title Row
    DrawCustomText(m->Name, (float)bar.x + 20, (float)bar.y + 12, 22, (Color){20, 25, 35, 255});

    int nameW = MeasureCustomText(m->Name, 22);
    int typeX = (int)bar.x + 20 + nameW + 14;
    DrawTypeBadge(m->type, typeX, (int)bar.y + 12, 64, 24, 11);

    const char *catStr = (m->category == CATEGORY_PHYSICAL) ? "Physical" :
                         (m->category == CATEGORY_SPECIAL  ? "Special"  : "Status");
    Color catBg = (m->category == CATEGORY_PHYSICAL) ? (Color){230, 100, 40, 255} :
                  (m->category == CATEGORY_SPECIAL  ? (Color){70, 130, 230, 255} : (Color){130, 140, 150, 255});
    Rectangle catRec = { (float)typeX + 72, (float)bar.y + 12, 78, 24 };
    DrawRectangleRounded(catRec, 0.35f, 4, catBg);
    DrawRectangleRoundedLines(catRec, 0.35f, 4, (Color){0, 0, 0, 150});
    int catW = MeasureCustomText(catStr, 11);
    DrawCustomText(catStr, catRec.x + (catRec.width - catW) / 2, catRec.y + 5, 11, WHITE);

    // Target Matchup Tag
    if (target) {
        if (m->category == CATEGORY_STATUS) {
            DrawCustomText("Status Move", (float)bar.x + bar.width - 120, (float)bar.y + 16, 14, DARKGRAY);
        } else {
            double eff = type_effectiveness(m->type, target->type1) *
                         type_effectiveness(m->type, target->type2);
            if (eff > 1.0) {
                const char *effStr = TextFormat("SUPER EFFECTIVE (%.1fx)", eff);
                int effW = MeasureCustomText(effStr, 14);
                DrawCustomText(effStr, (float)bar.x + bar.width - effW - 20, (float)bar.y + 16, 14, (Color){39, 174, 96, 255});
            } else if (eff == 0.0) {
                const char *effStr = "NO EFFECT (0x)";
                int effW = MeasureCustomText(effStr, 14);
                DrawCustomText(effStr, (float)bar.x + bar.width - effW - 20, (float)bar.y + 16, 14, (Color){192, 57, 43, 255});
            } else if (eff < 1.0) {
                const char *effStr = TextFormat("NOT VERY EFFECTIVE (%.2gx)", eff);
                int effW = MeasureCustomText(effStr, 14);
                DrawCustomText(effStr, (float)bar.x + bar.width - effW - 20, (float)bar.y + 16, 14, (Color){230, 126, 34, 255});
            } else {
                const char *effStr = "NEUTRAL (1.0x)";
                int effW = MeasureCustomText(effStr, 14);
                DrawCustomText(effStr, (float)bar.x + bar.width - effW - 20, (float)bar.y + 16, 14, (Color){100, 105, 115, 255});
            }
        }
    }

    // Divider line
    DrawRectangle((int)bar.x + 16, (int)bar.y + 46, (int)bar.width - 32, 1, (Color){225, 230, 238, 255});

    // 2. Power, Accuracy, PP Row
    char powerStr[32];
    if (m->BP > 0) snprintf(powerStr, sizeof(powerStr), "Power: %d", m->BP);
    else snprintf(powerStr, sizeof(powerStr), "Power: --");

    char accStr[32];
    if (m->Accuracy > 0) snprintf(accStr, sizeof(accStr), "Accuracy: %d%%", m->Accuracy);
    else snprintf(accStr, sizeof(accStr), "Accuracy: --");

    char ppStr[32];
    snprintf(ppStr, sizeof(ppStr), "PP: %d/%d", m->Current_PP, m->Max_PP);

    DrawCustomText(powerStr, (float)bar.x + 20, (float)bar.y + 54, 16, (Color){30, 35, 45, 255});
    DrawCustomText(accStr,   (float)bar.x + 180, (float)bar.y + 54, 16, (Color){30, 35, 45, 255});
    DrawCustomText(ppStr,    (float)bar.x + 360, (float)bar.y + 54, 16, (Color){30, 35, 45, 255});

    // Divider line
    DrawRectangle((int)bar.x + 16, (int)bar.y + 80, (int)bar.width - 32, 1, (Color){225, 230, 238, 255});

    // 3. Status Effect Row
    DrawCustomText("Status Application:", (float)bar.x + 20, (float)bar.y + 89, 14, (Color){100, 105, 115, 255});
    if (m->effect != STATUS_NONE) {
        const char *effName = GetStatusFullText(m->effect);
        DrawStatusBadge(m->effect, (int)bar.x + 165, (int)bar.y + 86, 56, 22, 11);
        DrawCustomText(TextFormat("(%d%% chance to apply %s)", m->probability, effName), (float)bar.x + 230, (float)bar.y + 89, 14, (Color){40, 45, 55, 255});
    } else {
        DrawCustomText("None (Deals direct damage only)", (float)bar.x + 165, (float)bar.y + 89, 14, (Color){120, 125, 135, 255});
    }

    // Divider line
    DrawRectangle((int)bar.x + 16, (int)bar.y + 120, (int)bar.width - 32, 1, (Color){225, 230, 238, 255});

    // 4. Instructions
    DrawCustomText("[Click to Attack with this move, or press key 1 - 4]", (float)bar.x + 20, (float)bar.y + 138, 13, (Color){130, 135, 145, 255});
}

void DrawIdleBattlePromptInTextBox(const Pokemon *playerActive, bool inMoveMenu)
{
    Rectangle bar = { 50, 510, 680, 180 };

    DrawRectangleRounded(bar, 0.05f, 6, (Color){250, 252, 255, 255});
    DrawRectangleRoundedLines(bar, 0.05f, 6, (Color){35, 45, 60, 255});

    if (inMoveMenu) {
        DrawCustomText("CHOOSE AN ATTACK MOVE:", (float)bar.x + 24, (float)bar.y + 24, 22, (Color){25, 30, 40, 255});
        DrawCustomText("Hover over any move button to view Power, Accuracy & Matchup.", (float)bar.x + 24, (float)bar.y + 60, 16, (Color){41, 128, 185, 255});
        DrawCustomText("Click a move or press [1 - 4] to execute.", (float)bar.x + 24, (float)bar.y + 92, 15, (Color){60, 65, 75, 255});
        DrawCustomText("Press [ESC] or click 'Back to Menu' to return to options.", (float)bar.x + 24, (float)bar.y + 135, 13, DARKGRAY);
    } else {
        const char *name = playerActive ? playerActive->Name : "POKEMON";
        DrawCustomText(TextFormat("What will %s do?", name), (float)bar.x + 24, (float)bar.y + 22, 24, (Color){20, 25, 35, 255});
        DrawCustomText("[1] FIGHT       - Select attack moves", (float)bar.x + 24, (float)bar.y + 62, 16, (Color){215, 60, 45, 255});
        DrawCustomText("[2] POKEMON  - Switch active party member", (float)bar.x + 24, (float)bar.y + 92, 16, (Color){39, 174, 96, 255});
        DrawCustomText("Tip: Hover over any Pokemon or Move to inspect its full stats!", (float)bar.x + 24, (float)bar.y + 135, 13, (Color){110, 115, 125, 255});
    }
}
