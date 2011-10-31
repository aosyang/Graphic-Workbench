/********************************************************************
	created:	2011/9/5
	filename: 	Character.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
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
	RenderSystem::DrawColoredSprite(Vector2(-0.5f * m_sCharSize, -0.5f * m_sCharSize) + m_Position,
									Vector2(0.5f * m_sCharSize, 0.5f * m_sCharSize) + m_Position,
									GWColor(1.0f, 0.949f, 0.0f));
}

void Player::Update()
{
	if (m_ClimbingLadder)
	{
		m_Velocity = Vector2::ZERO;
	}
	else
	{
		// Base class update
		Actor::Update();
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
		m_Velocity.y = 0.35f;
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
	RenderSystem::DrawColoredSprite(Vector2(-0.45f, -0.45f) + m_Position,
									Vector2(0.45f, 0.45f) + m_Position,
									GWColor(0.0f, 1.0f, 0.949f));
}

void Patient::Interactive()
{
	KleinGame()->SetWorldview(m_WorldId);
}
