// Av's TRIPMINE... essentially a copy of the HL one, but for TFC!


#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "soundent.h"
#include "externmsg.h"

#include "studio.h"
#include "bot.h"
#include "avdll.h"
#include "sadll.h"
#include "avtripmine.h"
#include "avdamage.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

int g_sModelIndexLaser = 0;
extern int g_sModelIndexFireball2;
extern bool AdminLoggedIn[33];

static FILE *fp;

bool thunkYet[1025];

int evilMode[1025];




extern playerinfo_t PlayerInfo[33];


void TripPrecache()
{
	PRECACHE_MODEL("models/v_tripmine.mdl");
	PRECACHE_MODEL("models/p_tripmine.mdl");
	PRECACHE_SOUND("weapons/mine_activate.wav");
	PRECACHE_SOUND("weapons/mine_charge.wav");
	
	g_sModelIndexLaser = PRECACHE_MODEL( "sprites/laserbeam.spr" );

}

void TripShowWep( edict_t *pEntity )
{	
	
	// Show the smoke grenade in this guys hand

	entvars_t *pev = VARS( pEntity );
	
	pev->viewmodel = MAKE_STRING("models/v_tripmine.mdl");
	pev->weaponmodel = MAKE_STRING("models/p_tripmine.mdl");
	pev->weaponanim = 6;
		
	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
		WRITE_BYTE( 6 );						// sequence number
		WRITE_BYTE( 0 );					// weaponmodel bodygroup.
	MESSAGE_END();

	
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pEntity );
		WRITE_BYTE( 1 );		// state
		WRITE_BYTE( 25 );	// id
		WRITE_BYTE( -1 );	// clip
	MESSAGE_END();

}
void TripAttack( edict_t *pEntity )
{
	// See if we can place a tripmine here.


	
	// create and place the tripmine entity

	entvars_t *pPev = VARS( pEntity );

	// make sure we dont already have more than 3 tripmines

	int radiocount = 0;
	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
	{
		for (i; i < 1025; i++) {

			frontEnt = INDEXENT ( i );
			if (frontEnt) {
				pClassname =  (char *)STRING(frontEnt->v.classname); 
				if (FStrEq("monster_tripmine", pClassname)) {

					if (frontEnt->v.euser4 == pEntity) 
					{
						radiocount++;
					}
				}
			}
		}
	}

	if (radiocount > 2)
	{
		ClientPrint( VARS(pEntity), HUD_PRINTTALK, "* You can't have more than 3 tripmines at a time!\n");
		return;
	}

	if (pEntity->v.iuser1 > 0) return;

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, pEntity , &tr );

	if (tr.flFraction < 1.0)
	{
		if (tr.pHit && !(tr.pHit->v.flags & FL_CONVEYOR))	// Make sure it isnt a conveyor!
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			// Create the camera here!

			KeyValueData	kvd;
			
			//edict_t *pent = CREATE_ENTITY();
			edict_t *pent = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
			entvars_t *pev = VARS( pent );
			
			pev->origin = tr.vecEndPos + tr.vecPlaneNormal * 8;
			pev->angles = angles;
			pev->euser4 = pEntity;
			pev->owner = pEntity;
			
			// for now don't take damage
			pev->takedamage = DAMAGE_YES;
			pev->max_health = 40 + 10000;
			pev->health = 40 + 10000;

			// Call the SPAWN routine to set more stuff
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "monster_tripmine";
			DispatchKeyValue( pent, &kvd );
			kvd.fHandled = FALSE;

			pev->movetype = MOVETYPE_FLY;
			//pev->solid = SOLID_BBOX;

			SET_MODEL(pent, "models/v_tripmine.mdl");
			
			pev->frame = 0;
			pev->body = 3;
			pev->sequence = 7;
			pev->framerate = 0;
			
			UTIL_SetSize(pev, Vector( -8, -8, -8), Vector(8, -2, 8));
			UTIL_SetOrigin( pev, pev->origin );

			// play deploy sound
			EMIT_SOUND_DYN2( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);
			EMIT_SOUND_DYN2( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM, 0, 100 ); // chargeup

			pev->iuser2 = pev->euser4->v.team; // Set the team this camera belongs to
			pev->nextthink = gpGlobals->time + 0.2; // after think
			pev->iuser1 = 1; // powerupthink
			
			UTIL_MakeAimVectors( pev->angles );

			pev->vuser1 = gpGlobals->v_forward;
			
			// end point
			pev->vuser2 = pev->origin + pev->vuser1 * 8192;

	
			pev->fuser1 = gpGlobals->time + 2.5;
			pev->dmg = 40;

			int MyNumber = ENTINDEX(pEntity);

			pev->rendercolor.x = PlayerInfo[MyNumber].tripmineR;
			pev->rendercolor.y = PlayerInfo[MyNumber].tripmineG;
			pev->rendercolor.z = PlayerInfo[MyNumber].tripmineB;


			evilMode[ENTINDEX(pent)] = PlayerInfo[MyNumber].tripMode;

			
			thunkYet[ENTINDEX(pent)] = 0;

			// do a little animation

			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 3 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
		else
		{
			//ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place camera here!\n");
			return;
		}
		
	}
	else
	{
		//ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place camera here!\n");
		
	}
	return;


}
void TripThink( edict_t *pent )
{
	// Tripmine think routine.
	int evmode = evilMode[ENTINDEX(pent)];

	entvars_t *pev = VARS( pent );
	
	if (pev->iuser1 == 1)  // powerup think
	{

		TraceResult tr;

		if (pev->euser1 == NULL)
		{
			
			if (thunkYet[ENTINDEX(pent)] == 0)
			{
				MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pent->v.euser4 );
					WRITE_BYTE( 6 );						// sequence number
					WRITE_BYTE( 0 );					// weaponmodel bodygroup.
				MESSAGE_END();
				thunkYet[ENTINDEX(pent)] = 1;
			}

			// find an owner
			edict_t *oldowner = pev->owner;
			pev->owner = NULL;
			//UTIL_TraceLine( pev->origin, pev->vuser2, dont_ignore_monsters, ENT( pev ), &tr );

			UTIL_TraceLine( pev->origin + pev->vuser1 * 8, pev->origin - pev->vuser1 * 32, dont_ignore_monsters, ENT( pev ), &tr );

			
			ALERT(at_console, "TRIPDBG: fStartSolid: %i, HIT OWNER: %s\n", tr.fStartSolid, tr.pHit == oldowner ? "YES" : "NO");

			
			if (tr.fStartSolid || (oldowner && tr.pHit == oldowner))
			{
				// we hit ourselves, so delay the end powerup until the owner is out of the way
				pev->owner = oldowner;
				pev->fuser1 += 0.1;
				pev->nextthink = gpGlobals->time + 0.1;
				return;
			}

			if (tr.flFraction < 1.0)
			{
				pev->owner = tr.pHit;
				pev->euser1 = pev->owner;
				pev->vuser3 = pev->owner->v.origin;
				pev->vuser4 = pev->owner->v.angles;
			}
			else
			{
				STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
				STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
				
				pev->flags |= FL_KILLME;
				pev->nextthink = gpGlobals->time + 0.1;
				return;
			}
		}
		else if (pev->vuser3 != pev->euser1->v.origin || pev->vuser4 != pev->euser1->v.angles)
		{
			// disable if the ent we are attached to has moved / turned
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
			STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
			
			// this is supposed to drop a pick-upable mine here
			//CBaseEntity *pMine = Create( "weapon_tripmine", pev->origin + m_vecDir * 24, pev->angles );
			//pMine->pev->spawnflags |= SF_NORESPAWN;

			
			//KillBeam();

			pev->flags |= FL_KILLME;
			pev->nextthink = gpGlobals->time + 0.1;

			return;
		}
		 
		if (gpGlobals->time > pev->fuser1)
		{
			// make solid
			pev->solid = SOLID_BBOX;
			UTIL_SetOrigin( pev, pev->origin );

			//MakeBeam( );
			pev->iuser1 = 2;
			// play enabled sound
			EMIT_SOUND_DYN2( ENT(pev), CHAN_VOICE, "weapons/mine_activate.wav", 0.5, ATTN_NORM, 1.0, 75 );

			// Attempt at creating beam here

			UTIL_TraceLine( pev->origin, pev->vuser2, dont_ignore_monsters, ENT( pev ), &tr );
			pev->fuser2 = tr.flFraction;

			// set to follow laser spot
			
			TripMakeBeam( pent );
			// set when to next make beam
			pev->fuser3 = gpGlobals->time + 5;
		}
		pev->nextthink = gpGlobals->time + 0.1;

	}
	else if (pev->iuser1 == 2)  // beambreak think
	{

		// REGULAR THINK


		int bBlowup = 0;

		TraceResult tr;

		if (evmode == 0 || evmode == 200 || evmode == 300)
		{

			// HACKHACK Set simple box using this really nice global!
			gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
			UTIL_TraceLine( pev->origin, pev->vuser2, dont_ignore_monsters, ENT( pev ), &tr );


			//pev->vuser2 = tr.vecEndPos;

			// respawn detect. 
			
			if (evmode == 0)
			{


				if (gpGlobals->time > pev->fuser3 && evmode == 0)
				{
					TripMakeBeam( pent );
					// set when to next make beam
					pev->fuser3 = gpGlobals->time + 5;
				}

				if (fabs( pev->fuser2 - tr.flFraction ) > 0.001)
				{
					bBlowup = 1;
				}

				if (tr.pHit && tr.pHit->v.team == pev->iuser2 && tr.pHit != pev->euser4) bBlowup = 0;
			}
			else if (!FNullEnt(tr.pHit))
			{

				if (FStrEq("player", STRING(tr.pHit->v.classname)))
				{
					// move player
					
					Vector newVel = tr.pHit->v.velocity;


					if (evmode == 200) newVel = newVel + (pev->vuser1 * 300);
					if (evmode == 300) newVel = newVel - (pev->vuser1 * 300);

					tr.pHit->v.velocity = newVel;

				}
			}
		}
		
		if (pev->euser1 == NULL)
			bBlowup = 2;
		else if (pev->vuser3 != pev->euser1->v.origin)
			bBlowup = 3;
		else if (pev->vuser4  != pev->euser1->v.angles)
			bBlowup = 4;

		// unless the person is on our team
		// or unless we hit ourselves

		
		if (bBlowup > 0)
		{
			
			ALERT(at_console, "BLOWUP FLAG: %i\n", bBlowup);

			// make me blow up
			TripBlowUp(pent);

			return;
		}

		pev->nextthink = gpGlobals->time + 0.1;



	}

	ALERT(at_console, "HEALTH: %f\n", pent->v.health);

	// see if we have been destroyed
	if (!pent->v.euser4 || pent->v.health <= 10000 || pent->v.euser4->v.iuser1 != 0 || pent->v.iuser2 != pent->v.euser4->v.team)
	{
		
		
		ALERT(at_console, "OTHER REASON BLOWUP\n");
		ALERT(at_console, "REASON 1: %i\n", (int)(!pent->v.euser4));
		ALERT(at_console, "REASON 2: %i\n", (int)(pent->v.health <= 10000));
		ALERT(at_console, "REASON 3: %i\n", (int)(pent->v.euser4->v.iuser1 != 0));
		ALERT(at_console, "REASON 4: %i\n", (int)(pent->v.iuser2 != pent->v.euser4->v.team));
		
		TripBlowUp(pent);
	}

}


void TripMakeBeam(edict_t *pent)
{
	entvars_t *pev = VARS( pent );
	// make the tripmine beam
	
	
	
	

	int evmode = evilMode[ENTINDEX(pent)];

	if (evmode == 0)
	{

		Vector vecTmpEnd = pev->origin + pev->vuser1 * 8192 * pev->fuser2;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pent->v.origin );

			WRITE_BYTE( TE_BEAMENTPOINT );
			WRITE_SHORT( ENTINDEX(pent) );

			WRITE_COORD( vecTmpEnd.x );
			WRITE_COORD( vecTmpEnd.y );
			WRITE_COORD( vecTmpEnd.z );
			
			WRITE_SHORT( g_sModelIndexLaser );
			
			WRITE_BYTE( 1 ); // start frame
			WRITE_BYTE( 10 ); // frame rate
			WRITE_BYTE( 52 ); // life
			WRITE_BYTE( 10 ); // line width
			WRITE_BYTE( 0 ); // noise
			
			WRITE_BYTE( pent->v.rendercolor.x );	// r
			WRITE_BYTE( pent->v.rendercolor.y );	// g
			WRITE_BYTE( pent->v.rendercolor.z );	// b
			
			WRITE_BYTE( 190 ); // brightness
			WRITE_BYTE( 255 ); // scroll speed

		MESSAGE_END();
	}
	else if (evmode == 1 || evmode == 20 || evmode == 30)
	{
	
		Vector vecTmpEnd = pev->origin + pev->vuser1 * 8192;

		//ALERT(at_console, "STARTING TRIPMINE EVIL MODE!!\n");


		// Set to EVIL MODE 2 - spawned.
		evilMode[ENTINDEX(pent)] = 2;
		if (evmode == 20) evilMode[ENTINDEX(pent)] = 200;
		if (evmode == 30) evilMode[ENTINDEX(pent)] = 300;
		
		// Mode 1 -- Create the laser and the target

		edict_t *pent2 = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
		pent2->v.origin = vecTmpEnd;
		pent2->v.owner = pent;

		// Set it's target name

		int itsnum = ENTINDEX(pent2);

		KeyValueData kvd;
		char buf2[40];

		// SET THE CLASSNAME
		kvd.fHandled = FALSE;
		kvd.szClassName = "info_target";
		kvd.szKeyName = "classname";
		kvd.szValue = "info_target";
		DispatchKeyValue( pent2, &kvd );
		
		// Set the TARGETNAME
		sprintf(buf2, "beamtarg_%i", itsnum);

		kvd.fHandled = FALSE;
		kvd.szClassName = "info_target";
		kvd.szKeyName = "targetname";
		kvd.szValue = buf2;
		DispatchKeyValue( pent2, &kvd );
		
		DispatchSpawn(pent2);

		pent->v.euser3 = pent2;


		// OK!
		// Now make the ENV_LASER!!!

		pent2 = CREATE_NAMED_ENTITY(MAKE_STRING("env_laser"));
		pent2->v.origin = pent->v.origin + (pent->v.vuser1 * 3);
		pent2->v.owner = pent;

		// set it's color

		pent2->v.rendercolor = pent->v.rendercolor;
		pent2->v.renderamt = 255;

		
		// SET THE CLASSNAME
		kvd.fHandled = FALSE;
		kvd.szClassName = "env_laser";
		kvd.szKeyName = "classname";
		kvd.szValue = "env_laser";
		DispatchKeyValue( pent2, &kvd );
		
		// Set the TARGETNAME
		sprintf(buf2, "beamtarg_%i", itsnum);

		kvd.fHandled = FALSE;
		kvd.szClassName = "env_laser";
		kvd.szKeyName = "LaserTarget";
		kvd.szValue = buf2;
		DispatchKeyValue( pent2, &kvd );

		// Set the WIDTH
		kvd.fHandled = FALSE;
		kvd.szClassName = "env_laser";
		kvd.szKeyName = "width";
		kvd.szValue = "10";
		if (evmode == 20 || evmode == 30)	kvd.szValue = "30";

		DispatchKeyValue( pent2, &kvd );

		// Set the TEXTURE
		kvd.fHandled = FALSE;
		kvd.szClassName = "env_laser";
		kvd.szKeyName = "texture";
		kvd.szValue = "sprites/laserbeam.spr";
		DispatchKeyValue( pent2, &kvd );

		// Set the DAMAGE
		kvd.fHandled = FALSE;
		kvd.szClassName = "env_laser";
		kvd.szKeyName = "damage";
		
		if (evmode == 20 || evmode == 30)	kvd.szValue = "0";
		else 								kvd.szValue = "10000";

		DispatchKeyValue( pent2, &kvd );

		// Set the SPAWNFLAGS
		kvd.fHandled = FALSE;
		kvd.szClassName = "env_laser";
		kvd.szKeyName = "spawnflags";
		kvd.szValue = "1";
		//kvd.szValue = "197";
		DispatchKeyValue( pent2, &kvd );

		// Set the TEXTURE SCROLL
		kvd.fHandled = FALSE;
		kvd.szClassName = "env_laser";
		kvd.szKeyName = "TextureScroll";
		kvd.szValue = "10";
		DispatchKeyValue( pent2, &kvd );



		// Set the TEAM
		//sprintf(buf2, "%i", pent->v.iuser2);

		//kvd.fHandled = FALSE;
		//kvd.szClassName = "env_laser";
		//kvd.szKeyName = "team_no";
		//kvd.szValue = buf2;
		//DispatchKeyValue( pent2, &kvd );	

		// make the thingy
		/*
		if (evmode == 10)
		{
		
			// find the centrepoint
			ALERT(at_console, "SPAWNING SOLID BLOCK...\n");	

			TraceResult tr;
			UTIL_TraceLine( pev->origin, pev->origin + pev->vuser1 * 8192, dont_ignore_monsters, ENT( pev ), &tr );
			
			// get the distance

			float tDist = 8192 * tr.flFraction;

			ALERT(at_console, "DISTANCE: %f\n", tDist);
		
			// halve the distance

			tDist /= 2;

			// spawn the good ol' solid block eh?

		

			edict_t *pent4 = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
			entvars_t *pev2 = VARS( pent4 );
			
			pev2->origin = pev->origin + pev->vuser1 * tDist;
			pev2->angles = pev->angles;
			pev2->owner = pent;
			
			// for now don't take damage
			pev2->takedamage = DAMAGE_NO;

			// Call the SPAWN routine to set more stuff
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "func_wall";
			DispatchKeyValue( pent4, &kvd );
			kvd.fHandled = FALSE;

			pev2->solid = SOLID_BBOX;

			SET_MODEL(pent4, "models/v_tripmine.mdl");
			
			pev2->frame = 0;
			pev2->body = 3;
			pev2->sequence = 7;
			pev2->framerate = 0;
			
			UTIL_SetSize(pev2, Vector( -8, -tDist, -8), Vector(8, tDist, 8));
			UTIL_SetOrigin( pev2, pev2->origin );

		}
*/

		// ALL DONE
		DispatchSpawn(pent2);
		pent->v.euser2 = pent2;

	}

}

void TripBlowUp(edict_t *pent)
{
	pent->v.flags |= FL_KILLME;
	// Boom
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pent->v.origin );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( pent->v.origin.x );	// Send to PAS because of the sound
		WRITE_COORD( pent->v.origin.y );
		WRITE_COORD( pent->v.origin.z );
		WRITE_SHORT( g_sModelIndexFireball2 );
		WRITE_BYTE( 30  ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( 0 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY );

		WRITE_BYTE( TE_KILLBEAM );
		WRITE_SHORT ( ENTINDEX(pent) );

	MESSAGE_END();	

	// kill the beams

	if (pent->v.euser3 != NULL)
	{
		pent->v.euser3->v.flags |= FL_KILLME;
	}
	if (pent->v.euser2 != NULL)
	{
		pent->v.euser2->v.flags |= FL_KILLME;
	}


	AvRadiusDamage( pent->v.origin, VARS(pent), VARS(pent->v.euser4), 120, 500, DMG_BLAST);
}