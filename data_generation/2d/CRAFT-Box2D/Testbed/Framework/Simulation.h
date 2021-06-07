/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

//RENAMED AND UPDATED THE ORIGINAL VERSION FROM BOX2D

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Box2D/Box2D.h"
#include "SimulationDefines.h"

#if USE_DEBUG_DRAW
#include "DebugDraw.h"
#else
#include "SimulationRenderer.hpp"
#include "Box2D/Extension/b2VisWorld.hpp"
#endif

#include "Camera.hpp"

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include "Testbed/glad/glad.h"
#endif
#include "Testbed/glfw/glfw3.h"

#include <stdlib.h> 
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <random>
#include <iostream>

class Simulation;
struct SettingsBase;

typedef Simulation* TestCreateFcn();

#define	RAND_LIMIT	32767
#define DRAW_STRING_NEW_LINE 16

/// Random number in range [-1,1]
inline float32 RandomFloat()
{
	float32 r = (float32)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float32 RandomFloat(float32 lo, float32 hi)
{
	float32 r = (float32)(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}

inline float32 RandomFloatFromHardware(float32 lo, float32 hi)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_real_distribution<> distr(lo, hi); // define the range 
	return distr(eng);
}

inline float32 RandomFloatWithSeed(float32 lo, float32 hi, int seed)
{
	std::mt19937 eng(seed); // seed the generator
	std::uniform_real_distribution<> distr(lo, hi); // define the range 
	return distr(eng);
}

inline int RandomInteger(int lo, int hi)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(lo, hi); // define the range 
	return distr(eng);
}

inline float32 RandomFloatWithinRanges(std::vector<float32> minimums, std::vector<float32> maximums)
{
	if (minimums.size() != maximums.size()) throw "The two extremum lists must have the same size!";

	int rand_range_i = RandomInteger(0, minimums.size() - 1);

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_real_distribution<> distr(minimums[rand_range_i], maximums[rand_range_i]); // define the range 
	return distr(eng);
}

inline b2Vec2 RandomUnitVector()
{
	float32 r = 1.0f * sqrt(RandomFloat(0.0f, 1.0f));
	float32 theta = RandomFloat(0.0f, 1.0f) * M_PI * 2;
	float32 x = cos(theta) * r;
	float32 y = sin(theta) * r;
	b2Vec2 randUnitVector = b2Vec2();
	randUnitVector.Set(x, y);
	return randUnitVector;
}

/// Test settings. Some can be controlled in the GUI.
struct SettingsBase
{
	typedef std::shared_ptr<SettingsBase> Ptr;

	SettingsBase()
	{
		hz = 60.0f;
		velocityIterations = 8;
		positionIterations = 3;
		drawShapes = true;
		drawJoints = true;
		drawAABBs = false;
		drawContactPoints = false;
		drawContactNormals = false;
		drawContactImpulse = false;
		drawFrictionImpulse = false;
		drawCOMs = false;
		drawStats = false;
		drawProfile = false;
		enableWarmStarting = true;
		enableContinuous = true;
		enableSubStepping = false;
		enableSleep = true;
		pause = false;
		singleStep = false;
		bufferWidth = 640;
		bufferHeight = 320;
        stepCount = 0;
	}

	virtual ~SettingsBase()
	{

	}

	float32 hz;
	int32 velocityIterations;
	int32 positionIterations;
	bool drawShapes;
	bool drawJoints;
	bool drawAABBs;
	bool drawContactPoints;
	bool drawContactNormals;
	bool drawContactImpulse;
	bool drawFrictionImpulse;
	bool drawCOMs;
	bool drawStats;
	bool drawProfile;
	bool enableWarmStarting;
	bool enableContinuous;
	bool enableSubStepping;
	bool enableSleep;
	bool pause;
	bool singleStep;
	int bufferWidth;
	int bufferHeight;
    int stepCount;
};

struct TestEntry
{
	const char* name;
	TestCreateFcn* createFcn;
};

extern TestEntry g_testEntries[];
// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class DestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Fixture* fixture) override { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint) override;

	Simulation* test;
};

const int32 k_maxContactPoints = 2048;

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
	float32 normalImpulse;
	float32 tangentImpulse;
	float32 separation;
};

class Simulation : public b2ContactListener
{
public:
	typedef std::shared_ptr<Simulation> Ptr;

	Simulation();
	virtual ~Simulation();

	void DrawTitle(const char* string);
	virtual void Step(SettingsBase* settings);
	virtual void Keyboard(int key) { B2_NOT_USED(key); }
	virtual void KeyboardUp(int key) { B2_NOT_USED(key); }
	void ShiftMouseDown(const b2Vec2& p);
	virtual void MouseDown(const b2Vec2& p);
	virtual void MouseUp(const b2Vec2& p);
	void MouseMove(const b2Vec2& p);
	void LaunchBomb();
	void LaunchBomb(const b2Vec2& position, const b2Vec2& velocity);

	void SpawnBomb(const b2Vec2& worldPt);
	void CompleteBombSpawn(const b2Vec2& p);

	// Let derived tests know that a joint was destroyed.
	virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

	// Callbacks for derived classes.
	virtual void BeginContact(b2Contact* contact)  override { B2_NOT_USED(contact); }
	virtual void EndContact(b2Contact* contact)  override { B2_NOT_USED(contact); }
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
	}

	void ShiftOrigin(const b2Vec2& newOrigin);

protected:
	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	b2Body* m_groundBody;
	b2AABB m_worldAABB;
	ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;
	DestructionListener m_destructionListener;
	int32 m_textLine;
	WORLD* m_world;
	b2Body* m_bomb;
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_bombSpawnPoint;
	bool m_bombSpawning;
	b2Vec2 m_mouseWorld;
	int32 m_StepCount;

	b2Profile m_maxProfile;
	b2Profile m_totalProfile;
};

#endif
