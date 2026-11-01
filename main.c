#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

int minnn(int a, int b) {
	if(a<b) return a;
	else return b;
}

#define MAX_MESSAGES 30
#define MESSAGE_SIZE 256
#define MESSAGE_DURATION 1.5

char messageBuffer[MAX_MESSAGES][MESSAGE_SIZE];

int messageCount = 0;
int currentMessage = 0;

double messageStartTime = 0.0;
int messageTimerStarted = 0;

void AddBattleMessage(const char *format, ...)
{
    if (messageCount >= MAX_MESSAGES)
        return;

    va_list arguments;

    va_start(arguments, format);

    vsnprintf(
        messageBuffer[messageCount],
        MESSAGE_SIZE,
        format,
        arguments
    );

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
}void DrawWrappedText(
    const char *text,
    Rectangle area,
    int fontSize,
    int lineSpacing,
    Color color
) {
    char line[512] = "";
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
                char testLine[512];

                if (line[0] == '\0')
                    snprintf(testLine, sizeof(testLine), "%s", word);
                else
                    snprintf(testLine, sizeof(testLine), "%s %s", line, word);

                if (MeasureText(testLine, fontSize) <= area.width) {
                    snprintf(line, sizeof(line), "%s", testLine);
                }
                else {
                    DrawText(
                        line,
                        (int)area.x,
                        (int)y,
                        fontSize,
                        color
                    );

                    y += fontSize + lineSpacing;

                    snprintf(line, sizeof(line), "%s", word);
                }
            }

            wordLength = 0;

            if (c == '\n') {
                DrawText(
                    line,
                    (int)area.x,
                    (int)y,
                    fontSize,
                    color
                );

                line[0] = '\0';
                y += fontSize + lineSpacing;
            }

            if (c == '\0')
                break;
        }
    }

    if (line[0] != '\0') {
        DrawText(
            line,
            (int)area.x,
            (int)y,
            fontSize,
            color
        );
    }
}
void DrawBattleTextBox(void)
{
    if (currentMessage >= messageCount)
        return;

    float boxWidth =
        GetScreenWidth() / 2.0f - 40;

    float boxHeight =
        GetScreenHeight() / 2.0f - 40;

    Rectangle bar = {
        20,
        333,
        boxWidth,
        boxHeight
    };

    DrawRectangleRec(
        bar,
        (Color){245, 245, 235, 255}
    );

    DrawRectangleLinesEx(
        bar,
        4,
        BLACK
    );

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
//             NOR FIR WAT GRA ELE ICE FIG POI GRO FLY PSY BUG ROC GHO DRA DAR STE FAI
const int type_chart[18][18] = {
/*NORMAL  */ { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  5,  0, 10, 10,  5, 10 },
/*FIRE    */ { 10,  5,  5, 20, 10, 20, 10, 10, 10, 10, 10, 20,  5, 10,  5, 10, 20, 10 },
/*WATER   */ { 10, 20,  5,  5, 10, 10, 10, 10, 20, 10, 10, 10, 20, 10,  5, 10, 10, 10 },
/*GRASS   */ { 10,  5, 20,  5, 10, 10, 10,  5, 20,  5, 10,  5, 20, 10,  5, 10,  5, 10 },
/*ELECTRIC*/ { 10, 10, 20,  5,  5, 10, 10, 10,  0, 20, 10, 10, 10, 10,  5, 10, 10, 10 },
/*ICE     */ { 10,  5,  5, 20, 10,  5, 10, 10, 20, 20, 10, 10, 10, 10, 20, 10,  5, 10 },
/*FIGHTING*/ { 20, 10, 10, 10, 10, 20, 10,  5, 10,  5,  5,  5, 20,  0, 10, 20, 20,  5 },
/*POISON  */ { 10, 10, 10, 20, 10, 10, 10,  5,  5, 10, 10, 10,  5,  5, 10, 10,  0, 20 },
/*GROUND  */ { 10, 20, 10,  5, 20, 10, 10, 20, 10,  0, 10,  5, 20, 10, 10, 10, 20, 10 },
/*FLYING  */ { 10, 10, 10, 20,  5, 10, 20, 10, 10, 10, 10, 20,  5, 10, 10, 10,  5, 10 },
/*PSYCHIC */ { 10, 10, 10, 10, 10, 10, 20, 20, 10, 10,  5, 10, 10, 10, 10,  0,  5, 10 },
/*BUG     */ { 10,  5, 10, 20, 10, 10,  5,  5, 10,  5, 20, 10, 10,  5, 10, 20,  5,  5 },
/*ROCK    */ { 10, 20, 10, 10, 10, 20,  5, 10,  5, 20, 10, 20, 10, 10, 10, 10,  5, 10 },
/*GHOST   */ { 0,  10, 10, 10, 10, 10, 10, 10, 10, 10, 20, 10, 10, 20, 10,  5, 10, 10 },
/*DRAGON  */ { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 20, 10,  5,  0 },
/*DARK    */ { 10, 10, 10, 10, 10, 10,  5, 10, 10, 10, 20, 10, 10, 20, 10,  5,  10,  5 },
/*STEEL   */ { 10,  5,  5, 10,  5, 20, 10, 10, 10, 10, 10, 10, 20, 10, 10, 10,  5, 20 },
/*FAIRY   */ { 10,  5, 10, 10, 10, 10, 20,  5, 10, 10, 10, 10, 10, 10, 20, 20,  5, 10 },
};

typedef struct Pokemon Pokemon;

typedef enum{
	TYPE_NONE,
	TYPE_NORMAL,
	TYPE_FIRE,
	TYPE_WATER,
	TYPE_GRASS,
	TYPE_ELECTRIC,
	TYPE_ICE,
	TYPE_FIGHTING,
	TYPE_POISON,
	TYPE_GROUND,
	TYPE_FLYING,
	TYPE_PSYCHIC,
	TYPE_BUG,
	TYPE_ROCK,
	TYPE_GHOST,
	TYPE_DRAGON,
	TYPE_DARK,
	TYPE_STEEL,
	TYPE_FAIRY
} ElementalType;

typedef enum{
	CATEGORY_PHYSICAL,
	CATEGORY_SPECIAL,
	CATEGORY_STATUS
} MoveCategory;

typedef enum{
	STATUS_NONE,
	STATUS_POISON,
	STATUS_BURN,
	STATUS_PARALYSIS,
	STATUS_SLEEP,
	STATUS_FREEZE
} StatusEffect;
const char *GetStatusText(StatusEffect effect){
	switch(effect){
		case STATUS_POISON: return "PSN";
		case STATUS_BURN: return "BRN";
		case STATUS_PARALYSIS: return "PAR";
		case STATUS_SLEEP: return "SLP";
		case STATUS_FREEZE: return "FRZ";
		default: return "";
	}
}
typedef struct move move;
struct move{
	int BP;
	int Accuracy; 
	int Max_PP; 
	int Current_PP;
	char Name[100];
	ElementalType type;
	MoveCategory category;
	StatusEffect effect; 
	int probability; 

};
struct Pokemon {
	char Name[100];
	int max_health;
	int current_health;
	int max_speed;
	int speed;
	int max_defense;
	int defense;
	int max_attack;
	int attack;
	int special_attack;
	int special_defense;
	ElementalType type1;
	ElementalType type2;
	//evasiveness and accuracy are in-battle stats (always at 100% at start of match);
	int evasiveness;
	int accuracy;
	int max_evasiveness;
	int max_accuracy;
	move Moves[4];
	int level;
	StatusEffect effect;
	int status_conditions_rounds_left;
};
typedef struct BattleContext context;

struct BattleContext{
	Pokemon a;
	Pokemon b;
	move *player_a_active_move;
	move *player_b_active_move;
};

void turn_processing(Pokemon *A, Pokemon *B, move *MoveA);
double type_effectiveness(ElementalType typeA, ElementalType typeB)
{
    if (typeA == TYPE_NONE)
        return 0.0;

    if (typeB == TYPE_NONE)
        return 1.0;

    return type_chart[typeA - 1][typeB - 1] / 10.0;
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
int special_damage_calculator(Pokemon *pokemon,Pokemon *pokemonB, move *Move, double random_factor)
{
	if (pokemonB->special_defense <= 0) pokemonB->special_defense = 1;
    int damage = (int)
        (((((2 * pokemon->level) / 5.0 + 2)
        * Move->BP
        * pokemon->special_attack
        / pokemonB->special_defense) / 50.0 + 2)
       	* ((pokemon->type1 == Move->type || pokemon->type2 == Move->type) ? 1.5 : 1.0)
        * type_effectiveness(Move->type, pokemonB->type1)
		* type_effectiveness(Move->type, pokemonB->type2)
        * random_factor)
    ;

    return damage;
}

int get_move_weight(move Move, Pokemon target)
{
	int weight = 0;

    if (Move.Current_PP <= 0) {
        return 0;
    }

    // Prevent invalid array index for TYPE_NONE
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
	if(Move.effect != STATUS_NONE && target.effect == STATUS_NONE){
		weight += 70;
		//zero truncation!!!
		weight = (weight*Move.probability)/100;
	}
	weight = (weight * (100-(rand() % 25)))/100;
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
void status_effect_processing(Pokemon *A, Pokemon *B){
	if(A->effect == STATUS_BURN){
		A->current_health -= A->max_health/16;
		AddBattleMessage("%s: %d Damage taken from BURN effect\n", A->Name, A->max_health/16);
	}
	if(A->effect == STATUS_POISON){
		int dam = (16-A->status_conditions_rounds_left)*A->max_health/16;
		if(A->status_conditions_rounds_left > 0)
			A->current_health -= dam;
		else
			A->current_health -= 15 * A->max_health/16;
		AddBattleMessage("%s: %d Damage taken from POISON effect\n", A->Name, dam);
	}
	if(A->effect == STATUS_SLEEP){
		AddBattleMessage("%s is still sleeping.\n", A->Name);
		if(A->status_conditions_rounds_left <= 0){
			A->effect = STATUS_NONE;
			AddBattleMessage("%s woke up!\n", A->Name);
		}
	}
	if(A->effect == STATUS_PARALYSIS){
		AddBattleMessage("%s is still paralyzed!\n", A->Name);
		A->speed = (75 * A->max_speed)/100;
	}
	if(A->effect == STATUS_FREEZE){
		int roll = rand() % 256;
		if(roll <= 25){
			AddBattleMessage("%s thawed and can move again!\n", A->Name);
			A->effect = STATUS_NONE;
		}else{
			AddBattleMessage("%s is still frozen and can't do anything\n", A->Name);
		}
	}
	A->status_conditions_rounds_left -= 1;
}

void turn_processing(Pokemon *A, Pokemon *B, move *MoveA)
{
	if(!(A->effect == STATUS_SLEEP || (A->effect == STATUS_PARALYSIS && rand() % 100 < 25) || A->effect == STATUS_FREEZE)){

		if (MoveA->Current_PP <= 0) {
			AddBattleMessage("%s has no PP left!\n", MoveA->Name);
			return;
		}
		
		AddBattleMessage("%s used %s!\n", A->Name, MoveA->Name);
		
		MoveA->Current_PP--;
		if (B->evasiveness <= 0) B->evasiveness = 1;
		int accuracy = MoveA->Accuracy 	* A->accuracy / B->evasiveness;
		
		if (accuracy > 100)
        accuracy = 100;
		
		if (rand() % 100 >= accuracy) {
			AddBattleMessage("%s's attack missed!\n", A->Name);
			return;
		}
		
		double random_factor =
        ((rand() % 39) + 217) / 255.0;
		
		int damage = 0;
		
		if (MoveA->category == CATEGORY_PHYSICAL) {
			damage = damage_calculator(A, B, MoveA, random_factor);
			move_status_effect_processing(A,B,MoveA);
		}
		else if (MoveA->category == CATEGORY_SPECIAL) {
			damage = special_damage_calculator(A, B, MoveA, random_factor);
			move_status_effect_processing(A,B,MoveA);
		}
		else {
			//AddBattleMessage("Status moves are not implemented yet.\n");
			//implement status calculaations
			move_status_effect_processing(A,B,MoveA);
			return;
		}
		
		AddBattleMessage("%s took %d damage.\n", B->Name, minnn(B->current_health, damage));

		B->current_health -= damage;
		
		if (B->current_health < 0)
        B->current_health = 0;
		
		
		double effectiveness =
        type_effectiveness(MoveA->type, B->type1) * type_effectiveness(MoveA->type, B->type2);
		
		if (effectiveness > 1.0)
        AddBattleMessage("It was super effective!\n");
		else if (effectiveness == 0.0)
        AddBattleMessage("It had no effect!\n");
		else if (effectiveness < 1.0)
        AddBattleMessage("It was not very effective!\n");
	}

}
int main(){
	srand(time(NULL));
	
	move Vine_Whip = {.Name = "Vine Whip", .BP = 45, .Accuracy = 100, .Max_PP = 25, .Current_PP = 25, .type = TYPE_GRASS, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE};
	move Razor_Leaf = {.Name = "Razor Leaf", .BP = 55, .Accuracy = 95, .Max_PP = 25, .Current_PP = 25, .type = TYPE_GRASS, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE};
	move Sleep_Powder = {.Name = "Sleep Powder", .BP = 0, .Accuracy = 75, .Max_PP = 15, .Current_PP = 15, .type = TYPE_GRASS, .category = CATEGORY_STATUS, .effect = STATUS_SLEEP, 100};
	move Poison_Powder = {.Name = "Poison Powder", .BP = 0, .Accuracy = 75, .Max_PP = 35, .Current_PP = 35, .type = TYPE_POISON, .category = CATEGORY_STATUS, .effect = STATUS_POISON, 100};
	move Flamethrower = {.Name = "Flamethrower", .BP = 90, .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_FIRE, .category = CATEGORY_SPECIAL, .effect = STATUS_NONE};
	move Wing_Attack = {.Name = "Wing Attack", .BP = 60, .Accuracy = 100, .Max_PP = 35, .Current_PP = 35, .type = TYPE_FLYING, .category = CATEGORY_PHYSICAL, .effect = STATUS_BURN, 10};
	move Metal_Claw = {.Name = "Metal Claw", .BP = 50, .Accuracy = 95, .Max_PP = 35, .Current_PP = 35, .type = TYPE_STEEL, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE};
	move Slash = {.Name = "Slash", .BP = 70, .Accuracy = 100, .Max_PP = 20, .Current_PP = 20, .type = TYPE_NORMAL, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE};
	move Crunch = {.Name = "Crunch", .BP = 80, .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_DARK, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE};
	move Dragon_Claw = {.Name = "Dragon Claw", .BP = 80, .Accuracy = 100, .Max_PP = 15, .Current_PP = 15, .type = TYPE_DRAGON, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE};
	move ExtremeSpeed = {.Name = "Extreme Speed", .BP = 80, .Accuracy = 100, .Max_PP = 5, .Current_PP = 5, .type = TYPE_NORMAL, .category = CATEGORY_PHYSICAL, .effect = STATUS_NONE};

	Pokemon Venusaur = {
		.Name = "Venusaur", .max_health = 364, .current_health = 364, .max_speed = 284, .speed = 284,
		.max_defense = 291, .defense = 291, .max_attack = 289, .attack = 289, .special_attack = 328, .special_defense = 328,
		.type1 = TYPE_GRASS,.type2 = TYPE_POISON, .evasiveness = 100, .accuracy = 100, .max_evasiveness = 100, .max_accuracy = 100,
		.Moves = {Vine_Whip, Razor_Leaf, Sleep_Powder, Poison_Powder}, .level = 100, .effect = STATUS_NONE, .status_conditions_rounds_left = 0
	};
	Pokemon Charizard = {
		.Name = "Charizard", .max_health = 360, .current_health = 360, .max_speed = 328, .speed = 328,
		.max_defense = 280, .defense = 280, .max_attack = 293, .attack = 293, .special_attack = 348, .special_defense = 295,
		.type1 = TYPE_FIRE, .type2 = TYPE_FLYING, .evasiveness = 100, .accuracy = 100, .max_evasiveness = 100, .max_accuracy = 100,
		.Moves = {Flamethrower, Wing_Attack, Metal_Claw, Slash}, .level = 100, .effect = STATUS_NONE, .status_conditions_rounds_left = 0
	};

	Pokemon Rayquaza = {
		.Name = "Rayquaza", .max_health = 414, .current_health = 414, .max_speed = 317, .speed = 317,
		.max_defense = 306, .defense = 306, .max_attack = 438, .attack = 438, .special_attack = 438, .special_defense = 306,
		.type1 = TYPE_DRAGON, .type2 = TYPE_FLYING, .evasiveness = 100, .accuracy = 100, .max_evasiveness = 100, .max_accuracy = 100,
		.Moves = {Flamethrower, Dragon_Claw, ExtremeSpeed, Crunch}, .level = 100,  .effect = STATUS_NONE, .status_conditions_rounds_left = 0
	};

	context contxt;
	contxt.a = Venusaur;
	contxt.b = Rayquaza;
	//announce enemy pokemon
	AddBattleMessage("Enemy pokemon appeared\n");
	//announce player turn.
	//AddBattleMessage("Go! %s\n", contxt.a.Name);
	InitWindow(800, 450, "Pokemon Battle");
	SetTargetFPS(60);

	Texture2D aSprite = LoadTexture("images/venusaur_back.png");

	Texture2D bSprite = LoadTexture("images/rayquaza_front.png");

	int selectedMove = 0;

	int displayedPlayerHP = contxt.a.current_health;
	int displayedEnemyHP = contxt.b.current_health;

	double lastHpUpdate = 0;
	int hpAnimationStage = 0;
	int nextHpAnimationStage = 0;

	while (!WindowShouldClose()) {

		int messageActive = UpdateBattleMessages();
		if (!messageActive){
			
			int hpAnimating = displayedPlayerHP != contxt.a.current_health || displayedEnemyHP != contxt.b.current_health;

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
				int bestEnemyMove = 0;
				int bestWeight = get_move_weight(contxt.b.Moves[0], contxt.a);
				for (int i = 1; i < 4; i++) {
					int weight =
						get_move_weight(contxt.b.Moves[i], contxt.a);
					if (weight > bestWeight) {
						bestWeight = weight;
						bestEnemyMove = i;
					}
				}
				contxt.player_b_active_move = &contxt.b.Moves[bestEnemyMove];

				if (contxt.a.speed >= contxt.b.speed) {
						turn_processing(&contxt.a, &contxt.b, contxt.player_a_active_move);

					if (contxt.b.current_health > 0) {
						turn_processing(&contxt.b, &contxt.a, contxt.player_b_active_move);
					}
				}
				else {
					turn_processing(&contxt.b, &contxt.a, contxt.player_b_active_move);

					if (contxt.a.current_health > 0) {
						turn_processing( &contxt.a, &contxt.b, contxt.player_a_active_move);
					}
				}

				if (contxt.a.current_health > 0 && contxt.b.current_health > 0) {

					status_effect_processing(&contxt.a, &contxt.b);
					status_effect_processing(&contxt.b, &contxt.a);
				}

				if (contxt.a.current_health < 0) contxt.a.current_health = 0;

				if (contxt.b.current_health < 0) contxt.b.current_health = 0;

				if (contxt.a.speed >= contxt.b.speed) {
					hpAnimationStage = 1;       // enemy HP first
					nextHpAnimationStage = 2;   // then player HP
				}
				else {
					hpAnimationStage = 2;       // player HP first
					nextHpAnimationStage = 1;   // then enemy HP
				}
			}

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

		//textbox
		DrawBattleTextBox();

		DrawTextureEx(aSprite, (Vector2){90, 190}, 0.0f, 2.0f, WHITE);
		DrawTextureEx(bSprite, (Vector2){560, 40},0.0f, 2.0f, WHITE);

		DrawText(TextFormat("%s  Lv.%d", contxt.b.Name, contxt.b.level),50, 40, 25, BLACK);
		//DrawText(contxt.b.Name, 50, 40, 25, BLACK);
		if(contxt.b.effect != STATUS_NONE){
			DrawText(GetStatusText(contxt.b.effect), 150 + MeasureText(contxt.b.Name, 25) + 10, 43, 18, GRAY);
		}
		DrawText(TextFormat("HP: %d/%d", displayedEnemyHP, contxt.b.max_health),50, 70, 22, BLACK);

		/* Player real stats */
		DrawText(TextFormat("%s  Lv.%d",contxt.a.Name,contxt.a.level),500, 240, 25, BLACK);
		if(contxt.a.effect != STATUS_NONE){
			DrawText(GetStatusText(contxt.a.effect), 600 + MeasureText(contxt.a.Name, 25) + 10, 243, 18, GRAY);	
		}
		DrawText(TextFormat("HP: %d/%d",displayedPlayerHP, contxt.a.max_health),500, 270, 22, BLACK);

		Vector2 positions[4] = {
			{400, 330},
			{590, 330},
			{400, 385},
			{590, 385}
		};

		for (int i = 0; i < 4; i++) {

			int x = positions[i].x;
			int y = positions[i].y;

			if (selectedMove == i)
				DrawRectangle(x, y, 180, 45, LIGHTGRAY);

			DrawRectangleLines(x, y, 180, 45, BLACK);

			DrawText(
				TextFormat("%d. %s",
						i + 1,
						contxt.a.Moves[i].Name),
				x + 10, y + 5, 18, BLACK
			);

			DrawText(
				TextFormat("PP: %d/%d",
						contxt.a.Moves[i].Current_PP,
						contxt.a.Moves[i].Max_PP),
				x + 10, y + 27, 12, DARKGRAY
			);
		}

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

