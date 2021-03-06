#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;


public:
	int width, height;
	b2Body* body = nullptr;
	Module* listener = nullptr;
	bool to_delete = false;

};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, bool dynamic = true, int density = 1.0f, float restitution = 0.0f);
	PhysBody* CreateRectangle(int x, int y, int width, int height);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, float angle = 0.0f);
	PhysBody* CreateChain(int x, int y, int* points, int size, bool dynamic = true, bool loop = true);
	PhysBody* CreateConvexPoly(int x, int y, int* points, int size, bool dynamic = true, bool isSensor = false, float density = 1.0f, float restitution = 0.0f);

	bool DestroyObject(PhysBody* body);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

	void SetJoints(PhysBody* bodyA, PhysBody* bodyB);
	b2RevoluteJoint* SetJoint(b2RevoluteJointDef* joint);
	void DestroyMouseJoint(b2MouseJoint** mouseJoint);

	// vars
	b2World* world;
	b2Body* clickedBody = nullptr;
	b2MouseJoint* mouse_joint = nullptr;

private:

	bool debug;
	//b2World* world;
	
	b2Body* ground;
	
	b2WeldJoint* flipper_joint_r = nullptr;

};