
#include <max.h>

#include <IGame/IGame.h>

#include "RootExporter.h"

extern "C" __declspec( dllexport ) int DoExport( const MCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options );

int DoExport( const MCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options )
{
	UserCoord exporterCoord =
	{
		1,		// right-handed coordinate
		1,		// X axis goes right
		2,		// Y axis goes up
		5,		// Z axis goes out
		1,		// U goes right
		0,		// V goes up
	};

	//// d3d setup
	//UserCoord exporterCoord =
	//{
	//	0,		// left-handed coordinate
	//	0,		// X axis goes left
	//	2,		// Y axis goes up
	//	5,		// Z axis goes out
	//	1,		// U goes right
	//	1,		// V goes down
	//};

	IGameScene*		igScene = GetIGameInterface();

	IGameConversionManager* cm = GetConversionManager();
	cm->SetUserCoordSystem(exporterCoord);

	igScene->InitialiseIGame();

	// export the very first frame
	igScene->SetStaticFrame( 0 );

	RootExporter rootExporter;

	// go through all top nodes
	for ( int i=0; i<igScene->GetTopLevelNodeCount(); i++ )
	{
		IGameNode* gameNode = igScene->GetTopLevelNode( i );
		rootExporter.AddNode( gameNode );
	}

	rootExporter.SaveToFile( name );

	// done
	igScene->ReleaseIGame();

	return TRUE;
}
