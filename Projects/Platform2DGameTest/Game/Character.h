#ifndef Character_h__
#define Character_h__

#include "Actor.h"

typedef struct StageGeom STAGE_GEOM;

class Character : public Actor
{
public:
	Character();

	virtual void Render();
	virtual void Character::Update( float delta_time );

	void Jump();

	void SetClimbingLadder(bool climbing) { m_ClimbingLadder = climbing; }
	bool IsClimbingLadder() { return m_ClimbingLadder; }

private:

	void OnHitTop();
	void OnHitGround();

	bool				m_CanJump;
	bool				m_ClimbingLadder;

	static float		m_sCharSize;
};

#endif // Character_h__
