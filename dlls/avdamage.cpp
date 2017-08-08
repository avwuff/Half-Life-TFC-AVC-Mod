// replacement stuff so i can give the player damage


#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "soundent.h"
#include "engine.h"

#include "studio.h"
#include "bot.h"
#include "avdamage.h"
#include "studio.h"
#include "externmsg.h"
#include "sadll.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

#define ARMOR_RATIO	 0.2	// Armor Takes 80% of the damage
#define ARMOR_BONUS  0.5	// Each Point of Armor is work 1/x points of health

extern int m_gibModel;

void AvTakeDamage( entvars_t *pev, entvars_t *pevAttacker, entvars_t *pevInflictor, float flDamage, int bitsDamageType )
{
	// have suit diagnose the problem - ie: report damage type
	int bitsDamage = bitsDamageType;
	int ffound = TRUE;
	float flRatio;
	float flBonus;
	float flHealthPrev = pev->health;
	float	flTake;
	Vector	vecDir;

	int isplayer = (FStrEq("player", (char*)STRING(pev->classname)));


	flBonus = ARMOR_BONUS;
	flRatio = ARMOR_RATIO;

	if ( bitsDamageType & DMG_BLAST )
	{
		// blasts damage armor more.
		flBonus *= 2;
	}

	// Already dead
	if (pev->deadflag != DEAD_NO) return;
	
	// go take the damage first

	
	CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);
	CBaseEntity *pInflictor = CBaseEntity::Instance(pevInflictor);
	CBaseEntity *pMe = CBaseEntity::Instance(pev);


	if ( pev->takedamage == DAMAGE_NO )
	{
		// Refuse the damage
		return;
	}

	
	// Armor. 
	if (pev->armorvalue && !(bitsDamageType & (DMG_FALL | DMG_DROWN)) && isplayer )// armor doesn't protect against fall or drown damage!
	{
		float flNew = flDamage * flRatio;
		float flArmor;

		flArmor = (flDamage - flNew) * flBonus;

		// Does this use more armor than we have?
		if (flArmor > pev->armorvalue)
		{
			flArmor = pev->armorvalue;
			flArmor *= (1/flBonus);
			flNew = flDamage - flArmor;
			pev->armorvalue = 0;
		}
		else
			pev->armorvalue -= flArmor;
		flDamage = flNew;
	}


	// this cast to INT is critical!!! If a player ends up with 0.5 health, the engine will get that
	// as an int (zero) and think the player is dead! (this will incite a clientside screentilt, etc)
	//fTookDamage = CBaseMonster::TakeDamage(pevInflictor, pevAttacker, (int)flDamage, bitsDamageType);

	if ( pev->deadflag == DEAD_NO )
	{
		// no pain sound during death animation.

		// make sure a PLAYER is making these sounds

		if ( isplayer )
		{
			int iSound;
			iSound = RANDOM_LONG ( 1 , 6 ); 
			
			if ( iSound == 1 )		EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/pain1.wav", 1, ATTN_NORM, 0, 100);
			else if ( iSound == 2 )	EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/pain2.wav", 1, ATTN_NORM, 0, 100);
			else if ( iSound == 3 )	EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/pain3.wav", 1, ATTN_NORM, 0, 100);
			else if ( iSound == 4 )	EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/pain4.wav", 1, ATTN_NORM, 0, 100);
			else if ( iSound == 5 )	EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/pain5.wav", 1, ATTN_NORM, 0, 100);
			else if ( iSound == 6 )	EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/pain6.wav", 1, ATTN_NORM, 0, 100);
		}
	}

	//!!!LATER - make armor consideration here!
	flTake = flDamage;

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = Vector( 0, 0, 0 );
	if (!FNullEnt( pevInflictor ) && isplayer )
	{
		if (pAttacker)
		{
			vecDir = ( pevInflictor->origin - Vector ( 0, 0, 10 ) - pev->origin ).Normalize();
			vecDir = vecDir.Normalize();
		}
	}

	pev->dmg_inflictor = ENT(pevInflictor);
	pev->dmg_take += flTake;

	// if this is a player, move him around!
	if (  (pev->movetype == MOVETYPE_WALK) && (!pevAttacker || pevAttacker->solid != SOLID_TRIGGER) && isplayer)
	{
		pev->velocity = pev->velocity + vecDir * -AvDamageForce( pev, flDamage );
	}

	/*
	char msg[80];
	sprintf(msg, "Damaging a %s!\n", (char*)STRING(pev->classname));
	UTIL_ClientPrintAll( HUD_PRINTTALK, msg);
	*/

	// do the damage
	pev->health -= flTake;

	// HACKHACK Don't kill monsters in a script.  Let them break their scripts first

	if ( pev->health <= 0 && isplayer )
	{

		if ( bitsDamageType & DMG_ALWAYSGIB )
		{
			AvKilled( pev, pevAttacker, pevInflictor, GIB_ALWAYS );
		}
		else if ( bitsDamageType & DMG_NEVERGIB )
		{
			AvKilled( pev, pevAttacker, pevInflictor, GIB_NEVER );
		}
		else
		{
			AvKilled( pev, pevAttacker, pevInflictor, GIB_NORMAL );
		}
		return;
	}

	if (isplayer) pev->punchangle.x = -2;
}

float AvDamageForce( entvars_t *pev, float damage )
{ 
	float force = damage * ((32 * 32 * 72.0) / (pev->size.x * pev->size.y * pev->size.z)) * 5;
	
	if ( force > 1000.0) 
	{
		force = 1000.0;
	}

	return force;
}

void AvKilled ( entvars_t *pev, entvars_t *pevAttacker, entvars_t *pevInflictor, int iGib )
{
	if ( FStrEq("player", (char*)STRING(pev->classname)) )
	{

		Vector mepos = pev->origin;
		mepos.z += 20;
		ClearBits(pev->flags, FL_ONGROUND);
		
		SET_ORIGIN(ENT(pev), mepos);

		AvPlayerKilled( pev, pevAttacker, pevInflictor );
		AvSetActivity( pev, "die_simple");

		pev->frame = 14;

		
		//pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes

		pev->deadflag		= DEAD_DYING;
		pev->movetype		= MOVETYPE_TOSS;
		
		if (pev->velocity.z < 10)
			pev->velocity.z += RANDOM_FLOAT(0,300);


		// IMPORTANT: If this was a medic, delete their timer

		if(pev->playerclass == 5)
		{
			CBaseEntity *pCTimer = NULL;
			while(pCTimer = UTIL_FindEntityByClassname(pCTimer, "timer"))
			{
				if(pCTimer->pev->owner == ENT(pev))
				{
					pCTimer->pev->flags |= FL_KILLME;
				}
			}
		}

	
	// send "health" update message to zero



		MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, pev );
			WRITE_BYTE( 0 );
		MESSAGE_END();

		// Tell Ammo Hud that the player is dead
		MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
			WRITE_BYTE(0);
			WRITE_BYTE(0XFF);
			WRITE_BYTE(0xFF);
		MESSAGE_END();

		// reset FOV
		//m_iFOV = m_iClientFOV = 0;

		MESSAGE_BEGIN( MSG_ONE, gmsgSetFOV, NULL, pev );
			WRITE_BYTE(0);
		MESSAGE_END();
	}

	


	// dont allow gibbing until i find a better way of spawning gibs
	/*
	if ( ( pev->health < -40 && iGib != GIB_NEVER ) || iGib == GIB_ALWAYS )
	{
		pev->solid			= SOLID_NOT;

		//GibMonster();	// This clears pev->model
		pev->effects |= EF_NODRAW;
		return;
	}

	*/


	pev->effects |= EF_NODRAW;
	pev->angles.x = 0;
	pev->angles.z = 0;
	
	//throw some gibs

	

	
	for (int i = 0; i < 4; i++)
	{
		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40,90) + gpGlobals->v_up * RANDOM_FLOAT(75,200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_MODEL);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( vecShellVelocity.x);
			WRITE_COORD( vecShellVelocity.y);
			WRITE_COORD( vecShellVelocity.z);
			WRITE_ANGLE( pev->angles.y );
			
			WRITE_SHORT( m_gibModel );
			
			WRITE_BYTE ( TE_BOUNCE_NULL );
			WRITE_BYTE ( 100 );// 2.5 seconds
		MESSAGE_END();
	}

	//CGib::SpawnHeadGib( pev );
	//CGib::SpawnRandomGibs( pev, 4, 1 );	// throw some human gibs.



	pev->nextthink = gpGlobals->time + 0.1;

	// water death sounds
	if (pev->waterlevel == 3)
	{
		EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/h20death.wav", 1, ATTN_NORM, 0, 100);
		return;
	}

	if ( FStrEq("player", (char*)STRING(pev->classname)) )
	{

		// death sound
		int iSound;
		iSound = RANDOM_LONG ( 1 , 4 ); 
		if ( iSound == 1 )		EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/death1.wav", 1, ATTN_NORM, 0, 100);
		else if ( iSound == 2 )		EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/death3.wav", 1, ATTN_NORM, 0, 100);
		else if ( iSound == 3 )		EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/death4.wav", 1, ATTN_NORM, 0, 100);
		else if ( iSound == 4 )		EMIT_SOUND_DYN2(ENT(pev), CHAN_VOICE, "player/death5.wav", 1, ATTN_NORM, 0, 100);
	}
}

void AvPlayerKilled( entvars_t *pev, entvars_t *pKiller, entvars_t *pInflictor )
{
	AvDeathNotice( pev, pKiller, pInflictor );

	//	pVictim->m_iDeaths += 1;
	
	
	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );

	if ( pev == pKiller )  
	{  // killed self
		pev->frags -= 1;
	}
	else if ( FStrEq("player", (char*)STRING(pKiller->classname)) )
	{
		// if a player dies in a deathmatch game and the killer is a client, award the killer some points
		pKiller->frags += 1;
	}
	else
	{  // killed by the world
		pev->frags -= 1;
	}





	/*
	int deaths = 40;

	// update the scores
	// killed scores

	MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX( ENT(pev) ) );
		WRITE_SHORT( pev->frags );
		WRITE_SHORT( deaths );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
	MESSAGE_END();

	// killers score, if it's a player
	
	if ( FStrEq("player", (char*)STRING(pKiller->classname)) )
	{
	
		deaths = 50;
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX( ENT(pKiller) ) );
			WRITE_SHORT( pKiller->frags );
			WRITE_SHORT( deaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( 0 );
		MESSAGE_END();

	}
	*/
}

void AvDeathNotice( entvars_t *pev, entvars_t *pKiller, entvars_t *pInflictor)
{

	// Work out what killed the player, and send a message to all clients about it
	CBaseEntity *Killer = CBaseEntity::Instance( pKiller );

	const char *killer_weapon_name = "world";		// by default, the player is killed by the world
	int killer_index = 0;
	
	if ( pKiller->flags & FL_CLIENT )
	{
		killer_index = ENTINDEX(ENT(pKiller));
		killer_weapon_name = STRING(pInflictor->classname);  // it's just that easy
	}
	else
	{
		killer_weapon_name = STRING(pInflictor->classname);
	}

	// strip the monster_* or weapon_* from the inflictor's classname
	if ( strncmp( killer_weapon_name, "weapon_", 7 ) == 0 )
		killer_weapon_name += 7;
	else if ( strncmp( killer_weapon_name, "monster_", 8 ) == 0 )
		killer_weapon_name += 8;
	else if ( strncmp( killer_weapon_name, "func_", 5 ) == 0 )
		killer_weapon_name += 5;
	else if ( strncmp( killer_weapon_name, "building_", 9 ) == 0 )
		killer_weapon_name += 9;

	MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
		WRITE_BYTE( killer_index );						// the killer
		WRITE_BYTE( ENTINDEX( ENT(pev) ) );		// the victim
		WRITE_STRING( killer_weapon_name );		// what they were killed by (should this be a string?)
	MESSAGE_END();
	char team1[15];
	char team2[15];

	AvGetTeam(ENT(pKiller), team1);
	AvGetTeam(ENT(pev), team1);

	UTIL_LogPrintf( "\"%s<%i><%u><%s>\" killed \"%s<%i><%u><%s>\" with \"%s\"\n",  
		STRING( pKiller->netname ),
		GETPLAYERUSERID( ENT(pKiller) ),
		pfnGetPlayerWONId( ENT(pKiller) ),
		team1 ,
		STRING( pev->netname ),
		GETPLAYERUSERID( ENT(pev) ),
		pfnGetPlayerWONId( ENT(pev) ),
		team2,
		killer_weapon_name );
}


void AvSetActivity ( entvars_t *pev, char *szSequence )
{
	int	iSequence;

	iSequence = AvLookupSequence ( pev, szSequence );

	// Set to the desired anim, or default anim if the desired is not present
	if ( iSequence > 0 )
	{
		pev->frame = 0;
		pev->sequence		= iSequence;	// Set to the reset anim (if it's there)
		pev->animtime = gpGlobals->time;
		pev->framerate = 1.0;
	}
	else
	{
		pev->sequence		= 0;	// Set to the reset anim (if it's there)
	}
}

int AvLookupSequence( entvars_t *pev, const char *label )
{

	void *pmodel = GET_MODEL_PTR( ENT(pev) );
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return 0;

	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);

	for (int i = 0; i < pstudiohdr->numseq; i++)
	{
		if (stricmp( pseqdesc[i].label, label ) == 0)
			return i;
	}
	return -1;
}


	
void AvRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int bitsDamageType )
{
	CBaseEntity *pEntity = NULL;
	TraceResult	tr;
	float		flAdjustedDamage, falloff;
	Vector		vecSpot;

	if ( flRadius )
		falloff = flDamage / flRadius;
	else
		falloff = 1.0;


	int bInWater = (UTIL_PointContents ( vecSrc ) == CONTENTS_WATER);

	vecSrc.z += 1;// in case grenade is lying on the ground


	if ( !pevAttacker )
		pevAttacker = pevInflictor;

	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, vecSrc, flRadius )) != NULL)
	{


		
		if ( pEntity->pev->takedamage != DAMAGE_NO )
		{


			
			// blast's don't travel into or out of water
			if (bInWater && pEntity->pev->waterlevel == 0)
				continue;
			if (!bInWater && pEntity->pev->waterlevel == 3)
				continue;

			vecSpot = pEntity->pev->origin; //pEntity->BodyTarget( vecSrc );
			
			UTIL_TraceLine ( vecSrc, vecSpot, dont_ignore_monsters, ENT(pevInflictor), &tr );



			if ( tr.flFraction == 1.0 || tr.pHit == pEntity->edict() )
			{// the explosion can 'see' this entity, so hurt them!
				if (tr.fStartSolid)
				{
					// if we're stuck inside them, fixup the position and distance
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0.0;
				}
				
				// decrease damage for an ent that's farther from the bomb.
				flAdjustedDamage = ( vecSrc - tr.vecEndPos ).Length() * falloff;
				flAdjustedDamage = flDamage - flAdjustedDamage;
			
				if ( flAdjustedDamage < 0 )
				{
					flAdjustedDamage = 0;
				}
				
				bool dodamage = 1;
				// dont damage team...
				if (pevAttacker->team == pEntity->pev->team) dodamage = 0;
				//unles we are hurting ourselves
				if (pevAttacker == pEntity->pev) dodamage = 1;


				if (dodamage) AvTakeDamage( pEntity->pev, pevAttacker, pevInflictor, flAdjustedDamage, bitsDamageType);
			}
		}
	}
}