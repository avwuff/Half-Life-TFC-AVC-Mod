
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
#include "avsign.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

// This CPP deals with creating the engineers CAMERA (The THIRD thing they can build)
// Basics: We use the TRIPMINE model, and stick it onto the wall just as we would a tripmine!





signtext_t texts[1025];

extern bool AdminLoggedIn[33];

bool SignRemove( edict_t *pEntity )
{

	
	int radiocount = 0;
	int i = 1;
	char *pClassname;
	edict_t *frontEnt;
	edict_t *oldSign;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_sign", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{
					radiocount++;
					oldSign = frontEnt;
				}
			}
		}
	}

	
	if (radiocount >= 1) {
		// blow up old sign
		oldSign->v.flags |= FL_KILLME;
		ClientPrint( VARS(pEntity), HUD_PRINTTALK, "* Sign removed!\n");
	}
	return 1;
	
}

bool SignCreate( edict_t *pEntity )
{
	// Create the sign and stick to the wall

	entvars_t *pPev = VARS( pEntity );

	if (pPev->iuser1 > 0) return 0;


	// make sure we dont already have a radio

	int radiocount = 0;
	int i = 1;
	char *pClassname;
	edict_t *frontEnt;
	edict_t *oldSign;

	if (strlen((char*) Cmd_Argv(1)) < 1) return 0;


	if (strlen((char *)Cmd_Args()) > 79)
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Sign text too long!\n");
		return 0;
	}
	if (strlen((char *)Cmd_Args()) < 2)
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Sign text too short!\n");
		return 0;
	}

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_sign", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{
					radiocount++;
					oldSign = frontEnt;
				}
			}
		}
	}

	
	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
	{
		if (radiocount >= 1) {
			// blow up old sign

			oldSign->v.flags |= FL_KILLME;
		}
	}

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, pEntity , &tr );

	if (tr.flFraction < 1.0)
	{

		
		if (tr.pHit && !(tr.pHit->v.flags & FL_CONVEYOR) && (FStrEq((char *)STRING(tr.pHit->v.classname), "worldspawn") || FStrEq((char *)STRING(tr.pHit->v.classname), "func_wall") || FStrEq((char *)STRING(tr.pHit->v.classname), "building_dancemachine")))	// Make sure it isnt a conveyor!
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			if (angles.x > 30 || angles.x < -30)
			{
				ClientPrint( pPev, HUD_PRINTTALK, "* Can't place signs on floors or cielings!\n");
				return 0;
			}
			// Create the camera here!
			Vector vecOri = tr.vecEndPos + tr.vecPlaneNormal * 2;

			KeyValueData	kvd;
			
			edict_t *tEntity;
			tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
		
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(tEntity);
			
			char buf[80];
			sprintf( buf, "%s", "building_sign");

			// Set the KEYVALUES here!
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = buf;

			DispatchKeyValue( tEntity, &kvd );

			// place this in front

			pRunOnPev->origin = vecOri;
			
			pRunOnPev->angles = angles;

			DispatchSpawn( ENT( pRunOnPev ) );

			SET_MODEL( ENT( pRunOnPev ) , "models/avnewsign.mdl");
			UTIL_SetSize( pRunOnPev, Vector( -6, -6 ,-14), Vector(6, 6, 14));
			
			pRunOnPev->takedamage = DAMAGE_NO;
			pRunOnPev->euser4 = pEntity;
				
			tEntity->v.solid = SOLID_BBOX;

			// play deploy sound
			EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);
			
			pRunOnPev->iuser2 = pRunOnPev->euser4->v.team; // Set the team this radio belongs to
			pRunOnPev->nextthink = gpGlobals->time + 1;
			pRunOnPev->fuser4 = gpGlobals->time + 70;

			pRunOnPev->skin = RANDOM_LONG( 0, 4 );

			if (FStrEq(Cmd_Argv(0), "signbusy")) pRunOnPev->skin = 6;
			if (FStrEq(Cmd_Argv(0), "signavsays")) pRunOnPev->skin = 5;

			sprintf( texts[ENTINDEX(tEntity)].text , "%s", (char *)Cmd_Args());

			SET_ORIGIN( ENT(pRunOnPev) , pRunOnPev->origin );

			return 1;
		}
		else
		{
			ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place sign here!\n");
			return 0;
		}
		
	}
	else
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place sign here!\n");
		
	}
	return 0;
}


void SignPrecache()
{

	
	PRECACHE_MODEL("models/avnewsign.mdl");
	
}

void SignThink ( edict_t *pent )
{
	
	// make sure my owner is still alive
	pent->v.nextthink = gpGlobals->time + 2;
/*
	if (CVAR_GET_FLOAT("deathmatch"))
	{
		int hiswon = pfnGetPlayerWONId( pent->v.euser4 );
		if (hiswon <= 0)	
		{
			pent->v.flags |= FL_KILLME;
		}
	}

  */
	if (FNullEnt(pent->v.euser4)) pent->v.flags |= FL_KILLME;


}

void SignTouch ( edict_t *pent , edict_t *pentTouched)
{
			
	if (FStrEq("player", (char *)STRING(pentTouched->v.classname))) 
	{
		// player is touching us... make sure we arent stuck in him

		bool stuck = 0;
		entvars_t *pev = VARS(pentTouched);
		entvars_t *sign = VARS(pent);

		stuck = 1;

		if ( sign->absmin.x + 1 > pev->absmax.x - 1 ||
			 sign->absmin.y + 1 > pev->absmax.y - 1 ||
			 sign->absmin.z + 1 > pev->absmax.z - 1 ||
			 sign->absmax.x - 1 < pev->absmin.x + 1 ||
			 sign->absmax.y - 1 < pev->absmin.y + 1 ||
			 sign->absmax.z - 1 < pev->absmin.z + 1 ) stuck = 0;

		if (stuck)
		{
			// player is blocked, remove sign
			sign->flags |= FL_KILLME;
			ClientPrint( pev, HUD_PRINTTALK, "* Sign removed because you were stuck in it!\n");
		}
	}
/*
	if (FStrEq("func_ladder", (char *)STRING(pentTouched->v.classname))) 
	{
		pent->v.flags |= FL_KILLME;
	}
	else if (FStrEq("func_door", (char *)STRING(pentTouched->v.classname))) 
	{
		pent->v.flags |= FL_KILLME;
	}
	else if (FStrEq("func_rot_door", (char *)STRING(pentTouched->v.classname))) 
	{
		pent->v.flags |= FL_KILLME;
	}
	else if (FStrEq("func_train", (char *)STRING(pentTouched->v.classname))) 
	{
		pent->v.flags |= FL_KILLME;
	}
*/
}

