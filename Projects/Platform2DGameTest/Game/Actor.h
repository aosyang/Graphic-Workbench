/********************************************************************
	created:	2011/10/22
	filename: 	Actor.h
	author:		Mwolf
	
	purpose:	Base class for movable objects
*********************************************************************/
#ifndef Actor_h__
#define Actor_h__

#include "Math/GWVectors.h"
#include "BoundBox.h"

typedef struct StageGeom STAGE_GEOM;

class Actor;

Actor* GetFirstActor();
Actor* GetNextActor(Actor* actor);
void AddActorToGame(Actor* actor);
void RemoveActorFromGame(Actor* actor);

class Actor
{
public:
	BoundBox GetWorldBoundBox() const { return m_Bound.Translate(m_Position); }

	const BoundBox& GetLocalBoundBox() const { return m_Bound; }

	virtual void Render() = 0;

	bool TestCollision( const BoundBox& bound, const Vector2& new_pos_rel = Vector2::ZERO) const;

	// Test if actor can move in this direction, if not, give an available movement
	bool DoCollisionMove(const BoundBox& other, const Vector2& input, Vector2* output);
	virtual void Update();

	// Move actor in local direction
	void Translate(const Vector2& vecRel);

	Vector2& Velocity() { return m_Velocity; }
	Vector2& MoveController() { return m_MoveController; }

	void SetPosition( const Vector2& pos ) { m_Position = pos; }
	const Vector2& GetPosition() const { return m_Position; }

	virtual void Interactive();

protected:
	Actor();
	virtual ~Actor() {}

	virtual void OnHitTop();
	virtual void OnHitGround();

protected:
	BoundBox			m_Bound;
	Vector2				m_Position;
	Vector2				m_Velocity;

	Vector2				m_MoveController;

private:

	Actor*				next;

	friend Actor* GetFirstActor();
	friend Actor* GetNextActor(Actor* actor);
	friend void AddActorToGame(Actor* actor);
	friend void RemoveActorFromGame(Actor* actor);
};

#endif // Actor_h__