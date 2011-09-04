#include "EmdExporter.h"
#include "../../libEmdMesh/EmdDefine.h"

#define EMD_EXPORTER_CLASS_ID	Class_ID(0x68c00ebd, 0x410742d2)

class EmdMaxExporter : public SceneExport
{
public:
	static HWND hParams;

	int				ExtCount();					// Number of extensions supported
	const TCHAR *	Ext( int n );				// Extension #n (i.e. "3DS")
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1
	const TCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout( HWND hWnd );		// Show DLL's "About..." box

	int				DoExport( const MCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts/* =FALSE */, DWORD options/* =0 */ );
};

int EmdMaxExporter::ExtCount()
{
	return 1;
}

const TCHAR * EmdMaxExporter::Ext( int n )
{
	return _T("emd");
}

const TCHAR * EmdMaxExporter::LongDesc()
{
	return _T("Existence Mesh Exporter for Max 2011");
}

const TCHAR * EmdMaxExporter::ShortDesc()
{
	return _T("Emd Exporter");
}

const TCHAR * EmdMaxExporter::AuthorName()
{
	return _T("Mwolf");
}

const TCHAR * EmdMaxExporter::CopyrightMessage()
{
	return _T("");
}

const TCHAR * EmdMaxExporter::OtherMessage1()
{
	return _T("");
}

const TCHAR * EmdMaxExporter::OtherMessage2()
{
	return _T("");
}

unsigned int EmdMaxExporter::Version()
{
	return EMDL_VERSION;
}

void EmdMaxExporter::ShowAbout( HWND hWnd )
{

}

int EmdMaxExporter::DoExport( const MCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts/* =FALSE */, DWORD options/* =0 */ )
{

	typedef int(*ExportFunc)( const MCHAR*, ExpInterface*, Interface*, BOOL, DWORD );

	// Get full path of .dll which shares same path with .dle
	TCHAR binFullPath[_MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE] = { 0 },
		  szDir[_MAX_DIR] = { 0 },
		  szFNAME[_MAX_FNAME] = { 0 },
		  szExt[_MAX_EXT] = { 0 };

	GetModuleFileName( hInstance, binFullPath, sizeof(binFullPath) );

#ifdef UNICODE
	_wsplitpath( binFullPath, szDrive, szDir, NULL, NULL );
#else
	_splitpath( binFullPath, szDrive, szDir, NULL, NULL );
#endif

	TCHAR dllFullPath[_MAX_PATH] = { 0 };

#ifdef UNICODE

	wcscat( dllFullPath, szDrive );
	wcscat( dllFullPath, szDir );
#ifdef _DEBUG
	wcscat( dllFullPath, _T("EmdExporterMax2011_d.dll") );
#else
	wcscat( dllFullPath, _T("EmdExporterMax2011.dll") );
#endif	// #ifdef DEBUG

#else

	strcat( dllFullPath, szDrive );
	strcat( dllFullPath, szDir );
#ifdef _DEBUG
	strcat( dllFullPath, _T("EmdExporterMax2011_d.dll") );
#else
	strcat( dllFullPath, _T("EmdExporterMax2011.dll") );
#endif	// #ifdef DEBUG

#endif	// #ifdef UNICODE

	// Load exporter dll here and do the export
	HINSTANCE hDLL;
	hDLL = LoadLibrary( dllFullPath );

	ExportFunc exportFunc = (ExportFunc)GetProcAddress( hDLL, "DoExport" );

	// well, we don't have a DoExport method...
	if ( !exportFunc ) return FALSE;

	BOOL result = (*exportFunc)( name, ei, i, suppressPrompts, options );

	FreeLibrary( hDLL );

	return result;
}

class EmdExporterClassDesc : public ClassDesc2
{
public:
	int				IsPublic() { return TRUE; }
	void*			Create( BOOL loading = FALSE ) { return new EmdMaxExporter(); }
	const TCHAR*	ClassName() { return GetString( IDS_CLASS_NAME ); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return EMD_EXPORTER_CLASS_ID; }
	const TCHAR*	Category() { return _T(""); }				// No category specified for exporter

	const TCHAR*	InternalName() { return _T("EmdExporterMax2011"); }
	HINSTANCE		HInstance() { return hInstance; }
};

static EmdExporterClassDesc EmdExporterDesc;
ClassDesc2* GetEmdExporterDesc() { return &EmdExporterDesc; }
