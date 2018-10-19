#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void DrawBg(SDL_Texture* Background_Tex); 

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	//p2List<PhysBody*> startChain;

	SDL_Texture* circle = nullptr;
	SDL_Texture* box = nullptr;
	SDL_Texture* rick = nullptr;

	SDL_Texture* Background_Tex = nullptr;

	uint bonus_fx;

	// background physbodys
	PhysBody* startPhysBody = nullptr;
};
