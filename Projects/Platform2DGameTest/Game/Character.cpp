#include "Character.h"
#include "BoundBox.h"
#include "GameStage.h"
#include "GameMain.h"

#include <d3dx9.h>

// define size of a character
float Player::m_sCharSize = 0.8f;

Player* CreatePlayer()
{
	Player* player = new Player;
	AddActorToGame(player);

	return player;
}

Patient* CreatePatient()
{
	Patient* patient = new Patient;
	AddActorToGame(patient);

	return patient;
}

Player::Player()
: m_CanJump(false),
  m_ClimbingLadder(false)
{
	m_Bound = BoundBox(-0.5f * m_sCharSize, -0.5f * m_sCharSize,
						0.5f * m_sCharSize, 0.5f * m_sCharSize);
}

void Player::Render()
{
	D3DXMATRIXA16 transform;
	D3DXMatrixTranslation(&transform, m_Position.x, m_Position.y, 0.0f);
	RenderSystem::Device()->SetTransform( D3DTS_WORLD, &transform );

	RenderSystem::DrawColoredSprite(Vector2(-0.5f * m_sCharSize, -0.5f * m_sCharSize),
									Vector2(0.5f * m_sCharSize, 0.5f * m_sCharSize),
									0xFFFFF200);
}

void Player::Update( float delta_time )
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

void Player::Jump()
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

void Player::OnHitGround()
{
	Actor::OnHitGround();
	m_CanJump = true;
}

Patient::Patient()
: m_WorldId(1)
{
	m_Bound = BoundBox(-0.45f, -0.45f, 0.45f, 0.45f);
}

void Patient::Render()
{
	D3DXMATRIXA16 transform;
	D3DXMatrixTranslation(&transform, m_Position.x, m_Position.y, 0.0f);
	RenderSystem::Device()->SetTransform( D3DTS_WORLD, &transform );

	RenderSystem::DrawColoredSprite(Vector2(-0.45f, -0.45f),
									Vector2(0.45f, 0.45f),
									0xFF00FFF2);
}

void Patient::Interactive()
{
	KleinGame()->SetWorldview(m_WorldId);
}
