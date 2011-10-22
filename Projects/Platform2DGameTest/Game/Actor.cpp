#include "Actor.h"
#include "RenderDevice.h"
#include "GameStage.h"
#include "GameDef.h"

#include <d3dx9.h>

Actor::Actor()
: m_Position(0.0f, 0.0f),
  m_Velocity(Vector2::ZERO)
{
	m_Bound = BoundBox(-0.5f, -0.5f, 0.5f, 0.5f);
}

void Actor::Render()
{
	D3DXMATRIXA16 transform;
	D3DXMatrixTranslation(&transform, m_Position.x, m_Position.y, 0.0f);
	RenderSystem::Device()->SetTransform( D3DTS_WORLD, &transform );

	RenderSystem::DrawColoredSprite(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f), 0xF2FFF200);
}

bool Actor::TestCollision( STAGE_GEOM* stage_geom, const Vector2& new_pos_rel /*= Vector2::ZERO*/ ) const
{
	BoundBox bb = m_Bound.Translate(m_Position + new_pos_rel);

	return bb.TestBoxCollision(stage_geom->bound);
}

bool Actor::DoCollisionMove( const BoundBox& other, const Vector2& input, Vector2* output )
{
	Vector2 newPos = m_Position + input;

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
			output->x -= box.xMax - other.xMin + KLEIN_COLLISION_TOLERANCE;
		}
		else if (last_x_col==1)
		{
			output->x += other.xMax - box.xMin + KLEIN_COLLISION_TOLERANCE;
		}
		else if (last_y_col==-1)
		{
			output->y -= box.yMax - other.yMin + KLEIN_COLLISION_TOLERANCE;
			OnHitTop();
		}
		else if (last_y_col==1)
		{
			output->y += other.yMax - box.yMin + KLEIN_COLLISION_TOLERANCE;
			OnHitGround();
		}
	}

	return true;
}

void Actor::Update( float delta_time )
{
	// Gravity
	m_Velocity += Vector2(0.0f, -0.03f);
}

void Actor::Translate( const Vector2& vecRel )
{
	m_Position += vecRel;
}

void Actor::OnHitTop()
{
	// Bumped my head, stop moving upward
	m_Velocity.y = 0.0f;
}

void Actor::OnHitGround()
{
	m_Velocity.y = 0.0f;
}
