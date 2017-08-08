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
#include "avleash.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

extern bool AdminLoggedIn[33];
extern origentdata_t EntData[1025];



int LeashBoxCreate( edict_t *pEntity )
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
	sprintf( buf, "%s", "building_leashbox");
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = buf;

	DispatchKeyValue( tEntity, &kvd );

	// place this in front

	pRunOnPev->origin = vecOri;
	pRunOnPev->angles.y = (pPev->v_angle.y) + 180;
	
	SET_MODEL( ENT( pRunOnPev ) , "models/crystal.mdl");
	UTIL_SetSize( pRunOnPev, Vector( -15, -15 ,0), Vector(15, 15, 70));
	
	pRunOnPev->takedamage = DAMAGE_NO;
	pRunOnPev->health = 40;
	pRunOnPev->euser4 = pEntity;
	
	pRunOnPev->solid = SOLID_BBOX;

	SET_ORIGIN( tEntity , vecOri );
	DROP_TO_FLOOR( tEntity );
	
	// Spawn routine
	
	pRunOnPev->movetype = MOVETYPE_NONE;

	pRunOnPev->nextthink = gpGlobals->time + 0.1;

	pRunOnPev->effects = 0;
	pRunOnPev->frame = 0;
	
	sprintf( EntData[ENTINDEX(tEntity)].digitgroup, "%s", Cmd_Argv(1));

	return 1;
}

void LeashBoxPrecache()
{

	
	PRECACHE_MODEL("models/crystal.mdl");

}

void LeashBoxThink ( edict_t *pent )
{
	// find people who are within 50 units of this thing and who are supposed to be here
	bool sret = 0;

	edict_t *pEntity = NULL;
	int mynum = ENTINDEX(pent);

	pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "player" );
	while (!FNullEnt(pEntity))
	{

		// see if this is one

		if (FStrEq( EntData[ENTINDEX(pEntity)].digitgroup, EntData[mynum].digitgroup))
		{

			ALERT(at_console, "Found %s!\n", STRING(pEntity->v.netname));
			
			// move towards



			Vector vecGo = pent->v.origin - pEntity->v.origin;
			
			if (vecGo.Length() > 200)
			{
				vecGo = vecGo.Normalize();
				pEntity->v.velocity = vecGo * 600;
				sret = 1;

			}
		}
		pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "player" );
	}

	entvars_t *pev = VARS( pent );
	pev->nextthink		= gpGlobals->time + 0.1;
	
	if (sret == 1)
	{

		pent->v.renderfx = 19;
		pent->v.renderamt = 100;
		pent->v.rendercolor.z = 255;
	}
	else
	{
		pent->v.renderfx = 0;
	}

}

void LeashBoxTouch(edict_t *pent, edict_t *pentTouch)
{

}