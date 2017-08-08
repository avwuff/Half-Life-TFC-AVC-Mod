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
#include "avbfg.h"
#include "avdamage.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

int modelBFGAmmo = 0;
int m_drop = 0;
int g_sModelIndexFireball3;
int g_sModelIndexSmoke;


extern bool AdminLoggedIn[33];
extern int m_beamSprite;
extern int m_funnelSprite;


static FILE *fp;

// keep an array of our snowball charging time.
float bfgtimes[33];
int bfgmode[33];
int bfgammo[33];


void BFGPrecache()
{
	PRECACHE_MODEL("avatar-x/avadd40.avfil");
	PRECACHE_MODEL("avatar-x/avadd41.avfil");
	PRECACHE_MODEL("avatar-x/avadd42.avfil");
	
	PRECACHE_SOUND("weapons/flmgrexp.wav");
	PRECACHE_SOUND("ambience/biggun3.wav");

	
 
	//m_drop = PRECACHE_MODEL( "models/cactusgibs.mdl" );
	g_sModelIndexFireball3 = PRECACHE_MODEL( "sprites/hexplo.spr" );
	g_sModelIndexSmoke = PRECACHE_MODEL ("sprites/steam1.spr");// smoke
}

void BFGShowWep( edict_t *pEntity )
{	
	
	// Show the BFG in this guys hand
	entvars_t *pev = VARS( pEntity );
	
	pev->viewmodel = MAKE_STRING("avatar-x/avadd40.avfil");
	pev->weaponmodel = MAKE_STRING("avatar-x/avadd41.avfil");
		
	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
		WRITE_BYTE( 2 );						// sequence number
		WRITE_BYTE( 0 );					// weaponmodel bodygroup.
	MESSAGE_END();
	
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pEntity );
		WRITE_BYTE( 1 );		// state
		WRITE_BYTE( 28 );	// id
		WRITE_BYTE( -1 );	// clip
	MESSAGE_END();

}
void BFGAttackStart( edict_t *pEntity )
{
	// Started holding down the button.
	if (pEntity->v.iuser1 > 0) return;
	int mynum = ENTINDEX(pEntity);

	if (bfgtimes[ENTINDEX(pEntity)] < gpGlobals->time)
	{

		entvars_t *pPev = VARS( pEntity );
		
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( 1 );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
			
		// fire the weapon.
	
		if (!AdminLoggedIn[ENTINDEX(pEntity)] && bfgammo[ENTINDEX(pEntity)] > 0) bfgammo[ENTINDEX(pEntity)]--;

		// make and fire the BFG shot

		edict_t *pent;
		pent = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

		KeyValueData	kvd;

		char buf[80];
		sprintf( buf, "%s", "bfg_shot");
		// Set the KEYVALUES here!
		kvd.fHandled = FALSE;
		kvd.szClassName = NULL;
		kvd.szKeyName = "classname";
		kvd.szValue = buf;
		DispatchKeyValue( pent, &kvd );


//		edict_t *pent = CREATE_ENTITY();

		entvars_t *pev = VARS( pent );
				
		//pev->classname = MAKE_STRING("bfg_shot");

		Vector anglesAim = pPev->v_angle + pPev->punchangle;
		UTIL_MakeVectors( anglesAim );

		anglesAim.x		 = -anglesAim.x;
		Vector vecSrc	 = GetGunPosition( pEntity ) + gpGlobals->v_up * 1 + gpGlobals->v_right * 3;
		Vector vecDir	 = gpGlobals->v_forward;

		pev->origin = vecSrc;
		
		pev->owner = pEntity;
		pev->euser4 = pEntity;

		pev->takedamage = DAMAGE_NO;
		pev->health = 20;

		pev->velocity = vecDir * 300;
		pev->speed = 300;


		pev->renderfx = 19;
		pev->renderamt = 50;
		pev->rendercolor.y = 255;

		pev->movetype = MOVETYPE_FLY;
		pev->solid = SOLID_BBOX;
				
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMFOLLOW );
			WRITE_SHORT( ENTINDEX(pent) );	// entity
			WRITE_SHORT( m_beamSprite );	// model
			WRITE_BYTE( 20 ); // life
			WRITE_BYTE( 3 );  // width
			WRITE_BYTE( 0 );   // r
			WRITE_BYTE( 255 );   // g
			WRITE_BYTE( 0 );   // b
			WRITE_BYTE( 100 );	// brightness
		MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

		SET_MODEL(pent, "avatar-x/avadd42.avfil"); // so i can see it use a big model

		UTIL_SetOrigin( pev, pev->origin );
		UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

		// play BOOM sound!

		EMIT_SOUND_DYN2( pent, CHAN_VOICE, "weapons/flmgrexp.wav", 1.0, ATTN_NORM , 0, 100);

		//bfgtimes[mynum] = gpGlobals->time;

		pev->fuser1 = gpGlobals->time;
		pev->fuser1 = gpGlobals->time + 0.2;

		pev->nextthink = gpGlobals->time + 0.1;

		bfgtimes[mynum] = gpGlobals->time + 1.0;
		pev->angles = vecDir;
	}
}


void BFGReload( edict_t *pEntity )
{
	// Started holding down the button.
	if (pEntity->v.iuser1 > 0) return;

	if (bfgtimes[ENTINDEX(pEntity)] < gpGlobals->time && bfgammo[ENTINDEX(pEntity)] < 5)
	{
		// set reload state.

	}
}

void BFGSpawn( edict_t *pEntity )
{
	// called when someone spawns

	bfgammo[ENTINDEX(pEntity)] = 5;
	bfgmode[ENTINDEX(pEntity)] = 3;
	bfgtimes[ENTINDEX(pEntity)] = gpGlobals->time + 0.5;

}


void BFGTick( edict_t *pEntity )
{
	int mynum = ENTINDEX(pEntity);

}

void BFGAttackFly( edict_t *pEntity )
{
	// let go of button


}

edict_t *BFGFindTarget( edict_t *pent )
{

	bool found1 = 0;
	bool found2 = 0;
	CBaseEntity *pOther = NULL;
	Vector vecDir;
	entvars_t *pev = VARS(pent);
	float flDist, flMax;
	TraceResult tr;	
	flMax = 4096;

	edict_t *found = NULL;
	edict_t *foundme = NULL;

	// find a suitable target

	while ((pOther = UTIL_FindEntityByClassname( pOther, "player" )) != NULL)
	{
		UTIL_TraceLine ( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );
		// ALERT( at_console, "%f\n", tr.flFraction );
		if (tr.flFraction >= 0.90)
		{
			vecDir = pOther->pev->origin - pev->origin;
			flDist = vecDir.Length( );
			vecDir = vecDir.Normalize( );
			//flDot = DotProduct( gpGlobals->v_forward, vecDir );


			if (flDist < flMax && pOther->pev->deadflag == DEAD_NO && pOther->pev->iuser1 == 0 && pOther->pev->team > 0)
			{
				if (pOther->edict() == pev->euser4)
				{
					// We seem to have found me!
					foundme = pOther->edict();
					found2 = 1;
				}
				else
				{
					if (pOther->pev->team != pent->v.euser4->v.team)
					{
						found = pOther->edict();
						found1 = 1;
						flMax = flDist;
					}
				}
			}
		}
	}

	if (found1 == 0 && found2 == 1)
	{
		found = foundme;
	}

	return found;
}

void BFGThink( edict_t *pent )
{
	// Flying BFG shot think routine.
	
	entvars_t *pev = VARS(pent);

	//CBaseEntity *pOther = NULL;
	Vector vecTarget;
	//Vector vecDir;
	//Vector vecMeTarget;
	//float flDist, flMax;
	


	UTIL_MakeAimVectors( pev->angles );

	vecTarget = gpGlobals->v_forward;
	
	// See if we still have a valid target
	bool getnewtarget = 0;

	if (pev->euser1 != NULL)
	{
		
		// ok but make sure they arent a spec and arnt dead
		if (pev->euser1->v.deadflag != DEAD_NO) getnewtarget = 1;
		if (pev->euser1->v.iuser1 > 0) getnewtarget = 1;
	}
	else
	{
		getnewtarget = 1;
	}

	if (getnewtarget) pev->euser1 = BFGFindTarget( pent );

	if (pev->euser1 != NULL)
	{
		vecTarget = pev->euser1->v.origin - pev->origin;
		vecTarget = vecTarget.Normalize( );
	}

	pev->angles = UTIL_VecToAngles( vecTarget );

	/*
	if (gpGlobals->time > pev->fuser2)
	{

		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_MODEL);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( 0);
			WRITE_COORD( 0);
			WRITE_COORD( 0);
			WRITE_ANGLE( 0 );
			WRITE_SHORT( m_drop );
			WRITE_BYTE ( TE_BOUNCE_NULL );
			WRITE_BYTE ( 20 );// 2.5 seconds
		MESSAGE_END();
		
		pev->fuser2 = gpGlobals->time + RANDOM_FLOAT(0.3, 1.0);
		
	}
	*/

	if (gpGlobals->time - pev->fuser1 > 0.3)
		{

		if (pev->owner != NULL) 
		{
			pev->owner = NULL;
		}

		// Examine all entities within a reasonable radius


		pev->angles = UTIL_VecToAngles( vecTarget );

		// this acceleration and turning math is totally wrong, but it seems to respond well so don't change it.
		float flSpeed = pev->velocity.Length();
		if (gpGlobals->time - pev->fuser1 < 25)
		{

			int spdadd  = 600;

			// if we have been flying for more than 5 seconds, go twice as fast
			if (gpGlobals->time - pev->fuser1 > 4) {
				spdadd = 1800;
				
				if (pev->iuser3 == 0)
				{
					// Go into HYPER mode

					pev->iuser3 = 1;
					pev->renderamt = 150;
					pev->rendercolor.y = 0;
					pev->rendercolor.x = 255;

					MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
						WRITE_BYTE( TE_BEAMFOLLOW );
						WRITE_SHORT( ENTINDEX(pent) );	// entity
						WRITE_SHORT( m_beamSprite );	// model
						WRITE_BYTE( 20 ); // life
						WRITE_BYTE( 40 );  // width
						WRITE_BYTE( 255 );   // r
						WRITE_BYTE( 0 );   // g
						WRITE_BYTE( 0 );   // b
						WRITE_BYTE( 100 );	// brightness
					MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

				}
			}


			if (gpGlobals->time - pev->fuser1 > 3 && pev->iuser2 == 0) 
			{
				// if we have been flying for more than 3 seconds, spawn another BFG.
				pev->iuser2 = 1;

				//edict_t *pent2 = CREATE_ENTITY();
				
				edict_t *pent2 = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

				KeyValueData	kvd;

				char buf[80];
				sprintf( buf, "%s", "bfg_shot");
				// Set the KEYVALUES here!
				kvd.fHandled = FALSE;
				kvd.szClassName = NULL;
				kvd.szKeyName = "classname";
				kvd.szValue = buf;
				DispatchKeyValue( pent2, &kvd );

				
				entvars_t *pev2 = VARS( pent2 );
				
				

				//pev2->classname = MAKE_STRING("bfg_shot");

				pev2->origin = pev->origin;
				pev2->origin.z += 10;

				pev2->owner = pent;

				pev2->euser4 = pev->euser4;
				pev2->euser1 = pev->euser1;

				pev2->takedamage = DAMAGE_NO;
				pev2->health = 20;

				pev2->velocity = pev->velocity;
				pev2->speed = pev->speed;

				pev2->renderfx = 19;
				pev2->renderamt = 50;
				pev2->rendercolor.y = 255;

				pev2->movetype = MOVETYPE_STEP;
				pev2->solid = SOLID_BBOX;
						
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_BEAMFOLLOW );
					WRITE_SHORT( ENTINDEX(pent2) );	// entity
					WRITE_SHORT( m_beamSprite );	// model
					WRITE_BYTE( 20 ); // life
					WRITE_BYTE( 3 );  // width
					WRITE_BYTE( 0 );   // r
					WRITE_BYTE( 255 );   // g
					WRITE_BYTE( 0 );   // b
					WRITE_BYTE( 100 );	// brightness
				MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

				SET_MODEL(pent2, "avatar-x/avadd42.avfil"); // so i can see it use a big model

				UTIL_SetOrigin( pev2, pev2->origin );
				UTIL_SetSize(pev2, Vector(0, 0, 0), Vector(0, 0, 0));

				pev2->fuser1 = gpGlobals->time + 0.1;
				pev2->nextthink = gpGlobals->time + 0.1;
				pev2->angles = pev->angles;
			}

			pev->velocity = pev->velocity * 0.2 + vecTarget * (flSpeed * 0.8 + 400);
			if (pev->waterlevel == 3)
			{
				// go slow underwater
				if (pev->velocity.Length() > 300)
				{
					pev->velocity = pev->velocity.Normalize() * 300;
				}
				//UTIL_BubbleTrail( pev->origin - pev->velocity * 0.1, pev->origin, 4 );
			} 
			else 
			{
				if (pev->velocity.Length() > spdadd)
				{
					pev->velocity = pev->velocity.Normalize() * spdadd;
				}
			}
		}
		else
		{
			if (pev->effects & EF_LIGHT)
			{
				pev->effects = 0;
				STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );
			}
			pev->velocity = pev->velocity * 0.2 + vecTarget * flSpeed * 0.798;
			if (pev->waterlevel == 0 && pev->velocity.Length() < 1500)
			{
				//Detonate( );
				BFGTouch(pent, NULL);
			}
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;

}

void BFGTouch( edict_t *pent, edict_t *pentTouch )
{

	entvars_t *pev = VARS( pent );
	TraceResult	tr;	

	// make a splat
	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine ( pev->origin - gpGlobals->v_forward * 4, pev->origin - gpGlobals->v_forward * 2 , ignore_monsters, pent, &tr);
		
	int ind = (int)DECAL_INDEX( "{scorch1" );

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
			//EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd36.avfil", 1.0, ATTN_NORM , 0, 100);

		}
		else
		{
			// play hit sound
			//EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd35.avfil", 1.0, ATTN_NORM , 0, 100);
		}
	}
	else 
	{
		// play HIT sound
		
		entityIndex = 0;
	}
	

	edict_t *theowner = pent->v.euser4;
	AvRadiusDamage( pev->origin, VARS(pent), VARS(theowner), 300, 500, DMG_BLAST);
	
	EMIT_SOUND_DYN2( pent, CHAN_VOICE, "ambience/biggun3.wav", 1.0, ATTN_NORM , 0, 100);

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

	/*
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
			WRITE_SHORT( m_beamSprite );
			WRITE_BYTE ( TE_BOUNCE_NULL );
			WRITE_BYTE ( 20 );// 2.5 seconds
		MESSAGE_END();
	}
	*/

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY , pev->origin );
		WRITE_BYTE( TE_LARGEFUNNEL );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		
		WRITE_SHORT( m_funnelSprite );

		WRITE_SHORT( 1 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( g_sModelIndexFireball3 );
		WRITE_BYTE( 40  ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( 0 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( g_sModelIndexSmoke );
		WRITE_BYTE( RANDOM_LONG(0,49) + 50 ); // scale * 10
		WRITE_BYTE( RANDOM_LONG(0, 3) + 8  ); // framerate
	MESSAGE_END();

	//SnowBlowUp( pent );

}