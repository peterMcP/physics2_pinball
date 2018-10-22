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

// chains pivots header
#include "chainsPivots.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/wheel.png");
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");

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
	Ball_Safety_Chain = App->physics->CreateChain(0, 18, safetyZonePoints, 30, false, false);
	

	// board main body perimeter parts chain
	//mainBoardChain = App->physics->CreateChain(0, 18, mainBoard, 178, false, false);
	// black hole gravity zone circle collider // TODO, search if a circle can have interior collisions, if not, make a chain
	// blackHoleCircle = App->physics->CreateCircle(65, 162, 56, false);
	// exit loop tap (prevents the pinball ball to return to the exit loop)
	//exitLoopTapChain = App->physics->CreateChain(0, 18, exitLoopTapPivots, 20, false, false);
	//b2Fixture* f = exitLoopTapChain->body->GetFixtureList();
	//f->SetSensor(true);
	// ------------------------------------------------------------------------------------

	// TRIGGERS/SENSORS
	// 364,129,8,8
	exitLoopTrigger = App->physics->CreateRectangleSensor(372, 140, 8, 8);
	exitLoopTrigger->listener = this;
	Lose_Life_Trigger = App->physics->CreateRectangleSensor(216, 530, 60, 20);
	Lose_Life_Trigger->listener = this;

	// TEST BALL -------
	balls.add(App->physics->CreateCircle(400, 420, 11));           // There are 5 balls at the start
	balls.add(App->physics->CreateCircle(422, 363, 11));
	balls.add(App->physics->CreateCircle(422, 352, 11));
	balls.add(App->physics->CreateCircle(422, 341, 11));
	//balls.add(App->physics->CreateCircle(422, 330, 11));
	// set all balls as bullet, maybe we only set the first ball, and when the next ball enters at game set it
	p2List_item<PhysBody*>* item = balls.getFirst();
	while (item)
	{
		item->data->listener = this;
		item->data->body->SetBullet(true);
		item = item->next;
	}


	//balls.getFirst()->data->listener = this;              // FIFO (first ball in, first ball out)
	

	b2Fixture* f = balls.getFirst()->data->body->GetFixtureList();
	f->SetFriction(0.7f);

	// ADD MAIN COMPONENTS TO LIST, its components are static and never change
	mainBoardComponents.add(App->physics->CreateChain(0, 18, topSeparatorPoints, 12, false, true));
	mainBoardComponents.add(App->physics->CreateChain(37, 18, topSeparatorPoints, 12, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, bottomUpWallL, 16, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, bottomUpWallR, 16, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, rightBottomWayPoints, 28, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, leftBottomWayPoints, 22, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, turboCompressorPoints, 32, false, false));

	//Next_To_Flipper_Chain_R = App->physics->CreateChain(0, 18, rightBottomWayPoints, 28, false, true);
	//Next_To_Flipper_Chain_L = App->physics->CreateChain(0, 18, leftBottomWayPoints, 22, false, true);


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
	/*circles.add(App->physics->CreateCircle(217, 186, 12, false, 1.0f, 1.0f));
	circles.add(App->physics->CreateCircle(258, 221, 12, false, 1.0f, 1.0f));
	circles.add(App->physics->CreateCircle(174, 221, 12, false, 1.0f, 1.0f));*/
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
	

	// -----------------------------------------------------------------------------------------------

	// bottom security kickers
	leftSecurityKicker = App->physics->CreateRectangleSensor(97, 420, 10, 10, 0.0f);
	rightSecurityKicker = App->physics->CreateRectangleSensor(334, 420, 10, 10, 0.0f);



	// other special sensors
	Vacuum_Cleaner_Trigger = App->physics->CreateRectangleSensor(216, 299, 26, 16); 
	Extra_Ball_Trigger = App->physics->CreateRectangleSensor(370, 242, 6, 12, -0.8f);
	Gravity_Zone_Trigger = App->physics->CreateRectangleSensor(45, 200, 26, 12);
    Inside_Hole_Trigger = App->physics->CreateRectangleSensor(65, 161, 7, 7);

	// ANIMATIONS
	for (int i = 9; i >= 0; --i)
		centerArrowsAnim.PushBack({ i * 15, 0, 15, 135});
	centerArrowsAnim.speed = 0.15f;

	
	for (int i = 0; i<=39; ++i)
		TopHole.PushBack({ i*28, 1, 28, 28 });
	TopHole.speed = 0.0f;
	
	TopHole.loop = false; 

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


	App->textures->Unload(circle);
	App->textures->Unload(box);
	App->textures->Unload(rick);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{

	LOG("NUM OF BALLS: %i", balls.count());
	LOG("NUM OF INGAME BALLS: %i", inGameBalls);

	/*if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		balls.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 11));
		balls.getLast()->data->listener = this;
		Current_Ball = balls.getLast()->data;
	}*/

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		
		
		//startChainBg = App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), startChain, 166, false);
	}

	// test addimpulse to ball
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && scene_phase != ENDGAME)
	{
		// balls.getLast()->data->body->ApplyForce(b2Vec2(0,-420), balls.getLast()->data->body->GetWorldCenter(), true);

		if (balls.getFirst()->data != nullptr) {
			balls.getFirst()->data->body->ApplyForce(b2Vec2(0, -420), balls.getFirst()->data->body->GetWorldCenter(), true);
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
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

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

	// draw all sensors
	p2List_item<activableSensors>* sensors = sensor_list.getFirst();
	while (sensors != NULL)
	{
		int posx, posy = 0;
		sensors->data.b->GetPosition(posx, posy);
		App->renderer->Blit(sprites_tex, posx, posy, &sensors->data.rect[sensors->data.state]);

		sensors = sensors->next;
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

	// DRAW ANIMATIONS ------------------------------------

	App->renderer->Blit(centerArrowsAnim_tex, 207, 320, &centerArrowsAnim.GetCurrentFrame());

	// draw flippers --------------------------------------
	int x, y;
	Flipper_Chain_L->GetPosition(x, y);
	App->renderer->Blit(leftFlipper_tex, x, y, NULL, 1.0f, Flipper_Chain_L->GetRotation(), 0, 0);
	Flipper_Chain_R->GetPosition(x, y);
	App->renderer->Blit(rightFlipper_tex, x, y, NULL, 1.0f, Flipper_Chain_R->GetRotation(), 0, 0);

	// draw balls
	c = balls.getFirst();
	
	while (c != NULL)
	{
		int x, y;

		c->data->GetPosition(x, y);
		if (ball_state == ballState::BLIT) {
			App->renderer->Blit(ball_tex, x, y - 1, NULL, 1.0f, c->data->GetRotation());
		}
			
		c = c->next;
	}

	
	// DRAW TURBO SPRITE

	App->renderer->Blit(turboLogo_tex, -24, 66, NULL);


	// check if we are on in game phase to draw the second layer on top of the ball

	if (scene_phase == game_loop::INGAME || scene_phase == game_loop::FAILURE)
	{
		SDL_Rect r = { 0,0,55,120 };
		App->renderer->Blit(second_layer_tex, 303, 99, &r, 1.0f); 
		r = { 56,3,74,110 };
		App->renderer->Blit(second_layer_tex, 48, 109, &r, 1.0f);
		r = { 132,31,80,52 };
		App->renderer->Blit(second_layer_tex, 176, 264, &r, 1.0f);

	}

	uint Now = SDL_GetTicks();

	if (Inside_Vacuum) {

	                                              // do it only when entering
			if (!Inside_Vacuum_Flag) {
				Vacuum_Time = Now;
				balls.getFirst()->data->body->SetTransform(b2Vec2(PIXEL_TO_METERS(215), PIXEL_TO_METERS(301)), 0.0f);
				balls.getFirst()->data->body->GetTransform();
				balls.getFirst()->data->body->SetType(b2_staticBody);
				Inside_Vacuum_Flag = true;
			}


			/*if (Now < Vacuum_Time + 3000) {
				LOG("Ball is trapped in vacuum");


			}*/

			else if (Now > Vacuum_Time + 3000) {
				LOG("Ball ejected from vacuum!");

				balls.getFirst()->data->body->SetType(b2_dynamicBody);
				balls.getFirst()->data->body->SetGravityScale(1.0f);

				balls.getFirst()->data->body->ApplyForceToCenter(b2Vec2(10, 100), true);  // then eject it 

				Inside_Vacuum = false;
				Inside_Vacuum_Flag = false;

				// play sfx
				App->audio->PlayFx(canon_sfx);
				// add score
				App->player->score += vacuumScore;
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

		if (bodyB == enterBoardTrigger)
		{
			LOG("ball entered to game board");
			enterBoardTrigger->to_delete = true;
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

		if (bodyB == leftSecurityKicker || bodyB == rightSecurityKicker)
		{
			LOG("security kicker");
			bodyA->body->ApplyForce(b2Vec2(0, -100), bodyA->body->GetWorldCenter(), true);
			App->audio->PlayFx(rampBallKicked_sfx);
		}


		if (bodyB == Vacuum_Cleaner_Trigger) {
			Vacuum_Cleaner_Trigger->to_delete = true; 
			Inside_Vacuum = true; 	
		}

		if (bodyB == Extra_Ball_Trigger) {
			// check if we still have more balls
			 // test, count() returns the total of all balls in the list
			// we assume here we have 1 ball in game, and limits the functionality for lock
			// only if in game we have only 1 ball
			if (balls.count() > 1 && inGameBalls < 2)
			{
				LOG("New ball incoming...");
				linkedBody = bodyA;
				claimNewBall = true;
			}
		}

		if (bodyB == Gravity_Zone_Trigger) {
			mainBoardChain->to_delete = true; 
			topDividerLeft->to_delete = true; 
			scene_phase = game_loop::BLACK_HOLE; 
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
				// create top dividers
				topDividerLeft = App->physics->CreateChain(0, 18, topLeftWayPoints, 32, false, true);
				topDividerRight = App->physics->CreateChain(0, 18, topRightWayPoints, 30, false, true);

			}

		}

		if (enterBoardTrigger != nullptr)
		{
			if (enterBoardTrigger->to_delete)
			{
				App->physics->DestroyObject(enterBoardTrigger);
				delete enterBoardTrigger;
				enterBoardTrigger = nullptr;
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

		// check if the player puts the ball on lock function
		if (claimNewBall)
		{
			linkedBody->body->SetActive(false);
			claimNewBall = false;
			// "free" the pointer
			linkedBody = nullptr;
			// prepare the board for the new incoming ball
			newBall();
			scene_phase = game_loop::START;
		}

		// create a list of items to the balls for check if anyone wants to be destroyed
		while (itemBalls != NULL)
		{
			if (itemBalls->data->to_delete)
			{
	
				App->physics->DestroyObject(itemBalls->data);
				balls.del(itemBalls); // todo, delete item from list crashes?
				//itemBalls = nullptr;

				if (inGameBalls < 1)
				{
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
				balls.getFirst()->data->body->SetGravityScale(0); 

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
			balls.getFirst()->data->body->SetType(b2_staticBody); 

			LOG("Ball is static");

			 balls.getFirst()->data->body->SetTransform(b2Vec2(PIXEL_TO_METERS(217), PIXEL_TO_METERS(81)), balls.getFirst()->data->body->GetAngle()); // set teleport and dissappear 

			TopHole.speed = 0.4f; 
			

		}
		
			else if (TopHole.Finished()) {
				// 	balls.getFirst()->data->body->GetTransform();   // teleport

				balls.getFirst()->data->body->SetType(b2_dynamicBody);
				balls.getFirst()->data->body->SetGravityScale(1.0f);
				balls.getFirst()->data->body->SetLinearVelocity(b2Vec2(2, 0));  // so that it doesn't fall straight
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

				scene_phase = game_loop::INGAME;
			}

			else {
				b2Vec2 Ball_Pos = balls.getFirst()->data->body->GetWorldCenter(); 
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


				balls.getFirst()->data->body->ApplyForceToCenter(Gravity_Force*Distance, true); 
				
			}
		

		break;

	case FAILURE:

		if (App->player->Lives != 0) {                            
			// deletes main board chain
			App->physics->DestroyObject(mainBoardChain);
			// deletes corner tap exit loop
			App->physics->DestroyObject(exitLoopTapChain);
			// deletes top dividers
			App->physics->DestroyObject(topDividerLeft);
			App->physics->DestroyObject(topDividerRight);

			newBall();
			scene_phase = game_loop::START;
		}
		else {
			if (Lose_Life_Trigger != nullptr) {
				Lose_Life_Trigger->to_delete = true;
				App->physics->DestroyObject(Lose_Life_Trigger);

				delete Lose_Life_Trigger;       // change this later in the destroy method       
				Lose_Life_Trigger = nullptr;
			}
			restartBoard(); // restart entire board
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



	return ret;
}

bool ModuleSceneIntro::newBall()
{
	bool ret = true;

	// add listener to next ball
	//balls.getFirst()->data->listener = this;

	// deletes main board chain
	App->physics->DestroyObject(mainBoardChain);
	// deletes corner tap exit loop
	App->physics->DestroyObject(exitLoopTapChain);
	// deletes top dividers
	App->physics->DestroyObject(topDividerLeft);
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

	int playerScore = App->player->score; // test

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