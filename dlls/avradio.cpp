
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
#include "avradio.h"
#include "avdamage.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;
int g_sModelIndexFireball2;

// This CPP deals with creating the engineers CAMERA (The THIRD thing they can build)
// Basics: We use the TRIPMINE model, and stick it onto the wall just as we would a tripmine!

float menushow2[33];
int lastMenuPage[33];
int lastMenuEnt[33];


int lastButtons[33];

#define DJSPOOF "avatar-x/djspoof1.wav"



extern bool AdminLoggedIn[33];

bool RadioCreate( edict_t *pEntity )
{
	// Create the radio and stick to the wall

	entvars_t *pPev = VARS( pEntity );

	if (pPev->iuser1 > 0) return 0;


	// make sure we dont already have a radio

	int radiocount = 0;
	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_radio", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{
					radiocount++;
				}
			}
		}
	}

	
	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0)
	{
		if (radiocount >= 2) {
			ClientPrint( pPev, HUD_PRINTTALK, "* Cant have more than 2 radios!\n");
			return 0;
		}
	}

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, pEntity , &tr );

	if (tr.flFraction < 1.0 || AdminLoggedIn[ENTINDEX(pEntity)])
	{

		
		if (tr.pHit && !(tr.pHit->v.flags & FL_CONVEYOR) && (FStrEq((char *)STRING(tr.pHit->v.classname), "worldspawn") || FStrEq((char *)STRING(tr.pHit->v.classname), "func_wall") || AdminLoggedIn[ENTINDEX(pEntity)] || FStrEq((char *)STRING(tr.pHit->v.classname), "building_dancemachine")))	// Make sure it isnt a conveyor!
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			if ((angles.x > 30 || angles.x < -30) && AdminLoggedIn[ENTINDEX(pEntity)] == 0)
			{
				ClientPrint( pPev, HUD_PRINTTALK, "* Can't place radios on floors or cielings!\n");
				return 0;
			}
			// Create the camera here!
			Vector vecOri = tr.vecEndPos + tr.vecPlaneNormal * 14;
			//Vector vecOri = tr.vecEndPos + tr.vecPlaneNormal * 15;

			int maxdist = (int)CVAR_GET_FLOAT("sa_radiospread");

			// make sure we arent placing it within 400 units of another radio
			for (i=1; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pClassname =  (char *)STRING(frontEnt->v.classname); 
					if (FStrEq("building_radio", pClassname)) {
						
						if ((frontEnt->v.origin - vecOri).Length() < maxdist && AdminLoggedIn[ENTINDEX(pEntity)] == 0)
						{
							ClientPrint( pPev, HUD_PRINTTALK, "* Can't place a radio so close to another radio!\n");
							return 0;
						}
					}
				}
			}

			KeyValueData	kvd;
			
			//edict_t *pent = CREATE_ENTITY();
			//edict_t *pent = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

			edict_t *tEntity;
			tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
		
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(tEntity);
			
			char buf[80];
			sprintf( buf, "%s", "building_radio");

			// Set the KEYVALUES here!
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = buf;

			DispatchKeyValue( tEntity, &kvd );

			// place this in front

			pRunOnPev->origin = vecOri;
			SET_ORIGIN( tEntity , vecOri );
			pRunOnPev->angles = angles;

			//DispatchSpawn( ENT( pRunOnPev ) );

			pRunOnPev->solid = SOLID_BBOX;

			SET_MODEL( ENT( pRunOnPev ) , "avatar-x/avadd16.avfil");
			UTIL_SetSize( pRunOnPev, Vector( -2, -2 ,-2) - (tr.vecPlaneNormal * 15), Vector(2, 2, 16) - (tr.vecPlaneNormal * 15));
			
			pRunOnPev->takedamage = DAMAGE_YES;
			pRunOnPev->max_health = 40 + 10000;
			pRunOnPev->health = 40 + 10000;
			pRunOnPev->euser4 = pEntity;
			
			/*
			edict_t *pent;
			pent = CREATE_NAMED_ENTITY(MAKE_STRING("xen_tree"));
			entvars_t *pv = VARS( pent );
			
			pv->origin = vecOri;
			SET_ORIGIN(pent, vecOri);

			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "xen_tree";
			DispatchKeyValue( pent, &kvd );

			DispatchSpawn(pent);
			//pev->angles = angles;
			
			
			*/

			//pev->iuser1 = angles.y;
			//pv->vuser3 = angles;

			// for now don't take damage
			//pev->takedamage = DAMAGE_YES;
			//pev->max_health = 40 + 10000;
			//pev->health = 40 + 10000;

			/*
			// Call the SPAWN routine to set more stuff
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "building_radio";
			//DispatchKeyValue( pent, &kvd );
			kvd.fHandled = FALSE;

			

			
			*/

			RadioSpawn( tEntity );
	
			return 1;
		}
		else
		{
			ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place radio here!\n");
			return 0;
		}
		
	}
	else
	{
		ClientPrint( pPev, HUD_PRINTTALK, "* Couldn't place radio here!\n");
		
	}
	return 0;
}

void RadioSpawn( edict_t *tEntity )
{
	
	// Spawn routine

	entvars_t *pRunOnPev = VARS( tEntity );

	//pev->movetype = MOVETYPE_NONE;
	tEntity->v.solid = SOLID_NOT;

	//SET_MODEL(pent, "avatar-x/avadd16.avfil");
	

	//pev->frame = 0;
	//pev->framerate = 0;
	
	//UTIL_SetSize(pev, Vector( -12, -12, -12), Vector(12, 12, 12));
	//UTIL_SetOrigin( pev, pev->origin );

	// play deploy sound
	EMIT_SOUND_DYN2( tEntity, CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM , 0, 100);
	
	pRunOnPev->iuser2 = pRunOnPev->euser4->v.team; // Set the team this radio belongs to
	pRunOnPev->nextthink = gpGlobals->time + 1;
	pRunOnPev->fuser4 = gpGlobals->time + 70;

	// set default song
	pRunOnPev->iuser3 = 1;

	// set default pitch
	pRunOnPev->iuser1 = 101;

	

	//UTIL_SetSize(pev, Vector( -12, -12, -12), Vector(12, 12, 12));
	//SET_ORIGIN( pent, pev->origin );
	//pev->solid = SOLID_BBOX;
	
	// start playing annoying music
	//UTIL_EmitAmbientSound(ENT(pev), pev->origin, szSoundFile, 0, 0, SND_STOP, 0);

	//ATTN_STATIC

	char songname[80];
	RadioGetSong( pRunOnPev->iuser3, songname);
	UTIL_EmitAmbientSound(tEntity, pRunOnPev->origin, songname, 1.0, ATTN_STATIC, 0, pRunOnPev->iuser1);
	
	pRunOnPev->fuser3 = gpGlobals->time + (float)RANDOM_LONG(30, 60);


}

void RadioPrecache()
{

	
//	PRECACHE_MODEL("models/w_oxygen.mdl");

	//PRECACHE_SOUND("doors/aliendoor3.wav");

	PRECACHE_MODEL("avatar-x/avadd16.avfil");
	PRECACHE_SOUND("avatar-x/avadd21.avfil");
	PRECACHE_SOUND("avatar-x/avjoins.wav");
	PRECACHE_SOUND("weapons/mine_deploy.wav");

	PRECACHE_SOUND(RADLOOP1);
	PRECACHE_SOUND(RADLOOP2);
	PRECACHE_SOUND(RADLOOP3);
	PRECACHE_SOUND(RADLOOP4);
	PRECACHE_SOUND(RADLOOP5);
	PRECACHE_SOUND(RADLOOP6);
	PRECACHE_SOUND(RADLOOP7);
	PRECACHE_SOUND(RADLOOP8);
	PRECACHE_SOUND(RADLOOP9);
	PRECACHE_SOUND(RADLOOP10);


	/*
	if (CVAR_GET_FLOAT("sa_premusic") == 1)
	{
		PRECACHE_SOUND(RADLOOP5);
		PRECACHE_SOUND(RADLOOP6);
		PRECACHE_SOUND(RADLOOP7);
		PRECACHE_SOUND(RADLOOP8);
	}
	*/

	PRECACHE_SOUND("avatar-x/djtalk1.wav");
	PRECACHE_SOUND("avatar-x/djtalk2.wav");
	PRECACHE_SOUND("avatar-x/djtalk3.wav");
	PRECACHE_SOUND("avatar-x/avjoins.wav");

	g_sModelIndexFireball2 = PRECACHE_MODEL ("sprites/hexplo.spr");// fireball
	
}

void RadioKill( edict_t *pEntity )
{
	// Delete the radio

	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_radio", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{

					// Delete this radio
					frontEnt->v.flags |= FL_KILLME;

				}
			}
		}
	}
}

void RadioKillBoom( edict_t *pEntity )
{
	// Delete the radio

	int i = 1;
	char *pClassname;
	edict_t *frontEnt;

	for (i; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		if (frontEnt) {
			pClassname =  (char *)STRING(frontEnt->v.classname); 
			if (FStrEq("building_radio", pClassname)) {

				if (frontEnt->v.euser4 == pEntity) 
				{

					// Delete this radio
					
					frontEnt->v.flags |= FL_KILLME;

					// Boom

					MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, frontEnt->v.origin );
						WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
						WRITE_COORD( frontEnt->v.origin.x );	// Send to PAS because of the sound
						WRITE_COORD( frontEnt->v.origin.y );
						WRITE_COORD( frontEnt->v.origin.z );
						WRITE_SHORT( g_sModelIndexFireball2 );
						WRITE_BYTE( 30  ); // scale * 10
						WRITE_BYTE( 15  ); // framerate
						WRITE_BYTE( 0 );
					MESSAGE_END();
					
					char songname[80];
					RadioGetSong( frontEnt->v.iuser3, songname);

					UTIL_EmitAmbientSound(frontEnt, frontEnt->v.origin, songname, 0, 0, SND_STOP, 0);

					

				}
			}
		}
	}
}

void RadioThink ( edict_t *pent )
{


	if (pent->v.fuser3 < gpGlobals->time)
	{
		// babble
		pent->v.fuser3 = gpGlobals->time + (float)RANDOM_LONG(50, 80);
	
		int babnum = RANDOM_LONG(1, 3);
		if (babnum == 1) EMIT_SOUND_DYN2(pent, CHAN_WEAPON, "avatar-x/djtalk1.wav", 0.98, ATTN_STATIC, 0, 100); 
		if (babnum == 2) EMIT_SOUND_DYN2(pent, CHAN_WEAPON, "avatar-x/djtalk2.wav", 0.98, ATTN_STATIC, 0, 100); 
		if (babnum == 3) EMIT_SOUND_DYN2(pent, CHAN_WEAPON, "avatar-x/djtalk3.wav", 0.98, ATTN_STATIC, 0, 100); 

	}

	if (pent->v.iuser4 == 1)
	{

		// Start playing new song
		char songname[80];
		RadioGetSong( pent->v.iuser3 , songname);
		UTIL_EmitAmbientSound(pent, pent->v.origin, songname, 1.0, ATTN_STATIC, 0, pent->v.iuser1);
		
		pent->v.iuser4 = 0;
		pent->v.fuser4 = gpGlobals->time + 70;

	}
	if (pent->v.fuser1 == 0) 
	{
		pent->v.solid = SOLID_BBOX;
		pent->v.fuser1 = 1;
	}
	if (pent->v.fuser4 < gpGlobals->time) 
	{

		// stop and restart the radio
		//char songname[80];
		//RadioGetSong( pent->v.iuser3, songname);

		pent->v.fuser4 = gpGlobals->time + 70;
	}

	// see if we have been destroyed
	if (!pent->v.euser4 || pent->v.health <= 10000 || pent->v.euser4->v.iuser1 != 0 || pent->v.iuser2 != pent->v.euser4->v.team)
	{
		// We've been destroyed! Make a boom and tell the euser4

		if (pent->v.euser4) {
			
			ClientPrint( VARS( pent->v.euser4 ), HUD_PRINTTALK, "* Your radio has been destroyed!\n");
		}
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

		char songname[80];
		RadioGetSong( pent->v.iuser3, songname);
		UTIL_EmitAmbientSound(pent, pent->v.origin, songname, 0, 0, SND_STOP, 0);

		// hurt anyone who is near this radio
		//AvTakeDamage( VARS(pent->v.euser4), VARS(pent->v.euser4), VARS(pent), 60, DMG_BLAST);
		AvRadiusDamage( pent->v.origin, VARS(pent), VARS(pent->v.euser4), 60, 200, DMG_BLAST);
	}

	pent->v.nextthink = gpGlobals->time + 0.2;

}


void RadioUse( edict_t *pentUsed, edict_t *pEntity )
{
	if (pentUsed->v.iuser4 == 1) return;

	// Show the menu

	RadioShowMenu( pEntity, pentUsed, 1);

}

#define MAX_MENU_CHUNK 150

void RadioShowMenu( edict_t *pEntity, edict_t *pRadio, int n )
{
	// Cant use radio in spec mode
	if (pEntity->v.iuser1 > 0) return;
	// dont use if radio is tuning
	if (pRadio->v.iuser1 == 1) return;

	char menutext[1024];
	int bitsel = 0;

	if (n < 3)
	{

		sprintf(menutext, "Switch to song:\n\n");

		if (n == 1)
		{

			strcat(menutext, "1. Take my Breath Away\n");
			strcat(menutext, "2. Dreamgate - Now we are free\n");
			strcat(menutext, "3. Hamsterdance\n");
			strcat(menutext, "4. Muse - Plug In Baby\n");
			strcat(menutext, "5. Scooter - Ramp [Supertramp - Logical Song]\n\n");
			
			strcat(menutext, "6. Show Page 2\n\n");
			
		}
		else
		{

			strcat(menutext, "1. Boomtang Boys - Popcorn [Micro Mix]\n");
			strcat(menutext, "2. Mr. Roboto\n");
			strcat(menutext, "3. Chicane - Salt Water\n");
			strcat(menutext, "4. John Jeski - Sailing Accordion Polka\n");
			strcat(menutext, "5. Warrior - Voodoo\n\n");
			
			strcat(menutext, "6. Show Page 1\n\n");
		}
	
		strcat(menutext, "7. Switch to next song\n");
		strcat(menutext, "8. Switch to previous song\n\n");
		strcat(menutext, "9. Show pitch menu\n");
	
	}
	else
	{

		sprintf(menutext, "Change the pitch and speed:\n\n");

		strcat(menutext, "1. Change to normal pitch\n\n");
		
		strcat(menutext, "2. 0.5x speed/pitch\n");
		strcat(menutext, "3. 0.8x speed/pitch\n");
		strcat(menutext, "4. 1.2x speed/pitch\n");
		strcat(menutext, "5. 1.4x speed/pitch\n");
		strcat(menutext, "6. 1.5x speed/pitch\n");
		strcat(menutext, "7. 1.8x speed/pitch\n");
		strcat(menutext, "8. 2.0x speed/pitch\n\n");
		
		strcat(menutext, "9. Back to song menu\n");


	}


	strcat(menutext, "\n\n0. Cancel\n");
	
	bitsel |= 1<<0; // activate this choice in the menu 1
	bitsel |= 1<<1; // activate this choice in the menu 2
	bitsel |= 1<<2; // activate this choice in the menu 3
	bitsel |= 1<<3; // activate this choice in the menu 4
	bitsel |= 1<<4; // activate this choice in the menu 5
	bitsel |= 1<<5; // activate this choice in the menu 6
	bitsel |= 1<<6; // activate this choice in the menu 7
	bitsel |= 1<<7; // activate this choice in the menu 8
	bitsel |= 1<<8; // activate this choice in the menu 9
	bitsel |= 1<<9; // activate this choice in the menu 9

	// Now, show the menu to everyone!


	int gmsgShowMenu = 0;
	gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 ); 
	
	int char_count = 0;
	char *pFileList = menutext;
	char *aFileList = menutext;

	while ( pFileList && *pFileList && char_count < 1024 )
	{
		char chunk[MAX_MENU_CHUNK+1];
		
		if ( strlen( pFileList ) < MAX_MENU_CHUNK )
		{
			strcpy( chunk, pFileList );
		}
		else
		{
			strncpy( chunk, pFileList, MAX_MENU_CHUNK );
			chunk[MAX_MENU_CHUNK] = 0;		// strncpy doesn't always append the null terminator
		}

		char_count += strlen( chunk );
		
		if ( char_count < 1024 )
			pFileList = aFileList + char_count; 
		else
			*pFileList = 0;

		ALERT(at_console, "SENDING %i!!!\n", char_count);

		MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pEntity );
			WRITE_SHORT( bitsel );
			WRITE_CHAR( 60 );
			WRITE_BYTE( *pFileList ? TRUE : FALSE );	// TRUE means there is still more message to come
			WRITE_STRING( chunk );
		MESSAGE_END();
	}


	lastMenuPage[ENTINDEX(pEntity)] = n;
	lastMenuEnt[ENTINDEX(pEntity)] = ENTINDEX(pRadio);

	

	// Set the values that determine how long we accept input...
	menushow2[ENTINDEX(pEntity)] = gpGlobals->time + 60;
}


void RadioHandleMenuItem(edict_t *pEntity, const char *itm)
{

	int ind = ENTINDEX(pEntity);
	if (!pEntity) return;
	
	// Cant use camera in spec mode
	if (pEntity->v.iuser1 > 0) return;
	
	// Are we even supposed to hangle this?
	if (menushow2[ind] != 0 && (gpGlobals->time <= menushow2[ind])) {

		menushow2[ind] = 0;
		
		edict_t *pentUsed = INDEXENT(lastMenuEnt[ind]);
		if (FNullEnt(pentUsed)) return;

		// dont use if radio is tuning
		if (pentUsed->v.iuser1 == 1) return;


		// See what they pressed.
		
		// menu page 1

		if (lastMenuPage[ind] == 1 || lastMenuPage[ind] == 2)
		{

			if (lastMenuPage[ind] == 1)
			{

				if (FStrEq(itm, "1"))
				{
					RadioSwitchToSong(1, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "2"))
				{
					RadioSwitchToSong(2, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "3"))
				{
					RadioSwitchToSong(3, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "4"))
				{
					RadioSwitchToSong(4, lastMenuEnt[ind]);
				}	
				else if (FStrEq(itm, "5"))
				{
					RadioSwitchToSong(5, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "6"))
				{
					RadioShowMenu( pEntity, pentUsed, 2 );
				}
				
			}
			else if (lastMenuPage[ind] == 2)
			{
				// menu page 2
				if (FStrEq(itm, "1"))
				{
					RadioSwitchToSong(6, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "2"))
				{
					RadioSwitchToSong(7, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "3"))
				{
					RadioSwitchToSong(8, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "4"))
				{
					RadioSwitchToSong(9, lastMenuEnt[ind]);
				}	
				else if (FStrEq(itm, "5"))
				{
					RadioSwitchToSong(10, lastMenuEnt[ind]);
				}
				else if (FStrEq(itm, "6"))
				{
					RadioShowMenu( pEntity, pentUsed, 1 );
				}
			}

			if (FStrEq(itm, "7"))
			{
				RadioSwitchToSong(100, lastMenuEnt[ind]);
			}
			else if (FStrEq(itm, "8"))
			{
				RadioSwitchToSong(101, lastMenuEnt[ind]);
			}
			else if (FStrEq(itm, "9"))
			{
				RadioShowMenu( pEntity, pentUsed, 3 );
			}
		}
		else if (lastMenuPage[ind] == 3)
		{

			if (FStrEq(itm, "1"))
			{
				RadioChangeSongPitch(100, pentUsed);
			}
			else if (FStrEq(itm, "2"))
			{
				RadioChangeSongPitch(50, pentUsed);
			}
			else if (FStrEq(itm, "3"))
			{
				RadioChangeSongPitch(80, pentUsed);
			}
			else if (FStrEq(itm, "4"))
			{
				RadioChangeSongPitch(120, pentUsed);
			}
			else if (FStrEq(itm, "5"))
			{
				RadioChangeSongPitch(140, pentUsed);
			}
			else if (FStrEq(itm, "6"))
			{
				RadioChangeSongPitch(150, pentUsed);
			}
			else if (FStrEq(itm, "7"))
			{
				RadioChangeSongPitch(180, pentUsed);
			}
			else if (FStrEq(itm, "8"))
			{
				RadioChangeSongPitch(200, pentUsed);
			}
			else if (FStrEq(itm, "9"))
			{
				RadioShowMenu( pEntity, pentUsed, 1 );
			}
			
		}
	}
}




void RadioSwitchToSong(int n, int entCode)
{

	edict_t *pentUsed = INDEXENT(entCode);

	if (FNullEnt(pentUsed)) return;


	// make radio play something else
	// store current song in IUSER3
	int song = pentUsed->v.iuser3;
	char songname[80];

	if (pentUsed->v.iuser4 == 1) return;

	// cycle thru songs: 1 2 3 1 2 3 etc
	
	if (n == 100)
	{
		song++;
		if (song > 10) song = 1;
	}
	else if (n == 101)
	{
		song--;
		if (song < 1) song = 10;
	}
	else
	{
		song = n;
	}

	// Change songs
	// 1.87 seconds long... the tune sound
	// First stop the OLD sound from playing.
	RadioGetSong( pentUsed->v.iuser3, songname);
	UTIL_EmitAmbientSound(pentUsed, pentUsed->v.origin, songname, 0, 0, SND_STOP, 0);

	// Now play the TUNING SOUND

	UTIL_EmitAmbientSound(pentUsed, pentUsed->v.origin, "avatar-x/avadd21.avfil", 1.0, ATTN_STATIC, 0, 100);
	
	pentUsed->v.iuser3 = song;

	// Set to play the selected song in 1.87 seconds
	
	pentUsed->v.iuser4 = 1;
	pentUsed->v.nextthink = gpGlobals->time + 1.87;
	pentUsed->v.fuser4 = gpGlobals->time + 70;

}

void RadioChangeSongPitch(int newPitch, edict_t *pEntity)
{

	pEntity->v.iuser1 = newPitch;


	char songname[80];
	RadioGetSong( pEntity->v.iuser3, songname);
	UTIL_EmitAmbientSound(pEntity, pEntity->v.origin, songname, 1.0, ATTN_STATIC, SND_CHANGE_PITCH, pEntity->v.iuser1);

	
	ALERT(at_console, "Setting pitch to %i on %s\n", pEntity->v.iuser1, songname);

}


void RadioGetSong( int num, char *songname )
{
	
	
	// returns the song

	if (num == 1) sprintf(songname, RADLOOP1);
	if (num == 2) sprintf(songname, RADLOOP2);
	if (num == 3) sprintf(songname, RADLOOP3);
	if (num == 4) sprintf(songname, RADLOOP4);
	if (num == 5) sprintf(songname, RADLOOP5);
	if (num == 6) sprintf(songname, RADLOOP6);
	if (num == 7) sprintf(songname, RADLOOP7);
	if (num == 8) sprintf(songname, RADLOOP8);
	if (num == 9) sprintf(songname, RADLOOP9);
	if (num == 10) sprintf(songname, RADLOOP10);
	
	/*
	if (num == 8) sprintf(songname, RADLOOP5);
	if (num == 9) sprintf(songname, RADLOOP6);
	if (num == 10) sprintf(songname, RADLOOP7);
	if (num == 11) sprintf(songname, RADLOOP8);

	/*
	if (num == 8) sprintf(songname, RADLOOP5);
	if (num == 9) sprintf(songname, RADLOOP6);
	if (num == 10) sprintf(songname, RADLOOP7);
	if (num == 11) sprintf(songname, RADLOOP8);
	if (num == 12) sprintf(songname, RADLOOP9);
	if (num == 13) sprintf(songname, RADLOOP10);
	if (num == 14) sprintf(songname, RADLOOP11);
	if (num == 15) sprintf(songname, RADLOOP12);*/
}

void RadioGetSongName( int num, char *songname )
{
	
	// returns the song
	if (num == 1) sprintf(songname, "Take my Breath Away");
	if (num == 2) sprintf(songname, "Dreamgate - Now we are free");
	if (num == 3) sprintf(songname, "Hamsterdance");
	if (num == 4) sprintf(songname, "Muse - Plug In Baby");
	if (num == 5) sprintf(songname, "Scooter - Ramp [Supertramp - Logical Song]");
	if (num == 6) sprintf(songname, "Boomtang Boys - Popcorn [Micro Mix]");
	if (num == 7) sprintf(songname, "Mr. Roboto");
	if (num == 8) sprintf(songname, "Chicane - Salt Water");
	if (num == 9) sprintf(songname, "John Jeski - Sailing Accordion Polka");
	if (num == 10) sprintf(songname, "Warrior - Voodoo");

}


#define	PLAYER_SEARCH_RADIUS	(float)64

void RadioUseCheck(  edict_t *pEntity )
{

	// Elaborate and stupid function to see if the player is pressing USE,
	// cause if we dont do this we cant catch it cause its really handled by the TFC DLL

	int MyNumber = ENTINDEX( pEntity );
	
	entvars_t *pev = VARS( pEntity );

	int buttonsChanged = (lastButtons[MyNumber] ^ pev->button);	// These buttons have changed this frame
	int m_afButtonPressed =  buttonsChanged & pev->button;		// The changed ones still down are "pressed"
	int m_afButtonReleased = buttonsChanged & (~pev->button);	// The ones not down are "released"


	lastButtons[MyNumber] = pev->button;
		

	// Was use pressed or released?
	if ( !(buttonsChanged & IN_USE))
		return;

	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	Vector		vecLOS;
	float flMaxDot = VIEW_FIELD_NARROW;
	float flDot;

	UTIL_MakeVectors ( pev->v_angle );// so we know which way we are facing

	while ((pObject = UTIL_FindEntityInSphere( pObject, pev->origin, PLAYER_SEARCH_RADIUS )) != NULL)
	{

		char *pClassname = (char *)STRING(pObject->pev->classname);
		if (FStrEq(pClassname, "building_radio"))
		{

			vecLOS = ((pObject->pev->absmin + ( pObject->pev->size * 0.5 )) - (pev->origin + pev->view_ofs));
			vecLOS = UTIL_ClampVectorToBox( vecLOS, pObject->pev->size * 0.5 );
			
			flDot = DotProduct (vecLOS , gpGlobals->v_forward);
			if (flDot > flMaxDot )
			{
				pClosest = pObject;
				flMaxDot = flDot;
			}
		}
	}
	pObject = pClosest;

	// Found an object
	if (pObject )
	{
		
		if (m_afButtonPressed & IN_USE) 
		{
			char *pClassname = (char *)STRING(pObject->pev->classname);

			if (FStrEq(pClassname, "building_radio"))
			{
				// It's a radio. Do the RADIO USE thingummy, letting the radio decide what use does.

				RadioUse( pObject->edict(), pEntity );

			}
		}
	}

}


void RadioTouch( edict_t *pEntity, edict_t *pTouch )
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
			//ClientPrint( pev, HUD_PRINTTALK, "* Radio removed because you were stuck in it!\n");
		}
	}
}

