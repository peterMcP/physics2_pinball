#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

enum game_loop
{
	START,
	INGAME,
	BLACK_HOLE,
	FAILURE,
	ENDGAME
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	 

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	//p2List<PhysBody*> startChain;
	p2List<PhysBody*> balls;
	p2List<PhysBody*> mainBoardComponents;

	SDL_Texture* circle = nullptr;
	SDL_Texture* box = nullptr;
	SDL_Texture* rick = nullptr;

	SDL_Texture* board_tex = nullptr;
	SDL_Texture* background_tex = nullptr;
	SDL_Texture* scoreboard_tex = nullptr;
	SDL_Texture* ball_tex = nullptr;
	SDL_Texture* second_layer_tex = nullptr;

	uint bonus_fx;
	uint music;     

	// background chain physbodys
	//PhysBody* startLoopChain = nullptr;
	PhysBody* mainBoardChain = nullptr;
	PhysBody* exitLoopTapChain = nullptr;
	PhysBody* blackHoleCircle = nullptr;
	PhysBody* Next_To_Flipper_Chain_1 = nullptr;
	PhysBody* Ball_Safety_Chain = nullptr;
	PhysBody* onlyLoopChain = nullptr;
	PhysBody* topDividerLeft = nullptr;
	PhysBody* topDividerRight = nullptr;

	// p2List<PhysBody*> Round_Objects; 

	// TRIGGERS
	PhysBody* exitLoopTrigger = nullptr;
	PhysBody* enterBoardTrigger = nullptr;
	PhysBody* Lose_Life_Trigger = nullptr;
	//bool to_delete = false;

	// SCENE LOOP
	game_loop scene_phase = game_loop::START;

	// Balls
	PhysBody* Current_Ball = nullptr; 
};
