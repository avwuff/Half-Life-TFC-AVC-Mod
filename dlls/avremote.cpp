// Remotes.

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
#include "avremote.h"
#include "avdamage.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;
extern int g_sModelIndexFireball2;
extern int m_beamSprite;

float remoteTimes[33];


extern bool AdminLoggedIn[33];

// Remote Types - 
// 1 - Camera Remote
// 2 - Fly remote
// 3 - View Remote

bool RemoteCreate( edict_t *pEntity, int mode )
{


	float timexp = gpGlobals->time - remoteTimes[ENTINDEX(pEntity)];
	if (timexp < 0) timexp = 10;
	if (timexp < 9 && !AdminLoggedIn[ENTINDEX(pEntity)]) return 0;

	remoteTimes[ENTINDEX(pEntity)] = gpGlobals->time;


	// make sure we dont already have a remote
	// Create the remote and let it fly
	entvars_t *pPev = VARS( pEntity );

	int radiocount = 0;
	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("av_remote", pClassname)) {

				if (frontEnt->v.owner == pEntity) 
				{
					RemoteBlowUp(frontEnt);
					//radiocount++;
				}
			}
		}
	}

	
	if (radiocount >= 1) {
		ClientPrint( pPev, HUD_PRINTTALK, "* You already have a remote!\n");
		return 0;
	}

	if (pPev->iuser1 > 0) return 0; // no specs can make remotes
	if (pPev->deadflag != DEAD_NO) return 0; // dead people can't make remotes

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecOri = pPev->origin + vecAiming;
	vecOri.z = vecOri.z + 15;

	KeyValueData	kvd;
	
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	entvars_t *pev;
	pev =  VARS(tEntity);
	
	char buf[80];
	sprintf( buf, "%s", "av_remote");
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = buf;

	DispatchKeyValue( tEntity, &kvd );

	// place this in front

	pev->origin = vecOri;
	pev->angles = pPev->v_angle;
		
	SET_MODEL( ENT( pev ) , "models/shell.mdl");
	UTIL_SetSize( pev, Vector( -4, -4 ,-4), Vector(4, 4, 4));
	
	pev->takedamage = DAMAGE_YES;
	pev->health = 10010;
	pev->owner = pEntity;
	
	SET_ORIGIN( tEntity , vecOri );
		
	pev->iuser1 = mode;
	
	// Spawn routine
	tEntity->v.solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_FLY;

	pev->nextthink = gpGlobals->time + 0.1;
	pev->fuser1 = gpGlobals->time; // time we started
	pev->vuser1 = pPev->v_angle;

	pev->rendermode = 5;
	pev->renderamt = 0;
						
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( ENTINDEX(tEntity) );	// entity
		WRITE_SHORT( m_beamSprite );	// model
		WRITE_BYTE( 50 ); // life
		WRITE_BYTE( 3 );  // width
		WRITE_BYTE( 255 );   // r
		WRITE_BYTE( 255 );   // g
		WRITE_BYTE( 0 );   // b
		WRITE_BYTE( 100 );	// brightness
	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
	
	pev->effects |= EF_LIGHT;

	SET_VIEW(pEntity, tEntity);
	
	return 1;
}

void RemoteThink( edict_t *pEntity ) 
{

	entvars_t *pev = VARS(pEntity);
	entvars_t *owner = VARS(pev->owner);
	// depending on the mode, do stuff
	int mode = pev->iuser1;

	if (owner->deadflag != DEAD_NO) RemoteBlowUp(pEntity);
	if (gpGlobals->time - pev->fuser1 > 45) RemoteBlowUp(pEntity);

	if (mode == 1)
	{
		// fly in the direction they are looking
		pev->angles = owner->v_angle;
		
		Vector aimer = pev->angles;
		aimer.x = -aimer.x;
	
		UTIL_MakeAimVectors( aimer );	
		pev->velocity = gpGlobals->v_forward * 400;
	}
	else if (mode == 2)
	{
		// fly up until we hit something
		pev->angles = owner->v_angle;
		pev->angles.x = -90;
		
		Vector aimer = pev->angles;
		aimer.x = -aimer.x;
	
		UTIL_MakeAimVectors( aimer );	
		pev->velocity = gpGlobals->v_forward * 400;
	}
	else if (mode == 10)
	{
		// part 2 of mode 2, just stay still and look around
		pev->angles = owner->v_angle;
		
		pev->velocity.x = 0;
		pev->velocity.y = 0;
		pev->velocity.z = 0;
	}
	else if (mode == 3)
	{
		// fly in the direction they are looking, and pull them along
		pev->angles = owner->v_angle;
		
		Vector aimer = pev->angles;
		aimer.x = -aimer.x;
		aimer.y = aimer.y + 180;
	
		UTIL_MakeAimVectors( aimer );	
		pev->velocity = gpGlobals->v_forward * 400;

		float speed = (pev->origin - owner->origin).Length() * 0.6;

		owner->velocity = ( pev->origin - owner->origin ).Normalize() * speed;
	}

	pev->nextthink = gpGlobals->time + 0.05;
}



void RemoteTouch( edict_t *pEntity, edict_t *touched ) 
{

	entvars_t *pev = VARS(pEntity);
	entvars_t *owner = VARS(pev->owner);
	// depending on the mode, do stuff
	int mode = pev->iuser1;

	if (owner->deadflag != DEAD_NO) RemoteBlowUp(pEntity);
	
	if (mode == 1 || mode == 3)
	{
		RemoteBlowUp(pEntity);	
	}
	if (mode == 2)
	{
		// change mode to 10;
		pev->iuser1 = 10;
	}


}

void RemoteBlowUp( edict_t *pEntity )
{
	entvars_t *pev = VARS(pEntity);
	entvars_t *owner = VARS(pev->owner);

	SET_VIEW(pev->owner, pev->owner);

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( pev->origin.x );	// Send to PAS because of the sound
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( g_sModelIndexFireball2 );
		WRITE_BYTE( 30  ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( 0 );
	MESSAGE_END();

	pev->flags |= FL_KILLME;

}