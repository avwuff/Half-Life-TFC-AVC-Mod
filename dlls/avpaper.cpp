// Fly paper stuff
// Written by Avatar-X, started at 9:05 PM, Sunday May 27, 2001


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
#include "avradio.h"
#include "avdamage.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;
extern bool AdminLoggedIn[33];

bool PaperCreate( edict_t *pEntity )
{
	// Create the fly paper and stick it on the floor

	entvars_t *pPev = VARS( pEntity );

	if (pPev->iuser1 > 0) return 0;

	// make sure we dont already have fly paper

	int papercount = 0;
	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_flypaper", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{
					papercount++;
				}
			}
		}
	}

	
	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
	{
		if (papercount >= 1) {
			ClientPrint( pPev, HUD_PRINTTALK, "* Cant have more than one piece of fly paper at once!\n");
			return 0;
		}
	}

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, pEntity , &tr );

	if (tr.flFraction < 1.0)
	{
		if (tr.pHit && !(tr.pHit->v.flags & FL_CONVEYOR) && (FStrEq((char *)STRING(tr.pHit->v.classname), "worldspawn") || FStrEq((char *)STRING(tr.pHit->v.classname), "func_wall")))	// Make sure it isnt a conveyor!
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			if (angles.x != 90)
			{
				ClientPrint( pPev, HUD_PRINTTALK, "* Can't place fly paper on walls!\n");
				return 0;
			}

			angles.x -= 90;

			// Create the paper here!
			Vector vecOri = tr.vecEndPos; //+ tr.vecPlaneNormal * 14;
			//Vector vecOri = tr.vecEndPos + tr.vecPlaneNormal * 15;

			KeyValueData	kvd;
			
			edict_t *tEntity;
			tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
		
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(tEntity);
			
			// Set the KEYVALUES here!
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "building_flypaper";

			DispatchKeyValue( tEntity, &kvd );

			// place this in front

			pRunOnPev->origin = vecOri;
			pRunOnPev->solid = SOLID_BBOX;

			SET_ORIGIN( tEntity , vecOri );
			pRunOnPev->angles = angles;

			// DispatchSpawn( ENT( pRunOnPev ) );

			pRunOnPev->solid = SOLID_BBOX;

			SET_MODEL( ENT( pRunOnPev ) , "avatar-x/avadd43.avfil");

//			UTIL_MakeVectors( tr.vecPlaneNormal );

			UTIL_SetSize( pRunOnPev, Vector( -50, -50 , 0), Vector(50, 50, 4) );
			
			pRunOnPev->takedamage = DAMAGE_NO;
			pRunOnPev->max_health = 40 + 10000;
			pRunOnPev->health = 40 + 10000;
			pRunOnPev->euser4 = pEntity;
			//pRunOnPev->owner = pEntity;
			
			// play deploy sound
			EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);
			pRunOnPev->iuser2 = pRunOnPev->euser4->v.team; // Set the team this radio belongs to
			pRunOnPev->nextthink = gpGlobals->time + 0.3;
			
			// when it will "expire"
			pRunOnPev->fuser4 = gpGlobals->time + 5;
			
			if (AdminLoggedIn[ENTINDEX(pEntity)]) pRunOnPev->fuser4 = gpGlobals->time + 10;

			pEntity->v.origin.z += 10;

			return 1;
		}
		else
		{
			ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place fly paper here!\n");
			return 0;
		}
		
	}
	else
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place fly paper here!\n");
		
	}
	return 0;
}

void PaperPrecache()
{
	PRECACHE_MODEL("avatar-x/avadd43.avfil");
	PRECACHE_SOUND("bullchicken/bc_bite1.wav");
}

void PaperThink ( edict_t *pent )
{
	// has this flypaper expired?
	pent->v.nextthink = gpGlobals->time + 0.3;

	if (pent->v.fuser4 - gpGlobals->time < 0)
	{

		entvars_t *pev = VARS(pent);
	
		if (pev->fuser2 == 0)
		{
			pev->renderamt = 255;
			pev->rendermode = kRenderTransTexture;
			pev->fuser2 =1;
		}

		if ( pev->renderamt > 7 )
		{
			pev->renderamt -= 13;
			if (pev->renderamt < 1) pev->renderamt = 1;
			pev->nextthink = gpGlobals->time + 0.1;
		}
		else 
		{
			pev->renderamt = 1;
			// Kill it.
			pent->v.flags |= FL_KILLME;

			// Reset anyone who is stuck on it.
			int i = 1;
			int mynum = ENTINDEX( pent );
			char *pClassname;
			edict_t *frontEnt;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pClassname =  (char *)STRING(frontEnt->v.classname); 
					if (FStrEq("player", pClassname)) {

						if (frontEnt->v.vuser4.y == mynum) 
						{
							// yes he is stuck to me

							frontEnt->v.maxspeed = (int)frontEnt->v.vuser4.x;
							frontEnt->v.vuser4.x = 0;
							frontEnt->v.vuser4.y = 0;

						}
					}
				}
			}

		}		
	}
	// see if we have been destroyed
	if (!pent->v.euser4 || pent->v.euser4->v.iuser1 != 0 || pent->v.iuser2 != pent->v.euser4->v.team)
	{
		// We've been destroyed! Make a boom and tell the euser4

		if (pent->v.euser4) {
			
			ClientPrint( VARS( pent->v.euser4 ), HUD_PRINTTALK, "* Your flypaper has been destroyed!\n");
		}
		pent->v.flags |= FL_KILLME;

		int i = 1;
		int mynum = ENTINDEX( pent );
		char *pClassname;
		edict_t *frontEnt;

		for (i; i < 1025; i++) {

			frontEnt = INDEXENT ( i );
			if (frontEnt) {
				pClassname =  (char *)STRING(frontEnt->v.classname); 
				if (FStrEq("player", pClassname)) {

					if (frontEnt->v.vuser4.y == mynum) 
					{
						// yes he is stuck to me

						frontEnt->v.maxspeed = (int)frontEnt->v.vuser4.x;
						frontEnt->v.vuser4.x = 0;
						frontEnt->v.vuser4.y = 0;

					}
				}
			}
		}

	}

}

void PaperTouch( edict_t *pent, edict_t *pentTouch )
{
	// if someone touches me, make them "stick" to me until i have expired.
	
	if (AdminLoggedIn[ENTINDEX(pentTouch)]) return;
	entvars_t *pev = VARS(pentTouch);
	
	
	if (FStrEq("player", (char *)STRING(pentTouch->v.classname))) 
	{
		
		// Store their max speed
		
		

		if (pev->vuser4.y > 0) return;
	
		if (pev->maxspeed > 1) pev->vuser4.x = pev->maxspeed;
		
		// Store which ent we are stuck on.

		pev->vuser4.y = ENTINDEX(pent);

		// Make them stick.
		
		
		pev->speed = 1;
		pev->maxspeed = 1;
		pev->velocity = g_vecZero;
		pev->basevelocity = g_vecZero;
		pev->clbasevelocity = g_vecZero;
		
		//pev->origin.z -= 1;

		EMIT_SOUND_DYN2( pent, CHAN_VOICE, "bullchicken/bc_bite1.wav", 1.0, ATTN_NORM , 0, 100);

		ClientPrint( pev, HUD_PRINTTALK, "Oh no! You've been caught in the fly paper!\n");
	}
}