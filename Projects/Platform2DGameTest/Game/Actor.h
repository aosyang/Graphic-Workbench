#ifndef Actor_h__
#define Actor_h__

#include "Math/GWVectors.h"
#include "BoundBox.h"

typedef struct StageGeom STAGE_GEOM;

class Actor
{
public:
	Actor();
	virtual ~Actor() {}


	BoundBox GetWorldBoundBox() const
	{
		return m_Bound.Translate(m_Position);
	}

	const BoundBox& GetLocalBoundBox() const { return m_Bound; }

	virtual void Render();

	bool TestCollision(STAGE_GEOM* stage_geom, const Vector2& new_pos_rel = Vector2::ZERO) const;

	// Test if actor can move in this direction, if not, give an available movement
	bool DoCollisionMove(const BoundBox& other, const Vector2& input, Vector2* output);
	void Update(float delta_time);

	// Move actor in local direction
	void Translate(const Vector2& vecRel);

	Vector2& Velocity() { return m_Velocity; }
	Vector2& WorldPosition() { return m_Position; }

protected:

	virtual void OnHitTop();
	virtual void OnHitGround();

protected:
	BoundBox			m_Bound;
	Vector2				m_Position;
	Vector2				m_Velocity;
};

#endif // Actor_h__