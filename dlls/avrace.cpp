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
#include "avrace.h"
#include "avdamage.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;
extern origentdata_t EntData[1025];		// The original entity data, before we showed it.

static FILE *fp;
extern bool AdminLoggedIn[33];
extern playerinfo_t PlayerInfo[33];

edict_t *activeCam = NULL;
edict_t *lastCam = NULL;
float lastviewset = 0;

edict_t *thecam = NULL;

edict_t* CamTriggerCreate( edict_t *pEntity )
{
	// Create the camera trigger
	entvars_t *pPev = NULL;

	if (pEntity != NULL)  pPev = VARS( pEntity );

	if (pEntity != NULL) 
	{
		if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
		{
			ClientPrint( pPev, HUD_PRINTTALK, "* Admin only feature\n");
			return 0;
		}
	}
	// Create the paper here!
	KeyValueData	kvd;
			
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	entvars_t *pev;
	pev =  VARS(tEntity);
	
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "race_camtrigger";

	DispatchKeyValue( tEntity, &kvd );

	if (pEntity != NULL) 
	{

		char buf[80];
		sprintf(buf, "%s", CMD_ARGV(1));
		
		kvd.fHandled = FALSE;
		kvd.szClassName = NULL;
		kvd.szKeyName = "target";
		kvd.szValue = buf;

		DispatchKeyValue( tEntity, &kvd );
	}

	// place this in front
	if (pEntity != NULL) pev->origin = pEntity->v.origin;

	pev->solid = SOLID_TRIGGER;
	pev->rendermode = 2;
	pev->renderamt = 0;

	pev->takedamage = DAMAGE_NO;
	pev->health = 40;
	pev->movetype = MOVETYPE_NONE;

	pev->effects |= EF_NODRAW;
	
	SET_ORIGIN( tEntity , pev->origin );
	SET_MODEL( ENT( pev ) , "models/shell.mdl");
	UTIL_SetSize( pev, Vector( -64, -64 , -32), Vector(64, 64, 32) );

	// play deploy sound
	if (pEntity != NULL) EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);

	// no thinking
	pev->nextthink = gpGlobals->time;
		
	return tEntity;
}

void CamTriggerTouch( edict_t *pEntity, edict_t *pTouch )
{
	if (FStrEq(STRING(pTouch->v.classname), "player"))
	{

		if (!PlayerInfo[ENTINDEX(pTouch)].racer) return;

		// try to find a camera that matches

		bool findagain = 1;


		if (pEntity->v.euser3 != NULL && !FNullEnt(pEntity->v.euser3))
		{

			if (FStrEq(STRING(pEntity->v.euser3->v.targetname), STRING(pEntity->v.target)))
			{
				// got it
				findagain = 0;
			}
		}
		
		if (findagain)
		{
			pEntity->v.euser3 = FIND_ENTITY_BY_STRING( NULL, "targetname", STRING(pEntity->v.target));
		}
		

		if (pEntity->v.euser3 != NULL && !FNullEnt(pEntity->v.euser3))
		{
			// finally got the cam
			activeCam = pEntity->v.euser3;
			
			if (activeCam != lastCam && gpGlobals->time - lastviewset > 0.5)
			{
				// set view of everyone here
				
				lastCam = activeCam;


				lastviewset = gpGlobals->time;
				
				if (thecam == NULL)
				{
					ActualCamCreate();
				}

				thecam->v.origin = activeCam->v.origin;
				SET_ORIGIN(thecam, thecam->v.origin);
				thecam->v.euser1 = pTouch;

				thecam->v.iuser1 = 1;
				RaceCamThink(thecam);



				int i;
				edict_t *pPlayerEdict;
				
				for ( i = 1; i <= gpGlobals->maxClients; i++ )
				{
					if ( PlayerInfo[i].racecam && !PlayerInfo[i].racer  )
					{
						pPlayerEdict = INDEXENT( i );
						if ( pPlayerEdict && !pPlayerEdict->free )
						{

							if (FStrEq( STRING(pPlayerEdict->v.classname), "player")) // make sure its actually a player.
							{
							

								SET_VIEW(pPlayerEdict, thecam);

							}
						}
					}
				}	

			}		
		}
	}
}


edict_t* RaceCamCreate( edict_t *pEntity )
{
	// Create the camera trigger

	entvars_t *pPev = NULL;

	if (pEntity != NULL)  pPev = VARS( pEntity );
	if (pEntity != NULL) 
	{

		if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
		{
			ClientPrint( pPev, HUD_PRINTTALK, "* Admin only feature\n");
			return 0;
		}
	}

	// Create the paper here!
	KeyValueData	kvd;
			
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	entvars_t *pev;
	pev =  VARS(tEntity);
	
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "race_campos";

	DispatchKeyValue( tEntity, &kvd );
	if (pEntity != NULL) 
	{

		char buf[80];
		sprintf(buf, "%s", CMD_ARGV(1));
		
		kvd.fHandled = FALSE;
		kvd.szClassName = NULL;
		kvd.szKeyName = "targetname";
		kvd.szValue = buf;

		DispatchKeyValue( tEntity, &kvd );
	}

	// place this in front
	if (pEntity != NULL) pev->origin = pEntity->v.origin;
	pev->solid = SOLID_NOT;

	pev->rendermode = 5;
	pev->renderamt = 255;

	pev->takedamage = DAMAGE_NO;
	pev->health = 40;
	pev->movetype = MOVETYPE_FLY;

	pev->effects |= EF_NODRAW;

	SET_ORIGIN( tEntity , pev->origin );
	SET_MODEL( ENT( pev ) , "sprites/flare6.spr");
	//UTIL_SetSize( pev, Vector( -64, -64 , -32), Vector(64, 64, 32) );

	// play deploy sound
	if (pEntity != NULL) EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);

	// no thinking
	pev->nextthink = 0;
		
	return tEntity;
}


bool ActualCamCreate( )
{
	KeyValueData	kvd;
			
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	entvars_t *pev;
	pev =  VARS(tEntity);
	
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "race_camera";

	DispatchKeyValue( tEntity, &kvd );

	
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "targetname";
	kvd.szValue = "av_actualcam";

	DispatchKeyValue( tEntity, &kvd );


	// place this in front
	pev->origin = Vector(0,0,0);
	pev->solid = SOLID_NOT;

	pev->rendermode = 5;
	pev->renderamt = 255;

	pev->takedamage = DAMAGE_NO;
	pev->health = 40;
	pev->movetype = MOVETYPE_FLY;

	SET_ORIGIN( tEntity , pev->origin );
	SET_MODEL( ENT( pev ) , "sprites/flare6.spr");
	//UTIL_SetSize( pev, Vector( -64, -64 , -32), Vector(64, 64, 32) );

	// play deploy sound
	EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);

	// no thinking
	pev->nextthink = gpGlobals->time + 0.1;
	
	thecam = tEntity;

	return 1;
}


void RaceCamThink(edict_t *pent)
{

	entvars_t *pev = VARS(pent);


	// see if we are the active cam
	if (activeCam != NULL)
	{
		ALERT(at_console, "I AM THE ACTIVE CAM\n");

		// follow player here
		edict_t *pen = pev->euser1;

		if (pen && !FNullEnt(pen))
		{
			Vector lookVec = pen->v.origin - pev->origin;
			//lookVec.Normalize;
			lookVec = UTIL_VecToAngles(lookVec);
			lookVec.x = -lookVec.x;
			Vector goVec = (lookVec - pev->angles);
			
			while (goVec.x > 360) goVec.x = goVec.x - 360;
			while (goVec.x < 0) goVec.x = goVec.x + 360;

			while (goVec.y > 360) goVec.y = goVec.y - 360;
			while (goVec.y < 0) goVec.y = goVec.y + 360;

			while (goVec.z > 360) goVec.z = goVec.z - 360;
			while (goVec.z < 0) goVec.z = goVec.z + 360;
			
			if (goVec.x > 180) goVec.x = -(360 - goVec.x);
			if (goVec.x < -180) goVec.x = (360 + goVec.x);
		
			if (goVec.y > 180) goVec.y = -(360 - goVec.y);
			if (goVec.y < -180) goVec.y = (360 + goVec.y);
			
			if (goVec.z > 180) goVec.z = -(360 - goVec.z);
			if (goVec.z < -180) goVec.z = (360 + goVec.z);
			
			//look at this
			
			if (pev->iuser1)
			{
				pev->angles = lookVec;
				pev->iuser1 = 0;
			}
			else
			{

				pev->avelocity = goVec * 8;	
			}
			
		}
	}
	
	pev->nextthink = gpGlobals->time + 0.1;
}