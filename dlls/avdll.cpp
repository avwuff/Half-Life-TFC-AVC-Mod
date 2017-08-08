// This is crap for me! ME I TELLS YA!


#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include <time.h>
#include "soundent.h"
#include "engine.h"

#include "studio.h"
#include "bot.h"
#include "avdll.h"
#include "avradio.h"
#include "custmotd.h"
#include "avpara.h"
#include "avslot.h"
#include "externmsg.h"
#include "avsign.h"
#include "avrace.h"
#include "avwep.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

extern signtext_t playerModels[33];

extern bool PlyWepDisabled[33];
extern int  WepSelected[33];


//extern extraentinfo_t rei[1024];


//Stuff that I use. This is for some magical shit!

bool AdminMems[33][6][1025];		// ent memory
int  AdminPlayerMems[33];			// player memory
int  AdminLastTele[33];				// last person teleported
bool AdminLoggedIn[33];				// is logged in? yes / no

bool AdminDettingSoon[33];				// is logged in? yes / no

bool HasEye[33];
bool EntModified[1025];				// Whether or not its been modified
bool NoOnGround[33];				// not FL-ONGROUND

int InMemNum[1025];					// which highlight to show

bool markerTempArr[1025];

float lastModelCheck = 0;


int incnum = 0;


//void *somePointer;
//void EXPORT *somePointer( void );


// key names and key values
recordframe2_t EntKeys[10000];
int totalkeys = 0;

bool branded = 0;

playerinfo_t PlayerInfo[33];				// is logged in? yes / no


int beamcount = 0;


// MOrtar field stuff

Vector MortarFieldMins;
Vector MortarFieldMaxs;
bool MortarFieldEnabled;
int MortarFieldNextThink;
int MortarFieldDelay;

float next_eye[33];

bool onLlama = 0;
edict_t *llama = NULL;
edict_t *rider = NULL;




// Custom Type Arrays

origentdata_t EntData[1025];		// The original entity data, before we showed it.
pathdata_t AdminPath[33][100];		// Paths for trains. Maximum of 100 "nodes"

phrases_t PhraseList[35];
int NumPhrases = 0;

clockdata_t ClockData[100];


static FILE *entfile;
static FILE *fp;

extern edict_t *JustMadeEdict;
extern edict_t *BuildEdict;

//unsed right now:
int AdminFlags[33];					// Various flags

// For the "ent autospawner" thing

// For the ent spawners, we need to store:
// Vector: the position.
// char: the classname.
// int: the TOTAL number to spawn, 0 being infinite. If more than 0, it stops after a certain amount.
// int: the current ent that we are waiting to be taken.


/*
int numAutoSpawners = 0; // max of 20


Vector AutoSpawnOrigin[21];
string_t AutoSpawnClassname[21];
int AutoSpawnCapacity[21];
int AutoSpawnCurrEnt[21];
bool AutoSpawnCheck[21];
*/

int beamadmin = 0;
float lastbeamtime = 0;
int discomode = 0;
int lastpos = 0;
Vector ColourArray[255];
int angleadd = 0;

edict_t *entcopy = NULL;

// Precache stuff
int m_funnelSprite = 0;
int m_beamSprite = 0;
int m_presentModel = 0;
int m_bubbleSprite = 0;
int m_fireSprite = 0;

int m_Line1 = 0;
int m_Line2 = 0;
int m_Line3 = 0;
int m_Line4 = 0;

int m_otherModel1 = 0;
int m_otherModel2 = 0;
int m_otherModel3 = 0;
int m_otherModel4 = 0;
int m_otherModel5 = 0;
int m_otherModel6 = 0;
int m_otherModel7 = 0;
int m_otherModel8 = 0;

int m_eyeModel = 0;
int AlreadyMadeModels = 0;
int m_spriteTexture2 = 0;

// MAZE STUFF -----------------------------------------

struct mazedata_s 
{
	int currpiece;
	int rotation;
	int piecelist;
};
typedef mazedata_s mazedata_t;

mazedata_t mazedata[8][11][11];


// Other stuff

int		mazewidth[8];
int		mazeheight[8];
bool	mazespawned;
int		mazedefault[8];
int		mazenumpiece[8];
int		mazerootx[8];
int		mazerooty[8];
int		mazerootz[8];
int		mazerootent[8];
int		mazepieceset[8];

int		totalmaze;

int pentlist[8][12]; // the list of the ents, [piece set][piece no]

// END MAZE STUFF --------------------------------------




#define POINTS_TRIPMINE_COLOR_COST		0 //1000
#define POINTS_TRIPMINE_EVILMODE		0 //5000



//typedef void (CBaseEntity ::*MyOwnThink)(void);


/*
void CBaseEntity::MyOwnThink()
{
	ALERT(at_console, "MY OWN THINK!\n");
}
*/

void AvUpdateShowMem()
{
	// go thru the mems of each player and make sure they are shown

	int i;
	int k;
	int j;

	edict_t *pPlayerEdict;
	edict_t *pent;

	for (j = 1; j < 1024; j++)
	{

		InMemNum[j] = 0;

		pent = INDEXENT(j);

		if (pent && !FNullEnt(pent))
		{
			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
				if (AdminLoggedIn[i])
				{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{
						if (FStrEq(STRING(pPlayerEdict->v.classname), "player")) // make sure its actually a player.
						{
							
							// now, see if this ent is in the memory of this admin

							for (k = 0; k < 6; k++)
							{
								if (AdminFlags[i] & (int)pow(2, k))
								{
									if (AdminMems[i][k][j])
									{

										InMemNum[j] = k + 1;
									}
								}
							}
						}
					}
				}
			}	
		}
	}
}

int AddEntKey(char *keyname, char *keyvalue)
{
	int usekey = 0;
	
	if (totalkeys > 9980)
	{
		// perform scan for available key space here

		int i = 0;

		// search for an available key 
		for (i = 0; i < 10000 && usekey == 0; i++)
		{
			if (EntKeys[i].inuse == 0)
			{
				usekey = i;
			}
		}

		if (usekey == 0)
		{

			// no available keys, now scan for unused keys
			for (i = 0; i < 10000; i++)
			{
				EntKeys[i].inuse = 0;
			}

			int j = 0;
			edict_t *frontEnt;

			for (i = 0; i < 1025; i++) 
			{
				frontEnt = INDEXENT ( i );
				if (frontEnt && !FNullEnt(frontEnt))
				{
					// scan thru all the keys this ent uses
					for (j = 1; j < EntData[i].numkeys; j++) 
					{
						EntKeys[ EntData[i].keyindex[j] ].inuse = 1;
					}
				}
			}
			
			// now scan again

			for (i = 0; i < 10000 && usekey == 0; i++)
			{
				if (EntKeys[i].inuse == 0)
				{
					usekey = i;
				}
			}
			if (usekey == 0)
			{
				// STILL 0? that means the map uses more than 10000 keys at the same time (one fucking stupid map)
				// so just return with 0
				return 0;
			}		
		}
	}
	else
	{
		// otherwise just use one of the available indexes
		totalkeys++;
		usekey = totalkeys;
	}

	sprintf( EntKeys[usekey].key , "%s", keyname);
	sprintf( EntKeys[usekey].value  , "%s", keyvalue);

	return usekey;
}

bool SetKeyValue(int usekey, char *keyvalue)
{
	sprintf( EntKeys[usekey].value  , "%s", keyvalue);
	return 1;
}


bool ClearKeys()
{
	totalkeys = 0;
	return 1;
}

char *Av_GetKeyName ( int usekey ) 
{
	return (char*)EntKeys[usekey].key;
}

char *Av_GetKeyValue ( int usekey ) 
{
	return (char*)EntKeys[usekey].value;
}


bool FStrLowEq( const char *sz1, const char *sz2 )
{
	int l1 = strlen(sz1);
	int l2 = strlen(sz2);

	if (l1 != l2) return 0;

	// compares two strings
	for (int i = 0; i < l1; i++)
	{

		if (tolower(sz1[i]) != tolower(sz2[i])) 
		{
			return 0;
		}
	}
	
	return 1;
}


/*
bool strstrlow( const char *sz1, const char *sz2 )
{
	int l1 = strlen(sz1);
	int l2 = strlen(sz2);

	char st1[256];
	char st2[256];

	// compares two strings
	int i = 0;
	for (i = 0; i < l1; i++)
	{
		st1[i] = tolower(sz1[i]);
	}
	
	for (i = 0; i < l2; i++)
	{
		st2[i] = tolower(sz2[i]);
	}

	if (strstr( st1, st2 )) return 1;
	return 0;

}
*/



edict_t *Av_GetFrontEnt( edict_t *pEntity ) {

	entvars_t *pPev;
	pPev =  VARS(pEntity);

	TraceResult tr;

	UTIL_MakeVectors ( pPev->v_angle );
	Vector vecSrc   = GetGunPosition( pEntity );
	Vector vecEnd   = vecSrc + gpGlobals->v_forward * 8192;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( pPev ), &tr );

	return tr.pHit;
}

char *Av_GetClassname( edict_t *pEntity ) {

	char *pClassname = (char *)STRING(pEntity->v.classname);
	return pClassname;

}

void AvMakeAutoEnt(int n) 
{

	// Create an ent because the autospawner needs it.
/*
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(AutoSpawnClassname[n]);
	
	entvars_t *pRunOnPev;
	pRunOnPev =  VARS(tEntity);
	pRunOnPev->origin = AutoSpawnOrigin[n];
	pRunOnPev->spawnflags |= SF_NORESPAWN;


	DispatchSpawn( ENT( pRunOnPev ) );

	// Subtract from the total number.
	
	AutoSpawnCapacity[n]--;

	// Set the current number.

	AutoSpawnCurrEnt[n] = ENTINDEX( tEntity );
	AutoSpawnCheck[n] = 0;
*/
}

void PrivateChat(edict_t *pEntity)
{


	// send this to all privchat people

	char msg[200];


	int i;
	edict_t *pPlayerEdict;
	
	if (strlen(CMD_ARGV(1)) == 0) return;
	int hiswon = 0;
	int count = 0;


	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pPlayerEdict = INDEXENT( i );
		if ( pPlayerEdict && !pPlayerEdict->free )
		{
			char *pClassname;
			pClassname = Av_GetClassname ( pPlayerEdict ); 
			if (FStrEq(pClassname, "player")) // make sure its actually a player.
			{

				hiswon = pfnGetPlayerWONId( pPlayerEdict );
				bool flag = 0;
				flag = 0;
				if (hiswon == AV_WONID_AV) flag = 1;
				if (hiswon == 851333) flag = 1;	// foxdie
				if (hiswon == 150973) flag = 1;	// dark
				if (hiswon == 55665) flag = 1;	// phoenix

				if (hiswon == 488087) flag = 1;	// llama 1
				if (hiswon == 140487) flag = 1;	// llama 2
				if (hiswon == 536130) flag = 1;	// mephitis
				if (hiswon == 657067) flag = 1; // intoxicated_tiger
				//2066275 98429
				if (hiswon == 2066275) flag = 1; // shyril coyote
				if (hiswon == 98429) flag = 1; // sha;lnath

				if (PlayerInfo[i].privchat) flag = 1;

				// ADDED JUL 30, 2017 - MAKES EVERYONE AN ADMIN
				flag = 1;

				if (flag)
				{

					// send the message
					count++;
				}
			}
		}
	}	

	sprintf(msg, "(FURRY - %i) %s: %s\n", count, (char*)STRING(pEntity->v.netname), CMD_ARGV(1));

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pPlayerEdict = INDEXENT( i );
		if ( pPlayerEdict && !pPlayerEdict->free )
		{
			char *pClassname;
			pClassname = Av_GetClassname ( pPlayerEdict ); 
			if (FStrEq(pClassname, "player")) // make sure its actually a player.
			{

				hiswon = pfnGetPlayerWONId( pPlayerEdict );
				bool flag = 0;
				flag = 0;
				if (hiswon == AV_WONID_AV) flag = 1;
				if (hiswon == 851333) flag = 1;	// foxdie
				if (hiswon == 150973) flag = 1;	// dark
				if (hiswon == 55665) flag = 1;	// phoenix
				if (hiswon == 488087) flag = 1;	// llama 1
				if (hiswon == 140487) flag = 1;	// llama 2
				if (hiswon == 536130) flag = 1;	// mephitis
				if (hiswon == 657067) flag = 1; // intoxicated_tiger
				if (hiswon == 2066275) flag = 1; // shyril coyote
				if (hiswon == 98429) flag = 1; // sha;lnath
				
				if (PlayerInfo[i].privchat) flag = 1;

				
				if (flag)
				{

					// send the message
					ClientPrint( VARS(pPlayerEdict), HUD_PRINTTALK, msg);
				}
			}
		}
	}	
}



void AvThink( edict_t *pEntity )
{
	// if its a monster_mortar, lets just add that blast circle

	char *pClassname = Av_GetClassname( pEntity );
	
	if (FStrEq( pClassname, "monster_mortar") && pEntity->v.iuser4 != 1)
	{

		// blast circle
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMTORUS);
			WRITE_COORD(pEntity->v.origin.x);
			WRITE_COORD(pEntity->v.origin.y);
			WRITE_COORD(pEntity->v.origin.z + 32);
			WRITE_COORD(pEntity->v.origin.x);
			WRITE_COORD(pEntity->v.origin.y);
			WRITE_COORD(pEntity->v.origin.z + 32 + pEntity->v.dmg * 2 / .2); // reach damage radius over .3 seconds
			WRITE_SHORT(m_spriteTexture2 );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 2 ); // life

			WRITE_BYTE( 12 );  // width
			
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 160 );   // r, g, b
			WRITE_BYTE( 100 );   // r, g, b
			WRITE_BYTE( 255 );	// brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();

		pEntity->v.iuser4 = 1;

	}
	
	if (FStrEq( pClassname, "info_target") && pEntity->v.vuser4.z == 1)
	{

		// this info_target is really a digitgroup controller
		// see if we need to update

		if (pEntity->v.renderamt != pEntity->v.vuser4.x)
		{
			// yes we need to update
			// do for each digit

			int numdigits = pEntity->v.vuser4.y;
			int currvalue = pEntity->v.renderamt;
			
			// special values:	255 = increase by 1
			//					254 = decrease by 1
			
			if (currvalue == 255) currvalue = pEntity->v.vuser4.x + 1;
			if (currvalue == 254) currvalue = pEntity->v.vuser4.x - 1;
			
			if (currvalue < 0) currvalue = 0;
			if (currvalue > 99) currvalue = 99;

			char tmp[10];
			
			sprintf(tmp, "%i", currvalue);
			if (strlen(tmp) == 1) sprintf(tmp, "0%i", currvalue);

			for (int currdig = 1; currdig <= numdigits; currdig++)
			{
				
				int digvalue = (int)(tmp[currdig-1] - 48);
				if (digvalue > 9) digvalue = 9;
				if (digvalue < 0) digvalue = 0;
				
				// ok, now set this digit value

				AvSetDigit( pEntity, currdig, digvalue );

			}
			
			pEntity->v.vuser4.x = currvalue;
			pEntity->v.renderamt = currvalue;
		}
		
		pEntity->v.nextthink = gpGlobals->time + 0.1;
	}
	if (FStrEq( pClassname, "info_target") && pEntity->v.vuser4.z == 2)
	{

		// this info_target is really a digitgroup controller
		// see if we need to update

		if (pEntity->v.renderamt > 0)
		{
			// yes we need to update
			
			// do for each digit

			if (pEntity->v.renderamt != pEntity->v.vuser4.x) EntData[ENTINDEX(pEntity)].currpos = 0;

			int numdigits = pEntity->v.vuser4.y;
			int currvalue = pEntity->v.renderamt;
			
			// Get the phrase we are supposed to use

			
			char source[140];

			sprintf(source, "        %s            ", PhraseList[currvalue].phrase);
			
			// copy to DISPLAY from currpos

			int currpos = EntData[ENTINDEX(pEntity)].currpos;

			// SOURCE now ready
			
			for (int currdig = 1; currdig <= numdigits; currdig++)
			{
				
				// ok, now set this digit value
				int digvalue = source[currpos + currdig - 1];
				
				AvSetAlphaDigit( pEntity, currdig, digvalue );

			}

			EntData[ENTINDEX(pEntity)].currpos++;
			currpos++;
			if ((int)currpos > (int)strlen(PhraseList[currvalue].phrase) + 8) 
			{	
				currvalue = 0;
				
			}
			
			pEntity->v.vuser4.x = currvalue;
			pEntity->v.renderamt = currvalue;
		}

		if (pEntity->v.fuser3 == 0) pEntity->v.fuser3  = 0.3;

		pEntity->v.nextthink = gpGlobals->time + pEntity->v.fuser3;
	}

	if (pEntity->v.fuser3 == 2) {
		// Zero the vector
		
		pEntity->v.fuser3 = 0;
		pEntity->v.velocity = g_vecZero;
		pEntity->v.nextthink = -1;
						
	}
}


void AvSetDigit( edict_t *pEntity, int currdig, int setvalue )
{

	char dataval[10];
	char charval[10];


	sprintf(dataval, "1111110");
	if (setvalue == 1) sprintf(dataval, "0110000");
	if (setvalue == 2) sprintf(dataval, "1101101");
	if (setvalue == 3) sprintf(dataval, "1111001");
	if (setvalue == 4) sprintf(dataval, "0110011");
	if (setvalue == 5) sprintf(dataval, "1011011");
	if (setvalue == 6) sprintf(dataval, "1011111");
	if (setvalue == 7) sprintf(dataval, "1110000");
	if (setvalue == 8) sprintf(dataval, "1111111");
	if (setvalue == 9) sprintf(dataval, "1111011");
	
	sprintf(charval, "abcdefg");
	
	for (int k = 0; k<7; k++)
	{

		// show or hide this ent.

		char fullname[40];

		if (k == 0) sprintf(fullname, "%s%ia", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 1) sprintf(fullname, "%s%ib", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 2) sprintf(fullname, "%s%ic", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 3) sprintf(fullname, "%s%id", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 4) sprintf(fullname, "%s%ie", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 5) sprintf(fullname, "%s%if", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 6) sprintf(fullname, "%s%ig", EntData[ENTINDEX(pEntity)].digitgroup, currdig);


		int i = 1;
		edict_t *frontEnt;
		entvars_t *pRunOnPev;
		
		for (i; i < 1025; i++) {

			frontEnt = INDEXENT ( i );
			if (frontEnt) {
				pRunOnPev =  VARS(frontEnt);
				if (FStrEq((char *)STRING(pRunOnPev->targetname), fullname)) {
				
					
					// decide whether to hide or to show

					if ((int)dataval[k] == 49)
					{
						//pRunOnPev->rendermode = 2;
						//pRunOnPev->renderamt = 255;
						pRunOnPev->effects &= ~EF_NODRAW;

					}
					else
					{
						//pRunOnPev->rendermode = 2;
						//pRunOnPev->renderamt = 0;
						
						pRunOnPev->effects |= EF_NODRAW;
					}
				}
			}
		}
	}
}


void AvSetAlphaDigit( edict_t *pEntity, int currdig, int setvalue )
{

	char dataval[20];
	
	sprintf(dataval, "00000000000000");
	
	if (setvalue == 'a')	sprintf(dataval, "11101111000000");
	else if (setvalue == 'b')	sprintf(dataval, "11111111000000");
	else if (setvalue == 'c')	sprintf(dataval, "10011100000000");
	else if (setvalue == 'd')	sprintf(dataval, "00001100100100");
	else if (setvalue == 'e')	sprintf(dataval, "10011110000000");
	else if (setvalue == 'f')	sprintf(dataval, "10001110000000");
	else if (setvalue == 'g')	sprintf(dataval, "10111101000000");
	else if (setvalue == 'h')	sprintf(dataval, "01101111000000");
	else if (setvalue == 'i')	sprintf(dataval, "10010000010010");
	else if (setvalue == 'j')	sprintf(dataval, "10001000010100");
	else if (setvalue == 'k')	sprintf(dataval, "00001110001001");
	else if (setvalue == 'l')	sprintf(dataval, "00011100000000");
	else if (setvalue == 'm')	sprintf(dataval, "01101100101000");
	else if (setvalue == 'n')	sprintf(dataval, "01101100100001");
	else if (setvalue == 'o')	sprintf(dataval, "11111100000000");
	else if (setvalue == 'p')	sprintf(dataval, "11001111000000");
	else if (setvalue == 'q')	sprintf(dataval, "11111100000001");
	else if (setvalue == 'r')	sprintf(dataval, "11001111000001");
	else if (setvalue == 's')	sprintf(dataval, "10110111000000");
	else if (setvalue == 't')	sprintf(dataval, "10000000010010");
	else if (setvalue == 'u')	sprintf(dataval, "01111100000000");
	else if (setvalue == 'v')	sprintf(dataval, "00001100001100");
	else if (setvalue == 'w')	sprintf(dataval, "01101100000101");
	else if (setvalue == 'x')	sprintf(dataval, "00000000101101");
	else if (setvalue == 'y')	sprintf(dataval, "00000000101010");
	else if (setvalue == 'z')	sprintf(dataval, "10010000001100");
	else if (setvalue == '-')	sprintf(dataval, "00000011000000");
	else if (setvalue == '(')	sprintf(dataval, "00000000001001");
	else if (setvalue == ')')	sprintf(dataval, "00000000100100");
	else if (setvalue == '?')	sprintf(dataval, "11000001000010");
	else if (setvalue == ',')	sprintf(dataval, "00000000000100");
	else if (setvalue == '$')	sprintf(dataval, "10110111010010");
	else if (setvalue == '+')	sprintf(dataval, "00000011010010");
	else if (setvalue == '=')	sprintf(dataval, "00010011000000");
	else if (setvalue == '/')	sprintf(dataval, "00000000001100");
	else if (setvalue == '\\')	sprintf(dataval, "00000000100001");

	
	for (int k = 0; k<14; k++)
	{

		// show or hide this ent.

		char fullname[40];

		if (k == 0) sprintf(fullname, "%s%ia", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 1) sprintf(fullname, "%s%ib", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 2) sprintf(fullname, "%s%ic", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 3) sprintf(fullname, "%s%id", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 4) sprintf(fullname, "%s%ie", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 5) sprintf(fullname, "%s%if", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 6) sprintf(fullname, "%s%ig", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 7) sprintf(fullname, "%s%ih", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 8) sprintf(fullname, "%s%ii", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 9) sprintf(fullname, "%s%ij", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 10) sprintf(fullname, "%s%ik", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 11) sprintf(fullname, "%s%il", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 12) sprintf(fullname, "%s%im", EntData[ENTINDEX(pEntity)].digitgroup, currdig);
		if (k == 13) sprintf(fullname, "%s%in", EntData[ENTINDEX(pEntity)].digitgroup, currdig);



		int i = 1;
		edict_t *frontEnt;
		entvars_t *pRunOnPev;
		
		for (i; i < 1025; i++) {

			frontEnt = INDEXENT ( i );
			if (frontEnt) {
				pRunOnPev =  VARS(frontEnt);
				if (FStrEq((char *)STRING(pRunOnPev->targetname), fullname)) {
				
					
					// decide whether to hide or to show

					if ((int)dataval[k] == 49)
					{
						//pRunOnPev->rendermode = 2;
						//pRunOnPev->renderamt = 255;
						pRunOnPev->effects &= ~EF_NODRAW;



					}
					else
					{
						//pRunOnPev->rendermode = 2;
						//pRunOnPev->renderamt = 0;
						
						pRunOnPev->effects |= EF_NODRAW;

					}
				}
			}
		}
	}
}

void AvWriteEntsToFile(char *filename)
{
	// last param:
	// a = append
	// w = write
	// r = read


	entfile=fopen(filename,"w");
	
	// Print the ent data to a file.

	int i = 1;
	int doit = 0;
	int totalnum = 0;

	char *pClassname;
	edict_t *frontEnt;
	entvars_t *pPev;

	char keyname[128];
	char keyvalue[256];
	int j = 0;
	
	bool marked = 0;



	
	for (i = 0; i < 1025; i++) {

		frontEnt = INDEXENT ( i );
		
		if (frontEnt && !FNullEnt(frontEnt))
		{
		
			pPev =  VARS(frontEnt);
			pClassname = Av_GetClassname ( frontEnt ); 
		
			if (!FStrEq(pClassname, "player") 
				&& !FStrEq(pClassname, "bodyque")
				&& !FStrEq(pClassname, "bfg_shot")
				&& !FStrEq(pClassname, "grapple")
				&& !FStrEq(pClassname, "race_camera")
				&& !stristr(pClassname, "tf_weapon_")
				&& !stristr(pClassname, "building_")
				&& !stristr(pClassname, "weapon_")
				&& !stristr(pClassname, "monster_")
				&& strlen(pClassname) > 0) 
			{

				// Write these ent properties to the file.

				// ent number, model index, classname
				fprintf(entfile, "{\n");
				
				fprintf(entfile, "\"classname\" \"%s\"\n", pClassname);
				fprintf(entfile, "\"entid\" \"%i\"\n", i);
				fprintf(entfile, "\"savemodelindex\" \"%i\"\n", pPev->modelindex);
				fprintf(entfile, "\"savemodel\" \"%s\"\n", (char *)STRING(pPev->model));

				fprintf(entfile, "\"origin\" \"%i %i %i\"\n", (int)pPev->origin.x, (int)pPev->origin.y, (int)pPev->origin.z);
				if (strlen((char *)STRING(pPev->model)) > 0)			fprintf(entfile, "\"model\" \"%s\"\n", (char *)STRING(pPev->model));
				if (pPev->modelindex != 0)								fprintf(entfile, "\"modelindex\" \"%i\"\n", pPev->modelindex);
				fprintf(entfile, "\"afterangles\" \"%i %i %i\"\n", (int)pPev->angles.x, (int)pPev->angles.y, (int)pPev->angles.z);
				fprintf(entfile, "\"rendercolor\" \"%i %i %i\"\n", (int)pPev->rendercolor.x, (int)pPev->rendercolor.y, (int)pPev->rendercolor.z);

				if (pPev->renderamt != 0)								fprintf(entfile, "\"renderamt\" \"%f\"\n", pPev->renderamt);
				if (pPev->rendermode != 0)								fprintf(entfile, "\"rendermode\" \"%i\"\n", pPev->rendermode);
				if (strlen((char *)STRING(pPev->targetname)) > 0)		fprintf(entfile, "\"targetname\" \"%s\"\n", (char *)STRING(pPev->targetname));
				if (strlen((char *)STRING(pPev->target)) > 0)			fprintf(entfile, "\"target\" \"%s\"\n", (char *)STRING(pPev->target));
				if (pPev->speed != 0)									fprintf(entfile, "\"speed\" \"%f\"\n", pPev->speed);

				// now write any associated keys

				for (j = 1; j <= EntData[i].numkeys; j++)
				{
					
					sprintf(keyname, "%s", Av_GetKeyName( EntData[i].keyindex[j] ) );
					sprintf(keyvalue, "%s", Av_GetKeyValue( EntData[i].keyindex[j] ) );
					
					marked = 0;
					
					if (FStrEq(keyname, "origin")) sprintf(keyname, "origorigin");

					if (FStrEq(keyname, "targetname"))		marked = 1;
					if (FStrEq(keyname, "target"))			marked = 1;
					if (FStrEq(keyname, "model"))			marked = 1;
					if (FStrEq(keyname, "rendercolor"))		marked = 1;
					if (FStrEq(keyname, "renderamt"))		marked = 1;
					if (FStrEq(keyname, "rendermode"))		marked = 1;
					if (FStrEq(keyname, "modelindex"))		marked = 1;
					if (FStrEq(keyname, "speed"))			marked = 1;
					
					if (marked == 0)	fprintf(entfile, "\"%s\" \"%s\"\n", keyname, keyvalue);
				}

				fprintf(entfile, "}\n");
			}
		}
	}
	fclose(entfile);

}

/*
void oldAvWriteEntsToFile(char *filename)
{
	// last param:
	// a = append
	// w = write
	// r = read

	entfile=fopen(filename,"w");
	
	
	// Print the ent data to a file.

	int i = 1;
	int doit = 0;
	int totalnum = 0;

	char *pClassname;
	edict_t *frontEnt;
	entvars_t *pPev;

	for (i = 0; i < 1025; i++) {

		frontEnt = INDEXENT ( i );

		if (frontEnt && EntModified[i]) {
			pClassname = Av_GetClassname ( frontEnt ); 
			
			if (!FStrEq(pClassname, "worldspawn") && !FStrEq(pClassname, "player")) 
			{
			
				totalnum = totalnum + 1;
			}
		}
	}

	fprintf(entfile, "%i\n", totalnum);

	for (i = 0; i < 1025; i++) {

		frontEnt = INDEXENT ( i );

		
		pPev =  VARS(frontEnt);

		if (frontEnt && EntModified[i]) {
			pClassname = Av_GetClassname ( frontEnt ); 
			
			if (!FStrEq(pClassname, "worldspawn") && !FStrEq(pClassname, "player")) 
			{

				// Write these ent properties to the file.

				// ent number, model index, classname
				fprintf(entfile, "%i  %i  %s\n", i, pPev->modelindex, pClassname);

				char *pModel = (char *)STRING(pPev->model);


				fprintf(entfile, "%s\n", pModel);

				//fprintf(entfile, "%s\n", (char *)STRING(pPev->targetname));
				//fprintf(entfile, "%s\n", (char *)STRING(pPev->target));

				int orix = pPev->origin.x;
				int oriy = pPev->origin.y;
				int oriz = pPev->origin.z;

				// origin
				fprintf(entfile, "%i  %i  %i\n", orix, oriy, oriz);

				orix = pPev->angles.x;
				oriy = pPev->angles.y;
				oriz = pPev->angles.z;

				// angles
				fprintf(entfile, "%i  %i  %i\n", orix, oriy, oriz);

				// rendermode, renderamt, renderfx
				fprintf(entfile, "%i  %f  %i\n", pPev->rendermode, pPev->renderamt, pPev->renderfx);

				orix = pPev->rendercolor.x;
				oriy = pPev->rendercolor.y;
				oriz = pPev->rendercolor.z;

				// rendercolor
				fprintf(entfile, "%i  %i  %i\n", orix, oriy, oriz);
			}
		}
	}
	
	fclose(entfile);

	//fscanf(stdin, "%d", &i)

}

*/

void AvReadEntsFromFile(entvars_t *pPev, char *filename)
{
	// last param:
	// a = append
	// w = write
	// r = read

	entfile=fopen(filename,"r");
	
	if (entfile == NULL) return;
	
	
	// Loads the ent data from the file.
	// Data is in standard ent data format

	int i = 1;
	int j = 1;

	int doit = 0;
	int docomp = 0;


	for (i = 1; i < 1025; i++)
	{
		EntData[i].justduped = 0;
	}
	i = 1;

	
	int num = 0;
	int totalnum = 0;
	
	KeyValueData	kvd;
	
	char buf[1024];
	char keyname[128];
	char keyvalue[256];
	char classname[80];
	char model[256];
	int modelindex = 0;

	int entid = 0;
	bool thr = 0;
	Vector afterAngles;
	Vector afterOrigin;
	Vector afterOrigin2;
	bool useafterorigin;
	bool useafterorigin2;


	edict_t *tEntity;

	int readtest = 1;

	while (readtest)
	{
		readtest = fscanf(entfile, "%[^\n]\n", &buf);

		doit = 0;
		docomp = 0;

		thr = 0;
		useafterorigin = 0;
		useafterorigin2 = 0;

		afterAngles = g_vecZero;
		afterOrigin = g_vecZero;
		afterOrigin2 = g_vecZero;

		if (readtest)
		{

			// ok, now lets see what we read. first line of a new ent should be "{"

			if (buf[0] == '/' && buf[1] == '/')
			{
				// comment, ignore
			}
			else if (buf[0] == 0)
			{
				// nothing
			}
			else if (buf[0] == '{')
			{
				// ok, now we are getting somewhere.
				// start reading in the keys of this ent

				while (readtest)
				{

					//readtest = fscanf(entfile, "\"%[^\"]\" \"%[^\"]\"", &keyname, &keyvalue);
					
					readtest = fscanf(entfile, "%[^\n]\n", &buf);
					
					//ALERT(at_console, "READ: %s\n", buf);

					if (readtest)
					{
						if (buf[0] == '/' && buf[1] == '/')
						{
							// comment, ignore
						}
						else if (buf[0] == 0)
						{
							// nothing
						}
						else if (buf[0] == '}')
						{

							// done the ent, now spawn it and stuff

							if (doit == 5)
							{

								SET_ORIGIN(ENT(tEntity), tEntity->v.origin );

								if (useafterorigin)
								{
									SET_ORIGIN(ENT(tEntity), afterOrigin );
								}


								if (FStrEq(classname, "race_camtrigger"))
								{
								}
								else if (FStrEq(classname, "race_campos"))
								{
								}
								else
								{
									DispatchSpawn(tEntity);
								}

								if (useafterorigin2)
								{
									SET_ORIGIN(ENT(tEntity), afterOrigin2 );
								}

								tEntity->v.angles = afterAngles;

								SET_ORIGIN(ENT(tEntity), tEntity->v.origin );
							}
							
							readtest = 0;
						}
						else if (buf[0] == '\"')
						{
							// scan for the value of the field
							sscanf(buf, "\"%[^\"]\" \"%[^\"]\"", &keyname, &keyvalue);
							
							//ALERT(at_console, "GOT KEY: %s = %s\n", keyname, keyvalue);
							if (FStrEq(keyname, "classname")) 
							{
								sprintf(classname, "%s", keyvalue);
								doit++;
							}
							else if (FStrEq(keyname, "entid"))
							{
								entid = atoi(keyvalue);
								doit++;
							}
							else if (FStrEq(keyname, "savemodelindex"))
							{
								modelindex = atoi(keyvalue);
								doit++;
							}
							else if (FStrEq(keyname, "savemodel"))
							{
								sprintf(model, "%s", keyvalue);
								doit++;
							}
							else
							{
								// doit must be complete
								docomp = 1;
							}
							
							
							if (FStrEq(keyname, "afterangles"))
							{
								afterAngles = UTIL_StringToVector( keyvalue );
							}
							else if (FStrEq(keyname, "origorigin"))
							{
								afterOrigin = UTIL_StringToVector( keyvalue );
								useafterorigin = 1;
							}
							else if (FStrEq(keyname, "origin"))
							{
								afterOrigin2 = UTIL_StringToVector( keyvalue );
								useafterorigin2 = 1;
							}

							if (doit == 5)
							{
								if (!FStrEq(keyname, "afterangles") && !FStrEq(keyname, "origorigin"))
								{
									
									// copy all the keys to this ent
									kvd.fHandled = FALSE;
									kvd.szClassName = classname;
									kvd.szKeyName = keyname;
									kvd.szValue = keyvalue;
									EntData[ENTINDEX(tEntity)].addingkey = 1;
									DispatchKeyValue( tEntity, &kvd );

								}
							}

							if ((doit == 4 || docomp) && doit < 5)
							{
								// time to see what to do with this ent
								// see if we can match it somewhere
								tEntity = INDEXENT ( entid );
								thr = 0;
								bool flag = 0;
								if (modelindex == 0)
								{

										flag = 1;	

								}

								if (tEntity && !FNullEnt(tEntity)) {
									// exists
									// make sure the classname matches
//									if (flag) ALERT(at_console, "EXISTS...\n");
									if (FStrEq(classname, (char*)STRING(tEntity->v.classname))) 
									{
									//	if (flag) ALERT(at_console, "CLASS MATCH...\n");
									
										if (FStrEq(model, (char*)STRING(tEntity->v.model)) || tEntity->v.modelindex == 0 ) 
										{
									//		if (flag) ALERT(at_console, "MODEL MATCH...\n");
									
											if (modelindex == tEntity->v.modelindex) 
											{
									//			if (flag) ALERT(at_console, "MODELINDEX MATCH...\n");

												// match, so lets use this one	
												if (EntData[entid].justduped == 0)  thr = 1;
											}
										
										}
									}
								}
								if (thr == 0)
								{
     								// create it
	
									if (FStrEq(classname, "race_camtrigger"))
									{

										tEntity = CamTriggerCreate(NULL);
									}
									else if (FStrEq(classname, "race_campos"))
									{

										tEntity = RaceCamCreate(NULL);
									}
									else
									{
										tEntity = CREATE_NAMED_ENTITY(MAKE_STRING(classname));
									}
									EntData[ENTINDEX(tEntity)].numkeys = 0;
									EntData[ENTINDEX(tEntity)].justduped = 1;
								}

								// ready!
								doit = 5;

								kvd.fHandled = FALSE;
								kvd.szClassName = NULL;
								kvd.szKeyName = "classname";
								kvd.szValue = classname;
								DispatchKeyValue( tEntity, &kvd );


							}







						}
					}

					if (feof(entfile)) readtest = 0;
				}
			}

			readtest = 1;

		}

		if (feof(entfile)) readtest = 0;

	}


	fclose(entfile);

/*

	ALERT(at_console, "There are %i ents.\n", totalnum);

	for (j=1; j < totalnum+1; j++) {

		ALERT(at_console, "Starting %i\n", j);


		// ent number, model index, classname
		test = fscanf(entfile, "%i  %i  %s\n", &num, &modelindex, &pClassname);
		test = fscanf(entfile, "%s\n", &pModel);
		//test = fscanf(entfile, "%s\n", &targname);
		//t/est = fscanf(entfile, "%s\n", &targ);

		ALERT(at_console, "Classname: %s\nModel: %s\nTargetname: %s\nTarget: %s\n", pClassname, pModel, targname, targ);
		
		// see if it exists...

		entvars_t *pNew;
		edict_t *tEntity;

		tEntity = INDEXENT ( num );

		thr = 0;

		if (tEntity && !FNullEnt(tEntity)) {
			// exists
			char *dClassname;
			dClassname = Av_GetClassname ( tEntity ); 

			if (FStrEq(pClassname, dClassname)) {
				
				thr = 1;
			}
		}

		if (EntData[num].justduped == 1) thr = 0; // This is an ent we just created.
		
		string_t classname = MAKE_STRING(pClassname);


		if (thr == 0)
		{
     		// create it
			tEntity = CREATE_NAMED_ENTITY(classname);
		}

		pNew =  VARS(tEntity);
		// Set the various values

		pNew->modelindex = modelindex;
		//pNew->model = MAKE_STRING(pModel);

		// Set classname shit here!

		EntModified[ENTINDEX(tEntity)] = 1;
		EntData[ENTINDEX(tEntity)].justduped = 1;
						
		sprintf(buf, "%s", pClassname);

		// Set the KEYVALUES here!
		kvd.fHandled = FALSE;
		kvd.szClassName = NULL;
		kvd.szKeyName = "classname";
		kvd.szValue = buf;
		DispatchKeyValue( tEntity, &kvd );
		kvd.fHandled = FALSE;
		
		kvd.szClassName = buf;

		sprintf(buf, "%s", pModel);

		kvd.szKeyName = "model";
		kvd.szValue = buf;
		DispatchKeyValue( tEntity, &kvd );
		

		// origin
		test = fscanf(entfile, "%i  %i  %i\n", &orix, &oriy, &oriz);
		neworig.x = orix;
		neworig.y = oriy;
		neworig.z = oriz;
		
		ALERT(at_console, "Origin: %i  %i  %i\n", orix, oriy, oriz);

			
		SET_ORIGIN(ENT(pNew), neworig );
		

		// angles
		test = fscanf(entfile, "%i  %i  %i\n", &angx, &angy, &angz);

		// rendermode, renderamt, renderfx
		test = fscanf(entfile, "%i  %f  %i\n", &pNew->rendermode, &pNew->renderamt, &pNew->renderfx);

		// rendercolor
		test = fscanf(entfile, "%i  %i  %i\n", &orix, &oriy, &oriz);
		
		pNew->rendercolor.x = orix;
		pNew->rendercolor.y = oriy;
		pNew->rendercolor.z = oriz;

		if (thr == 0) DispatchSpawn( ENT( pNew ) );
		
		
		pNew->angles.x = angx;
		pNew->angles.y = angy;
		pNew->angles.z = angz;
		

	}

	fclose(entfile);


	// Clear the JUSTDUPED flags

	for (j = 0; j < 1025; j++) {
		EntData[j].justduped = 0;
	}

	//fscanf(stdin, "%d", &i)
*/
}



void OldAvReadEntsFromFile(entvars_t *pPev, char *filename)
{
	// last param:
	// a = append
	// w = write
	// r = read

	entfile=fopen(filename,"r");
	
	if (entfile == NULL) return;
	
	
	// Loads the ent data from the file.

	int i = 1;
	int j = 1;
	
//	char msg[80];

	int doit = 0;
	char pClassname[80];
	char pModel[120];
	char targname[120];
	char targ[120];

	char *mClassname = NULL;

		
	int modelindex = 0;
	int num = 0;
	int totalnum = 0;
	
	Vector neworig = g_vecZero;
	KeyValueData	kvd;
	char buf[120];

	int thr = 0;

	int orix=0;
	int oriy=0;
	int oriz=0;


	int angx=0;
	int angy=0;
	int angz=0;

	int test = 0;

	// get total number
	test = fscanf(entfile, "%i\n", &totalnum);
	
	//sprintf(msg,"* There are %i , %i entries!\n", test);
	//ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

	//ALERT(at_console, "There are %i ents.\n", totalnum);

	for (j=1; j < totalnum+1; j++) {

		ALERT(at_console, "Starting %i\n", j);


		// ent number, model index, classname
		test = fscanf(entfile, "%i  %i  %s\n", &num, &modelindex, &pClassname);
		test = fscanf(entfile, "%s\n", &pModel);
		//test = fscanf(entfile, "%s\n", &targname);
		//t/est = fscanf(entfile, "%s\n", &targ);

		ALERT(at_console, "Classname: %s\nModel: %s\nTargetname: %s\nTarget: %s\n", pClassname, pModel, targname, targ);
		
		// see if it exists...

		entvars_t *pNew;
		edict_t *tEntity;

		tEntity = INDEXENT ( num );

		thr = 0;

		if (tEntity && !FNullEnt(tEntity)) {
			// exists
			char *dClassname;
			dClassname = Av_GetClassname ( tEntity ); 

			if (FStrEq(pClassname, dClassname)) {
				
				thr = 1;
			}
		}

		if (EntData[num].justduped == 1) thr = 0; // This is an ent we just created.
		
		string_t classname = MAKE_STRING(pClassname);


		if (thr == 0)
		{
     		// create it
			tEntity = CREATE_NAMED_ENTITY(classname);
		}

		pNew =  VARS(tEntity);
		// Set the various values

		pNew->modelindex = modelindex;
		//pNew->model = MAKE_STRING(pModel);

		// Set classname shit here!

		EntModified[ENTINDEX(tEntity)] = 1;
		EntData[ENTINDEX(tEntity)].justduped = 1;
						
		sprintf(buf, "%s", pClassname);

		// Set the KEYVALUES here!
		kvd.fHandled = FALSE;
		kvd.szClassName = NULL;
		kvd.szKeyName = "classname";
		kvd.szValue = buf;
		DispatchKeyValue( tEntity, &kvd );
		kvd.fHandled = FALSE;
		
		kvd.szClassName = buf;

		sprintf(buf, "%s", pModel);

		kvd.szKeyName = "model";
		kvd.szValue = buf;
		DispatchKeyValue( tEntity, &kvd );
		

		// origin
		test = fscanf(entfile, "%i  %i  %i\n", &orix, &oriy, &oriz);
		neworig.x = orix;
		neworig.y = oriy;
		neworig.z = oriz;
		
		ALERT(at_console, "Origin: %i  %i  %i\n", orix, oriy, oriz);

			
		SET_ORIGIN(ENT(pNew), neworig );
		

		// angles
		test = fscanf(entfile, "%i  %i  %i\n", &angx, &angy, &angz);

		// rendermode, renderamt, renderfx
		test = fscanf(entfile, "%i  %f  %i\n", &pNew->rendermode, &pNew->renderamt, &pNew->renderfx);

		// rendercolor
		test = fscanf(entfile, "%i  %i  %i\n", &orix, &oriy, &oriz);
		
		pNew->rendercolor.x = orix;
		pNew->rendercolor.y = oriy;
		pNew->rendercolor.z = oriz;

		if (thr == 0) DispatchSpawn( ENT( pNew ) );
		
		
		pNew->angles.x = angx;
		pNew->angles.y = angy;
		pNew->angles.z = angz;
		

	}

	fclose(entfile);


	// Clear the JUSTDUPED flags

	for (j = 0; j < 1025; j++) {
		EntData[j].justduped = 0;
	}

	//fscanf(stdin, "%d", &i)

}


void AvBeforeTouch( edict_t *pEntity )
{

	// See if this is an AUTOSPAWNER ENT, and if it is... set it so that next time it checks that it's still there.
	int n = ENTINDEX( pEntity ) ;
	int i = 0;
/*
	for (i; i < 21; i++) {
		if (n == AutoSpawnCurrEnt[i]) AutoSpawnCheck[i] = 1; // set it to check.
	}
*/	

}

void AvAfterTouch( )
{
/*
	edict_t *frontEnt;

	int i = 1;
	for (i; i < numAutoSpawners+1; i++) {

		frontEnt = INDEXENT ( AutoSpawnCurrEnt[i] );

		if (FNullEnt(frontEnt)) {

			AvMakeAutoEnt(i);
			//char msg[80];
			//sprintf(msg,"Remade the ent!\n");
			//ClientPrint( pPev, HUD_PRINTTALK, msg);
		}
	}
*/
}

void AvRunPrecache()
{
	// Precache the sprite we need for the funnel


	m_funnelSprite = PRECACHE_MODEL ( "sprites/flare6.spr" );
	
	//m_Line1 = PRECACHE_MODEL ( "sprites/line1.spr" );
	


	m_beamSprite = PRECACHE_MODEL ( "sprites/laserbeam.spr" );
	m_bubbleSprite = PRECACHE_MODEL ( "models/player/furry2/furry2.mdl" );
	
	// mortar sprite
	m_spriteTexture2 = PRECACHE_MODEL( "sprites/lgtning.spr" );
	PRECACHE_SOUND ("weapons/mortar.wav");
	PRECACHE_SOUND ("weapons/mortarhit.wav");
	
	#ifdef SZ_DLL
		PRECACHE_SOUND ("misc/r_tele2.wav");

		//PRECACHE_MODEL( "models/tree.mdl" );
		
		//PRECACHE_MODEL( "avatar-x/avadd1.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd2.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd3.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd4.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd5.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd6.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd7.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd8.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd14.avfil" );
		//PRECACHE_MODEL( "avatar-x/avadd15.avfil" );
		PRECACHE_MODEL( "avatar-x/avadd22.avfil" );
		PRECACHE_MODEL( "avatar-x/avadd23.avfil" );
		PRECACHE_SOUND( "../avatar-x/avadd9.avfil" ); // moo sound
	#endif

	// Xen tree stuff

	PRECACHE_SOUND ("zombie/claw_strike1.wav");
	PRECACHE_SOUND ("zombie/claw_strike2.wav");
	PRECACHE_SOUND ("zombie/claw_strike3.wav");
	
	PRECACHE_SOUND ("zombie/claw_miss1.wav");
	PRECACHE_SOUND ("zombie/claw_miss2.wav");

	m_funnelSprite = PRECACHE_MODEL ( "sprites/flare6.spr" );
	m_beamSprite = PRECACHE_MODEL ( "sprites/laserbeam.spr" );

	#ifdef SZ_DLL
		m_bubbleSprite = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_fireSprite = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_presentModel = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel1 = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel2 = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel3 = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel4 = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel5 = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel6 = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel7 = PRECACHE_MODEL ( "models/presentsm.mdl" );
		m_otherModel8 = PRECACHE_MODEL ( "models/presentsm.mdl" );

		/*
		PRECACHE_MODEL( "models/player/radish/radish.mdl" );
		PRECACHE_MODEL( "models/player/ribbon/ribbon.mdl" );

		PRECACHE_MODEL( "models/player/furry2/furry2.mdl" );
		PRECACHE_MODEL( "models/player/seppuko/seppuko.mdl" );
		PRECACHE_MODEL( "models/player/md/md.mdl" );
		PRECACHE_MODEL( "models/player/duncan/duncan.mdl" );
		PRECACHE_MODEL( "models/player/bone/bone.mdl" );
		PRECACHE_MODEL( "models/player/evh/evh.mdl" );
		PRECACHE_MODEL( "models/player/daffy/daffy.mdl" );
		PRECACHE_MODEL( "models/player/freaky/freaky.mdl" );
		PRECACHE_MODEL( "models/player/gg/gg.mdl" );
		PRECACHE_MODEL( "models/player/hedkase/hedkase.mdl" );
		// PRECACHE_MODEL( "models/player/seppdark/seppdark.mdl" );
		PRECACHE_MODEL( "models/player/shrek/shrek.mdl" );
		PRECACHE_MODEL( "models/player/rotten/rotten.mdl" );
		PRECACHE_MODEL( "models/player/barney/barney.mdl" );
		PRECACHE_MODEL( "models/player/hgrunt/hgrunt.mdl" );
		PRECACHE_MODEL( "models/player/rabid1/rabid1.mdl" );
		
		PRECACHE_MODEL( "models/player/aquafresh/aquafresh.mdl" );
		PRECACHE_MODEL( "models/player/aswat/aswat.mdl" );
		PRECACHE_MODEL( "models/player/badday/badday.mdl" );
		PRECACHE_MODEL( "models/player/cf/cf.mdl" );
		PRECACHE_MODEL( "models/player/gstc/gstc.mdl" );
		PRECACHE_MODEL( "models/player/irongiant/irongiant.mdl" );
		PRECACHE_MODEL( "models/player/jthm/jthm.mdl" );
		PRECACHE_MODEL( "models/player/kermit/kermit.mdl" );
		PRECACHE_MODEL( "models/player/scream/scream.mdl" );
		PRECACHE_MODEL( "models/player/slave/slave.mdl" );
		PRECACHE_MODEL( "models/player/spawn2/spawn2.mdl" );
		PRECACHE_MODEL( "models/player/yodan/yodan.mdl" );
		PRECACHE_MODEL( "models/player/mib/mib.mdl" );
		PRECACHE_MODEL( "models/player/pink/pink.mdl" );
		PRECACHE_MODEL( "models/player/grinch/grinch.mdl" );
		PRECACHE_MODEL( "models/player/monk/monk.mdl" );
		PRECACHE_MODEL( "models/player/robo/robo.mdl" );
		PRECACHE_MODEL( "models/player/scientist/scientist.mdl" );
		PRECACHE_MODEL( "models/player/gina/gina.mdl" );
		PRECACHE_MODEL( "models/player/gordon/gordon.mdl" );
		PRECACHE_MODEL( "models/player/recon/recon.mdl" );
		PRECACHE_MODEL( "models/player/zombie/zombie.mdl" );
		PRECACHE_MODEL( "models/player/dark/dark.mdl" );
		PRECACHE_MODEL( "models/player/tigger/tigger.mdl" );
		*/


	#endif
	/*

	m_funnelSprite = PRECACHE_MODEL ( "sprites/flare6.spr" );
	m_presentModel = PRECACHE_MODEL ( "models/presentsm.mdl" );
	
	m_beamSprite = PRECACHE_MODEL ( "sprites/laserbeam.spr" );
	m_bubbleSprite = PRECACHE_MODEL ( "sprites/bubble.spr" );
	m_fireSprite = PRECACHE_MODEL ( "sprites/playerflame.spr" );

	m_otherModel1 = PRECACHE_MODEL ( "models/presentlg.mdl" );
	m_otherModel2 = PRECACHE_MODEL ( "models/can.mdl" );
	m_otherModel3 = PRECACHE_MODEL ( "models/gasmask.mdl" );
	m_otherModel4 = PRECACHE_MODEL ( "models/rockgibs.mdl" );
	m_otherModel5 = PRECACHE_MODEL ( "models/scientist.mdl" );
	m_otherModel6 = PRECACHE_MODEL ( "models/sentry2.mdl" );
	m_otherModel7 = PRECACHE_MODEL ( "models/chumtoad.mdl" );
	m_otherModel8 = PRECACHE_MODEL ( "models/boid.mdl" );

	m_eyeModel = PRECACHE_MODEL ( "models/aveye.mdl" );
	*/

	AlreadyMadeModels = 0;
	MortarFieldEnabled = 0;
	
	
	AvAddClockPoints();

    int i = 0;

	for (i; i < 1025; i++) {
		EntModified[i] = 0;
		EntData[i].donttouchme = 0;
	}

	#ifdef SZ_DLL
		AvMakeModels();
	#endif
	// Fill the colour array


	int R = 255;
	int G = 0;
	int B = 0;
	int rr = 0;
	int gg = 5;
	int bb = 0;

	for (int x = 0; x <= 255; x++)
	{
		R = R + rr;
		G = G + gg;
		B = B + bb;
    
		if (G >= 255) {rr = -5; gg = 0;}
		if (R <= 0) {rr = 0; gg = -5; bb = 5;}
		if (B > 255) {bb = -5; rr = 5; gg = 0;}
		if (R >= 255 && B >= 255) { bb = -5; rr = 0; B = 254;}
		if (B <= 0 && R >= 255) { bb = 0;}
        
		if (R > 255) {R = 255; rr = -rr;}
		if (G > 255) {G = 255; gg = -gg;}
		if (B > 255) {B = 255; bb = -bb;}
    
		if (R < 0) {R = 0; rr = -rr;}
		if (G < 0) {G = 0; gg = -gg;}
		if (B < 0) {B = 0; bb = -bb;}
    
    
		ColourArray[x].x = R;
		ColourArray[x].y = G;
		ColourArray[x].z = B;
	}
}

void AvSpawnMazeParts()
{

	// create the maze
				fp=fopen("maze.txt","w");
				fclose(fp);		

	int x = 1;
	int y = 1;
	int actualx = 0;
	int actualy = 0;
	int actualz = 0;

	int cm = 0;
	int i = 1;

	char entname[30];
	edict_t *pent;

	for (cm = 1; cm <= totalmaze; cm++)
	{	
	
		fp=fopen("maze.txt","a");
		fprintf(fp, "Beginning Maze Spawn... ID: %i\n", cm);
		fclose(fp);	

		mazerootx[cm] = INDEXENT(mazerootent[cm])->v.origin.x;
		mazerooty[cm] = INDEXENT(mazerootent[cm])->v.origin.y;
		mazerootz[cm] = INDEXENT(mazerootent[cm])->v.origin.z;



		// first find the ents we need

		for (i = 1; i <= mazenumpiece[cm]; i++)
		{
			
			sprintf( entname, "piece%i-%i", mazepieceset[cm],  i);
			// look for an ent thats called "piece<i>"
			pent = FIND_ENTITY_BY_TARGETNAME( NULL, entname );

			if (!FNullEnt( pent ))
			{
				pentlist[mazepieceset[cm]][i] = ENTINDEX( pent );
			}

			fp=fopen("maze.txt","a");
			fprintf(fp, "%i - Found piece: %s\n", cm, entname);
			fclose(fp);	

		}


		fp=fopen("maze.txt","a");
		fprintf(fp, "%i - Found all the ents for piece set %i\n", cm, mazepieceset[cm]);
		fclose(fp);	

		// now, spawn the x pieces for each square in the grid
		edict_t *work;

		for (x = 1; x <= mazewidth[cm]; x++)
		{
			for (y = 1; y <= mazeheight[cm]; y++)
			{

				i = mazedefault[cm];

				fp=fopen("maze.txt","a");
				fprintf(fp, "%i - X: %i  Y: %i  -- Generating the ent index: %i\n", cm, x, y, pentlist[mazepieceset[cm]][i]);
				fclose(fp);	

				
				// Spawn the 8 pieces here.

				actualx = mazerootx[cm] + (256 * (x-1));
				actualy = mazerooty[cm] - (256 * (y-1));
				actualz = mazerootz[cm];

				

				// first dup it	
				
				work = AvMazeDup( INDEXENT( pentlist[mazepieceset[cm]][i] ), actualx, actualy, actualz);

				work->v.iuser1 = 120;

				// set which 

				// set it in the array

				mazedata[cm][x][y].piecelist = ENTINDEX( work );

				EntData[ ENTINDEX(work) ].mazex = x;
				EntData[ ENTINDEX(work) ].mazey = y;
				EntData[ ENTINDEX(work) ].mazeid = cm;
				
			}
		}
	}
	
		fp=fopen("maze.txt","a");
		fprintf(fp, "Maze Spawn Complete!\n");
		fclose(fp);	


}

edict_t *AvMazeDup( edict_t *dupFrom, int orix, int oriy, int oriz)
{

	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING(  STRING(dupFrom->v.classname)  ));
	
	entvars_t *pNew = VARS(tEntity);
	entvars_t *pev = VARS(dupFrom);
	// place this in front

	pNew->origin = pev->origin;
	pNew->origin.x = orix;
	pNew->origin.y = oriy;
	pNew->origin.z = oriz;

	pNew->modelindex = pev->modelindex;
	pNew->model = pev->model;

	pNew->solid = pev->solid;
	pNew->skin = pev->skin;
	pNew->body = pev->body;
	pNew->effects = pev->effects;
	pNew->takedamage = pev->takedamage;

	pNew->spawnflags = pev->spawnflags;
	pNew->flags = pev->flags;
	pNew->team = pev->team;
	pNew->dmg = pev->dmg;

	pNew->rendermode = pev->rendermode;
	pNew->renderamt = pev->renderamt;
	pNew->renderfx = pev->renderfx;
	pNew->rendercolor = pev->rendercolor;

	DispatchSpawn( ENT( pNew ) );

	pNew->angles.x = pev->angles.x;
	pNew->angles.y = pev->angles.y;
	pNew->angles.z = pev->angles.z;

	return tEntity;
}


void AvLinearMove( edict_t *pEntity , Vector vecDest, float flSpeed )
{

	// Perform a linear move
	
	// get the origin...

	Vector origin = (pEntity->v.absmin + pEntity->v.absmax)* 0.5;

	// Already there?
	if (vecDest == origin)
	{
		return;
	}
		
	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDest - origin;
	
	// divide vector length by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to LinearMoveDone when dest is reached
	pEntity->v.nextthink = pEntity->v.ltime + flTravelTime;
	
	// scale the destdelta vector by the time spent traveling to get velocity
	pEntity->v.velocity = vecDestDelta / flTravelTime;


	// Finally, set an AV FUNCTION here: Tell my code to zero the vector.
	pEntity->v.fuser3 = 2;


}

void AvStartFrame()
{
	// Nothing here!
	beamcount = 0;
}

void AvSpawnTakegun()
{
	
	KeyValueData	kvd;
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_tfgoal"));
	

	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "info_tfgoal";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szClassName = "info_tfgoal";
	kvd.szKeyName = "origin";
	kvd.szValue = "-4000 4000 4000";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "wait";
	kvd.szValue = "0.1";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "remove_item_group";
	kvd.szValue = "5601";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "has_item_from_group";
	kvd.szValue = "5601";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "netname";
	kvd.szValue = "takegun";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "message";
	kvd.szValue = "The accessory has been removed!";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "g_e";
	kvd.szValue = "1";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "g_a";
	kvd.szValue = "1";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "goal_state";
	kvd.szValue = "2";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	EntData[ENTINDEX( tEntity )].donttouchme = 1;

	DispatchSpawn( tEntity );

	// This ent is now spawned.
}


void AvSpawnRemove()
{
	
	KeyValueData	kvd;
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_tfgoal"));
	

	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "info_tfgoal";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szClassName = "info_tfgoal";
	kvd.szKeyName = "origin";
	kvd.szValue = "-4000 4000 4000";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "wait";
	kvd.szValue = "0.1";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "remove_item_group";
	kvd.szValue = "5600";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "has_item_from_group";
	kvd.szValue = "5600";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "netname";
	kvd.szValue = "remove";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "message";
	kvd.szValue = "The model has been removed!";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "g_e";
	kvd.szValue = "1";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "g_a";
	kvd.szValue = "1";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "goal_state";
	kvd.szValue = "2";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	EntData[ENTINDEX( tEntity )].donttouchme = 1;

	DispatchSpawn( tEntity );

	// This ent is now spawned.
}


void AvSpawnOneModel(int GoalNum, char *Netname, char *Message, int GroupNum, char *ModelFile, char *Items, char *SpeedReduc, int Body, int Skin)
{
	
	KeyValueData	kvd;
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("item_tfgoal"));
	
	char buf[120];


	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "item_tfgoal";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szClassName = "item_tfgoal";
	kvd.szKeyName = "origin";
	kvd.szValue = "-4000 4000 4000";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%s", ModelFile);

	kvd.szKeyName = "mdl";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%i", GroupNum);

	kvd.szKeyName = "group_no";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "hasnt_item_from_group";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%s", Items);

	kvd.szKeyName = "items";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%s", SpeedReduc);

	kvd.szKeyName = "speed_reduction";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%i", GoalNum);

	kvd.szKeyName = "goal_no";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%s", Netname);

	kvd.szKeyName = "netname";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%s", Message);

	kvd.szKeyName = "message";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "g_e";
	kvd.szValue = "1";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "g_a";
	kvd.szValue = "1212";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	kvd.szKeyName = "goal_state";
	kvd.szValue = "2";
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;
	
	sprintf(buf, "%i", Body);
	kvd.szKeyName = "body";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	sprintf(buf, "%i", Skin);
	kvd.szKeyName = "skin";
	kvd.szValue = buf;
	DispatchKeyValue( tEntity, &kvd );
	kvd.fHandled = FALSE;

	DispatchSpawn( tEntity );

	EntData[ENTINDEX( tEntity )].donttouchme = 1;
	// This ent is now spawned.


}

void AvMakeModels()
{

	return; // disabled for now

}


void AvPutInServer( edict_t *pEntity )
{
	int MyNumber = ENTINDEX(pEntity);
	AdminLoggedIn[MyNumber] = 0;
	
	//HasEye[MyNumber] = 0;
	//next_eye[MyNumber] =0;

	PlayerInfo[MyNumber].noPlayers  = 0;
	PlayerInfo[MyNumber].hasSpecialMode  = 0;
	PlayerInfo[MyNumber].superBright  = 0;
	PlayerInfo[MyNumber].privchat  = 0;
	sprintf(EntData[MyNumber].digitgroup, "none");
	EntData[MyNumber].leash_master = NULL;

	checkMapSpecificModels();
	
	// Reset the weapons!

	PlyWepDisabled[ MyNumber ] = 0;
	WepSelected[ MyNumber ]  = 0;

	
	PlayerInfo[MyNumber].tripmineR = 0;
	PlayerInfo[MyNumber].tripmineG = 214;
	PlayerInfo[MyNumber].tripmineB = 198;

	// mode 1 - EVIL KILLER MODE
	PlayerInfo[MyNumber].tripMode = 0;


//	int hiswon = pfnGetPlayerWONId( pEntity );
	
}

void checkMapSpecificModels()
{

	
		if (stristr((char *)STRING(gpGlobals->mapname), "dha_pacman_b4"))
		{

			// Assign models to all players.

			ALERT(at_console, "MAP DO DO\n");

			int i;
			int ghostcount = 0;

			edict_t *pPlayerEdict;
			
			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{

						char *pClassname;
						pClassname = Av_GetClassname ( pPlayerEdict ); 
						if (FStrEq(pClassname, "player")) // make sure its actually a player.
						{
							

							if (pPlayerEdict->v.health > 0 && pPlayerEdict->v.team > 0)
							{

	
								ALERT(at_console, "TEAM: %i", pPlayerEdict->v.team);

								// blue -- pacman
								if (pPlayerEdict->v.team == 1)
								{	
									
									sprintf( playerModels[ENTINDEX(pPlayerEdict)].text , "%s", "pacman" );
									pPlayerEdict->v.skin = 0;
								}
								else if (pPlayerEdict->v.team == 2)
								{	
									// Make sure these people ARENT ghosts.

									if (FStrEq(playerModels[ENTINDEX(pPlayerEdict)].text, "pac_ghost") || FStrEq(playerModels[ENTINDEX(pPlayerEdict)].text, "pacman"))
									{
										sprintf( playerModels[ENTINDEX(pPlayerEdict)].text , "%s", "kermit" );
									}
								
								}
								else if (pPlayerEdict->v.team == 3)
								{	

									sprintf( playerModels[ENTINDEX(pPlayerEdict)].text , "%s", "pac_ghost" );
									pPlayerEdict->v.skin = ghostcount;
									ghostcount++;
								}

								g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pPlayerEdict) , g_engfuncs.pfnGetInfoKeyBuffer( pPlayerEdict ), "model", playerModels[ENTINDEX(pPlayerEdict)].text );

							}
						}
					}
				
			}	


			
			
			//pPev->body = atoi(arg2);
			



		}
	



}

void AvModelChange( edict_t *pEntity, entvars_t *pPev, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int MyNumber)
{
	// Change this fewls model.

	// First see how many points he has...


	char msg[80];

	int numPoints = 0;
	numPoints = pPev->iuser4;

	if (FStrEq(arg2, "give")) {
		pPev->iuser4 = 1000000;
		numPoints = pPev->iuser4;
	}

	if (pPev->playerclass == 8)
	{
		sprintf(msg,"* Sorry, you can't use models as a spy!\n");
		ClientPrint( pPev, HUD_PRINTTALK, msg);
		return;

	}

	if (false) // no longer require points
	{

		sprintf(msg,"* Sorry, but you need 8000 points to access the models!\n");
		ClientPrint( pPev, HUD_PRINTTALK, msg);
		
		return;
	}	
	else
	{
		// check if the model he wants is a reserved one...
		int hiswon = pfnGetPlayerWONId( pEntity );

		if (FStrEq(arg3, "av")) hiswon = AV_MODEL_AV;

		int allowed = 0;
		int notallowed = 0;
		
		if (FStrEq(arg1, "remove"))
		{

			pPev->vuser3.x = 0;
			pPev->vuser3.y = 0;

			return;
		}

		int specskin = 0;

		char usemodel[80];
		sprintf (usemodel, "%s", arg1);

		branded = 0;
		if (CVAR_GET_FLOAT("fox_branded")) branded = 1;
		branded = 0;


		//if (hiswon == AV_MODEL_AV) {
			// always allowed.
			// allowed = 1;
			if (FStrEq(arg1, "foxdie")) { allowed = 1; specskin = 0; sprintf(usemodel, "furry2");}
			if (FStrEq(arg1, "wolf")) { allowed = 1; specskin = 2; sprintf(usemodel, "furry2");}
			if (FStrEq(arg1, "avwolf")) { allowed = 1; specskin = 3; sprintf(usemodel, "furry2");}
			if (FStrEq(arg1, "seppdark")) { allowed = 1; specskin = 1; sprintf(usemodel, "seppuko");}
			if (FStrEq(arg1, "blackfox")) { allowed = 1; specskin = 4; sprintf(usemodel, "furry2");}

		//}

		else if (FStrEq(arg1, "foxdie")		/*&& hiswon == 851333*/ && branded == 0) { allowed = 1; specskin = 0; sprintf(usemodel, "furry2");}
		
		else if (FStrEq(arg1, "md")			/*&& hiswon == 196029*/) allowed = 1;
		else if (FStrEq(arg1, "duncan")		/*&& hiswon == 412124*/) allowed = 1;

		else if (FStrEq(arg1, "bone")		/*&& hiswon == 110327*/) allowed = 1;
		else if (FStrEq(arg1, "bone")		/*&& hiswon == 251930*/) allowed = 1;
		else if (FStrEq(arg1, "evh")		/*&& hiswon == 808757*/) allowed = 1;
		else if (FStrEq(arg1, "evh")		/*&& hiswon == 553476*/) allowed = 1;
		else if (FStrEq(arg1, "daffy")		/*&& hiswon == 42354*/) allowed = 1;
		else if (FStrEq(arg1, "freaky")		/*&& hiswon == 643387*/) allowed = 1;
		else if (FStrEq(arg1, "freaky")		/*&& hiswon == 321009*/) allowed = 1;
		else if (FStrEq(arg1, "freaky")		/*&& hiswon == 18913*/) allowed = 1;
		else if (FStrEq(arg1, "gg")			/*&& hiswon == 918587*/) allowed = 1;
		else if (FStrEq(arg1, "gg")			/*&& hiswon == 83960*/) allowed = 1;
		else if (FStrEq(arg1, "hedkase")	/*&& hiswon == 145280*/) allowed = 1;
		else if (FStrEq(arg1, "seppuko")	/*&& hiswon == 48003*/) { allowed = 1; specskin = 0;}
		else if (FStrEq(arg1, "seppdark")	/*&& hiswon == 48003*/) { allowed = 1; specskin = 1; sprintf(usemodel, "seppuko");}
		else if (FStrEq(arg1, "shrek")		/*&& hiswon == 297467*/) allowed = 1;
		else if (FStrEq(arg1, "shrek")		/*&& hiswon == 1015348*/) allowed = 1;
		else if (FStrEq(arg1, "radish")		/*&& hiswon == AV_WONID_RADISH*/) allowed = 1;	
		else if (FStrEq(arg1, "wolf")		/*&& hiswon == 150973*/) { allowed = 1; specskin = 2; sprintf(usemodel, "furry2");}			// dark destroyer
		else if (FStrEq(arg1, "rotten")		/*&& hiswon == 38692*/) allowed = 1;			// rotten
		else if (FStrEq(arg1, "barney")		/*&& hiswon == 115657*/) allowed = 1;			// silent
		else if (FStrEq(arg1, "hgrunt")		/*&& hiswon == 94142*/) allowed = 1;			// cell
		else if (FStrEq(arg1, "rabid1")		/*&& hiswon == 140487*/) allowed = 1;			// rabid llama
		

		// now see if its a non-restricted model.
		if (allowed == 0) {
			
			if (FStrEq(arg1, "aquafresh")) allowed = 1;
			else if (FStrEq(arg1, "aswat")) allowed = 1;
			else if (FStrEq(arg1, "badday")) allowed = 1;
			else if (FStrEq(arg1, "cf")) allowed = 1;
			else if (FStrEq(arg1, "gstc")) allowed = 1;
			else if (FStrEq(arg1, "irongiant")) allowed = 1;
			else if (FStrEq(arg1, "jthm")) allowed = 1;
			else if (FStrEq(arg1, "kermit")) allowed = 1;
			else if (FStrEq(arg1, "ribbon")) allowed = 1;
			else if (FStrEq(arg1, "scream")) allowed = 1;
			else if (FStrEq(arg1, "slave")) allowed = 1;
			else if (FStrEq(arg1, "spawn2")) allowed = 1;
			else if (FStrEq(arg1, "yodan")) allowed = 1;
			else if (FStrEq(arg1, "mib")) allowed = 1;
			else if (FStrEq(arg1, "pink")) allowed = 1;
			else if (FStrEq(arg1, "grinch")) allowed = 1;
			else if (FStrEq(arg1, "monk")) allowed = 1;
			else if (FStrEq(arg1, "robo")) allowed = 1;
			else if (FStrEq(arg1, "scientist")) allowed = 1;
			else if (FStrEq(arg1, "gina")) allowed = 1;
			else if (FStrEq(arg1, "gordon")) allowed = 1;
			else if (FStrEq(arg1, "recon")) allowed = 1;
			else if (FStrEq(arg1, "zombie")) allowed = 1;
			else if (FStrEq(arg1, "dark")) allowed = 1;
			else if (FStrEq(arg1, "tigger")) allowed = 1;




		}

		// if it's STILL 0...

		if (allowed == 0) {
			sprintf(msg,"* Sorry, but this model is restricted or was not found. You cannot use it.\n");
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}
		else
		{

			pPev->fuser1 = 1;
			pPev->vuser3.x = 1;

			g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity) , g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", (char *)usemodel );
			
			sprintf( playerModels[ENTINDEX(pEntity)].text , "%s", (char*)usemodel );
			
			//pPev->body = atoi(arg2);
			pPev->skin = specskin;

			// Move him to this model.
			
			checkMapSpecificModels();

			/*
			int i = 1;
			edict_t *frontEnt;
			entvars_t *pModelPev;

			int mdlfound = 0;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pModelPev =  VARS(frontEnt);
					if (FStrEq((char *)STRING(pModelPev->netname), arg1)) {
						// Touch this ent.
						mdlfound = 1;
						(*other_gFunctionTable.pfnTouch)(frontEnt, pEntity);
					}
				}
			}

			if (mdlfound == 0) {
				sprintf(msg,"* The model was not found. It may not be loaded in this map.\n");
				ClientPrint( pPev, HUD_PRINTTALK, msg);
				return;
			}
			else {
				
				// we may need to set them into INVIS or remove them from INVIS.

				//if allowed is 1, we set invis. If allowed is 2, we remove invis.

				if (allowed == 1) {

					pPev->rendermode = kRenderTransTexture;
					pPev->renderamt = 1;
					pPev->renderfx = 0;

				}
				else if (allowed == 2) {

					pPev->rendermode = 0;
					//pPev->renderamt = 0;
					pPev->renderfx = 0;

				}

				// all done.
				sprintf(msg,"* The model has been set to you!\n");
				ClientPrint( pPev, HUD_PRINTTALK, msg);
			}
*/


		}
	}
}

void AvTouch (edict_t *pentUsed, edict_t *pentOther)
{
/*
	char *netname;

	netname = (char *)STRING(pentUsed->v.netname);

	entvars_t *pModelPev;
	pModelPev = VARS(pentOther);

	char msg[80];
	
	// Check NAME here
	if ( strstr(netname, "loadent" )) {
		// load the ents.
		
		sprintf(msg,"* Loaded the map! Go play now!\n");
		ClientPrint( pModelPev, HUD_PRINTTALK, msg);

		char pFilename[80];
		sprintf(pFilename, "%s.edt", netname);
		
		AvReadEntsFromFile( VARS( pentOther ), pFilename );

	}
*/



}

void AvAttachEye (edict_t *pEntity, int MyNumber)
{
		
	// Attach the eye here.
	
	entvars_t *pModelPev;
	pModelPev = VARS(pEntity);
	

	if (HasEye[MyNumber] == 0 && pModelPev->iuser1) {


		MESSAGE_BEGIN( MSG_ALL, SVC_TEMPENTITY );


			WRITE_BYTE( TE_PLAYERATTACHMENT );
			WRITE_BYTE( MyNumber );
			WRITE_COORD( 0 );
			WRITE_SHORT( m_eyeModel );
			WRITE_SHORT( 110 );


		MESSAGE_END();
		
	}

	HasEye[MyNumber] = 0;
	
}
void AvRemoveEye (edict_t *pEntity, int MyNumber)
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_KILLPLAYERATTACHMENTS );
		WRITE_BYTE( MyNumber );

	MESSAGE_END();

	HasEye[MyNumber] = 0;

}

void AvAfterCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6)
{
}


void AvSwitchFront( edict_t *pEntity )
{
	
	// Rotate the front maze ent
	edict_t *frontEnt = Av_GetFrontEnt ( pEntity );
	
	if (!FNullEnt( frontEnt ))
	{


		int i = ENTINDEX( frontEnt );

		int x = EntData[i].mazex;
		int y = EntData[i].mazey;
		int id = EntData[i].mazeid ;

		if (x > 0 && y > 0)
		{
			
			if (frontEnt->v.iuser1 == 120)
			{

				// switch this ent around.

				int currone = mazedata[id][x][y].currpiece;
				// hide this one

				//frontEnt->v.effects |= EF_NODRAW;
				//frontEnt->v.solid = SOLID_NOT;

				currone++;

				if (currone > mazenumpiece[id]) currone = 1;

				edict_t *newone = INDEXENT(pentlist[mazepieceset[id]][currone]);

				SET_MODEL( frontEnt, STRING(newone->v.model) );
				frontEnt->v.modelindex = newone->v.modelindex;


				//INDEXENT(mazedata[x][y].piecelist[currone])->v.effects &= ~EF_NODRAW;
				//INDEXENT(mazedata[x][y].piecelist[currone])->v.solid = SOLID_BSP;
				//INDEXENT(mazedata[x][y].piecelist[currone])->v.angles.y = currrot;

				mazedata[id][x][y].currpiece = currone;

			}
		}
	}
}

void AvRotFront( edict_t *pEntity )
{

	// Rotate the front maze ent
	edict_t *frontEnt = Av_GetFrontEnt ( pEntity );
	
	if (!FNullEnt( frontEnt ))
	{


		int i = ENTINDEX( frontEnt );

		int x = EntData[i].mazex;
		int y = EntData[i].mazey;
		int id = EntData[i].mazeid ;

		if (x > 0 && y > 0)
		{
			
			if (frontEnt->v.iuser1 == 120)
			{

				// switch this ent around.

				int currrot = mazedata[id][x][y].rotation;
				// hide this one

				currrot = currrot + 90;

				if (currrot > 270) currrot = 0;

				frontEnt->v.angles.y = currrot;
				
				mazedata[id][x][y].rotation = currrot;

			}
		}
	}
}


void AvCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9)
{

	// Before we do ANYTHING ELSE, check if either the password matches 
	// or the wonid matches or the user is already logged in.

	int MyNumber = ENTINDEX(pEntity);
	char msg[80];
	entvars_t *pPev;
	pPev =  VARS(pEntity);
	
	#ifdef SZ_DLL
		if (FStrEq(pcmd, "mdlc"))
		{
			// Run the model change function!
			AvModelChange( pEntity, pPev, arg1, arg2, arg3, arg4, arg5, arg6, MyNumber);
		}

		if (FStrEq(pcmd, "switchfront"))
		{
			AvSwitchFront( pEntity );
		}
		if (FStrEq(pcmd, "rotfront"))
		{
			AvRotFront( pEntity );
		}

	#endif

	if (FStrEq(pcmd, "playercount"))
	{
		sprintf(msg, "Players in Server: %i   Players using Reserved Slots: %i\n", AvCountPlayers2(), AvCountPlayers());
		ClientPrint( pPev, HUD_PRINTTALK, msg);
	}
	
	if (FStrEq(pcmd, "msg") && AdminLoggedIn[MyNumber])
	{
		char sbuf0[150];
		sprintf(sbuf0, CMD_ARGS());
		UTIL_HudMessageColAll(  sbuf0 );
	}

	if (FStrEq(pcmd, "dismount") && AdminLoggedIn[MyNumber]) // mount the llama
	{
		int hiswon = pfnGetPlayerWONId( pEntity );
		if (hiswon == AV_WONID_AV || true)
		{
			// get off
			Dismount();

		}
	}


	if (FStrEq(pcmd, "mount") && AdminLoggedIn[MyNumber]) // mount the llama
	{
		int hiswon = pfnGetPlayerWONId( pEntity );
		if (hiswon == AV_WONID_AV || true)
		{
			// find the llama!
			
			bool foundllama = 0;

			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) 
				{
					pRunOnPev =  VARS(frontEnt);
					
					if ((pRunOnPev->origin - pPev->origin).Length() <= 200) 
					{
						// Only find llama's within 200 units of me
						if (FStrEq("player", (char*)STRING(pRunOnPev->classname))) 
						{
							
							if (pRunOnPev->deadflag == DEAD_NO)
							{
								if (pRunOnPev->playerclass > 0)
								{
									hiswon = pfnGetPlayerWONId( frontEnt );

									if (hiswon == 140487) // rabid llama
									{
										foundllama = 1;

										// code to MOUNT the llama.


										// change llama's model to the riding one
										pRunOnPev->fuser1 = 1;
										pRunOnPev->vuser3.x = 1;
										g_engfuncs.pfnSetClientKeyValue( ENTINDEX(frontEnt) , g_engfuncs.pfnGetInfoKeyBuffer( frontEnt ), "model", "rabid2" );
										sprintf( playerModels[ENTINDEX(frontEnt)].text , "%s", "rabid2" );
										pRunOnPev->body = 0;
										pRunOnPev->skin = 0;

										// change AV to INVISIBLE, GOD, and NOT SOLID

										pEntity->v.rendermode = 2;
										pEntity->v.renderamt = 0;
										pEntity->v.solid = SOLID_NOT;
										pEntity->v.takedamage = DAMAGE_NO;

										// change llama to GOD
										frontEnt->v.takedamage = DAMAGE_NO;
			
										onLlama = 1;
										llama = frontEnt;
										rider = pEntity;
										
										// prevent AV from moving

										pEntity->v.maxspeed = 1;
										pEntity->v.movetype = MOVETYPE_FLY;
										pEntity->v.gravity = 0.00001;

										llama->v.maxspeed = 300;		

										sprintf(msg,"* Avatar-X is now riding you!\n");
										ClientPrint( VARS(llama), HUD_PRINTTALK, msg);
										sprintf(msg,"* You are now riding your llama!\n");
										ClientPrint( VARS(rider), HUD_PRINTTALK, msg);

										return;										
									}
								}
							}
						}
					}
				}
			}
			if (foundllama == 0)
			{
				sprintf(msg,"* Sorry, but the llama could not be found.\n");
				ClientPrint( pPev, HUD_PRINTTALK, msg);
				return;
			}

		}
		else
		{
			sprintf(msg,"* Sorry, but this command is for Av Only.\n");
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}

	}

	// changing of tripmine color
	if (FStrEq(pcmd, "tripcolor"))
	{

		if (PlayerInfo[MyNumber].numPoints < POINTS_TRIPMINE_COLOR_COST)
		{

			sprintf(msg,"* Sorry, but you don't have enough points to use this command.\n");
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}
		
		if (FStrEq(arg1, "red"))
		{

			PlayerInfo[MyNumber].tripmineR = 255;
			PlayerInfo[MyNumber].tripmineG = 0;
			PlayerInfo[MyNumber].tripmineB = 0;
		}
		else if (FStrEq(arg1, "green"))
		{

			PlayerInfo[MyNumber].tripmineR = 0;
			PlayerInfo[MyNumber].tripmineG = 255;
			PlayerInfo[MyNumber].tripmineB = 0;
		}
		else if (FStrEq(arg1, "blue"))
		{

			PlayerInfo[MyNumber].tripmineR = 0;
			PlayerInfo[MyNumber].tripmineG = 0;
			PlayerInfo[MyNumber].tripmineB = 255;
		}
		else if (FStrEq(arg1, "yellow"))
		{

			PlayerInfo[MyNumber].tripmineR = 255;
			PlayerInfo[MyNumber].tripmineG = 255;
			PlayerInfo[MyNumber].tripmineB = 0;
		}
		else if (FStrEq(arg1, "pink"))
		{

			PlayerInfo[MyNumber].tripmineR = 255;
			PlayerInfo[MyNumber].tripmineG = 0;
			PlayerInfo[MyNumber].tripmineB = 128;
		}
		else if (FStrEq(arg1, "violet"))
		{

			PlayerInfo[MyNumber].tripmineR = 255;
			PlayerInfo[MyNumber].tripmineG = 0;
			PlayerInfo[MyNumber].tripmineB = 255;
		}
		else if (FStrEq(arg1, "purple"))
		{

			PlayerInfo[MyNumber].tripmineR = 128;
			PlayerInfo[MyNumber].tripmineG = 0;
			PlayerInfo[MyNumber].tripmineB = 128;
		}
		else if (FStrEq(arg1, "cyan"))
		{

			PlayerInfo[MyNumber].tripmineR = 0;
			PlayerInfo[MyNumber].tripmineG = 255;
			PlayerInfo[MyNumber].tripmineB = 255;
		}
		else if (FStrEq(arg1, "white"))
		{

			PlayerInfo[MyNumber].tripmineR = 255;
			PlayerInfo[MyNumber].tripmineG = 255;
			PlayerInfo[MyNumber].tripmineB = 255;
		}
		else
		{
			int r = atoi(arg1);
			int g = atoi(arg2);
			int b = atoi(arg3);


			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			
			if ((r + g + b) / 3 < 50)
			{
				r += 50;
				g += 50;
				b += 50;
			}

			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;

			PlayerInfo[MyNumber].tripmineR = r;
			PlayerInfo[MyNumber].tripmineG = g;
			PlayerInfo[MyNumber].tripmineB = b;
		}

		sprintf(msg,"* Tripmine Colour set to RGB %i %i %i.\n", PlayerInfo[MyNumber].tripmineR, PlayerInfo[MyNumber].tripmineG, PlayerInfo[MyNumber].tripmineB);
		ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
	
		return;
	}

	//
	// changing of tripmine mode
	if (FStrEq(pcmd, "tripmode"))
	{

		if (PlayerInfo[MyNumber].numPoints < POINTS_TRIPMINE_EVILMODE)
		{

			sprintf(msg,"* Sorry, but you don't have enough points to use this command.\n");
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}

		if (FStrEq(arg1, "evil"))
		{
			PlayerInfo[MyNumber].tripMode = 1;
			
			sprintf(msg,"* Tripmines set to EVIL mode.\n");
			ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
			return;
		}
		else if (FStrEq(arg1, "soliddd"))
		{
			// solid mode - 10
			PlayerInfo[MyNumber].tripMode = 10;
			
			sprintf(msg,"* Tripmines set to SOLID mode.\n");
			ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
			return;
		}
		else if (FStrEq(arg1, "push"))
		{
			// solid mode - 10
			PlayerInfo[MyNumber].tripMode = 20;
			
			sprintf(msg,"* Tripmines set to PUSHING mode.\n");
			ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
			return;
		}
		else if (FStrEq(arg1, "pull"))
		{
			// solid mode - 10
			PlayerInfo[MyNumber].tripMode = 30;
			
			sprintf(msg,"* Tripmines set to PULLING mode.\n");
			ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
			return;
		}
		else
		{
			PlayerInfo[MyNumber].tripMode = 0;
			
			sprintf(msg,"* Tripmines set to NORMAL mode.\n");
			ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
			return;
		}

		return;
	}



	if (!FStrEq(pcmd, "avc")) return;







	if (!AdminLoggedIn[MyNumber]) // not logged in
	{
		// see if we autologin?
		
		int loginnow = 0;

		if (FStrEq(arg1, "magic")) loginnow = 1;
		
		// now, check WON ID.

		// unsigned int pfnGetPlayerWONId( edict_t *e );
		int hiswon = pfnGetPlayerWONId( pEntity );
	
		// Current ops: BILLDOOR, AVATAR-X, FREAKY, INVICTUS

		if (hiswon == AV_WONID_AV)		loginnow = 2;		// av
		if (hiswon == AV_WONID_FREAKY)	loginnow = 2;		// freaky
		if (hiswon == AV_WONID_JEFFR)	loginnow = 2;	
		if (hiswon == AV_WONID_RADISH)	loginnow = 2;	
		if (hiswon == AV_WONID_CROWN)	loginnow = 2;	
		//if (hiswon == 48003)			loginnow = 2;		// sepp
		if (hiswon == 251930)			loginnow = 2;		// phat
		//if (hiswon == 139505)			loginnow = 2;		// baba
		if (hiswon == 406619)			loginnow = 2;		// maul 1
		if (hiswon == 101617)			loginnow = 2;		// maul 2
		if (hiswon == 473594)			loginnow = 2;		// ceown
		if (hiswon == 196029)			loginnow = 2;		// MattDavis
		if (hiswon == 488087)			loginnow = 2;		// rabid1
		if (hiswon == 140487)			loginnow = 2;		// rabid2
		if (hiswon == 851333)			loginnow = 2;		// foxdie
		if (hiswon == 180279)			loginnow = 2;		// voogru
		if (hiswon == 521805)			loginnow = 2;		// king kahuna
		if (hiswon == 115657)			loginnow = 2;		// Silent Warrior
		
		// ADDED JUL 30, 2017 - MAKES EVERYONE AN ADMIN
		loginnow = 2;


		if (hiswon == AV_WONID_AV)
		{
			// play news flash on radio
			int i = 1;
			char *pClassname;
			edict_t *frontEnt;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pClassname = (char *)STRING(frontEnt->v.classname); 
					if (FStrEq("building_radio", pClassname)) {
						// play news flash
						EMIT_SOUND_DYN2(frontEnt, CHAN_WEAPON, "avatar-x/avjoins.wav", 0.98, ATTN_STATIC, 0, 100); 
					}
				}
			}
		}

		if (loginnow > 0) {
			
			// log this admin in!
			
			AdminLoggedIn[MyNumber] = 1;

			// clear all his vars.

			int i = 0;
			int j = 0;

			for (j; j < 6; j++) {
				for (i; i < 1025; i++) {
					AdminMems[MyNumber][j][i] = 0;
				}
			}

			AdminPlayerMems[MyNumber] = 0;
			AdminLastTele[MyNumber] = 0;
			AdminFlags[MyNumber] = 0;

			sprintf(msg,"* YOU ARE NOW LOGGED IN TO Avatar-X's Commands!\n");
			ClientPrint( pPev, HUD_PRINTTALK, msg);

			#ifdef SZ_DLL
				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pEntity );
					WRITE_BYTE( 27 );
				MESSAGE_END();
				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pEntity );
					WRITE_BYTE( 28 );
				MESSAGE_END();
				pPev->weapons |= (1<<27);
				pPev->weapons |= (1<<28);
			#endif

			if (loginnow == 1) return;
		}
		else {
			sprintf(msg,"* SORRY BUT THIS IS NOT FOR YOU!\n");
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}
	}
	

    // LOG TO FILE


	struct tm *tmTime;
	time_t tTime;
	
	tTime=time(NULL);
	tmTime=localtime(&tTime);

	fp=fopen("avc.log","a");
	fprintf(fp, "[%i:%i:%i %i/%i/%i] %s used %s\n", (int)tmTime->tm_hour, (int)tmTime->tm_min, (int)tmTime->tm_sec, (int)tmTime->tm_mday, ((int)tmTime->tm_mon + 1), ((int)tmTime->tm_year + 1900) , STRING(pEntity->v.netname), CMD_ARGS() );
	fclose(fp);	


	int Cmd = 0;
		

	// New ENT COMMAND SYSTEM
	//Figure out what command it is.

	if (FStrEq(arg1, "emove"))			Cmd = AV_ENT_MOVE;
	if (FStrEq(arg1, "emoverel"))		Cmd = AV_ENT_MOVEREL;
	if (FStrEq(arg1, "emovevel"))		Cmd = AV_ENT_MOVEVEL;
	if (FStrEq(arg1, "erot"))			Cmd = AV_ENT_ROT;
	if (FStrEq(arg1, "euse"))			Cmd = AV_ENT_USE;
	if (FStrEq(arg1, "etouch"))			Cmd = AV_ENT_TOUCH;
	if (FStrEq(arg1, "ekill"))			Cmd = AV_ENT_KILL;
	if (FStrEq(arg1, "ehide"))			Cmd = AV_ENT_HIDE;
	if (FStrEq(arg1, "eshow"))			Cmd = AV_ENT_SHOW;
	if (FStrEq(arg1, "esetrender"))		Cmd = AV_ENT_SETRENDER;
	if (FStrEq(arg1, "esetsolid"))		Cmd = AV_ENT_SETSOLID;
	if (FStrEq(arg1, "emovetome"))		Cmd = AV_ENT_MOVETOME;
	if (FStrEq(arg1, "einfo"))			Cmd = AV_ENT_INFO;
	if (FStrEq(arg1, "emoreinfo"))		Cmd = AV_ENT_MOREINFO;
	if (FStrEq(arg1, "elist"))			Cmd = AV_ENT_LIST;
	if (FStrEq(arg1, "esetcolor"))		Cmd = AV_ENT_SETCOLOR;
	if (FStrEq(arg1, "esetgod"))		Cmd = AV_ENT_SETGOD;
	if (FStrEq(arg1, "espawn"))			Cmd = AV_ENT_SPAWN;
	if (FStrEq(arg1, "edup"))			Cmd = AV_ENT_DUPLICATE;
	if (FStrEq(arg1, "ewrite"))			Cmd = AV_ENT_WRITE;
	if (FStrEq(arg1, "eread"))			Cmd = AV_ENT_READ;
	if (FStrEq(arg1, "ereadold"))		Cmd = AV_ENT_READOLD;
	if (FStrEq(arg1, "ekey"))			Cmd = AV_ENT_KEYVALUE;
	if (FStrEq(arg1, "efree"))			Cmd = AV_ENT_FREE;
	if (FStrEq(arg1, "emaketrain"))		Cmd = AV_ENT_MAKETRAIN;
	

	if (FStrEq(arg1, "evel"))			Cmd = AV_ENT_VELOCITY;
	if (FStrEq(arg1, "eavel"))			Cmd = AV_ENT_AVELOCITY;
	if (FStrEq(arg1, "estop"))			Cmd = AV_ENT_STOP;

	// Special memory commands
	if (FStrEq(arg1, "ememadd"))		Cmd = AV_ENT_ADDTOMEM;
	if (FStrEq(arg1, "ememrem"))		Cmd = AV_ENT_REMFROMMEM;
	if (FStrEq(arg1, "ememclear"))		Cmd = AV_ENT_CLEARMEM;
	if (FStrEq(arg1, "ememshow"))		Cmd = AV_ENT_SHOWMEM;
	if (FStrEq(arg1, "ememhide"))		Cmd = AV_ENT_HIDEMEM;
	if (FStrEq(arg1, "ememlist"))		Cmd = AV_ENT_LISTMEM;
	if (FStrEq(arg1, "ememprint"))		Cmd = AV_ENT_PRINTMEMNUM;
	if (FStrEq(arg1, "ememswap"))		Cmd = AV_ENT_SWAPMEM;
	if (FStrEq(arg1, "emortarfield"))	Cmd = AV_ENT_MORTARFIELD;


	// Path Commands
	if (FStrEq(arg1, "epathadd"))		Cmd = AV_ENT_PATHADD;
	if (FStrEq(arg1, "epathrem"))		Cmd = AV_ENT_PATHREM;
	if (FStrEq(arg1, "epathshow"))		Cmd = AV_ENT_PATHSHOW;
	if (FStrEq(arg1, "epathclear"))		Cmd = AV_ENT_PATHCLEAR;
	if (FStrEq(arg1, "epathmake"))		Cmd = AV_ENT_PATHMAKE;
	if (FStrEq(arg1, "epathmove"))		Cmd = AV_ENT_PATHMOVE;

	if (FStrEq(arg1, "emakecopy"))		Cmd = AV_ENT_MAKECOPY;
	if (FStrEq(arg1, "ecopyclass"))		Cmd = AV_ENT_COPYCLASS;
	if (FStrEq(arg1, "ecopykey"))		Cmd = AV_ENT_COPYKEY;
	if (FStrEq(arg1, "ecopyspawn"))		Cmd = AV_ENT_COPYSPAWN;
	
	if (FStrEq(arg1, "eshowbox"))		Cmd = AV_ENT_SHOWBOX;
	if (FStrEq(arg1, "espawnagain"))	Cmd = AV_ENT_SPAWNAGAIN;

	if (FStrEq(arg1, "eshowtrigger"))	Cmd = AV_ENT_SHOWTRIGGER;
	if (FStrEq(arg1, "eshowpos"))		Cmd = AV_ENT_SHOWPOS;
	if (FStrEq(arg1, "emovemeto"))		Cmd = AV_ENT_MOVEMETO;
	if (FStrEq(arg1, "emakedoor"))		Cmd = AV_ENT_MAKEDOOR;

	if (FStrEq(arg1, "emusic"))			Cmd = AV_ENT_MUSIC;
	if (FStrEq(arg1, "emstop"))			Cmd = AV_ENT_MSTOP;
	if (FStrEq(arg1, "ealways"))		Cmd = AV_ENT_ALWAYS;
	if (FStrEq(arg1, "ecircle"))		Cmd = AV_ENT_CIRCLE;
	
	if (FStrEq(arg1, "eradiosong"))		Cmd = AV_ENT_RADIOSONG;
	if (FStrEq(arg1, "eradiopitch"))	Cmd = AV_ENT_RADIOPITCH;

	
	


	// Autospawn
	//if (FStrEq(arg1, "estartauto"))		Cmd = AV_ENT_STARTAUTO;
	//if (FStrEq(arg1, "elistauto"))		Cmd = AV_ENT_LISTAUTO;
	//if (FStrEq(arg1, "estopauto"))		Cmd = AV_ENT_STOPAUTO;

	// player commands
	if (FStrEq(arg1, "oteleport"))		Cmd = AV_PLY_TELEPORT;
	if (FStrEq(arg1, "steleport"))		Cmd = AV_PLY_METELEPORT;		// special case, not in the ME section
	if (FStrEq(arg1, "okill"))			Cmd = AV_PLY_KILL;
	if (FStrEq(arg1, "orun"))			Cmd = AV_PLY_COMMAND;
	if (FStrEq(arg1, "ogod"))			Cmd = AV_PLY_GOD;
	if (FStrEq(arg1, "oinvis"))			Cmd = AV_PLY_INVIS;
	if (FStrEq(arg1, "ograv"))			Cmd = AV_PLY_GRAV;
	if (FStrEq(arg1, "ofly"))			Cmd = AV_PLY_FLY;
	if (FStrEq(arg1, "otarget"))		Cmd = AV_PLY_TARGET;
	if (FStrEq(arg1, "ofriction"))		Cmd = AV_PLY_FRIC;
	if (FStrEq(arg1, "ocorp"))			Cmd = AV_PLY_SOLID;
	if (FStrEq(arg1, "omemset"))		Cmd = AV_PLY_ADDMEM;
	if (FStrEq(arg1, "otelelast"))		Cmd = AV_PLY_TELELAST;
	if (FStrEq(arg1, "oglowshell"))		Cmd = AV_PLY_GLOWSHELL;
	if (FStrEq(arg1, "ofunnel"))		Cmd = AV_PLY_FUNNEL;
	if (FStrEq(arg1, "opresent"))		Cmd = AV_PLY_SPAWNPRES;
	if (FStrEq(arg1, "ospritetrail"))	Cmd = AV_PLY_SPRITETRAIL;
	if (FStrEq(arg1, "obeamtorus"))		Cmd = AV_PLY_BEAMTORUS;
	if (FStrEq(arg1, "obeamfollow"))	Cmd = AV_PLY_BEAMFOLLOW;
	if (FStrEq(arg1, "obubbles"))		Cmd = AV_PLY_BUBBLETRAIL;
	if (FStrEq(arg1, "oprojectile"))	Cmd = AV_PLY_PROJECTILE;
	if (FStrEq(arg1, "oattach"))		Cmd = AV_PLY_ATTACH;
	if (FStrEq(arg1, "okillattach"))	Cmd = AV_PLY_REMATTACH;
	if (FStrEq(arg1, "ofirefield"))		Cmd = AV_PLY_FIREFIELD;
	if (FStrEq(arg1, "olocate"))		Cmd = AV_PLY_LOCATE;
	if (FStrEq(arg1, "oteletest"))		Cmd = AV_PLY_TELETEST;
	if (FStrEq(arg1, "obeamring"))		Cmd = AV_PLY_BEAMRING;
	if (FStrEq(arg1, "oiuser"))			Cmd = AV_PLY_IUSER;
	if (FStrEq(arg1, "odrawclock"))		Cmd = AV_PLY_DRAWCLOCK;

	// new stuff
	if (FStrEq(arg1, "ovelup"))			Cmd = AV_PLY_VELUP;
	if (FStrEq(arg1, "oteam"))			Cmd = AV_PLY_TEAM;
	if (FStrEq(arg1, "oclass"))			Cmd = AV_PLY_CLASS;
	if (FStrEq(arg1, "omovetype"))		Cmd = AV_PLY_MOVETYPE;
	if (FStrEq(arg1, "obeam"))			Cmd = AV_PLY_BEAM;
	if (FStrEq(arg1, "oweapons"))		Cmd = AV_PLY_WEAPONS;
	if (FStrEq(arg1, "ofrags"))			Cmd = AV_PLY_FRAGS;
	if (FStrEq(arg1, "ohealth"))		Cmd = AV_PLY_HEALTH;
	if (FStrEq(arg1, "omaxhealth"))		Cmd = AV_PLY_MAXHEALTH;
	if (FStrEq(arg1, "ogiveair"))		Cmd = AV_PLY_GIVEAIR;
	if (FStrEq(arg1, "ospeed"))			Cmd = AV_PLY_SPEED;
	if (FStrEq(arg1, "ofov"))			Cmd = AV_PLY_FOV;
	if (FStrEq(arg1, "olongbeam"))		Cmd = AV_PLY_LONGBEAM;
	if (FStrEq(arg1, "omortar"))		Cmd = AV_PLY_MORTAR;
	if (FStrEq(arg1, "ogive"))			Cmd = AV_PLY_GIVE;
	if (FStrEq(arg1, "olight"))			Cmd = AV_PLY_FLASHLIGHT;
	if (FStrEq(arg1, "osetview"))		Cmd = AV_PLY_SETVIEW;
	if (FStrEq(arg1, "oresetview"))		Cmd = AV_PLY_RESETVIEW;
	if (FStrEq(arg1, "opopup"))			Cmd = AV_PLY_POPUP;
	if (FStrEq(arg1, "odisco"))			Cmd = AV_PLY_DISCO;
	if (FStrEq(arg1, "oground"))		Cmd = AV_PLY_NOTGROUND;
	if (FStrEq(arg1, "omodel"))			Cmd = AV_PLY_CHANGEMODEL;
	
	if (FStrEq(arg1, "ospecial"))		Cmd = AV_PLY_SPECIAL;
	if (FStrEq(arg1, "oleash"))			Cmd = AV_PLY_LEASH;
	if (FStrEq(arg1, "ospriteboom"))	Cmd = AV_PLY_SPRITEBOOM;
	if (FStrEq(arg1, "owords"))			Cmd = AV_PLY_WORDS;
	if (FStrEq(arg1, "ovangle"))		Cmd = AV_PLY_SETVIEWANGLE;
	if (FStrEq(arg1, "ocolormap"))		Cmd = AV_PLY_COLORMAP;
	if (FStrEq(arg1, "osetpoints"))		Cmd = AV_PLY_SETPOINTS;
	

	

	// self commands
	if (FStrEq(arg1, "sgod"))			Cmd = AV_ME_GOD;
	if (FStrEq(arg1, "sgrav"))			Cmd = AV_ME_GRAV;
	if (FStrEq(arg1, "sfriction"))		Cmd = AV_ME_FRIC;
	if (FStrEq(arg1, "sinvis"))			Cmd = AV_ME_INVIS;
	if (FStrEq(arg1, "sfly"))			Cmd = AV_ME_FLY;
	if (FStrEq(arg1, "starget"))		Cmd = AV_ME_TARGET;
	if (FStrEq(arg1, "scorp"))			Cmd = AV_ME_SOLID;
	if (FStrEq(arg1, "sglowshell"))		Cmd = AV_ME_GLOWSHELL;
	if (FStrEq(arg1, "steam"))			Cmd = AV_ME_TEAM;

	// noncommands

	if (FStrEq(arg1, "nteleport"))		Cmd = AV_NON_TELEPORT;
	if (FStrEq(arg1, "nlocate"))		Cmd = AV_NON_LOCATE;
	if (FStrEq(arg1, "nbeamring"))		Cmd = AV_NON_BEAMRING;

	// See if it's an ENT command
	if (Cmd > AV_RANGE_ENT_START && Cmd < AV_RANGE_ENT_END) {
		//yes it is.
		AvEntCommand( pEntity, pPev, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, Cmd );
	}

	// See if it's a PLAYER command
	if (Cmd > AV_RANGE_PLY_START && Cmd < AV_RANGE_PLY_END) {
		//yes it is.
		AvPlayerCommand( pEntity, pPev, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, Cmd );
	}

	// See if it's a SELF crommand
	if (Cmd > AV_RANGE_ME_START && Cmd < AV_RANGE_ME_END) {
		//yes it is.
		AvSelfCommand( pEntity, pPev, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, Cmd );
	}

	// See if it's a NON command
	if (Cmd > AV_RANGE_NON_START && Cmd < AV_RANGE_NON_END) {
		//yes it is.
		AvNonCommand( pEntity, pPev, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, Cmd );
	}
}


void AvEntCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd )
{

	// First get what we are talking to. Format for ALL Ent Commands:
	// <command> <What ent we are talking to> <extra crap>
	
	int MyNumber = ENTINDEX(pEntity);
	int MsgsSent = 0;
	int startArg = 0;


	if (Cmd == AV_ENT_CIRCLE)
	{

		// clear the temp
		for (int jjk = 0; jjk < 1024; jjk++) markerTempArr[jjk] = 0;
		

	}


	// Some commands don't use this interface.
	if (Cmd != AV_ENT_HIDEMEM 
		&& Cmd != AV_ENT_SHOWMEM 
		&& Cmd != AV_ENT_LISTMEM 
		&& Cmd != AV_ENT_PRINTMEMNUM 
		&& Cmd != AV_ENT_CLEARMEM 
		&& Cmd != AV_ENT_SPAWN
		&& Cmd != AV_ENT_STARTAUTO 
		&& Cmd != AV_ENT_LISTAUTO 
		&& Cmd != AV_ENT_STOPAUTO
		&& Cmd != AV_ENT_WRITE 
		&& Cmd != AV_ENT_READ 
		&& Cmd != AV_ENT_FREE 
		&& Cmd != AV_ENT_READOLD
		&& Cmd != AV_ENT_SWAPMEM 
		&& Cmd != AV_ENT_PATHADD 
		&& Cmd != AV_ENT_PATHREM 
		&& Cmd != AV_ENT_PATHSHOW 
		&& Cmd != AV_ENT_PATHCLEAR 
		&& Cmd != AV_ENT_PATHMAKE  
		&& Cmd != AV_ENT_PATHMOVE 
		&& Cmd != AV_ENT_MORTARFIELD 
		&& Cmd != AV_ENT_COPYCLASS 
		&& Cmd != AV_ENT_COPYKEY 
		&& Cmd != AV_ENT_COPYSPAWN
		)
	{

		if (FStrEq(arg2, "mem")) 
		{
			// This command to be performed to all ents in memory.
			// Usage: avb <command> mem <memnum>

			int i = 1;
			int memnum = atoi(arg3);
			if (memnum < 0 || memnum > 5) {
				char msg[80];
				sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum);
				ClientPrint( pPev, HUD_PRINTTALK, msg);
				return;
			}

			edict_t *frontEnt;
			
			// Clear all the JUSTDUPED flags!

			for (i = 1; i < 1025; i++) {
				EntData[i].justduped = 0;
			}

			for (i = 1; i < 1025; i++) {
				if (AdminMems[MyNumber][memnum][i] && EntData[i].justduped == 0) {
					
					frontEnt = INDEXENT ( i );
					if (frontEnt) {
						int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
						startArg = 4;
					}
				}
				EntData[i].justduped = 0;
			}


		}
		else if	(FStrEq(arg2, "me")) 
		{
			// performed on ME			
			MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, pEntity, arg3, arg4, arg5, arg6, arg7, arg8, Cmd);
			startArg = 3;
		}
		else if	(FStrEq(arg2, "front")) 
		{
			// This command to be performed to the ent I am looking at.

			edict_t *frontEnt = Av_GetFrontEnt ( pEntity );
			MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg3, arg4, arg5, arg6, arg7, arg8, Cmd);
			startArg = 3;
			
			//			int ind = ENTINDEX(frontEnt);


		}
		else if	(FStrEq(arg2, "class")) 
		{
			// This command to be performed to all ents of this type

			int i = 1;
			char *pClassname;
			edict_t *frontEnt;
			
			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pClassname = Av_GetClassname ( frontEnt ); 
					if (FStrEq(arg3, pClassname)) {
												int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
						startArg = 4;
					}
				}
			}


		}
		else if	(FStrEq(arg2, "dance")) 
		{
			// This command to be performed to all ents where the dance name matches this

			int i = 1;
			edict_t *frontEnt;
			
			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					
					if (FStrEq(arg3, EntData[i].digitgroup)) {
						
						int extnow = 0;
						
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
						startArg = 4;
					}
				}
			}



		}

		else if	(FStrEq(arg2, "rad")) 
		{
			// This command to be performed to all ents within (arg3) units of me.
			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev;
			bool classfilter = 0;
			
			if (FStrEq(arg4, "class")) classfilter = 1;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);
					

					if ((((pRunOnPev->absmin + pRunOnPev->absmax)* 0.5) - pPev->origin).Length() <= atoi(arg3)) 
					{
						
						// RAD supports special operators, like additional class filtering.

						

						if (classfilter)
						{
							if (FStrEq(arg5, (char*)STRING(pRunOnPev->classname))) {
								int extnow = 0;
								if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
								if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
								if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
								if (EntData[i].donttouchme == 1) extnow = 1;
								if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg6, arg7, arg8, arg9, "", "", Cmd);							
								startArg = 6;
							}
						}
						else
						{
							int extnow = 0;
							if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
							if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
							if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
							if (EntData[i].donttouchme == 1) extnow = 1;
							if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
							startArg = 4;
						}
					}
				}
			}
		}
		else if	(FStrEq(arg2, "hidden")) 
		{
			// This command to be performed to all ents that are currently hidden

			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);
					if (pRunOnPev->effects & EF_NODRAW) {
												int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg3, arg4, arg5, arg6, arg7, arg8, Cmd);
						startArg = 3;
					}
				}
			}

		}
		else if	(FStrEq(arg2, "target")) 
		{
			// This command to be performed to all ents that have this target

			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);
					if (FStrEq((char *)STRING(pRunOnPev->target), arg3)) {
												int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
						startArg = 4;
					}
				}
			}

		}
		else if	(FStrEq(arg2, "targetname")) 
		{
			// This command to be performed to all ents that have this targetname

			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev;
			bool partial = 0;
			if (FStrEq(arg4, "partial")) partial = 1;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);
					if (FStrEq((char *)STRING(pRunOnPev->targetname), arg3) && partial == 0) {
						
						int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
						startArg = 4;

					}
					else if (strstr((char *)STRING(pRunOnPev->targetname), arg3) && partial == 1) {
						
						int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg5, arg6, arg7, arg8, arg9, "", Cmd);
						startArg = 5;

					}

				}
			}
		}
		else if	(FStrEq(arg2, "netname")) 
		{
			// This command to be performed to all ents that have this netname

			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev;
			bool partial = 0;
			if (FStrEq(arg4, "partial")) partial = 1;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);
					if (FStrEq((char *)STRING(pRunOnPev->netname), arg3) && partial == 0) {
						
						int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
						startArg = 4;

					}
					else if (strstr((char *)STRING(pRunOnPev->netname), arg3) && partial == 1) {
						
						int extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
	
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg5, arg6, arg7, arg8, arg9, "", Cmd);
						startArg = 5;

					}

				}
			}
		}
		else if (atoi(arg2) > 0)
		{
			// This command to be performed to the ent with this number.
			
			edict_t *frontEnt = INDEXENT ( atoi(arg2) );
			if (frontEnt) {
				MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg3, arg4, arg5, arg6, arg7, arg8, Cmd);
				startArg = 3;
			}
		}
		else
		{
			// This command to be performed on all ents that have a keyvalue matching this one


			int i = 1;
			int j = 0;

			edict_t *frontEnt;
			entvars_t *pRunOnPev;
			bool matched;
			bool extnow;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);
					matched = 0;

					
					// try to find a matching key
					for (j = 1; (j <= EntData[i].numkeys) && matched == 0; j++)
					{
						
						if (FStrEq( Av_GetKeyName(EntData[i].keyindex[j]), arg2)) 
						{
							// this ent has the key, now make sure the value matches							
							if (FStrEq( Av_GetKeyValue(EntData[i].keyindex[j]), arg3)) 
							{
								
								// match!
								matched = 1;
							}
						}
					}

					if (matched)
					{

						extnow = 0;
						if (MsgsSent > 7 && Cmd == AV_ENT_INFO) extnow = 1;
						if (MsgsSent > 1 && Cmd == AV_ENT_MOREINFO) extnow = 1;
						if (MsgsSent > 40 && Cmd == AV_ENT_LIST) extnow = 1;
						if (EntData[i].donttouchme == 1) extnow = 1;
		
						if (extnow == 0)	MsgsSent = MsgsSent + AvRunEnt (pEntity, pPev, MyNumber, frontEnt, arg4, arg5, arg6, arg7, arg8, arg9, Cmd);
						startArg = 4;
					}
				}
			}




		}


		if (MsgsSent > 0 && Cmd == AV_ENT_CIRCLE)
		{

			// OK, it's time to arrange these ents in a circle around me.

			// Syntax for avc ecircle:
			// avc ecircle <ents> <radius> <start angle> <arc> <bool: turn ents on Y axis>

			float arcDist = 0;
			float radius = 0;
			float startAng = 0;
			int rotEnts = 0;

			if (startArg == 3)
			{
				radius		= atof(arg3);
				startAng	= atof(arg4);
				arcDist		= atof(arg5);
				rotEnts		= atoi(arg6);
			}
			else if (startArg == 4)
			{
				radius		= atof(arg4);
				startAng	= atof(arg5);
				arcDist		= atof(arg6);
				rotEnts		= atoi(arg7);
			}
			else if (startArg == 5)
			{
				radius		= atof(arg5);
				startAng	= atof(arg6);
				arcDist		= atof(arg7);
				rotEnts		= atoi(arg8);
			}
			else if (startArg == 6)
			{
				radius		= atof(arg6);
				startAng	= atof(arg7);
				arcDist		= atof(arg8);
				rotEnts		= atoi(arg9);
			}


			if (radius == 0) radius = 300;
			if (arcDist == 0) arcDist = 360;
			
			// Now, figure out the increment.
			float increment = arcDist / MsgsSent;

			edict_t *pent2;
			float currang;
			float pi = 3.141592654;
			Vector newOri;

			// begin cycling through the ents
			for (int jjk = 0; jjk < 1024; jjk++) 
			{
				if (markerTempArr[jjk])
				{
				
					pent2 = INDEXENT(jjk);
					

					// Position this ent
					currang = startAng * (pi / 180);

					newOri.x = (int)(cos(currang) * radius);
					newOri.y = (int)(sin(currang) * radius);
					newOri.z = pent2->v.origin.z;
					
					newOri.x += pEntity->v.origin.x;
					newOri.y += pEntity->v.origin.y;

					SET_ORIGIN(pent2, newOri);


					if (rotEnts)
					{
						pent2->v.angles.y = startAng;

					}


					// After ent has been placed, increment.

					startAng += increment;


				}
			}
		}


		char msg[80];
		sprintf(msg,"* %i ents effected by command.\n", MsgsSent);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}
	else if (Cmd == AV_ENT_PRINTMEMNUM) {
		
		// Print number of ents in memory!

		int i = 1;
		int nm = 0;
		int memnum = atoi(arg2);
		if (memnum < 0 || memnum > 5) {
			char msg[80];
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum);
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}

		for (i; i < 1025; i++) {
			if (AdminMems[MyNumber][memnum][i]) nm++;
		}
		char msg[80];
		sprintf(msg,"* Number of ents in memory number %i: %i\n", memnum, nm);
		ClientPrint( pPev, HUD_PRINTTALK, msg);
	}

	else if (Cmd == AV_ENT_SHOWMEM) {
		
		// Show ents in this memory

		int i = 1;
		int nm = 0;

		if (FStrEq(arg2, "all"))
		{

			char msg[80];

			AdminFlags[MyNumber] |= AV_FLAG_SHOWMEM0;
			AdminFlags[MyNumber] |= AV_FLAG_SHOWMEM1;
			AdminFlags[MyNumber] |= AV_FLAG_SHOWMEM2;
			AdminFlags[MyNumber] |= AV_FLAG_SHOWMEM3;
			AdminFlags[MyNumber] |= AV_FLAG_SHOWMEM4;
			AdminFlags[MyNumber] |= AV_FLAG_SHOWMEM5;

			AvUpdateShowMem();

			sprintf(msg,"* All memories shown.\n");
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
			return;
		}
		
		int memnum = atoi(arg2);
		if (memnum < 0 || memnum > 5) {
			char msg[80];
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum);
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}
/*

		entvars_t *pRunOnPev;
		edict_t *frontEnt;
		for (i; i < 1025; i++) {
			if (AdminMems[MyNumber][memnum][i]) {
				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);

					// First store this ents data in the array, so we don't fuck it up.

					if (EntData[i].alreadydone == 0) {
						
						EntData[i].rendermode = (int)pRunOnPev->rendermode;
						EntData[i].renderamt = (float)pRunOnPev->renderamt;
						EntData[i].renderfx = (int)pRunOnPev->renderfx;
						EntData[i].rendercolorx = (int)pRunOnPev->rendercolor.x;
						EntData[i].rendercolory = (int)pRunOnPev->rendercolor.y;
						EntData[i].rendercolorz = (int)pRunOnPev->rendercolor.z;
					}

					EntData[i].alreadydone = 1;
					// Set colors based on memory number... 

					AvShowMem( pRunOnPev, memnum );
				}
			}
		}
*/

		char msg[80];

		// Set that we are showing memory.

		int theflag = 0;
		if (memnum == 0) theflag = AV_FLAG_SHOWMEM0;
		if (memnum == 1) theflag = AV_FLAG_SHOWMEM1;
		if (memnum == 2) theflag = AV_FLAG_SHOWMEM2;
		if (memnum == 3) theflag = AV_FLAG_SHOWMEM3;
		if (memnum == 4) theflag = AV_FLAG_SHOWMEM4;
		if (memnum == 5) theflag = AV_FLAG_SHOWMEM5;

		AdminFlags[MyNumber] |= theflag;

		AvUpdateShowMem();

		sprintf(msg,"* Memory %i ents shown.\n", memnum);
		ClientPrint( pPev, HUD_PRINTTALK, msg);

	}
	else if (Cmd == AV_ENT_HIDEMEM) {
		
		// Hide ents in this memory

		int i = 1;
		int nm = 0;


		if (FStrEq(arg2, "all"))
		{

			char msg[80];

			AdminFlags[MyNumber] &= ~AV_FLAG_SHOWMEM0;
			AdminFlags[MyNumber] &= ~AV_FLAG_SHOWMEM1;
			AdminFlags[MyNumber] &= ~AV_FLAG_SHOWMEM2;
			AdminFlags[MyNumber] &= ~AV_FLAG_SHOWMEM3;
			AdminFlags[MyNumber] &= ~AV_FLAG_SHOWMEM4;
			AdminFlags[MyNumber] &= ~AV_FLAG_SHOWMEM5;

			AvUpdateShowMem();

			sprintf(msg,"* All memories hidden.\n");
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}


		int memnum = atoi(arg2);
		if (memnum < 0 || memnum > 5) {
			char msg[80];
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum);
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}

		/*
		entvars_t *pRunOnPev;
		edict_t *frontEnt;

		for (i; i < 1025; i++) {
			if (AdminMems[MyNumber][memnum][i]) {
				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);

					// Set the render vars back to what they are supposed to be

					pRunOnPev->rendercolor.x = EntData[i].rendercolorx;
					pRunOnPev->rendercolor.y = EntData[i].rendercolory;
					pRunOnPev->rendercolor.z = EntData[i].rendercolorz;
					pRunOnPev->rendermode = EntData[i].rendermode;
					pRunOnPev->renderamt = EntData[i].renderamt;
					pRunOnPev->renderfx = EntData[i].renderfx;

					EntData[i].alreadydone = 0;
				}
			}
		}
*/

		int theflag = 0;
		if (memnum == 0) theflag = AV_FLAG_SHOWMEM0;
		if (memnum == 1) theflag = AV_FLAG_SHOWMEM1;
		if (memnum == 2) theflag = AV_FLAG_SHOWMEM2;
		if (memnum == 3) theflag = AV_FLAG_SHOWMEM3;
		if (memnum == 4) theflag = AV_FLAG_SHOWMEM4;
		if (memnum == 5) theflag = AV_FLAG_SHOWMEM5;

		AdminFlags[MyNumber] &= ~theflag;
		
		AvUpdateShowMem();

		char msg[80];
		sprintf(msg,"* Memory %i ents normal.\n", memnum);
		ClientPrint( pPev, HUD_PRINTTALK, msg);
	}
	else if (Cmd == AV_ENT_CLEARMEM) {
		
		// Clear memory

		int i = 1;
		int nm = 0;
		int memnum = atoi(arg2);
		if (memnum < 0 || memnum > 5) {
			char msg[80];
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum);
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}

		for (i; i < 1025; i++) {
			if (AdminMems[MyNumber][memnum][i]) {
				nm++;
				AdminMems[MyNumber][memnum][i] = 0;
			}
		}

		AvUpdateShowMem();

		char msg[80];
		sprintf(msg,"* Memory number %i cleared of %i ents!\n", memnum, nm);
		ClientPrint( pPev, HUD_PRINTTALK, msg);
	}
	
	
	else if (Cmd == AV_ENT_SPAWN) { 
		
		// Spawn an ent in front of me.
	
		// format:
		// avc espawn <classname> 

		if (!FStrEq(arg2, "")) {

			edict_t *tEntity;
			int mde = 0;

			if (FStrEq(arg2, "item_parachute"))
			{
				tEntity = ParaSpawnItem( NULL );
				mde = 1;
			}
			else  tEntity = CREATE_NAMED_ENTITY(MAKE_STRING(arg2));


			EntData[ENTINDEX(tEntity)].numkeys = 0;

			if (!FNullEnt(tEntity))
			{

				entvars_t *pRunOnPev;
				pRunOnPev =  VARS(tEntity);

				KeyValueData kvd;
				char buf[80];
				sprintf( buf, "%s", arg2);

				// Set the KEYVALUES here!
				kvd.fHandled = FALSE;
				kvd.szClassName = NULL;
				kvd.szKeyName = "classname";
				kvd.szValue = buf;

				if (mde==0) DispatchKeyValue( tEntity, &kvd );


				// place this in front

				UTIL_MakeVectors ( pPev->v_angle );
				Vector vecSrc  = GetGunPosition( pEntity );
				Vector vecEnd  = vecSrc + gpGlobals->v_forward * 80;

				pRunOnPev->origin = vecEnd;
				SET_ORIGIN( tEntity , vecEnd );
				pRunOnPev->angles.y = pPev->v_angle.y;

				if (mde==0) DispatchSpawn( ENT( pRunOnPev ) );
				
				if (FStrEq(arg3, "drop")) DROP_TO_FLOOR ( ENT( pRunOnPev ) ) ;

				char msg[80];
				sprintf(msg,"Spawned a %s\n", arg2);
				ClientPrint( pPev, HUD_PRINTTALK, msg);
			}
			else
			{
				ClientPrint( pPev, HUD_PRINTTALK, "Error: Null ent");
			}
		}
	}
	else if (Cmd == AV_ENT_STARTAUTO) { 
//		char msg[80];
		/*
		if (numAutoSpawners == 20) {
			sprintf(msg, "Reached maximum number of autospawners!");
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}
		
		// Add one.

		if (!FStrEq(arg2, "")) {

			numAutoSpawners++;
			
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pEntity);

			// Set the various properties

			AutoSpawnOrigin[numAutoSpawners] = pRunOnPev->origin;
			
			//sprintf(AutoSpawnClassname[numAutoSpawners], "%s", arg2);

			AutoSpawnClassname[numAutoSpawners] = MAKE_STRING(arg2);

			
			AutoSpawnCapacity[numAutoSpawners] = atoi(arg3);
			AutoSpawnCurrEnt[numAutoSpawners] = 0;
			
			AvMakeAutoEnt(numAutoSpawners);
			
			sprintf(msg, "Created an AutoSpawner of type %s", arg2);
			ClientPrint( pPev, HUD_PRINTTALK, msg);
		}

  */
	}
	else if (Cmd == AV_ENT_WRITE) {
		
		char msg[80];
		char pFilename[80];
		sprintf(pFilename, "%s_%s.ent",(char *)STRING(gpGlobals->mapname), arg2);
		
		AvWriteEntsToFile((char *)pFilename);

		sprintf(msg, "Wrote ents to file %s!\n", arg2);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

	}
	else if (Cmd == AV_ENT_READ) {
		char msg[80];
		char pFilename[80];
		sprintf(pFilename, "%s_%s.ent", (char *)STRING(gpGlobals->mapname), arg2);
		
		AvReadEntsFromFile(pPev, (char *)pFilename);

		sprintf(msg, "Read ents from file %s!\n", arg2);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}
	else if (Cmd == AV_ENT_READOLD) {
		char msg[80];
		char pFilename[80];
		sprintf(pFilename, "%s.edt", arg2);
		
		OldAvReadEntsFromFile(pPev, (char *)pFilename);

		sprintf(msg, "Read ents from file %s!\n", arg2);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}
	else if (Cmd == AV_ENT_FREE) {
	
		char msg[80];
		// Tell player how many ents are free.
		int i;
		int fr = 0;

		edict_t *pPlayerEdict;
		
		for ( i = 1; i <= 1024; i++ )
		{
			pPlayerEdict = INDEXENT( i );
			if ( !pPlayerEdict )
			{
				fr++;
			}
			else if ( pPlayerEdict && pPlayerEdict->free )
			{
				fr++;
			}

			//
		}	

		sprintf(msg, "* Number of free ents: %d\n", fr);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}
	else if (Cmd == AV_ENT_SWAPMEM) {
		
		// Copy memory from 1 slot to another

		int i = 1;
		int nm = 0;
		int memnum1 = atoi(arg2);
		int memnum2 = atoi(arg3);

		if (memnum1 < 0 || memnum1 > 5) {
			char msg[80];
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum1);
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}
		if (memnum2 < 0 || memnum2 > 5) {
			char msg[80];
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum2);
			ClientPrint( pPev, HUD_PRINTTALK, msg);
			return;
		}

		entvars_t *pRunOnPev;
		edict_t *frontEnt;

		for (i; i < 1025; i++) {
			
			int oth = AdminMems[MyNumber][memnum2][i];
			
			if (AdminMems[MyNumber][memnum1][i]) {
				nm++;
				// swap this one.

				AdminMems[MyNumber][memnum1][i] = 0;
				AdminMems[MyNumber][memnum2][i] = 1;

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);

					// Set the render vars to what new mem colors are.
					
					if (EntData[i].alreadydone) 
					{
						//AvShowMem( pRunOnPev, memnum2 );
					}
				}
			}

			if (oth) {
				nm++;
				// swap this one.

				AdminMems[MyNumber][memnum1][i] = 1;
				AdminMems[MyNumber][memnum2][i] = 0;

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev =  VARS(frontEnt);

					// Set the render vars to what new mem colors are.
					
					if (EntData[i].alreadydone) 
					{
						//AvShowMem( pRunOnPev, memnum1 );
					}
				}
			}

		}

		AvUpdateShowMem();

		char msg[80];
		sprintf(msg,"* Memory number %i swapped with mem num %i, containing %i ents!\n", memnum1, memnum2, nm);
		ClientPrint( pPev, HUD_PRINTTALK, msg);
	}
	else if (Cmd >= AV_ENT_PATHADD && Cmd <= AV_ENT_PATHMAKE) {
		AvPathCommands( pEntity, pPev, MyNumber, arg2, arg3, arg4, arg5, Cmd);
	}
	else if (Cmd == AV_ENT_MORTARFIELD) {
		

		// Set the various attributes of the mortar feild

		char msg[80];

		sprintf(msg, "Unknown Mortar Field Command.\n");
		
		if (FStrEq(arg2, "enable")) {
			MortarFieldEnabled = 1;
			sprintf(msg, "Mortar Field Enabled\n");

		}
		else if (FStrEq(arg2, "disable")) {
			MortarFieldEnabled = 0;
			sprintf(msg, "Mortar Field Disabled\n");
		}
		else if (FStrEq(arg2, "mins")) {
			MortarFieldMins = pPev->origin;
			sprintf(msg, "Mortar Field MINS Set.\n");
		}
		else if (FStrEq(arg2, "maxs")) {
			MortarFieldMaxs = pPev->origin;
			sprintf(msg, "Mortar Field MAXS Set.\n");
		}
		else if (FStrEq(arg2, "delay")) {
			MortarFieldDelay = atoi(arg3);
			sprintf(msg, "Mortar Field DELAY set to %i.\n", atoi(arg3));
		}
		else if (FStrEq(arg2, "show")) {

			// show the mortar field boundaries
	


			//Bottom box

			pathdata_t mn;
			pathdata_t mx;
			
			mn.x = (int)MortarFieldMins.x;
			mn.y = (int)MortarFieldMins.y;
			mn.z = (int)MortarFieldMins.z;

			mx.x = (int)MortarFieldMaxs.x;
			mx.y = (int)MortarFieldMaxs.y;
			mx.z = (int)MortarFieldMaxs.z;



			// bottom square

			AvMakeBeam2( mn.x, mn.y, mn.z, mn.x, mx.y, mn.z, 100, 50, 0, 255, 0); // point 1 -> point 2
			AvMakeBeam2( mn.x, mn.y, mn.z, mx.x, mn.y, mn.z, 100, 50, 0, 255, 0); // point 1 -> point 4
			
			AvMakeBeam2( mn.x, mx.y, mn.z, mx.x, mx.y, mn.z, 100, 50, 0, 255, 0); // point 2 -> point 3
			AvMakeBeam2( mx.x, mn.y, mn.z, mx.x, mx.y, mn.z, 100, 50, 0, 255, 0); // point 4 -> point 3

			// Top square:

			AvMakeBeam2( mn.x, mn.y, mx.z, mn.x, mx.y, mx.z, 100, 50, 0, 255, 0); // point 5 -> point 6
			AvMakeBeam2( mn.x, mn.y, mx.z, mx.x, mn.y, mx.z, 100, 50, 0, 255, 0); // point 5 -> point 8
			
			AvMakeBeam2( mn.x, mx.y, mx.z, mx.x, mx.y, mx.z, 100, 50, 0, 255, 0); // point 6 -> point 7
			AvMakeBeam2( mx.x, mn.y, mx.z, mx.x, mx.y, mx.z, 100, 50, 0, 255, 0); // point 8 -> point 7

			// Sides

			AvMakeBeam2( mn.x, mn.y, mn.z, mn.x, mn.y, mx.z, 100, 50, 0, 255, 0); // point 1 -> point 5
			AvMakeBeam2( mx.x, mx.y, mn.z, mx.x, mx.y, mx.z, 100, 50, 0, 255, 0); // point 3 -> point 7
			AvMakeBeam2( mn.x, mx.y, mn.z, mn.x, mx.y, mx.z, 100, 50, 0, 255, 0); // point 2 -> point 6
			AvMakeBeam2( mx.x, mn.y, mn.z, mx.x, mn.y, mx.z, 100, 50, 0, 255, 0); // point 4 -> point 8

			

			sprintf(msg, "Mortar Field Shown.\n");

		}

		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}
	else if (Cmd == AV_ENT_COPYCLASS) 
	{

		// run CLASSNAME on the copyent

		if (entcopy) 
		{
			char buf[80];
			KeyValueData kvd;			
			
			sprintf(buf, "%s", arg2);
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = buf;
			DispatchKeyValue( entcopy, &kvd );

			char msg[80];
			sprintf(msg, "Set Ent Classname.\n");
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		}
		else
		{

			char msg[80];
			sprintf(msg, "Could not set Ent Classname.\n");
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
		}
	}
	else if (Cmd == AV_ENT_COPYKEY) 
	{

		// run KEY on the copyent

		if (entcopy) 
		{
			char buf[80];
			char buf2[80];
			KeyValueData kvd;			
			
			sprintf(buf, "%s", arg2);
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = buf;
			sprintf(buf2, "%s", arg3);
			kvd.szValue = buf2;
			DispatchKeyValue( entcopy, &kvd );

			char msg[80];
			sprintf(msg, "Set Ent Key.\n");
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		}
		else
		{

			char msg[80];
			sprintf(msg, "Could not set Ent Key.\n");
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
		}
	}
	else if (Cmd == AV_ENT_COPYSPAWN) 
	{

		// run SPAWN on the copyent

		if (entcopy) 
		{
			DispatchSpawn( entcopy );

			char msg[80];
			sprintf(msg, "Spawned Ent! %i.\n", ENTINDEX(entcopy));
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		}
		else
		{

			char msg[80];
			sprintf(msg, "NOT Spawned Ent!.\n");
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
		}
	}


}


void Dismount()
{
	if (onLlama == 0) return;

//	char msg[80];

	// change llama's model to the normal one
	//llama->v.fuser1 = 1;
	//llama->v.vuser3.x = 1;
	
	g_engfuncs.pfnSetClientKeyValue( ENTINDEX(llama) , g_engfuncs.pfnGetInfoKeyBuffer( llama ), "model", "rabid1" );
	sprintf( playerModels[ENTINDEX(llama)].text , "%s", "rabid1" );

	//llama->v.body = 0;
	//llama->v.skin = 0;


	// change AV to NORMAL

	rider->v.rendermode = 0;
	rider->v.renderamt = 0;
	rider->v.solid = SOLID_SLIDEBOX;

	// change llama to NORMAL
	llama->v.takedamage = DAMAGE_AIM;

	// prevent AV from moving


	llama->v.maxspeed = 500;		
	rider->v.maxspeed = 500;
	rider->v.movetype = MOVETYPE_WALK;
	rider->v.gravity = 1;
	// move av outside the llama

	Vector outside = llama->v.origin;
	outside.z = outside.z + 70;
	SET_ORIGIN(rider, outside);

	//sprintf(msg,"* Avatar-X has dismounted you!\n");
	//ClientPrint( VARS(llama), HUD_PRINTTALK, msg);

	//sprintf(msg,"* You've dismounted your llama!\n");
	//ClientPrint( VARS(rider), HUD_PRINTTALK, msg);

	onLlama = 0;
	//llama = NULL;
	//rider = NULL;


}

void AvPrePlayerThink2 ( edict_t *pEntity )
{
	
	int MyNumber = ENTINDEX(pEntity);

	


	if (onLlama)
	{	
		if (pEntity == llama)
		{

			// disable strafe

			pEntity->v.button &= ~IN_MOVELEFT;
			pEntity->v.button &= ~IN_MOVERIGHT;

			// disable duck

			pEntity->v.button &= ~IN_DUCK;
		}

		if (pEntity == rider)
		{

			// disable all movement

			pEntity->v.button &= ~IN_FORWARD;
			pEntity->v.button &= ~IN_MOVELEFT;
			pEntity->v.button &= ~IN_MOVERIGHT;
			pEntity->v.button &= ~IN_BACK;
			pEntity->v.button &= ~IN_JUMP;
			pEntity->v.button &= ~IN_DUCK;
		}
	}


	if (EntData[MyNumber].leash_master != NULL)
	{

		if (!FNullEnt(EntData[MyNumber].leash_master))
		{
			
			// move me towards

			
			Vector vecGo = EntData[MyNumber].leash_master->v.origin - pEntity->v.origin;
			if (vecGo.Length() > 200)
			{
				vecGo = vecGo.Normalize();
				pEntity->v.velocity = vecGo * 600;
			}

		}

	}

	if (NoOnGround[MyNumber])
	{
		pEntity->v.flags &= ~FL_ONGROUND;
	}

	// keep av close
	if (onLlama)
	{
		if (pEntity == llama)
		{
			// if llama dies, dismount

			if (llama->v.deadflag != DEAD_NO)
			{
				Dismount();
			}

			if (llama->v.playerclass == 0)
			{
				Dismount();
			}

			
			if (llama && rider && onLlama)
			{
				// move av so he is with the llama
				rider->v.velocity = llama->v.velocity;
				rider->v.avelocity = llama->v.avelocity;
				
				Vector myLook =  llama->v.origin;

				myLook.z = myLook.z + 20;

				SET_ORIGIN(rider, myLook);
			}
			else
			{
				// dismount
				Dismount();
			}

		}
	}

	if (lastModelCheck < gpGlobals->time)
	{
		checkMapSpecificModels();
		lastModelCheck = gpGlobals->time + 2;
	}


	if (lastbeamtime <= gpGlobals->time && beamadmin == MyNumber)
	{
		entvars_t *pPev = VARS(pEntity);

		TraceResult tr;

		Vector vangle = pPev->v_angle;
		vangle.y = vangle.y + angleadd;


		UTIL_MakeVectors ( vangle );
		Vector vecSrc   = GetGunPosition( pEntity );
		Vector vecEnd   = vecSrc + gpGlobals->v_forward * 8192;

		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( pPev ), &tr );

		vecSrc = pPev->origin;
		vecSrc.z = vecSrc.z - 10;
		vecSrc = vecSrc - gpGlobals->v_forward * 40;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMPOINTS );


			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );

			WRITE_COORD( tr.vecEndPos.x );
			WRITE_COORD( tr.vecEndPos.y );
			WRITE_COORD( tr.vecEndPos.z );

			WRITE_SHORT ( m_beamSprite );
					
			if (discomode == 1)
			{

				WRITE_BYTE( 1 );  // start frame
				WRITE_BYTE( 10 ); // frame rate
				WRITE_BYTE( 48 ); // life
				WRITE_BYTE( 60 ); // line width
				WRITE_BYTE( 0 ); // noise
			}
			else if (discomode == 2)
			{

				WRITE_BYTE( 1 );  // start frame
				WRITE_BYTE( 10 ); // frame rate
				WRITE_BYTE( 48 ); // life
				WRITE_BYTE( 4 ); // line width
				WRITE_BYTE( 100 ); // noise
			}
			else if (discomode == 3)
			{

				WRITE_BYTE( 1 );  // start frame
				WRITE_BYTE( 10 ); // frame rate
				WRITE_BYTE( 15 ); // life
				WRITE_BYTE( 255 ); // line width
				WRITE_BYTE( 0 ); // noise
			}
			else
			{
				WRITE_BYTE( 1 );  // start frame
				WRITE_BYTE( 10 ); // frame rate
				WRITE_BYTE( 48 ); // life
				WRITE_BYTE( 10 ); // line width
				WRITE_BYTE( 10 ); // noise
			}

			WRITE_BYTE( (int)ColourArray[lastpos].x ); 
			WRITE_BYTE( (int)ColourArray[lastpos].y ); 
			WRITE_BYTE( (int)ColourArray[lastpos].z ); 
			
			WRITE_BYTE( 255 );
			WRITE_BYTE( 10 );

		MESSAGE_END();
	

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_DLIGHT );

			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );

			WRITE_BYTE( 40 ); //radius

			// colour
			WRITE_BYTE( (int)ColourArray[lastpos].x ); 
			WRITE_BYTE( (int)ColourArray[lastpos].y ); 
			WRITE_BYTE( (int)ColourArray[lastpos].z ); 


			//WRITE_BYTE( 200 ); // brightness
			WRITE_BYTE( 2 );  // life
			WRITE_BYTE( 0 );  // decay
		MESSAGE_END();


		lastpos = lastpos + 3;
		angleadd = angleadd + 15;


		pPev->rendermode = 2;
		pPev->renderamt = 1;
		pPev->renderfx = 19;
		pPev->rendercolor = ColourArray[lastpos];

		if (lastpos > 255) lastpos = 0;
		if (angleadd > 360) angleadd = angleadd - 360;

		lastbeamtime = gpGlobals->time + 0.2;

	}


	// Used for the mortar field

	if (MortarFieldEnabled) 
	{

		// See if we are still not thinking

		if (MortarFieldNextThink <= gpGlobals->time)
		{
				
			// Ok, now see if theres someone in our field!

			if (AvCheckIfInField(pEntity)) {

				if (MortarFieldDelay == 0) MortarFieldDelay = 5;
				MortarFieldNextThink = gpGlobals->time + MortarFieldDelay;

				// Now spawn a mortar here.
				
				int pitch = RANDOM_LONG(95,124);

				EMIT_SOUND_DYN2(pEntity, CHAN_AUTO, "weapons/mortar.wav", 1.0, ATTN_NONE, 0, pitch);	

				edict_t	*pent;

				pent = CREATE_NAMED_ENTITY( MAKE_STRING("monster_mortar"));

				TraceResult tr;
				UTIL_TraceLine( pEntity->v.origin + Vector( 0, 0, 1024 ), pEntity->v.origin - Vector( 0, 0, 1024 ), dont_ignore_monsters, pEntity, &tr );

				pent->v.origin = tr.vecEndPos;
				pent->v.angles = Vector( 0, 0, 0 );
				DispatchSpawn( pent );
				pent->v.nextthink = gpGlobals->time + 2.5;
				
			}
		}
	}
}

bool AvCheckIfInField(  edict_t *pEntity )
{

	// Sees if this dude is in our mortar field

	int tot = 0;

	if (MortarFieldMaxs.x > MortarFieldMins.x) {
		if (pEntity->v.origin.x >= MortarFieldMins.x && pEntity->v.origin.x <= MortarFieldMaxs.x) tot++;
	}
	else
	{
		if (pEntity->v.origin.x <= MortarFieldMins.x && pEntity->v.origin.x >= MortarFieldMaxs.x) tot++;
	}

	if (MortarFieldMaxs.y > MortarFieldMins.y) {
		if (pEntity->v.origin.y >= MortarFieldMins.y && pEntity->v.origin.y <= MortarFieldMaxs.y) tot++;
	}
	else
	{
		if (pEntity->v.origin.y <= MortarFieldMins.y && pEntity->v.origin.y >= MortarFieldMaxs.y) tot++;
	}

	if (MortarFieldMaxs.z > MortarFieldMins.z) {
		if (pEntity->v.origin.z >= MortarFieldMins.z && pEntity->v.origin.z <= MortarFieldMaxs.z) tot++;
	}
	else
	{
		if (pEntity->v.origin.z <= MortarFieldMins.z && pEntity->v.origin.z >= MortarFieldMaxs.z) tot++;
	}

	// make sure he isnt a spec and isnt dead

	if (pEntity->v.iuser1 != 0) tot = 0;
	if (pEntity->v.deadflag != 0) tot = 0;


	if (tot == 3) return 1;
	return 0;
		
	
}


void AvPathCommands( edict_t *pEntity, entvars_t *pPev, int MyNumber, const char *arg1, const char *arg2, const char *arg3, const char *arg4, int Cmd)
{

	int num = AvCountPathNums(MyNumber);
	int i = 0;
	char msg[80];


	// This function handles the new PATH CREATION COMMANDS

	if (Cmd == AV_ENT_PATHADD) 
	{
		// Adding a new node to the path.
		// Arg1: the node to add it AFTER, if 0 means add it after the last one.
		// Arg2: the Z offset, good for making a path while walking.

		int whereadd = atoi(arg1);
		int zoff = atoi(arg2);
		int flags = atoi(arg3);
	
		if (whereadd == 0) whereadd = num;
		whereadd++;

		// Since we are sticking this node into the middle, we must shift everything above it up.
	
		if ( num + 1 > whereadd ) {
			for ( i = num + 1; i > whereadd; i--) {
				AdminPath[MyNumber][i].x = AdminPath[MyNumber][i-1].x;
				AdminPath[MyNumber][i].y = AdminPath[MyNumber][i-1].y;
				AdminPath[MyNumber][i].z = AdminPath[MyNumber][i-1].z;
				AdminPath[MyNumber][i].flags = AdminPath[MyNumber][i-1].flags;
			}
		}

		// Ok, we now have a free slot.
		// Mark this position to the player's spot.
		
		AdminPath[MyNumber][whereadd].x = (int)pPev->origin.x;
		AdminPath[MyNumber][whereadd].y = (int)pPev->origin.y;
		AdminPath[MyNumber][whereadd].z = (int)pPev->origin.z + zoff;
		AdminPath[MyNumber][whereadd].flags = flags;


		sprintf(msg,"Added a node to your path, its node %i of %i\n", whereadd, num);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}
	else if (Cmd == AV_ENT_PATHSHOW) 
	{
		// Draw this path using beams between nodes.
	
		for ( i = 1; i < num; i++) {	

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

				WRITE_BYTE( TE_BEAMPOINTS );
				WRITE_COORD( AdminPath[MyNumber][i].x );
				WRITE_COORD( AdminPath[MyNumber][i].y );
				WRITE_COORD( AdminPath[MyNumber][i].z );

				WRITE_COORD( AdminPath[MyNumber][i+1].x );
				WRITE_COORD( AdminPath[MyNumber][i+1].y );
				WRITE_COORD( AdminPath[MyNumber][i+1].z );
				
				WRITE_SHORT( m_beamSprite );
				
				WRITE_BYTE( 1 ); // start frame
				WRITE_BYTE( 10 ); // frame rate
				WRITE_BYTE( 50 ); // life
				WRITE_BYTE( 10 ); // line width
				WRITE_BYTE( 0 ); // noise
				
				WRITE_BYTE( 255 );	// r
				WRITE_BYTE( 0 );	// g
				WRITE_BYTE( 0 );	// b
				
				WRITE_BYTE( 255 ); // brightness
				WRITE_BYTE( 10 ); // scroll speed

			MESSAGE_END();
		}
	}

	if (Cmd == AV_ENT_PATHREM) 
	{
		// Removes a node from the path. If 0, removes last one.

		int whererem = atoi(arg1);
		if (whererem == 0) whererem = num;

		// Since we might be removing a node from the middle, we gotta shift the other ones up.

		if ( whererem < num ) {
			for ( i = whererem; i < num; i++) {
				AdminPath[MyNumber][i].x = AdminPath[MyNumber][i+1].x;
				AdminPath[MyNumber][i].y = AdminPath[MyNumber][i+1].y;
				AdminPath[MyNumber][i].z = AdminPath[MyNumber][i+1].z;
				AdminPath[MyNumber][i].flags = AdminPath[MyNumber][i+1].flags;
			}
		}

		// Ok, we now have a free slot.
		// Mark this position to the player's spot.
		
		AdminPath[MyNumber][num].x = 0;
		AdminPath[MyNumber][num].y = 0;
		AdminPath[MyNumber][num].z = 0;
		AdminPath[MyNumber][num].flags = 0;

		sprintf(msg,"Removed node %i from your path.\n", whererem);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}
	else if (Cmd == AV_ENT_PATHMOVE) 
	{
		// Moving a node.

		int whereadd = atoi(arg1);
	
		if (whereadd == 0) whereadd = num;

		AdminPath[MyNumber][whereadd].x = (int)pPev->origin.x;
		AdminPath[MyNumber][whereadd].y = (int)pPev->origin.y;
		AdminPath[MyNumber][whereadd].z = (int)pPev->origin.z;

		sprintf(msg,"Moved node %i to you\n", whereadd);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	}

	else if (Cmd == AV_ENT_PATHCLEAR) 
	{
		// Clear the path
	
		for ( i = 0; i <= num; i++) {	
			AdminPath[MyNumber][i].x = 0;
			AdminPath[MyNumber][i].y = 0;
			AdminPath[MyNumber][i].z = 0;
			AdminPath[MyNumber][i].flags = 0;
		}
		sprintf(msg,"Path Cleared!\n");
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);				

	}

	else if (Cmd == AV_ENT_PATHMAKE) 
	{
		// Now, it's time to CREATE this path
		// To do this, we need a name for it (arg1)

		//char pathname[80];
		char buf[80];

		for ( i = 1; i <= num; i++) {	

			KeyValueData kvd;
			Vector neworig;
					
			string_t classname = MAKE_STRING("path_corner");

			edict_t *tEntity = CREATE_NAMED_ENTITY(classname);
			
			entvars_t *pNew = VARS(tEntity);
			// Set the various values

			// Set the KEYVALUES here!
			kvd.fHandled = FALSE;
			kvd.szClassName = NULL;
			kvd.szKeyName = "classname";
			kvd.szValue = "path_corner";
			DispatchKeyValue( tEntity, &kvd );
			kvd.fHandled = FALSE;
			
			kvd.szClassName = "path_corner";

			sprintf( buf , "%s_%i", arg1, i);
			
			kvd.szKeyName = "targetname";
			kvd.szValue = buf;
			DispatchKeyValue( tEntity, &kvd );
			kvd.fHandled = FALSE;

			int trg = i + 1;
			if (i == num) trg = 1;


			sprintf( buf , "%s_%i", arg1, trg);
			
			kvd.szKeyName = "target";
			kvd.szValue = buf;
			DispatchKeyValue( tEntity, &kvd );
			kvd.fHandled = FALSE;


			// origin
			neworig.x = AdminPath[MyNumber][i].x;
			neworig.y = AdminPath[MyNumber][i].y;
			neworig.z = AdminPath[MyNumber][i].z;
						
			SET_ORIGIN(ENT(pNew), neworig );
			
			pNew->spawnflags = AdminPath[MyNumber][i].flags;
			
			DispatchSpawn( ENT( pNew ) );
				
		}
		
		sprintf(msg,"Created the path!\n");
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);				
	}
}



int AvCountPathNums( int MyNumber ) 
{
	// returns how many nodes are in this admin's path.
	
	int i = 1;
	int tot = 0;

	for (i; i < 100; i++) 
	{
		if ((AdminPath[MyNumber][i].x + AdminPath[MyNumber][i].y + AdminPath[MyNumber][i].z) != 0) {
			tot++;
		}
	}

	return tot;
}

void AvShowMem( entvars_t *pRunOnPev, int memnum ) 
{

	if (memnum == 0) {
		pRunOnPev->rendercolor.x = 255;
		pRunOnPev->rendercolor.y = 0;
		pRunOnPev->rendercolor.z = 0;
	}
	if (memnum == 1) {
		pRunOnPev->rendercolor.x = 255;
		pRunOnPev->rendercolor.y = 255;
		pRunOnPev->rendercolor.z = 0;
	}
	if (memnum == 2) {
		pRunOnPev->rendercolor.x = 0;
		pRunOnPev->rendercolor.y = 255;
		pRunOnPev->rendercolor.z = 0;
	}
	if (memnum == 3) {
		pRunOnPev->rendercolor.x = 100;
		pRunOnPev->rendercolor.y = 100;
		pRunOnPev->rendercolor.z = 255;
	}
	if (memnum == 4) {
		pRunOnPev->rendercolor.x = 255;
		pRunOnPev->rendercolor.y = 0;
		pRunOnPev->rendercolor.z = 255;
	}
	if (memnum == 5) {
		pRunOnPev->rendercolor.x = 0;
		pRunOnPev->rendercolor.y = 255;
		pRunOnPev->rendercolor.z = 255;
	}

	pRunOnPev->rendermode = kRenderTransColor;
	pRunOnPev->renderamt = 200;

}

int AvRunEnt( edict_t *pEntity , entvars_t *pPev, int MyNumber, edict_t *pRunOn, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int Cmd)
{
	//Actually DOES the crap on the ent. Only messes with the ONE ent.

	char msg[80];

	int ind = ENTINDEX(pRunOn);
	int effected = 0;

	char *pClassname = Av_GetClassname ( pRunOn );

	if (FStrEq(pClassname, "worldspawn")) return 0;
	
	sprintf(msg,"Nothing done to ent %s <%i>\n", pClassname, ind);

	if (Cmd == AV_ENT_MOVE) {

		if (!FStrEq(pClassname, "worldspawn")) {

			// Move this ent.
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			Vector newOrigin;

			float newx = pRunOnPev->origin.x;
			float newy = pRunOnPev->origin.y;
			float newz = pRunOnPev->origin.z;
			
			//UTIL_SetOrigin(pev, pTarg->pev->origin - (pev->mins + pev->maxs)* 0.5);
		
			float realx = (pRunOnPev->mins.x + pRunOnPev->maxs.x)* 0.5;
			float realy = (pRunOnPev->mins.y + pRunOnPev->maxs.y)* 0.5;
			float realz = (pRunOnPev->mins.z + pRunOnPev->maxs.z)* 0.5;

			newx = newx + atoi(arg1);
			newy = newy + atoi(arg2);
			newz = newz + atoi(arg3);

			realx = realx + atoi(arg1);
			realy = realy + atoi(arg2);
			realz = realz + atoi(arg3);

			newOrigin.x = newx;
			newOrigin.y = newy;
			newOrigin.z = newz;

			SET_ORIGIN(ENT(pRunOnPev), newOrigin );

			//sprintf(msg,"Moved ent %s <%i> to %d, %d, %d\n", pClassname, ind, realx, realy, realz);
			effected = 1;
		}
	}
	else if (Cmd == AV_ENT_MOVEREL) {

		if (!FStrEq(pClassname, "worldspawn")) {

			// Move this ent.
			// This move is good because it moves based on the way the admin is facing, so it always puts FORWARD as FORWARD.


			// Figure out which way the admin is facing.
			// since we only want to move at right angles and not diagonally, and also not up and down.

			int vy = (int)pPev->v_angle.y;

			Vector vecForward;
			Vector vecRight;
			Vector vecUp = Vector(0, 0, 1);

			if (vy >= -45 && vy < 45) {
				vecForward = Vector( 1, 0, 0 );
				vecRight = Vector( 0, 1, 0 );
			}
			else if (vy >= 45 && vy < 135) {
				vecForward = Vector( 0, 1, 0 );
				vecRight = Vector( -1, 0, 0 );
			}
			else if (vy >= 135 || vy < -135) {
				vecForward = Vector( -1, 0, 0 );
				vecRight = Vector( 0, -1, 0 );
			}
			else if (vy >= -135 && vy < -45) {
				vecForward = Vector( 0, -1, 0 );
				vecRight = Vector( 1, 0, 0 );
			}

			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			Vector newOrigin;

			newOrigin = pRunOnPev->origin + (vecForward * atof(arg1)) + (vecRight * atof(arg2)) + (vecUp * atof(arg3));
			
			SET_ORIGIN(ENT(pRunOnPev), newOrigin );

			//sprintf(msg,"Moved ent %s <%i> to %d, %d, %d\n", pClassname, ind, realx, realy, realz);
			effected = 1;
		}
	}	
	else if (Cmd == AV_ENT_MOVEVEL) {

		if (!FStrEq(pClassname, "worldspawn")) {

			// Move this ent.
			// This move is good because it moves based on the way the admin is facing, so it always puts FORWARD as FORWARD.


			// Figure out which way the admin is facing.
			// since we only want to move at right angles and not diagonally, and also not up and down.

			int vy = (int)pPev->v_angle.y;

			Vector vecForward;
			Vector vecRight;
			Vector vecUp = Vector(0, 0, 1);

			if (vy >= -45 && vy < 45) {
				vecForward = Vector( 1, 0, 0 );
				vecRight = Vector( 0, 1, 0 );
			}
			else if (vy >= 45 && vy < 135) {
				vecForward = Vector( 0, 1, 0 );
				vecRight = Vector( -1, 0, 0 );
			}
			else if (vy >= 135 || vy < -135) {
				vecForward = Vector( -1, 0, 0 );
				vecRight = Vector( 0, -1, 0 );
			}
			else if (vy >= -135 && vy < -45) {
				vecForward = Vector( 0, -1, 0 );
				vecRight = Vector( 1, 0, 0 );
			}

			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			Vector newOrigin;

			newOrigin = ((pRunOnPev->absmin + pRunOnPev->absmax)* 0.5) + (vecForward * atof(arg1)) + (vecRight * atof(arg2)) + (vecUp * atof(arg3));
			
			//SET_ORIGIN(ENT(pRunOnPev), newOrigin );

			AvLinearMove( pRunOn, newOrigin, 10000 );

			//sprintf(msg,"Moved ent %s <%i> to %d, %d, %d\n", pClassname, ind, realx, realy, realz);
			effected = 1;
		}
	}	

	else if (Cmd == AV_ENT_VELOCITY) {

		if (!FStrEq(pClassname, "worldspawn")) {

			// Set vel
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			UTIL_MakeVectors ( pPev->v_angle );
			Vector vecSrc  = GetGunPosition( pEntity );
			Vector vecEnd  = vecSrc + gpGlobals->v_forward * atoi(arg2);

			if (!FStrEq(arg1, "me")) {
				
				vecEnd.x = atoi(arg1);
				vecEnd.y = atoi(arg2);
				vecEnd.z = atoi(arg3);
			}

			
			//UTIL_MakeVectors(vecEnd);
			//pRunOnPev->movedir = gpGlobals->v_forward;
			

			AvLinearMove( pRunOn, pPev->origin, atof(arg1) );
			

//			pRunOnPev->movedir.x = vecEnd.x;
//			pRunOnPev->movedir.y = vecEnd.y;
//			pRunOnPev->movedir.z = vecEnd.z;
			
			//pRunOnPev->movetype	= MOVETYPE_PUSH;
			//'pRunOnPev->speed = 40;

			//pRunOnPev->velocity.x = vecEnd.x;
			//pRunOnPev->velocity.y = vecEnd.y;
			//pRunOnPev->velocity.z = vecEnd.z;


			effected = 1;
		}
	}
	else if (Cmd == AV_ENT_AVELOCITY) {

		if (!FStrEq(pClassname, "worldspawn")) {

			// Set vel
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

		/*	UTIL_MakeVectors ( pPev->v_angle );
			Vector vecSrc  = GetGunPosition( pEntity );
			Vector vecEnd  = vecSrc + gpGlobals->v_forward * atoi(arg2);

			if (!FStrEq(arg1, "me")) 
			{
				
				vecEnd.x = atof(arg1);
				vecEnd.y = atof(arg2);
				vecEnd.z = atof(arg3);
			}
			*/
			pRunOnPev->avelocity.x = atof(arg1);
			pRunOnPev->avelocity.y = atof(arg2);
			pRunOnPev->avelocity.z = atof(arg3);

			pRunOnPev->movedir = pRunOnPev->avelocity.Normalize();


			//pRunOnPev->sequence = ACT_IDLE;
			
			pRunOnPev->animtime = gpGlobals->time;
			pRunOnPev->nextthink = gpGlobals->time + 0.1;
			
			
			//pRunOnPev->solid = SOLID_NOT;

			pRunOnPev->movetype = MOVETYPE_PUSH;

			effected = 1;
		}
	}

	else if (Cmd == AV_ENT_MOVETOME) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

			// Move this ent to the same loc as the admin.
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			Vector newOrigin;

			// the relative to 0 coords
			float newx = pRunOnPev->origin.x;
			float newy = pRunOnPev->origin.y;
			float newz = pRunOnPev->origin.z;

			// the real co-ords
			float realx = (pRunOnPev->absmin.x + pRunOnPev->absmax.x)* 0.5;
			float realy = (pRunOnPev->absmin.y + pRunOnPev->absmax.y)* 0.5;
			float realz = (pRunOnPev->absmin.z + pRunOnPev->absmax.z)* 0.5;

			// Get the DIFFERENCE
			float rnewx = pPev->origin.x - realx;
			float rnewy = pPev->origin.y - realy;
			float rnewz = pPev->origin.z - realz;
			
			// add to the old co-ords
			newOrigin.x = newx + rnewx;
			newOrigin.y = newy + rnewy;
			newOrigin.z = newz + rnewz;

			SET_ORIGIN(ENT(pRunOnPev), newOrigin );

			//sprintf(msg,"Moved ent %s <%i> to %d, %d, %d\n", pClassname, ind, realx, realy, realz);
			effected = 1;
		}
	
	}
	else if (Cmd == AV_ENT_ROT) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

			// Set vel
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			pRunOnPev->angles.x += atoi(arg1);
			pRunOnPev->angles.y += atoi(arg2);
			pRunOnPev->angles.z += atoi(arg3);

			effected = 1;
		}

	}
	else if (Cmd == AV_ENT_CIRCLE) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

			// Set vel
			// Add to marker array

			markerTempArr[ENTINDEX(pRunOn)] = 1;
			effected = 1;
		}

	}
	else if (Cmd == AV_ENT_STOP) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

			// Set vel
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			pRunOnPev->velocity.x = 0;
			pRunOnPev->velocity.y = 0;
			pRunOnPev->velocity.z = 0;
			pRunOnPev->avelocity.x = 0;
			pRunOnPev->avelocity.y = 0;
			pRunOnPev->avelocity.z = 0;

			effected = 1;
		}

	}	
	else if (Cmd == AV_ENT_MAKECOPY) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

			// Set vel
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			// make a basic copy of this guy
					
			edict_t *tEntity;
			if (strlen(arg1) == 0)
			{
				tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("func_wall"));
			}
			else
			{
				tEntity = CREATE_NAMED_ENTITY(MAKE_STRING(arg1));
			}
			
			if (!FNullEnt(tEntity))
			{


				EntData[ENTINDEX(tEntity)].numkeys = 0;
							
				entvars_t *pNew;
				pNew =  VARS(tEntity);

				pNew->origin = pRunOnPev->origin;
				pNew->modelindex = pRunOnPev->modelindex;
				pNew->model = pRunOnPev->model;
				pNew->speed = pRunOnPev->speed;
				pNew->dmg = 100;

				entcopy = tEntity;
				effected = 1;
			}
		}

	}	
	else if (Cmd == AV_ENT_MAKEDOOR) {
		
		if (!FStrEq(pClassname, "worldspawn") && !FStrEq(pClassname, "player")) {

			// Set vel
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			// make a basic copy of this guy
			//				          1      2       3       4    5      6
			// avc emakedoor front <wait> <speed> <angles> <ang> <Ang> <lip>
		
			int wait = atoi(arg1);
			int speed = atoi(arg2);
			int lip = atoi(arg6);

			if (wait == 0) wait = 1;
			if (speed == 0) speed = 1;

			edict_t *tEntity;
			tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("func_door"));
			
			entvars_t *pNew;
			pNew =  VARS(tEntity);

			pNew->origin = pRunOnPev->origin;
			pNew->modelindex = pRunOnPev->modelindex;
			pNew->model = pRunOnPev->model;
			pNew->speed = pRunOnPev->speed;
			pNew->rendermode = pRunOnPev->rendermode;
			pNew->renderamt = pRunOnPev->renderamt;
			pNew->renderfx = pRunOnPev->renderfx;
			pNew->rendercolor.x = pRunOnPev->rendercolor.x;
			pNew->rendercolor.y = pRunOnPev->rendercolor.y;
			pNew->rendercolor.z = pRunOnPev->rendercolor.z;
		
			
			pNew->dmg = 100;

			entcopy = tEntity;
			effected = 1;

			KeyValueData	kvd;

			// Set the KEYVALUES here!


			char keyvalue[120];

			sprintf(keyvalue, "%i", wait);

			kvd.fHandled = FALSE;
			kvd.szClassName = "func_door";
			kvd.szKeyName = "classname";
			kvd.szValue = "func_door";
			DispatchKeyValue( tEntity, &kvd );			
			DispatchSpawn(tEntity);

			sprintf(keyvalue, "%i", wait);

			kvd.fHandled = FALSE;
			kvd.szClassName = "func_door";
			kvd.szKeyName = "wait";
			kvd.szValue = keyvalue;
			DispatchKeyValue( tEntity, &kvd );
			
			sprintf(keyvalue, "%i", speed);

			kvd.fHandled = FALSE;
			kvd.szClassName = "func_door";
			kvd.szKeyName = "speed";
			kvd.szValue = keyvalue;
			DispatchKeyValue( tEntity, &kvd );

			sprintf(keyvalue, "%i", lip);

			kvd.fHandled = FALSE;
			kvd.szClassName = "func_door";
			kvd.szKeyName = "lip";
			kvd.szValue = keyvalue;
			DispatchKeyValue( tEntity, &kvd );

			sprintf(keyvalue, "%s %s %s", arg3, arg4, arg5);

			kvd.fHandled = FALSE;
			kvd.szClassName = "func_door";
			kvd.szKeyName = "angles";
			kvd.szValue = keyvalue;
			DispatchKeyValue( tEntity, &kvd );

			DispatchSpawn(tEntity);
			pRunOn->v.flags |= FL_KILLME;
		}

	}	
	else if (Cmd == AV_ENT_SHOWBOX) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

			// Set vel
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			pathdata_t mn;
			pathdata_t mx;
			
			mn.x = (int)pRunOnPev->absmin.x;
			mn.y = (int)pRunOnPev->absmin.y;
			mn.z = (int)pRunOnPev->absmin.z;

			mx.x = (int)pRunOnPev->absmax.x;
			mx.y = (int)pRunOnPev->absmax.y;
			mx.z = (int)pRunOnPev->absmax.z;



			// bottom square

			AvMakeBeam3( mn.x, mn.y, mn.z, mn.x, mx.y, mn.z); // point 1 -> point 2
			AvMakeBeam3( mn.x, mn.y, mn.z, mx.x, mn.y, mn.z); // point 1 -> point 4
		
			AvMakeBeam3( mn.x, mx.y, mn.z, mx.x, mx.y, mn.z); // point 2 -> point 3
			AvMakeBeam3( mx.x, mn.y, mn.z, mx.x, mx.y, mn.z); // point 4 -> point 3

			// Top square:

			AvMakeBeam3( mn.x, mn.y, mx.z, mn.x, mx.y, mx.z); // point 5 -> point 6
			AvMakeBeam3( mn.x, mn.y, mx.z, mx.x, mn.y, mx.z); // point 5 -> point 8
			
			AvMakeBeam3( mn.x, mx.y, mx.z, mx.x, mx.y, mx.z); // point 6 -> point 7
			AvMakeBeam3( mx.x, mn.y, mx.z, mx.x, mx.y, mx.z); // point 8 -> point 7

			// Sides

			AvMakeBeam3( mn.x, mn.y, mn.z, mn.x, mn.y, mx.z); // point 1 -> point 5
			AvMakeBeam3( mx.x, mx.y, mn.z, mx.x, mx.y, mx.z); // point 3 -> point 7
			AvMakeBeam3( mn.x, mx.y, mn.z, mn.x, mx.y, mx.z); // point 2 -> point 6
			AvMakeBeam3( mx.x, mn.y, mn.z, mx.x, mn.y, mx.z); // point 4 -> point 8
			



			effected = 1;
		}
	}	
	else if (Cmd == AV_ENT_MUSIC) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

/*
			if (FStrEq(arg1, "get"))
			{
				
					//fp=fopen("private.txt","a");
					//fprintf(fp, "Beginning Maze Spawn... ID: %i\n", cm);
					//fwrite(pRunOn->pvPrivateData, 1, sizeof(pRunOn->pvPrivateData), fp);
					//fclose(fp);	

					CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pRunOn);
					pEntity->MyOwnThink = pEntity->m_pfnThink;



			}
			if (FStrEq(arg1, "set"))
			{
				
					//fp=fopen("private.txt","a");
					//fprintf(fp, "Beginning Maze Spawn... ID: %i\n", cm);
					//fwrite(pRunOn->pvPrivateData, 1, sizeof(pRunOn->pvPrivateData), fp);
					//fclose(fp);	

					CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pRunOn);
					//pEntity->m_pfnThink = &CBaseEntity::MyOwnThink;
					pEntity->m_pfnThink = pEntity->MyOwnThink;
					pRunOn->v.nextthink = gpGlobals->time + 0.1;

			}

*/
			
			if (atoi(arg1) == 1) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP1, 0.98, ATTN_STATIC, 0, 100); 
			if (atoi(arg1) == 2) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP2, 0.98, ATTN_STATIC, 0, 100); 
			if (atoi(arg1) == 3) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP3, 0.98, ATTN_STATIC, 0, 100); 
			if (atoi(arg1) == 4) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP4, 0.98, ATTN_STATIC, 0, 100); 
			if (atoi(arg1) == 5) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP5, 0.98, ATTN_STATIC, 0, 100); 
			if (atoi(arg1) == 6) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP6, 0.98, ATTN_STATIC, 0, 100); 
			if (atoi(arg1) == 7) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP7, 0.98, ATTN_STATIC, 0, 100); 
			if (atoi(arg1) == 8) EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP8, 0.98, ATTN_STATIC, 0, 100); 
	
			effected = 1;
		}
	}	
	else if (Cmd == AV_ENT_MSTOP) {
		
		if (!FStrEq(pClassname, "worldspawn")) {

			
			//UTIL_EmitAmbientSound(pRunOn, pRunOn->v.origin, RADLOOP1, 0, 0, SND_STOP, 0);
			EMIT_SOUND_DYN2(pRunOn, CHAN_WEAPON, RADLOOP1, 0, ATTN_STATIC, SND_STOP, 0); 

			effected = 1;
		}
	}	
	else if (Cmd == AV_ENT_RADIOSONG) {
		
		if (FStrEq(pClassname, "building_radio")) {

			if (atoi(arg1) >= 1 && atoi(arg1) <= 10)
			{

				RadioSwitchToSong(atoi(arg1), ENTINDEX(pRunOn));

				effected = 1;
			}
		}
	}
	else if (Cmd == AV_ENT_RADIOPITCH) {
		
		if (FStrEq(pClassname, "building_radio")) {

			if (atoi(arg1) >= 0 && atoi(arg1) <= 200)
			{

				RadioChangeSongPitch(atoi(arg1), pRunOn);

				effected = 1;
			}
		}
	}
	else if (Cmd == AV_ENT_INFO) {
			
		// Some info on this ent

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		int orix = (int)(pRunOnPev->absmin.x + pRunOnPev->absmax.x)* 0.5;
		int oriy = (int)(pRunOnPev->absmin.y + pRunOnPev->absmax.y)* 0.5;
		int oriz = (int)(pRunOnPev->absmin.z + pRunOnPev->absmax.z)* 0.5;
	
		sprintf(msg,"ENT INFO ON ENT NUMBER %i:\nClassname: %s\n", ind, pClassname);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Origin: %i, %i, %i\n\n", orix, oriy, oriz);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
		

		sprintf(msg,"Target: %s\n", (char *)STRING(pRunOnPev->target));
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Targetname: %s\n", (char *)STRING(pRunOnPev->targetname));
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Netname: %s\n", (char *)STRING(pRunOnPev->netname));
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);


		sprintf(msg,"Rendermode: %i\nRenderamt:%i\nRenderFX:%i\n", (int)pRunOnPev->rendermode, (int)pRunOnPev->renderamt, (int)pRunOnPev->renderfx);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		effected = 1;
	}
	else if (Cmd == AV_ENT_MOREINFO) {
		
		// Some info on this ent

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		int orix = (int)(pRunOnPev->absmin.x + pRunOnPev->absmax.x)* 0.5;
		int oriy = (int)(pRunOnPev->absmin.y + pRunOnPev->absmax.y)* 0.5;
		int oriz = (int)(pRunOnPev->absmin.z + pRunOnPev->absmax.z)* 0.5;
	
		sprintf(msg,"MORE ENT INFO ON ENT NUMBER %i:\nClassname: %s\n", ind, pClassname);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Origin: %i, %i, %i\n\n", orix, oriy, oriz);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Avel: %i, %i, %i\n\n", (int)pRunOnPev->avelocity.x, (int)pRunOnPev->avelocity.y, (int)pRunOnPev->avelocity.z);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);


		sprintf(msg,"Target: %s\n", (char *)STRING(pRunOnPev->target));
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Targetname: %s\n", (char *)STRING(pRunOnPev->targetname));
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Netname: %s\n", (char *)STRING(pRunOnPev->netname));
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Model: %s\n", (char *)STRING(pRunOnPev->model));
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"ModelIndex: %i\n", pRunOnPev->modelindex);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Movetype: %i\n", pRunOnPev->movetype);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"Solid: %i\n", pRunOnPev->solid);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);


		sprintf(msg,"Body: %i     Skin: %i\n", pRunOnPev->body, pRunOnPev->skin);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		if (EntData[ind].hasorigin == 1) sprintf(msg,"Has Origin: Yes\n");
		if (EntData[ind].hasorigin == 0) sprintf(msg,"Has Origin: No\n");
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);


		sprintf(msg,"Rendermode: %i\nRenderamt:%i\nRenderFX:%i\n", (int)pRunOnPev->rendermode, (int)pRunOnPev->renderamt, (int)pRunOnPev->renderfx);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		sprintf(msg,"iUsers: 1: %i   2: %i   3: %i   4: %i\n", pRunOnPev->iuser1, pRunOnPev->iuser2, pRunOnPev->iuser3, pRunOnPev->iuser4);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
		sprintf(msg,"fUsers: 1: %f   2: %f   3: %f   4: %f\n", pRunOnPev->fuser1, pRunOnPev->fuser2, pRunOnPev->fuser3, pRunOnPev->fuser4);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
		
		// new key stuff

		int mynum = ENTINDEX(pRunOn);

		int tot = EntData[mynum].numkeys;
		sprintf(msg, "\n\n%i KEYS AND VALUES:\n", tot);
		ClientPrint( pPev, HUD_PRINTNOTIFY, msg);

		
		for (int i = 1; i <= tot; i++)
		{
			sprintf(msg, "%s = %s\n", Av_GetKeyName(EntData[mynum].keyindex[i])  , Av_GetKeyValue(EntData[mynum].keyindex[i])  );
			ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
		}

		effected = 1;
	}
	


	else if (Cmd == AV_ENT_SHOWPOS) {
		
		// Some info on this ent

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		int orix = (int)(pRunOnPev->absmin.x + pRunOnPev->absmax.x)* 0.5;
		int oriy = (int)(pRunOnPev->absmin.y + pRunOnPev->absmax.y)* 0.5;
		int oriz = (int)(pRunOnPev->absmin.z + pRunOnPev->absmax.z)* 0.5;
	
		// draw line

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMPOINTS );

			WRITE_COORD( pPev->origin.x );
			WRITE_COORD( pPev->origin.y );
			WRITE_COORD( pPev->origin.z );

			WRITE_COORD( orix );
			WRITE_COORD( oriy );
			WRITE_COORD( oriz );

			WRITE_SHORT ( m_beamSprite );
					
			WRITE_BYTE( 1 ); 
			WRITE_BYTE( 10 );
			WRITE_BYTE( 255 );  // life
			WRITE_BYTE( 10 );
			WRITE_BYTE( 0 ); 
			
			AvWriteColor("255", "0", "255");
			
			WRITE_BYTE( 255 );
			WRITE_BYTE( 10 );

		MESSAGE_END();


		effected = 1;
	}
	else if (Cmd == AV_ENT_MOVEMETO) {
		
		// Some info on this ent

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		int orix = (int)(pRunOnPev->absmin.x + pRunOnPev->absmax.x)* 0.5;
		int oriy = (int)(pRunOnPev->absmin.y + pRunOnPev->absmax.y)* 0.5;
		int oriz = (int)(pRunOnPev->absmin.z + pRunOnPev->absmax.z)* 0.5;

		Vector newPos = (pRunOnPev->absmin + pRunOnPev->absmax) * 0.5;

		SET_ORIGIN( ENT(pPev), newPos );

		effected = 1;
	}
	else if (Cmd == AV_ENT_LIST) {
		
		//list format:
		// <num>: <class> - <targetname> - <target>

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);
	
		int nogo = 0;
		if (atoi(arg1) > 0 && ind < atoi(arg1)) nogo = 1;
	
		if (nogo == 0) {


			sprintf(msg,"%i: %s - %s - %s\n", ind, pClassname, (char *)STRING(pRunOnPev->targetname), (char *)STRING(pRunOnPev->target));
			ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
			
			effected = 1;
		}
	}
	else if (Cmd == AV_ENT_DUPLICATE) { 
		
		// Attempt at duplication!
	
		// format:
		// avc edup

		if (!FStrEq(pClassname, "player"))
		{

			edict_t *tEntity;
			tEntity = CREATE_NAMED_ENTITY(MAKE_STRING(pClassname));
			

			if (!FNullEnt(tEntity))
			{

				entvars_t *pNew;
				pNew =  VARS(tEntity);

				entvars_t *pRunOnPev;
				pRunOnPev =  VARS(pRunOn);
				// place this in front

				UTIL_MakeVectors ( pPev->v_angle );
				Vector vecSrc  = GetGunPosition( pEntity );
				Vector vecEnd  = vecSrc + gpGlobals->v_forward * 48;

				pNew->origin = pRunOnPev->origin;
				pNew->modelindex = pRunOnPev->modelindex;
				pNew->model = pRunOnPev->model;

				pNew->speed = pRunOnPev->speed;
				pNew->movedir = pRunOnPev->movedir;

				pNew->solid = pRunOnPev->solid;
				pNew->skin = pRunOnPev->skin;
				pNew->body = pRunOnPev->body;
				pNew->effects = pRunOnPev->effects;
				pNew->takedamage = pRunOnPev->takedamage;

				pNew->spawnflags = pRunOnPev->spawnflags;
				pNew->flags = pRunOnPev->flags;
				pNew->team = pRunOnPev->team;
				pNew->dmg = pRunOnPev->dmg;

				//if (FStrEq(arg1, "copy") || )
				//{
					// copy all keys and values

					//char keyname[80];
					//char keyvalue[120];
					
				int p = ENTINDEX(pRunOn);

				KeyValueData	kvd;
				char keyname[80];
				char keyvalue[120];
				bool first = 0;

				for (int n = 1; n <= EntData[p].numkeys; n++)
				{

					
					sprintf(keyname, "%s", Av_GetKeyName( EntData[p].keyindex[n])  );
					sprintf(keyvalue, "%s", Av_GetKeyValue( EntData[p].keyindex[n]));

					if (!FStrEq(keyname, "origin") && !FStrEq(keyname, "classname"))
					{

						kvd.fHandled = FALSE;
						kvd.szClassName = pClassname;
						kvd.szKeyName = keyname;
						kvd.szValue = keyvalue;
						
						EntData[p].addingkey = 0;

						if (first) EntData[p].addingkey = 1;
						first = 1;

						DispatchKeyValue( tEntity, &kvd );
					}
				}

	/*
				if (EntData[ind].alreadydone == 1) {

					// This ent is currently being shown as part of the memory...
					// so dont copy the memory-set render values.

					pNew->rendermode = EntData[ind].rendermode ;
					pNew->renderamt = EntData[ind].renderamt ;
					pNew->renderfx = EntData[ind].renderfx ;
					pNew->rendercolor.x = EntData[ind].rendercolorx ;
					pNew->rendercolor.y = EntData[ind].rendercolory ;
					pNew->rendercolor.z = EntData[ind].rendercolorz ;

				}
				else
				{
	*/
					pNew->rendermode = pRunOnPev->rendermode;
					pNew->renderamt = pRunOnPev->renderamt;
					pNew->renderfx = pRunOnPev->renderfx;
					pNew->rendercolor = pRunOnPev->rendercolor;
	/*
				}
	*/
					
				EntModified[ENTINDEX(tEntity)] = 1;
				
				if (FStrEq(arg1, "target")) {
					pNew->targetname = pRunOnPev->targetname;
					pNew->target = pRunOnPev->target;
				}
				
				DispatchSpawn( ENT( pNew ) );

				pNew->angles.x = pRunOnPev->angles.x;
				pNew->angles.y = pRunOnPev->angles.y;
				pNew->angles.z = pRunOnPev->angles.z;

				SET_ORIGIN(ENT(pNew), pRunOnPev->origin);



				effected = 1;
				
				int ind2 = ENTINDEX(tEntity);
				

				if (FStrEq(arg2, "swap") || FStrEq(arg1, "swap")) {
					// swap this one in mem for the other one
					int j = 0;
					int theflag = 0;

					EntData[ind2].justduped = 1;

					for (j; j < 6; j++) {

						if (AdminMems[MyNumber][j][ind] == 1) {
						
							AdminMems[MyNumber][j][ind] = 0;
							AdminMems[MyNumber][j][ind2] = 1;

							// We may need to show this one and hide the other one...

							// First hide the one we just removed from the memory!
	/*
							
							theflag = 0;
							if (j == 0) theflag = AV_FLAG_SHOWMEM0;
							if (j == 1) theflag = AV_FLAG_SHOWMEM1;
							if (j == 2) theflag = AV_FLAG_SHOWMEM2;
							if (j == 3) theflag = AV_FLAG_SHOWMEM3;
							if (j == 4) theflag = AV_FLAG_SHOWMEM4;
							if (j == 5) theflag = AV_FLAG_SHOWMEM5;

							if (AdminFlags[MyNumber] & theflag) {

								pRunOnPev->rendercolor.x = EntData[ind].rendercolorx;
								pRunOnPev->rendercolor.y = EntData[ind].rendercolory;
								pRunOnPev->rendercolor.z = EntData[ind].rendercolorz;
								pRunOnPev->rendermode = EntData[ind].rendermode;
								pRunOnPev->renderamt = EntData[ind].renderamt;
								pRunOnPev->renderfx = EntData[ind].renderfx;

								EntData[ind].alreadydone = 0;

								// Now SHOW the one we just copied!

								if (EntData[ind2].alreadydone == 0) {
									
									EntData[ind2].rendermode = (int)pNew->rendermode;
									EntData[ind2].renderamt = (float)pNew->renderamt;
									EntData[ind2].renderfx = (int)pNew->renderfx;
									EntData[ind2].rendercolorx = (int)pNew->rendercolor.x;
									EntData[ind2].rendercolory = (int)pNew->rendercolor.y;
									EntData[ind2].rendercolorz = (int)pNew->rendercolor.z;
								}

								EntData[ind2].alreadydone = 1;
								// Set colors based on memory number... 

								AvShowMem( pNew, j );

							}
	*/
						
						}

						

					}	
					
					AvUpdateShowMem();
				}
			}
		}

	}
	else if (Cmd == AV_ENT_USE) {
		// Use this ent.
		if (!FStrEq(pClassname, "worldspawn") && !FStrEq(pClassname, "player")) {
			
			if (FStrEq(arg1, "notme")) {
				DispatchUse(pRunOn, NULL);
			}
			else
			{
				DispatchUse(pRunOn, pEntity);
			}
			
			//sprintf(msg,"Used ent %s <%i>\n", pClassname, ind);
			effected = 1;
		}
	}
	else if (Cmd == AV_ENT_SPAWNAGAIN) {
		// Use this ent.
		if (!FStrEq(pClassname, "worldspawn") && !FStrEq(pClassname, "player")) {
			
			pRunOn->v.effects &= ~EF_NODRAW;


			// key it the angles

			if (FStrEq(pClassname, "func_door") || FStrEq(pClassname, "func_button") || FStrEq(pClassname, "func_water") || FStrEq(pClassname, "trigger_push")) 
			{

				KeyValueData	kvd;
				char keyname[80];
				char keyvalue[120];

				int p = ENTINDEX(pRunOn);

				for (int n = 1; n <= EntData[p].numkeys; n++)
				{
					
					sprintf(keyname, "%s", Av_GetKeyName( EntData[p].keyindex[n])  );

					if (FStrEq(keyname, "angles"))
					{
						sprintf(keyvalue, "%s", Av_GetKeyValue( EntData[p].keyindex[n]));

						kvd.fHandled = FALSE;
						kvd.szClassName = pClassname;
						kvd.szKeyName = keyname;
						kvd.szValue = keyvalue;
						
						EntData[p].addingkey = 1;

						DispatchKeyValue( pRunOn, &kvd );
					}
				}
			}


			DispatchSpawn(pRunOn);
			
			//sprintf(msg,"Used ent %s <%i>\n", pClassname, ind);
			effected = 1;
		}
	}

	
	else if (Cmd == AV_ENT_KEYVALUE) {
		// send this ent a KEYVALUE
		if (!FStrEq(pClassname, "worldspawn")) {


			if (FStrEq(arg1, "iuser1"))
			{
				pRunOn->v.iuser1 = atoi(arg2);
			}
			else if (FStrEq(arg1, "iuser2"))
			{
				pRunOn->v.iuser2 = atoi(arg2);
			}
			else if (FStrEq(arg1, "iuser3"))
			{
				pRunOn->v.iuser3 = atoi(arg2);
			}
			else if (FStrEq(arg1, "iuser4"))
			{
				pRunOn->v.iuser4 = atoi(arg2);
			}
			else if (FStrEq(arg1, "fuser1"))
			{
				pRunOn->v.fuser1 = atof(arg2);
			}
			else if (FStrEq(arg1, "fuser2"))
			{
				pRunOn->v.fuser2 = atof(arg2);
			}
			else if (FStrEq(arg1, "fuser3"))
			{
				pRunOn->v.fuser3 = atof(arg2);
			}
			else if (FStrEq(arg1, "fuser4"))
			{
				pRunOn->v.fuser4 = atof(arg2);
			}
			else
				{

				KeyValueData	kvd;

				// Set the KEYVALUES here!
				
				char keyname[80];
				char keyvalue[120];
				
				sprintf(keyname, "%s", arg1);
				sprintf(keyvalue, "%s", arg2);
				if (strlen(arg3) > 0) sprintf(keyvalue, "%s %s", keyvalue, arg3);
				if (strlen(arg4) > 0) sprintf(keyvalue, "%s %s", keyvalue, arg4);
				if (strlen(arg5) > 0) sprintf(keyvalue, "%s %s", keyvalue, arg5);
				if (strlen(arg6) > 0) sprintf(keyvalue, "%s %s", keyvalue, arg6);


				kvd.fHandled = FALSE;
				kvd.szClassName = pClassname;
				kvd.szKeyName = keyname;
				kvd.szValue = keyvalue;

				EntData[ENTINDEX(pRunOn)].addingkey = 1;

				DispatchKeyValue( pRunOn, &kvd );
			}						
			
			if (FStrEq(arg1, "iuser4") && FStrEq(arg2, "me"))
			{
				pRunOn->v.iuser4 = (int)MyNumber;
			}
			effected = 1;
		}
	}	
	else if (Cmd == AV_ENT_TOUCH) {
		// Touch this ent.
		if (!FStrEq(pClassname, "worldspawn") && !FStrEq(pClassname, "player")) {
			(*other_gFunctionTable.pfnTouch)(pRunOn, pEntity);
			//sprintf(msg,"Touched ent %s <%i>\n", pClassname, ind);
			effected = 1;
		}

	}
	else if (Cmd == AV_ENT_KILL) {
		// Kill this ent.
		
		if (!FStrEq(pClassname, "worldspawn") && !FStrEq(pClassname, "player")) {
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);
			
			// wait, what if its a XEN_TREE? Look for the TTRIGGER and kill that one too.
			
			if (FStrEq(pClassname, "xen_tree")) 
			{
			
				int i = 1;
				edict_t *frontEnt;
				
				for (i; i < 1025; i++) {

					frontEnt = INDEXENT ( i );
					if (frontEnt) {
						
						if (frontEnt->v.owner == pRunOn) {
							SetBits( frontEnt->v.flags , FL_KILLME);
						}
					}
				}
			}

			
			SetBits( pRunOnPev->flags , FL_KILLME);

			//sprintf(msg,"Killed ent %s <%i>\n", pClassname, ind);
			effected = 1;

			// remove from memory
			int j = 0;

			for (j; j < 6; j++) {
				AdminMems[MyNumber][j][ind] = 0;
			}
			
			EntModified[ind] = 0;
			EntData[ind].alreadydone = 0;

		}
	}
	else if (Cmd == AV_ENT_HIDE) {
		// Hide the ent from view, like a func_wall_toggle

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);
		
		pRunOnPev->solid = SOLID_NOT;
		pRunOnPev->effects |= EF_NODRAW;
		//sprintf(msg,"Hid ent %s <%i>\n", pClassname, ind);
		effected = 1;


	}
	else if (Cmd == AV_ENT_SHOW) {

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);
		
		pRunOnPev->solid = SOLID_BSP;
		pRunOnPev->effects &= ~EF_NODRAW;
		//sprintf(msg,"Revealed ent %s <%i>\n", pClassname, ind);
		effected = 1;

	}
	else if (Cmd == AV_ENT_SHOWTRIGGER) {

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);
		
		if (atoi(arg1) == 1) pRunOnPev->effects &= ~EF_NODRAW;
		if (atoi(arg1) == 0) pRunOnPev->effects |= EF_NODRAW;

		effected = 1;

	}

	else if (Cmd == AV_ENT_SETRENDER) {
		// arg1 = render mode
		// arg2 = render amount
		// arg3 = render fx
		
		int new1 = -1;
		int new2 = -1;
		int new3 = -1;

		if	(FStrEq(arg1, "normal") || FStrEq(arg1, "0"))  new1 = kRenderNormal;
		if	(FStrEq(arg1, "color") || FStrEq(arg1, "1"))  new1 = kRenderTransColor;
		if	(FStrEq(arg1, "texture") || FStrEq(arg1, "2"))  new1 = kRenderTransTexture;
		if	(FStrEq(arg1, "glow") || FStrEq(arg1, "3"))  new1 = kRenderGlow;
		if	(FStrEq(arg1, "solid") || FStrEq(arg1, "4"))  new1 = kRenderTransAlpha;
		if	(FStrEq(arg1, "additive") || FStrEq(arg1, "5"))  new1 = kRenderTransAdd;

		new2 = atoi(arg2);
		new3 = atoi(arg3);

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		if (new1 > -1) {
			pRunOnPev->rendermode = new1;
		}

		if (new2 > -1) {
			pRunOnPev->renderamt = new2;
		}
		
		if (new3 > -1) {
			pRunOnPev->renderfx = new3;
		}

		//sprintf(msg,"Set render properties for ent %s <%i>\n", pClassname, ind);
		effected = 1;

		
	}
	else if (Cmd == AV_ENT_SETCOLOR) {
		// arg1 = render mode
		// arg2 = render amount
		// arg3 = render fx
		
		int new1 = -1;
		int new2 = -1;
		int new3 = -1;

		new1 = atoi(arg1);
		new2 = atoi(arg2);
		new3 = atoi(arg3);

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		if (new1 > 255) new1 = 255;
		if (new2 > 255) new2 = 255;
		if (new3 > 255) new3 = 255;

		if (new1 < 0) new1 = 0;
		if (new2 < 0) new2 = 0;
		if (new3 < 0) new3 = 0;

		if (FStrEq(arg1, "rand")) {
			new1 = (int)RANDOM_FLOAT(0,255);
			new2 = (int)RANDOM_FLOAT(0,255);
			new3 = (int)RANDOM_FLOAT(0,255);
		}
		if (FStrEq(arg1, "rain")) {
			new1 = (int)ColourArray[lastpos].x;
			new2 = (int)ColourArray[lastpos].y;
			new3 = (int)ColourArray[lastpos].z;
		}
		if (new1 > -1) {
			pRunOnPev->rendercolor.x = new1;
			pRunOnPev->rendercolor.y = new2;
			pRunOnPev->rendercolor.z = new3;
		}


		//sprintf(msg,"Set render color for ent %s <%i>\n", pClassname, ind);
		effected = 1;

		
	}
	else if (Cmd == AV_ENT_ALWAYS) 
	{

		// arg1 -> 1 = solid, 0 = not solid

		if	(FStrEq(arg1, "on")) 
		{
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);
			
			pRunOnPev->flags |= FL_ALWAYS;
			effected = 1;
		}
		else
		{
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);
			
			pRunOnPev->flags &= ~FL_ALWAYS;
			effected = 1;
		}
	}
	else if (Cmd == AV_ENT_SETSOLID) 
	{

		// arg1 -> 1 = solid, 0 = not solid

		if	(FStrEq(arg1, "1")) {
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);
			
			pRunOnPev->solid = SOLID_BSP;
			//sprintf(msg,"Set ent to solid, %s <%i>\n", pClassname, ind);
			effected = 1;
		}
		else if	(FStrEq(arg1, "2")) {
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);
			
			pRunOnPev->solid = SOLID_BBOX;
			//sprintf(msg,"Set ent to solid, %s <%i>\n", pClassname, ind);
			effected = 1;
		}
		else if	(FStrEq(arg1, "4")) {
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);
			
			pRunOnPev->solid = SOLID_BSP;
			pRunOnPev->movetype = MOVETYPE_PUSH;
			//sprintf(msg,"Set ent to solid, %s <%i>\n", pClassname, ind);
			effected = 1;
		}
		else {
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);
			
			pRunOnPev->solid = SOLID_NOT;
			//sprintf(msg,"Set ent to non-solid, %s <%i>\n", pClassname, ind);
			effected = 1;
		}

	}
	else if (Cmd == AV_ENT_ADDTOMEM) {
		
		// Add this ent to the mem.

		int memnum = atoi(arg1);
		if (memnum < 0 || memnum > 5) {
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum);
		}
		else if (!FStrEq(pClassname, "worldspawn")) {
			AdminMems[MyNumber][memnum][ind] = 1;
			//sprintf(msg,"Added ent to memory number %i: %s <%i>\n", memnum, pClassname, ind);


			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			// We may need to show it!
/*
			int theflag = 0;
			if (memnum == 0) theflag = AV_FLAG_SHOWMEM0;
			if (memnum == 1) theflag = AV_FLAG_SHOWMEM1;
			if (memnum == 2) theflag = AV_FLAG_SHOWMEM2;
			if (memnum == 3) theflag = AV_FLAG_SHOWMEM3;
			if (memnum == 4) theflag = AV_FLAG_SHOWMEM4;
			if (memnum == 5) theflag = AV_FLAG_SHOWMEM5;
		
			if (AdminFlags[MyNumber] & theflag) {

				if (EntData[ind].alreadydone == 0) {
					
					EntData[ind].rendermode = (int)pRunOnPev->rendermode;
					EntData[ind].renderamt = (float)pRunOnPev->renderamt;
					EntData[ind].renderfx = (int)pRunOnPev->renderfx;
					EntData[ind].rendercolorx = (int)pRunOnPev->rendercolor.x;
					EntData[ind].rendercolory = (int)pRunOnPev->rendercolor.y;
					EntData[ind].rendercolorz = (int)pRunOnPev->rendercolor.z;
				}

				EntData[ind].alreadydone = 1;
				// Set colors based on memory number... 
			
				AvShowMem( pRunOnPev, memnum );

			}
*/
			AvUpdateShowMem();

			effected = 1;
		}
	}
	else if (Cmd == AV_ENT_REMFROMMEM) {

		// remove this ent from the mem.
		int memnum = atoi(arg1);
		if (memnum < 0 || memnum > 5) {
			sprintf(msg,"* Invalid memory numer (%i). Valid numbers are from 0 to 5.\n", memnum);
		}
		else if (!FStrEq(pClassname, "worldspawn")) {
			AdminMems[MyNumber][memnum][ind] = 0;
			//sprintf(msg,"Removed ent from memory number %i: %s <%i>\n", memnum, pClassname, ind);
/*
			entvars_t *pRunOnPev;
			pRunOnPev =  VARS(pRunOn);

			// We may need to set its mem shit to normal!
			int theflag = 0;
			if (memnum == 0) theflag = AV_FLAG_SHOWMEM0;
			if (memnum == 1) theflag = AV_FLAG_SHOWMEM1;
			if (memnum == 2) theflag = AV_FLAG_SHOWMEM2;
			if (memnum == 3) theflag = AV_FLAG_SHOWMEM3;
			if (memnum == 4) theflag = AV_FLAG_SHOWMEM4;
			if (memnum == 5) theflag = AV_FLAG_SHOWMEM5;
		
			if (AdminFlags[MyNumber] & theflag) {

				pRunOnPev->rendercolor.x = EntData[ind].rendercolorx;
				pRunOnPev->rendercolor.y = EntData[ind].rendercolory;
				pRunOnPev->rendercolor.z = EntData[ind].rendercolorz;
				pRunOnPev->rendermode = EntData[ind].rendermode;
				pRunOnPev->renderamt = EntData[ind].renderamt;
				pRunOnPev->renderfx = EntData[ind].renderfx;

				EntData[ind].alreadydone = 0;
			}

			effected = 1;
			*/

			AvUpdateShowMem();
		}

	}

	else if (Cmd == AV_ENT_SETGOD) {
		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		// sets god mode front ent
		if (FStrEq(arg1, "on"))
		{
			pRunOnPev->takedamage = DAMAGE_NO;
			effected = 1;
		}
		else if	(FStrEq(arg1, "off"))
		{
			pRunOnPev->takedamage = DAMAGE_AIM;
			effected = 1;
		}
	}
	else if (Cmd == AV_ENT_MAKETRAIN) {


		// Create this ent as a func_train

		// ARG1 is the path it should follow.
		// ARG2 is the trains name.
		// ARG3 is the trains speed.

		char buf[80];

		int speed = atoi(arg3);
		if (speed == 0) speed = 100;
		
		KeyValueData kvd;
				
		edict_t *tEntity;
		tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("func_train"));
		
		entvars_t *pNew;
		pNew =  VARS(tEntity);

		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		pNew->origin = pRunOnPev->origin;
		pNew->modelindex = pRunOnPev->modelindex;
		pNew->model = pRunOnPev->model;

		pNew->speed = speed;
		
		pNew->dmg = 100;

		// Set the TARGET and TARGETNAME here.

		kvd.fHandled = FALSE;
		kvd.szClassName = NULL;
		kvd.szKeyName = "classname";
		kvd.szValue = "func_train";
		DispatchKeyValue( tEntity, &kvd );
		kvd.fHandled = FALSE;
		
		kvd.szClassName = "func_train";
		sprintf( buf , "%s", arg2);
		
		kvd.szKeyName = "targetname";
		kvd.szValue = buf;
		DispatchKeyValue( tEntity, &kvd );
		kvd.fHandled = FALSE;

		// path to follow
		sprintf( buf , "%s_1", arg1);
		
		kvd.szKeyName = "target";
		kvd.szValue = buf;
		DispatchKeyValue( tEntity, &kvd );
		kvd.fHandled = FALSE;

		// now spawn it

		DispatchSpawn( ENT( pNew ) );

		effected = 1;
		
		int ind2 = ENTINDEX(tEntity);
		
	
		

	}

	if (effected == 1) {
		
		// Mark that this has been modified.
		EntModified[ind] = 1;

	}




	//if (MsgsSent < 60) ClientPrint( pPev, HUD_PRINTNOTIFY, msg);
	return effected;
}


void AvPlayerCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd )
{

	// First get what we are talking to. Format for ALL Player Commands:
	// <command> <what player(s) we are effecting> <extra crap>
	
	int MyNumber = ENTINDEX(pEntity);
	int MsgsSent = 0;


	if (Cmd != AV_PLY_TELELAST) {

		if (FStrEq(arg2, "mem")) 
		{
			// This command to be performed to all players in memory
			// Usage: avb <command> mem <args>
			
			
			int i = AdminPlayerMems[MyNumber];
		
			if ( i > 0 && i <= gpGlobals->maxClients )
			{
				edict_t *pPlayerEdict = INDEXENT( i );

				if ( pPlayerEdict && !pPlayerEdict->free )
				{

					AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg3, arg4, arg5, arg6, arg7, arg8, Cmd, 3);
				}
			}

		}
		else if	(FStrEq(arg2, "front")) 
		{
			// This command to be performed to the player I am looking at.

			edict_t *pPlayerEdict = Av_GetFrontEnt ( pEntity );
			
			int i = ENTINDEX(pPlayerEdict);

			if ( i > 0 && i <= gpGlobals->maxClients )
			{
				pPlayerEdict = INDEXENT( i );

				if ( pPlayerEdict && !pPlayerEdict->free )
				{
					AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg3, arg4, arg5, arg6, arg7, arg8, Cmd, 3);
				}
			}
		}
		else if	(FStrEq(arg2, "me")) 
		{
			// This command to be performed on me

			AvRunPlayer (pEntity, pPev, MyNumber, pEntity, arg3, arg4, arg5, arg6, arg7, arg8, Cmd, 3);

		}
		else if	(FStrEq(arg2, "all")) 
		{
			// This command to be performed to all players 

			int i;
			edict_t *pPlayerEdict;
			
			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
				if ( i != MyNumber && AdminLoggedIn[i] == 0)
				{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{

						char *pClassname;
						pClassname = Av_GetClassname ( pPlayerEdict ); 
						if (FStrEq(pClassname, "player")) // make sure its actually a player.
						{
							AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg3, arg4, arg5, arg6, arg7, arg8, Cmd, 3);
						}
					}
				}
			}	
		}
		else if	(FStrEq(arg2, "realall")) 
		{
			// This command to be performed to all players 

			int i;
			edict_t *pPlayerEdict;
			
			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{

						char *pClassname;
						pClassname = Av_GetClassname ( pPlayerEdict ); 
						if (FStrEq(pClassname, "player")) // make sure its actually a player.
						{
							AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg3, arg4, arg5, arg6, arg7, arg8, Cmd, 3);
						}
					}
			}	
		}
		else if	(FStrEq(arg2, "team")) 
		{
			// This command to be performed to all players whos team matches the specified one

			int i;
			edict_t *pPlayerEdict;
			
			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
				if ( i != MyNumber && AdminLoggedIn[i] == 0)
				{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{

						char *pClassname;
						pClassname = Av_GetClassname ( pPlayerEdict ); 
						if (FStrEq(pClassname, "player")) // make sure its actually a player.
						{

							// Make sure team matches

							if (pPlayerEdict->v.team == atoi(arg3)) {
								AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg4, arg5, arg6, arg7, arg8, arg9, Cmd, 4);
							}
						}
					}
				}
			}	
		}
		else if	(FStrEq(arg2, "class")) 
		{
			// This command to be performed to all players whos class matches the specified one

			int i;
			edict_t *pPlayerEdict;
			
			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
				if ( i != MyNumber && AdminLoggedIn[i] == 0)
				{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{

						char *pClassname;
						pClassname = Av_GetClassname ( pPlayerEdict ); 
						if (FStrEq(pClassname, "player")) // make sure its actually a player.
						{

							// Make sure class matches

							if (pPlayerEdict->v.playerclass == atoi(arg3)) {
								AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg4, arg5, arg6, arg7, arg8, arg9, Cmd, 4);
							}
						}
					}
				}
			}	
		}
		else
		{
			// This command to be performed to the player with this userid
			
			int usid = atoi(arg2);

			int i;
			edict_t *pPlayerEdict;
			

			if (usid == 0) {
				// Ok, so its not a user id. Search for matching names.
				
				char *playerName;

				for ( i = 1; i <= gpGlobals->maxClients; i++ )
				{
					if ( i > 0 && i <= gpGlobals->maxClients )
					{
						pPlayerEdict = INDEXENT( i );
						if ( pPlayerEdict && !pPlayerEdict->free )
						{

							playerName = (char *)STRING(pPlayerEdict->v.netname);

							char *pClassname;
							pClassname = Av_GetClassname ( pPlayerEdict ); 
							if (FStrEq(pClassname, "player")) // make sure its actually a player.
							{
								// Check NAME here
								if ( stristr( playerName, arg2 )) {
									// NAME matches, perform commands here.
									AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg3, arg4, arg5, arg6, arg7, arg8, Cmd, 3);
								}
							}
						}
					}
				}

			}
			else if (!FStrEq(arg2, ""))
			{

				for ( i = 1; i <= gpGlobals->maxClients; i++ )
				{
					if ( i > 0 && i <= gpGlobals->maxClients )
					{
						pPlayerEdict = INDEXENT( i );
						if ( pPlayerEdict && !pPlayerEdict->free )
						{
							// Check USERID here
							if ( GETPLAYERUSERID( pPlayerEdict ) == usid ) {
								// UserID matches, perform commands here.

								char *pClassname;
								pClassname = Av_GetClassname ( pPlayerEdict ); 
								if (FStrEq(pClassname, "player")) // make sure its actually a player.
								{
									AvRunPlayer (pEntity, pPev, MyNumber, pPlayerEdict, arg3, arg4, arg5, arg6, arg7, arg8, Cmd, 3);
								}
							}
						}
					}
				}
			}
		}
	}
	else if (Cmd == AV_PLY_TELELAST) {

		// Move last player to the same loc as the admin.

		Vector newOrigin;

		edict_t *pTeleLast;
		
		if (AdminLastTele[MyNumber] > 0) {

			pTeleLast = INDEXENT(AdminLastTele[MyNumber]);
			
			if (pTeleLast) {

				entvars_t *pTelePev;
				pTelePev =  VARS(pTeleLast);
			
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_TELEPORT ); // neat -- MODE 1 (DEFAULT)
					WRITE_COORD( pTelePev->origin.x );
					WRITE_COORD( pTelePev->origin.y );
					WRITE_COORD( pTelePev->origin.z );
				MESSAGE_END();

				newOrigin = pPev->origin;
				pTelePev->angles = pPev->angles;

				SET_ORIGIN(ENT(pTelePev), newOrigin );
			}
		}
	}
}

void AvRunPlayer( edict_t *pEntity , entvars_t *pPev, int MyNumber, edict_t *pRunOn, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int Cmd, int FirstArg)
{
	//Actually DOES the crap on the ent. Only messes with the ONE ent.

	char msg[80];
	
	entvars_t *pRunOnPev;
	pRunOnPev =  VARS(pRunOn);

	int ind = ENTINDEX(pRunOn);
	
	char *playerName = (char *)STRING(pRunOn->v.netname);

	sprintf(msg,"Nothing done to player %s\n", playerName);

	if (Cmd == AV_PLY_TELEPORT) {

		// Move this player to the same loc as the admin.

		Vector newOrigin;
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_TELEPORT ); // neat -- MODE 1 (DEFAULT)
			WRITE_COORD( pRunOnPev->origin.x );
			WRITE_COORD( pRunOnPev->origin.y );
			WRITE_COORD( pRunOnPev->origin.z );
		MESSAGE_END();

		newOrigin = pPev->origin;
		pRunOnPev->angles = pPev->angles;

		SET_ORIGIN(ENT(pRunOnPev), newOrigin );

		sprintf(msg,"Teleported player %s to you.\n", playerName);

		AdminLastTele[MyNumber] = ind;

	}
	else if (Cmd == AV_PLY_METELEPORT) {

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_TELEPORT ); // neat -- MODE 1 (DEFAULT)
			WRITE_COORD( pPev->origin.x );
			WRITE_COORD( pPev->origin.y );
			WRITE_COORD( pPev->origin.z );
		MESSAGE_END();

		// Move this admin to the player
		entvars_t *pRunOnPev;
		pRunOnPev =  VARS(pRunOn);

		Vector newOrigin;
		
		newOrigin = pRunOnPev->origin;
		pPev->angles = pRunOnPev->angles;

		SET_ORIGIN(ENT(pPev), newOrigin );

		sprintf(msg,"Teleported you to player %s!\n", playerName);

	}
	else if (Cmd == AV_PLY_KILL) {
		
		CLIENT_COMMAND( pRunOn, "kill\n");
		sprintf(msg,"Killed player %s!\n", playerName);

	}
	else if (Cmd == AV_PLY_COMMAND) {

		char cmd[300];

		sprintf(cmd, "%s %s %s\n", arg1, arg2, arg3);
		
		CLIENT_COMMAND( pRunOn, cmd );

		sprintf(msg,"Ran command %s on %s!\n", cmd, playerName);

	}
	else if (Cmd == AV_PLY_CHANGEMODEL)
	{

		pRunOnPev->fuser1 = 1;

		//SET_MODEL( pRunOn, arg1 );
		pRunOnPev->vuser3.x = 1;
		g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pRunOn) , g_engfuncs.pfnGetInfoKeyBuffer( pRunOn ), "model", (char *)arg1 );
		sprintf( playerModels[ENTINDEX(pRunOn)].text , "%s", (char*)arg1 );
		
		pRunOnPev->body = atoi(arg2);
		pRunOnPev->skin = atoi(arg3);

		sprintf(msg,"Set %s's model to %s.\n", playerName, arg1);

	}
	else if (Cmd == AV_PLY_SPECIAL)
	{

		if (FStrEq(arg1, "evil"))
		{

			int n1 = 0;
			int n2 = 2;

			//int n3 = n2 / n1;

		}
		
		if (FStrEq(arg1, "blue"))
		{
			if (FStrEq(arg2, "on"))
			{
				PlayerInfo[ind].hasSpecialMode = 1;
 				sprintf(msg,"Enabled blue ents for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				PlayerInfo[ind].hasSpecialMode = 0;
 				sprintf(msg,"Disabled blue ents for %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "usg"))
		{
			if (FStrEq(arg2, "on"))
			{
				AdminDettingSoon[ind] = 1;
 				sprintf(msg,"Enabled unlimited buildings for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				AdminDettingSoon[ind] = 0;
 				sprintf(msg,"Disabled unlimited buildings for %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "brand"))
		{
			if (FStrEq(arg2, "on"))
			{
				branded = 1;
 				sprintf(msg,"Branding enabled!\n");	
			}
			if (FStrEq(arg2, "off"))
			{
				branded = 0;
 				sprintf(msg,"Branding disabled!");	
			}
		}
		if (FStrEq(arg1, "super"))
		{
			if (FStrEq(arg2, "on"))
			{
				PlayerInfo[ind].superBright  = 1;
 				sprintf(msg,"Enabled super bright mode for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				PlayerInfo[ind].superBright  = 0;
 				sprintf(msg,"Disabled super bright mode %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "noplayer"))
		{
			if (FStrEq(arg2, "on"))
			{
				PlayerInfo[ind].noPlayers  = 1;
 				sprintf(msg,"Enabled no players for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				PlayerInfo[ind].noPlayers  = 0;
 				sprintf(msg,"Disabled no players for %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "rc"))
		{
			if (FStrEq(arg2, "on"))
			{
				PlayerInfo[ind].racecam  = 1;
 				sprintf(msg,"Enabled Race Cam for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				PlayerInfo[ind].racecam  = 0;
 				sprintf(msg,"Disabled Race Cam for %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "racer"))
		{
			if (FStrEq(arg2, "on"))
			{
				PlayerInfo[ind].racer  = 1;
 				sprintf(msg,"Enabled Racer Mode for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				PlayerInfo[ind].racer  = 0;
 				sprintf(msg,"Disabled Racer Mode for %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "priv"))
		{
			if (FStrEq(arg2, "on"))
			{
				PlayerInfo[ind].privchat  = 1;
 				sprintf(msg,"Enabled private chat for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				PlayerInfo[ind].privchat  = 0;
 				sprintf(msg,"Disabled private chat for %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "all"))
		{
			if (FStrEq(arg2, "on"))
			{
				PlayerInfo[ind].noPlayers  = 1;
				PlayerInfo[ind].hasSpecialMode   = 1;
				PlayerInfo[ind].superBright   = 1;
 				sprintf(msg,"Enabled all special effects for %s\n", playerName);	
			}
			if (FStrEq(arg2, "off"))
			{
				PlayerInfo[ind].noPlayers  = 0;
				PlayerInfo[ind].hasSpecialMode  = 0;
				PlayerInfo[ind].superBright  = 0;
 				sprintf(msg,"Disabled all special effects for %s\n", playerName);	
			}
		}
		if (FStrEq(arg1, "light"))
		{



			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

				WRITE_BYTE( TE_DLIGHT );

				WRITE_COORD( pRunOnPev->origin.x );
				WRITE_COORD( pRunOnPev->origin.y );
				WRITE_COORD( pRunOnPev->origin.z );

				WRITE_BYTE( 40 ); //radius

				// colour
				WRITE_BYTE( atoi(arg2) ); 
				WRITE_BYTE( atoi(arg3) ); 
				WRITE_BYTE( atoi(arg4) ); 


				//WRITE_BYTE( 200 ); // brightness
				WRITE_BYTE( 255 );  // life
				WRITE_BYTE( 0 );  // decay
			MESSAGE_END();

		}


	}
	else if (Cmd == AV_PLY_SETPOINTS)
	{
	
		PlayerInfo[ind].numPoints = atoi(arg1);
 		sprintf(msg,"Set %s's points to %i\n", playerName, atoi(arg1));	
	}
	else if (Cmd == AV_PLY_SPRITEBOOM) 
	{
		// #define TE_EXPLODEMODEL		107		// spherical shower of models, picks from set
		// coord, coord, coord (origin)
		// coord (velocity)
		// short (model index)
		// short (count)
		// byte (life in 0.1's)


		int vel = atoi(arg3);
		if (vel == 0) vel = 1000;
		int wh = atoi(arg2);
		//if (wh > 1) wh = 0;

		int i = 1;
		edict_t *frontEnt;
		entvars_t *pRunOnPev2;
		char *pClassname;

		for (i; i < 1025; i++) {

			frontEnt = INDEXENT ( i );
			if (frontEnt) 
			{
				pRunOnPev2 = VARS(frontEnt);
				
				bool dothis = 0;

				
				if (FStrEq(arg1,  EntData[ENTINDEX(frontEnt)].digitgroup)) dothis = 1;
				
				if (dothis) 
				{
								
					pClassname = Av_GetClassname ( frontEnt ); 
					if (FStrEq("building_dancemachine", pClassname)) 
					{

						MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

							WRITE_BYTE( TE_EXPLODEMODEL );
							
							WRITE_COORD( pRunOnPev2->origin.x );
							WRITE_COORD( pRunOnPev2->origin.y );
							WRITE_COORD( pRunOnPev2->origin.z );

							WRITE_COORD(vel);

							if (wh > 0) WRITE_SHORT( wh );
							if (wh == 0) WRITE_SHORT( m_bubbleSprite );
							
							WRITE_SHORT( 4 );
							
							WRITE_BYTE( 30 );
							
						MESSAGE_END();
					}
				}
			}
		}
	}
	else if (Cmd == AV_PLY_GOD) {
		if (FStrEq(arg1, "on"))
		{
			pRunOnPev->takedamage = DAMAGE_NO;
			sprintf(msg,"Set %s's god mode to ON.\n", playerName);
		}
		else if	(FStrEq(arg1, "off"))
		{
			pRunOnPev->takedamage = DAMAGE_AIM;
			sprintf(msg,"Set %s's god mode to OFF.\n", playerName);
		}
	}
	else if (Cmd == AV_PLY_LEASH) {
		if (FStrEq(arg1, "off"))
		{
			sprintf(EntData[ENTINDEX(pRunOn)].digitgroup, "none");

			sprintf(msg,"Disabled %s's leash.\n", playerName);
		}
		else if (strlen(arg1) > 0)
		{
			sprintf(EntData[ENTINDEX(pRunOn)].digitgroup, arg1);
			sprintf(msg,"Leashed %s to %s\n", playerName, arg1);
			EntData[ENTINDEX(pRunOn)].leash_master = NULL;
		}
		else
		{
			// leash this person to me
			sprintf(EntData[ENTINDEX(pRunOn)].digitgroup, "none");
			
			EntData[ENTINDEX(pRunOn)].leash_master = pEntity;

			sprintf(msg,"Leashed %s to you!\n", playerName);
	
		}
	}
	else if (Cmd == AV_PLY_INVIS) {
		if (FStrEq(arg1, "on") || FStrEq(arg1, "full"))
		{
			pRunOnPev->rendermode = kRenderTransTexture;
			pRunOnPev->renderamt = 0;
			pRunOnPev->renderfx = 0;

			sprintf(msg,"Set %s's invis mode to FULLY INVISIBLE.\n", playerName);
		}
		else if	(FStrEq(arg1, "off"))
		{
			pRunOnPev->rendermode = kRenderNormal;
			pRunOnPev->renderamt = 0;
			pRunOnPev->renderfx = 0;

			sprintf(msg,"Set %s's invis mode to OFF.\n", playerName);
		}
		else if	(FStrEq(arg1, "holo"))
		{
			
			pRunOnPev->rendermode = 5;
			pRunOnPev->renderamt = 172;
			pRunOnPev->renderfx = 16;

			sprintf(msg,"Set %s's invis mode to HOLO.\n", playerName);
		}
		else if	(FStrEq(arg1, "semi"))
		{
			
			pRunOnPev->rendermode = 2;
			pRunOnPev->renderamt = 150;
			pRunOnPev->renderfx = 0;

			sprintf(msg,"Set %s's invis mode to SEMI-INVIS.\n", playerName);
		}
		else if	(FStrEq(arg1, "fade"))
		{
			pRunOnPev->rendermode = 2;
			pRunOnPev->renderamt = 100;
			pRunOnPev->renderfx = 4;

			sprintf(msg,"Set %s's invis mode to FADING.\n", playerName);
		}
	}
	else if (Cmd == AV_PLY_GLOWSHELL) {

		// makes a glow shell of color arg1, arg2, arg3

		pRunOnPev->renderfx = 19;

		int new1 = 0;
		int new2 = 0;
		int new3 = 0;

		new1 = atoi(arg1);
		new2 = atoi(arg2);
		new3 = atoi(arg3);

		if (new1 > 255) new1 = 255;
		if (new2 > 255) new2 = 255;
		if (new3 > 255) new3 = 255;

		if (new1 < 0) new1 = 0;
		if (new2 < 0) new2 = 0;
		if (new3 < 0) new3 = 0;
		
		if (FStrEq(arg1, "rand")) {
			new1 = (int)RANDOM_FLOAT(0,255);
			new2 = (int)RANDOM_FLOAT(0,255);
			new3 = (int)RANDOM_FLOAT(0,255);
		}
		if (FStrEq(arg1, "rain")) {
			new1 = (int)ColourArray[lastpos].x;
			new2 = (int)ColourArray[lastpos].y;
			new3 = (int)ColourArray[lastpos].z;
		}
			if (new1 > -1) {
			pRunOnPev->rendercolor.x = new1;
			pRunOnPev->rendercolor.y = new2;
			pRunOnPev->rendercolor.z = new3;
			pRunOnPev->renderamt = 100;
		}

		sprintf(msg,"Set %s's glow shell to RGB %i, %i, %i\n", playerName, new1, new2, new3);
	}
	else if (Cmd == AV_PLY_GRAV) {

		float newgrav = atof(arg1);

		pRunOnPev->gravity = newgrav;
		sprintf(msg,"Set %s's gravity fraction to %f.\n", playerName, newgrav);

	}
	else if (Cmd == AV_PLY_FLY) {
		if (FStrEq(arg1, "on"))
		{
			pRunOnPev->movetype = MOVETYPE_NOCLIP;
			sprintf(msg,"Set %s's fly mode to ON.\n", playerName);
		}
		else if	(FStrEq(arg1, "off"))
		{
			pRunOnPev->movetype = MOVETYPE_WALK;
			sprintf(msg,"Set %s's fly mode to OFF.\n", playerName);
		}
	}
	else if (Cmd == AV_PLY_TARGET) {
		if (FStrEq(arg1, "off"))
		{
			pRunOnPev->flags |= FL_NOTARGET;
			sprintf(msg,"Set %s's target mode to NOTARGET.\n", playerName);
		}
		else if	(FStrEq(arg1, "on"))
		{
			pRunOnPev->flags &= ~FL_NOTARGET;
			sprintf(msg,"Set %s's target mode to NORMAL.\n", playerName);
		}
	}
	else if (Cmd == AV_PLY_FRIC) {

		float newfric = atof(arg1);

		pRunOnPev->friction = (float)newfric;
		sprintf(msg,"Set %s's friction fraction to %f.\n", playerName, newfric);

	}
	else if (Cmd == AV_PLY_SOLID) {

		//SOLID_SLIDEBOX

		if (FStrEq(arg1, "on"))
		{
			pRunOnPev->solid = SOLID_SLIDEBOX;
			sprintf(msg,"Set %s's corp mode to ON.\n", playerName);
		}
		else if	(FStrEq(arg1, "off"))
		{
			pRunOnPev->solid = SOLID_NOT;
			sprintf(msg,"Set %s's corp mode to OFF.\n", playerName);
		}
	}


	else if (Cmd == AV_PLY_VELUP) {
		//gives em a burst of velocity
		pRunOnPev->velocity.z = pRunOnPev->velocity.z + atoi(arg1);
		sprintf(msg,"0");
	}
	else if (Cmd == AV_PLY_TEAM) {
		//gives em a burst of velocity
		pRunOnPev->team = atoi(arg1);
		sprintf(msg,"Set %s's team to %i.\n", playerName, atoi(arg1));
	}
	else if (Cmd == AV_PLY_CLASS) {
		//gives em a burst of velocity
		pRunOnPev->playerclass = atoi(arg1);
		sprintf(msg,"Set %s's class to %i.\n", playerName, atoi(arg1));
	}
	
	else if (Cmd == AV_PLY_MOVETYPE) {

		//SOLID_SLIDEBOX

		if (FStrEq(arg1, "walk"))
		{
			pRunOnPev->movetype = MOVETYPE_WALK;
			sprintf(msg,"Set %s's movetype to WALK.\n", playerName);
		}
		else if	(FStrEq(arg1, "step"))
		{
			pRunOnPev->movetype = MOVETYPE_STEP;
			sprintf(msg,"Set %s's movetype to STEP.\n", playerName);
		}
		else if	(FStrEq(arg1, "fly"))
		{
			pRunOnPev->movetype = MOVETYPE_FLY;
			pRunOnPev->flags |= FL_ONGROUND;

			sprintf(msg,"Set %s's movetype to FLY.\n", playerName);
		}
		else if	(FStrEq(arg1, "toss"))
		{
			pRunOnPev->movetype = MOVETYPE_TOSS;
			sprintf(msg,"Set %s's movetype to TOSS.\n", playerName);
		}
		else if	(FStrEq(arg1, "push"))
		{
			pRunOnPev->movetype = MOVETYPE_PUSH;
			sprintf(msg,"Set %s's movetype to PUSH.\n", playerName);
		}
		else if	(FStrEq(arg1, "bounce"))
		{
			pRunOnPev->movetype = MOVETYPE_BOUNCE;
			sprintf(msg,"Set %s's movetype to BOUNCE.\n", playerName);
		}
		else if	(FStrEq(arg1, "pushstep"))
		{
			pRunOnPev->movetype = MOVETYPE_PUSHSTEP;
			sprintf(msg,"Set %s's movetype to PUSHSTEP.\n", playerName);
			
			pRunOnPev->maxspeed = 400;
			pRunOnPev->skin = 50;
			SetBits( pRunOnPev->flags, FL_FLOAT );

		}
	}
	else if (Cmd == AV_PLY_BEAM) {
		//creates a short beam "flash"

		int mde = atoi(arg4);
		if (mde == 1)
		{
			// mode 1... draw beams from all dance machines with the right name
			// (as specified in arg4)

			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev2;
			char *pClassname;
			edict_t *firstOne;
			edict_t *lastOne = NULL;
			bool foundfirst = 0;


			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev2 =  VARS(frontEnt);
					
					bool dothis = 0;

					if (FStrEq(arg5,  EntData[ENTINDEX(frontEnt)].digitgroup)) dothis = 1;
					
					if (dothis) 
					{
									
						pClassname = Av_GetClassname ( frontEnt ); 
						if (FStrEq("building_dancemachine", pClassname)) {
			
							if (foundfirst == 0)
							{
								foundfirst = 1;
								firstOne = frontEnt;
							}
							

							if (lastOne != NULL)
							{

								MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

									WRITE_BYTE( TE_BEAMPOINTS );

									WRITE_COORD( frontEnt->v.origin.x );
									WRITE_COORD( frontEnt->v.origin.y );
									WRITE_COORD( frontEnt->v.origin.z);

									WRITE_COORD( lastOne->v.origin.x );
									WRITE_COORD( lastOne->v.origin.y );
									WRITE_COORD( lastOne->v.origin.z );

									WRITE_SHORT ( m_beamSprite );
											
									WRITE_BYTE( 1 ); 
									WRITE_BYTE( 10 );
									WRITE_BYTE( 20 );  // life
									WRITE_BYTE( 70 ); // width
									WRITE_BYTE( 10 );  // noise
									
									AvWriteColor(arg1, arg2, arg3);
									
									WRITE_BYTE( 255 );
									WRITE_BYTE( 10 );

								MESSAGE_END();
							}
							lastOne = frontEnt;
						}
					}
				}
			}

			if (lastOne != NULL && firstOne != NULL && firstOne != lastOne)
			{

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_BEAMPOINTS );
					WRITE_COORD( firstOne->v.origin.x );
					WRITE_COORD( firstOne->v.origin.y );
					WRITE_COORD( firstOne->v.origin.z);

					WRITE_COORD( lastOne->v.origin.x );
					WRITE_COORD( lastOne->v.origin.y );
					WRITE_COORD( lastOne->v.origin.z );

					WRITE_SHORT ( m_beamSprite );

					WRITE_BYTE( 1 ); 
					WRITE_BYTE( 10 );
					WRITE_BYTE( 20 );  // life
					WRITE_BYTE( 70 ); // width
					WRITE_BYTE( 10 );  // noise
					
					AvWriteColor(arg1, arg2, arg3);
					
					WRITE_BYTE( 255 );
					WRITE_BYTE( 10 );

				MESSAGE_END();
			}
		}
		else
		{
			TraceResult tr;

			UTIL_MakeVectors ( pRunOnPev->v_angle );
			Vector vecSrc   = GetGunPosition( pRunOn );
			Vector vecEnd   = vecSrc + gpGlobals->v_forward * 8192;

			UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( pRunOnPev ), &tr );

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

				WRITE_BYTE( TE_BEAMPOINTS );

				WRITE_COORD( pRunOnPev->origin.x );
				WRITE_COORD( pRunOnPev->origin.y );
				WRITE_COORD( pRunOnPev->origin.z +10);

				WRITE_COORD( tr.vecEndPos.x );
				WRITE_COORD( tr.vecEndPos.y );
				WRITE_COORD( tr.vecEndPos.z );

				WRITE_SHORT ( m_beamSprite );
						
				WRITE_BYTE( 1 ); 
				WRITE_BYTE( 10 );
				WRITE_BYTE( 2 );  // life
				WRITE_BYTE( 10 );
				WRITE_BYTE( 0 ); 
				
				AvWriteColor(arg1, arg2, arg3);
				
				WRITE_BYTE( 255 );
				WRITE_BYTE( 10 );

			MESSAGE_END();
		}
		sprintf(msg,"0");
		
	}
	else if (Cmd == AV_PLY_WORDS) {
		//make a beam word??

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMPOINTS );

			WRITE_COORD( 500 );
			WRITE_COORD( 700 );
			WRITE_COORD( 300);

			WRITE_COORD( 500 );
			WRITE_COORD( -700 );
			WRITE_COORD( 300 );

			WRITE_SHORT ( m_Line1 );
					
			WRITE_BYTE( 1 );  //(starting frame)
			WRITE_BYTE( 100 ); //(frame rate in 0.1's) 
			WRITE_BYTE( 200 );  // life
			WRITE_BYTE( 150 ); //(line width in 0.1's) 
			WRITE_BYTE( 0 );  // noise
			
			
			WRITE_BYTE( 255 ); //r
			WRITE_BYTE( 255 ); //g
			WRITE_BYTE( 255 ); //b
			
			WRITE_BYTE( 255 ); //brightness
			WRITE_BYTE( 4 ); // scroll speed

		MESSAGE_END();
		sprintf(msg,"0");
		
	}
	else if (Cmd == AV_PLY_SETVIEWANGLE) {
		//make a beam word??

		pRunOnPev->v_angle.x = atof(arg1);
		pRunOnPev->v_angle.y = atof(arg2);
		pRunOnPev->v_angle.z = atof(arg3);
		pRunOnPev->angles = pRunOnPev->v_angle;

		pRunOnPev->fixangle = TRUE;

		
	}
	else if (Cmd == AV_PLY_COLORMAP) {
		//make a beam word??

		ALERT(at_console, "Previous colormap: %i\n", pRunOnPev->colormap);
		pRunOnPev->colormap = (atoi(arg1) * 255) + atoi(arg2);
		ALERT(at_console, "Current colormap: %i\n", pRunOnPev->colormap);

		
	}
	else if (Cmd == AV_PLY_LONGBEAM) {
		//creates a 10 second beam

		TraceResult tr;

		UTIL_MakeVectors ( pRunOnPev->v_angle );
		Vector vecSrc   = GetGunPosition( pRunOn );
		Vector vecEnd   = vecSrc + gpGlobals->v_forward * 8192;

		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( pRunOnPev ), &tr );

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMPOINTS );

			WRITE_COORD( pRunOnPev->origin.x );
			WRITE_COORD( pRunOnPev->origin.y );
			WRITE_COORD( pRunOnPev->origin.z +10);

			WRITE_COORD( tr.vecEndPos.x );
			WRITE_COORD( tr.vecEndPos.y );
			WRITE_COORD( tr.vecEndPos.z );

			WRITE_SHORT ( m_beamSprite );
					
			WRITE_BYTE( 1 ); 
			WRITE_BYTE( 10 );
			WRITE_BYTE( 100 );  // life
			WRITE_BYTE( 10 );
			WRITE_BYTE( 0 ); 
			
			AvWriteColor(arg1, arg2, arg3);
			
			WRITE_BYTE( 255 );
			WRITE_BYTE( 10 );

		MESSAGE_END();
		sprintf(msg,"0");
	}

	else if (Cmd == AV_PLY_WEAPONS) {
		
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pRunOn );
			WRITE_BYTE( 27 );
		MESSAGE_END();
		pRunOnPev->weapons |= (1<<27);


		sprintf(msg,"Gave %s the TRACTOR GUN!\n", playerName);
	}
	else if (Cmd == AV_PLY_FRAGS) {
		// Sets frags
		pRunOnPev->frags = atoi(arg1);
		sprintf(msg,"Set %s's frags to %i.\n", playerName, atoi(arg1));
	}
	else if (Cmd == AV_PLY_HEALTH) {
		// Sets frags
		pRunOnPev->health = atoi(arg1);
		sprintf(msg,"Set %s's health to %i.\n", playerName, atoi(arg1));
	}
	else if (Cmd == AV_PLY_MAXHEALTH) {
		// Sets frags
		pRunOnPev->max_health = atoi(arg1);
		sprintf(msg,"Set %s's max health to %i.\n", playerName, atoi(arg1));
	}
	else if (Cmd == AV_PLY_GIVEAIR) {
		// Sets frags
		pRunOnPev->air_finished = gpGlobals->time + atoi(arg1);
		sprintf(msg,"Gave %s %i seconds of air.\n", playerName, atoi(arg1));
	}
	else if (Cmd == AV_PLY_SPEED) {
		// Sets frags
		pRunOnPev->maxspeed = atoi(arg1);
		sprintf(msg,"Set %s's max speed to %i.\n", playerName, atoi(arg1));
	}
	else if (Cmd == AV_PLY_FOV) {
		// Sets frags
		pRunOnPev->fov = atoi(arg1);
		sprintf(msg,"Set %s's fov to %i.\n", playerName, atoi(arg1));
	}
	else if (Cmd == AV_PLY_MORTAR) {

		int pitch = RANDOM_LONG(95,124);

		EMIT_SOUND_DYN2(pRunOn, CHAN_AUTO, "weapons/mortar.wav", 1.0, ATTN_NONE, 0, pitch);	

		edict_t	*pent;

		pent = CREATE_NAMED_ENTITY( MAKE_STRING("monster_mortar"));

		TraceResult tr;
		UTIL_TraceLine( pRunOnPev->origin + Vector( 0, 0, 1024 ), pRunOnPev->origin - Vector( 0, 0, 1024 ), dont_ignore_monsters, pRunOn, &tr );

		pent->v.origin = tr.vecEndPos;
		pent->v.angles = Vector( 0, 0, 0 );
		DispatchSpawn( pent );

		pent->v.nextthink = gpGlobals->time + 2.5;


		sprintf(msg,"Mortared %s!\n", playerName);
	}
	else if (Cmd == AV_PLY_GIVE) {
		
		// works just like GIVE in the console with cheats on

		edict_t	*pent;

		int istr = MAKE_STRING(arg1);
		pent = CREATE_NAMED_ENTITY(istr);
		pent->v.origin = pRunOnPev->origin;
		pent->v.spawnflags |= SF_NORESPAWN;

		DispatchSpawn( pent );
		DispatchTouch( pent, pRunOn );
	}
	else if (Cmd == AV_PLY_FLASHLIGHT) {
		
		// turns on a light from them

		if (FStrEq(arg1, "bright")) 
		{

			if (FStrEq(arg2, "on")) 
			{
				pRunOnPev->effects |= EF_BRIGHTLIGHT;
				sprintf(msg,"Turned on a bright light at %s!\n", playerName);
			}
			else
			{
				pRunOnPev->effects &= ~EF_BRIGHTLIGHT;
				sprintf(msg,"Turned off a bright light at %s!\n", playerName);
			}
		}
		else if (FStrEq(arg1, "glow")) 
		{

			if (FStrEq(arg2, "on")) 
			{
				pRunOnPev->effects |= EF_DIMLIGHT;
				sprintf(msg,"Turned on a glow at %s!\n", playerName);
			}
			else
			{
				pRunOnPev->effects &= ~EF_DIMLIGHT;
				sprintf(msg,"Turned off a glow at %s!\n", playerName);
			}
		}
		else if (FStrEq(arg1, "particle")) 
		{

			if (FStrEq(arg2, "on")) 
			{
				pRunOnPev->effects |= EF_BRIGHTFIELD;
				sprintf(msg,"Turned on a particle field at %s!\n", playerName);
			}
			else
			{
				pRunOnPev->effects &= ~EF_BRIGHTFIELD;
				sprintf(msg,"Turned off a particle field at %s!\n", playerName);
			}
		}
		else if (FStrEq(arg1, "light")) 
		{

			if (FStrEq(arg2, "on")) 
			{
				pRunOnPev->effects |= EF_LIGHT;
				sprintf(msg,"Turned on a light at %s!\n", playerName);
			}
			else
			{
				pRunOnPev->effects &= ~EF_LIGHT;
				sprintf(msg,"Turned off a light at %s!\n", playerName);
			}
		}
	}


	else if (Cmd == AV_PLY_RESETVIEW) {
		// Sets frags
		SET_VIEW( pRunOn, pRunOn);
		sprintf(msg,"Reset %s's view!\n", playerName);
	}
	else if (Cmd == AV_PLY_DISCO) {
		// Sets frags

		if (FStrEq(arg1, "on")) 
		{
			beamadmin = ENTINDEX(pRunOn);
			lastpos = 0;
			lastbeamtime = 0;
			discomode = atoi(arg2);
			sprintf(msg,"Started Disco at %s!\n", playerName);
		}
		else
		{
			beamadmin = 0;
			sprintf(msg,"Stopped Disco at %s!\n", playerName);
		}

	}
	else if (Cmd == AV_PLY_NOTGROUND) {
		// Sets frags

		if (FStrEq(arg1, "on")) 
		{
			NoOnGround[ENTINDEX(pRunOn)] = 1;
			sprintf(msg,"%s is no longer on ground!\n", playerName);
		}
		else
		{
			NoOnGround[ENTINDEX(pRunOn)] = 0;
			sprintf(msg,"%s is now back on the ground!\n", playerName);
		}

	}

	else if (Cmd == AV_PLY_POPUP) {
		// Sets frags

		char buf[250];
		sprintf(buf, "%s", arg1);
		SendPopupToOne( pRunOn, buf );

		sprintf(msg,"Popped message on %s's Screen!\n", playerName);
	}

	else if (Cmd == AV_PLY_SETVIEW) {
		
		// find the player

		if (atoi(arg1) > 0)
		{
			SET_VIEW( pRunOn, INDEXENT(atoi(arg1)) );
			sprintf(msg,"Set %s's view to ent %s!\n", playerName, arg1);
		}
		if (FStrEq(arg1, "dance"))
		{

			int i = 1;
			edict_t *frontEnt;
			
			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					
					if (FStrEq(arg2, EntData[i].digitgroup)) {


						SET_VIEW( pRunOn, frontEnt );
						

					}
				}
			}
			sprintf(msg,"Set %s's view to ent %s!\n", playerName, arg1);						
		}
		
	}
	else if (Cmd == AV_PLY_ADDMEM) {
		
		AdminPlayerMems[MyNumber] = ind;
		sprintf(msg,"Set memory to %s.\n", playerName);
	}
	else if (Cmd == AV_PLY_FUNNEL) {
		
		// Creates a funnel. 
		int which = atoi(arg1);

		if (which < 0) which = 0;
		if (which > 9) which = 9;
		
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_LARGEFUNNEL );
			WRITE_COORD( pRunOnPev->origin.x );
			WRITE_COORD( pRunOnPev->origin.y );
			WRITE_COORD( pRunOnPev->origin.z );
			
			if (which == 0) WRITE_SHORT( m_funnelSprite );
			if (which == 1) WRITE_SHORT( m_presentModel );
			if (which == 2) WRITE_SHORT( m_otherModel1 );
			if (which == 3) WRITE_SHORT( m_otherModel2 );
			if (which == 4) WRITE_SHORT( m_otherModel3 );
			if (which == 5) WRITE_SHORT( m_otherModel4 );
			if (which == 6) WRITE_SHORT( m_otherModel5 );
			if (which == 7) WRITE_SHORT( m_otherModel6 );
			if (which == 8) WRITE_SHORT( m_otherModel7 );
			if (which == 9) WRITE_SHORT( m_otherModel8 );		

			if (FStrEq(arg2, "reverse")) // funnel flows in reverse?
			{
				WRITE_SHORT( 1 );
			}
			else
			{
				WRITE_SHORT( 0 );
			}


		MESSAGE_END();

		sprintf(msg,"Spawned a funnel at %s\n", playerName);
	}
	else if (Cmd == AV_PLY_SPAWNPRES) {
		// Spawns a bouncing present on the player end :-)
		// presentlg.mdl

		int which = atoi(arg1);

		if (which < 1) which = 1;
		if (which > 9) which = 9;

		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40,90) + gpGlobals->v_up * RANDOM_FLOAT(75,200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pRunOnPev->origin );
			WRITE_BYTE( TE_MODEL);
			WRITE_COORD( pRunOnPev->origin.x);
			WRITE_COORD( pRunOnPev->origin.y);
			WRITE_COORD( pRunOnPev->origin.z);
			WRITE_COORD( vecShellVelocity.x);
			WRITE_COORD( vecShellVelocity.y);
			WRITE_COORD( vecShellVelocity.z);
			WRITE_ANGLE( pRunOnPev->angles.y );
			
			if (which == 1) WRITE_SHORT( m_presentModel );
			if (which == 2) WRITE_SHORT( m_otherModel1 );
			if (which == 3) WRITE_SHORT( m_otherModel2 );
			if (which == 4) WRITE_SHORT( m_otherModel3 );
			if (which == 5) WRITE_SHORT( m_otherModel4 );
			if (which == 6) WRITE_SHORT( m_otherModel5 );
			if (which == 7) WRITE_SHORT( m_otherModel6 );
			if (which == 8) WRITE_SHORT( m_otherModel7 );
			if (which == 9) WRITE_SHORT( m_otherModel8 );			
			
			WRITE_BYTE ( TE_BOUNCE_SHELL );
			WRITE_BYTE ( 100 );// 2.5 seconds
		MESSAGE_END();

		sprintf(msg,"Spawned a present at %s\n", playerName);
	}
	else if (Cmd == AV_PLY_SPRITETRAIL) {

		// creates a bunch of sprites along a line.

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			int dist = atoi(arg2);
			int count = atoi(arg1);
			int which = atoi(arg3);
	
			if (which < 0) which = 0;
			if (which > 9) which = 9;			

			if (dist == 0) dist = 1000;
			if (count == 0) count = 15;

			if (count > 90) count = 90;


			UTIL_MakeVectors ( pRunOnPev->v_angle );
			Vector vecSrc  = GetGunPosition( pRunOn );
			Vector vecEnd  = vecSrc + gpGlobals->v_forward * dist;

			WRITE_BYTE( TE_SPRITETRAIL );
			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );

			WRITE_COORD( vecEnd.x );
			WRITE_COORD( vecEnd.y );
			WRITE_COORD( vecEnd.z );
			
			if (which == 0) WRITE_SHORT( m_funnelSprite );
			if (which == 1) WRITE_SHORT( m_presentModel );
			if (which == 2) WRITE_SHORT( m_otherModel1 );
			if (which == 3) WRITE_SHORT( m_otherModel2 );
			if (which == 4) WRITE_SHORT( m_otherModel3 );
			if (which == 5) WRITE_SHORT( m_otherModel4 );
			if (which == 6) WRITE_SHORT( m_otherModel5 );
			if (which == 7) WRITE_SHORT( m_otherModel6 );
			if (which == 8) WRITE_SHORT( m_otherModel7 );
			if (which == 9) WRITE_SHORT( m_otherModel8 );		
			
			WRITE_BYTE( count );
			WRITE_BYTE( 150 );
			WRITE_BYTE( 10 );
			WRITE_BYTE( 2 );
			WRITE_BYTE( 1 );

		MESSAGE_END();

		sprintf(msg,"Spawned a beam trail at %s\n", playerName);


	}
	else if (Cmd == AV_PLY_BEAMTORUS) 
	{

		// beam torus test
//TE_BEAMTORUS		19		// screen aligned beam ring, expands to max radius over lifetime
// coord coord coord (center position) 
// coord coord coord (axis and radius) 
// short (sprite index) 
// byte (starting frame) 
// byte (frame rate in 0.1's) 
// byte (life in 0.1's) 
// byte (line width in 0.1's) 
// byte (noise amplitude in 0.01's) 
// byte,byte,byte (color)
// byte (brightness)
// byte (scroll speed in 0.1's)

		int beamnoise = AvValue(arg5, 0);
		int xmove = AvValue(Cmd_Argv(FirstArg + 6), 0);
		int ymove = AvValue(Cmd_Argv(FirstArg + 7), 0);
		int zmove = AvValue(Cmd_Argv(FirstArg + 8), 200);

		int beamwidth = AvValue(Cmd_Argv(FirstArg + 9), 50);
		int beamlife = AvValue(Cmd_Argv(FirstArg + 10), 50);
		int beambright = AvValue(Cmd_Argv(FirstArg + 11), 255);
		int beammode = AvValue(Cmd_Argv(FirstArg + 12), 0);

		if (!FStrEq(arg6, "0") && strlen(arg6) > 0)
		{
			int mode = 0;
			// get the mode
			if (atoi(arg6) == 0) // using name
				mode = 1;
			
			if (FStrEq(arg6, "all")) mode = 2; // all

			int i = 1;
			edict_t *frontEnt;
			entvars_t *pRunOnPev2;
			char *pClassname;

			for (i; i < 1025; i++) {

				frontEnt = INDEXENT ( i );
				if (frontEnt) {
					pRunOnPev2 =  VARS(frontEnt);
					
					bool dothis = 0;

					if (mode == 0 && (pRunOnPev2->origin - pPev->origin).Length() <= atoi(arg6)) dothis = 1;
					if (mode == 1 && FStrEq(arg6,  EntData[ENTINDEX(frontEnt)].digitgroup)) dothis = 1;
					if (mode == 2) dothis = 1;

					if (dothis) 
					{
									
						pClassname = Av_GetClassname ( frontEnt ); 
						if (FStrEq("building_dancemachine", pClassname)) {
			

							MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

								if (FStrEq(arg4, "cyl")) WRITE_BYTE( TE_BEAMCYLINDER );
								else if (FStrEq(arg4, "disk")) WRITE_BYTE( TE_BEAMDISK );
								else WRITE_BYTE( TE_BEAMTORUS );

								WRITE_COORD( pRunOnPev2->origin.x );
								WRITE_COORD( pRunOnPev2->origin.y );
								WRITE_COORD( pRunOnPev2->origin.z );

								WRITE_COORD( pRunOnPev2->origin.x + xmove);
								WRITE_COORD( pRunOnPev2->origin.y + ymove);
								WRITE_COORD( pRunOnPev2->origin.z + zmove);
								
								if (beammode == 0) WRITE_SHORT( m_beamSprite );
								if (beammode > 0) WRITE_SHORT( m_spriteTexture2 );
								
								WRITE_BYTE( 0 );
								WRITE_BYTE( 0 );
								WRITE_BYTE( beamlife ); // life
								WRITE_BYTE( beamwidth ); // width
								WRITE_BYTE( beamnoise ); // noise
								
								AvWriteColor(arg1, arg2, arg3);		
								
								WRITE_BYTE( beambright ); // brightness
								WRITE_BYTE( beammode ); // scroll speed

							MESSAGE_END();

						}
					}
				}
			}
		}

		else
		{


			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

				if (FStrEq(arg4, "cyl")) WRITE_BYTE( TE_BEAMCYLINDER );
				else if (FStrEq(arg4, "disk")) WRITE_BYTE( TE_BEAMDISK );
				else WRITE_BYTE( TE_BEAMTORUS );

				WRITE_COORD( pRunOnPev->origin.x );
				WRITE_COORD( pRunOnPev->origin.y );
				WRITE_COORD( pRunOnPev->origin.z );

				WRITE_COORD( pRunOnPev->origin.x + xmove);
				WRITE_COORD( pRunOnPev->origin.y + ymove );
				WRITE_COORD( pRunOnPev->origin.z + zmove);
				
				if (beammode == 0) WRITE_SHORT( m_beamSprite );
				if (beammode > 0) WRITE_SHORT( m_spriteTexture2 );
				
				WRITE_BYTE( 0 );
				WRITE_BYTE( 0 );
				WRITE_BYTE( beamlife ); // life
				WRITE_BYTE( beamwidth ); // width
				WRITE_BYTE( beamnoise ); // noise
				
				AvWriteColor(arg1, arg2, arg3);		
				
				WRITE_BYTE( beambright ); // brightness
				WRITE_BYTE( beammode ); // scroll speed


			MESSAGE_END();

		}
			sprintf(msg,"Spawned a beam torus at %s\n", playerName);


	}

	else if (Cmd == AV_PLY_BEAMFOLLOW) {

		// beam follows player

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			Vector vecSrc  = pRunOnPev->origin;
			Vector vecCen  = gpGlobals->v_up * 50;

			WRITE_BYTE( TE_BEAMFOLLOW );

			WRITE_SHORT( ind );
			WRITE_SHORT( m_beamSprite );

			WRITE_BYTE( AvValue(arg5, 50) ); // life
			WRITE_BYTE( AvValue(arg4, 10) );
			
			AvWriteColor(arg1, arg2, arg3);			

			WRITE_BYTE( 255 ); // brightness

		MESSAGE_END();

		sprintf(msg,"Spawned a beam follow behind %s\n", playerName);
	}

	else if (Cmd == AV_PLY_BUBBLETRAIL) {

		// makes some happy bubbles

		int speed = atoi(arg2);
		int count = atoi(arg1);

		if (count == 0) count = 30;
		if (speed == 0) speed = 10;

		
		UTIL_MakeVectors ( pRunOnPev->v_angle );
		Vector vecSrc  = GetGunPosition( pRunOn );
		Vector vecEnd  = vecSrc + gpGlobals->v_forward * 5;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	
			WRITE_BYTE( TE_BUBBLETRAIL );

			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );
			
			WRITE_COORD( vecEnd.x );
			WRITE_COORD( vecEnd.y );
			WRITE_COORD( vecEnd.z );

			WRITE_COORD( 500 );

			WRITE_SHORT( m_bubbleSprite );

			WRITE_BYTE( count );

			WRITE_COORD( speed );


		MESSAGE_END();

		sprintf(msg,"Spawned BUBBLES at %s\n", playerName);
	}
	else if (Cmd == AV_PLY_PROJECTILE) {

		// makes some happy bubbles

		int speed = atoi(arg2);
		int which = atoi(arg1);

		if (which < 1) which = 1;
		
		if (speed == 0) speed = 2000;
		if (speed < 5) speed = 5;


		UTIL_MakeVectors ( pRunOnPev->v_angle );
		Vector vecSrc  = GetGunPosition( pRunOn ); 
		Vector vecEnd  = gpGlobals->v_forward * speed ;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	
			WRITE_BYTE( TE_PROJECTILE );

			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z - 15);
			
			WRITE_COORD( vecEnd.x );
			WRITE_COORD( vecEnd.y );
			WRITE_COORD( vecEnd.z );

			if (which == 1) WRITE_SHORT( m_presentModel );
			if (which > 1) WRITE_SHORT( which );
			
			WRITE_BYTE( 100 );

			WRITE_BYTE( ind );


		MESSAGE_END();

		sprintf(msg,"\n");
	}
	else if (Cmd == AV_PLY_ATTACH) {

		// attach test

		int which = atoi(arg1);

		if (which < 1) which = 1;
		if (which > 9) which = 9;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	
			WRITE_BYTE( TE_PLAYERATTACHMENT );

			WRITE_BYTE( ind );

			WRITE_COORD( 0 );

			if (which == 1) WRITE_SHORT( m_presentModel );
			if (which == 2) WRITE_SHORT( m_otherModel1 );
			if (which == 3) WRITE_SHORT( m_otherModel2 );
			if (which == 4) WRITE_SHORT( m_otherModel3 );
			if (which == 5) WRITE_SHORT( m_otherModel4 );
			if (which == 6) WRITE_SHORT( m_otherModel5 );
			if (which == 7) WRITE_SHORT( m_otherModel6 );
			if (which == 8) WRITE_SHORT( m_otherModel7 );
			if (which == 9) WRITE_SHORT( m_otherModel8 );
			
			WRITE_SHORT( 90000000 );


		MESSAGE_END();

		sprintf(msg,"Attached a model to %s.\n", playerName);
	}
	else if (Cmd == AV_PLY_REMATTACH) {

		// attach test

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	
			WRITE_BYTE( TE_KILLPLAYERATTACHMENTS );
			WRITE_BYTE( ind );

		MESSAGE_END();

		sprintf(msg,"Removed all attachments from %s.\n", playerName);
	}
	else if (Cmd == AV_PLY_FIREFIELD) {

		// beam torus test

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_FIREFIELD );
			WRITE_COORD( pRunOnPev->origin.x );
			WRITE_COORD( pRunOnPev->origin.y );
			WRITE_COORD( pRunOnPev->origin.z );
			
			WRITE_SHORT( 100 );
			WRITE_SHORT( m_fireSprite );
			
			WRITE_BYTE( 20 );
			WRITE_BYTE(TEFIRE_FLAG_PLANAR);
			WRITE_BYTE( 50 ); // life

		MESSAGE_END();

		sprintf(msg,"Spawned a firefield at %s\n", playerName);


	}
	else if (Cmd == AV_PLY_LOCATE) {

		// put a beam from admin to the player

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMENTS );

			WRITE_SHORT ( ENTINDEX(pEntity) );
			WRITE_SHORT ( ind );

			WRITE_SHORT ( m_beamSprite );
					
			WRITE_BYTE( 1 ); 
			WRITE_BYTE( 10 );
			WRITE_BYTE( 255 );  // life
			WRITE_BYTE( 10 );
			WRITE_BYTE( 0 ); 
			
			AvWriteColor(arg1, arg2, arg3);
			
			WRITE_BYTE( 255 );
			WRITE_BYTE( 10 );

		MESSAGE_END();

		sprintf(msg,"Located %s\n", playerName);
	}

	else if (Cmd == AV_PLY_TELETEST) {

		// beam torus test


		

		int which = atoi(arg1);
		int which2 = atoi(arg2);
		//if (which == 0) which = 1;

		if (which == 0)
		{

			if (which2 == 0)
			{
				//g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pRunOn), g_engfuncs.pfnGetInfoKeyBuffer( pRunOn ), "model", "avatar" );
			}
			else
			{
				edict_t *pent = CREATE_ENTITY();
				entvars_t *pev = VARS( pent );
				pev->classname = MAKE_STRING("player_model");

				pev->origin = pRunOnPev->origin;
				
				pev->owner = pRunOn;

				pev->takedamage = DAMAGE_NO;

				SET_MODEL(pent, "models/barney.mdl"); // so i can see it use a big model

				UTIL_SetOrigin( pev, pev->origin );
				// play BOOM sound!

				EMIT_SOUND_DYN2( pent, CHAN_VOICE, "weapons/flmgrexp.wav", 1.0, ATTN_NORM , 0, 100);
					
				pev->movetype = MOVETYPE_FOLLOW;
				pev->solid = SOLID_NOT;
				pev->aiment = pRunOn;
				//pev->modelindex = PRECACHE_MODEL("models/barney.mdl"); 
				pev->model = MAKE_STRING("models/barney.mdl");
				pev->nextthink = gpGlobals->time + 1;
			}
		}
		else
		{

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

				if (which == 1) {
					WRITE_BYTE( TE_TELEPORT ); // neat -- MODE 1 (DEFAULT)
				}

				if (which == 2) {
					WRITE_BYTE( TE_LAVASPLASH ); // ugly as shit
				}

				if (which == 3) {
					WRITE_BYTE( TE_EXPLOSION2 ); // not bad -- MODE 2
				}
				
				if (which == 4) {
					WRITE_BYTE( TE_IMPLOSION ); // Kinda neat
				}

				if (which == 5) {
					WRITE_BYTE( TE_EXPLODEMODEL ); // LOL
				}

				if (which == 6) {
					WRITE_BYTE( TE_TAREXPLOSION ); // MODE 
				}

				
				WRITE_COORD( pRunOnPev->origin.x );
				WRITE_COORD( pRunOnPev->origin.y );
				WRITE_COORD( pRunOnPev->origin.z );

				if (which == 3) {
					WRITE_BYTE( 1 );
					WRITE_BYTE( 10 );
				}

				if (which == 4) {
					WRITE_BYTE( 50 );
					WRITE_BYTE( 50 );
					WRITE_BYTE( 50 );
				}
				if (which == 5) {
					WRITE_COORD( 50 );
					WRITE_SHORT( m_presentModel );
					WRITE_SHORT( 30 );
					WRITE_BYTE( 50 );
				}

			MESSAGE_END();

			sprintf(msg,"Teletest at %s\n", playerName);
		}
	}
	else if (Cmd == AV_PLY_BEAMRING) {

		// put a beam ring from admin to the player

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMRING );

			WRITE_SHORT ( ENTINDEX(pEntity) );
			WRITE_SHORT ( ind );

			WRITE_SHORT ( m_beamSprite );
					
			WRITE_BYTE( 1 ); 
			WRITE_BYTE( 10 );
			WRITE_BYTE( 255 );  // life
			WRITE_BYTE( 10 );
			WRITE_BYTE( 0 ); 
			
		
			AvWriteColor(arg1, arg2, arg3);
			
			WRITE_BYTE( 255 );
			WRITE_BYTE( 10 );

		MESSAGE_END();

		sprintf(msg,"Located %s\n", playerName);
	}
	else if (Cmd == AV_PLY_IUSER) {

		sprintf(msg,"IUser1: %i  IUser2: %i  IUser3: %i  IUser4: %i  \n", pRunOnPev->iuser1, pRunOnPev->iuser2, pRunOnPev->iuser3, pRunOnPev->iuser4);



	}
	else if (Cmd == AV_PLY_DRAWCLOCK) {

		AvDrawClock( pEntity );
		sprintf(msg,"Drew a clock!\n");

	}

	if (!FStrEq(msg, "0")) ClientPrint( pPev, HUD_PRINTCONSOLE, msg);
}

void AvWriteColor( const char *arg1, const char *arg2, const char *arg3)
{

	int bmr = atoi(arg1);
	int bmg = atoi(arg2);
	int bmb = atoi(arg3);
	if (bmr < 0) bmr = 0;
	if (bmg < 0) bmg = 0;
	if (bmb < 0) bmb = 0;
	if (bmr > 255) bmr = 255;
	if (bmg > 255) bmg = 255;
	if (bmb > 255) bmb = 255;

	if (bmr == 0 && bmg == 0) {
		if (bmb == 0) {
			bmr = 255;
		}
	}

	if (FStrEq(arg1, "rand")) {
		// he wants a random color

		bmr = (int)RANDOM_FLOAT(0,255);
		bmg = (int)RANDOM_FLOAT(0,255);
		bmb = (int)RANDOM_FLOAT(0,255);

	}


	if (FStrEq(arg1, "rain")) {
		// he wants a random color

		bmr = (int)ColourArray[lastpos].x;
		bmg = (int)ColourArray[lastpos].y;
		bmb = (int)ColourArray[lastpos].z;

	}
	
	if (FStrEq(arg1, "inc")) 
	{
		// incremented colour
		incnum++;
		if (incnum > 12) incnum = 1;
		
		if (incnum == 1) { bmr = 255; bmg = 0; bmb = 0; }
		if (incnum == 2) { bmr = 255; bmg = 0; bmb = 0; }
		if (incnum == 3) { bmr = 255; bmg = 0; bmb = 0; }
		if (incnum == 4) { bmr = 255; bmg = 0; bmb = 0; }
		if (incnum == 5) { bmr = 0; bmg = 255; bmb = 0; }
		if (incnum == 6) { bmr = 0; bmg = 255; bmb = 0; }
		if (incnum == 7) { bmr = 0; bmg = 255; bmb = 0; }
		if (incnum == 8) { bmr = 0; bmg = 255; bmb = 0; }
		if (incnum == 9) { bmr = 0; bmg = 255; bmb = 255; }
		if (incnum == 10) { bmr = 0; bmg = 255; bmb = 255; }
		if (incnum == 11) { bmr = 0; bmg = 255; bmb = 255; }
		if (incnum == 12) { bmr = 0; bmg = 255; bmb = 255; }
			
	}

	WRITE_BYTE( bmr ); //r
	WRITE_BYTE( bmg ); //g
	WRITE_BYTE( bmb ); //b

}

int AvValue( const char *arg1, int deflt)
{
	
	int ret = atoi(arg1);
	if (strlen(arg1) > 0) return ret;
	return deflt;
}

void AvSelfCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd )
{

	// Just run the command! Nothing else to it!
	// <command> <args>
	
	int MyNumber = ENTINDEX(pEntity);
	int MsgsSent = 0;

	AvRunMe (pEntity, pPev, MyNumber, arg2, arg3, arg4, arg5, arg6, arg7, Cmd);
}


void AvRunMe( edict_t *pEntity , entvars_t *pPev, int MyNumber, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int Cmd)
{
	//Runs this shiznit on ME!
	
	// Just pass it thru the O commands, but with my number.

	int newCmd = 0;
	
	if (Cmd == AV_ME_GOD)		newCmd = AV_PLY_GOD;
	if (Cmd == AV_ME_GRAV)		newCmd = AV_PLY_GRAV;
	if (Cmd == AV_ME_FRIC)		newCmd = AV_PLY_FRIC;
	if (Cmd == AV_ME_INVIS)		newCmd = AV_PLY_INVIS;
	if (Cmd == AV_ME_FLY)		newCmd = AV_PLY_FLY;
	if (Cmd == AV_ME_TARGET)	newCmd = AV_PLY_TARGET;
	if (Cmd == AV_ME_SOLID)		newCmd = AV_PLY_SOLID;
	if (Cmd == AV_ME_GLOWSHELL)	newCmd = AV_PLY_GLOWSHELL;
	if (Cmd == AV_ME_TEAM)		newCmd = AV_PLY_TEAM;

	if (newCmd > 0) AvRunPlayer(pEntity, pPev, MyNumber, pEntity, arg1, arg2, arg3, arg4, arg5, arg6, newCmd, 2);

}

void AvDrawClock( edict_t *pEntity )
{

	// Figure out where we want to actually draw this...

	UTIL_MakeVectors ( pEntity->v.v_angle );
	Vector vecSrc   = GetGunPosition( pEntity );
	// place it 50 units in front of player.
	Vector vecEnd   = vecSrc + gpGlobals->v_forward * 50;

	// Start drawing!

	int i;
	int n = 0;
	int r = 255;
	int g = 0;
	int b = 0;
	int dist = 0;
	vecEnd.z = vecEnd.z  + 40;


	struct tm *tmTime;
	time_t tTime;
	
	tTime=time(NULL);
	tmTime=localtime(&tTime);


	for ( i = 0; i < 96; i++) {	


		Vector vecP1 = vecEnd + gpGlobals->v_up * ClockData[i].y + gpGlobals->v_right * ClockData[i].x;
		
		n = ClockData[i].connectto;
		
		if (i == 21) { r = 0; g = 255;}

		if (n > -1)
		{
		
			
			Vector vecP2 = vecEnd + gpGlobals->v_up * ClockData[n].y + gpGlobals->v_right * ClockData[n].x;


			AvMakeBeam( vecP1, vecP2, 100, 4, r, g, b);

		}
	}

	// FINALLY, draw the hands.

	float pi = 3.141592654;

	// Get hand position...

	float a = 0;
	int hr = tmTime->tm_hour;
	int min = tmTime->tm_min ;

	if (hr > 12) hr = hr - 12;
	if (hr == 0) hr = 12;
		
	a = (360 / 12) * hr;
	a = a + 90;
	
	// also add the distance the MINUTE hand has moved..
	float extra = (((float)min) / 60);
	extra = extra * (360 / 12);

	a = a + extra;
		
	AvMakeHand( a, 30, vecEnd, 0, 100, 255);

	a = 0;

	a = (360 / 60) * min;
	a = a + 90;
		
	// convert
	
	AvMakeHand( a, 45, vecEnd, 0, 100, 255);



}

void AvMakeHand(float a, int rad, Vector vecEnd, int r, int g, int b)
{
	float pi = 3.141592654;
	float ar = 0;

	ar = (a - 10) * (pi / 180);
	int x = -(int)(cos(ar) * 5);
	int y = (int)(sin(ar) * 5);
	Vector vecSide1 = vecEnd + gpGlobals->v_up * y + gpGlobals->v_right * x;
	AvMakeBeam( vecSide1, vecEnd, 100, 4, r, g, b);

	ar = (a + 10) * (pi / 180);
	x = -(int)(cos(ar) * 5);
	y = (int)(sin(ar) * 5);
	Vector vecSide2 = vecEnd + gpGlobals->v_up * y + gpGlobals->v_right * x;
	AvMakeBeam( vecSide2, vecEnd, 100, 4, r, g, b);

	ar = a * (pi / 180);
	x = -(int)(cos(ar) * rad);
	y = (int)(sin(ar) * rad);
	Vector vecMn = vecEnd + gpGlobals->v_up * y + gpGlobals->v_right * x;
	
	AvMakeBeam( vecMn, vecSide1, 100, 4, r, g, b);
	AvMakeBeam( vecMn, vecSide2, 100, 4, r, g, b);

}

void AvMakeBeam( Vector vecP1, Vector vecP2, int life, int width, int r, int g, int b)
{

	MESSAGE_BEGIN( MSG_ALL, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD( vecP1.x );
		WRITE_COORD( vecP1.y );
		WRITE_COORD( vecP1.z );

		WRITE_COORD( vecP2.x );
		WRITE_COORD( vecP2.y );
		WRITE_COORD( vecP2.z );
		
		WRITE_SHORT( m_beamSprite );
		
		WRITE_BYTE( 1 ); // start frame
		WRITE_BYTE( 10 ); // frame rate
		WRITE_BYTE( life ); // life
		WRITE_BYTE( width ); // line width
		WRITE_BYTE( 0 ); // noise
		
		WRITE_BYTE( r );	// r
		WRITE_BYTE( g );	// g
		WRITE_BYTE( b );	// b
		
		WRITE_BYTE( 255 ); // brightness
		WRITE_BYTE( 10 ); // scroll speed

	MESSAGE_END();


}


void AvMakeBeam2( int x1, int y1, int z1, int x2, int y2, int z2, int life, int width, int r, int g, int b)
{

	MESSAGE_BEGIN( MSG_ALL, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD( x1 );
		WRITE_COORD( y1 );
		WRITE_COORD( z1 );

		WRITE_COORD( x2 );
		WRITE_COORD( y2 );
		WRITE_COORD( z2 );
		
		WRITE_SHORT( m_beamSprite );
		
		WRITE_BYTE( 1 ); // start frame
		WRITE_BYTE( 10 ); // frame rate
		WRITE_BYTE( life ); // life
		WRITE_BYTE( width ); // line width
		WRITE_BYTE( 0 ); // noise
		
		WRITE_BYTE( r );	// r
		WRITE_BYTE( g );	// g
		WRITE_BYTE( b );	// b
		
		WRITE_BYTE( 255 ); // brightness
		WRITE_BYTE( 10 ); // scroll speed

	MESSAGE_END();


}

void AvMakeBeam3( int x1, int y1, int z1, int x2, int y2, int z2)
{

	if (beamcount > 100) return;

	beamcount++;


	MESSAGE_BEGIN( MSG_ALL, SVC_TEMPENTITY );

		WRITE_BYTE( TE_SHOWLINE );
		WRITE_COORD( x1 );
		WRITE_COORD( y1 );
		WRITE_COORD( z1 );

		WRITE_COORD( x2 );
		WRITE_COORD( y2 );
		WRITE_COORD( z2 );

	MESSAGE_END();

}


void AvNonCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd )
{

	// First get what we are talking to. Format for ALL Player Commands:
	// nlocate <type> <typedata> <stuff to pass to PlayerCommand>
	//ex:
	// nlocate fred jake


	int MyNumber = ENTINDEX(pEntity);
	int MsgsSent = 0;

	if (FStrEq(arg2, "mem")) 
	{
		// This command to be performed to all players in memory
		// Usage: avb <command> mem <args>
		
		
		int i = AdminPlayerMems[MyNumber];
	
		if ( i > 0 && i <= gpGlobals->maxClients )
		{
			edict_t *pPlayerEdict = INDEXENT( i );

			if ( pPlayerEdict && !pPlayerEdict->free )
			{
				AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg3, arg4, arg5, arg6, arg7, arg8, arg9, "", Cmd - 300);
			}
		}

	}
	else if	(FStrEq(arg2, "front")) 
	{
		// This command to be performed to the player I am looking at.

		edict_t *pPlayerEdict = Av_GetFrontEnt ( pEntity );
		
		int i = ENTINDEX(pPlayerEdict);

		if ( i > 0 && i <= gpGlobals->maxClients )
		{
			pPlayerEdict = INDEXENT( i );

			if ( pPlayerEdict && !pPlayerEdict->free )
			{
				AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg3, arg4, arg5, arg6, arg7, arg8, arg9, "", Cmd - 300);
			}
		}
	}
	else if	(FStrEq(arg2, "all")) 
	{
		// This command to be performed to all players 

		int i;
		edict_t *pPlayerEdict;
		
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			if ( i != MyNumber && AdminLoggedIn[i] == 0)
			{
				pPlayerEdict = INDEXENT( i );
				if ( pPlayerEdict && !pPlayerEdict->free )
				{

					char *pClassname;
					pClassname = Av_GetClassname ( pPlayerEdict ); 
					if (FStrEq(pClassname, "player")) // make sure its actually a player.
					{
						AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg3, arg4, arg5, arg6, arg7, arg8, arg9, "", Cmd - 300);
					}
				}
			}
		}	
	}
	else if	(FStrEq(arg2, "realall")) 
	{
		// This command to be performed to all players 

		int i;
		edict_t *pPlayerEdict;
		
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
				pPlayerEdict = INDEXENT( i );
				if ( pPlayerEdict && !pPlayerEdict->free )
				{

					char *pClassname;
					pClassname = Av_GetClassname ( pPlayerEdict ); 
					if (FStrEq(pClassname, "player")) // make sure its actually a player.
					{
						AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg3, arg4, arg5, arg6, arg7, arg8, arg9, "", Cmd - 300);
					}
				}
		}	
	}
	else if	(FStrEq(arg2, "team")) 
	{
		// This command to be performed to all players whos team matches the specified one

		int i;
		edict_t *pPlayerEdict;
		
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			if ( i != MyNumber && AdminLoggedIn[i] == 0)
			{
				pPlayerEdict = INDEXENT( i );
				if ( pPlayerEdict && !pPlayerEdict->free )
				{

					char *pClassname;
					pClassname = Av_GetClassname ( pPlayerEdict ); 
					if (FStrEq(pClassname, "player")) // make sure its actually a player.
					{

						// Make sure team matches

						if (pPlayerEdict->v.team == atoi(arg3)) {
							AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg4, arg5, arg6, arg7, arg8, arg9, "", "", Cmd - 300);
						}
					}
				}
			}
		}	
	}
	else if	(FStrEq(arg2, "class")) 
	{
		// This command to be performed to all players whos class matches the specified one

		int i;
		edict_t *pPlayerEdict;
		
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			if ( i != MyNumber && AdminLoggedIn[i] == 0)
			{
				pPlayerEdict = INDEXENT( i );
				if ( pPlayerEdict && !pPlayerEdict->free )
				{

					char *pClassname;
					pClassname = Av_GetClassname ( pPlayerEdict ); 
					if (FStrEq(pClassname, "player")) // make sure its actually a player.
					{

						// Make sure class matches

						if (pPlayerEdict->v.playerclass == atoi(arg3)) {
							AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg4, arg5, arg6, arg7, arg8, arg9, "", "", Cmd - 300);
						}
					}
				}
			}
		}	
	}
	else
	{
		// This command to be performed to the player with this userid
		
		int usid = atoi(arg2);

		int i;
		edict_t *pPlayerEdict;
		

		if (usid == 0) {
			// Ok, so its not a user id. Search for matching names.
			
			char *playerName;

			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
				if ( i > 0 && i <= gpGlobals->maxClients )
				{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{

						playerName = (char *)STRING(pPlayerEdict->v.netname);

						char *pClassname;
						pClassname = Av_GetClassname ( pPlayerEdict ); 
						if (FStrEq(pClassname, "player")) // make sure its actually a player.
						{
							// Check NAME here
							if ( stristr( playerName, arg2 )) {
								// NAME matches, perform commands here.
								AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg3, arg4, arg5, arg6, arg7, arg8, arg9, "", Cmd - 300);
							}
						}
					}
				}
			}

		}
		else if (!FStrEq(arg2, ""))
		{

			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
				if ( i > 0 && i <= gpGlobals->maxClients )
				{
					pPlayerEdict = INDEXENT( i );
					if ( pPlayerEdict && !pPlayerEdict->free )
					{
						// Check USERID here
						if ( GETPLAYERUSERID( pPlayerEdict ) == usid ) {
							// UserID matches, perform commands here.

							char *pClassname;
							pClassname = Av_GetClassname ( pPlayerEdict ); 
							if (FStrEq(pClassname, "player")) // make sure its actually a player.
							{
								AvPlayerCommand( pPlayerEdict, VARS(pPlayerEdict), arg3, arg4, arg5, arg6, arg7, arg8, arg9, "", Cmd - 300);
							}
						}
					}
				}
			}
		}
	}

}


void AvAddClockPoints() 
{
	ClockData[0].x = 65;
	ClockData[0].y = -22;
	ClockData[0].connectto = -1;
	ClockData[1].x = 68;
	ClockData[1].y = 0;
	ClockData[1].connectto = 0;
	ClockData[2].x = 65;
	ClockData[2].y = 21;
	ClockData[2].connectto = 1;
	ClockData[3].x = 55;
	ClockData[3].y = 40;
	ClockData[3].connectto = 2;
	ClockData[4].x = 40;
	ClockData[4].y = 55;
	ClockData[4].connectto = 3;
	ClockData[5].x = 21;
	ClockData[5].y = 65;
	ClockData[5].connectto = 4;
	ClockData[6].x = -1;
	ClockData[6].y = 68;
	ClockData[6].connectto = 5;
	ClockData[7].x = -22;
	ClockData[7].y = 65;
	ClockData[7].connectto = 6;
	ClockData[8].x = -41;
	ClockData[8].y = 55;
	ClockData[8].connectto = 7;
	ClockData[9].x = -57;
	ClockData[9].y = 40;
	ClockData[9].connectto = 8;
	ClockData[10].x = -66;
	ClockData[10].y = 21;
	ClockData[10].connectto = 9;
	ClockData[11].x = -69;
	ClockData[11].y = -1;
	ClockData[11].connectto = 10;
	ClockData[12].x = -66;
	ClockData[12].y = -22;
	ClockData[12].connectto = 11;
	ClockData[13].x = -57;
	ClockData[13].y = -41;
	ClockData[13].connectto = 12;
	ClockData[14].x = -41;
	ClockData[14].y = -57;
	ClockData[14].connectto = 13;
	ClockData[15].x = -22;
	ClockData[15].y = -66;
	ClockData[15].connectto = 14;
	ClockData[16].x = 0;
	ClockData[16].y = -69;
	ClockData[16].connectto = 15;
	ClockData[17].x = 21;
	ClockData[17].y = -66;
	ClockData[17].connectto = 16;
	ClockData[18].x = 40;
	ClockData[18].y = -57;
	ClockData[18].connectto = 17;
	ClockData[19].x = 55;
	ClockData[19].y = -41;
	ClockData[19].connectto = 18;
	ClockData[20].x = 65;
	ClockData[20].y = -22;
	ClockData[20].connectto = 19;
	ClockData[21].x = -8;
	ClockData[21].y = 59;
	ClockData[21].connectto = -1;
	ClockData[22].x = -5;
	ClockData[22].y = 64;
	ClockData[22].connectto = 21;
	ClockData[23].x = -5;
	ClockData[23].y = 51;
	ClockData[23].connectto = 22;
	ClockData[24].x = -1;
	ClockData[24].y = 63;
	ClockData[24].connectto = -1;
	ClockData[25].x = 4;
	ClockData[25].y = 65;
	ClockData[25].connectto = 24;
	ClockData[26].x = 8;
	ClockData[26].y = 62;
	ClockData[26].connectto = 25;
	ClockData[27].x = 0;
	ClockData[27].y = 51;
	ClockData[27].connectto = 26;
	ClockData[28].x = 8;
	ClockData[28].y = 52;
	ClockData[28].connectto = 27;
	ClockData[29].x = 56;
	ClockData[29].y = 6;
	ClockData[29].connectto = -1;
	ClockData[30].x = 61;
	ClockData[30].y = 10;
	ClockData[30].connectto = 29;
	ClockData[31].x = 64;
	ClockData[31].y = 2;
	ClockData[31].connectto = 30;
	ClockData[32].x = 58;
	ClockData[32].y = 0;
	ClockData[32].connectto = 31;
	ClockData[33].x = 62;
	ClockData[33].y = -2;
	ClockData[33].connectto = 32;
	ClockData[34].x = 62;
	ClockData[34].y = -10;
	ClockData[34].connectto = 33;
	ClockData[35].x = 56;
	ClockData[35].y = -8;
	ClockData[35].connectto = 34;
	ClockData[36].x = 4;
	ClockData[36].y = -50;
	ClockData[36].connectto = -1;
	ClockData[37].x = -3;
	ClockData[37].y = -51;
	ClockData[37].connectto = 36;
	ClockData[38].x = -5;
	ClockData[38].y = -61;
	ClockData[38].connectto = 37;
	ClockData[39].x = -1;
	ClockData[39].y = -65;
	ClockData[39].connectto = 38;
	ClockData[40].x = 4;
	ClockData[40].y = -63;
	ClockData[40].connectto = 39;
	ClockData[41].x = 4;
	ClockData[41].y = -60;
	ClockData[41].connectto = 40;
	ClockData[42].x = -4;
	ClockData[42].y = -57;
	ClockData[42].connectto = 41;
	ClockData[43].x = -59;
	ClockData[43].y = 4;
	ClockData[43].connectto = -1;
	ClockData[44].x = -63;
	ClockData[44].y = 3;
	ClockData[44].connectto = 43;
	ClockData[45].x = -66;
	ClockData[45].y = 7;
	ClockData[45].connectto = 44;
	ClockData[46].x = -62;
	ClockData[46].y = 11;
	ClockData[46].connectto = 45;
	ClockData[47].x = -58;
	ClockData[47].y = 4;
	ClockData[47].connectto = 46;
	ClockData[48].x = -58;
	ClockData[48].y = -3;
	ClockData[48].connectto = 47;
	ClockData[49].x = -64;
	ClockData[49].y = -5;
	ClockData[49].connectto = 48;
	ClockData[50].x = -36;
	ClockData[50].y = -42;
	ClockData[50].connectto = -1;
	ClockData[51].x = -26;
	ClockData[51].y = -42;
	ClockData[51].connectto = 50;
	ClockData[52].x = -32;
	ClockData[52].y = -55;
	ClockData[52].connectto = 51;
	ClockData[53].x = -52;
	ClockData[53].y = -21;
	ClockData[53].connectto = -1;
	ClockData[54].x = -56;
	ClockData[54].y = -24;
	ClockData[54].connectto = 53;
	ClockData[55].x = -53;
	ClockData[55].y = -28;
	ClockData[55].connectto = 54;
	ClockData[56].x = -56;
	ClockData[56].y = -31;
	ClockData[56].connectto = 55;
	ClockData[57].x = -56;
	ClockData[57].y = -35;
	ClockData[57].connectto = 56;
	ClockData[58].x = -50;
	ClockData[58].y = -37;
	ClockData[58].connectto = 57;
	ClockData[59].x = -45;
	ClockData[59].y = -33;
	ClockData[59].connectto = 58;
	ClockData[60].x = -49;
	ClockData[60].y = -27;
	ClockData[60].connectto = 59;
	ClockData[61].x = -47;
	ClockData[61].y = -24;
	ClockData[61].connectto = 60;
	ClockData[62].x = -49;
	ClockData[62].y = -20;
	ClockData[62].connectto = 61;
	ClockData[63].x = -56;
	ClockData[63].y = 34;
	ClockData[63].connectto = -1;
	ClockData[64].x = -56;
	ClockData[64].y = 21;
	ClockData[64].connectto = 63;
	ClockData[65].x = -50;
	ClockData[65].y = 35;
	ClockData[65].connectto = -1;
	ClockData[66].x = -53;
	ClockData[66].y = 27;
	ClockData[66].connectto = 65;
	ClockData[67].x = -49;
	ClockData[67].y = 21;
	ClockData[67].connectto = 66;
	ClockData[68].x = -45;
	ClockData[68].y = 29;
	ClockData[68].connectto = 67;
	ClockData[69].x = -50;
	ClockData[69].y = 35;
	ClockData[69].connectto = 68;
	ClockData[70].x = -39;
	ClockData[70].y = 51;
	ClockData[70].connectto = -1;
	ClockData[71].x = -35;
	ClockData[71].y = 56;
	ClockData[71].connectto = 70;
	ClockData[72].x = -34;
	ClockData[72].y = 43;
	ClockData[72].connectto = 71;
	ClockData[73].x = -31;
	ClockData[73].y = 52;
	ClockData[73].connectto = -1;
	ClockData[74].x = -28;
	ClockData[74].y = 56;
	ClockData[74].connectto = 73;
	ClockData[75].x = -28;
	ClockData[75].y = 44;
	ClockData[75].connectto = 74;
	ClockData[76].x = 25;
	ClockData[76].y = 51;
	ClockData[76].connectto = -1;
	ClockData[77].x = 31;
	ClockData[77].y = 55;
	ClockData[77].connectto = 76;
	ClockData[78].x = 31;
	ClockData[78].y = 42;
	ClockData[78].connectto = 77;
	ClockData[79].x = 44;
	ClockData[79].y = 33;
	ClockData[79].connectto = -1;
	ClockData[80].x = 49;
	ClockData[80].y = 37;
	ClockData[80].connectto = 79;
	ClockData[81].x = 53;
	ClockData[81].y = 32;
	ClockData[81].connectto = 80;
	ClockData[82].x = 45;
	ClockData[82].y = 23;
	ClockData[82].connectto = 81;
	ClockData[83].x = 53;
	ClockData[83].y = 22;
	ClockData[83].connectto = 82;
	ClockData[84].x = 47;
	ClockData[84].y = -23;
	ClockData[84].connectto = -1;
	ClockData[85].x = 47;
	ClockData[85].y = -30;
	ClockData[85].connectto = 84;
	ClockData[86].x = 57;
	ClockData[86].y = -30;
	ClockData[86].connectto = 85;
	ClockData[87].x = 52;
	ClockData[87].y = -26;
	ClockData[87].connectto = -1;
	ClockData[88].x = 52;
	ClockData[88].y = -39;
	ClockData[88].connectto = 87;
	ClockData[89].x = 33;
	ClockData[89].y = -43;
	ClockData[89].connectto = -1;
	ClockData[90].x = 23;
	ClockData[90].y = -43;
	ClockData[90].connectto = 89;
	ClockData[91].x = 24;
	ClockData[91].y = -50;
	ClockData[91].connectto = 90;
	ClockData[92].x = 33;
	ClockData[92].y = -51;
	ClockData[92].connectto = 91;
	ClockData[93].x = 35;
	ClockData[93].y = -54;
	ClockData[93].connectto = 92;
	ClockData[94].x = 32;
	ClockData[94].y = -57;
	ClockData[94].connectto = 93;
	ClockData[95].x = 24;
	ClockData[95].y = -59;
	ClockData[95].connectto = 94;


}
