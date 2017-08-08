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
#include "avdance.h"
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

bool DanceCreate( edict_t *pEntity )
{
	// Create the dance machine

	entvars_t *pPev = VARS( pEntity );


	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Admin only feature\n");
		return 0;
	}

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, pEntity , &tr );

	//if (tr.flFraction < 1.0)
	//{
	
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			// Create the paper here!
			KeyValueData	kvd;
			
			edict_t *tEntity;
			tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
		
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(tEntity);
			
			// Set the KEYVALUES here!
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "building_dancemachine";

			DispatchKeyValue( tEntity, &kvd );

			// place this in front
			Vector vecOri = tr.vecEndPos;
			pRunOnPev->origin = vecOri;
			pRunOnPev->solid = SOLID_BBOX;

			SET_ORIGIN( tEntity , vecOri );
			pRunOnPev->angles = angles;
			pRunOnPev->solid = SOLID_BBOX;

			SET_MODEL( ENT( pRunOnPev ) , "models/sphere.mdl");

			UTIL_SetSize( pRunOnPev, Vector( -16, -16 , -16), Vector(16, 16, 16) );

			pRunOnPev->takedamage = DAMAGE_NO;
			pRunOnPev->max_health = 40 + 10000;
			pRunOnPev->health = 40 + 10000;
			pRunOnPev->euser4 = pEntity;
			pRunOnPev->movetype = MOVETYPE_FLY;

			//DROP_TO_FLOOR( tEntity );


			// play deploy sound
			EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);
			//unOnPev->iuser2 = pRunOnPev->euser4->v.team; // Set the team this radio belongs to
			pRunOnPev->nextthink = gpGlobals->time + 1;
			
			pRunOnPev->avelocity.y = 50;
	
			sprintf( EntData[ENTINDEX(tEntity)].digitgroup, "%s", Cmd_Argv(1));

			if (FStrEq(Cmd_Argv(2), "rot"))
			{
				// rotates. get the distance, angular speed, and initial angle

				pRunOnPev->iuser1 = atoi(Cmd_Argv(3));
				pRunOnPev->fuser1 = atof(Cmd_Argv(4));
				pRunOnPev->iuser3 = atoi(Cmd_Argv(5));
	
				// mark original origin
				pRunOnPev->vuser1 = pEntity->v.origin;
				pRunOnPev->avelocity.y = 0;
	
			}


			
	return 0;
}

bool DanceCreateMachine( Vector cent, edict_t *pEntity, const char *dancename)
{


	entvars_t *pPev = VARS( pEntity );


	KeyValueData	kvd;
			
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
		
	entvars_t *pev;
	pev =  VARS(tEntity);
			
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "building_dancemachine";

	DispatchKeyValue( tEntity, &kvd );

	// place this in front
	
	pev->origin = cent;
	pev->solid = SOLID_BBOX;
	
	SET_ORIGIN( tEntity , cent );
	//pev->angles = angles;
	pev->solid = SOLID_BBOX;

	SET_MODEL( ENT( pev ) , "models/sphere.mdl");

	UTIL_SetSize( pev, Vector( -16, -16 , -16), Vector(16, 16, 16) );

	pev->takedamage = DAMAGE_NO;
	pev->max_health = 40 + 10000;
	pev->health = 40 + 10000;
	pev->euser4 = pEntity;
	pev->movetype = MOVETYPE_FLY;

	// play deploy sound
	EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);
	pev->nextthink = gpGlobals->time + 1;
			
	pev->avelocity.y = 50;
	
	sprintf( EntData[ENTINDEX(tEntity)].digitgroup, "%s", dancename);

	if (FStrEq(Cmd_Argv(2), "rot"))
	{
		// rotates. get the distance, angular speed, and initial angle

		pev->iuser1 = atoi(Cmd_Argv(3));
		pev->fuser1 = atof(Cmd_Argv(4));
		pev->iuser3 = atoi(Cmd_Argv(5));

		// mark original origin
		pev->vuser1 = pEntity->v.origin;
		pev->avelocity.y = 0;
	}
			
	return 0;

}

bool DanceCreateRing( edict_t *pEntity , int ringid)
{
	// Create the dance machine

	entvars_t *pPev = VARS( pEntity );


	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Admin only feature\n");
		return 0;
	}


	// figure out which ring

	if (ringid == 1)
	{
	
		Vector newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(0 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(0 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");
		
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(45 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(45 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");
		
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(90 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(90 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");
		
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(135 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(135 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(180 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(180 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(225 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(225 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(270 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(270 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(315 * (3.14159 / 180)) * 128;
		newOri.y = newOri.y + (float)sin(315 * (3.14159 / 180)) * 128;
		DanceCreateMachine( newOri, pEntity, "a");

	}
	else if (ringid == 2)
	{
		Vector newOri = pPev->origin;
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(45 * (3.14159 / 180)) * 256;
		newOri.y = newOri.y + (float)sin(45 * (3.14159 / 180)) * 256;
		DanceCreateMachine( newOri, pEntity, "b");
		
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(135 * (3.14159 / 180)) * 256;
		newOri.y = newOri.y + (float)sin(135 * (3.14159 / 180)) * 256;
		DanceCreateMachine( newOri, pEntity, "b");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(225 * (3.14159 / 180)) * 256;
		newOri.y = newOri.y + (float)sin(225 * (3.14159 / 180)) * 256;
		DanceCreateMachine( newOri, pEntity, "b");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(315 * (3.14159 / 180)) * 256;
		newOri.y = newOri.y + (float)sin(315 * (3.14159 / 180)) * 256;
		DanceCreateMachine( newOri, pEntity, "b");

	}
	else if (ringid == 3)
	{
		Vector newOri = pPev->origin;
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(45 * (3.14159 / 180)) * 64;
		newOri.y = newOri.y + (float)sin(45 * (3.14159 / 180)) * 64;
		DanceCreateMachine( newOri, pEntity, "c");
		
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(135 * (3.14159 / 180)) * 64;
		newOri.y = newOri.y + (float)sin(135 * (3.14159 / 180)) * 64;
		DanceCreateMachine( newOri, pEntity, "c");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(225 * (3.14159 / 180)) * 64;
		newOri.y = newOri.y + (float)sin(225 * (3.14159 / 180)) * 64;
		DanceCreateMachine( newOri, pEntity, "c");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(315 * (3.14159 / 180)) * 64;
		newOri.y = newOri.y + (float)sin(315 * (3.14159 / 180)) * 64;
		DanceCreateMachine( newOri, pEntity, "c");

	}	
	else if (ringid == 4)
	{
		Vector newOri = pPev->origin;
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(45 * (3.14159 / 180)) * 400;
		newOri.y = newOri.y + (float)sin(45 * (3.14159 / 180)) * 400;
		newOri.z = newOri.z + 100;
		DanceCreateMachine( newOri, pEntity, "e");
		
		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(135 * (3.14159 / 180)) * 400;
		newOri.y = newOri.y + (float)sin(135 * (3.14159 / 180)) * 400;
		newOri.z = newOri.z + 100;
		DanceCreateMachine( newOri, pEntity, "e");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(225 * (3.14159 / 180)) * 400;
		newOri.y = newOri.y + (float)sin(225 * (3.14159 / 180)) * 400;
		newOri.z = newOri.z + 100;
		DanceCreateMachine( newOri, pEntity, "e");

		newOri = pPev->origin;
		newOri.x = newOri.x + (float)cos(315 * (3.14159 / 180)) * 400;
		newOri.y = newOri.y + (float)sin(315 * (3.14159 / 180)) * 400;
		newOri.z = newOri.z + 100;
		DanceCreateMachine( newOri, pEntity, "e");

	}		
//			sprintf( EntData[ENTINDEX(tEntity)].digitgroup, "%s", Cmd_Argv(1));



	return 1;
}

void DancePrecache()
{
	PRECACHE_MODEL("models/sphere.mdl");
}

void DanceThink ( edict_t *pent )
{
	entvars_t *pev = VARS(pent);

	if (pev->iuser2 > 0 && pev->fuser3 == 0)
	{
		// rotate position
		double ang = (double)pev->iuser3 + (double)pev->fuser1;
		double newx = (double)pev->vuser1.x + cos(ang * (3.14159 / 180)) * (double)pev->iuser1;
		double newy = (double)pev->vuser1.y + sin(ang * (3.14159 / 180)) * (double)pev->iuser1;
		if (ang > 360) ang = ang - 360;
		if (ang < 0) ang = ang + 360;

		pev->iuser3 = (int)ang;


		// set origin

		Vector newOri;
		newOri.x = newx;
		newOri.y = newy;
		newOri.z = pev->origin.z;

		//SET_ORIGIN(pent, newOri);

	

		Vector MoveVec = newOri - pev->origin;
		pev->velocity = MoveVec;

	}
	if (pev->iuser4 > 0)
	{
		// center on ent

		edict_t *pen = INDEXENT(pev->iuser4);

		if (pen && !FNullEnt(pen))
		{

			pev->vuser1 = pen->v.origin;


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
			pev->avelocity = goVec * 4;


			if (pev->fuser3 != 0)
			{
				// chase cam on this person

				// move self to be BEHIND him, iuser1 units away, and fuser3 units above/below

				

				//UTIL_MakeVectors( lookVec );
				//lookVec = gpGlobals->v_forward * -100;

				//lookVec.x = 0;
				//lookVec.z = 0;

				//lookVec = lookVec * (double)pev->iuser1;


				
				double ang = (double)pen->v.v_angle.y;
				ang -= 180;

				double newx = (double)pen->v.origin.x + cos(ang * (3.14159 / 180)) * (double)pev->iuser1;
				double newy = (double)pen->v.origin.y + sin(ang * (3.14159 / 180)) * (double)pev->iuser1;

				Vector lookVec = pen->v.origin;

				lookVec.x = newx;
				lookVec.y = newy;
				lookVec.z += pev->fuser3;


				// move here



				Vector MoveVec = lookVec - pev->origin;
				pev->velocity = MoveVec;					



			}

			
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

