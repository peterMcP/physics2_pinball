#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

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

	Background_Tex = App->textures->Load("pinball/Background.png");
	DrawBg(Background_Tex);


	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	return ret;
}

void ModuleSceneIntro::DrawBg(SDL_Texture* Background_Tex) {

	int points[116] = {
		25, 19,
		27, 15,
		30, 9,
		35, 6,
		40, 3,
		390, 3,
		395, 6,
		400, 9,
		405, 15,
		407, 19,
		407, 92,
		413, 100,
		420, 108,
		424, 116,
		429, 132,
		431, 146,
		431, 159,
		426, 179,
		420, 206,
		420, 301,
		433, 301,
		440, 304,
		444, 308,
		449, 317,
		449, 424,
		447, 431,
		439, 439,
		432, 441,
		425, 441,
		411, 456,
		398, 456,
		398, 505,
		395, 512,
		391, 518,
		385, 523,
		376, 526,
		261, 526,
		255, 532,
		245, 532,
		239, 538,
		191, 538,
		185, 532,
		173, 532,
		168, 526,
		40, 526,
		30, 523,
		22, 515,
		18, 505,
		18, 179,
		14, 174,
		10, 165,
		9, 163,
		6, 153,
		6, 134,
		8, 125,
		13, 114,
		17, 106,
		25, 98
	};

	App->physics->CreateChain(0, 0, points, 116, false);
	App->renderer->Blit(Background_Tex, 0, 0, NULL, 0.0f);

}



// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	App->textures->Unload(Background_Tex);
	Background_Tex = nullptr;

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 25));
		circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int startChain[166] = {
			414, 298,
			414, 215,
			417, 191,
			425, 163,
			426, 142,
			423, 124,
			417, 112,
			408, 99,
			402, 92,
			385, 82,
			368, 79,
			350, 79,
			334, 85,
			326, 91,
			318, 100,
			312, 110,
			307, 128,
			306, 142,
			309, 160,
			315, 174,
			324, 184,
			334, 191,
			346, 196,
			362, 196,
			376, 192,
			386, 183,
			393, 172,
			396, 158,
			397, 142,
			392, 128,
			384, 115,
			369, 128,
			377, 142,
			377, 152,
			372, 167,
			361, 173,
			345, 173,
			334, 166,
			329, 152,
			328, 138,
			331, 124,
			338, 114,
			350, 105,
			364, 103,
			377, 105,
			389, 112,
			399, 123,
			405, 137,
			406, 148,
			405, 161,
			402, 171,
			398, 193,
			395, 210,
			392, 229,
			391, 251,
			390, 280,
			390, 295,
			390, 313,
			390, 341,
			390, 370,
			390, 393,
			390, 412,
			390, 434,
			404, 434,
			430, 434,
			438, 431,
			442, 425,
			444, 414,
			444, 372,
			443, 328,
			441, 317,
			438, 311,
			430, 308,
			422, 309,
			422, 333,
			422, 399,
			422, 407,
			419, 412,
			414, 412,
			411, 407,
			411, 390,
			411, 317,
			413, 311
		};
		

		startPhysBody = App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), startChain, 166, false);
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

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->audio->PlayFx(bonus_fx);
}
