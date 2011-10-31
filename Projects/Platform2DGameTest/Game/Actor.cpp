/********************************************************************
	created:	2011/10/22
	filename: 	Actor.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "Actor.h"
#include "Renderer/GWRenderDevice.h"
#include "GameStage.h"
#include "GameDef.h"

#include <d3dx9.h>

static Actor*	ActorListHead = NULL;
static Actor*	ActorListTail = NULL;

Actor* GetFirstActor()
{
	return ActorListHead;
}

Actor* GetNextActor( Actor* actor )
{
	return actor->next;
}

void AddActorToGame( Actor* actor )
{
	if (ActorListTail == NULL)
	{
		// Build new linked list
		ActorListHead = actor;
		ActorListTail = actor;
	}
	else
	{
		// Add new actor to the tail
		ActorListTail->next = actor;
		ActorListTail = actor;
	}
}

void RemoveActorFromGame( Actor* actor )
{
	if ( actor == NULL ) return;

	Actor* actor_elem = GetFirstActor();

	if ( ActorListHead == actor )
	{
		if ( ActorListTail == ActorListHead )
		{
			// Last element, clean the list
			ActorListHead = ActorListTail = NULL;
		}
		else
		{
			ActorListHead = ActorListHead->next;
		}
		
		delete actor;

		return;
	}

	while ( actor_elem )
	{
		if ( actor_elem->next == actor )
		{
			actor_elem->next = actor->next;

			// Move tail pointer forward if tail removed
			if ( ActorListTail == actor )
				ActorListTail = actor_elem;

			delete actor;
			return;
		}

		actor_elem = GetNextActor( actor_elem );
	}
}

Actor::Actor()
: m_Position(0.0f, 0.0f),
  m_Velocity(Vector2::ZERO),
  m_MoveController(0.0f, 0.0f),
  next(NULL)
{
}

bool Actor::TestCollision( const BoundBox& bound, const Vector2& new_pos_rel /*= Vector2::ZERO*/ ) const
{
	BoundBox bb = m_Bound.Translate(m_Position + new_pos_rel);

	return bb.TestBoxCollision(bound);
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

void Actor::Update()
{
	// Gravity
	m_Velocity += Vector2(0.0f, -0.03f);
}

void Actor::Translate( const Vector2& vecRel )
{
	m_Position += vecRel;
}

void Actor::Interactive()
{
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
