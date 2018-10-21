#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include <cstdint>

#define MAX_ROUND_LIVES 4

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:
	uint8_t Lives = MAX_ROUND_LIVES; 
};