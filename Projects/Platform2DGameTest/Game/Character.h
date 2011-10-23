#ifndef Character_h__
#define Character_h__

#include "Actor.h"

typedef struct StageGeom STAGE_GEOM;

class Player;
class Patient;

Player* CreatePlayer();
Patient* CreatePatient();

class Player : public Actor
{
	friend Player* CreatePlayer();
public:
	Player();

	virtual void Render();
	virtual void Update( float delta_time );

	void Jump();

	void SetClimbingLadder(bool climbing) { m_ClimbingLadder = climbing; }
	bool IsClimbingLadder() { return m_ClimbingLadder; }

private:

	void OnHitGround();

	bool				m_CanJump;
	bool				m_ClimbingLadder;

	static float		m_sCharSize;
};

class Patient : public Actor
{
	friend Patient* CreatePatient();
public:
	virtual void Render();

protected:
	Patient();
};

#endif // Character_h__
