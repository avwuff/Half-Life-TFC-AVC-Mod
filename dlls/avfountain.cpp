
#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "soundent.h"

#include "studio.h"
#include "bot.h"
#include "avdll.h"
#include "sadll.h"
#include "avdamage.h"
#include "avfountain.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

extern bool AdminLoggedIn[33];

int FountainCreate( edict_t *pEntity )
{

	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0) return 0;

	// Create the fountain and stick to the wall
	entvars_t *pPev = VARS( pEntity );

	if (pPev->iuser1 > 0) return 0;

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecOri = pPev->origin + vecAiming * 128;

	KeyValueData	kvd;
	
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("env_sprite"));

	entvars_t *pRunOnPev;
	pRunOnPev =  VARS(tEntity);
	
	char buf[80];
	sprintf( buf, "%s", "building_fountain");
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = buf;

	DispatchKeyValue( tEntity, &kvd );

	// place this in front

	pRunOnPev->origin = vecOri;
	pRunOnPev->angles.y = (pPev->v_angle.y) + 180;
	
	SET_MODEL( ENT( pRunOnPev ) , "avatar-x/avadd46.avfil");
	UTIL_SetSize( pRunOnPev, Vector( -35, -35 ,0), Vector(35, 35, 50));
	
	pRunOnPev->takedamage = DAMAGE_NO;
	pRunOnPev->health = 40;
	pRunOnPev->euser4 = pEntity;
	
	pRunOnPev->solid = SOLID_BBOX;

	SET_ORIGIN( tEntity , vecOri );
	DROP_TO_FLOOR( tEntity );
	
	// Spawn routine
	
	pRunOnPev->movetype = MOVETYPE_NONE;

	pRunOnPev->nextthink = gpGlobals->time;

	pRunOnPev->effects = 0;
	pRunOnPev->frame = 0;

	pRunOnPev->fuser1 = gpGlobals->time;
	pRunOnPev->fuser2 = (float) MODEL_FRAMES( pRunOnPev->modelindex ) - 1;
	pRunOnPev->framerate = 20;

	EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "ambience/waterfall3.wav", 1.0, ATTN_NORM , 0, 100);
	return 1;

}

void FountainPrecache()
{

	
	PRECACHE_MODEL("avatar-x/avadd46.avfil");
	PRECACHE_SOUND("ambience/waterfall3.wav");

}

void FountainThink ( edict_t *pent )
{
	
	entvars_t *pev = VARS( pent );


	float frames = pev->framerate * (gpGlobals->time - pev->fuser1) ;

	pev->frame += frames;
	if ( pev->frame > pev->fuser2 )
	{
		if ( pev->fuser2 > 0 )
			pev->frame = fmod( pev->frame, pev->fuser2 );
	}

	pev->nextthink		= gpGlobals->time + 0.1;
	pev->fuser1			= gpGlobals->time;


}

void FountainTouch(edict_t *pent, edict_t *pentTouch)
{

	if (FClassnameIs( pentTouch, "player"))
	{
		
		entvars_t *pev = VARS( pentTouch);

		if (pev->health < pev->max_health)
		{
			pev->health++;
		}
	}
}