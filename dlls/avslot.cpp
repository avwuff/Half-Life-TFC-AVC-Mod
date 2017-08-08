// reserved slots stuff

#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "soundent.h"
#include "externmsg.h"
#include "engine.h"

#include "studio.h"
#include "bot.h"
#include "avdll.h"
#include "sadll.h"
#include "avslot.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;
bool IsReserved[33]; // keeping track of who is using reserved slots

int AvCountPlayers()
{
	// return number of connected players
	int i = 0;
	int count = 0;

	edict_t *pPlayerEdict;
	CBaseEntity *pPlayer = NULL;

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		if ( i > 0 && i <= gpGlobals->maxClients )
		{
			//pPlayerEdict = INDEXENT( i );

			pPlayer = UTIL_PlayerByIndex( i );
			if ( pPlayer )
			{
				pPlayerEdict = pPlayer->edict();
				if ( pPlayerEdict && !pPlayerEdict->free && FStrEq((char *)STRING(pPlayerEdict->v.classname), "player") && IsReserved[i] == 1 && GETPLAYERUSERID( pPlayerEdict ) > 0)
				{
					count++;	
				}
			}
		}
	}
	return count;
}
int AvCountPlayers2()
{
	// return number of connected players
	int i = 0;
	int count = 0;

	edict_t *pPlayerEdict;
	CBaseEntity *pPlayer = NULL;
	
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		if ( i > 0 && i <= gpGlobals->maxClients )
		{
			pPlayer = UTIL_PlayerByIndex( i );
			if ( pPlayer )
			{
				pPlayerEdict = pPlayer->edict();
				if ( pPlayerEdict && !pPlayerEdict->free && FStrEq((char *)STRING(pPlayerEdict->v.classname), "player") && GETPLAYERUSERID( pPlayerEdict ) > 0)
				{
					count++;	
				}
			}
		}
	}
	return count;
}

bool AvClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ]  )
{
	/*
	IsReserved[ENTINDEX(pEntity)] = 0;

	// Client Connect routine. We may be reserving some slots for regs!

	int reservedslots = (int)CVAR_GET_FLOAT("sa_reserveslots");
	int rescount = AvCountPlayers();
	int playercount = AvCountPlayers2();

	int res_left = reservedslots - rescount;
	
	int notres_left = gpGlobals->maxClients - reservedslots - playercount + rescount;
	
	// ok, would this person be using a reserved slot?
	if (AvCheckIfInList(pEntity))
	{
		IsReserved[ENTINDEX(pEntity)] = 1;
	}

	if (notres_left <= 0)
	{
		// yes.
		if (IsReserved[ENTINDEX(pEntity)] == 1)
		{
			// they are getting a reserved slot
		}
		else
		{
			// not in list, not getting reserved slot
			sprintf(szRejectReason, "The server is full, just keep trying!");
			return 0;
		}
	}
	*/
	return 1;
	
}

bool AvCheckIfInList( edict_t *pEntity )
{
	
	// ADDED JUL 30, 2017 - MAKES EVERYONE AN ADMIN
	return true;
	
	int hiswon = pfnGetPlayerWONId( pEntity );

	// go thru the list
	fp=fopen("wonlist.dat","r");
	if (fp == NULL) return 1;

	int i = 1;
	int j = 1;
	int test = 0;
	int totalnum = 0;
	int wonid = 0;
	bool isthere = 0;

	test = fscanf(fp, "%i\n", &totalnum);


	for (i=1; i < totalnum+1; i++) {
		// Scan each line of the file for a WONID
		test = fscanf(fp, "%i\n", &wonid);

		if (wonid == hiswon) isthere = 1;
	}

	fclose(fp);
	return isthere;
}
	
