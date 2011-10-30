/********************************************************************
	created:	2011/9/5
	filename: 	Character.h
	author:		Mwolf
	
	purpose:	Definitions for characters in game
*********************************************************************/
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
	virtual void Update();

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

	virtual void Interactive();

protected:
	Patient();

	int		m_WorldId;
};

#endif // Character_h__
