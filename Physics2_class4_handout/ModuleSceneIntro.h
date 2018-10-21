#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "p2Animation.h"

class PhysBody;
class b2RevoluteJoint;
//class Animation;

enum sensorState
{
	active,
	inactive,
	error
};

struct activableSensors
{
	
	PhysBody* b = nullptr;
	SDL_Rect rect[2]; // active and inactive sprite
	sensorState state = sensorState::inactive;
	uint scoreToGain = 0;
};

enum game_loop
{
	START,
	INGAME,
	BLACK_HOLE,
	FAILURE,
	ENDGAME
};

enum ballState
{
	BLIT,
	DISAPPEAR
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

	bool restartBoard(); // player loose all balls, restart round
	bool newBall(); // sets the chains ready to next ball
	void DrawScore();


public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	//p2List<PhysBody*> startChain;
	p2List<PhysBody*> balls;
	p2List<PhysBody*> mainBoardComponents;

	SDL_Texture* circle = nullptr;
	SDL_Texture* box = nullptr;
	SDL_Texture* rick = nullptr;

	SDL_Texture* score_tex = nullptr;
	SDL_Rect scoreCutRect[10]; // we have 10 numbers on score texture
	SDL_Texture* sprites_tex = nullptr;
	SDL_Texture* board_tex = nullptr;
	SDL_Texture* background_tex = nullptr;
	SDL_Texture* scoreboard_tex = nullptr;
	SDL_Texture* ball_tex = nullptr;
	SDL_Texture* second_layer_tex = nullptr;
	SDL_Texture* leftFlipper_tex = nullptr;
	SDL_Texture* rightFlipper_tex = nullptr;

	SDL_Texture* centerArrowsAnim_tex = nullptr;
	Animation centerArrowsAnim;

	SDL_Texture* TopHole_tex = nullptr;
	Animation TopHole; 

	uint ding_sfx = 0;
	uint bumper_sfx = 0;
	uint rampBallKicked_sfx = 0;
	uint extraBall_sfx = 0;
	uint canon_sfx = 0;
	uint flipper_sfx = 0;


	uint bonus_fx;
	uint music; // convert music to ogg

	// background chain physbodys
	//PhysBody* startLoopChain = nullptr;
	PhysBody* mainBoardChain = nullptr;
	PhysBody* exitLoopTapChain = nullptr;
	PhysBody* blackHoleCircle = nullptr;
	PhysBody* Next_To_Flipper_Chain_R = nullptr;
	PhysBody* Next_To_Flipper_Chain_L = nullptr;
	PhysBody* Ball_Safety_Chain = nullptr;
	PhysBody* onlyLoopChain = nullptr;
	PhysBody* topDividerLeft = nullptr;
	PhysBody* topDividerRight = nullptr;
	PhysBody* Flipper_Chain_R = nullptr;
	PhysBody* Flipper_Chain_L= nullptr;
	PhysBody* anchorFlipperL = nullptr;
	PhysBody* anchorFlipperR = nullptr;
	PhysBody* Gravity_Zone_Chain = nullptr; 

	// p2List<PhysBody*> Round_Objects; 

	// TRIGGERS
	PhysBody* exitLoopTrigger = nullptr;
	PhysBody* enterBoardTrigger = nullptr;
	PhysBody* Lose_Life_Trigger = nullptr;
	PhysBody* leftBottomBouncerTrigger = nullptr;
	PhysBody* rightBottomBouncerTrigger = nullptr;
	PhysBody* leftSecurityKicker = nullptr;
	PhysBody* rightSecurityKicker = nullptr;
	PhysBody* Vacuum_Cleaner_Trigger = nullptr;
	PhysBody* Gravity_Zone_Trigger = nullptr;
	PhysBody* Extra_Ball_Trigger = nullptr;
	PhysBody* Inside_Hole_Trigger = nullptr;

	//bool to_delete = false;

	// SCENE LOOP
	game_loop scene_phase = game_loop::START;
	b2RevoluteJoint* flipper_joint_left = nullptr;
	b2RevoluteJoint* flipper_joint_right = nullptr;

	// Balls
	PhysBody* testCircle = nullptr;
	ballState ball_state = ballState::BLIT; 

	// activable sensors
	activableSensors sensor[19];
	p2List<activableSensors> sensor_list;


	// variables
	bool Inside_Vacuum = false;
	bool Inside_Vacuum_Flag = false;
	uint Vacuum_Time = 0; 
	uint Reset_Vacuum_Time = 0; 
	bool Reset_Vacuum_Flag = false;

	bool Switch_From_Hole_To_Ingame = false; 
	
};
