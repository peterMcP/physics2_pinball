#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"

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

	// assets textures
	ball_tex = App->textures->Load("pinball/ball.png");
	
	// audio
	//music = App->audio->LoadFx("pinball/audio/soundtrack.wav");    // music as a Fx, so that it plays many times 
	//App->audio->PlayFx(1, -1); 
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	// -----------------------------------------------------------------------------------
	// create background chains ---
	onlyLoopChain = App->physics->CreateChain(0, 18, loopPartPoints, 120, false, false);
	Ball_Safety_Chain = App->physics->CreateChain(0, 18, safetyZonePoints, 30, false, false);
	

	// board main body perimeter parts chain
	//mainBoardChain = App->physics->CreateChain(0, 18, mainBoard, 178, false, false);
	// black hole gravity zone circle collider // TODO, search if a circle can have interior collisions, if not, make a chain
	//blackHoleCircle = App->physics->CreateCircle(65, 162, 56, false);
	// exit loop tap (prevents the pinball ball to return to the exit loop)
	//exitLoopTapChain = App->physics->CreateChain(0, 18, exitLoopTapPivots, 20, false, false);
	//b2Fixture* f = exitLoopTapChain->body->GetFixtureList();
	//f->SetSensor(true);
	// ------------------------------------------------------------------------------------

	// TRIGGERS/SENSORS
	// 364,129,8,8
	exitLoopTrigger = App->physics->CreateRectangleSensor(372, 140, 8, 8);
	exitLoopTrigger->listener = this;
	Lose_Life_Trigger = App->physics->CreateRectangleSensor(218, 492, 30, 19);
	Lose_Life_Trigger->listener = this;

	// TEST BALL -------
	balls.add(App->physics->CreateCircle(400, 420, 11));           // There are 5 balls at the start
	balls.add(App->physics->CreateCircle(422, 363, 11));
	balls.add(App->physics->CreateCircle(422, 352, 11));
	balls.add(App->physics->CreateCircle(422, 341, 11));
	balls.add(App->physics->CreateCircle(422, 330, 11));

	
	

	/*balls.getLast()->data->listener = this;
	Current_Ball = balls.getLast()->data; 

	b2Fixture* f = balls.getLast()->data->body->GetFixtureList();
	f->SetFriction(0.7f);*/

	balls.getFirst()->data->listener = this;              // FIFO (first ball in, first ball out)
	

	b2Fixture* f = balls.getFirst()->data->body->GetFixtureList();
	f->SetFriction(0.7f);

	// ADD MAIN COMPONENTS TO LIST, its components are static and never change
	mainBoardComponents.add(App->physics->CreateChain(0, 18, rightBottomWayPoints, 28, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, leftBottomWayPoints, 22, false, true));
	mainBoardComponents.add(App->physics->CreateChain(0, 18, topSeparatorPoints, 12, false, true));
	mainBoardComponents.add(App->physics->CreateChain(37, 18, topSeparatorPoints, 12, false, true));

	// ADD SPECIAL COMPONENTS
	Flipper_Chain_R = App->physics->CreateChain(0, 18, Flipper_R, 20, false);
	Flipper_Chain_L = App->physics->CreateChain(0, 18, Flipper_L, 20, false);

	// ------------------

	return ret;
}

//void ModuleSceneIntro::DrawBg(SDL_Texture* Background_Tex) {
//
//
//	App->renderer->Blit(Background_Tex, 0, 0, NULL, 0.0f);
//
//}



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
	

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
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
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		// balls.getLast()->data->body->ApplyForce(b2Vec2(0,-420), balls.getLast()->data->body->GetWorldCenter(), true);

		if (balls.getFirst()->data != nullptr) {
			balls.getFirst()->data->body->ApplyForce(b2Vec2(0, -420), balls.getFirst()->data->body->GetWorldCenter(), true);
		}

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

	/*c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}*/

	// draw background
	App->renderer->Blit(background_tex, 0, 0, NULL, 1.0f); //, c->data->GetRotation());
	// draw board
	App->renderer->Blit(board_tex, 0, 18, NULL, 1.0f);
	// draw scoreboard
	App->renderer->Blit(scoreboard_tex, 30, 0, NULL, 1.0f); 

	// draw animations// balls etc

	c = balls.getFirst();
	
	while (c != NULL)
	{
		int x, y;

		c->data->GetPosition(x, y);
		App->renderer->Blit(ball_tex, x, y - 1, NULL, 1.0f, c->data->GetRotation());
			
		c = c->next;
	}

	// check if we are on in game phase to draw the second layer on top of the ball

	if (scene_phase == game_loop::INGAME || scene_phase == game_loop::FAILURE)
	{
		SDL_Rect r = { 0,0,55,120 };
		App->renderer->Blit(second_layer_tex, 303, 99, &r, 1.0f); 
		r = { 56,3,74,110 };
		App->renderer->Blit(second_layer_tex, 48, 109, &r, 1.0f);
	}


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
		}

		if (bodyB == enterBoardTrigger)
		{
			LOG("ball entered to game board");
			enterBoardTrigger->to_delete = true;
		}

		break; 

	case INGAME:
		if(bodyB == Lose_Life_Trigger)
		{
			LOG("ball is ready to be destroyed !!!!");
			if (bodyA != nullptr) {
				bodyA->to_delete = true;
			}

			//Ball_Safety_Chain->to_delete = true;

			App->player->Lives -= 1;
			//scene_phase = game_loop::FAILURE;
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
				//b2Body* body = startLoopChain->body;
				// Adds the main board chain
				mainBoardChain = App->physics->CreateChain(0, 18, mainBoard, 170, false, false);
				//Next_To_Flipper_Chain_1 = App->physics->CreateChain(0, 18, Next_To_Flipper_1, 54, false, true);

				circles.add(App->physics->CreateCircle(217, 186, 12, false));
				circles.add(App->physics->CreateCircle(258, 221, 12, false));
				circles.add(App->physics->CreateCircle(174, 221, 12, false));


				enterBoardTrigger = App->physics->CreateRectangleSensor(280, 100, 8, 8);
				enterBoardTrigger->listener = this;



				//Ball_Safety_Chain = App->physics->CreateChain(0, 18, Safety_Ball, 26, false, false);

			}

		}

		if (enterBoardTrigger != nullptr)
		{
			if (enterBoardTrigger->to_delete)
			{
				App->physics->DestroyObject(enterBoardTrigger);
				//delete onlyLoopChain;
				//startLoopChain = nullptr;
				// create tap
				exitLoopTapChain = App->physics->CreateChain(0, 18, exitLoopTapPivots, 20, false, false);
				//switch game state
				scene_phase = game_loop::INGAME;
			}
		}

		break;

	case INGAME:
		// create a list of items to the balls for check if anyone wants to be destroyed
		while (itemBalls != NULL)
		{

			if (itemBalls->data->to_delete)
			{
				App->physics->DestroyObject(itemBalls->data);
				balls.del(itemBalls); // todo, delete item from list crashes?
				//itemBalls = nullptr;
				break;
			}
			itemBalls = itemBalls->next;
		}
		break;

	case BLACK_HOLE:
		break;

	case FAILURE:

		if (App->player->Lives != 0) {
			scene_phase = game_loop::START;
		}
		else {
			if (Lose_Life_Trigger != nullptr) {
				Lose_Life_Trigger->to_delete = true;
				App->physics->DestroyObject(Lose_Life_Trigger);

				delete Lose_Life_Trigger;       // change this later in the destroy method       
				Lose_Life_Trigger = nullptr;
			}
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
