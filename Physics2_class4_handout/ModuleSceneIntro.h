#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "p2Animation.h"

#define NUM_FLAMES 4

class PhysBody;
class b2RevoluteJoint;
class b2PrismaticJoint;
//class Animation;

enum sensorState
{
	active,
	inactive,
	error
};

struct activableNoSensor // for instantiate swapping frames multiple times
{
	SDL_Rect rect[2];
	iPoint positions[10]; // enables repetition print on different positions
	int instances = 0; // relationed with positions, if more than 10, increase positions index, fast workaround

};

struct activableSensors
{
	PhysBody* b = nullptr;
	SDL_Rect rect[2]; // active and inactive sprite
	sensorState state = sensorState::inactive;
	uint scoreToGain = 0;
	uint eventTime;
	uint totalTime;
};

enum game_loop
{
	START,
	INGAME,
	BLACK_HOLE,
	FAILURE,
	ENDGAME,
	HIGHSCORE
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
	bool generateStartBalls();
	bool shootBall();
	void setRestitution(PhysBody* body,float restitution = 0.0f);


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
	SDL_Texture* turboLogo_tex = nullptr;
	SDL_Texture* flames_tex = nullptr;
	Animation flamesAnim[NUM_FLAMES];
	SDL_Texture* scoreTypes_tex = nullptr;
	SDL_Texture* Ball_Lost_tex_1 = nullptr;
	SDL_Texture* Ball_Lost_tex_2 = nullptr;

	SDL_Texture* centerArrowsAnim_tex = nullptr;
	Animation centerArrowsAnim;

	SDL_Texture* TopHole_tex = nullptr;
	Animation TopHole; 

	// SFX indexes ----------------------------
	uint ding_sfx = 0;
	uint bumper_sfx = 0;
	uint rampBallKicked_sfx = 0;
	uint extraBall_sfx = 0;
	uint canon_sfx = 0;
	uint flipper_sfx = 0;

	uint bonus_fx;
	uint music; // convert music to ogg
	// ----------------------------------------

	// background chain physbodys
	//PhysBody* startLoopChain = nullptr;
	PhysBody* mainBoardChain = nullptr;
	PhysBody* exitLoopTapChain = nullptr;
	PhysBody* blackHoleCircle = nullptr;
	PhysBody* Next_To_Flipper_Chain_R = nullptr;
	PhysBody* Next_To_Flipper_Chain_L = nullptr;
	PhysBody* Ball_Safety_Chain = nullptr;
	PhysBody* ball_launcher_leftWall = nullptr;
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
	activableSensors topBallsSensors[3];
	p2List<activableSensors> topBallslist;
	
	// activables without sensor ie: called from anywhere
	activableNoSensor stars;


	// variables
	//
	bool Vacuum_Combo = true; 
	bool Inside_Vacuum_Flag = false;
	bool Vacuum_Combo_Flag = false; 
	

	uint Vacuum_Time = 0;
	uint Vacuum_Combo_Time = 0; 
	uint Ejected_Balls = 0; 
	uint Vacuum_Combo_Times = 1; 

	//
	bool Inside_Vacuum = false;
	uint Reset_Vacuum_Time = 0;
	bool Reset_Vacuum_Flag = false;
	PhysBody* Vacuum_Body = nullptr;

	uint starsCounter = 0;


	bool Switch_From_Hole_To_Ingame = false; 
	


	uint Active_Balls = 1; 
	bool Ball_Reset = false; 


	float Gravity_Force = 0; 
	bool Reset_Gravity = false; 
	PhysBody* Gravity_Body= nullptr;

	// CORE DATA ---------------------------------------------
	// SCORES for different places ----
	int vacuumScore = 10420;
	int topHoleScore = 16545;
	int topBallsScore = 6504;
	int bumperScore = 14320;
	int allSensorsActives = 152562;
	int lockScore = 34737;
	int allStarScore = 50650;
	uint Vacuum_Combo_Score = 500000;
	uint comboVaccumShootedBallScore = 6521;
	// vars
	//int numBaseBalls = 4; // counts the number of round start balls
	int inGameBalls = 0; // counts the actual balls on board

	// Rects
	SDL_Rect yourScoreRect;
	SDL_Rect highScoreRect;

	
	// lock functionality variables
	PhysBody* linkedBody = nullptr;
	bool claimNewBall = false;
	bool checkInactiveBalls = false;
	activableNoSensor lockLogo;
	bool automaticShoot = false;

	// ball launcher/kicker full functions vars || CORE
	int safetyPlateBalls = 0; // counts the remaining balls we have to launch
	SDL_Rect mainKickerRect;
	PhysBody* mainKicker_body = nullptr;
	PhysBody* mainKickerAnchorBody = nullptr;
	b2PrismaticJoint* kickerJoint = nullptr;
	bool preventyZone = false;
	PhysBody* preventyTrigger = nullptr;


	PhysBody* testBall = nullptr;

	// anims
	bool Play_Death = false; 
	uint Death_Score_Time = 0; 
	bool Finished_Death = false; 

};