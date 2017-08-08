// Ah, a fresh page of code. What shall we fill in here?

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
#include "avpara.h"
#include "externmsg.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

// globals

// Chute states:
// 0 - no chute
// 1 - has chute, undeployed
// 2 - chute is currently deployed, the player has low grav / speed

int ChuteState[33];


// ok, some base functions

void ParaPrecache()
{

//	PRECACHE_MODEL( "models/w_longjump.mdl" );
	PRECACHE_SOUND( "items/r_item1.wav" );
	

}

edict_t *ParaSpawnItem( edict_t *pent )
{
	// Spawns the parachute on the ground that people can pick up to "get" a parachute
	// item_parachute

	edict_t *pent2 = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
	entvars_t *pev = VARS( pent2 );
	
	if (pent) {

		pev->origin = pent->v.origin;
		pev->angles = pent->v.angles;
	}

	SET_ORIGIN( pent2 , pev->origin );

	// for now don't take damage
	pev->takedamage = DAMAGE_NO;
	pev->max_health = 20;
	pev->health = 20;
	//pev->owner = pent;
	
	// Call the SPAWN routine to set more stuff
	KeyValueData	kvd;

	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "item_avparachute";
	(*other_gFunctionTable.pfnKeyValue)( pent2, &kvd );
	kvd.fHandled = FALSE;
	
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	pev->effects &= ~EF_NODRAW;
	pev->iuser1 = 1;

	DROP_TO_FLOOR( pent2 );

	
	SET_MODEL(pent2, "models/w_longjump.mdl");
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));

	pev->nextthink = gpGlobals->time + 0.1;	

	return pent2;

}

void ParaGiveChute( edict_t *pEntity )
{

	// Gives this person a parachute
	ChuteState[ ENTINDEX( pEntity ) ] = 1;

}

void ParaThink( edict_t *pent )
{

	// Since we are thinking, we must be respawning the parachute item.
	// also play respawn sound
	
	// it sucks to look at other peoples code and not know what the fuck is going on cause they didnt comment it...
	// this way if i (or someone else) sees this they will be able to figure out whats happening


	#ifdef DEBUGLOG 
	fp=fopen("para.txt","a");
	fprintf(fp, "Parachute thinking!\n");
	fclose(fp);	
	#endif	


	pent->v.nextthink = gpGlobals->time + 5;	// dont think again until someone takes me

	if ( pent->v.effects & EF_NODRAW )
	{
		// changing from invisible state to visible.
		EMIT_SOUND_DYN2( pent, CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150 );
		
		pent->v.effects &= ~EF_NODRAW;
		pent->v.effects |= EF_MUZZLEFLASH;	// when items respawn they make a single frame flash
		
		// enable touching again
		pent->v.solid = SOLID_TRIGGER;

		SET_ORIGIN( pent , pent->v.origin );

	}
}

void ParaTouch( edict_t *pent, edict_t *pEntity )
{
	// Called when someone touches the parachute on the ground
	// Remote it, give them a chute, and make it respawn in 20 seconds.

	if (!FStrEq((char*)STRING(pEntity->v.classname), "player")) return;


	#ifdef DEBUGLOG 
	fp=fopen("para.txt","a");
	fprintf(fp, "Starting Para Touch\n");
	fclose(fp);	
	#endif	

	// check if he already has a chute
	if ( ChuteState[ ENTINDEX( pEntity ) ] != 0 ) return;

	// give the player the chute
	ParaGiveChute( pEntity );


	// make a sound that sounds like you are getting a chute
	// use TFC r_item1 sound... its pretty good

	EMIT_SOUND_DYN2( pent, CHAN_VOICE, "items/r_item1.wav", 1.0, ATTN_NORM, 0, 100 );
	
	// respawn in 20 seconds

	pent->v.nextthink = gpGlobals->time + 20;

	// hide the chute from being seen
	pent->v.solid = SOLID_NOT;
	pent->v.effects |= EF_NODRAW;

	// Put the little long jump icon in the client's HUD

	// register the message that tells clients we picked up an item, so that we can USE this message.
	// if we dont register it, the client doesnt know what we are talking about

	//	if (gmsgItemPickup == 0) gmsgItemPickup = REG_USER_MSG( "ItemPickup", -1 );
	
	#ifdef DEBUGLOG 
	fp=fopen("para.txt","a");
	fprintf(fp, "It appears to be %i\n", gmsgItemPickup);
	fclose(fp);	
	#endif	

	MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pEntity );
		WRITE_STRING( "item_longjump" );
	MESSAGE_END();

	ClientPrint( VARS(pEntity), HUD_PRINTTALK, "* You've picked up a parachute! Bind 'deploypara' to use it!\n");



}

// The code following this line has to do with making the parachute actually WORK for people.


void ParaEnable( edict_t *pEntity )
{
	
	if (ChuteState[ ENTINDEX( pEntity ) ] != 1) {
		ClientPrint( VARS(pEntity), HUD_PRINTTALK, "* You don't have a parachute! Go pick one up!\n");
		return;
	}
	// cant use the parachute when you are on the ground

	if (pEntity->v.flags & FL_ONGROUND) {
		ClientPrint( VARS(pEntity), HUD_PRINTTALK, "* Parachutes only work in the air!\n");
		return;
	}
	
	// turns on the parachute
	ChuteState[ ENTINDEX( pEntity ) ] = 2;
	entvars_t *pev = VARS( pEntity );

	// set initial vars
	pev->gravity = 0.1;
	pev->speed = 0;
	pev->velocity = g_vecZero;

	// give them the parachute model

	int i = 1;
	edict_t *frontEnt;
	entvars_t *pModelPev;

	int mdlfound = 0;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pModelPev =  VARS(frontEnt);
			if (FStrEq((char *)STRING(pModelPev->netname), "para")) {
				// Touch this ent.
				(*other_gFunctionTable.pfnTouch)(frontEnt, pEntity);
			}
		}
	}

	// The parachute has been given to em
	// Now, we just wait for em to hit the ground then we take the chute again


}

void ParaPrePlayerThink( edict_t *pEntity )
{

	if (ChuteState[ ENTINDEX( pEntity ) ] != 2) return;
	
	// we touched the ground, remove the parachute. OR we are going up (parachutes only work when you are falling)

	if ((pEntity->v.flags & FL_ONGROUND) || (pEntity->v.velocity.z > 0)) {

		ChuteState[ ENTINDEX( pEntity ) ] = 0;

		// remove model

		entvars_t *pev = VARS( pEntity );
		// set initial vars
		pev->gravity = 1;

		int i = 1;
		edict_t *frontEnt;
		entvars_t *pModelPev;

		int mdlfound = 0;

		for (i; i < 1025; i++) {

			frontEnt = INDEXENT ( i );
			if (frontEnt) {
				pModelPev =  VARS(frontEnt);
				if (FStrEq((char *)STRING(pModelPev->netname), "takegun")) {
					// Touch this ent.
					(*other_gFunctionTable.pfnTouch)(frontEnt, pEntity);
				}
			}
		}


	}
}

void ParaJoinGame( edict_t *pEntity )
{
	ChuteState[ ENTINDEX( pEntity ) ] = 0;
}