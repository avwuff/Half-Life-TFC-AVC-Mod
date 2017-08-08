//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// dll.cpp
//


#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "engine.h"

#include "studio.h"
#include "bot.h"
#include "custmotd.h"
#include "drivecheck.h"

#include <time.h>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// BEGIN ELEVATOR DATA
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



struct elevator_s // elevator data
{	
	int direction;
	Vector buttonofs[32];
	Vector dooropenpos;
	Vector doorclosedpos;
	int currfloor;
	bool moving;


	int buttonindex[32];
	int updownindex[3];
	int doorindex;
	int movewithindex[32];
	int floorindindex; // floor indicator
};
typedef elevator_s elevator_t;

struct floor_s // floor data
{	
	
	bool calledup;
	bool calleddown;
	bool elethere[8];
	Vector stoploc[8];
	int callupindex;
	int calldownindex;
	int doorindex[8];
	Vector doorclosedpos[8];
	Vector dooropenpos[8];
};
typedef floor_s floor_t;

int numfloors = 0;
int numelevs = 0;
bool firstthink = 0;






// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

char callee[256];

int lastSeenBy[33];


//#include "waypoint.h"
#ifdef GRAPPLE
	#include "avgrapple.h"
#endif

#ifdef CAMERA
	#include "buildcam.h"
#endif


#include "sadll.h"

#ifdef AVC
	#include "avdll.h"
	//#include "billdll.h"

	extern origentdata_t EntData[1025];
	extern phrases_t PhraseList[21];
	extern playerinfo_t PlayerInfo[33];

#endif

commandlog_t mCommandLog[10];
int mCommandLogPos = 0;

#ifdef SZ_DLL

	

	#include "avwep.h"
	#include "avpara.h"
	#include "avtripmine.h"
	#include "avslot.h"
	#include "avclock.h"
	#include "avradio.h"
	#include "avsnow.h"
	#include "avflare.h"
	#include "avtractor.h"
	#include "avbfg.h"
	#include "avpaper.h"
	#include "avdance.h"
	#include "avfountain.h"
	#include "avleash.h"
	#include "avfox.h"
	#include "avsign.h"
	#include "bd.h"
	#include "avremote.h"
	#include "avrace.h"

#ifdef RECORD

	#include "avrecord.h"

#endif
	extern signtext_t texts[1025];

	#include "avmon1.h"

#endif

//#include "gamerules.h"

extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
 //bool AdminMems[33][6][1025];		// ent memory

extern int InMemNum[1025];					// which highlight to show
extern bool AdminLoggedIn[33];

extern bool shutup[33];

signtext_t playerModels[33];

int gmsgStatusText = 0;
int m_gibModel = 0;

bool DLLON = 1;
bool breakingnow = 0;

DLL_FUNCTIONS other_gFunctionTable;
DLL_GLOBAL const Vector g_vecZero = Vector(0,0,0);

int mod_id = 0;
bool mapspawn = 0;

int sentOutCrashlog = 0;

extern bool AdminDettingSoon[33];
float detsoontime = 0;

bool g_GameRules = FALSE;

static FILE *fp;
static FILE *fp2;

fpos_t pos;   

#ifdef CRASHLOOK
	static FILE *debug0;
	static FILE *debug1;
	static FILE *debug2;
	static FILE *debug3;
	static FILE *debug4;
	static FILE *debug5;
	static FILE *debug6;
	static FILE *debug7;
	static FILE *debug8;
	static FILE *debug9;

	static FILE *debug_at;

	static FILE *cmlg;
#endif


edict_t *BuildEdict = NULL;
edict_t *JustMadeEdict = NULL;

//cvar_t sv_bot = {"bot",""};

#ifdef SZ_DLL
	cvar_t sz_moosound 		= {"sz_moosound","1"};
	cvar_t sa_allowradio	= {"sa_allowradio","1"};
	cvar_t sa_radiospread	= {"sa_radiospread","700"};
	cvar_t sa_reserveslots	= {"sa_reserveslots","5"};
	cvar_t sa_allowtripmine	= {"sa_allowtripmine","1"};
	cvar_t sa_onlysnow		= {"sa_onlysnow","0"};
	cvar_t sa_talkicon		= {"sa_talkicon","1"};
	cvar_t sa_allowsnow		= {"sa_allowsnow","1"};
	cvar_t sa_allowpaper		= {"sa_allowpaper","1"};
	
	cvar_t sa_avc			= {"sa_avc","1"};
	cvar_t sa_nodll			= {"sa_nodll","0"};
	
	cvar_t sa_switchtalk	= {"sa_switchtalk","0"};
	cvar_t sa_switchtalkavc	= {"sa_switchtalkavc","0"};
	cvar_t sa_allowsign		= {"sa_allowsign","1"};
	cvar_t sa_premusic		= {"sa_premusic","0"};

	cvar_t sa_remotes		= {"sa_remotes","1"};
	cvar_t sa_pullremote	= {"sa_pullremote","1"};
	cvar_t sa_lookremote	= {"sa_lookremote","1"};
	cvar_t sa_camremote		= {"sa_camremote","1"};

	cvar_t fox_branded		= {"fox_branded","1"};

#endif

#ifdef GRAPPLE
	cvar_t sa_allowgrapple	= {"sa_allowgrapple","1"};
#endif
#ifdef CAMERA
	cvar_t sa_allowcamera	= {"sa_allowcamera","1"};
#endif

#ifdef DEBUGLOGFILE
	int rnd = 0;
	char fle[30];
#endif


int m_say = 0;

extern int		mazewidth[8];
extern int		mazeheight[8];
extern bool		mazespawned;
extern int		mazedefault[8];
extern int		mazenumpiece[8];
extern int		mazerootx[8];
extern int		mazerooty[8];
extern int		mazerootz[8];
extern int		mazerootent[8];
extern int		mazepieceset[8];
extern int		totalmaze;

int giPrecacheGrunt = 0;
int gmsgShake = 0;
int gmsgFade = 0;
int gmsgSelAmmo = 0;
int gmsgFlashlight = 0;
int gmsgFlashBattery = 0;
int gmsgResetHUD = 0;
int gmsgInitHUD = 0;	
int gmsgShowGameTitle = 0;
int gmsgCurWeapon = 0;
int gmsgHealth = 0;
int gmsgDamage = 0;
int gmsgBattery = 0;
int gmsgTrain = 0;
int gmsgLogo = 0;
int gmsgWeaponList = 0;
int gmsgAmmoX = 0;
int gmsgHudText = 0;
int gmsgDeathMsg = 0;
int gmsgScoreInfo = 0;
int gmsgTeamInfo = 0;
int gmsgTeamScore = 0;
int gmsgGameMode = 0;
int gmsgMOTD = 0;
int gmsgAmmoPickup = 0;
int gmsgWeapPickup = 0;
int gmsgItemPickup = 0;
int gmsgHideWeapon = 0;
int gmsgSetCurWeap = 0;
int gmsgSayText = 0;
int gmsgTextMsg = 0;
int gmsgShowMenu = 0;
int gmsgGeigerRange = 0;
int gmsgSetFOV = 0;


int		tempmazewidth;
int		tempmazeheight;
int		tempmazedefault;
int		tempmazenumpiece;
int		tempmazerootent;
int		tempmazepieceset;

char data[800];
//char longchar[800];

#ifdef CRASHLOOK
void addCommandToLog(const char *cmd, const char *args, const char *player)
{
	
	// Move to the next position.

	mCommandLogPos++;
	if (mCommandLogPos > 9) mCommandLogPos = 0;

	// Mark this into the arrays

	strcpy(mCommandLog[mCommandLogPos].playername, player);
	sprintf(mCommandLog[mCommandLogPos].value, "%s %s", cmd, args);

	mCommandLog[mCommandLogPos].runtime = gpGlobals->time;
	
	// Now output to a file


	pos = 0;
	fsetpos( cmlg, &pos );

	int i = mCommandLogPos;
	if (i > 9) i = 0;	

	for (int n = 0; n < 10; n++)
	{
		i++;
		if (i > 9) i = 0;	
		fprintf(cmlg, "[%f] by %s: %sé", mCommandLog[i].runtime, mCommandLog[i].playername , mCommandLog[i].value );
		
	} 

	fprintf(cmlg, "\n");

}
#endif


char *UTIL_StringSearchReplace(const char *Search, const char *Replace, const char *Input)
{
	static char Output[8192], Temp[8192];
	char *ptr;
	sprintf(Output, "%s", Input);
	while(ptr = strstr(Output, Search))
	{
		*ptr = '\0';
    	sprintf(Temp, "%s%s%s", Output, Replace, ptr + strlen(Search));
		sprintf(Output, "%s", Temp);
	}
	return Output;
}


void fixSGS()
{
	// scan for SG's

	edict_t *frontEnt;

	for (int i = 1; i < 1025; i++) 
	{

		frontEnt = INDEXENT ( i );
		if (frontEnt) 
		{
			if (FStrEq( STRING(frontEnt->v.classname), "building_sentrygun"))
			{
				// take a look at it's vital stats.

				CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(frontEnt);
				pEntity->MyOwnThink = pEntity->m_pfnThink;
			}
		}
	}

}
#ifdef CRASHLOOK
void AvAddDebug( char* szFmt, ...)
{
	static first;
	static int debugnum;

	if (sentOutCrashlog == 0) return;

	if (first != 1) {
		debug0 = fopen("DEBUG0.txt","w");
		debug1 = fopen("DEBUG1.txt","w");
		debug2 = fopen("DEBUG2.txt","w");
		debug3 = fopen("DEBUG3.txt","w");
		debug4 = fopen("DEBUG4.txt","w");
		debug5 = fopen("DEBUG5.txt","w");
		debug6 = fopen("DEBUG6.txt","w");
		debug7 = fopen("DEBUG7.txt","w");
		debug8 = fopen("DEBUG8.txt","w");
		debug9 = fopen("DEBUG9.txt","w");
		
		debug_at = fopen("DEBUG_AT.txt","w");

		cmlg   = fopen("cm_log.txt","w");
		first = 1;
		debugnum = 0;
	}
	pos = 0;

	FILE *pointer;

	
	if (debugnum == 0) pointer = debug0;
	if (debugnum == 1) pointer = debug1;
	if (debugnum == 2) pointer = debug2;
	if (debugnum == 3) pointer = debug3;
	if (debugnum == 4) pointer = debug4;
	if (debugnum == 5) pointer = debug5;
	if (debugnum == 6) pointer = debug6;
	if (debugnum == 7) pointer = debug7;
	if (debugnum == 8) pointer = debug8;
	if (debugnum == 9) pointer = debug9;

	fsetpos( pointer, &pos );

	va_list			argptr;
	static char		string[2048];
	
	va_start ( argptr, szFmt );
	vsprintf ( string, szFmt, argptr );
	va_end   ( argptr );

	fprintf(pointer, "[%f] Map: %s -- %s", gpGlobals->time, STRING(gpGlobals->mapname), string);

	fsetpos(debug_at, &pos);
	fprintf(debug_at, "Last Logged Line Was: %i\n", debugnum);
	
	debugnum++;
	if (debugnum > 9) debugnum = 0;

}
#endif

void AvLinkUserMessages()
{
	if ( gmsgCurWeapon )
	{
		return;
	}

	return;

	//gmsgSelAmmo = REG_USER_MSG("SelAmmo", sizeof(SelAmmo));
	gmsgCurWeapon = REG_USER_MSG("CurWeapon", 3);
	//gmsgGeigerRange = REG_USER_MSG("Geiger", 1);
	//gmsgFlashlight = REG_USER_MSG("Flashlight", 2);
	//gmsgFlashBattery = REG_USER_MSG("FlashBat", 1);
	gmsgHealth = REG_USER_MSG( "Health", 1 );
	gmsgDamage = REG_USER_MSG( "Damage", 12 );
	//gmsgBattery = REG_USER_MSG( "Battery", 2);
	//gmsgTrain = REG_USER_MSG( "Train", 1);
	gmsgHudText = REG_USER_MSG( "HudText", -1 );
	gmsgSayText = REG_USER_MSG( "SayText", -1 );
	gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 );
	gmsgWeaponList = REG_USER_MSG("WeaponList", -1);
	//gmsgResetHUD = REG_USER_MSG("ResetHUD", 1);		// called every respawn
	//gmsgInitHUD = REG_USER_MSG("InitHUD", 0 );		// called every time a new player joins the server
	//gmsgShowGameTitle = REG_USER_MSG("GameTitle", 1);
	gmsgDeathMsg = REG_USER_MSG( "DeathMsg", -1 );
	gmsgScoreInfo = REG_USER_MSG( "ScoreInfo", 5 );
	//gmsgTeamInfo = REG_USER_MSG( "TeamInfo", -1 );  // sets the name of a player's team
	//gmsgTeamScore = REG_USER_MSG( "TeamScore", -1 );  // sets the score of a team on the scoreboard
	//gmsgGameMode = REG_USER_MSG( "GameMode", 1 );
	gmsgMOTD = REG_USER_MSG( "MOTD", -1 );
	//gmsgAmmoPickup = REG_USER_MSG( "AmmoPickup", 2 );
	gmsgWeapPickup = REG_USER_MSG( "WeapPickup", 1 );
	//gmsgItemPickup = REG_USER_MSG( "ItemPickup", -1 );
	gmsgHideWeapon = REG_USER_MSG( "HideWeapon", 1 );
	gmsgSetFOV = REG_USER_MSG( "SetFOV", 1 );
	gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
	//gmsgShake = REG_USER_MSG("ScreenShake", sizeof(ScreenShake));
	//gmsgFade = REG_USER_MSG("ScreenFade", sizeof(ScreenFade));
	//gmsgAmmoX = REG_USER_MSG("AmmoX", 2);
}


void AddLogEntry(char *entry)
{

	#ifdef DEBUGLOGFILE
		// add a log entry
		if (rnd == 0) 
		{
			rnd = RANDOM_LONG(0, 10000);
			//rnd =3;
			sprintf(fle, "debug%i.log", rnd);
			fp=fopen("logs.lst","a");
			fprintf(fp, "Recent file: %s\n", fle);
			fclose(fp);	
		}
		
		fp=fopen(fle,"w");
		fprintf(fp, "%s", entry);
		fclose(fp);	
	#endif
}

#ifdef CRASHLOOK
int getDebugEntryFromDebugFile(char *filen, char *out)
{

    fp = fopen(filen,"r");

	if (fp == NULL) 
	{
		sprintf(out, "Could not open!");
		return 0;
	}
	else
	{
		//int test;
		//test = fscanf(fp, "%s\n", &logData );
		fgets( out, 499, fp );            
		fclose(fp);	

		int enterpress = 0;
		// find the enter press
		for (int i = 0; i < 499; i++)
		{
			if (out[i] == 13)
			{	
				enterpress = i;
			}
		}
		
		if(enterpress > 0) out[enterpress] = 0;
	}
	return 1;
}
#endif


void GameDLLInit( void )
{
	
#ifdef CRASHLOOK

	struct tm *tmTime;
	time_t tTime;
	
	tTime=time(NULL);
	tmTime=localtime(&tTime);

	char debugEntry0[500];

	char commandLog[10000];

	char finalEntry[10000];

	strcat(finalEntry, "0: ");
	getDebugEntryFromDebugFile("DEBUG0.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);

	strcat(finalEntry, "1: ");
	getDebugEntryFromDebugFile("DEBUG1.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);

	strcat(finalEntry, "2: ");
	getDebugEntryFromDebugFile("DEBUG2.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "3: ");
	getDebugEntryFromDebugFile("DEBUG3.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "4: ");
	getDebugEntryFromDebugFile("DEBUG4.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "5: ");
	getDebugEntryFromDebugFile("DEBUG5.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "6: ");
	getDebugEntryFromDebugFile("DEBUG6.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "7: ");
	getDebugEntryFromDebugFile("DEBUG7.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "8: ");
	getDebugEntryFromDebugFile("DEBUG8.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "9: ");
	getDebugEntryFromDebugFile("DEBUG9.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);
	
	strcat(finalEntry, "\nLastLine: ");
	getDebugEntryFromDebugFile("DEBUG_AT.TXT", debugEntry0);
	strcat(finalEntry, debugEntry0);


	getDebugEntryFromDebugFile("cm_log.txt", commandLog);
	
	for (int j = 0; j < (int)strlen(commandLog); j++)
	{
		if (commandLog[j] == 'é') commandLog[j] = 13;
	}
	 
	fp=fopen("crashes.log","a");
	fprintf(fp, "------------------\nOccured At: %i:%i:%i %i/%i/%i  -- Last five entries, check timestamp for time:\n%s\nLast Commands:\n%s\n", (int)tmTime->tm_hour, (int)tmTime->tm_min, (int)tmTime->tm_sec, (int)tmTime->tm_mday, (int)tmTime->tm_mon, (int)tmTime->tm_year , finalEntry, commandLog);
	fclose(fp);	

	sentOutCrashlog = 1;

	// check drive serial
	//ALERT(at_console, "****** CHECKING SERIAL ******\n");
	bool yesno = CheckSerial();


	if (!yesno)
	{
		char quitstring[10];
		sprintf(quitstring, "q");
		sprintf(quitstring, "%su", quitstring);
		sprintf(quitstring, "%si", quitstring);
		sprintf(quitstring, "%st", quitstring);
		sprintf(quitstring, "%s\n", quitstring);

		SERVER_COMMAND(quitstring);
	}
	//ALERT(at_console, "******   DONE SERIAL   ******\n");

#endif



   //CVAR_REGISTER (&sv_bot);
	//fp=fopen("DEBUG124.txt","w");
	#ifdef GRAPPLE
		CVAR_REGISTER (&sa_allowgrapple);
	#endif
	#ifdef CAMERA
		CVAR_REGISTER (&sa_allowcamera);
	#endif
	
	
	#ifdef SZ_DLL
		CVAR_REGISTER (&sa_radiospread);
		CVAR_REGISTER (&sz_moosound);
		CVAR_REGISTER (&sa_allowradio);
		CVAR_REGISTER (&sa_reserveslots);
		CVAR_REGISTER (&sa_allowtripmine);
		CVAR_REGISTER (&sa_onlysnow);
		CVAR_REGISTER (&sa_talkicon);
		CVAR_REGISTER (&sa_allowsnow);
		CVAR_REGISTER (&sa_allowpaper);

		CVAR_REGISTER (&sa_avc);
		CVAR_REGISTER (&sa_nodll);

		CVAR_REGISTER (&sa_switchtalk);
		CVAR_REGISTER (&sa_switchtalkavc);
		CVAR_REGISTER (&sa_allowsign);
		CVAR_REGISTER (&sa_premusic);
		CVAR_REGISTER (&sa_remotes);
		CVAR_REGISTER (&sa_camremote);
		CVAR_REGISTER (&fox_branded);
		CVAR_REGISTER (&sa_lookremote);
		CVAR_REGISTER (&sa_pullremote);
	
		//DEBUGOUTCOMMENT 
		BDGameDLLInit();
		
		pfnAddServerCommand( "saytime", SpeakTime );

	#endif	

	AvGameInit(); // actually SAGAMEINIT

	// force unmodified files
	//FORCE_TYPE type, float *mins, float *maxs, const char *filename

	
	// see if we need to log a debug.txt
	


	
   (*other_gFunctionTable.pfnGameInit)();
   
}


int DispatchSpawn( edict_t *pent )
{
	

	#ifdef CRASHLOOK
		fprintf(fp, "DispatchSpawn called on a %s\n", STRING(pent->v.classname));
	#endif

	#ifdef SZ_DLL

		// set the original origin
		
		KeyValueData kvd;
		char buf1[40];
		char buf2[40];
		
		sprintf(buf1, "%s", STRING(pent->v.classname));
		sprintf(buf2, "%f %f %f", pent->v.origin.x, pent->v.origin.y, pent->v.origin.z);

		kvd.fHandled = FALSE;
		kvd.szClassName = buf1;
		kvd.szKeyName = "origin";
		kvd.szValue = buf2;

		EntData[ENTINDEX(pent)].addingkey = 1;

		DispatchKeyValue( pent, &kvd );

		//DEBUGOUTCOMMENT 
		BDDispatchSpawn( pent );

		EntData[ENTINDEX(pent)].hasorigin = 0;

		if (pent->v.origin.x != 0 || pent->v.origin.y != 0 || pent->v.origin.z != 0)
		{
			// has an origin
			// mark it off
			EntData[ENTINDEX(pent)].hasorigin = 1;
		}

	#endif

	char *pClassname = (char *)STRING(pent->v.classname);
	int IamNum = ENTINDEX(pent);
	
	#ifdef AVC
		EntData[IamNum].isnotspawned = 0;
	#endif
	//	ChessSpawn( pent );	
	mapspawn = 0;

	if (strcmp(pClassname, "worldspawn") == 0)
	{
		// do level initialization stuff here...
		mapspawn = 1;
		firstthink = 0;
		SaPrecache();
		mazespawned = 0;

		ClearKeys();

		if (CVAR_GET_FLOAT("sa_nodll") != 1) {
			DLLON = 1;
		}
		else
		{
			DLLON = 0;
		}


		#ifdef GRAPPLE
		
			if (DLLON) 
				GrapPrecache();
			

		#endif

		#ifdef AVC
			AvRunPrecache();	 
		#endif

		#ifdef SZ_DLL
		
			if (DLLON) {
				 
				 ParaPrecache();
				 RadioPrecache();
				 TripPrecache();
				 ClockPrecache();
				 SnowPrecache();
				 FlarePrecache();
				 TracPrecache();
				 BFGPrecache();
				 PaperPrecache();
				 DancePrecache();
				 FountainPrecache();
				 LeashBoxPrecache();
				 FoxPrecache();
				 SignPrecache();
			}

			

			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop2.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop3.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop4.wav");
			//E/NGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop5.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop6.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop7.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop8.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop9.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop10.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop11.wav");
			//ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "sound/avatar-x/radioloop12.wav");
		#endif

		#ifdef CAMERA
			if (DLLON) 
				CamPrecache();
		#endif

		m_gibModel = PRECACHE_MODEL("models/hgibs.mdl");

		#ifdef SZ_DLL
		if (DLLON) 
			m_say = PRECACHE_MODEL("avatar-x/avadd38.avfil");
		#endif
		g_GameRules = TRUE;
	}
	  
    // Special After-Spawn routine for airtank

	int somethine = 0;

	
	somethine = (*other_gFunctionTable.pfnSpawn)(pent);
	if (mapspawn)
	{
//		ENGINE_FORCE_UNMODIFIED( force_exactfile, NULL, NULL, "models/player/cf/cf.mdl");
	}

	#ifdef CRASHLOOK
		AvAddDebug( "After DispatchSpawn\n");
	#endif
	
	
	return somethine;
}


void DispatchThink( edict_t *pent )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "DispatchThink called on a %s (modelindex: %i, model: %s, targetname: %s)\n", 
			STRING(pent->v.classname), 
			pent->v.modelindex, 
			STRING(pent->v.model), 
			STRING(pent->v.targetname));
	#endif

	#ifdef SZ_DLL

		if (firstthink == 0)
		{
			// search for ents and mark those that have origin brushes
			firstthink = 1;
		}
	#endif

	#ifdef CRASHLOOK
	//	AvAddDebug( "DispatchThink called on a %s\n", STRING(pent->v.classname));
	#endif

	mapspawn = 0;
	if (DLLON) 
	{

		#ifdef SZ_DLL
		if (mazespawned != 1)
		{
			mazespawned = 1;
			if (mazewidth[1] > 0)
			{
				// Create the maze here.
				AvSpawnMazeParts();
			}
		}


			AvThink( pent );	
		#endif

		char *pClassname = (char *)STRING(pent->v.classname);

	#ifdef GRAPPLE
		if (FStrEq( pClassname, "grapple"))
		{	
			// this is a grapple, pass it to my Think function not this shit
			GrapThink( pent );
		}
	#endif
	#ifdef CAMERA
		else if (FStrEq( pClassname, "building_camera"))
		{	
			// camera think function
			CamThink( pent );
		}
	#endif
	#ifdef SZ_DLL
		else if (FStrEq( pClassname, "building_radio"))
		{	
			// radio think function
			RadioThink( pent );
		}
		else if (FStrEq( pClassname, "monster_snark"))
		{	
			// SNARK think function
			FlareThink( pent );
		}
		else if (FStrEq( pClassname, "av_remote"))
		{	
			// radio think function
			RemoteThink( pent );
		}
		else if (FStrEq( pClassname, "building_flypaper"))
		{	
			// radio think function
			PaperThink( pent );
		}
		else if (FStrEq( pClassname, "building_clock"))
		{	
			// radio think function
			ClockThink( pent );
		}
		else if (FStrEq( pClassname, "building_fountain"))
		{	
			// radio think function
			FountainThink( pent );
		}		
		else if (FStrEq( pClassname, "building_leashbox"))
		{	
			// radio think function
			LeashBoxThink( pent );
		}		
		else if (FStrEq( pClassname, "av_pet"))
		{	
			// radio think function
			FoxThink( pent );
		}		
		else if (FStrEq( pClassname, "building_dancemachine"))
		{	
			// radio think function
			DanceThink( pent );
		}		

		else if (FStrEq( pClassname, "monster_tripmine"))
		{	
			// radio think function
			TripThink( pent );
		}
		else if (FStrEq( pClassname, "race_camera"))
		{	
			// radio think function
			RaceCamThink( pent );
		}
		else if (FStrEq( pClassname, "building_sign"))
		{	
			// radio think function
			SignThink( pent );
		}
		else if (FStrEq( pClassname, "item_avparachute"))
		{	
			ParaThink( pent );
		}
		else if (FStrEq( pClassname, "bfg_shot"))
		{	
			BFGThink( pent );
		}
		else if (FStrEq( pClassname, "func_breakable"))
		{	
			if (pent->v.solid == SOLID_NOT)
			{
				
			}
			else
			{
				(*other_gFunctionTable.pfnThink)(pent);
			}
		}
	#endif
		else
		{
			(*other_gFunctionTable.pfnThink)(pent);
		}
	}
	else
	{
		(*other_gFunctionTable.pfnThink)(pent);
	}

	

	#ifdef CRASHLOOK
		AvAddDebug( "After DispatchThink!\n");
	#endif
	
}

void DispatchUse( edict_t *pentUsed, edict_t *pentOther )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "DispatchUse called on a %s\n", STRING(pentUsed->v.classname));
	#endif

	char *pClassname = (char *)STRING(pentUsed->v.classname);
	if (FStrEq( pClassname, "building_radio")) 
	{

		RadioUse(pentUsed, pentOther);

	}
	else
	{
		(*other_gFunctionTable.pfnUse)(pentUsed, pentOther);
	}
	
	#ifdef CRASHLOOK
		AvAddDebug( "After DispatchUse\n");
	#endif

}

void DispatchTouch( edict_t *pentTouched, edict_t *pentOther )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "DispatchTouch called on a %s (modelindex: %i, model: %s, targetname: %s) by %s (modelindex: %i, model: %s, targetname: %s) \n", 
			STRING(pentTouched->v.classname), 
			pentTouched->v.modelindex, 
			STRING(pentTouched->v.model), 
			STRING(pentTouched->v.targetname), 
			
			STRING(pentOther->v.classname), 
			pentOther->v.modelindex, 
			STRING(pentOther->v.model), 
			STRING(pentOther->v.targetname));
	#endif

	if (DLLON) 
	{		
		char *pClassname = (char *)STRING(pentTouched->v.classname);
		char *pClassname2 = (char *)STRING(pentOther->v.classname);

		#ifdef AVC
		   AvTouch( pentTouched, pentOther );
		#endif

		#ifdef CRASHLOOK
			AvAddDebug( "Touch Marker: 1 - AvTouch Completed Sucessfully, GrapTouch is Next\n");
		#endif

		#ifdef GRAPPLE
			if (FStrEq( pClassname, "grapple"))
			{	
				// this is a grapple, pass it to my Touch function not this shit
				GrapTouch( pentTouched, pentOther );
			}
		#endif

		#ifdef CRASHLOOK
			AvAddDebug( "Touch Marker: 1 - GrapTouch was Good, CamTouch Next\n");
		#endif

		#ifdef CAMERA
			if (FStrEq( pClassname, "building_camera")) // Avatar's Special Camera!
			{	
				CamTouch( pentTouched, pentOther);
			}
		#endif

		#ifdef CRASHLOOK
			AvAddDebug( "Touch Marker: 1 - CamTouch was Good, SnowTouch Next\n");
		#endif


		#ifdef SZ_DLL
			if (FStrEq( pClassname, "snowball"))
			{	
				// this is a grapple, pass it to my Touch function not this shit
				SnowBallTouch( pentTouched, pentOther );
			}
			else if (FStrEq( pClassname, "monster_snark"))
			{	
				// this is a grapple, pass it to my Touch function not this shit
				FlareTouch( pentTouched, pentOther );
			}
			else if (FStrEq( pClassname, "av_remote"))
			{	
				// this is a grapple, pass it to my Touch function not this shit
				RemoteTouch( pentTouched, pentOther );
			}
			else if (FStrEq( pClassname, "building_radio")) // Avatar's Special Camera!
			{	
				RadioTouch(pentTouched, pentOther);
			}

			else if (FStrEq( pClassname, "building_flypaper")) // Avatar's Special Camera!
			{	
				PaperTouch( pentTouched, pentOther );

			}	
			else if (FStrEq( pClassname, "building_sign")) // Avatar's Special Camera!
			{	
				SignTouch( pentTouched, pentOther );
			}	
			else if (FStrEq( pClassname, "item_avparachute"))
			{	
				if (pentTouched->v.iuser1 == 1) ParaTouch( pentTouched, pentOther );
			}
			else if (FStrEq( pClassname, "building_clock"))
			{	
				ClockTouch(pentTouched, pentOther);
			}
			else if (FStrEq( pClassname, "building_fountain"))
			{	
				FountainTouch(pentTouched, pentOther);
			}
			else if (FStrEq( pClassname, "building_leashbox"))
			{	
				LeashBoxTouch(pentTouched, pentOther);
			}
			else if (FStrEq( pClassname, "race_camtrigger"))
			{	
				CamTriggerTouch(pentTouched, pentOther);
			}
			else if (FStrEq( pClassname, "av_pet"))
			{	
				FoxTouch(pentTouched, pentOther);
			}
			else if (FStrEq( pClassname2, "av_pet"))
			{	
				
				FoxTouch2(pentTouched, pentOther);
			}
			else if (FStrEq( pClassname, "bfg_shot"))
			{	
				BFGTouch(pentTouched, pentOther);
			}


		#endif
		else
		{
		#ifdef CRASHLOOK
			AvAddDebug( "Touch Marker 2, sending it to TFC.DLL\n");
		#endif

			(*other_gFunctionTable.pfnTouch)(pentTouched, pentOther);
		}
	}
	else
	{
		(*other_gFunctionTable.pfnTouch)(pentTouched, pentOther);
	}
	
	
	#ifdef CRASHLOOK
		AvAddDebug( "Touch completed sucessfully!\n");
	#endif
}

void DispatchBlocked( edict_t *pentBlocked, edict_t *pentOther )
{
   
	#ifdef CRASHLOOK
		AvAddDebug( "DispatchBlocked Called on a %s by %s\n", STRING(pentBlocked->v.classname), STRING(pentOther->v.classname));
	#endif

	// make sure it isnt a tripmine or camera that is causing this block.
	char *pClassname = (char*)STRING(pentOther->v.classname);

	if (FStrEq(pClassname, "building_camera"))
	{
		// get rid of it
		pentOther->v.health = 1;
	}
	else if (FStrEq(pClassname, "monster_tripmine"))
	{
		// get rid of it
		pentOther->v.health = 1;
	}
	else if (FStrEq(pClassname, "building_radio"))
	{
		// get rid of it
		pentOther->v.health = 1;
	}
	else if (FStrEq(pClassname, "building_sign"))
	{
		// get rid of it
		pentOther->v.flags |= FL_KILLME;
	}
	else if (FStrEq(pClassname, "monster_snark"))
	{
		// get rid of it
		
	}
	else
	{
		(*other_gFunctionTable.pfnBlocked)(pentBlocked, pentOther);
	}

	#ifdef CRASHLOOK
		AvAddDebug( "After DispatchBlocked!\n");
	#endif


}

void DispatchKeyValue( edict_t *pentKeyvalue, KeyValueData *pkvd )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "DispatchKeyValue called on a %s: %s = %s\n", pkvd->szClassName, pkvd->szKeyName, pkvd->szValue );
	#endif

	#ifdef SZ_DLL

		int mynum = ENTINDEX( pentKeyvalue );
		// put this into the EntData
			
		// clear if over
		if (EntData[mynum].isnotspawned == 0 && EntData[mynum].addingkey == 0)
		{
			// clear
			EntData[mynum].numkeys = 0;
			EntData[mynum].isnotspawned = 1;
		}
		
		EntData[mynum].addingkey = 0;
		// place data here.
		static char temp[3000];

		if (EntData[mynum].numkeys < 128)
		{
			
			// make sure it isnt one we dont want
			if (!FStrEq(pkvd->szKeyName, "classname") && !FStrEq(pkvd->szKeyName, "model"))
			{
				// now make sure it isnt ALREADY in the list
				int inlist = 0;

				for (int i = 1; i <= EntData[mynum].numkeys; i++)
				{
					if (FStrEq( Av_GetKeyName(EntData[mynum].keyindex[i]) , pkvd->szKeyName)) inlist = EntData[mynum].keyindex[i];
				}
				
				if (inlist > 0)
				{
					// is in the list
					SetKeyValue(inlist, pkvd->szValue );
				
				}
				else
				{
					// make a new one
					EntData[mynum].numkeys++;

					EntData[mynum].keyindex[EntData[mynum].numkeys] = AddEntKey( pkvd->szKeyName , pkvd->szValue );
				}
			}

			//ALERT(at_console, "ADDING DATA %i to id %i with value %s\n", mynum, n, temp);

		}
		// end put


		int mark = 0;


		if (FStrEq(pkvd->szKeyName, "digitgroup") && FStrEq(pkvd->szClassName, "info_target"))
		{
			// this item is part of a digit group

			if (pentKeyvalue->v.vuser4.z <= 0) pentKeyvalue->v.vuser4.z = 1;
			pentKeyvalue->v.vuser4.x = 250; // initial value reset
			sprintf(EntData[ENTINDEX(pentKeyvalue)].digitgroup, "%s", pkvd->szValue);
			
			pentKeyvalue->v.nextthink = gpGlobals->time + 0.1;

		}
		else if (FStrEq(pkvd->szKeyName, "numdigits") && FStrEq(pkvd->szClassName, "info_target"))
		{
			// this item is part of a digit group

			if (pentKeyvalue->v.vuser4.z <= 0) pentKeyvalue->v.vuser4.z = 1; // enabled
			pentKeyvalue->v.vuser4.x = 250; // initial value reset
			pentKeyvalue->v.vuser4.y = atoi(pkvd->szValue); // number of digits
			
			pentKeyvalue->v.nextthink = gpGlobals->time + 0.1;

		}
		else if (FStrEq(pkvd->szKeyName, "alphadisplay") && FStrEq(pkvd->szClassName, "info_target"))
		{
			// this item is part of a digit group

			pentKeyvalue->v.vuser4.z = 2; // enabled
			pentKeyvalue->v.vuser4.x = 250; // initial value reset
			
			pentKeyvalue->v.nextthink = gpGlobals->time + 0.1;

		}
		else if (FStrEq(pkvd->szKeyName, "scrollspeed") && FStrEq(pkvd->szClassName, "info_target"))
		{
			// this item is part of a digit group

			pentKeyvalue->v.fuser3 = atof(pkvd->szValue);

		}

		else if (atoi(pkvd->szKeyName) > 0 && FStrEq(pkvd->szClassName, "info_target"))
		{

			int phrasenum = atoi(pkvd->szKeyName);

			// add this phrase to the list
			sprintf(PhraseList[phrasenum].phrase, "%s", pkvd->szValue);
		}

		else if (FStrEq(pkvd->szKeyName, "width") && FStrEq(pkvd->szClassName, "info_target"))
		{
			tempmazewidth = atoi(pkvd->szValue);
			
			if (pentKeyvalue->v.iuser2 > 0)
				mazewidth[pentKeyvalue->v.iuser2] = tempmazewidth;

//			ALERT( at_console, "Got the MazeWidth %i\n", tempmazewidth);
		}
		else if (FStrEq(pkvd->szKeyName, "height") && FStrEq(pkvd->szClassName, "info_target"))
		{
			tempmazeheight = atoi(pkvd->szValue);
			
			if (pentKeyvalue->v.iuser2 > 0)
				mazeheight[pentKeyvalue->v.iuser2] = tempmazeheight;
			
//			ALERT( at_console, "Got the MazeHeight %i\n", tempmazeheight);
		}
		else if (FStrEq(pkvd->szKeyName, "default") && FStrEq(pkvd->szClassName, "info_target"))
		{
			tempmazedefault = atoi(pkvd->szValue);
			
			if (pentKeyvalue->v.iuser2 > 0)
				mazedefault[pentKeyvalue->v.iuser2] = tempmazedefault;
			
//			ALERT( at_console, "Got the MazeDefault %i\n", tempmazedefault);
		}
		else if (FStrEq(pkvd->szKeyName, "numpiece") && FStrEq(pkvd->szClassName, "info_target"))
		{
			tempmazenumpiece = atoi(pkvd->szValue);

			if (pentKeyvalue->v.iuser2 > 0)
				mazenumpiece[pentKeyvalue->v.iuser2] = tempmazenumpiece;
		
//			ALERT( at_console, "Got the MazeNumPiece %i\n", tempmazenumpiece);
		}
		else if (FStrEq(pkvd->szKeyName, "pieceset") && FStrEq(pkvd->szClassName, "info_target"))
		{
			tempmazepieceset = atoi(pkvd->szValue);

			if (pentKeyvalue->v.iuser2 > 0)
				mazepieceset[pentKeyvalue->v.iuser2] = tempmazepieceset;

//			ALERT( at_console, "Got the MazePieceSet %i\n", tempmazepieceset);
		}
		else if (FStrEq(pkvd->szKeyName, "mazeid") && FStrEq(pkvd->szClassName, "info_target"))
		{
			
			pentKeyvalue->v.iuser2 = atoi(pkvd->szValue);

			if (tempmazepieceset > 0)	mazepieceset	[pentKeyvalue->v.iuser2] = tempmazepieceset;
			if (tempmazenumpiece > 0)	mazenumpiece	[pentKeyvalue->v.iuser2] = tempmazenumpiece;
			if (tempmazewidth > 0)		mazewidth		[pentKeyvalue->v.iuser2] = tempmazewidth;
			if (tempmazeheight > 0)		mazeheight		[pentKeyvalue->v.iuser2] = tempmazeheight;
			if (tempmazedefault > 0)	mazedefault		[pentKeyvalue->v.iuser2] = tempmazedefault;

			mazerootent[pentKeyvalue->v.iuser2] = ENTINDEX( pentKeyvalue );
			
			if (pentKeyvalue->v.iuser2 > totalmaze) totalmaze = pentKeyvalue->v.iuser2 ;

//			ALERT( at_console, "Got the MazeID %i\n", pentKeyvalue->v.iuser2);


		}

	#endif

	(*other_gFunctionTable.pfnKeyValue)(pentKeyvalue, pkvd);


	#ifdef CRASHLOOK
		AvAddDebug( "DispatchKeyValue completed successfully!" );
	#endif

}

void DispatchSave( edict_t *pent, SAVERESTOREDATA *pSaveData )
{
	
   (*other_gFunctionTable.pfnSave)(pent, pSaveData);
}

int DispatchRestore( edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity )
{
   return (*other_gFunctionTable.pfnRestore)(pent, pSaveData, globalEntity);
}

void DispatchObjectCollisionBox( edict_t *pent )
{
   (*other_gFunctionTable.pfnSetAbsBox)(pent);
}

void SaveWriteFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
{
   (*other_gFunctionTable.pfnSaveWriteFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveReadFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
{
   (*other_gFunctionTable.pfnSaveReadFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveGlobalState( SAVERESTOREDATA *pSaveData )
{
   (*other_gFunctionTable.pfnSaveGlobalState)(pSaveData);
}

void RestoreGlobalState( SAVERESTOREDATA *pSaveData )
{
   (*other_gFunctionTable.pfnRestoreGlobalState)(pSaveData);
}

void ResetGlobalState( void )
{
   (*other_gFunctionTable.pfnResetGlobalState)();
}

BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ]  )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "ClientConnect: %s\n", STRING(pEntity->v.netname) );
	#endif

	//pEntity->v.iuser4 = 0;

	PlayerInfo[ENTINDEX(pEntity)].numPoints = -13;

	SaConnect ( pEntity );

	if (DLLON)
	{

		/*
		#ifdef SZ_DLL
			bool something = (bool)AvClientConnect(pEntity, pszName, pszAddress, szRejectReason);
			if (something == 0) return 0;
		#endif
		*/
	
		if (strlen(pszName) > 32)
		{
			sprintf( szRejectReason, "Sorry! Your name is too long to play here!");
			#ifdef CRASHLOOK
				AvAddDebug( "After ClientConnect\n");
			#endif
			return 0;
		}
	}

	BOOL retval = (*other_gFunctionTable.pfnClientConnect)(pEntity, pszName, pszAddress, szRejectReason);


	#ifdef CRASHLOOK
		AvAddDebug( "After ClientConnect\n");
	#endif

	return retval;
}

void ClientDisconnect( edict_t *pEntity )
{
	#ifdef CRASHLOOK
		AvAddDebug( "ClientDisconnect\n");
	#endif

	#ifdef SZ_DLL
		//DEBUGOUTCOMMENT 
		BDClientDisconnect( pEntity );
	#endif
	//	char entry[80];
	//	sprintf( entry, "ClientDisconnect: Name: %s\n", (char *)STRING(pEntity->v.netname));
	// AddLogEntry( entry );
	if (DLLON)
	{
		#ifdef CAMERA
			CamHandleMenuItem( pEntity, "3", 1 );
		#endif
	}

	(*other_gFunctionTable.pfnClientDisconnect)(pEntity);
	
	#ifdef CRASHLOOK
		AvAddDebug( "After ClientDisconnect\n");
	#endif

}

void ClientKill( edict_t *pEntity )
{
	#ifdef CRASHLOOK
		AvAddDebug( "ClientKill\n");
	#endif
   (*other_gFunctionTable.pfnClientKill)(pEntity);
	#ifdef CRASHLOOK
		AvAddDebug( "After ClientKill\n");
	#endif
}

void ClientPutInServer( edict_t *pEntity )
{
	#ifdef CRASHLOOK
		AvAddDebug( "ClientPutInServer: %s\n", STRING(pEntity->v.netname) );
	#endif

	#ifdef AVC
		AvPutInServer( pEntity );
	#endif

	#ifdef SZ_DLL
		//DEBUGOUTCOMMENT 
		BDClientPutInServer( pEntity );
		//ParaJoinGame( pEntity );
	#endif
	
	pEntity->v.vuser3.x = 0;
	pEntity->v.vuser3.y = 0;

	SaPutInServer( pEntity );
	(*other_gFunctionTable.pfnClientPutInServer)(pEntity);

	#ifdef CRASHLOOK
		AvAddDebug( "After ClientPutInServer\n");
	#endif

}

void ClientCommand( edict_t *pEntity )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "ClientCommand: %s used %s %s\n", STRING(pEntity->v.netname), Cmd_Argv(0), Cmd_Args() );

	fp=fopen("overall.log","a");
	fprintf(fp, "[%f] ClientCommand: %s used %s %s\n", gpGlobals->time, STRING(pEntity->v.netname), Cmd_Argv(0), Cmd_Args());
	fclose(fp);	


	addCommandToLog(Cmd_Argv(0), Cmd_Args(), STRING(pEntity->v.netname));
	#endif	

	const char *pcmd = Cmd_Argv(0);
	const char *arg1 = Cmd_Argv(1);
	const char *arg2 = Cmd_Argv(2);
	const char *arg3 = Cmd_Argv(3);
	const char *arg4 = Cmd_Argv(4);
	const char *arg5 = Cmd_Argv(5);
	const char *arg6 = Cmd_Argv(6);
	const char *arg7 = Cmd_Argv(7);
	const char *arg8 = Cmd_Argv(8);
	const char *arg9 = Cmd_Argv(9);

	
	#ifdef SZ_DLL
		//DEBUGOUTCOMMENT 
		BDClientCommand( pEntity , pcmd, arg1, arg2, arg3, arg4 );
		#ifdef RECORD
			EvClientCommand( pEntity );
		#endif
	#endif

	if (FStrEq(pcmd, "build") && FStrEq(arg1, "1")) {
		BuildEdict = pEntity;
	}

	if (DLLON)
	{

		#ifdef GRAPPLE
			if (FStrEq(pcmd, "+grapple" ))
			{
				
				if (pEntity->v.vuser4.y > 0) return;
				if (pEntity->v.deadflag == DEAD_NO && pEntity->v.movetype == MOVETYPE_WALK && pEntity->v.euser4 == NULL)
				{
					// create a grapple for this player
					GrapCreateBolt( pEntity );
				}
			}
			else if (FStrEq(pcmd, "-grapple" ))
			{
				// trash grapple for this player
				if (pEntity->v.euser4) {
					GrapRelease( pEntity->v.euser4 );
				}
			}
		#endif
			else if (FStrEq(pcmd, "menuselect")) {
				CamHandleMenuItem( pEntity, arg1 , 0);
				RadioHandleMenuItem( pEntity, arg1 );
			}
		#ifdef CAMERA
			else if (FStrEq(pcmd, "cammenu")) {
				if (CVAR_GET_FLOAT("sa_allowcamera") != 1) return;
				CamShowMenu( pEntity );
			}
			// cam commands
			else if (FStrEq(pcmd, "camcreate")) {
				if (CVAR_GET_FLOAT("sa_allowcamera") != 1) return;
				CamHandleMenuItem( pEntity, "1", 1 );
			}
			else if (FStrEq(pcmd, "camdet")) {
				CamHandleMenuItem( pEntity, "3", 1 );
			}
			else if (FStrEq(pcmd, "+camlook")) {
				if (CVAR_GET_FLOAT("sa_allowcamera") != 1) return;
				CamHandleMenuItem( pEntity, "4", 1 );
			}
			else if (FStrEq(pcmd, "-camlook")) {
				CamHandleMenuItem( pEntity, "5", 1 );
			}
		#endif
		#ifdef SZ_DLL
			else if (FStrEq(pcmd, "+foxlook")) {
				FoxSee( pEntity );
			}
			else if (FStrEq(pcmd, "privchat")) {
				PrivateChat( pEntity );
			}
			else if (FStrEq(pcmd, "-foxlook")) {
				FoxStopSee( pEntity );
			}
			else if (FStrEq(pcmd, "tracundo")) {
				TracUndo( pEntity );
			}
			
			else if (FStrEq(pcmd, "foxcontrol")) {
				FoxStartControl( pEntity );
			}
			else if (FStrEq(pcmd, "nofox")) {
				FoxStopControl( pEntity );
			}
			


			else if (FStrEq(pcmd, "radio")) {
				if (CVAR_GET_FLOAT("sa_allowradio") != 1 && AdminLoggedIn[ENTINDEX(pEntity)] != 1) return;
				//if (pEntity->v.iuser4 < 0) return;
				RadioCreate(pEntity);
			}

			
			else if (FStrEq(pcmd, "camremote")) {
				if (CVAR_GET_FLOAT("sa_remotes") != 1) return;
				if (CVAR_GET_FLOAT("sa_camremote") != 1) return;
				
				RemoteCreate(pEntity, 1);
			}
			else if (FStrEq(pcmd, "lookremote")) {
				if (CVAR_GET_FLOAT("sa_remotes") != 1) return;
				if (CVAR_GET_FLOAT("sa_lookremote") != 1) return;
				RemoteCreate(pEntity, 2);
			}
			else if (FStrEq(pcmd, "pullremote")) {
				if (CVAR_GET_FLOAT("sa_remotes") != 1) return;
				if (CVAR_GET_FLOAT("sa_pullremote") != 1) return;
				RemoteCreate(pEntity, 3);
			}

			else if (FStrEq(pcmd, "sign")) {
				if (CVAR_GET_FLOAT("sa_allowsign") != 1) return;
				//if (pEntity->v.iuser4 < 0) return;
				SignCreate(pEntity);
			}	
			else if (FStrEq(pcmd, "snark")) {
				FlareAttackFly(pEntity);
			}
			
			else if (FStrEq(pcmd, "signrem")) {
				if (CVAR_GET_FLOAT("sa_allowsign") != 1) return;
				//if (pEntity->v.iuser4 < 0) return;
				SignRemove(pEntity);
			}			
			else if (FStrEq(pcmd, "signbusy")) {
				if (CVAR_GET_FLOAT("sa_allowsign") != 1) return;
				//if (pEntity->v.iuser4 < 0) return;
				SignCreate(pEntity);
			}			
			else if (FStrEq(pcmd, "signavsays")) {
				if (CVAR_GET_FLOAT("sa_allowsign") != 1) return;
				//if (pEntity->v.iuser4 < 0) return;
				SignCreate(pEntity);
			}			
			else if (FStrEq(pcmd, "dance")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				
				
				DanceCreate(pEntity);
			}
			else if (FStrEq(pcmd, "camtrigger")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				CamTriggerCreate(pEntity);
			}
			else if (FStrEq(pcmd, "racecam")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				RaceCamCreate(pEntity);
			}
			else if (FStrEq(pcmd, "dancering")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				
				DanceCreateRing(pEntity, atoi(arg1));
			}
			else if (FStrEq(pcmd, "paper")) {
				if (CVAR_GET_FLOAT("sa_allowpaper") != 1) return;
				//if (pEntity->v.iuser4 < 5000) return;
				PaperCreate(pEntity);
			}
			else if (FStrEq(pcmd, "clock")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				ClockCreate(pEntity, atoi(arg1));
			}
			else if (FStrEq(pcmd, "fountain")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				FountainCreate(pEntity);
			}
			else if (FStrEq(pcmd, "leash")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				LeashBoxCreate(pEntity);
			}

			else if (FStrEq(pcmd, "fox")) {
				//if (CVAR_GET_FLOAT("sa_allowradio") != 1) return;
				FoxCreate(pEntity);
			}
			
			else if (FStrEq(pcmd, "wepmod")) {
				entvars_t *pev = VARS( pEntity );
				pev->weapons |= (1<<25);
				int gmsgWeapPickup = REG_USER_MSG( "WeapPickup", 1 );

				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pEntity );
					WRITE_BYTE( 25 );
				MESSAGE_END();
			}
			else if (FStrEq(pcmd, "wepmod2")) {
				entvars_t *pev = VARS( pEntity );
				pev->weapons |= (1<<24);
				int gmsgWeapPickup = REG_USER_MSG( "WeapPickup", 1 );

				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pEntity );
					WRITE_BYTE( 24 );
				MESSAGE_END();
			}
			
			else if (FStrEq(pcmd, "goaxe")) {
			
				entvars_t *pev = VARS( pEntity );


				MESSAGE_BEGIN( MSG_ONE, 65, NULL, pEntity );
					WRITE_BYTE( 1 );
					WRITE_BYTE( 5 );
					WRITE_BYTE( -1 );
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_ONE, 65, NULL, pEntity );
					WRITE_BYTE( 0 );
					WRITE_BYTE( 10 );
					WRITE_BYTE( -1 );
				MESSAGE_END();
			}
			
			else if (FStrEq(pcmd, "deploypara"))
			{
				ParaEnable( pEntity );
			}
			else if (FStrEq(pcmd, "jointeam"))
			{
				ParaJoinGame( pEntity );	
			}


			if (FStrEq(pcmd, "feign") &&  pEntity->v.euser4 != NULL) return;
			if (FStrEq(pcmd, "sfeign") && pEntity->v.euser4 != NULL) return;
			

			TracHandleCommand( pEntity, pcmd, arg1, arg2 );

		#endif
	}
	if (DLLON)
	{
		#ifdef SZ_DLL
			bool doit1 = WepCommand (pEntity, pcmd, arg1, arg2, arg3, arg4, arg5, arg6);
			if (doit1 == 0) return;
			
		#endif
		#ifdef AVC
			AvCommand (pEntity, pcmd, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	
			if (FStrEq(pcmd, "say"))
			{

				if (CVAR_GET_FLOAT("sa_talkicon") == 1)
				{

					MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
						WRITE_BYTE( TE_PLAYERATTACHMENT );
						WRITE_BYTE( ENTINDEX(pEntity) );
						WRITE_COORD( 0 );

						WRITE_SHORT( m_say );
										
						WRITE_SHORT( 40 );

					MESSAGE_END();
				}

				char fullsay[256];
				char newstring[256];

				sprintf(fullsay, "%s", CMD_ARGS());
				
				//ALERT(at_console, "He said: %s\n", fullsay);

				if (strlen(fullsay) > 5)
				{
					// CHECK FOR /me
					if (fullsay[1] == '/' && fullsay[2] == 'm' && fullsay[3] == 'e')
					{

						if (shutup[ENTINDEX(pEntity)]) return;
						
						int iLastQuote = 0;

						for (int i = 0; i < (int)strlen(fullsay); i++) // look for the last  quote in string
						{
							if (fullsay[i] == '"')
							{
								iLastQuote = i;
							}
						}

						fullsay[iLastQuote] = 0; // remove the last quote from string

						// See what happens if we try to cut off the beginning?

						//sprintf( text, "%c%s: ", 2, STRING( pEntity->v.netname ) );

						sprintf(newstring, "%c* %s ", 2, (char*)STRING(pEntity->v.netname));

						int prevlen = strlen(newstring);
						for (i = 5; i < (int)strlen(fullsay); i++)
						{
							strcat(newstring, "a");
						}

						for (i = 5; i < (int)strlen(fullsay); i++)
						{
							newstring[prevlen + i - 5] = fullsay[i];
						}

						while (strstr(newstring, "%h")) // search and replace %h with player's health
						{
							char health[10];
							char *pString;
							int playerhealth = pEntity->v.health;
							sprintf(health, "%i", playerhealth);
							pString = UTIL_StringSearchReplace("%h", health, newstring);
							sprintf(newstring, "%s", pString);
						}

						while (strstr(newstring, "%a")) // search and replace %a with player's armor
						{
							char armorvalue[10];
							char *pString;
							int playerarmorvalue = pEntity->v.armorvalue;
							sprintf(armorvalue, "%i", playerarmorvalue);
							pString = UTIL_StringSearchReplace("%a", armorvalue, newstring);
							sprintf(newstring, "%s", pString);
						}

						//============================================================
						//-------------------> ATTENTION AVATAR-X <-------------------
						//============================================================
						// Check the code's comments below

						while (strstr(newstring, "%i")) // search and replace %i with player's last seen player
						{
							char *pString;
							edict_t *pLast;
							char name2[64];

							sprintf(name2, "");

							pLast = INDEXENT(lastSeenBy[ENTINDEX(pEntity)]);
							
							if (pLast && !FNullEnt(pLast))
							{
								sprintf(name2, "%s", STRING(pLast->v.netname));
							}


							pString = UTIL_StringSearchReplace("%i", name2, newstring);

							// lastseenplayer is what you're supposed to store each
							// Players' last seen player obtained from a traceline

							sprintf(newstring, "%s", pString);
						}

						strcat(newstring, "\n");

						MESSAGE_BEGIN( MSG_ALL, gmsgSayText );
							WRITE_BYTE( ENTINDEX(pEntity) );
							WRITE_STRING( newstring );
						MESSAGE_END();


						char team[15];
						char temp[256];


						AvGetTeam( pEntity, team );
						// L 07/06/2002 - 00:41:19: "Pixletz]m3[-]C3K[-<6><404049><KNUCKLES>" say "im w33"
						
						sprintf(temp, "\"%s<%i><%i><%s>\" say \"%s\"\n",
							(char *)STRING( pEntity->v.netname), (int)GETPLAYERUSERID(  pEntity ),
							(int)pfnGetPlayerWONId (  pEntity ), team, newstring);

						UTIL_LogPrintf( temp );

						
						return;
						
					}
					else
					{
						if (fullsay[0] == '/' && fullsay[1] == 'm' && fullsay[2] == 'e') ALERT(at_console, "ME MODE IS TRUE!\n");
					}
				}

			}

		#endif

	
	}




	bool doit = SaCommand (pEntity, pcmd, arg1, arg2, arg3, arg4, arg5, arg6);
	if (doit == 1)
	{
		(*other_gFunctionTable.pfnClientCommand)(pEntity);

		if (DLLON)
		{
			#ifdef SZ_DLL
				
			
				
			#endif
		}
	}

	

	#ifdef CRASHLOOK
		AvAddDebug( "After ClientCommand\n");
	#endif
}

void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "ClientUserInfoChanged\n");
	#endif

	g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity) , infobuffer, "model", playerModels[ENTINDEX(pEntity)].text );
	
	bool doinfo =  SaClientUserInfoChanged(pEntity, infobuffer);
	if (doinfo) (*other_gFunctionTable.pfnClientUserInfoChanged)(pEntity, infobuffer);
   
	

	#ifdef CRASHLOOK
		AvAddDebug( "After ClientUserInfoChanged\n");
	#endif

}

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{

	#ifdef CRASHLOOK
		AvAddDebug( "ServerActivate\n");
	#endif

	fp=fopen("overall.log","a");
	fprintf(fp, "\n\n\n\n\n\n\n\n[%f] Starting up!\n", gpGlobals->time);
	fclose(fp);	
		
		(*other_gFunctionTable.pfnServerActivate)(pEdictList, edictCount, clientMax);

	#ifdef CRASHLOOK
		AvAddDebug( "After ServerActivate\n");
	#endif
}

void ServerDeactivate( void )
{
	#ifdef CRASHLOOK
		AvAddDebug( "Server Deactivate\n");
	#endif
	(*other_gFunctionTable.pfnServerDeactivate)();
	#ifdef CRASHLOOK
		AvAddDebug( "After ServerDeactivate\n");
	#endif
}

void PlayerPreThink( edict_t *pEntity )
{
	
	// SA's function
	#ifdef CRASHLOOK
		AvAddDebug( "PrePlayerThink on %s\n", STRING(pEntity->v.netname));
	#endif

	//ALERT(at_console, "AddPack: %i ents, last ent %i, prev total %i\n", (int)pEntity->v.vuser2.y, (int)pEntity->v.vuser2.x, (int)pEntity->v.vuser2.z);

	AvPrePlayerThink( pEntity );
	
	if (DLLON)
	{

		#ifdef AVC
			AvPrePlayerThink2( pEntity );
		#endif

		#ifdef CRASHLOOK
			AvAddDebug( "Player Prethink Marker 1\n");
		#endif


		#ifdef SZ_DLL
			// make sure the class model is correct
			#ifdef CRASHLOOK
			//	AvAddDebug( "BDPrePlayerThink on %s\n", STRING(pEntity->v.netname));
			#endif			

			//DEBUGOUTCOMMENT 
			BDPlayerPreThink( pEntity );
			
			#ifdef CRASHLOOK
			//	AvAddDebug( "GameDLLPrePlayerThink on %s\n", STRING(pEntity->v.netname));
			#endif			
		#endif
	}

	//WepPlayerThink( pEntity );


	#ifdef CRASHLOOK
		AvAddDebug( "Player Prethink Marker 2\n");
	#endif


	if (SaPostPlayerThink(pEntity)) (*other_gFunctionTable.pfnPlayerPreThink)(pEntity);

	

	#ifdef CRASHLOOK
		AvAddDebug( "After PlayerPreThink\n");
	#endif

}

void PlayerPostThink( edict_t *pEntity )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "PlayerPostThink\n");
	#endif	
	if (DLLON)
	{

		#ifdef SZ_DLL
			#ifdef CRASHLOOK
			//	AvAddDebug( "PostPlayerThink on %s\n", STRING(pEntity->v.netname));
			#endif
			WepPlayerThink1( pEntity );
			RadioUseCheck( pEntity );
		#endif
	}
	(*other_gFunctionTable.pfnPlayerPostThink)(pEntity);

	#ifdef CRASHLOOK
		AvAddDebug( "After PlayerPostThink\n");
	#endif
	

}

void StartFrame( void )
{
	
	#ifdef CRASHLOOK
		AvAddDebug( "StartFrame called!\n");
	#endif

	#ifdef RECORD
		PlaybackFrame();
	#endif

	if (breakingnow)
	{
		edict_t *ed;

		for (int i = 1; i < 1024; i++)
		{
			ed = INDEXENT(i);

			if (ed && !FNullEnt(ed)) 
			{
				if (FStrEq(STRING(ed->v.classname), "func_breakable"))
				{
					ed->v.flags &= ~FL_KILLME;
					if (ed->v.solid == SOLID_NOT) ed->v.effects |= EF_NODRAW;

				}
			}
		}
		breakingnow = 0;
	}

	if (DLLON)
	{
		#ifdef AVC
			AvStartFrame();
		#endif
	}
	#ifdef CRASHLOOK
		AvAddDebug( "After AvStartFrame!\n");
	#endif
	SaStartFrame();

	(*other_gFunctionTable.pfnStartFrame)();

	#ifdef CRASHLOOK
		AvAddDebug( "After StartFrame\n");
	#endif

	
}

void ParmsNewLevel( void )
{
	#ifdef CRASHLOOK
		AvAddDebug( "ParmsNewLevel\n");
	#endif
   (*other_gFunctionTable.pfnParmsNewLevel)();
	#ifdef CRASHLOOK
		AvAddDebug( "After ParmsNewLevel\n");
	#endif
}

void ParmsChangeLevel( void )
{
	#ifdef CRASHLOOK
		AvAddDebug( "ParmsChangeLevel\n");
	#endif
   (*other_gFunctionTable.pfnParmsChangeLevel)();
	#ifdef CRASHLOOK
		AvAddDebug( "After ParmsChangeLevel\n");
	#endif
}

const char *GetGameDescription( void )
{
	#ifdef CRASHLOOK
		AvAddDebug( "GetGameDescription\n");
	#endif


	const char *keyc = (*other_gFunctionTable.pfnGetGameDescription)();

	#ifdef CRASHLOOK
		AvAddDebug( "After getGameDescription\n");
	#endif

	return keyc;
}

void PlayerCustomization( edict_t *pEntity, customization_t *pCust )
{

//	fp2=fopen("cust.txt","a");
	//fprintf(fp2, "Cust from %s: %s\n", STRING(pEntity->v.netname), 	pCust->resource.szFileName);
	//fclose(fp2);	

	#ifdef CRASHLOOK
		AvAddDebug( "PlayerCustomization\n");
	#endif

	(*other_gFunctionTable.pfnPlayerCustomization)(pEntity, pCust);
	#ifdef CRASHLOOK
		AvAddDebug( "After PlayerCustomization\n");
	#endif

}

void SpectatorConnect( edict_t *pEntity )
{
	#ifdef CRASHLOOK
		AvAddDebug( "Spectator Connect\n");
	#endif

	(*other_gFunctionTable.pfnSpectatorConnect)(pEntity);
	#ifdef CRASHLOOK
		AvAddDebug( "After spectatorconnect\n");
	#endif

}

void SpectatorDisconnect( edict_t *pEntity )
{
	#ifdef CRASHLOOK
		AvAddDebug( "spectatordisconnect\n");
	#endif
   
	(*other_gFunctionTable.pfnSpectatorDisconnect)(pEntity);
	#ifdef CRASHLOOK
		AvAddDebug( "after spectatordisconnect\n");
	#endif
}

void SpectatorThink( edict_t *pEntity )
{
	#ifdef CRASHLOOK
		AvAddDebug( "spectatorthink\n");
	#endif
   (*other_gFunctionTable.pfnSpectatorThink)(pEntity);
	#ifdef CRASHLOOK
		AvAddDebug( "after spectatorthink\n");
	#endif
}

void Sys_Error( const char *error_string )
{

	#ifdef CRASHLOOK
		AvAddDebug( "!!!! SYS_ERROR: %s\n", error_string);
	#endif

	//char temp[256];

	//sprintf(temp, "\"HLDS PROCESS<0><0><None>\" say \"SYS ERROR: %s\"\n",
	//	error_string);

	//UTIL_LogPrintf( temp );

	(*other_gFunctionTable.pfnSys_Error)(error_string);
}

void PM_Move ( struct playermove_s *ppmove, int server )
{
	#ifdef CRASHLOOK
		AvAddDebug( "pm_move\n");
	#endif
   (*other_gFunctionTable.pfnPM_Move)(ppmove, server);
	#ifdef CRASHLOOK
		AvAddDebug( "after pm_move\n");
	#endif

}

void PM_Init ( struct playermove_s *ppmove )
{
	#ifdef CRASHLOOK
		AvAddDebug( "pm_init\n");
	#endif
   (*other_gFunctionTable.pfnPM_Init)(ppmove);
	#ifdef CRASHLOOK
		AvAddDebug( "after pm_init\n");
	#endif
}

char PM_FindTextureType( char *name )
{
	#ifdef CRASHLOOK
		AvAddDebug( "pm_find texture type\n");
	#endif

	return (*other_gFunctionTable.pfnPM_FindTextureType)(name);
	#ifdef CRASHLOOK
		AvAddDebug( "after pm_find texture type\n");
	#endif
}

void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas )
{
	#ifdef CRASHLOOK
		AvAddDebug( "SetupVisibility\n");
	#endif

	(*other_gFunctionTable.pfnSetupVisibility)(pViewEntity, pClient, pvs, pas);
	#ifdef CRASHLOOK
		AvAddDebug( "After SetupVisibility\n");
	#endif

}

void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd )
{

	bool doit = 1;

	entvars_t *pev = VARS( (edict_t*)ent );

	#ifdef CRASHLOOK
		AvAddDebug( "UpdateClientData %s\n", STRING(pev->netname));
		//AvAddDebug( data );
	#endif	
		
	#ifdef SZ_DLL


		if (pev->fuser2 < gpGlobals->time) 
		{
			pev->fuser2 = gpGlobals->time + 0.5;
			TraceResult tr;
			UTIL_MakeVectors( pev->v_angle + pev->punchangle );
			Vector vecSrc = GetGunPosition( (edict_t*)ent );
			Vector vecEnd = vecSrc + (gpGlobals->v_forward * 1024);
			UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, (edict_t*)ent, &tr);

			if (tr.flFraction != 1.0 && tr.pHit && !FNullEnt(tr.pHit))
			{
				// see what we lookin at
				char *pClassname = (char *)STRING( tr.pHit->v.classname );

				if (FStrEq(pClassname, "building_radio")) {
								
					if (tr.pHit->v.euser4) {
									
						pev->fuser2 = gpGlobals->time + 2;
						char sbuf0[150];
						char songname[80];
						RadioGetSongName( tr.pHit->v.iuser3, songname);

						int heal = (int)(tr.pHit->v.health - 10000);
						sprintf(sbuf0, "Radio placed by: %s  Health: %i\nCurrent Song: %s", (char *)STRING(tr.pHit->v.euser4->v.netname), heal, songname);
						UTIL_HudMessageOne( ENT(pev), sbuf0 );

					}
				}
				else if (FStrEq(pClassname, "building_camera")) {
								
					if (tr.pHit->v.euser4) {

						pev->fuser2 = gpGlobals->time + 2;

						char sbuf0[80];

						int heal = (int)(tr.pHit->v.health - 10000);
						sprintf(sbuf0, "Camera built by: %s  Health: %i", (char *)STRING(tr.pHit->v.euser4->v.netname), heal);

						UTIL_HudMessageOne( ENT(pev), sbuf0 );

					}
				}
				else if (FStrEq(pClassname, "building_sign")) {
	

					if (tr.pHit->v.euser4 && !FNullEnt(tr.pHit->v.euser4)) {


						pev->fuser2 = gpGlobals->time + 2;	
						char sbuf0[400];

						int ind = ENTINDEX(tr.pHit);

						sprintf(sbuf0, "Sign placed by: %s \n\n%s", 
							(char *)STRING(tr.pHit->v.euser4->v.netname), 
							texts[ind].text );
						UTIL_HudMessageOne( ENT(pev), sbuf0 );
					}
				}		
				else if (FStrEq(pClassname, "player"))
				{
					lastSeenBy[ENTINDEX(ENT(pev))] = ENTINDEX(tr.pHit);
				}
			}
		}

	#endif
	


	(*other_gFunctionTable.pfnUpdateClientData)(ent, sendweapons, cd);
	
	//cd->fuser1 = pev->fuser1;

	if (DLLON)
	{
		#ifdef SZ_DLL
		   WepPlayerThink2( (edict_t *)ent, cd );
		#endif
	}


	/*
	fp=fopen("clientdata.txt","a");
	fprintf(fp, "origin: %f %f %f\n", cd->origin.x, cd->origin.y, cd->origin.z );
	fprintf(fp, "velocity: %f %f %f\n", cd->velocity.x, cd->velocity.y, cd->velocity.z );
	
	fprintf(fp, "viewmodel: %i\n",	cd->viewmodel );
	fprintf(fp, "flags: %i\n",		cd->flags );
	fprintf(fp, "waterlevel: %i\n",	cd->waterlevel );
	fprintf(fp, "watertype: %i\n",	cd->watertype );
	fprintf(fp, "health: %f\n",		cd->health );

	fprintf(fp, "bInDuck: %i\n",		cd->bInDuck );
	fprintf(fp, "weapons: %i\n",		cd->weapons );

	fprintf(fp, "maxspeed: %f\n",		cd->maxspeed );
	fprintf(fp, "fov: %f\n",			cd->fov );

	fprintf(fp, "weaponanim: %i\n",		cd->weaponanim );


	fprintf(fp, "m_iId: %i\n",			cd->m_iId );
	fprintf(fp, "ammo_shells: %i\n",	cd->ammo_shells );
	fprintf(fp, "ammo_nails: %i\n",		cd->ammo_nails );
	fprintf(fp, "ammo_cells: %i\n",		cd->ammo_cells );
	fprintf(fp, "ammo_rockets: %i\n",	cd->ammo_rockets );

	fprintf(fp, "tfstate: %i\n",		cd->tfstate );

	fprintf(fp, "pushmsec: %i\n",	cd->pushmsec );
	fprintf(fp, "deadflag: %i\n",	cd->deadflag );
	fprintf(fp, "iusers: %i %i %i %i\n",	cd->iuser1, cd->iuser2, cd->iuser3, cd->iuser4 );
	fprintf(fp, "fusers: %f %f %f %f\n",	cd->fuser1, cd->fuser2, cd->fuser3, cd->fuser4 );

	fclose(fp);
	*/

	

	#ifdef CRASHLOOK
		AvAddDebug( "After UpdateClientData\n");
	#endif	
}

int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet )
{

	#ifdef CRASHLOOK
//		AvAddDebug( "Add to Full Pack\n");
	#endif	
	
	int retval = (*other_gFunctionTable.pfnAddToFullPack)(state, e, ent, host, hostflags, player, pSet);

	#ifdef SZ_DLL


		if (ent->v.flags & FL_ALWAYS)
		{
			//ALERT(at_console, "ALWAYS IS TRUE ON A %s\n", STRING(ent->v.classname));
			retval = 1;
		
			memset( state, 0, sizeof( *state ) );

			// Assign index so we can track this entity from frame to frame and
			//  delta from it.
			state->number	  = e;
			state->entityType = ENTITY_NORMAL;
			
			// Flag custom entities.
			if ( ent->v.flags & FL_CUSTOMENTITY )
			{
				state->entityType = ENTITY_BEAM;
			}

			// 
			// Copy state data
			//

			// Round animtime to nearest millisecond
			state->animtime   = (int)(1000.0 * ent->v.animtime ) / 1000.0;

			memcpy( state->origin, ent->v.origin, 3 * sizeof( float ) );
			memcpy( state->angles, ent->v.angles, 3 * sizeof( float ) );
			memcpy( state->mins, ent->v.mins, 3 * sizeof( float ) );
			memcpy( state->maxs, ent->v.maxs, 3 * sizeof( float ) );

			memcpy( state->startpos, ent->v.startpos, 3 * sizeof( float ) );
			memcpy( state->endpos, ent->v.endpos, 3 * sizeof( float ) );

			state->impacttime = ent->v.impacttime;
			state->starttime = ent->v.starttime;

			state->modelindex = ent->v.modelindex;
				
			state->frame      = ent->v.frame;

			state->skin       = ent->v.skin;
			state->effects    = ent->v.effects;

			// This non-player entity is being moved by the game .dll and not the physics simulation system
			//  make sure that we interpolate it's position on the client if it moves
			if ( !player &&
				 ent->v.animtime &&
				 ent->v.velocity[ 0 ] == 0 && 
				 ent->v.velocity[ 1 ] == 0 && 
				 ent->v.velocity[ 2 ] == 0 )
			{
				state->eflags |= EFLAG_SLERP;
			}

			state->scale	  = ent->v.scale;
			state->solid	  = ent->v.solid;
			state->colormap   = ent->v.colormap;

			state->movetype   = ent->v.movetype;
			state->sequence   = ent->v.sequence;
			state->framerate  = ent->v.framerate;
			state->body       = ent->v.body;
			int i;
			for (i = 0; i < 4; i++)
			{
				state->controller[i] = ent->v.controller[i];
			}

			for (i = 0; i < 2; i++)
			{
				state->blending[i]   = ent->v.blending[i];
			}

			state->rendermode    = ent->v.rendermode;
			state->renderamt     = ent->v.renderamt; 
			state->renderfx      = ent->v.renderfx;
			state->rendercolor.r = ent->v.rendercolor.x;
			state->rendercolor.g = ent->v.rendercolor.y;
			state->rendercolor.b = ent->v.rendercolor.z;

			state->aiment = 0;
			if ( ent->v.aiment )
			{
				state->aiment = ENTINDEX( ent->v.aiment );
			}

			state->owner = 0;
			if ( ent->v.owner )
			{
				int owner = ENTINDEX( ent->v.owner );
				
				// Only care if owned by a player
				if ( owner >= 1 && owner <= gpGlobals->maxClients )
				{
					state->owner = owner;	
				}
			}

			// HACK:  Somewhat...
			// Class is overridden for non-players to signify a breakable glass object ( sort of a class? )
			if ( !player )
			{
				state->playerclass  = ent->v.playerclass;
			}

			// Special stuff for players only
			if ( player )
			{
				memcpy( state->basevelocity, ent->v.basevelocity, 3 * sizeof( float ) );

				state->weaponmodel  = MODEL_INDEX( STRING( ent->v.weaponmodel ) );
				state->gaitsequence = ent->v.gaitsequence;
				state->spectator = ent->v.flags & FL_SPECTATOR;
				state->friction     = ent->v.friction;

				state->gravity      = ent->v.gravity;
		//		state->team			= ent->v.team;
		//		
				state->usehull      = ( ent->v.flags & FL_DUCKING ) ? 1 : 0;
				state->health		= ent->v.health;
			}	
		}

		int mynum = ENTINDEX(host);


		// see if this ent is in memory

		if (InMemNum[e])
		{
			if (InMemNum[e] == 1) {
				state->rendercolor.r = 255;
				state->rendercolor.g = 0;
				state->rendercolor.b = 0;
			}
			if (InMemNum[e] == 2) {
				state->rendercolor.r = 255;
				state->rendercolor.g = 255;
				state->rendercolor.b = 0;
			}
			if (InMemNum[e] == 3) {
				state->rendercolor.r = 0;
				state->rendercolor.g = 255;
				state->rendercolor.b = 0;
			}
			if (InMemNum[e] == 4) {
				state->rendercolor.r = 100;
				state->rendercolor.g = 100;
				state->rendercolor.b = 255;
			}
			if (InMemNum[e] == 5) {
				state->rendercolor.r = 255;
				state->rendercolor.g = 0;
				state->rendercolor.b = 255;
			}
			if (InMemNum[e] == 6) {
				state->rendercolor.r = 0;
				state->rendercolor.g = 255;
				state->rendercolor.b = 255;
			}

			state->rendermode = 1;
			state->renderamt = 200;


		}

		// end see

		
		if (PlayerInfo[mynum].hasSpecialMode)
		{
			state->rendermode = 1;
			state->renderamt = 100;
			state->rendercolor.r = 0;
			state->rendercolor.g = 0;
			state->rendercolor.b = 200;
		}
		if (PlayerInfo[mynum].superBright)
		{
			state->rendermode = 5;
			state->renderamt = 255;
		}
		if (host == ent)
		{
			// this is me
			state->angles.x = -state->angles.x;

			state->fuser1 = ent->v.fuser1;
			if (ent->v.fuser1) retval = 1;
		}

		if (player)
		{
			state->fuser1 = 1;

			// set colormap
//			state->colormap = 10000;

		}

		if (PlayerInfo[mynum].noPlayers && player)
		{
			state->rendermode = 2;
			state->renderamt = 0;
		}

		/* causes crash.
		else if (FStrEq(STRING(ent->v.classname), "av_pet"))
		{
			state->frame = ent->v.frame;
			retval = 1;
		}
	*/
	#endif

	if (retval)
	{
		
		//entvars_t *pev = VARS(host);

		// see if we are back at the start
		//if (e <= pev->vuser2.x)
		//{
			// yes. Reset counters.
		//	pev->vuser2.y = 0;
		//}

		// this ENT is being SENT, so increase counter.
		//pev->vuser2.y++;
		
		// mark off the ent
		//pev->vuser2.x = e;

		// if this is over the total, cancel the send.
		//if (pev->vuser2.y > 250)
		//{
		//	retval = 0;
		//}
	}

	#ifdef CRASHLOOK
		//AvAddDebug( "After Add to Full Pack\n" );
	#endif	




	return retval;
}

void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs )
{
	(*other_gFunctionTable.pfnCreateBaseline)(player, eindex, baseline, entity, playermodelindex, player_mins, player_maxs);
}

void RegisterEncoders( void )
{
	#ifdef CRASHLOOK
		AvAddDebug( "registerencoders\n" );
	#endif	
   (*other_gFunctionTable.pfnRegisterEncoders)();
	#ifdef CRASHLOOK
		AvAddDebug( "after registerencoders\n" );
	#endif	
}

int GetWeaponData( struct edict_s *player, struct weapon_data_s *info )
{
	#ifdef CRASHLOOK
		AvAddDebug( "getweapondata\n" );
	#endif	
   return (*other_gFunctionTable.pfnGetWeaponData)(player, info);
	#ifdef CRASHLOOK
		AvAddDebug( "after getweapondata\n" );
	#endif	
}

void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed )
{
	#ifdef CRASHLOOK
		AvAddDebug( "cmd start\n" );
	#endif	
   (*other_gFunctionTable.pfnCmdStart)(player, cmd, random_seed);
	#ifdef CRASHLOOK
		AvAddDebug( "after cmd start\n" );
	#endif	
}

void CmdEnd ( const edict_t *player )
{
	#ifdef CRASHLOOK
		AvAddDebug( "cmd end\n" );
	#endif	
   (*other_gFunctionTable.pfnCmdEnd)(player);
	#ifdef CRASHLOOK
		AvAddDebug( "after cmd end\n" );
	#endif	
}

int ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
   return (*other_gFunctionTable.pfnConnectionlessPacket)(net_from, args, response_buffer, response_buffer_size);
}

int GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	#ifdef CRASHLOOK
		AvAddDebug( "gethullbounds\n" );
	#endif	
   return (*other_gFunctionTable.pfnGetHullBounds)(hullnumber, mins, maxs);
	#ifdef CRASHLOOK
		AvAddDebug( "after gethullbounds\n" );
	#endif	
}

void CreateInstancedBaselines( void )
{
	#ifdef CRASHLOOK
		AvAddDebug( "createinstancedbaselines\n" );
	#endif	
   (*other_gFunctionTable.pfnCreateInstancedBaselines)();
	#ifdef CRASHLOOK
		AvAddDebug( "after createinstancedbaselines\n" );
	#endif	
}

int InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message )
{
	#ifdef CRASHLOOK
		AvAddDebug( "incon file\n" );
	#endif	
	return (*other_gFunctionTable.pfnInconsistentFile)(player, filename, disconnect_message);
	#ifdef CRASHLOOK
		AvAddDebug( "after incon file\n" );
	#endif	
}

int AllowLagCompensation( void )
{
	#ifdef CRASHLOOK
		AvAddDebug( "allow lag\n" );
	#endif	
	return (*other_gFunctionTable.pfnAllowLagCompensation)();
	#ifdef CRASHLOOK
		AvAddDebug( "after allow lag\n" );
	#endif	
}


const char *Cmd_Args( void )
{
   return (*g_engfuncs.pfnCmd_Args)();
}


const char *Cmd_Argv( int argc )
{
    return (*g_engfuncs.pfnCmd_Argv)(argc);
}


int Cmd_Argc( void )
{
   return (*g_engfuncs.pfnCmd_Argc)();
}

