
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
#include "buildcam.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

int g_sModelIndexFireball;
float menushow[33];

static FILE *fp;

// This CPP deals with creating the engineers CAMERA (The THIRD thing they can build)
// Basics: We use the TRIPMINE model, and stick it onto the wall just as we would a tripmine!

enum tripmine_e {
	TRIPMINE_IDLE1 = 0,
	TRIPMINE_IDLE2,
	TRIPMINE_ARM1,
	TRIPMINE_ARM2,
	TRIPMINE_FIDGET,
	TRIPMINE_HOLSTER,
	TRIPMINE_DRAW,
	TRIPMINE_WORLD,
	TRIPMINE_GROUND,
};

string_t clsname;



bool CamCreate( edict_t *pEntity )
{
	// Create the camera and stick to the wall

	entvars_t *pPev = VARS( pEntity );

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

			if (angles.x > 30 || angles.x < -30)
			{
				ClientPrint( pPev, HUD_PRINTTALK, "* Can't place cameras on floors or cielings!\n");
				return 0;
			}
			// Create the camera here!

			KeyValueData	kvd;
			
			//edict_t *pent = CREATE_ENTITY();
			edict_t *pent = CREATE_NAMED_ENTITY(clsname);
			entvars_t *pev = VARS( pent );
			
			pev->origin = tr.vecEndPos;
			pev->angles = angles;
			pev->controller[0] = 127;
			pev->controller[1] = 127;
			pev->euser4 = pEntity;

			//pev->iuser1 = angles.y;
			
			pev->vuser3 = angles;
			

			// for now don't take damage
			pev->takedamage = DAMAGE_YES;
			pev->max_health = 40 + 10000;
			pev->health = 40 + 10000;

			// Call the SPAWN routine to set more stuff

			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "building_camera";
			DispatchKeyValue( pent, &kvd );
			kvd.fHandled = FALSE;


			// Create the ent that we see out of.

			edict_t *pentview = CREATE_NAMED_ENTITY(clsname);

			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "building_camera_view";
			DispatchKeyValue( pentview, &kvd );
			kvd.fHandled = FALSE;

			pentview->v.angles = angles;
			pentview->v.origin = pev->origin + tr.vecPlaneNormal * 24;
			pentview->v.origin.z += 12;
			pentview->v.movetype = MOVETYPE_FLY;
			pentview->v.solid = SOLID_BBOX;
			pentview->v.takedamage = DAMAGE_NO;
			pentview->v.health = 20;
			pentview->v.euser4 = pEntity;

			SET_MODEL(pentview, "models/shell.mdl");

			pentview->v.angles.x = -pentview->v.angles.x;

			pentview->v.rendermode = 2;
			pentview->v.renderamt = 0;
			
			UTIL_SetOrigin( VARS(pentview), pentview->v.origin );

			pev->euser1 = pentview;

			(*other_gFunctionTable.pfnSpawn)(pentview);

			CamSpawn( pent );
	
			return 1;

		}
		else
		{
			ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place camera here!\n");
			return 0;
		}
		
	}
	else
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place camera here!\n");
		
	}
	return 0;



}

void CamSpawn( edict_t *pent )
{
	
	// Spawn routine

	entvars_t *pev = VARS( pent );

	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	//SET_MODEL(pent, "models/v_tripmine.mdl");
	SET_MODEL(pent, "avatar-x/avadd13.avfil");

	pev->frame = 0;
	//pev->body = 3;
	//pev->sequence = TRIPMINE_WORLD;
	pev->framerate = 0;
	
	UTIL_SetSize(pev, Vector( -12, -12, -12), Vector(12, 12, 12));
	UTIL_SetOrigin( pev, pev->origin );

	// play deploy sound
	EMIT_SOUND_DYN2( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);
	
	pev->iuser2 = pev->euser4->v.team; // Set the team this camera belongs to

	pev->nextthink = gpGlobals->time + 0.1;

	//	(*other_gFunctionTable.pfnSpawn)(pent);

	//pent->v.classname = clsname;
	UTIL_SetSize(pev, Vector( -12, -12, -12), Vector(12, 12, 12));
	SET_ORIGIN( pent, pev->origin );

	pev->solid = SOLID_BBOX;
	
}

void CamPrecache()
{

	//PRECACHE_MODEL("models/v_tripmine.mdl");
	PRECACHE_MODEL("avatar-x/avadd13.avfil");
	PRECACHE_SOUND("weapons/mine_deploy.wav");
	PRECACHE_SOUND("buttons/lever1.wav");
	

	g_sModelIndexFireball = PRECACHE_MODEL ("sprites/zerogxplode.spr");// fireball
	//clsname = MAKE_STRING("building_camera");
	clsname = MAKE_STRING("info_target");


}

void CamSee( edict_t *pEntity )
{
	// See out of the camera

	int i = 1;
	char *pClassname;
	edict_t *frontEnt;


	//frontEnt = pEntity->v.euser3;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_camera", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{
					//frontEnt->v.angles.x = -frontEnt->v.angles.x;
					UTIL_SetSize(VARS(frontEnt), Vector( -12, -12, -12), Vector(12, 12, 12));
					SET_ORIGIN( frontEnt, frontEnt->v.origin );

					frontEnt->v.iuser3 = 1;
					frontEnt->v.vuser4 = pEntity->v.v_angle;
					SET_VIEW( pEntity, frontEnt->v.euser1 );
					
					return;

				}
			}
		}
	}
}

void CamOff( edict_t *pEntity )
{
	// See out of the camera

	// first find their camera


	SET_VIEW( pEntity, pEntity );
	
	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	//frontEnt = pEntity->v.euser3;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_camera", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{
					//frontEnt->v.angles.x = -frontEnt->v.angles.x;

					frontEnt->v.iuser3 = 0;
					frontEnt->v.euser1->v.avelocity.y = 0;

					// reset to default state
					frontEnt->v.controller[0] = 127;
					frontEnt->v.controller[1] = 127;
					frontEnt->v.euser1->v.angles = frontEnt->v.vuser3;
					frontEnt->v.euser1->v.avelocity = g_vecZero;
					
					return;

				}
			}
		}
	}
}

void CamKill( edict_t *pEntity )
{
	// Delete the camera

	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_camera", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{

					// Delete this camera's VIEW and it.

					frontEnt->v.euser1->v.flags |= FL_KILLME;
					frontEnt->v.flags |= FL_KILLME;
				}
			}
		}
	}
}

void CamKillBoom( edict_t *pEntity )
{
	// Delete the camera

	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_camera", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{

					// Delete this camera's VIEW and it.

					frontEnt->v.euser1->v.flags |= FL_KILLME;
					frontEnt->v.flags |= FL_KILLME;

					// Boom

					MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, frontEnt->v.origin );
						WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
						WRITE_COORD( frontEnt->v.origin.x );	// Send to PAS because of the sound
						WRITE_COORD( frontEnt->v.origin.y );
						WRITE_COORD( frontEnt->v.origin.z );
						WRITE_SHORT( g_sModelIndexFireball );
						WRITE_BYTE( 30  ); // scale * 10
						WRITE_BYTE( 15  ); // framerate
						WRITE_BYTE( 0 );
					MESSAGE_END();

					// Test: Make euser4 take damage.
					/*
					fp=fopen("takedamage.txt","a");
					fprintf(fp, "Got to POINT 0A\n");
					fclose(fp);	

					CBaseEntity *pEnt = NULL;
					pEnt = CBaseEntity::Instance( pEntity );

					fp=fopen("takedamage.txt","a");
					fprintf(fp, "Got to POINT 0B\n");
					fclose(fp);	

					pEnt->TakeDamage( VARS( frontEnt ), VARS( frontEnt ), 10, DMG_CRUSH );

*/

				}
			}
		}
	}
}



bool CamCheck ( edict_t *pent )
{
	// Check if they have a camera

	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_camera", pClassname)) {

				if (frontEnt->v.euser4 == pent) 
				{

					// Delete this camera's VIEW and it.
					return 1;

				}
			}
		}
	}

	return 0;

}

void CamThink ( edict_t *pent )
{

	// turn the camera around a bit

	

	if (pent->v.iuser3 != 0) {
	
		/* //old code

		int angadd = pent->v.iuser4;
		int origang = pent->v.iuser1;

		if (angadd == 0) angadd = 11;

		int angval = origang - pent->v.euser1->v.angles.y;

		if (angval > 25 && angadd < 0) angadd = -angadd;
		if (angval < -25 && angadd > 0) angadd = -angadd;

		pent->v.euser1->v.avelocity.y = angadd;

		pent->v.iuser4 = angadd;
		*/

		// make the camera point the same way the user is looking
		Vector theone;
		Vector thetwo;

		theone = pent->v.euser4->v.v_angle;
		theone.y = pent->v.vuser3.y - (pent->v.vuser4.y - pent->v.euser4->v.v_angle.y) ;

		// see the difference and if needed subtract.

		thetwo = pent->v.vuser3 - theone;

		if (thetwo.y > 360) thetwo.y -= 360;
		if (thetwo.y < -360) thetwo.y += 360;
		if (thetwo.y > 180) thetwo.y -= 360;
		if (thetwo.y < -180) thetwo.y += 360;
		
		if (thetwo.x > 30) thetwo.x = 30;
		if (thetwo.x < -30) thetwo.x = -30;
		if (thetwo.y > 70) thetwo.y = 70;
		if (thetwo.y < -70) thetwo.y = -70;
		thetwo = -thetwo;
		
		// attempt at avelocity

		theone = pent->v.vuser3 + thetwo;
		pent->v.euser1->v.avelocity.y = ((theone.y - pent->v.euser1->v.angles.y) * 1.8);
		pent->v.euser1->v.avelocity.x = ((theone.x - pent->v.euser1->v.angles.x) * 1.8);
	
		// figure out the bone controller value
		// set the camera controllers so it looks at where the player is seeing

		pent->v.controller[0] = (int)(thetwo.y + 127);
		pent->v.controller[1] = (int)(thetwo.x + 127);

		// How about when the camera is TURNING it makes a little grating sound!!
		// Only play the sound if the camera is turning relatively fast

		if (abs(pent->v.euser1->v.avelocity.y) > 30 && pent->v.fuser2 < gpGlobals->time)
		{
			//														 ||| this is the volume
			EMIT_SOUND_DYN2( pent, CHAN_VOICE, "buttons/lever1.wav", 0.8, ATTN_NORM , 0, 110); // lets play it faster than normal
			pent->v.fuser2 = gpGlobals->time + 2;
		}
	}

	if (!pent->v.euser4 || pent->v.health <= 10000 || pent->v.euser4->v.iuser1 != 0 || pent->v.iuser2 != pent->v.euser4->v.team)
	{
		// We've been destroyed! Make a boom and tell the euser4

		if (pent->v.euser4) {
			
			ClientPrint( VARS( pent->v.euser4 ), HUD_PRINTTALK, "* Your camera has been destroyed!\n");
			CamOff( pent->v.euser4 );
		}
		pent->v.euser1->v.flags |= FL_KILLME;
		pent->v.flags |= FL_KILLME;
		// Boom
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pent->v.origin );
			WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD( pent->v.origin.x );	// Send to PAS because of the sound
			WRITE_COORD( pent->v.origin.y );
			WRITE_COORD( pent->v.origin.z );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 30  ); // scale * 10
			WRITE_BYTE( 15  ); // framerate
			WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	// check if the PLAYER is dead.
	
	if (pent->v.euser4->v.deadflag != DEAD_NO && pent->v.iuser3 != 0)
	{
		// turn off the camera
		CamOff( pent->v.euser4 );
	}


	pent->v.nextthink = gpGlobals->time + 0.2;

}

void CamShowMenu( edict_t *pEntity )
{
	// Cant use camera in spec mode
	if (pEntity->v.iuser1 > 0) return;

	char menutext[1024];
	int bitsel = 0;

	sprintf(menutext, "Build Camera Menu:\n\n");

	sprintf(menutext, "%s1. Place Camera Here\n", menutext);
	sprintf(menutext, "%s2. Dismantle Camera\n", menutext);
	sprintf(menutext, "%s3. Detonate Camera\n\n", menutext);
	
	sprintf(menutext, "%s4. Turn On Camera\n", menutext);
	sprintf(menutext, "%s5. Turn Off Camera\n\n", menutext);
	
	sprintf(menutext, "%s6. Cancel\n\n", menutext);
	
	sprintf(menutext, "%sNote: Bind +camlook to quickly\npeek into your camera!", menutext);
		
	
	bitsel |= 1<<0; // activate this choice in the menu 1
	bitsel |= 1<<1; // activate this choice in the menu 2
	bitsel |= 1<<2; // activate this choice in the menu 3
	bitsel |= 1<<3; // activate this choice in the menu 4
	bitsel |= 1<<4; // activate this choice in the menu 5
	bitsel |= 1<<5; // activate this choice in the menu 6
	
	// Now, show the menu to everyone!


	int gmsgShowMenu = 0;
	gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 ); 

	MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pEntity);

		WRITE_SHORT( bitsel);
		WRITE_CHAR( 60 );
		WRITE_BYTE( 0 );
		WRITE_STRING (menutext);

	MESSAGE_END();

	// Set the values that determine how long we accept input...
	menushow[ENTINDEX(pEntity)] = gpGlobals->time + 60;
}

void CamHandleMenuItem(edict_t *pEntity, const char *itm, bool ign)
{

	int ind = ENTINDEX(pEntity);

	if (!pEntity) return;
	
	// Cant use camera in spec mode
	if (pEntity->v.iuser1 > 0) return;
	
	// Are we running a vote?
	if ((menushow[ind] != 0 && (gpGlobals->time <= menushow[ind]) || ign)) {

		if (ign == 0) menushow[ind] = 0;

		// See what they pressed.
		char msg[80];
		sprintf(msg, "0");

		if (FStrEq(itm, "1"))
		{
			if (CamCheck(pEntity))
			{
				sprintf(msg, "* You already have a camera! Remove it first!\n");
			}
			else
			{

				// Make sure we are allowed to build a camera
				// we cant be dead
				if (pEntity->v.deadflag == DEAD_NO)
				{

					if (CamCreate( pEntity )) sprintf(msg, "* You have built a camera!\n");
				}
			}
		}
		else if (FStrEq(itm, "2"))
		{
			if (!CamCheck(pEntity))
			{
				sprintf(msg, "* You don't have a camera to dismantle!\n");
			}
			else
			{
				CamOff( pEntity );
				CamKill( pEntity );
				sprintf(msg, "* You have dismantled your camera!\n");
			}
		}
		else if (FStrEq(itm, "3"))
		{
			if (!CamCheck(pEntity))
			{
				sprintf(msg, "* You don't have a camera to detonate!\n");
			}
			else
			{
				CamOff( pEntity );
				CamKillBoom( pEntity ); // same as kill but with a boom
				sprintf(msg, "* You have detonated your camera!\n");
			}
		}
		else if (FStrEq(itm, "4"))
		{
			if (!CamCheck(pEntity))
			{
				sprintf(msg, "* You don't have a camera to turn on!\n");
			}
			else
			{
				CamSee( pEntity );
				if (!ign) sprintf(msg, "* Camera turned on!\n");
			}
		}	
		else if (FStrEq(itm, "5"))
		{
			if (!CamCheck(pEntity))
			{
				if (!ign) sprintf(msg, "* You don't have a camera to turn off!\n");
			}
			else
			{
				CamOff( pEntity );
				if (!ign) sprintf(msg, "* Camera turned off!\n");
			}
		}	
	
		if (!FStrEq(msg, "0")) ClientPrint( VARS(pEntity), HUD_PRINTTALK, msg);

	}
}

void CamTouch( edict_t *pEntity, edict_t *pTouch )
{

	if (FStrEq("player", (char *)STRING(pTouch->v.classname))) 
	{
		// player is touching us... make sure we arent stuck in him

		bool stuck = 0;
		entvars_t *pev = VARS(pTouch);
		entvars_t *sign = VARS(pEntity);

		stuck = 1;

		if ( sign->absmin.x + 1 > pev->absmax.x - 1 ||
			 sign->absmin.y + 1 > pev->absmax.y - 1 ||
			 sign->absmin.z + 1 > pev->absmax.z - 1 ||
			 sign->absmax.x - 1 < pev->absmin.x + 1 ||
			 sign->absmax.y - 1 < pev->absmin.y + 1 ||
			 sign->absmax.z - 1 < pev->absmin.z + 1 ) stuck = 0;

		if (stuck)
		{
			// player is blocked, remove sign
			sign->health = 1;
			//ClientPrint( pev, HUD_PRINTTALK, "* Camera removed because you were stuck in it!\n");
		}
	}
}
