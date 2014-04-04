#ifndef __PROXY_MAIN_H__
#define __PROXY_MAIN_H__

// called before the library is unloaded
// if the system is just reconfiguring, pass destroyWindow = qfalse,
// which will keep the screen from flashing to the desktop.
void	PROXY_Shutdown( qboolean destroyWindow );

// All data that will be used in a level should be
// registered before rendering any frames to prevent disk hits,
// but they can still be registered at a later time
// if necessary.
//
// BeginRegistration makes any existing media pointers invalid
// and returns the current gl configuration, including screen width
// and height, which can be used by the client to intelligently
// size display elements
void	PROXY_BeginRegistration( glconfig_t *config );
qhandle_t PROXY_RegisterModel( const char *name );
qhandle_t PROXY_RegisterSkin( const char *name );
qhandle_t PROXY_RegisterShader( const char *name );
qhandle_t PROXY_RegisterShaderNoMip( const char *name );
void	PROXY_LoadWorld( const char *name );

// the vis data is a large enough block of data that we go to the trouble
// of sharing it with the clipmodel subsystem
void	PROXY_SetWorldVisData( const byte *vis );

// EndRegistration will draw a tiny polygon with each texture, forcing
// them to be loaded into card memory
void	PROXY_EndRegistration( void );

// a scene is built up by calls to R_ClearScene and the various R_Add functions.
// Nothing is drawn until R_RenderScene is called.
void	PROXY_ClearScene( void );
void	PROXY_AddRefEntityToScene( const refEntity_t *re );
void	PROXY_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts, int num );
int		PROXY_LightForPoint( vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir );
void	PROXY_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b );
void	PROXY_AddAdditiveLightToScene( const vec3_t org, float intensity, float r, float g, float b );
void	PROXY_RenderScene( const refdef_t *fd );

void	PROXY_SetColor( const float *rgba );	// NULL = 1,1,1,1
void	PROXY_DrawStretchPic ( float x, float y, float w, float h, 
	float s1, float t1, float s2, float t2, qhandle_t hShader );	// 0 = white

// Draw images for cinematic rendering, pass as 32 bit rgba
void	PROXY_DrawStretchRaw (int x, int y, int w, int h, int cols, int rows, const byte *data, int client, qboolean dirty);
void	PROXY_UploadCinematic (int w, int h, int cols, int rows, const byte *data, int client, qboolean dirty);

void	PROXY_BeginFrame( stereoFrame_t stereoFrame );

// if the pointers are not NULL, timing info will be returned
void	PROXY_EndFrame( int *frontEndMsec, int *backEndMsec );


int		PROXY_MarkFragments( int numPoints, const vec3_t *points, const vec3_t projection,
				int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t *fragmentBuffer );

int		PROXY_LerpTag( orientation_t *tag,  qhandle_t model, int startFrame, int endFrame, 
					float frac, const char *tagName );
void	PROXY_ModelBounds( qhandle_t model, vec3_t mins, vec3_t maxs );

#ifdef __USEA3D
void    PROXY_A3D_RenderGeometry (void *pVoidA3D, void *pVoidGeom, void *pVoidMat, void *pVoidGeomStatus);
#endif
void	PROXY_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font);

//void	PROXY_RemapShader(const char *oldShader, const char *newShader, const char *offsetTime);
//qboolean PROXY_GetEntityToken( char *buffer, int size );
//qboolean PROXY_inPVS( const vec3_t p1, const vec3_t p2 );


#endif
