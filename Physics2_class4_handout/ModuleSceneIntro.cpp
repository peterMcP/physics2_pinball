#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

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

	// assets textures
	ball_tex = App->textures->Load("pinball/ball.png");
	
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	// -----------------------------------------------------------------------------------
	// create background chains ---
	startLoopChain = App->physics->CreateChain(0, 18, startChainPivots2, 134, false, false);
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

	// TEST BALL
	balls.add(App->physics->CreateCircle(400, 420, 11));
	balls.getLast()->data->listener = this;
	

	return ret;
}

void ModuleSceneIntro::DrawBg(SDL_Texture* Background_Tex) {


	App->renderer->Blit(Background_Tex, 0, 0, NULL, 0.0f);

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
	

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		balls.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 11));
		balls.getLast()->data->listener = this;
	}

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
		balls.getLast()->data->body->ApplyForce(b2Vec2(0,-400), balls.getLast()->data->body->GetWorldCenter(), true);

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
		App->renderer->Blit(ball_tex, x, y-1, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
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
			startLoopChain->to_delete = true;
			bodyB->to_delete = true;
		}
		if (bodyB == enterBoardTrigger)
		{
			LOG("ball entered to game board");
			enterBoardTrigger->to_delete = true;
		}
		break;
	}

}

update_status ModuleSceneIntro::PostUpdate()
{

	// START PHASE
	// check if we have to delete any body
	switch (scene_phase)
	{
	case START:
		if (startLoopChain != nullptr)
		{
			if (startLoopChain->to_delete)
			{
				App->physics->DestroyObject(exitLoopTrigger);

				App->physics->DestroyObject(startLoopChain);
				delete startLoopChain;
				startLoopChain = nullptr;
				//b2Body* body = startLoopChain->body;
				// Adds the main board chain
				mainBoardChain = App->physics->CreateChain(0, 18, mainBoard, 170, false, false);

				enterBoardTrigger = App->physics->CreateRectangleSensor(280, 100, 8, 8);
				enterBoardTrigger->listener = this;
			}
			
		}
		if (enterBoardTrigger != nullptr)
		{
			if (enterBoardTrigger->to_delete)
			{
				App->physics->DestroyObject(enterBoardTrigger);
				delete startLoopChain;
				startLoopChain = nullptr;
				// create tap
				exitLoopTapChain = App->physics->CreateChain(0, 18, exitLoopTapPivots, 20, false, false);
				//switch game state
				scene_phase = game_loop::INGAME;
			}
		}
		
		break;
	case INGAME:
		break;
	case BLACK_HOLE:
		break;
	case FAILURE:
		break;
	default:
		break;
	}

	
	return UPDATE_CONTINUE;
}
