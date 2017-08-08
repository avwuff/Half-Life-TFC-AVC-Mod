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
#include "avflare.h"
#include "avdamage.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

extern int g_sModelIndexFireball2;
extern bool AdminLoggedIn[33];
int	g_sModelIndexBloodSpray;
int	g_sModelIndexBloodDrop;
static FILE *fp;

// keep an array of our snowball charging time.
float flaretimes[33];
int flaremode[33];
int flareammo[33];
enum w_squeak_e {
	WSQUEAK_IDLE1 = 0,
	WSQUEAK_FIDGET,
	WSQUEAK_JUMP,
	WSQUEAK_RUN,
};

float soundTime;

float snarkTimes[33];


enum squeak_e {
	SQUEAK_IDLE1 = 0,
	SQUEAK_FIDGETFIT,
	SQUEAK_FIDGETNIP,
	SQUEAK_DOWN,
	SQUEAK_UP,
	SQUEAK_THROW
};
void FlarePrecache()
{
	PRECACHE_MODEL("models/w_squeak.mdl");
	PRECACHE_MODEL("models/v_squeak.mdl");
	PRECACHE_MODEL("models/p_squeak.mdl");

	PRECACHE_SOUND("squeek/sqk_blast1.wav");
	PRECACHE_SOUND("common/bodysplat.wav");
	PRECACHE_SOUND("squeek/sqk_die1.wav");
	PRECACHE_SOUND("squeek/sqk_hunt1.wav");
	PRECACHE_SOUND("squeek/sqk_hunt2.wav");
	PRECACHE_SOUND("squeek/sqk_hunt3.wav");
	PRECACHE_SOUND("squeek/sqk_deploy1.wav");

	g_sModelIndexBloodSpray = PRECACHE_MODEL ("sprites/bloodspray.spr"); // initial blood
	g_sModelIndexBloodDrop = PRECACHE_MODEL ("sprites/blood.spr"); // splattered blood 

	// flare sounds here later
}

void FlareShowWep( edict_t *pEntity )
{	
	
	// Show the chub in the guys hand

	entvars_t *pev = VARS( pEntity );
	
	pev->viewmodel = ALLOC_STRING("models/v_squeak.mdl");
	pev->weaponmodel = ALLOC_STRING("models/p_squeak.mdl");
	pev->weaponanim = SQUEAK_UP;
		
	if (flareammo[ENTINDEX(pEntity)] > 0)
	{
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( SQUEAK_UP );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
	}

	
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pEntity );
		WRITE_BYTE( 1 );		// state
		WRITE_BYTE( 29 );	// id
		WRITE_BYTE( -1 );	// clip
	MESSAGE_END();

}
void FlareAttackStart( edict_t *pEntity )
{
	// Started holding down the button.
//	if (pEntity->v.iuser1 > 0) return;


	
	float timexp = gpGlobals->time - snarkTimes[ENTINDEX(pEntity)];
	if (timexp < 0) timexp = 10;
	if (timexp < 2.5 && !AdminLoggedIn[ENTINDEX(pEntity)]) return;
	
	snarkTimes[ENTINDEX(pEntity)] = gpGlobals->time;

	if (flaretimes[ENTINDEX(pEntity)] < gpGlobals->time && flareammo[ENTINDEX(pEntity)] > 0)
	{

//		entvars_t *pPev = VARS( pEntity );
		//MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			//WRITE_BYTE( SQUEAK_THROW );						// sequence number
			//WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		//MESSAGE_END();
		
		// track when we started holding down button
		flaretimes[ENTINDEX(pEntity)] = gpGlobals->time;
		flaremode[ENTINDEX(pEntity)] = 1;
	}
}


void FlareReload( edict_t *pEntity )
{
	// Started holding down the button.
	if (pEntity->v.iuser1 > 0) return;
	if (flaretimes[ENTINDEX(pEntity)] < gpGlobals->time && flareammo[ENTINDEX(pEntity)] < 5)
	{
		// set reload state.
		flaremode[ENTINDEX(pEntity)] = 4;
		flaretimes[ENTINDEX(pEntity)] = gpGlobals->time + 2;
		
		// remove the weapon
		if (flareammo[ENTINDEX(pEntity)] > 0)
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( SQUEAK_DOWN );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( SQUEAK_DOWN );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
	}
}

void FlareSpawn( edict_t *pEntity )
{
	// called when someone spawns

	flareammo[ENTINDEX(pEntity)] = 1;
	flaremode[ENTINDEX(pEntity)] = 3;
	flaretimes[ENTINDEX(pEntity)] = gpGlobals->time + 0.5;

}

#define WEAPON_ALLWEAPONS		(~(1<<WEAPON_SUIT))

void FlareTick( edict_t *pEntity )
{
	int mynum = ENTINDEX(pEntity);

	if (flaremode[mynum] == 3) 
	{
		if (flaretimes[mynum] < gpGlobals->time)
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 18 );
				WRITE_BYTE( flareammo[ENTINDEX(pEntity)] );  // say how many snowballs we have
			MESSAGE_END();
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 19 );
				WRITE_BYTE( 1 );  // say how many snowballs we have
			MESSAGE_END();
		
			flaremode[mynum] = 0;
		}
	}
	if (flaremode[mynum] == 4) 
	{

		if (flaretimes[mynum] < gpGlobals->time)
		{
			flareammo[ENTINDEX(pEntity)]++;
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 18 );
				WRITE_BYTE( flareammo[ENTINDEX(pEntity)] );  // say how many snowballs we have
			MESSAGE_END();
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
				WRITE_BYTE( 19 );
				WRITE_BYTE( 1 );  // say how many snowballs we have
			MESSAGE_END();

			// show it again
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( SQUEAK_UP );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
			flaremode[mynum] = 0;
		}
	}	
	else if (flaretimes[mynum] < gpGlobals->time && flaretimes[mynum] > 0 && flaremode[mynum] == 0)
	{
		flaretimes[mynum] = 0;
		if (flareammo[mynum] > 0)
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( SQUEAK_UP );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( SQUEAK_UP );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
	}
}

void FlareAttackFly( edict_t *pEntity )
{
	// let go of button


	if (pEntity->v.iuser1 > 0) return;

	// make sure we pressed it at the valid time
	if (flaretimes[ENTINDEX(pEntity)] > gpGlobals->time) return;
	if (flaremode[ENTINDEX(pEntity)] == 0) return;

		
	if (!AdminLoggedIn[ENTINDEX(pEntity)] && flareammo[ENTINDEX(pEntity)] > 0) flareammo[ENTINDEX(pEntity)]--;
	

	MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pEntity );
		WRITE_BYTE( 18 );
		WRITE_BYTE( flareammo[ENTINDEX(pEntity)] );  // clamp the value to one byte
	MESSAGE_END();


	//ok, set when we can shoot again.
	flaretimes[ENTINDEX(pEntity)] = gpGlobals->time + 0.5;
	flaremode[ENTINDEX(pEntity)] = 0;
	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
		WRITE_BYTE( SQUEAK_THROW );						// sequence number
		WRITE_BYTE( 0 );					// weaponmodel bodygroup.
	MESSAGE_END();	
  

	entvars_t *pPev = VARS( pEntity );



	// Make and throw the flareball
	
	//edict_t *pent = CREATE_ENTITY();

	edict_t *pent = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
	entvars_t *pev = VARS( pent );
	
	KeyValueData	kvd;
	
	char buf[80];
	sprintf( buf, "%s", "monster_snark");
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = buf;

	DispatchKeyValue( pent, &kvd );

	Vector anglesAim = pPev->v_angle + pPev->punchangle;
	UTIL_MakeVectors( anglesAim );

	anglesAim.x		 = -anglesAim.x;
	Vector vecSrc	 = GetGunPosition( pEntity ) + gpGlobals->v_up * 1 + gpGlobals->v_right * 3;
	Vector vecDir	 = gpGlobals->v_forward;
	vecSrc = vecSrc + gpGlobals->v_forward * 30;

	pev->origin = vecSrc;
	pev->angles = anglesAim;
	pev->owner = pEntity;
	pev->velocity = gpGlobals->v_forward * 200 + pEntity->v.velocity;
	pev->takedamage = DAMAGE_AIM;
	
	// The bolt doesn't move as fast under water...
	
	//float timedown = gpGlobals->time - flaretimes[ENTINDEX(pEntity)];
	//timedown = timedown / (float)10;
	//timedown = timedown + 1;
	//if (timedown > 2) timedown = 2;

	pev->euser1 = pEntity;

	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(pent, "models/w_squeak.mdl"); // so i can see it use a big model

	UTIL_SetSize(pev, Vector( -4, -4, 0), Vector(4, 4, 8));
	UTIL_SetOrigin( pev, pev->origin );

	pev->nextthink = gpGlobals->time + 0.1;
	pev->fuser1 = gpGlobals->time;

	pev->health			= 10010;
	pev->gravity		= 0.5;
	pev->friction		= 0.5;
	pev->fuser2 = gpGlobals->time;
	pev->fuser3 = gpGlobals->time + 15;
	//m_flNextBounceSoundTime = gpGlobals->time;// reset each time a snark is spawned.

	pev->sequence = WSQUEAK_RUN;
	
	// play THROW sound

	//EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd37.avfil", 1.0, ATTN_NORM , 0, 100);
}


edict_t *FlareFindTarget( edict_t *pent )
{

	bool found1= 0;
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
		
		
		
		UTIL_TraceLine ( pev->origin, pOther->pev->origin, dont_ignore_monsters, pent, &tr );
	

		// ALERT( at_console, "%f\n", tr.flFraction );
		if (tr.flFraction >= 0.90)
		{
	
			vecDir = pOther->pev->origin - pev->origin;
			flDist = vecDir.Length( );
			vecDir = vecDir.Normalize( );
			//flDot = DotProduct( gpGlobals->v_forward, vecDir );

	
			if (flDist < flMax && pOther->pev->deadflag == DEAD_NO && pOther->pev->iuser1 == 0 && pOther->pev->team > 0)
			{

				
	
				if (pOther->edict() == pev->euser1)
				{
					// We seem to have found me!

					foundme = pOther->edict();
					found2 = 1;
				}
				else
				{
					if (pOther->pev->team != pent->v.euser1->v.team)
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


void FlareThink( edict_t *pent )
{
	// Snark think

	// ALERT( at_console, "think\n" );
	entvars_t *pev = VARS(pent);

	pev->framerate = 1;
	pev->frame++;

	
	
	pev->nextthink = gpGlobals->time + 0.1;

	// explode when ready
	if (gpGlobals->time - pev->fuser1 > 15 || pev->health < 10000)
	{
		
		pev->health = 0;
		FlareKill(pent);
		return;
	}

	
	// float
	if (pev->waterlevel != 0)
	{
		if (pev->movetype == MOVETYPE_BOUNCE)
		{
			pev->movetype = MOVETYPE_FLY;
		}
		pev->velocity = pev->velocity * 0.9;
		pev->velocity.z += 8.0;
	}
	else if (pev->movetype = MOVETYPE_FLY)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}

	
	ALERT( at_console, "think : before hunt check\n");
	// return if not time to hunt
	if (pev->fuser2 > gpGlobals->time)
		return;

	ALERT( at_console, "think : after hunt check\n");
	pev->fuser2 = gpGlobals->time + 2.0;
	
	Vector vecDir;

	
	
	//TraceResult tr;

	Vector vecFlat = pev->velocity;
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize( );

	UTIL_MakeVectors( pev->angles );
	
	bool findnewtarget = 0;



	if (pev->euser2 == NULL) findnewtarget = 1;
	if (findnewtarget == 0 && pev->euser2->v.deadflag != DEAD_NO) findnewtarget = 1;

	if (findnewtarget)
	{
		// find target, bounce a bit towards it.
		ALERT(at_console, "NEED NEW TARGET\n");
		pev->euser2 = FlareFindTarget( pent );
	}


	// squeek if it's about time blow up
	if ((pev->fuser3 - gpGlobals->time <= 0.5) && (pev->fuser3 - gpGlobals->time >= 0.3))
	{
		//EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd37.avfil", 1.0, ATTN_NORM , 0, 100);
		
		
		EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "squeek/sqk_die1.wav", 1, ATTN_NORM, 0, 100 + RANDOM_LONG(0,0x3F));
		

		//CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 256, 0.25 );
	}

	// higher pitch as squeeker gets closer to detonation time
	float flpitch = 155.0 - 60.0 * ((pev->fuser3 - gpGlobals->time) / 15);
	if (flpitch < 80)
		flpitch = 80;
	
	if (pev->euser2 != NULL)
	{
		vecDir = pev->euser2->v.origin - pev->origin;
		pev->vuser2 = vecDir.Normalize( );

		float flVel = pev->velocity.Length();
		float flAdj = 50.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;
			
		 ALERT( at_console, "think : enemy\n");

		 ALERT( at_console, "%.0f %.2f %.2f %.2f\n", flVel, pev->vuser2.x, pev->vuser2.y, pev->vuser2.z );

		pev->velocity = pev->velocity * flAdj + pev->vuser2 * 300;
	}
	
	if (pev->flags & FL_ONGROUND)
	{
		pev->avelocity = Vector( 0, 0, 0 );
	}
	else
	{
		if (pev->avelocity == Vector( 0, 0, 0))
		{
			pev->avelocity.x = RANDOM_FLOAT( -100, 100 );
			pev->avelocity.z = RANDOM_FLOAT( -100, 100 );
		}
	}
	
	if ((pev->origin - pev->vuser3).Length() < 1.0)
	{
		pev->velocity.x = RANDOM_FLOAT( -100, 100 );
		pev->velocity.y = RANDOM_FLOAT( -100, 100 );
	}
	pev->vuser3 = pev->origin;

	pev->angles = UTIL_VecToAngles( pev->velocity );
	pev->angles.z = 0;
	pev->angles.x = 0;

	
	
}
#define		BLOOD_COLOR_YELLOW	(BYTE)195

void FlareKill( edict_t *pent)
{
	
	entvars_t *pev = VARS(pent);

	pev->model = iStringNull;// make invisible
	pev->nextthink = gpGlobals->time + 0.1;

	// since squeak grenades never leave a body behind, clear out their takedamage now.
	// Squeaks do a bit of radius damage when they pop, and that radius damage will
	// continue to call this function unless we acknowledge the Squeak's death now. (sjb)
	pev->takedamage = DAMAGE_NO;

	// play squeek blast
	EMIT_SOUND_DYN2(ENT(pev), CHAN_ITEM, "squeek/sqk_blast1.wav", 1, 0.5, 0, PITCH_NORM);	

	UTIL_BloodDrips( pev->origin, g_vecZero, BLOOD_COLOR_YELLOW, 80 );

	AvRadiusDamage( pev->origin, pev, VARS(pev->euser1), 20, 100, DMG_BLAST);

	//if (m_hOwner != NULL)
		//RadiusDamage ( pev, m_hOwner->pev, pev->dmg, CLASS_NONE, DMG_BLAST );
	//else
	//	RadiusDamage ( pev, pev, pev->dmg, CLASS_NONE, DMG_BLAST );

	pent->v.flags |= FL_KILLME;
}

void FlareTouch( edict_t *pent, edict_t *pentTouch )
{
	
	entvars_t *pev = VARS(pent);
	entvars_t *pTouch = VARS(pentTouch);

	if (gpGlobals->time - pev->fuser1 < 1) return;


	float	flpitch;
	//UTIL_MakeVectors ( pev->angles );

	TraceResult tr;
	UTIL_TraceLine( pev->origin, pentTouch->v.origin, dont_ignore_monsters, pent , &tr );

	// don't hit the guy that launched this grenade
	if ( pev->owner && pentTouch == pev->owner )
		return;

	
	// at least until we've bounced once
	pev->owner = NULL;

	pev->angles.x = 0;
	pev->angles.z = 0;

	// avoid bouncing too much
	if (pev->vuser4.y > gpGlobals->time)
		return;

	
	// higher pitch as squeeker gets closer to detonation time
	flpitch = 155.0 - 60.0 * ((pev->fuser3 - gpGlobals->time) / 15);
	

	// deffs:
	// pev->vuser4.x = m_flNextAttack
	// pev->vuser4.y = m_flNextHit
	// pev->fuser2 = m_flNextHunt
	// pev->fuser3 = m_flDie
	// pev->fuser4 = m_flBounceSound

	if ( pTouch->takedamage && pev->vuser4.x < gpGlobals->time )
	{
		// attack!

		// make sure it's me who has touched them
		if (tr.pHit == pentTouch)
		{
			// and it's not another squeakgrenade
			if (tr.pHit->v.modelindex != pev->modelindex)
			{
				ALERT( at_console, "hit enemy\n");
				
				//ClearMultiDamage( );
				//pOther->TraceAttack(pev, gSkillData.snarkDmgBite, gpGlobals->v_forward, &tr, DMG_SLASH ); 
				//if (m_hOwner != NULL)
					//ApplyMultiDamage( pev, m_hOwner->pev );
				//else
					//ApplyMultiDamage( pev, pev );

				//pev->dmg += gSkillData.snarkDmgPop; // add more explosion damage
				// m_flDie += 2.0; // add more life
				
				AvTakeDamage( VARS(pentTouch), VARS(pev->euser1), pev, 5, DMG_CLUB);

				// make bite sound
				EMIT_SOUND_DYN2(ENT(pev), CHAN_WEAPON, "squeek/sqk_deploy1.wav", 1.0, ATTN_NORM, 0, (int)flpitch);
	
				pev->vuser4.x = gpGlobals->time + 0.5;
			}
		}
		else
		{
			ALERT( at_console, "been hit\n");
		}
	}
	
	pev->vuser4.y = gpGlobals->time + 0.1;
	pev->fuser2 = gpGlobals->time;

	if ( gpGlobals->time < soundTime )
	{
		// too soon!
		return;
	}
	
	if (!(pev->flags & FL_ONGROUND))
	{
		// play bounce sound
		float flRndSound = RANDOM_FLOAT ( 0 , 1 );

		if ( flRndSound <= 0.33 )
			EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt1.wav", 1, ATTN_NORM, 0, (int)flpitch);		
		else if (flRndSound <= 0.66)
			EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, (int)flpitch);
		else 
			EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, (int)flpitch);
		//CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 256, 0.25 );
	}
	else
	{
		// skittering sound
		//CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 100, 0.1 );
	}
	
	soundTime = gpGlobals->time + 0.5;// half second.


}
