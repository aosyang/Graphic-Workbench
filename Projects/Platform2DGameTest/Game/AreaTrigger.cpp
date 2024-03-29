/********************************************************************
	created:	2011/10/27
	filename: 	AreaTrigger.cpp
	author:		Mwolf
	
	purpose:	Implementations for area trigger
*********************************************************************/
#include "AreaTrigger.h"
#include "Actor.h"
#include "Renderer/GWRenderDevice.h"
#include <string.h>

TriggerFuncTable trigger_func_table[] =
{
	{ "TRIGGER_HURT_FIRE",			TriggerHurtFireFunc },
	{ "TRIGGER_FORCE_UPWARD",		TriggerForceUpwardFunc	},
	{ "",							NULL				},
};

TriggerFuncTable* GetTriggerFuncTable()
{
	return trigger_func_table;
}

TriggerFunc StringToTriggerFunc(const char* name)
{
	for ( int i=0; trigger_func_table[i].callback != NULL; i++ )
	{
		if (strcmp(trigger_func_table[i].name, name) == 0)
			return trigger_func_table[i].callback;
	}

	return NULL;
}

const char* TriggerFuncToString(TriggerFunc func)
{
	for ( int i=0; trigger_func_table[i].callback != NULL; i++ )
	{
		if (trigger_func_table[i].callback == func)
			return trigger_func_table[i].name;
	}

	return "";
}

static AREA_TRIGGER*	AreaTriggerListHead = NULL;
static AREA_TRIGGER*	AreaTriggerListTail = NULL;

AREA_TRIGGER* GetFirstAreaTrigger()
{
	return AreaTriggerListHead;
}

AREA_TRIGGER* GetNextAreaTrigger( AREA_TRIGGER* area_trigger )
{
	return area_trigger->next;
}

void AddAreaTriggerToGame( AREA_TRIGGER* area_trigger )
{
	area_trigger->next = NULL;

	if (AreaTriggerListTail == NULL)
	{
		// Build new linked list
		AreaTriggerListHead = area_trigger;
		AreaTriggerListTail = area_trigger;
	}
	else
	{
		// Add new area_trigger to the tail
		AreaTriggerListTail->next = area_trigger;
		AreaTriggerListTail = area_trigger;
	}
}

void RemoveAreaTriggerFromGame( AREA_TRIGGER* area_trigger )
{
	if ( area_trigger == NULL ) return;

	AREA_TRIGGER* area_trigger_elem = GetFirstAreaTrigger();

	if ( AreaTriggerListHead == area_trigger )
	{
		if ( AreaTriggerListTail == AreaTriggerListHead )
		{
			// Last element, clean the list
			AreaTriggerListHead = AreaTriggerListTail = NULL;
		}
		else
		{
			AreaTriggerListHead = AreaTriggerListHead->next;
		}

		delete area_trigger;

		return;
	}

	while ( area_trigger_elem )
	{
		if ( area_trigger_elem->next == area_trigger )
		{
			area_trigger_elem->next = area_trigger->next;

			// Move tail pointer forward if tail removed
			if ( AreaTriggerListTail == area_trigger )
				AreaTriggerListTail = area_trigger_elem;

			delete area_trigger;
			return;
		}

		area_trigger_elem = GetNextAreaTrigger( area_trigger_elem );
	}
}

void DebugDrawTriggers( int world_id )
{
	AREA_TRIGGER* trigger = GetFirstAreaTrigger();

	while (trigger)
	{
		if (world_id == trigger->world_id)
			RenderSystem::DrawColoredSprite(trigger->bound.vMin(), trigger->bound.vMax(), GWColor(0.867f, 0.525f, 0.467f, 0.5f));

		trigger = GetNextAreaTrigger(trigger);
	}
}

AREA_TRIGGER* PickAreaTriggerAtPoint( const Vector2& pos, int world_id )
{
	AREA_TRIGGER* trigger = GetFirstAreaTrigger();
	while (trigger)
	{
		if (trigger->world_id == world_id && 
			trigger->bound.IsPointInsideBox(pos))
		{
			return trigger;
		}

		trigger = GetNextAreaTrigger(trigger);
	}

	return NULL;
}

void TriggerHurtFireFunc( AREA_TRIGGER* trigger, Actor* actor )
{
}

void TriggerForceUpwardFunc( AREA_TRIGGER* trigger, Actor* actor )
{
	actor->Velocity().y += 0.05f;
}
