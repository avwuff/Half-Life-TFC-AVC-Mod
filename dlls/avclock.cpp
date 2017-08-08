
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
#include <time.h>

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

// This CPP deals with the CLOCK

extern bool AdminLoggedIn[33];

float nextLightTime = 0;


bool ClockCreate( edict_t *pEntity, int mode )
{
	
	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0) return 0;

	// Create the radio and stick to the wall
	entvars_t *pPev = VARS( pEntity );

	if (pPev->iuser1 > 0) return 0;

	UTIL_MakeVectors( pPev->v_angle + pPev->punchangle );
	Vector vecSrc	 = GetGunPosition( pEntity );
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecOri = pPev->origin + vecAiming * 48;

	KeyValueData	kvd;
	
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	entvars_t *pRunOnPev;
	pRunOnPev =  VARS(tEntity);
	
	char buf[80];
	sprintf( buf, "%s", "building_clock");
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = buf;

	DispatchKeyValue( tEntity, &kvd );

	// place this in front

	pRunOnPev->origin = vecOri;
	pRunOnPev->angles.y = (pPev->v_angle.y) + 180;
	
	SET_MODEL( ENT( pRunOnPev ) , "avatar-x/avadd24.avfil");
	UTIL_SetSize( pRunOnPev, Vector( -10, -10 ,0), Vector(10, 10, 80));
	
	pRunOnPev->takedamage = DAMAGE_NO;
	pRunOnPev->health = 40;
	pRunOnPev->euser4 = pEntity;
	
	SET_ORIGIN( tEntity , vecOri );
	DROP_TO_FLOOR( tEntity );
	
	if (mode == 1) 
	{
		pRunOnPev->maxspeed = 400;
		pRunOnPev->skin = 50;
		SetBits( pRunOnPev->flags, FL_FLOAT );
	}


	// Spawn routine
	tEntity->v.solid = SOLID_BBOX;
	pRunOnPev->movetype = MOVETYPE_PUSHSTEP;

	pRunOnPev->nextthink = gpGlobals->time + 0.1;

	SET_ORIGIN( tEntity , pRunOnPev->origin );
	return 1;

}

void ClockPrecache()
{

	
	PRECACHE_MODEL("avatar-x/avadd24.avfil");
	PRECACHE_SOUND("avatar-x/avadd25.avfil");
	PRECACHE_SOUND("avatar-x/avadd26.avfil");
	PRECACHE_SOUND("avatar-x/avadd27.avfil");
	
}

void SpeakTime(void)
{

	struct tm *tmTime;
	time_t tTime;
	
	tTime=time(NULL);
	tmTime=localtime(&tTime);

	int hr = tmTime->tm_hour;
	int min = tmTime->tm_min ;

	char hourname[20];
	char minutename[40];

	if (hr > 12) hr = hr - 12;
	if (hr == 0) hr = 12;


	if (hr == 1) sprintf(hourname, "one");
	if (hr == 2) sprintf(hourname, "two");
	if (hr == 3) sprintf(hourname, "three");
	if (hr == 4) sprintf(hourname, "four");
	if (hr == 5) sprintf(hourname, "five");
	if (hr == 6) sprintf(hourname, "six");
	if (hr == 7) sprintf(hourname, "seven");
	if (hr == 8) sprintf(hourname, "eight");
	if (hr == 9) sprintf(hourname, "nine");
	if (hr == 10) sprintf(hourname, "ten");
	if (hr == 11) sprintf(hourname, "eleven");
	if (hr == 12) sprintf(hourname, "twelve");

	bool addnum = 0;
	int mind = min;

	if (min < 10)
	{
		sprintf(minutename, "(e25) ok (e400)");
		mind = min;
		addnum = 1;
	}
	
	if (min == 10) sprintf(minutename, "ten");
	if (min == 11) sprintf(minutename, "eleven");
	if (min == 12) sprintf(minutename, "twelve");
	if (min == 13) sprintf(minutename, "thirteen");
	if (min == 14) sprintf(minutename, "fourteen");
	if (min == 15) sprintf(minutename, "fifteen");
	if (min == 16) sprintf(minutename, "sixteen");
	if (min == 17) sprintf(minutename, "seventeen");
	if (min == 18) sprintf(minutename, "eighteen");
	if (min == 19) sprintf(minutename, "nineteen");
	
	if (min == 20) sprintf(minutename, "twenty");
	if (min == 30) sprintf(minutename, "thirty");
	if (min == 40) sprintf(minutename, "fourty");
	if (min == 50) sprintf(minutename, "fifty");

	if (min > 20 && min < 30)
	{
		sprintf(minutename, "twenty");
		mind = min - 20;
		addnum = 1;
	}
	if (min > 30 && min < 40)
	{
		sprintf(minutename, "thirty");
		mind = min - 30;
		addnum = 1;
	}
	if (min > 40 && min < 50)
	{
		sprintf(minutename, "fourty");
		mind = min - 40;
		addnum = 1;
	}
	if (min > 50 && min < 60)
	{
		sprintf(minutename, "fifty");
		mind = min - 50;
		addnum = 1;
	}

	if (addnum)
	{
		if (mind == 0) sprintf(minutename, "%s (e25) ok (e400)", minutename);
		if (mind == 1) sprintf(minutename, "%s one", minutename);
		if (mind == 2) sprintf(minutename, "%s two", minutename);
		if (mind == 3) sprintf(minutename, "%s three", minutename);
		if (mind == 4) sprintf(minutename, "%s four", minutename);
		if (mind == 5) sprintf(minutename, "%s five", minutename);
		if (mind == 6) sprintf(minutename, "%s six", minutename);
		if (mind == 7) sprintf(minutename, "%s seven", minutename);
		if (mind == 8) sprintf(minutename, "%s eight", minutename);
		if (mind == 9) sprintf(minutename, "%s nine", minutename);
	}
	
	// finally, speak the time

	char totaltalk[200];
	sprintf(totaltalk, "speak \"attention _comma the time is now _comma %s %s _comma _comma _comma e (s30) yes (s0) (e60) team (e400)\"\n", hourname, minutename);
	
	//ALERT(at_console, totaltalk);


	int i;
	edict_t *pPlayerEdict;
	
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pPlayerEdict = INDEXENT( i );
		if ( pPlayerEdict && !pPlayerEdict->free )
		{
			// Run clientcommand.

			char *pClassname = AvGetClassname ( pPlayerEdict );
			if (FStrEq(pClassname, "player")) {
				
				
				CLIENT_COMMAND( pPlayerEdict, totaltalk);
			}
		}
	}	

}

void ClockThink ( edict_t *pent )
{
	
	if ( pent->v.fuser2 < gpGlobals->time  && pent->v.iuser4 == 1)
	{

		// make a light baby

		if (pent->v.rendercolor.x == 0 && pent->v.rendercolor.y == 0 && pent->v.rendercolor.z == 0)
		{
			pent->v.rendercolor.x = 255;
			pent->v.rendercolor.y = 255;
			pent->v.rendercolor.z = 255;
		}

		if (pent->v.fuser3 == 0) pent->v.fuser3 = 40;

		pent->v.fuser2 = gpGlobals->time + 20;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_DLIGHT );

			WRITE_COORD( pent->v.origin.x );
			WRITE_COORD( pent->v.origin.y );
			WRITE_COORD( pent->v.origin.z );

			WRITE_BYTE( (int)pent->v.fuser3 ); //radius

			// colour
			WRITE_BYTE( pent->v.rendercolor.x ); 
			WRITE_BYTE( pent->v.rendercolor.y ); 
			WRITE_BYTE( pent->v.rendercolor.z ); 


			//WRITE_BYTE( 200 ); // brightness
			WRITE_BYTE( 255 );  // life
			WRITE_BYTE( 0 );  // decay
		MESSAGE_END();



	}

	
	pent->v.nextthink = gpGlobals->time + 2;

	// every 2 seconds we make a TICK
	// every 2 seconds after those 2 we make a TOCK

	if (pent->v.iuser1 == 0)
	{
		EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd25.avfil", 1.0, ATTN_NORM , 0, 100);
		pent->v.iuser1 = 1;
	}
	else if (pent->v.iuser1 == 1) {
		EMIT_SOUND_DYN2( pent, CHAN_VOICE, "avatar-x/avadd26.avfil", 1.0, ATTN_NORM , 0, 100);
		pent->v.iuser1 = 0;
	}

	if (pent->v.iuser3 > 0)
	{
		pent->v.iuser3--;
		EMIT_SOUND_DYN2( pent, CHAN_WEAPON, "avatar-x/avadd27.avfil", 1.0, ATTN_NORM , 0, 100);
	}

	// Show the time on the clock
	struct tm *tmTime;
	time_t tTime;
	
	tTime=time(NULL);
	tmTime=localtime(&tTime);


	float a = 0;

	int hr = tmTime->tm_hour;
	int min = tmTime->tm_min ;
	int sec = tmTime->tm_sec ;

	if (hr > 12) hr = hr - 12;
	if (hr == 0) hr = 12;
		
	a = ((float)255 / (float)12) * hr;
		

	// also add the distance the MINUTE hand has moved..
	float extra = (((float)min) / (float)60);
	extra = extra * ((float)255 / (float)12);
	a = a + extra;

	pent->v.controller[0] = (int)a;
		
	float b = 0;
	float c = 0;
	c = ((float)255 / (float)60);
	b = (c * (float)min);

	// also take into account where the second hand WOULD be...
	extra = (float)sec / (float)60;
	extra = (float)extra * ((float)255 / (float)60);
	b = b + (float)extra;
	
	pent->v.controller[1] = (int)b;

	// See if its time to chime
	
	if (pent->v.iuser2 != min)
	{
		
		if (min == 15 || min == 30 || min == 45)
		{
			// Single chime
			EMIT_SOUND_DYN2( pent, CHAN_WEAPON, "avatar-x/avadd27.avfil", 1.0, ATTN_NORM , 0, 100);
		}
		if (min == 0)
		{
			// Chime the number of times of the hour
			pent->v.iuser3 = hr - 1;
			EMIT_SOUND_DYN2( pent, CHAN_WEAPON, "avatar-x/avadd27.avfil", 1.0, ATTN_NORM , 0, 100);
		}
	}

	// store the minute hand position
	pent->v.iuser2 = min;
}

void ClockTouch(edict_t *pent, edict_t *pentTouch)
{

	entvars_t  *pevToucher = VARS(pentTouch);

	if ( FStrEq((char*)STRING(pevToucher->classname), "worldspawn")) return;

	entvars_t  *pev = VARS(pent);
	int playerTouch = 0;

	// Is entity standing on this pushable ?
	if ( FBitSet(pevToucher->flags,FL_ONGROUND) && pevToucher->groundentity && VARS(pevToucher->groundentity) == pev )
	{
		// Only push if floating
		if ( pev->waterlevel > 0 )
			pev->velocity.z += pevToucher->velocity.z * 0.1;

		return;
	}

	if ( FStrEq((char*)STRING(pevToucher->classname), "player"))
	{
	//	if ( !(pevToucher->button & (IN_FORWARD|IN_USE)) )	// Don't push unless the player is pushing forward and NOT use (pull)
			//return;
		playerTouch = 1;
	}

	float factor;

	if ( playerTouch )
	{
		if ( !(pevToucher->flags & FL_ONGROUND) )	// Don't push away from jumping/falling players unless in water
		{
			if ( pev->waterlevel < 1 )
				return;
			else 
				factor = 0.1;
		}
		else
			factor = 1;
	}
	else 
		factor = 0.25;

	pev->velocity.x += pevToucher->velocity.x * factor;
	pev->velocity.y += pevToucher->velocity.y * factor;

	float length = sqrt( pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y );
	if ((length > pev->maxspeed))
	{
		pev->velocity.x = (pev->velocity.x * pev->maxspeed / length );
		pev->velocity.y = (pev->velocity.y * pev->maxspeed / length );
	}
	if ( playerTouch )
	{
		pevToucher->velocity.x = pev->velocity.x;
		pevToucher->velocity.y = pev->velocity.y;

		
		/*
		if ( (gpGlobals->time - m_soundTime) > 0.7 )
		{
			m_soundTime = gpGlobals->time;
			if ( length > 0 && FBitSet(pev->flags,FL_ONGROUND) )
			{
				m_lastSound = RANDOM_LONG(0,2);
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound], 0.5, ATTN_NORM);
	//			SetThink( StopSound );
	//			pev->nextthink = pev->ltime + 0.1;
			}
			else
				STOP_SOUND( ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound] );
		}
		*/
	}

}
