#ifndef Character_h__
#define Character_h__

#include "Vector3.h"
#include "BoundBox.h"
#include "RenderDevice.h"

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

	// Test if character can move in this direction, if not, give an available movement
	bool TestCollision(Vector3& vecRel, const BoundBox& other);

	void Update(float elapsedTime);

	// Move character in local direction
	void Translate(const Vector3& vecRel);

	Vector3& Velocity() { return m_Velocity; }

	void Jump();

private:

	void OnHitTop();
	void OnHitGround();

	struct Vertex
	{
		float x, y, z;
		DWORD color;
	};

	BoundBox	m_Bound;
	Vector3		m_Position;
	Vector3		m_Velocity;
	//int			m_LastXCol, m_LastYCol;
	bool		m_CanJump;
};

#endif // Character_h__
