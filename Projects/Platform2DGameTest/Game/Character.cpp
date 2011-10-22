#include "Character.h"
#include "BoundBox.h"
#include "GameStage.h"

#include <d3dx9math.h>

static float COL_TOLERANCE = 0.001f;

// define size of a character
float Character::m_sCharSize = 0.8f;

Character::Character()
: m_Position(0.0f, 0.0f, 0.0f),
  m_Velocity(Vector3::ZERO),
  m_CanJump(false),
  m_ClimbingLadder(false)
{
	m_Bound = BoundBox(-0.5f * m_sCharSize, -0.5f * m_sCharSize,
						0.5f * m_sCharSize, 0.5f * m_sCharSize);
}

Character::~Character()
{
}

void Character::Render()
{
	D3DXMATRIXA16 transform;
	D3DXMatrixTranslation(&transform, m_Position.x, m_Position.y, m_Position.z);
	RenderSystem::Device()->SetTransform( D3DTS_WORLD, &transform );

	RenderSystem::DrawColoredSprite(Vector2(-0.5f * m_sCharSize, -0.5f * m_sCharSize),
									Vector2(0.5f * m_sCharSize, 0.5f * m_sCharSize),
									0xFFFFF200);
}

bool Character::TestCollision( STAGE_GEOM* stage_geom, const Vector3& new_pos_rel /*= Vector3::ZERO*/ ) const
{
	BoundBox char_box = m_Bound.Translate(m_Position + new_pos_rel);

	return char_box.TestBoxCollision(stage_geom->bound);
}

bool Character::DoCollisionMove( const BoundBox& other, const Vector3& input, Vector3* output )
{
	Vector3 newPos = m_Position + input;

	BoundBox worldBox = m_Bound.Translate(m_Position);
	BoundBox box = m_Bound.Translate(newPos);

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
			output->x -= box.xMax - other.xMin + COL_TOLERANCE;
		}
		else if (last_x_col==1)
		{
			output->x += other.xMax - box.xMin + COL_TOLERANCE;
		}
		else if (last_y_col==-1)
		{
			output->y -= box.yMax - other.yMin + COL_TOLERANCE;
			OnHitTop();
		}
		else if (last_y_col==1)
		{
			output->y += other.yMax - box.yMin + COL_TOLERANCE;
			OnHitGround();
		}
	}

	return true;
}

void Character::Update( float delta_time )
{
	if (m_ClimbingLadder)
	{
		m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		// Gravity
		m_Velocity += Vector3(0.0f, -0.03f, 0.0f);
	}

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
	if (m_ClimbingLadder)
	{
		m_ClimbingLadder = false;
	}
	else if (m_CanJump)
	{
		m_Velocity = Vector3(0.0f, 0.35f, 0.0f);
	}
}
