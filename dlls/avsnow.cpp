// handles all the snowball stuff, including the weapon and the snow itself.

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
#include "avsnow.h"
#include "avdamage.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

int g_sModelIndexSnowBeam = 0;
int m_snowModel = 0;

extern int g_sModelIndexFireball2;
extern bool AdminLoggedIn[33];

static FILE *fp;

// keep an array of our snowball charging time.
float snowtimes[33];
int snowmode[33];
int snowammo[33];


void SnowPrecache()
{
	PRECACHE_MODEL("avatar-x/avadd33.avfil");
	PRECACHE_MODEL("avatar-x/avadd39.avfil");
	PRECACHE_MODEL("avatar-x/avadd32.avfil");
	
	PRECACHE_SOUND("avatar-x/avadd35.avfil");
	PRECACHE_SOUND("avatar-x/avadd36.avfil");
	PRECACHE_SOUND("avatar-x/avadd37.avfil");
	
//	PRECACHE_SOUND("../sprites/weapon_snowball.txt");

	g_sModelIndexSnowBeam = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	m_snowModel = PRECACHE_MODEL( "avatar-x/avadd34.avfil" );

}

void SnowShowWep( edict_t *pEntity )
{	
	
	// Show the smoke grenade in this guys hand

	entvars_t *pev = VARS( pEntity );
	
	pev->viewmodel = MAKE_STRING("avatar-x/avadd39.avfil");
	pev->weaponmodel = MAKE_STRING("avatar-x/avadd33.avfil");
	pev->weaponanim = 5;
		
	if (snowammo[ENTINDEX(pEntity)] > 0)
	{
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( 5 );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( 7 );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
	}

	
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pEntity );
		WRITE_BYTE( 1 );		// state
		WRITE_BYTE( 26 );	// id
		WRITE_BYTE( -1 );	// clip
	MESSAGE_END();

}
void SnowAttackStart( edict_t *pEntity )
{
	// Started holding down the button.
	if (pEntity->v.iuser1 > 0) return;
	if (snowtimes[ENTINDEX(pEntity)] < gpGlobals->time && snowammo[ENTINDEX(pEntity)] > 0)
	{

		entvars_t *pPev = VARS( pEntity );
		// make sure we dont already have more than 3 tripmines
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( 2 );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
		
		// track when we started holding down button
		snowtimes[ENTINDEX(pEntity)] = gpGlobals->time;
		snowmode[ENTINDEX(pEntity)] = 1;
	}
}


void SnowReload( edict_t *pEntity )
{
	// Started holding down the button.
	if (pEntity->v.iuser1 > 0) return;
	if (snowtimes[ENTINDEX(pEntity)] < gpGlobals->time && snowammo[ENTINDEX(pEntity)] < 5)
	{
		// set reload state.
		snowmode[ENTINDEX(pEntity)] = 4;
		snowtimes[ENTINDEX(pEntity)] = gpGlobals->time + 0.9;
		
		// remove the weapon
		if (snowammo[ENTINDEX(pEntity)] > 0)
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 4 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 6 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
	}
}

void SnowSpawn( edict_t *pEntity )
{
	// called when someone spawns

	snowammo[ENTINDEX(pEntity)] = 5;
	snowmode[ENTINDEX(pEntity)] = 3;
	snowtimes[ENTINDEX(pEntity)] = gpGlobals->time + 0.5;

}

#define WEAPON_ALLWEAPONS		(~(1<<WEAPON_SUIT))

void SnowTick( edict_t *pEntity )
{
	int mynum = ENTINDEX(pEntity);

	if (snowmode[mynum] == 3) 
	{
		if (snowtimes[mynum] < gpGlobals->time)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 16 );
				WRITE_BYTE( snowammo[ENTINDEX(pEntity)] );  // say how many snowballs we have
			MESSAGE_END();
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 17 );
				WRITE_BYTE( 1 );  // say how many snowballs we have
			MESSAGE_END();

			if (CVAR_GET_FLOAT("sa_onlysnow") == 1) 
			{
				// disable all other weapons
				
				pEntity->v.weapons &= ~WEAPON_ALLWEAPONS;
				pEntity->v.weapons |= (1<<26);

				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pEntity );
					WRITE_BYTE( 26 );
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pEntity );
					WRITE_BYTE(1);
					WRITE_BYTE(26);
					WRITE_BYTE(16);
				MESSAGE_END();

				CLIENT_COMMAND( pEntity, "weapon_9mmar\n");
				


			}

			snowmode[mynum] = 0;
		}
	}
	if (snowmode[mynum] == 4) 
	{

		if (snowtimes[mynum] < gpGlobals->time)
		{
			snowammo[ENTINDEX(pEntity)]++;
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 16 );
				WRITE_BYTE( snowammo[ENTINDEX(pEntity)] );  // say how many snowballs we have
			MESSAGE_END();
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 17 );
				WRITE_BYTE( 1 );  // say how many snowballs we have
			MESSAGE_END();

			// show it again
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 5 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
			snowmode[mynum] = 0;
		}
	}	else if (snowtimes[mynum] < gpGlobals->time && snowtimes[mynum] > 0 && snowmode[mynum] == 0)
	{
		snowtimes[mynum] = 0;
		if (snowammo[mynum] > 0)
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 5 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 7 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
	}
}

void SnowAttackFly( edict_t *pEntity )
{
	// let go of button


	if (pEntity->v.iuser1 > 0) return;

	// make sure we pressed it at the valid time
	if (snowtimes[ENTINDEX(pEntity)] > gpGlobals->time) return;
	if (snowmode[ENTINDEX(pEntity)] == 0) return;

		
	if (!AdminLoggedIn[ENTINDEX(pEntity)] && snowammo[ENTINDEX(pEntity)] > 0) snowammo[ENTINDEX(pEntity)]--;
	

	MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
		WRITE_BYTE( 16 );
		WRITE_BYTE( snowammo[ENTINDEX(pEntity)] );  // clamp the value to one byte
	MESSAGE_END();


	//ok, set when we can shoot again.
	snowtimes[ENTINDEX(pEntity)] = gpGlobals->time + 0.5;
	snowmode[ENTINDEX(pEntity)] = 0;

	entvars_t *pPev = VARS( pEntity );

	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
		WRITE_BYTE( 3 );						// sequence number
		WRITE_BYTE( 0 );					// weaponmodel bodygroup.
	MESSAGE_END();	

	// Make and throw the snowball
	
	edict_t *pent = CREATE_ENTITY();
	//edict_t *pent = CREATE_NAMED_ENTITY("info_target");
	entvars_t *pev = VARS( pent );
	
	pev->classname = MAKE_STRING("snowball");

	Vector anglesAim = pPev->v_angle + pPev->punchangle;
	UTIL_MakeVectors( anglesAim );

	anglesAim.x		 = -anglesAim.x;
	Vector vecSrc	 = GetGunPosition( pEntity ) + gpGlobals->v_up * 1 + gpGlobals->v_right * 3;
	Vector vecDir	 = gpGlobals->v_forward;

	pev->origin = vecSrc;
	pev->angles = anglesAim;
	pev->owner = pEntity;

	pev->takedamage = DAMAGE_NO;
	pev->health = 20;

	// The bolt doesn't move as fast under water...
	
	float timedown = gpGlobals->time - snowtimes[ENTINDEX(pEntity)];
	timedown = timedown / (float)10;
	timedown = timedown + 1;
	if (timedown > 2) timedown = 2;

	int spd = (int)((float)1000 * (float)timedown);
	
	pev->velocity = vecDir * spd;
	pev->speed = spd;

	pev->owner = pEntity;

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( ENTINDEX(pent) );	// entity
		WRITE_SHORT( g_sModelIndexSnowBeam );	// model
		WRITE_BYTE( 5 ); // life
		WRITE_BYTE( 3 );  // width
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 100 );	// brightness

	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	//pev->gravity = 0.5;

	SET_MODEL(pent, "avatar-x/avadd32.avfil"); // so i can see it use a big model

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	// play THROW sound

	EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd37.avfil", 1.0, ATTN_NORM , 0, 100);

	pev->nextthink = gpGlobals->time + 0.2;
}

void SnowBallThink( edict_t *pent )
{
	// Flying snowball think routine.
	
}

void SnowBallTouch( edict_t *pent, edict_t *pentTouch )
{

	entvars_t *pev = VARS( pent );
	TraceResult	tr;	

	// make a splat
	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine ( pev->origin - gpGlobals->v_forward * 4, pev->origin - gpGlobals->v_forward * 2 , ignore_monsters, pent, &tr);
	
	
	int ind = (int)DECAL_INDEX( "{break3" );


	//g_engfuncs.pfnStaticDecal( pev->origin , ind, 0, 0 );
	
	//UTIL_BloodDecalTrace( &tr, BLOOD_COLOR_RED );


	pent->v.flags |= FL_KILLME;

	int entityIndex=0;
	int msgsend = TE_WORLDDECAL;

	// if we hit a person or an ent, handle differently
	if ( pentTouch && !pentTouch->free && !FStrEq((char*)STRING(pentTouch->v.classname), "worldspawn"))
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( pentTouch );
		if ( pEntity && pEntity->IsBSPModel() )
		{
			entityIndex = ENTINDEX( pentTouch );
			msgsend = TE_DECAL;
		}

		// we hit a player!!!! make an "oof" sound
		if (FStrEq((char*)STRING(pentTouch->v.classname), "player"))
		{
			EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd36.avfil", 1.0, ATTN_NORM , 0, 100);
			if (pentTouch->v.team != pent->v.owner->v.team) AvTakeDamage( VARS(pentTouch), VARS(pent->v.owner), VARS(pent), 20, DMG_FREEZE);
		}
		else
		{
			// play hit sound
			EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd35.avfil", 1.0, ATTN_NORM , 0, 100);
		}
	}
	else 
	{
		// play HIT sound
		EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd35.avfil", 1.0, ATTN_NORM , 0, 100);
		entityIndex = 0;
	}
	
	//AvTakeDamage( VARS(pent->v.owner), VARS(pent->v.owner), VARS(pent), 20, DMG_FREEZE);
	

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( msgsend );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_BYTE( ind );
		if ( entityIndex )
			WRITE_SHORT( entityIndex );
	MESSAGE_END();

	Vector ori = pev->origin;
	
	Vector something = gpGlobals->v_forward * 20;

	ori.y = ori.y - something.y;
	ori.x = ori.x - something.x;
	ori.z = ori.z + something.z;

	for (int i=0;i<3;i++)
	{
		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(-90,90) + gpGlobals->v_up * RANDOM_FLOAT(-200,200) + gpGlobals->v_forward * RANDOM_FLOAT(0, 40);
		//vecShellVelocity = -vecShellVelocity;

		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_MODEL);
			WRITE_COORD( ori.x);
			WRITE_COORD( ori.y);
			WRITE_COORD( ori.z);
			WRITE_COORD( vecShellVelocity.x);
			WRITE_COORD( vecShellVelocity.y);
			WRITE_COORD( vecShellVelocity.z);
			//WRITE_ANGLE( pev->angles.y );
			WRITE_ANGLE( 0 );
			WRITE_SHORT( m_snowModel );
			WRITE_BYTE ( TE_BOUNCE_NULL );
			WRITE_BYTE ( 20 );// 2.5 seconds
		MESSAGE_END();
	}
	//SnowBlowUp( pent );

}
