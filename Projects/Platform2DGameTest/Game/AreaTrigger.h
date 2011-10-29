/********************************************************************
	created:	2011/10/27
	filename: 	AreaTrigger.h
	author:		Mwolf
	
	purpose:	Definitions for area trigger 
*********************************************************************/
#ifndef AreaTrigger_h__
#define AreaTrigger_h__

#include "BoundBox.h"

class Actor;
struct AreaTrigger;

typedef void (*TriggerFunc)( AreaTrigger* trigger, Actor* actor );

struct TriggerFuncTable
{
	char			name[256];
	TriggerFunc		callback;
};

TriggerFuncTable* GetTriggerFuncTable();
TriggerFunc StringToTriggerFunc(const char* name);
const char* TriggerFuncToString(TriggerFunc func);

typedef struct AreaTrigger
{
	BoundBox		bound;
	int				world_id;
	TriggerFunc		callback;

	AreaTrigger*	next;
} AREA_TRIGGER;

AREA_TRIGGER* GetFirstAreaTrigger();
AREA_TRIGGER* GetNextAreaTrigger( AREA_TRIGGER* area_trigger );
void AddAreaTriggerToGame( AREA_TRIGGER* area_trigger );
void RemoveAreaTriggerFromGame( AREA_TRIGGER* area_trigger );

void DebugDrawTriggers(int world_id);

void TriggerHurtFireFunc(AREA_TRIGGER* trigger, Actor* actor);
void TriggerForceUpwardFunc(AREA_TRIGGER* trigger, Actor* actor);
AREA_TRIGGER* PickAreaTriggerAtPoint(const Vector2& pos, int world_id);

#endif // AreaTrigger_h__