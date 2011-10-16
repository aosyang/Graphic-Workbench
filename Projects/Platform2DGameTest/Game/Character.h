#ifndef Character_h__
#define Character_h__

#include "Math/GWVectors.h"
#include "BoundBox.h"
#include "RenderDevice.h"

typedef struct StageGeom STAGE_GEOM;

struct ActorSpriteVertex
{
	float x, y, z;
	DWORD color;
};

#define ActorSpriteFVF D3DFVF_XYZ|D3DFVF_DIFFUSE

class Character
{
public:
	Character();
	~Character();

	BoundBox GetWorldBoundBox() const
	{
		return BoundBox(m_Bound.xMin + m_Position.x,
						m_Bound.yMin + m_Position.y,
						m_Bound.xMax + m_Position.x,
						m_Bound.yMax + m_Position.y);
	}

	const BoundBox& GetLocalBoundBox() const { return m_Bound; }

	void Render();

	bool TestCollision(STAGE_GEOM* stage_geom, const Vector3& new_pos_rel = Vector3::ZERO) const;

	// Test if character can move in this direction, if not, give an available movement
	bool DoCollisionMove(const BoundBox& other, const Vector3& input, Vector3* output);
	void Update(float delta_time);

	// Move character in local direction
	void Translate(const Vector3& vecRel);
	void Jump();

	Vector3& Velocity() { return m_Velocity; }
	Vector3& WorldPosition() { return m_Position; }

	void SetClimbingLadder(bool climbing) { m_ClimbingLadder = climbing; }
	bool IsClimbingLadder() { return m_ClimbingLadder; }

private:

	void OnHitTop();
	void OnHitGround();

	BoundBox			m_Bound;
	Vector3				m_Position;
	Vector3				m_Velocity;
	//int				m_LastXCol, m_LastYCol;
	bool				m_CanJump;
	bool				m_ClimbingLadder;

	static float		m_sCharSize;
};

#endif // Character_h__
