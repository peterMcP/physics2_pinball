#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "p2Animation.h"
#include "time.h"
// chains pivots header
#include "chainsPivots.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	srand(time(NULL));          
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	// board textures ---
	board_tex = App->textures->Load("pinball/board.png");
	background_tex = App->textures->Load("pinball/backgroundWallpaper.png");
	scoreboard_tex = App->textures->Load("pinball/scoreboard.png");
	second_layer_tex = App->textures->Load("pinball/secondLayer.png");
	// animation textures
	centerArrowsAnim_tex = App->textures->Load("pinball/centerArrowsAnim.png");
	TopHole_tex = App->textures->Load("pinball/Theblackhole.png"); 


	// assets textures
	ball_tex = App->textures->Load("pinball/ball.png");
	leftFlipper_tex = App->textures->Load("pinball/L_Flipper.png");
	rightFlipper_tex = App->textures->Load("pinball/R_Flipper.png");
	sprites_tex = App->textures->Load("pinball/sprites.png");
	score_tex = App->textures->Load("pinball/font.png");
	turboLogo_tex = App->textures->Load("pinball/turboLogo.png");
	flames_tex = App->textures->Load("pinball/flames2.png");
	scoreTypes_tex = App->textures->Load("pinball/scores2.png");
	BlackArrowsGravity_tex = App->textures->Load("pinball/BlackArrows.png");
	bigArrows_tex = App->textures->Load("pinball/bigArrowsAnim2.png");
	
	// audio
	// music = App->audio->LoadFx("pinball/audio/soundtrack.wav");    // music as a Fx, so that it plays many times 
	// App->audio->PlayFx(1, -1); 
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	ding_sfx = App->audio->LoadFx("pinball/audio/SFX/6_ding.wav");
	bumper_sfx = App->audio->LoadFx("pinball/audio/SFX/10_bumper.wav");
	rampBallKicked_sfx = App->audio->LoadFx("pinball/audio/SFX/9_rampBallKicked.wav");
	extraBall_sfx = App->audio->LoadFx("pinball/audio/SFX/2_extraBall.wav");
	canon_sfx = App->audio->LoadFx("pinball/audio/SFX/7_canon.wav");
	flipper_sfx = App->audio->LoadFx("pinball/audio/SFX/8_flipper.wav");

	// -----------------------------------------------------------------------------------
	// create background chains ---
	onlyLoopChain = App->physics->CreateChain(0, 18, loopPartPoints, 120, false, false);
	Ball_Safety_Chain = App->physics->CreateChain(0, 18, safetyZonePoints, 28, false, false);
	ball_launcher_leftWall = App->physics->CreateChain(0, 18, safetyZoneP2, 4, false, false);
	
	// TRIGGERS/SENSORS
	// 364,129,8,8
	exitLoopTrigger = App->physics->CreateRectangleSensor(372, 140, 8, 8);
	exitLoopTrigger->listener = this;
	Lose_Life_Trigger = App->physics->CreateRectangleSensor(216, 536, 60, 5);
	Lose_Life_Trigger->listener = this;

	// generate start balls
	generateStartBalls();

	// ADD MAIN COMPONENTS TO LIST, its components are static and never change
	mainBoardComponents.add(App->physics->CreateChain(0, 18, topSeparatorPoints, 12, false, true));
	mainBoardComponents.add(App->physics->CreateChain(37, 18, topSeparatorPoints, 12, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, bottomUpWallL, 16, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, bottomUpWallR, 16, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, rightBottomWayPoints, 28, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, leftBottomWayPoints, 22, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, turboCompressorPoints, 32, false, false));

	// ADD SPECIAL COMPONENTS ------------------------------------------------------------
	// FLIPPERS ----------------------------------------------------

	Flipper_Chain_R = App->physics->CreateConvexPoly(226, 474, new_R_Flipper, 16, true, false, 50.0f);
	Flipper_Chain_L = App->physics->CreateConvexPoly(150, 474, new_L_Flipper, 16, true, false, 50.0f);

	// add anchor circles to stick the flippers center point of rotation
	anchorFlipperL = App->physics->CreateCircle(158, 484, 3, false);
	anchorFlipperR = App->physics->CreateCircle(272, 484, 3, false);

	// adding revolution joint and motor to flipper - TEST, implement on setjoints function
	b2RevoluteJointDef jointDef;
	jointDef.bodyB = Flipper_Chain_L->body; //testCircle->body;
	jointDef.bodyA = anchorFlipperL->body;
	jointDef.Initialize(jointDef.bodyA, jointDef.bodyB, jointDef.bodyA->GetWorldCenter());
	jointDef.lowerAngle = -0.1f * b2_pi; //
	jointDef.upperAngle = 0.1f * b2_pi; //
	jointDef.enableLimit = true;
	jointDef.maxMotorTorque = 1000.0f;
	jointDef.motorSpeed = -20.0f;
	//jointDef.enableMotor = true;
	flipper_joint_left = App->physics->SetJoint(&jointDef);
	// right flipper joint test -------------
	jointDef.bodyB = Flipper_Chain_R->body;
	jointDef.bodyA = anchorFlipperR->body;
	jointDef.Initialize(jointDef.bodyA, jointDef.bodyB, jointDef.bodyA->GetWorldCenter());
	//jointDef.lowerAngle = -0.1f * b2_pi;//-0.1f * b2_pi; //
	//jointDef.upperAngle = 0.1f * b2_pi; //
	//jointDef.enableLimit = true;
	//jointDef.maxMotorTorque = 1000.0f;
	jointDef.motorSpeed = 20.0f;
	flipper_joint_right = App->physics->SetJoint(&jointDef);

	// --------------------------------------------------------------------------------------------
	// TRIGGERS / etc
	leftBottomBouncerTrigger = App->physics->CreateRectangleSensor(154, 423, 30, 5, 1.1f);
	rightBottomBouncerTrigger = App->physics->CreateRectangleSensor(278, 423, 30, 5, -1.1f);

	// top bouncing balls -----
	// sensors
	int t = 100;
	topBallsSensors[0].b = App->physics->CreateCircle(217, 186, 12, false, 1.0f, 1.0f);
	topBallsSensors[0].totalTime = t;
	topBallslist.add(topBallsSensors[0]);
	topBallsSensors[1].b = App->physics->CreateCircle(258, 221, 12, false, 1.0f, 1.0f);
	topBallsSensors[1].totalTime = t;
	topBallslist.add(topBallsSensors[1]);
	topBallsSensors[2].b = App->physics->CreateCircle(174, 221, 12, false, 1.0f, 1.0f);
	topBallsSensors[2].totalTime = t;
	topBallslist.add(topBallsSensors[2]);

	// make it sensors and push rects
	p2List_item<activableSensors>* topBallItem = topBallslist.getFirst();
	while (topBallItem != NULL)
	{
		// "push" rects
		topBallItem->data.rect[inactive] = { 0,0,0,0 };
		topBallItem->data.rect[active] = { 28,108,28,28 };
		b2Fixture* f = topBallItem->data.b->body->GetFixtureList(); // only first fixture
		f->SetSensor(true);
		topBallItem = topBallItem->next;
	}
	// normal body
	circles.add(App->physics->CreateCircle(217, 186, 10, false, 1.0f, 1.0f));
	circles.add(App->physics->CreateCircle(258, 221, 10, false, 1.0f, 1.0f));
	circles.add(App->physics->CreateCircle(174, 221, 10, false, 1.0f, 1.0f));

	// activable sensors --------------------------------------------------------------------------
	// CIRCULAR grey SPRITES
	for (int i = 0; i < 14; ++i) // "circular grey sprites" 14 in total, same rects,score etc
	{
		sensor[i].rect[active] = { 23,55,19,19 };
		sensor[i].rect[inactive] = { 67,54,19,19 };
		sensor[i].scoreToGain = 10000;
	}

	int i = 0;
	// top left
	sensor[i].b = App->physics->CreateRectangleSensor(124, 167, 10, 5, -0.4f);
	sensor_list.add(sensor[i++]);
	sensor[i].b = App->physics->CreateRectangleSensor(140, 160, 10, 5, -0.4f);
	sensor_list.add(sensor[i++]);
	sensor[i].b = App->physics->CreateRectangleSensor(157, 153, 10, 5, -0.4f);
	sensor_list.add(sensor[i++]);
	// top right
	sensor[i].b = App->physics->CreateRectangleSensor(270, 153, 10, 5, 0.4f);
	sensor_list.add(sensor[i++]);
	sensor[i].b = App->physics->CreateRectangleSensor(287, 160, 10, 5, 0.4f);
	sensor_list.add(sensor[i++]);
	sensor[i].b = App->physics->CreateRectangleSensor(304, 167, 10, 5, 0.4f);
	sensor_list.add(sensor[i++]);
	// mid
	sensor[i].b = App->physics->CreateRectangleSensor(193, 247, 10, 5, 0.0f);
	sensor_list.add(sensor[i++]);
	sensor[i].b = App->physics->CreateRectangleSensor(212, 244, 10, 5, 0.0f);
	sensor_list.add(sensor[i++]);
	sensor[i].b = App->physics->CreateRectangleSensor(231, 247, 10, 5, 0.0f);
	sensor_list.add(sensor[i++]);
	// bottom

	// up stars, only 3
	stars.instances = 3;
	stars.positions[0] = { 172, 104 };
	stars.positions[1] = { 208, 98 };
	stars.positions[2] = { 246, 104 };
	
	stars.rect[active] = { 0,0,16,16 };
	stars.rect[inactive] = { 43,48,16,16 };

	// Lock functionality "animated" sprite graphic
	lockLogo.instances = 1;
	lockLogo.positions[0] = { 309,260 };
	lockLogo.rect[active] = { 22,0,44,44 };
	lockLogo.rect[inactive] = { 73,103,44,44 };

	// gravity burn logo
	gravityLogo.instances = 1;
	gravityLogo.positions[0] = { 81, 265 };
	gravityLogo.rect[active] = { 104,0,45,60 };
	gravityLogo.rect[inactive] = { 149,0,45,60 };

	// gravity hole layer trick
	gravityLayerTexTrickRect = { 56,3,74,110 };

	// ball lost indicator logo
	ballLostLogo.totalTime = 350;
	ballLostLogo.rect[inactive] = { 31,79,67,21 };
	ballLostLogo.rect[active] = { 102,79,67,21 };

	// -----------------------------------------------------------------------------------------------

	// MAIN kicker
	// rect
	mainKickerRect = { 0,59,20,76 };
	mainKicker_body = App->physics->CreateRectangle(402, 441, 20, 20);
	//mainKicker_body->body->SetType(b2_staticBody);
	mainKickerAnchorBody = App->physics->CreateRectangle(402, 463, 20, 20);
	mainKickerAnchorBody->body->SetType(b2_staticBody);
	// create prismatic joint --------
	b2PrismaticJointDef prismJointDef;
	b2Vec2 worldAxis(0.0f, 1.0f);
	prismJointDef.Initialize(mainKickerAnchorBody->body,mainKicker_body->body, mainKicker_body->body->GetWorldCenter(), worldAxis);
	prismJointDef.lowerTranslation = PIXEL_TO_METERS(0.0f);
	prismJointDef.upperTranslation = PIXEL_TO_METERS(22.0f);
	prismJointDef.enableLimit = true;
	prismJointDef.maxMotorForce = 500.0f;
	prismJointDef.motorSpeed = -60.0f;
	//prismJointDef.enableMotor = true;
	kickerJoint = (b2PrismaticJoint*)App->physics->world->CreateJoint(&prismJointDef);
	// --------------------------------
	// creates preventy trigger
	preventyTrigger = App->physics->CreateRectangleSensor(392, 440, 4, 20);

	// other special sensors
	Vacuum_Cleaner_Trigger = App->physics->CreateRectangleSensor(216, 299, 26, 16); 
	Extra_Ball_Trigger = App->physics->CreateRectangleSensor(370, 242, 6, 12, -0.8f);
	Gravity_Zone_Trigger = App->physics->CreateRectangleSensor(45, 200, 26, 12);
    //Inside_Hole_Trigger = App->physics->CreateRectangleSensor(65, 161, 10, 10);
	Inside_Hole_Trigger = App->physics->CreateCircle(64, 161, 12, false);
	b2Fixture* ihtf = Inside_Hole_Trigger->body->GetFixtureList();
	ihtf->SetSensor(true);

	// ANIMATIONS
	for (int i = 9; i >= 0; --i)
		centerArrowsAnim.PushBack({ i * 15, 0, 15, 135});
	centerArrowsAnim.PushBack({ 0, 0, 0, 0 });
	centerArrowsAnim.PushBack({ 0, 0, 0, 0 });
	centerArrowsAnim.speed = 0.15f;

	for (int i = 3; i >= 0; --i)
		rightArrowsAnim.PushBack({ i * 15, 0, 15, 135 });
	rightArrowsAnim.PushBack({ 0, 0, 0, 0 });
	rightArrowsAnim.PushBack({ 0, 0, 0, 0 });
	rightArrowsAnim.speed = 0.15f;

	for (int i = 0; i<=39; ++i)
		TopHole.PushBack({ i*28, 1, 28, 28 });
	TopHole.speed = 0.0f;
	
	TopHole.loop = false; 

	// flames animation
	for (int i = 0; i < NUM_FLAMES; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			flamesAnim[i].PushBack({ j * 100 ,0, 100,100 });
			flamesAnim[i].speed = 0.25f;
		}
	}

	// SCORETYPES RECTs
	yourScoreRect = { 0,0,223,72 };
	highScoreRect = { 0,77,223,78 };
	// lateral kickers "sensors" rect
	kickerLeft.b = App->physics->CreateRectangleSensor(97, 420, 10, 10, 0.0f);
	kickerLeft.rect[inactive] = { 0,0,0,0 };
	kickerLeft.rect[active] = { 128,100,37,54 };
	kickerLeft.scoreToGain = 12001;
	kickerLeft.totalTime = 150;
	kickerRight.b = App->physics->CreateRectangleSensor(334, 420, 10, 10, 0.0f);
	kickerRight.rect[inactive] = { 0,0,0,0 };
	kickerRight.rect[active] = { 128,100,37,54 };
	kickerRight.scoreToGain = 12001;
	kickerRight.totalTime = 150;

	// big arrows animation --------------------------------
	// left
	leftArrowsOffRect = { 267,272,89,136 };
	bigLeftArrowsAnim.PushBack({ 178,272,89,136 });
	bigLeftArrowsAnim.PushBack({ 0,0,0,0 }); // dummy frames
	bigLeftArrowsAnim.PushBack({ 0,0,0,0 });
	bigLeftArrowsAnim.PushBack({ 89,272,89,136 });
	bigLeftArrowsAnim.PushBack({ 0,272,89,136 });
	bigLeftArrowsAnim.PushBack({ 0,0,0,0 });
	bigLeftArrowsAnim.PushBack({ 0,0,0,0 });
	bigLeftArrowsAnim.PushBack({ 356,136,89,136 });
	bigLeftArrowsAnim.PushBack({ 267,136,89,136 });
	bigLeftArrowsAnim.PushBack({ 178,136,89,136 });
	bigLeftArrowsAnim.PushBack({ 0,0,0,0 });
	bigLeftArrowsAnim.PushBack({ 0,0,0,0 });
	bigLeftArrowsAnim.speed = 0.15f;
	// right
	rightArrowsOffRect = { 0,0,89,136 };
	bigRightArrowsAnim.PushBack({ 89,136,89,136 });
	bigRightArrowsAnim.PushBack({ 0,0,0,0 });
	bigRightArrowsAnim.PushBack({ 0,0,0,0 });
	bigRightArrowsAnim.PushBack({ 0,136,89,136 });
	bigRightArrowsAnim.PushBack({ 356,0,89,136 });
	bigRightArrowsAnim.PushBack({ 0,0,0,0 });
	bigRightArrowsAnim.PushBack({ 0,0,0,0 });
	bigRightArrowsAnim.PushBack({ 267,0,89,136 });
	bigRightArrowsAnim.PushBack({ 178,0,89,136 });
	bigRightArrowsAnim.PushBack({ 89,0,89,136 });
	bigRightArrowsAnim.PushBack({ 0,0,0,0 });
	bigRightArrowsAnim.PushBack({ 0,0,0,0 });
	bigRightArrowsAnim.speed = 0.15f;
	// -----------------------------------------------------

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	App->textures->Unload(background_tex);
	background_tex = nullptr;
	App->textures->Unload(scoreboard_tex);
	scoreboard_tex = nullptr;
	App->textures->Unload(board_tex);
	board_tex = nullptr;
	App->textures->Unload(ball_tex);
	ball_tex = nullptr;
	App->textures->Unload(second_layer_tex);
	second_layer_tex = nullptr;
	App->textures->Unload(leftFlipper_tex);
	leftFlipper_tex = nullptr;
	App->textures->Unload(rightFlipper_tex);
	rightFlipper_tex = nullptr;
	App->textures->Unload(centerArrowsAnim_tex);
	centerArrowsAnim_tex = nullptr;
	App->textures->Unload(TopHole_tex);
	TopHole_tex = nullptr;
	App->textures->Unload(flames_tex);
	flames_tex = nullptr;
	App->textures->Unload(scoreTypes_tex);
	scoreTypes_tex = nullptr;
	App->textures->Unload(BlackArrowsGravity_tex);
	BlackArrowsGravity_tex = nullptr;
	App->textures->Unload(bigArrows_tex);
	bigArrows_tex = nullptr;

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	//LOG("NUM OF BALLS: %i", balls.count());
	//LOG("NUM OF INGAME BALLS: %i", inGameBalls);
	//LOG("SAFETY PLATE BALLS: %i", safetyPlateBalls);

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && balls.count() < 1)
	{
		switch (scene_phase)
		{
		case ENDGAME:
			scene_phase = game_loop::HIGHSCORE;
			break;
		case HIGHSCORE:
			restartBoard();
			break;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && scene_phase != ENDGAME)
	{
		if (preventyZone)
		{
			if (!kickerJoint->IsMotorEnabled())
			{
				shootBall();
				preventyZone = false;
			}
		}
	}

	// INPUT CONTROL FOR FLIPPERS ----------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		flipper_joint_left->EnableMotor(true);
		App->audio->PlayFx(flipper_sfx);
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
	{
		flipper_joint_left->EnableMotor(false);
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		flipper_joint_right->EnableMotor(true);
		App->audio->PlayFx(flipper_sfx);
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
	{
		flipper_joint_right->EnableMotor(false);
	}


	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();

	// All draw functions ------------------------------------------------------
	// draw background
	App->renderer->Blit(background_tex, 0, 0, NULL, 1.0f); //, c->data->GetRotation());
	// draw board
	App->renderer->Blit(board_tex, 0, 18, NULL, 1.0f);
	// DRAW animation top hole
	App->renderer->Blit(TopHole_tex, 202, 67, &TopHole.GetCurrentFrame());
	// draw top balls bouncer flash sprite
	p2List_item<activableSensors>* topBall = topBallslist.getFirst();
	while (topBall != NULL)
	{
		if (topBall->data.state == sensorState::active)
		{
			// blit sprite
			int x, y;
			topBall->data.b->GetPosition(x, y);
			App->renderer->Blit(sprites_tex, x - 2, y - 2, &topBall->data.rect[active]);

			// check time
			if (SDL_GetTicks() > topBall->data.eventTime + topBall->data.totalTime)
			{
				topBall->data.state = sensorState::inactive;
			}
		}
		topBall = topBall->next;
	}

	// draw scoreboard
	App->renderer->Blit(scoreboard_tex, 30, 0, NULL, 1.0f); 

	// draw animations// balls etc ---------------------------------------------------------------------------

	 if (SDL_GetTicks() > ballLostLogo.eventTime + ballLostLogo.totalTime)
		 ballLostLogo.state = sensorState::inactive;
	 App->renderer->Blit(sprites_tex, 182, 538, &ballLostLogo.rect[ballLostLogo.state]);


	// draw all sensors
	p2List_item<activableSensors>* sensors = sensor_list.getFirst();
	while (sensors != NULL)
	{
		int posx, posy = 0;
		sensors->data.b->GetPosition(posx, posy);
		App->renderer->Blit(sprites_tex, posx, posy, &sensors->data.rect[sensors->data.state]);

		sensors = sensors->next;
	}
	// and recheck if all of them are active, if yes, do something and restart state to all
	sensors = sensor_list.getFirst();
	int counter = 0;
	while (sensors != NULL)
	{
		if (sensors->data.state == sensorState::inactive)
		{
			break;
		}
		counter++;
		sensors = sensors->next;
		if (counter >= sensor_list.count()) // all sensors active
		{
			LOG("all sensors active");
			// do something: COMBO
			sensors = sensor_list.getFirst();
			while (sensors != NULL)
			{
				sensors->data.state = inactive;
				sensors = sensors->next;
			}
			// play sfx
			App->audio->PlayFx(extraBall_sfx);
			// add extra score
			App->player->score += allSensorsActives;
		}
	}

	// draw activable stars
	for (int i = 0; i < stars.instances; ++i)
	{
		// blit all inactive ones
		App->renderer->Blit(sprites_tex, stars.positions[i].x, stars.positions[i].y, &stars.rect[inactive]);
		// and draw on top the active ones
		if(starsCounter > (uint)i)
			App->renderer->Blit(sprites_tex, stars.positions[i].x, stars.positions[i].y, &stars.rect[active]);

	}

	// draw lock sprite graphic
	if (safetyPlateBalls > 0 && inGameBalls < 2)
	{
		App->renderer->Blit(sprites_tex, lockLogo.positions[0].x, lockLogo.positions[0].y, &lockLogo.rect[active]);
	}
	else
		App->renderer->Blit(sprites_tex, lockLogo.positions[0].x, lockLogo.positions[0].y, &lockLogo.rect[inactive]);



	// draw burn sprite graphic
	if (inGameBalls <= 1) {
		App->renderer->Blit(sprites_tex, gravityLogo.positions[0].x, gravityLogo.positions[0].y, &gravityLogo.rect[active]);
	}
	else {
		App->renderer->Blit(sprites_tex, gravityLogo.positions[0].x, gravityLogo.positions[0].y, &gravityLogo.rect[inactive]);
	}

	// DRAW ANIMATIONS ------------------------------------

	App->renderer->Blit(centerArrowsAnim_tex, 207, 320, &centerArrowsAnim.GetCurrentFrame());
	if(inGameBalls < 2)
		App->renderer->Blit(centerArrowsAnim_tex, 80, 204, &rightArrowsAnim.GetCurrentFrame(), NULL, -34);

	SDL_Rect r2 = { 0, 0, 50, 50 };
	App->renderer->Blit(BlackArrowsGravity_tex, 43, 218, &r2, 1.0f, 3);
	


	// draw flippers --------------------------------------
	int x, y;
	Flipper_Chain_L->GetPosition(x, y);
	App->renderer->Blit(leftFlipper_tex, x, y, NULL, 1.0f, Flipper_Chain_L->GetRotation(), 0, 0);
	Flipper_Chain_R->GetPosition(x, y);
	App->renderer->Blit(rightFlipper_tex, x, y, NULL, 1.0f, Flipper_Chain_R->GetRotation(), 0, 0);

	// DRAW main kicker
	mainKicker_body->GetPosition(x, y);
	App->renderer->Blit(sprites_tex, x, y, &mainKickerRect);

	// DRAW big arrows animations --------------------------

	// fixed off, left zone
	App->renderer->Blit(bigArrows_tex, 78, 98, &leftArrowsOffRect);
	// and animated
	App->renderer->Blit(bigArrows_tex, 78, 98, &bigLeftArrowsAnim.GetCurrentFrame());
	// fixed off, right zone
	App->renderer->Blit(bigArrows_tex, 266, 98, &rightArrowsOffRect);
	// and animated
	App->renderer->Blit(bigArrows_tex, 266, 98, &bigRightArrowsAnim.GetCurrentFrame());

	// -----------------------------------------------------

	// draw balls ------------------------------------------
	p2List_item<PhysBody*>* c = balls.getFirst();
	while (c != NULL)
	{
		int x, y;

		c->data->GetPosition(x, y);
		if (ball_state == ballState::BLIT) {
			App->renderer->Blit(ball_tex, x, y - 1, NULL, 1.0f, c->data->GetRotation());
		}
		else if (ball_state == ballState::DISAPPEAR && c->data != Gravity_Body) {
			App->renderer->Blit(ball_tex, x, y - 1, NULL, 1.0f, c->data->GetRotation());
		}
		c = c->next;
	}

	// DRAW TURBO SPRITE

	App->renderer->Blit(turboLogo_tex, -24, 66, NULL);

	// draw lateral kickers if trigger lateral kickers on
	if (kickerLeft.state == sensorState::active)
	{
		if (SDL_GetTicks() > kickerLeft.eventTime + kickerLeft.totalTime)
			kickerLeft.state = sensorState::inactive;
		int x, y;
		kickerLeft.b->GetPosition(x, y);
		App->renderer->Blit(sprites_tex, x - 13, y + 66, &kickerLeft.rect[kickerLeft.state]);
	}
	if (kickerRight.state == sensorState::active)
	{
		if (SDL_GetTicks() > kickerRight.eventTime + kickerRight.totalTime)
			kickerRight.state = sensorState::inactive;
		int x, y;
		kickerRight.b->GetPosition(x, y);
		App->renderer->Blit(sprites_tex, x - 13, y + 66, &kickerRight.rect[active]);
	}
	
	// check if we are on in game phase to draw the second layer on top of the ball -------
	
	if (scene_phase == game_loop::INGAME || scene_phase == game_loop::FAILURE)
	{
		SDL_Rect r = { 0,0,55,120 };
		App->renderer->Blit(second_layer_tex, 303, 99, &r, 1.0f); 
		r = { 132,31,80,52 };
		App->renderer->Blit(second_layer_tex, 176, 264, &r, 1.0f);
		App->renderer->Blit(second_layer_tex, 48, 109, &gravityLayerTexTrickRect, 1.0f);
	}
	// ------------------------------------------------------------------------------------
	uint Now = SDL_GetTicks();

	if (Inside_Vacuum) {

		if (App->player->score <= Vacuum_Combo_Score * Vacuum_Combo_Times || !Vacuum_Combo) {  // do it only when entering
			if (!Inside_Vacuum_Flag) {
				Vacuum_Time = Now;
				Vacuum_Body->body->SetTransform(b2Vec2(PIXEL_TO_METERS(215), PIXEL_TO_METERS(301)), 0.0f);
				Vacuum_Body->body->GetTransform();
				Vacuum_Body->body->SetType(b2_staticBody);
				Inside_Vacuum_Flag = true;
			}


			/*if (Now < Vacuum_Time + 3000) {
				LOG("Ball is trapped in vacuum");


			}*/

			else if (Now > Vacuum_Time + 3000) {
				LOG("Ball ejected from vacuum!");

				Vacuum_Body->body->SetType(b2_dynamicBody);
				Vacuum_Body->body->SetGravityScale(1.0f);

				uint dir = rand() % 2; 
				if (dir == 0) {
					Vacuum_Body->body->ApplyForceToCenter(b2Vec2(10, 100), true);  // then eject it 
				}
				else {
					Vacuum_Body->body->ApplyForceToCenter(b2Vec2(-10, 100), true);
				}

				Inside_Vacuum = false;
				Inside_Vacuum_Flag = false;

				// play sfx
				App->audio->PlayFx(canon_sfx);
				// add score
				App->player->score += vacuumScore;
			}

		}
	}

	uint Now2 = SDL_GetTicks(); 

	if (App->player->score > Vacuum_Combo_Score* Vacuum_Combo_Times && !Inside_Vacuum && Vacuum_Cleaner_Trigger != NULL) { // CHECK HERE
		
			if (!Vacuum_Combo_Flag) {
				Vacuum_Cleaner_Trigger->body->SetActive(false);  // first deactivate trigger
				Vacuum_Combo_Time = Now2; 
				Vacuum_Combo_Flag = true;
				Vacuum_Combo = true; 
				// play sfx
				App->audio->PlayFx(extraBall_sfx);
			}

			if (Vacuum_Combo) {
		        if (Now2 > Vacuum_Combo_Time && Ejected_Balls < 20) {       
			
				balls.add(App->physics->CreateCircle(216, 299, 11));                    // eject balls
				balls.getLast()->data->listener = this; 
				inGameBalls++;
				Ejected_Balls++; 

				uint dir = rand() % 2;
				if (dir == 0) {
					balls.getLast()->data->body->ApplyForceToCenter(b2Vec2(10, 100), true);
				}
				else {
					balls.getLast()->data->body->ApplyForceToCenter(b2Vec2(-10, 100), true);
				}

				Vacuum_Combo_Time += 200;
				// play sfx
				App->audio->PlayFx(canon_sfx);
				// add shooted ball score
				App->player->score += comboVaccumShootedBallScore;
			}

			else if (Ejected_Balls >= 20 && Now2 > Vacuum_Combo_Time + 200) {        // reactivate trigger after last ball
				Vacuum_Cleaner_Trigger->body->SetActive(true);
				Vacuum_Cleaner_Trigger->listener = this; 
				Vacuum_Combo_Flag = false; 
				Vacuum_Combo = false; 
				Ejected_Balls = 0; 
				Vacuum_Combo_Times++; 
			}
		}
		LOG("Score: %i, needed vacuum score:%i", App->player->score, Vacuum_Combo_Score * Vacuum_Combo_Times);
	}

	// auto shoot checker
	if (automaticShoot)
	{
		if (preventyZone)
		{
			shootBall();
			preventyZone = false;
			automaticShoot = false;
		}
	}

	// DRAW ENDGAME AND HIGHSCORE animations/sprites
	if (scene_phase == game_loop::ENDGAME || scene_phase == game_loop::HIGHSCORE)
	{
		int separation = 70;
		for (uint i = 0; i < NUM_FLAMES; ++i)
			App->renderer->Blit(flames_tex, 62 + (separation*i), 10, &flamesAnim[i].GetCurrentFrame());
		// Draw scoretype
		
		switch (scene_phase)
		{
		case ENDGAME:
			App->renderer->Blit(scoreTypes_tex, 106, 70, &yourScoreRect);
			break;
		case HIGHSCORE:
			App->renderer->Blit(scoreTypes_tex, 110, 70, &highScoreRect);
			break;
		}
	}

	// DRAW SCORE
	DrawScore();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	switch (scene_phase)
	{
	case START:
		// prevents launching a ball wich is not already in place
		if (bodyB == preventyTrigger)
		{
			LOG("PREVENTY TRIGGER REACHED");
			preventyZone = true;
		}

		//App->audio->PlayFx(bonus_fx);
		if (bodyB == exitLoopTrigger)
		{
			LOG("exit loop trigger");

			// destroy first loop part chain
			onlyLoopChain->to_delete = true;
			bodyB->to_delete = true;
			// add to in game ball counter
			inGameBalls++;
			// active the search for inactive balls
			checkInactiveBalls = true;
		}

		if (bodyB == enterBoardTrigger || bodyB == enterBoardTrigger2)
		{
			LOG("ball entered to game board");
			enterBoardTrigger->to_delete = true;
			enterBoardTrigger2->to_delete = true;
			// count the remaining balls in safe plate
			safetyPlateBalls--;
		}

		break; 

	case BLACK_HOLE:
		if (bodyB == Inside_Hole_Trigger) {
			Gravity_Zone_Chain->to_delete = true;
			Switch_From_Hole_To_Ingame = true;
		}
		break;


	case INGAME:
		if(bodyB == Lose_Life_Trigger)
		{
		
			LOG("ball is ready to be destroyed !!!!");
			if (bodyA != nullptr) {
				bodyA->to_delete = true;
			}

			App->player->Lives -= 1;
			inGameBalls--;
			// active lost ball logo
			ballLostLogo.state = sensorState::active;
			ballLostLogo.eventTime = SDL_GetTicks();
			//scene_phase = game_loop::FAILURE;
		}

		if (bodyB == leftBottomBouncerTrigger)
		{
			LOG("LEFT BOUNCER");
			//bodyA->body->ApplyLinearImpulse(b2Vec2(1.5f, -1.5f), bodyA->body->GetWorldCenter(), true); //ApplyForce(b2Vec2(50, -50), bodyA->body->GetWorldCenter(), true);//
			bodyA->body->ApplyForceToCenter(b2Vec2(60, -60), true);
			App->audio->PlayFx(bumper_sfx);
			App->player->score += bumperScore;
		}
		if (bodyB == rightBottomBouncerTrigger)
		{
			LOG("RIGHT BOUNCER");
			bodyA->body->ApplyForceToCenter(b2Vec2(-60, -60), true);
			App->audio->PlayFx(bumper_sfx);
			App->player->score += bumperScore;
		}

		// iterate sensor list for if anyone are oncollision

		p2List_item<activableSensors>* item = sensor_list.getFirst();
		while (item)
		{
			if (bodyB == item->data.b && item->data.state == sensorState::inactive)
			{
				LOG("Ding Collision");
				item->data.state = sensorState::active;
				App->player->score += (uint)item->data.scoreToGain;
				// play sfx
				App->audio->PlayFx(ding_sfx);
				
			}
			item = item->next;
		}

		if (bodyB == kickerLeft.b || bodyB == kickerRight.b)
		{
			LOG("security kicker");
			if (bodyB == kickerLeft.b)
			{
				kickerLeft.eventTime = SDL_GetTicks();
				kickerLeft.state = sensorState::active;
			}
			else
			{
				kickerRight.eventTime = SDL_GetTicks();
				kickerRight.state = sensorState::active;
			}
			bodyA->body->ApplyForce(b2Vec2(0, -100), bodyA->body->GetWorldCenter(), true);
			App->audio->PlayFx(rampBallKicked_sfx);
			// add score
			App->player->score += 7523;
		}


		if (bodyB == Vacuum_Cleaner_Trigger) {
			Vacuum_Body = bodyA; 
			Vacuum_Cleaner_Trigger->to_delete = true; 
			Inside_Vacuum = true; 	
		}

		if (bodyB == Extra_Ball_Trigger) {
			if (safetyPlateBalls > 0 && inGameBalls < 2)
			{
				LOG("New ball incoming...");
				linkedBody = bodyA;
				claimNewBall = true;
				// add score
				App->player->score += lockScore;
				//play sfx

			}
		}

		if (bodyB == Gravity_Zone_Trigger) {
			Gravity_Body = bodyA; 
			mainBoardChain->to_delete = true;
			topDividerLeft->to_delete = true;
			if (inGameBalls == 1) {
				scene_phase = game_loop::BLACK_HOLE;
			}
		}


		// check top ball sensors collisions
		p2List_item<activableSensors>* ballsItem = topBallslist.getFirst();
		while (ballsItem != NULL)
		{
			if (bodyB == ballsItem->data.b)
			{
				LOG("BALL BOUNCER TOUCHED");
				// play sfx
				App->audio->PlayFx(ding_sfx);
				ballsItem->data.state = active;
				// add score
				App->player->score += topBallsScore;
				// register event time
				ballsItem->data.eventTime = SDL_GetTicks();
			}
			ballsItem = ballsItem->next;
		}
		break;
	}
}

update_status ModuleSceneIntro::PostUpdate()
{
	// START PHASE
	// check if we have to delete any body

	p2List_item<PhysBody*>* itemBalls = balls.getFirst();

	switch (scene_phase)
	{
	case START:
		if (onlyLoopChain != nullptr)
		{
			if (onlyLoopChain->to_delete)
			{
				App->physics->DestroyObject(exitLoopTrigger);

				App->physics->DestroyObject(onlyLoopChain);
				delete onlyLoopChain;
				onlyLoopChain = nullptr;
				// Adds the main board chain
				mainBoardChain = App->physics->CreateChain(0, 18, mainBoard, 170, false, false);

				enterBoardTrigger = App->physics->CreateRectangleSensor(280, 100, 8, 8);
				enterBoardTrigger->listener = this;
				enterBoardTrigger2 = App->physics->CreateRectangleSensor(320, 240, 8, 8);
				enterBoardTrigger2->listener = this;
				// create top dividers
				topDividerLeft = App->physics->CreateChain(0, 18, topLeftWayPoints, 32, false, true);
				topDividerRight = App->physics->CreateChain(0, 18, topRightWayPoints, 30, false, true);

			}

		}

		if (enterBoardTrigger != nullptr || enterBoardTrigger2 != nullptr)
		{
			if (enterBoardTrigger->to_delete || enterBoardTrigger2->to_delete)
			{
				App->physics->DestroyObject(enterBoardTrigger);
				App->physics->DestroyObject(enterBoardTrigger2);
				delete enterBoardTrigger;
				delete enterBoardTrigger2;
				enterBoardTrigger = nullptr;
				enterBoardTrigger2 = nullptr;
				// create tap
				exitLoopTapChain = App->physics->CreateChain(0, 18, exitLoopTapPivots, 20, false, false);
				// search inactive balls
				if (checkInactiveBalls)
				{
					p2List_item<PhysBody*>* b = balls.getFirst();
					while (b)
					{
						if (!b->data->body->IsActive())
						{
							b->data->body->SetActive(true);
						}
						b = b->next;
					}
					checkInactiveBalls = false;
				}

				//switch game state
				scene_phase = game_loop::INGAME;
			}
		}

		break;

	case INGAME:

		if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN){               // useful for testing 
			balls.getFirst()->data->body->SetType(b2_staticBody); 
         }
		if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) {               // useful for testing 
			balls.getFirst()->data->body->SetType(b2_dynamicBody);
		}


		if (starsCounter >= 3) {  // extra ball if 3 stars collected
			TopHole.speed = 0.4f;	
			if (TopHole.Finished()) {
				balls.add(App->physics->CreateCircle(217, 81, 11));
				balls.getLast()->data->listener = this; 
				inGameBalls++; 

				balls.getLast()->data->body->SetLinearVelocity(b2Vec2(2, 0));  // so that it doesn't fall straight
				TopHole.Reset();
				TopHole.speed = 0;
				TopHole.loop = false;
				starsCounter = 0;
				// play sfx
				App->audio->PlayFx(extraBall_sfx);
				// add score
				App->player->score += allStarScore;
			}
		}

		// check if the player puts the ball on lock function
		if (claimNewBall)
		{
			linkedBody->body->SetActive(false);
			claimNewBall = false;
			// "free" the pointer
			linkedBody = nullptr;
			// prepare the board for the new incoming ball
			newBall();
			// get the next ball ready
			shootBall();
			// this is a automatic shoot
			automaticShoot = true;
			scene_phase = game_loop::START;
		}

		// create a list of items to the balls for check if anyone wants to be destroyed
		while (itemBalls != NULL)
		{
			if (itemBalls->data->to_delete)
			{
				// check for security if is a dragged mouse debug ball and set clicked to null
				if (itemBalls->data->body == App->physics->clickedBody && App->physics->clickedBody != nullptr)
					App->physics->DestroyMouseJoint(&App->physics->mouse_joint);
				App->physics->DestroyObject(itemBalls->data);
				balls.del(itemBalls); // todo, delete item from list crashes?
				//itemBalls = nullptr;

				if (inGameBalls < 1)
				{
					// prepare for next ball
					shootBall();
					scene_phase = game_loop::FAILURE;
				}
				break;
			}
			itemBalls = itemBalls->next;
		}

		if (Gravity_Zone_Chain != nullptr) {
			if (Gravity_Zone_Chain->to_delete) {
				App->physics->DestroyObject(Gravity_Zone_Chain);
				delete Gravity_Zone_Chain;
				Gravity_Zone_Chain = nullptr; 
			}
		}

		if (mainBoardChain == nullptr) {
			mainBoardChain = App->physics->CreateChain(0, 18, mainBoard, 170, false, false);   // restart board after gravity zone
		}

		if (topDividerLeft == nullptr) {
			topDividerLeft = App->physics->CreateChain(0, 18, topLeftWayPoints, 32, false, true);   // restart lest divider after gravity zone
		}

		if (Vacuum_Cleaner_Trigger != nullptr) {
			if (Vacuum_Cleaner_Trigger->to_delete) {
				App->physics->DestroyObject(Vacuum_Cleaner_Trigger);
				delete Vacuum_Cleaner_Trigger;
				Vacuum_Cleaner_Trigger = nullptr;
			}
		}
		else {
			uint Now = SDL_GetTicks(); 
			if (!Reset_Vacuum_Flag) {
				Reset_Vacuum_Time = Now;
				Reset_Vacuum_Flag = true; 
			}

			if (Now > Reset_Vacuum_Time + 3500) {
				Vacuum_Cleaner_Trigger = App->physics->CreateRectangleSensor(216, 299, 26, 16);
				Reset_Vacuum_Flag = false; 
			}
		}

			
		break;

	case BLACK_HOLE:

		if (mainBoardChain != nullptr) {
			if (mainBoardChain->to_delete) {
				App->physics->DestroyObject(mainBoardChain);
				delete mainBoardChain;
				mainBoardChain = nullptr;

				Gravity_Zone_Chain = App->physics->CreateChain(0, 18, Gravity_Zone, 68, false, true);
				Gravity_Body->body->SetGravityScale(0);

			}
		}
		
		 
		if (topDividerLeft != nullptr) {
			if (topDividerLeft->to_delete) {
				App->physics->DestroyObject(topDividerLeft);
				delete topDividerLeft;
				topDividerLeft = nullptr;
			}
		}


		if (Switch_From_Hole_To_Ingame) {
			Reset_Gravity = false; 
			
			Switch_From_Hole_To_Ingame = false;   
			ball_state = ballState::DISAPPEAR; 

			Gravity_Body->body->SetType(b2_staticBody);

			LOG("Ball is static");

			Gravity_Body->body->SetTransform(b2Vec2(PIXEL_TO_METERS(217), PIXEL_TO_METERS(81)), Gravity_Body->body->GetAngle()); // set teleport and dissappear 

			TopHole.speed = 0.4f; 
			

		}
		
			else if (TopHole.Finished()) {
				// 	balls.getFirst()->data->body->GetTransform();   // teleport

				Gravity_Body->body->SetType(b2_dynamicBody);
				Gravity_Body->body->SetGravityScale(1.0f);
				Gravity_Body->body->SetLinearVelocity(b2Vec2(2, 0));  // so that it doesn't fall straight
				ball_state = ballState::BLIT; 

				LOG("Ball is dynamic again");

				TopHole.Reset();
				TopHole.speed = 0;
				TopHole.loop = false;

				// adds counter stars
				starsCounter += 1;
				if (starsCounter >= 3) starsCounter = 3; // locks max stars
				// add score
				App->player->score += topHoleScore;
				// play sfx ?

				if (Gravity_Body != nullptr) {
					Gravity_Body = nullptr;
				}

				scene_phase = game_loop::INGAME;
			}

			else {
				b2Vec2 Ball_Pos = Gravity_Body->body->GetWorldCenter();
				b2Vec2 Hole_Pos = Inside_Hole_Trigger->body->GetWorldCenter(); 

				b2Vec2 Distance = Hole_Pos - Ball_Pos; 
				float Radius = Distance.Length(); 

				uint Constant = 1; 

				if (!Reset_Gravity) {
					Gravity_Force = 0.1f *(Constant / Radius * Radius);
					Reset_Gravity = true; 
				}

				else {
					Gravity_Force += 0.12f;
				}


				Gravity_Body->body->ApplyForceToCenter(Gravity_Force*Distance, true);

				// extra impulse to center
				Gravity_Body->body->ApplyForceToCenter(Hole_Pos, true);
				
			}
		

		break;

	case FAILURE:

		if (balls.count() > 0) {

			newBall();
			scene_phase = game_loop::START;
		}
		else {
			// saves highscore of all games
			if (App->player->score > App->player->highScore)
				App->player->highScore = App->player->score;
			scene_phase = game_loop::ENDGAME;
		}
		break;

	case ENDGAME:
		break;

	default:
		break;

	}

	return UPDATE_CONTINUE;

}
bool ModuleSceneIntro::restartBoard()
{
	bool ret = true;

	// vacuum combo relatives
	Vacuum_Combo = true;
	Inside_Vacuum_Flag = false;
	Vacuum_Combo_Flag = false;
	Vacuum_Time = 0;
	Vacuum_Combo_Time = 0;
	Ejected_Balls = 0;
	Vacuum_Combo_Times = 1;
	// adds balls to safe plate
	generateStartBalls();
	// recount base balls
	safetyPlateBalls = balls.count();
	// prepare for first ball
	newBall();
	// resets kicker joint motor
	kickerJoint->EnableMotor(false);
	// resets vars
	App->player->score = 0;
	starsCounter = 0;
	// reset all sensors
	p2List_item<activableSensors>* sensors = sensor_list.getFirst();
	while (sensors != NULL)
	{
		sensors->data.state = sensorState::inactive;
		sensors = sensors->next;
	}

	// maybe delete something or create any physbody, if any

	if (Vacuum_Body != nullptr) {
		// Vacuum_Body->body->SetActive(false); 
		Vacuum_Body = nullptr;
	}


	if (Gravity_Body != nullptr) {
		Gravity_Body = nullptr;
	}

	// change ingame state to start
	scene_phase = game_loop::START;
	
	return ret;
}

bool ModuleSceneIntro::newBall()
{
	bool ret = true;

	// deletes main board chain
	App->physics->DestroyObject(mainBoardChain);
	// deletes corner tap exit loop
	App->physics->DestroyObject(exitLoopTapChain);
	// deletes top dividers
	App->physics->DestroyObject(topDividerLeft);
	//if(topDividerRight != nullptr)
	App->physics->DestroyObject(topDividerRight);

	// create needed triggers
	exitLoopTrigger = App->physics->CreateRectangleSensor(372, 140, 8, 8);
	exitLoopTrigger->listener = this;
	// create the loop chain part
	onlyLoopChain = App->physics->CreateChain(0, 18, loopPartPoints, 120, false, false);

	return ret;
}

void ModuleSceneIntro::DrawScore()
{

	int x, y;
	x = 77;
	y = 29;// first draw position

	int playerScore;

	if (scene_phase == game_loop::HIGHSCORE)
	{
		playerScore = App->player->highScore;
	}
	else
	{
		playerScore = App->player->score;
	}

	int w, h = 0;
	int tex_lenght = SDL_QueryTexture(score_tex, NULL, NULL, &w, &h);

	// get the width rect proportion
	w = w / 10; // we have 10 numbers in texture
	// the height in this particular case we dont needed it, texture only has 1 row of numbers

	for (int i = 0; i < 10; ++i) // rect pushbacks
	{
		scoreCutRect[i] = { i * w, 0, w, h };
	}

	int separation = 8;
	for (int i = 11; i >= 0; --i) // draw all numbers on scoreboard panel
	{
		int tempScore = playerScore % 10;
		playerScore = playerScore / 10;

		App->renderer->Blit(score_tex, x + ((w + separation) * i), y, &scoreCutRect[tempScore]);
	}

}


bool ModuleSceneIntro::generateStartBalls()
{
	bool ret = true;

	// Add the five balls
	balls.add(App->physics->CreateCircle(432, 437, 11));           // There are 5 balls at the start
	balls.add(App->physics->CreateCircle(432, 414, 11));
	balls.add(App->physics->CreateCircle(432, 392, 11));
	balls.add(App->physics->CreateCircle(432, 369, 11));
	balls.add(App->physics->CreateCircle(432, 346, 11));
	// set all balls as bullet, maybe we only set the first ball, and when the next ball enters at game set it
	p2List_item<PhysBody*>* item = balls.getFirst();
	while (item)
	{
		item->data->listener = this;
		item->data->body->SetBullet(true);
		// tweaks the ball restitution
		setRestitution(item->data, 0.2f);
		item = item->next;
	}
	// count at start the baseballs we have
	safetyPlateBalls = balls.count();

	return ret;
}

bool ModuleSceneIntro::shootBall()
{
	bool ret = true;
	 
	// check if we still have remaining balls
	if (safetyPlateBalls > 0 && !kickerJoint->IsMotorEnabled())
	{
		kickerJoint->EnableMotor(true);
	}
	else if (safetyPlateBalls > 0 && kickerJoint->IsMotorEnabled())
	{
		kickerJoint->EnableMotor(false);
		// classic pinball shake to reposition balls
		balls.getFirst()->data->body->ApplyForce(b2Vec2(0, -20), balls.getFirst()->data->body->GetWorldCenter(), true);
	}
	
	return ret;

}

void ModuleSceneIntro::setRestitution(PhysBody* body, float restitution)
{
	// sets restitution for the first fixture attached to a body
	b2Fixture* f = body->body->GetFixtureList();
	f->SetRestitution(restitution);

}