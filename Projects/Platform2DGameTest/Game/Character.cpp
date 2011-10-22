#include "Character.h"
#include "BoundBox.h"
#include "GameStage.h"

#include <d3dx9.h>

// define size of a character
float Character::m_sCharSize = 0.8f;

Character::Character()
: m_CanJump(false),
  m_ClimbingLadder(false)
{
	m_Bound = BoundBox(-0.5f * m_sCharSize, -0.5f * m_sCharSize,
						0.5f * m_sCharSize, 0.5f * m_sCharSize);
}

void Character::Render()
{
	D3DXMATRIXA16 transform;
	D3DXMatrixTranslation(&transform, m_Position.x, m_Position.y, 0.0f);
	RenderSystem::Device()->SetTransform( D3DTS_WORLD, &transform );

	RenderSystem::DrawColoredSprite(Vector2(-0.5f * m_sCharSize, -0.5f * m_sCharSize),
									Vector2(0.5f * m_sCharSize, 0.5f * m_sCharSize),
									0xFFFFF200);
}

void Character::Update( float delta_time )
{
	if (m_ClimbingLadder)
	{
		m_Velocity = Vector2::ZERO;
	}
	else
	{
		// Base class update
		Actor::Update( delta_time );
	}

	m_CanJump = false;
}

void Character::Jump()
{
	if (m_ClimbingLadder)
	{
		m_ClimbingLadder = false;
	}
	else if (m_CanJump)
	{
		m_Velocity = Vector2(0.0f, 0.35f);
	}
}

void Character::OnHitTop()
{
	Actor::OnHitTop();
}

void Character::OnHitGround()
{
	Actor::OnHitGround();
	m_CanJump = true;
}
