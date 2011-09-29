#include "Character.h"
#include "BoundBox.h"

#include <d3dx9math.h>

// define size of a character
float Character::m_sCharSize = 1.0f;

Character::Character()
: m_Position(7.0f, 5.0f, 0.0f),
  m_Velocity(Vector3::ZERO),
  m_CanJump(false)
{
	m_Bound = BoundBox(-0.5f * m_sCharSize, -0.5f * m_sCharSize,
						0.5f * m_sCharSize, 0.5f * m_sCharSize);
}

Character::~Character()
{
}

void Character::Render()
{
	ActorSpriteVertex v[6] =
	{
		{ -0.5f * m_sCharSize, -0.5f * m_sCharSize, 0.0f, 0xFFFFF200 },
		{ -0.5f * m_sCharSize, 0.5f * m_sCharSize, 0.0f, 0xFFFFF200 },
		{ 0.5f * m_sCharSize, 0.5f * m_sCharSize, 0.0f, 0xFFFFF200 },

		{ 0.5f * m_sCharSize, 0.5f * m_sCharSize, 0.0f, 0xFFFFF200 },
		{ 0.5f * m_sCharSize, -0.5f * m_sCharSize, 0.0f, 0xFFFFF200 },
		{ -0.5f * m_sCharSize, -0.5f * m_sCharSize, 0.0f, 0xFFFFF200 },
	};

	D3DXMATRIXA16 transform;
	D3DXMatrixTranslation(&transform, m_Position.x, m_Position.y, m_Position.z);
	RenderSystem::Device()->SetTransform( D3DTS_WORLD, &transform );
	
	RenderSystem::Device()->SetTexture(0, NULL);
	RenderSystem::Device()->SetFVF(ActorSpriteFVF);
	RenderSystem::Device()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(ActorSpriteVertex));
}

bool Character::DoCollisionMove( const BoundBox& other, const Vector3& input, Vector3* output )
{
	Vector3 newPos = m_Position + input;

	BoundBox worldBox(m_Bound.xMin + m_Position.x,
					  m_Bound.yMin + m_Position.y,
					  m_Bound.xMax + m_Position.x,
					  m_Bound.yMax + m_Position.y);

	BoundBox box(m_Bound.xMin + newPos.x,
				 m_Bound.yMin + newPos.y,
				 m_Bound.xMax + newPos.x,
				 m_Bound.yMax + newPos.y);

	int x_col = 0,		// x collision type -1: to the left 0: collision 1: to the right
		y_col = 0;		// y collision type -1: to the bottom 0: collision 1: to the top
	int last_x_col = 0,
		last_y_col = 0;

	// Calculate collision type for last frame
	if (worldBox.xMax <= other.xMin)
	{
		last_x_col = -1;
	}
	else if (worldBox.xMin >= other.xMax)
	{
		last_x_col = 1;
	}

	if (worldBox.yMax <= other.yMin)
	{
		last_y_col = -1;
	}
	else if (worldBox.yMin >= other.yMax)
	{
		last_y_col = 1;
	}

	// Calculate collision type for this frame
	if (box.xMax <= other.xMin)
	{
		x_col = -1;
	}
	else if (box.xMin >= other.xMax)
	{
		x_col = 1;
	}

	if (box.yMax <= other.yMin)
	{
		y_col = -1;
	}
	else if (box.yMin >= other.yMax)
	{
		y_col = 1;
	}

	if (x_col!=0 || y_col!=0)
	{
		// no collision, done
		return false;
	}
	else
	{
		*output = input;

		// check last frame with collision type
		if (last_x_col==-1)
		{
			output->x -= box.xMax - other.xMin;
		}
		else if (last_x_col==1)
		{
			output->x += other.xMax - box.xMin;
		}
		else if (last_y_col==-1)
		{
			output->y -= box.yMax - other.yMin;
			OnHitTop();
		}
		else if (last_y_col==1)
		{
			output->y += other.yMax - box.yMin;
			OnHitGround();
		}
	}

	return true;
}

void Character::Update( float elapsedTime )
{
	m_CanJump = false;
}

void Character::Translate( const Vector3& vecRel )
{
	m_Position += vecRel;
}

void Character::OnHitTop()
{
	// Bumped my head, stop moving upward
	m_Velocity.y = 0.0f;
}

void Character::OnHitGround()
{
	m_Velocity.y = 0.0f;
	m_CanJump = true;
}

void Character::Jump()
{
	if (m_CanJump)
	{
		m_Velocity = Vector3(0.0f, 0.5f, 0.0f);
	}
}
