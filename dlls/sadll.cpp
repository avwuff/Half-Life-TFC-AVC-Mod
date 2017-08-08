// Avatar-X's Server Assistant
// Add-On DLL !!

// All code Copyright 2001 CyberWyre and Alex Hess

//	void		(*pfnAddServerCommand)		( char *cmd_name, void (*function) (void) );

#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "sadll.h"
#include "engine.h"

	// commands
#include "studio.h"
#include "bot.h"

#include "custmotd.h"

#include "avdll.h"


static FILE *fp;

// The needed CVARS

extern void AddLogEntry(char *entry);
extern DLL_FUNCTIONS other_gFunctionTable;
extern bool AdminLoggedIn[33];
extern playerinfo_t PlayerInfo[33];


cvar_t sa_disablenewmodels 		= {"sa_disablenewmodels","1"};
cvar_t sa_reloadcensorfile 		= {"sa_reloadcensorfile","0"};
cvar_t sa_censorenabled 		= {"sa_censorenabled","0"};
cvar_t sa_censorfile 			= {"sa_censorfile","censor.lst"};
cvar_t sa_allowshot 			= {"sa_allowshot","1"};
cvar_t sa_allowgrenades 		= {"sa_allowgrenades","1"};
cvar_t sa_rkillid 			= {"sa_rkillid","0"};

cvar_t sa_option9 			= {"sa_option9","0"};
cvar_t sa_option8 			= {"sa_option8","0"};
cvar_t sa_option7 			= {"sa_option7","0"};
cvar_t sa_option6 			= {"sa_option6","0"};
cvar_t sa_option5 			= {"sa_option5","0"};
cvar_t sa_option4 			= {"sa_option4","0"};
cvar_t sa_option3 			= {"sa_option3","0"};
cvar_t sa_option2 			= {"sa_option2","0"};
cvar_t sa_option1 			= {"sa_option1","0"};
cvar_t sa_question 			= {"sa_question","0"};
cvar_t sa_duration 			= {"sa_duration","0"};
cvar_t sa_abort 			= {"sa_abort","0"};

cvar_t sa_sendinfo 			= {"sa_sendinfo","0"};
cvar_t sa_setclassid 			= {"sa_setclassid","0"};
cvar_t sa_setclass 			= {"sa_setclass","0"};
cvar_t sa_shutupoff 			= {"sa_shutupoff","0"};
cvar_t sa_shutupon 			= {"sa_shutupon","0"};
cvar_t sa_logwritefreq  		= {"sa_logwritefreq","7"};

cvar_t sa_teleport_z 			= {"sa_teleport_z","0"};
cvar_t sa_teleport_y 			= {"sa_teleport_y","0"};
cvar_t sa_teleport_x 			= {"sa_teleport_x","0"};
cvar_t sa_teleport_usernum 		= {"sa_teleport_usernum","0"};

cvar_t sa_nameset 			= {"sa_nameset","0"};
cvar_t sa_namesetid 			= {"sa_namesetid","0"};

cvar_t sa_talkto 			= {"sa_talkto","0"};
cvar_t sa_talktoid 			= {"sa_talktoid","0"};
cvar_t sa_message_fadeout 		= {"sa_message_fadeout","0"};
cvar_t sa_message_fadein 		= {"sa_message_fadein","1"};
cvar_t sa_message_channel 		= {"sa_message_channel","0"};
cvar_t sa_message_effect 		= {"sa_message_effect","0"};
cvar_t sa_message_position_y  		= {"sa_message_position_y","0"};
cvar_t sa_message_position_x  		= {"sa_message_position_x","0"};
cvar_t sa_message_fxtime 		= {"sa_message_fxtime","0"};
cvar_t sa_message_holdtime 		= {"sa_message_holdtime","0"};
cvar_t sa_message_green2 		= {"sa_message_green2","0"};
cvar_t sa_message_green1 		= {"sa_message_green1","0"};
cvar_t sa_message_blue2 		= {"sa_message_blue2","0"};
cvar_t sa_message_blue1 		= {"sa_message_blue1","0"};
cvar_t sa_message_red2 			= {"sa_message_red2","0"};
cvar_t sa_message_red1 			= {"sa_message_red1","0"};

cvar_t sa_talk4 			= {"sa_talk4","0"};
cvar_t sa_talk3 			= {"sa_talk3","0"};
cvar_t sa_talk2 			= {"sa_talk2","0"};
cvar_t sa_talk1 			= {"sa_talk1","0"};
cvar_t sa_message 			= {"sa_message","0"};

cvar_t sa_ccommand 			= {"sa_ccommand","0"};
cvar_t sa_ccommandid 		= {"sa_ccommandid","0"};

cvar_t sa_conmsg 			= {"sa_conmsg","0"};
cvar_t sa_conmsgid 			= {"sa_conmsgid","0"};

cvar_t sa_setpoints 		= {"sa_setpoints","0"};
cvar_t sa_setpointsid		= {"sa_setpointsid","0"};


// new MOTD-popup stuff

cvar_t sa_popwindowfile		= {"sa_popwindowfile","0"};


float menushowing = 0;
float client_update_time = 0;
float logwritetimer = 0;
float next_checktime = 300;
float next_timeleft = 1;


// Map array
int MapArray[65][65]; // Stores Z coord at this location, used for keeping map data


censor_t censorlist[100];

bool shutup[33];

void SaStartFrame()
{
   
	
	if (next_checktime <= gpGlobals->time)
	{
		next_checktime = gpGlobals->time + 300;
		// Write the MAP ARRAY to a file.
		char entry[80];
		sprintf( entry, "Starting MAP ARRAY WRITE");
		AddLogEntry( entry );

		AvWriteMapArray();
	}

	#ifdef CS_DLL
		if (next_timeleft <= gpGlobals->time)
		{
			next_timeleft = gpGlobals->time + 1;
			// set mp_timeleft
			float timelimit = CVAR_GET_FLOAT( "mp_timelimit" ) * 60;
			
			if (timelimit > 0) {

				timelimit = timelimit - gpGlobals->time;
				int timeleft = (int)timelimit;
				
				CVAR_SET_FLOAT( "mp_timeleft", timeleft );
			}		
		}
	#endif
	

	if (logwritetimer <= gpGlobals->time)
	{
		float freq = (float) CVAR_GET_FLOAT( "sa_logwritefreq" );

		if (freq == 0) freq = 10;
		logwritetimer = gpGlobals->time + freq;

		// Now do the log write.
		char entry[80];
		sprintf( entry, "Starting LOG WRITE");
		AddLogEntry( entry );

		AvDoLogWrite();
	}

	// Prevention of TFC_NEWMODELS
    if (client_update_time <= gpGlobals->time)
	{
		client_update_time = gpGlobals->time + 10;

		char entry[80];
		sprintf( entry, "Starting MODEL PREVENTION");
		AddLogEntry( entry );

		AvPreventNewModels();
	}

}

void AvDoLogWrite()
{

	char temp[2048];
	sprintf(temp, "SAPOS: ");

	int i = 0;
	int isfirst = 0;

	edict_t *pPlayerEdict;

	
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		if ( i > 0 && i <= gpGlobals->maxClients )
		{
			pPlayerEdict = INDEXENT( i );
			if ( pPlayerEdict && !pPlayerEdict->free && pPlayerEdict->v.iuser1 == 0 && pPlayerEdict->v.movetype != MOVETYPE_NOCLIP && FStrEq((char *)STRING(pPlayerEdict->v.classname), "player"))
			{
				
				int usernum =  GETPLAYERUSERID( pPlayerEdict );
			
				// Add to the log list here.

				// AVPOS: 1=22,22,22&2=33,33,33
				if (isfirst != 0) sprintf(temp, "%s&", temp);
				isfirst = 1;

				sprintf(temp, "%s%i=%i,%i,%i", temp, usernum, (int)pPlayerEdict->v.origin.x, (int)pPlayerEdict->v.origin.y, (int)pPlayerEdict->v.origin.z);
			}
		}
	}

	sprintf(temp, "%s\n", temp);
	UTIL_LogPrintf( temp );

}

void AvPreventNewModels()
{

	// See if we are supposed to do it?

	int preventmodels = (int)CVAR_GET_FLOAT( "sa_disablenewmodels" );
	if (preventmodels == 0) return;

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
				
				
				CLIENT_COMMAND( pPlayerEdict, "tfc_newmodels 0\n");

				// Check if eye needs to be reset.
				
			}
		}
	}	

}



void SaPutInServer( edict_t *pEntity ) 
{
	int MyNumber = ENTINDEX(pEntity);
	// Clear the SHUTUP flag
	shutup[MyNumber] = 0;

}

void SaConnect( edict_t *pEntity ) 
{

	int MyNumber = ENTINDEX(pEntity);
	// Clear the SHUTUP flag
	shutup[MyNumber] = 0;

}

bool SaCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6)
{

	int MyNumber = ENTINDEX(pEntity);

	// Step 1: Check if its a MENU SELECT COMMAND and if so, check if we are currently running a vote.

	if (FStrEq(pcmd, "menuselect")) {
		// Are we running a vote?
		if (menushowing != 0 && gpGlobals->time <= menushowing) {
			// Yes we are, so print this data to the log.

			// Format required: VOTE: 1 registered for 10.
			UTIL_LogPrintf( "VOTE: %i registered for %i.\n", atoi(arg1), (int)GETPLAYERUSERID(pEntity));
		}
	}

	// Check the message for swear words

	// if the player is SHUT UP and he tried to use say_team, say, or tell, prevent him.
	
	if (shutup[MyNumber]) {

		if (FStrEq(pcmd, "say") || FStrEq(pcmd, "say_team") || FStrEq(pcmd, "tell"))
		{

			// prevent this command
			return 0;
		}
	}
	


	if (FStrEq(pcmd, "tell")) {
		// It's a TELL commands
		AvDoTell( pEntity );
	}

	if (FStrEq(pcmd, "writearray")) {
		AvWriteMapArray();

	}

	if (FStrEq(pcmd, "checkpoints")) {
		
		char msg[140];

		if (PlayerInfo[MyNumber].numPoints == -13)
		{
			sprintf(msg, "\n\nYour points have not been loaded. Please say ADMIN FIX MY POINTS to fix this.");
		}
		else
		{
			sprintf(msg, "\n\nYou have %i points!\n", PlayerInfo[MyNumber].numPoints);
		}
		ClientPrint( VARS(pEntity), HUD_PRINTCONSOLE, msg);
	}


	if (FStrEq(pcmd, "adm")) {
		// It's an ADMIN COMMAND
		AvDoAdm( pEntity );
	}

	if (FStrEq(pcmd, "+attack") || FStrEq(pcmd, "-attack")) {
		// See if prevention is on
		
		if (CVAR_GET_FLOAT( "sa_allowshot" ) == 0) {
			return 0;
		}
	}

	if (FStrEq(pcmd, "+gren1") || FStrEq(pcmd, "+gren2") || FStrEq(pcmd, "-gren1") || FStrEq(pcmd, "-gren2") || FStrEq(pcmd, "primeone") || FStrEq(pcmd, "primetwo")) {
		// See if prevention is on
		
		if (CVAR_GET_FLOAT( "sa_allowgrenades" ) == 0) {
			return 0;
		}
	}
	

	

	if (FStrEq(pcmd, "say") || FStrEq(pcmd, "say_team"))  
	{
	
		if (CVAR_GET_FLOAT("sa_switchtalk") == 2)
		{
	
			// swap what this guy is saying with somoene else.
			// to do this, simply pass along a different edict.
			
			bool dot = 1;
			if (CVAR_GET_FLOAT("sa_switchtalkavc") == 1 && AdminLoggedIn[ENTINDEX(pEntity)]) dot = 0;

			if (dot)
			{

				
				// check to make sure he's valid

				edict_t *pPlayerEdict = pEntity->v.euser3;
				bool good = 0;

				if ( pPlayerEdict  && !pPlayerEdict->free )
				{

					char *pClassname;
					pClassname = (char*)STRING(pPlayerEdict->v.classname);
					if (FStrEq(pClassname, "player")) // make sure its actually a player.
					{
						
						if (pPlayerEdict->v.playerclass > 0 && pPlayerEdict != pEntity)
						{
							good = 1;
						}
					}
				}

				edict_t *who;

				if (good) {
					who = pPlayerEdict;
				}
				else
				{	
					who = SaFindOther(pEntity);
				}

				pEntity->v.euser3 = who;
				
				// now pass it along from someone else.
				(*other_gFunctionTable.pfnClientCommand)(who);
			
				// disable the normal one
				return 0;
			}
		}
	}


    //AvCheckMessage();
	// Yes, pass this ClientCommand on.
	return 1;
}

edict_t *SaFindOther( edict_t *pEntity )
{

	// This command to be performed to all players 

	// first make an array of possible players
	int plycount = 0;
	int playerList[32];

	int i;
	edict_t *pPlayerEdict;
	edict_t *pPlayerEdict2;

	bool noavc = 0;
	if (CVAR_GET_FLOAT("sa_switchtalkavc") == 1) noavc = 1;

	
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pPlayerEdict = INDEXENT( i );
		if ( pPlayerEdict && !pPlayerEdict->free )
		{

			char *pClassname;
			pClassname = (char*)STRING(pPlayerEdict->v.classname);
			if (FStrEq(pClassname, "player")) // make sure its actually a player.
			{
				
				if (pPlayerEdict->v.playerclass > 0 && pPlayerEdict != pEntity)
				{

					if (noavc && AdminLoggedIn[i])
					{
						// an admin
					}
					else
					{

						// now, see if this "person" is already taken
						
						bool taken = 0;



						for ( int k = 1; k <= gpGlobals->maxClients; k++ )
						{
							pPlayerEdict2 = INDEXENT( k );
							if ( pPlayerEdict2 && !pPlayerEdict2->free )
							{

								char *pClassname2;
								pClassname2 = (char*)STRING(pPlayerEdict2->v.classname);
								if (FStrEq(pClassname2, "player")) // make sure its actually a player.
								{
									
									if (pPlayerEdict2->v.playerclass > 0 && pPlayerEdict2 != pEntity)
									{

										// is he taken?

										if (pPlayerEdict2->v.euser3 == pPlayerEdict) taken = 1;
									
									}
								}
							}
						}


						if (!taken)
						{
					
							// mark it in the array
							plycount++;
							playerList[plycount - 1] = i;
						}
					}
				}
			}
		}
	}	

	// ok, now see what we have.

	if (plycount > 0) 
	{
		
		// pick a random player
		int rn = (int)RANDOM_LONG(1, plycount);
		rn--;

		return INDEXENT(playerList[rn]);

	}
	else
	{

		return pEntity;
	}


}


bool SaPostPlayerThink( edict_t *pEntity ) 
{


	if (CVAR_GET_FLOAT("sa_switchtalk") == 1)
	{

		// swap what this guy is saying with somoene else.
		// to do this, simply pass along a different edict.
		
		bool dot = 1;
		if (CVAR_GET_FLOAT("sa_switchtalkavc") == 1 && AdminLoggedIn[ENTINDEX(pEntity)]) dot = 0;

		if (dot)
		{

			
			// check to make sure he's valid

			edict_t *pPlayerEdict = pEntity->v.euser3;
			bool good = 0;

			if ( pPlayerEdict  && !pPlayerEdict->free )
			{

				char *pClassname;
				pClassname = (char*)STRING(pPlayerEdict->v.classname);
				if (FStrEq(pClassname, "player")) // make sure its actually a player.
				{
					
					if (pPlayerEdict->v.playerclass > 0 && pPlayerEdict != pEntity)
					{
						good = 1;
					}
				}
			}

			edict_t *who;

			if (good) {
				who = pPlayerEdict;
			}
			else
			{	
				who = SaFindOther(pEntity);
			}

			pEntity->v.euser3 = who;
			
			// now pass it along from someone else.
			//(*other_gFunctionTable.pfnPlayerPostThink)(who);

			// swap

			int buttons = pEntity->v.button;
			//pEntity->v.button = who->v.button;
			who->v.button = buttons;


		
			// disable the normal one
			
		}
	}

	return 1;
}

bool SaClientUserInfoChanged( edict_t *pEntity, char *infobuffer) 
{

	// see if name changed while they are shut up
	int MyNumber = ENTINDEX(pEntity);

	if (shutup[MyNumber]) {
		if ( pEntity->v.netname && STRING(pEntity->v.netname)[0] != 0 && !FStrEq( STRING(pEntity->v.netname), g_engfuncs.pfnInfoKeyValue( infobuffer, "name" )) )
		{
			return 0;	
		}
	}
	
	

	return 1;
}



void AvCheckTime(void) 
{

	// Check all the variables and if needed, perform a function.

	// Start with the TALK cvars

//		char entry[80];
		//sprintf( entry, "Starting AvCheckTime");
//		AddLogEntry( entry );

	#ifdef CRASHLOOK
		AvAddDebug( "Starting AVCHECKTIME...\n");
	#endif



	AvDoTalk( "sa_talk1" );
	AvDoTalk( "sa_talk2" );
	AvDoTalk( "sa_talk3" );
	AvDoTalk( "sa_talk4" );

	// Move on to the sa_message stuff.

	AvDoMessage();


	#ifdef CRASHLOOK
		AvAddDebug( "Checktime Marker 1...\n");
	#endif

	// Player teleportation

	AvDoTeleport();
	
	// Name changing

	AvDoNameSet();

	// Private Messaging

	AvDoTalkTo();

	#ifdef CRASHLOOK
		AvAddDebug( "Checktime Marker 2...\n");
	#endif


	// Console Printing

	AvDoConMsg();
	
	// Remote Killing

	AvDoRKill();

	// Selection Menu

	AvDoMenu();

	#ifdef CRASHLOOK
		AvAddDebug( "Checktime Marker 3...\n");
	#endif

	// Voicing

	AvCheckShutupOn();
	AvCheckShutupOff();

	// Class changing

	AvDoChangeClass();

	// Player Info

	AvDoPlayerInfo();

	#ifdef CRASHLOOK
		AvAddDebug( "Checktime Marker 4...\n");
	#endif


	// popups

	AvDoPopUp();
	
	// commands

	AvDoClientCommands();
	

	// points
	AvDoSetPoints();

	#ifdef CRASHLOOK
		AvAddDebug( "AvCheckTime complete!...\n");
	#endif


}


void AvCheckShutupOn()
{
	
	
	int usernum = (int)CVAR_GET_FLOAT( "sa_shutupon" );
	if (usernum == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;
	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		int MyNumber = ENTINDEX(pPlayerEdict);
		 
		shutup[MyNumber] = 1;

		char temp[200];
		sprintf(temp, "* Player %s was de-voiced.", (char *)STRING(pPlayerEdict->v.netname));

		UTIL_ClientPrintAll( HUD_PRINTTALK, temp );

	}
	CVAR_SET_STRING( "sa_shutupon", "0" );


}


void AvCheckShutupOff()
{
	
	
	int usernum = (int)CVAR_GET_FLOAT( "sa_shutupoff" );
	if (usernum == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;
	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		int MyNumber = ENTINDEX(pPlayerEdict);
		
		shutup[MyNumber] = 0;

		char temp[200];
		sprintf(temp, "* Player %s was re-voiced.", (char *)STRING(pPlayerEdict->v.netname));

		UTIL_ClientPrintAll( HUD_PRINTTALK, temp );

	}
	CVAR_SET_STRING( "sa_shutupoff", "0" );


}





void AvDoMenu() 
{

	// Creates a selection menu for people to pick from.
	/*
	sa_option9
	sa_option8
	sa_option7
	sa_option6
	sa_option5
	sa_option4
	sa_option3
	sa_option2
	sa_option1
	sa_question
	sa_duration
	sa_abort
	*/

	char menutext[1024];
	int bitsel = 0;
	sprintf(menutext, "Nothing");
	
	int abort = (int)CVAR_GET_FLOAT( "sa_duration" );
	if (abort != 0) {
		// Abort current vote
		menushowing = 0;
		// Reset abort commands
		CVAR_SET_STRING( "sa_abort", "0" );
	}
	
	int displaytime = (int)CVAR_GET_FLOAT( "sa_duration" );
	if (displaytime == 0) return;

	char *q = (char *)CVAR_GET_STRING( "sa_question" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s\n\n", q);
	}

	q = (char *)CVAR_GET_STRING( "sa_option1" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s1. %s\n", menutext, q);
		
		bitsel |= 1<<0; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option2" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s2. %s\n", menutext, q);
		bitsel |= 1<<1; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option3" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s3. %s\n", menutext, q);
		bitsel |= 1<<2; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option4" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s4. %s\n", menutext, q);
		bitsel |= 1<<3; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option5" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s5. %s\n", menutext, q);
		bitsel |= 1<<4; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option6" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s6. %s\n", menutext, q);
		bitsel |= 1<<5; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option7" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s7. %s\n", menutext, q);
		bitsel |= 1<<6; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option8" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s8. %s\n", menutext, q);
		bitsel |= 1<<7; // activate this choice in the menu
	}
	q = (char *)CVAR_GET_STRING( "sa_option9" );
	if (!FStrEq(q, "0")) {
		AvHexToString ( q );
		// Add to the big one
		sprintf(menutext, "%s9. %s\n", menutext, q);
		bitsel |= 1<<8; // activate this choice in the menu
	}
	
	// Now, show the menu to everyone!

	ShowMenu( bitsel, displaytime, 0, menutext);

	// Set the values that determine how long we accept input...
	
	menushowing = gpGlobals->time + displaytime;

	// Clear the CVARS

	CVAR_SET_STRING( "sa_question", "0" );
	CVAR_SET_STRING( "sa_duration", "0" );
	CVAR_SET_STRING( "sa_option1", "0" );
	CVAR_SET_STRING( "sa_option2", "0" );
	CVAR_SET_STRING( "sa_option3", "0" );
	CVAR_SET_STRING( "sa_option4", "0" );
	CVAR_SET_STRING( "sa_option5", "0" );
	CVAR_SET_STRING( "sa_option6", "0" );
	CVAR_SET_STRING( "sa_option7", "0" );
	CVAR_SET_STRING( "sa_option8", "0" );
	CVAR_SET_STRING( "sa_option9", "0" );


}


void AvDoRKill()
{
	// Handles killing players

	//sa_rkillid
		
	int usernum = (int)CVAR_GET_FLOAT( "sa_rkillid" );
	if (usernum == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;
	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		// Run this command
		
		CLIENT_COMMAND( pPlayerEdict, "kill\n" );

	}

	CVAR_SET_STRING( "sa_rkillid", "0" );
}

void AvDoTalkTo()
{

	// handles the private messaging.
	//sa_talkto
	//sa_talktoid

	char *message = (char *)CVAR_GET_STRING( "sa_talkto" );
	if (FStrEq(message, "0")) return;

	// convert from HEX

	AvHexToString ( message );
	
	int usernum = (int)CVAR_GET_FLOAT( "sa_talktoid" );
	if (usernum == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;
	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		// Send him the message
		sprintf( message, "%s\n", message );

		ClientPrint( VARS( pPlayerEdict ), HUD_PRINTTALK, message);
	}

	CVAR_SET_STRING( "sa_talkto", "0" );
	CVAR_SET_STRING( "sa_talktoid", "0" );
}


void AvDoConMsg()
{

	// handles the private messaging.
	//sa_conmsg
	//sa_conmsgid

	int length;

	// Send the message of the day
	// read it chunk-by-chunk,  and send it in parts

	int usernum = (int)CVAR_GET_FLOAT( "sa_conmsgid" );
	if (usernum == 0) return;


	char *file = (char *)CVAR_GET_STRING( "sa_conmsg" );
	if (FStrEq(file, "0")) return;

	char *message = (char*)LOAD_FILE_FOR_ME( file, &length );

	

	ALERT(at_console, "Part 1!\n");


	if (message && length > 0)
	{

		// Find this player.
		
		edict_t *pPlayerEdict;
		
		pPlayerEdict = AvFindPlayerByUserID( usernum );
		if (pPlayerEdict != NULL) {

			// Send him the message
			strcat( message, "\n");

			//ClientPrint( VARS( pPlayerEdict ), HUD_PRINTTALK, message);

			CLIENT_PRINTF( pPlayerEdict, print_console, message);

		}
	}
	

	CVAR_SET_STRING( "sa_conmsg", "0" );
	CVAR_SET_STRING( "sa_conmsgid", "0" );
}


void AvDoSetPoints()
{

	// handles the setpoints
	//sa_setpoints
	//sa_setpointsid

	int usernum = (int)CVAR_GET_FLOAT( "sa_setpointsid" );
	if (usernum == 0) return;


	int pointnum = (int)CVAR_GET_FLOAT( "sa_setpoints" );

	edict_t *pPlayerEdict;
	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		PlayerInfo[ENTINDEX(pPlayerEdict)].numPoints = pointnum;

	}

	CVAR_SET_STRING( "sa_setpoints", "0" );
	CVAR_SET_STRING( "sa_setpointsid", "0" );
}




void AvDoClientCommands()
{

	// handles the C Commands
	//sa_talkto
	//sa_talktoid

	char *message = (char *)CVAR_GET_STRING( "sa_ccommand" );
	if (FStrEq(message, "0")) return;

	// convert from HEX

	AvHexToString ( message );
	
	int usernum = (int)CVAR_GET_FLOAT( "sa_ccommandid" );
	if (usernum == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;
	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		// Send him the message
		sprintf( message, "%s\n", message );
		CLIENT_COMMAND( pPlayerEdict, message );
	}

	CVAR_SET_STRING( "sa_ccommand", "0" );
	CVAR_SET_STRING( "sa_ccommandid", "0" );
}


void AvDoPopUp()
{

	// handles the MOTD-style popup msg
	//sa_popwindowfile

	char *message = (char *)CVAR_GET_STRING( "sa_popwindowfile" );
	if (FStrEq(message, "0")) return;

	// send it

	SendFileToAll( message );


	CVAR_SET_STRING( "sa_popwindowfile", "0" );
}


void AvDoNameSet()
{
	// Handles changing of player name.

	//sa_nameset
	//sa_namesetid
	
	char *newname = (char *)CVAR_GET_STRING( "sa_nameset" );

	char nameagain[256];

	if (FStrEq(newname, "0")) return;

	// convert from HEX

	AvHexToString ( newname );
	
	int usernum = (int)CVAR_GET_FLOAT( "sa_namesetid" );
	if (usernum == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;

	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		// Run this command
		sprintf (nameagain, "name \"%s\"\n", newname);
		CLIENT_COMMAND( pPlayerEdict, nameagain );

	}

	CVAR_SET_STRING( "sa_nameset", "0" );
	CVAR_SET_STRING( "sa_namesetid", "0" );

}

//'PLAYERINFO: 20  |   253   |  255  |  0   |    0   |   0   |    NO    |   NO  |  NO
//'Format: ENT NUM | USER ID | CLASS | TEAM | HEALTH | ARMOR | DEVOICED | ADMIN | FROZEN


void AvDoPlayerInfo()
{
	
	int doit = (int)CVAR_GET_FLOAT( "sa_sendinfo" );
	if (doit == 0) return;

	// Package the info.

	char temp[2048];
	
	

	int i = 0;
	edict_t *pPlayerEdict;

	
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		if ( i > 0 && i <= gpGlobals->maxClients )
		{
			pPlayerEdict = INDEXENT( i );
			if ( pPlayerEdict && !pPlayerEdict->free )
			{

				

				int userid =  GETPLAYERUSERID( pPlayerEdict );

				int cls = pPlayerEdict->v.playerclass;
				int team = pPlayerEdict->v.team;
				int health = pPlayerEdict->v.health;
				int armor = pPlayerEdict->v.armorvalue;
				
				if (shutup[i])
				sprintf(temp, "PLAYERINFO: %i  |  %i  |  %i  |  %i  |  %i  |  %i  |  YES  |  NO  |  NO\n",
					                       i,     userid, cls,   team,  health,armor );

				if (!shutup[i])
				sprintf(temp, "PLAYERINFO: %i  |  %i  |  %i  |  %i  |  %i  |  %i  |  NO  |  NO  |  NO\n",
					                       i,     userid, cls,   team,  health,armor );

			
				UTIL_LogPrintf( temp );
			}
		}
	}


	CVAR_SET_STRING( "sa_sendinfo", "0" );
}


void AvDoChangeClass()
{
	// Handles changing of player name.

	//sa_setclassid
	//sa_setclass

	
	int usernum = (int)CVAR_GET_FLOAT( "sa_setclassid" );
	if (usernum == 0) return;

	int cla = (int)CVAR_GET_FLOAT( "sa_setclass" );
	if (cla == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;

	
	pPlayerEdict = AvFindPlayerByUserID( usernum );
	if (pPlayerEdict != NULL) {

		pPlayerEdict->v.playerclass = cla;

		// also gotta kill him and print it in the log
		CLIENT_COMMAND( pPlayerEdict, "kill\n" );

		//"Duncan Idaho<43><412124><Blue>" changed class to "Civilian"

		char temp[200];
		
		char team[15];
		char cls[20];
		AvGetTeam( pPlayerEdict, team );
		AvGetClass( pPlayerEdict, cls );
	
		

		sprintf(temp, "\"%s<%i><%i><%s>\" changed role to \"%s\"\n", 
			(char *)STRING(pPlayerEdict->v.netname), (int)GETPLAYERUSERID( pPlayerEdict ), 
			(int)pfnGetPlayerWONId ( pPlayerEdict ), team, cls);


		UTIL_LogPrintf( temp );
			
	}

	CVAR_SET_STRING( "sa_setclass", "0" );
	CVAR_SET_STRING( "sa_setclassid", "0" );
}

void AvGetTeam( edict_t *pEntity , char *team)
{
	// return the team
	
	sprintf(team, "Unknown");
	if (pEntity->v.team == 1) sprintf(team, "Blue");
	if (pEntity->v.team == 2) sprintf(team, "Red");
	if (pEntity->v.team == 3) sprintf(team, "Yellow");
	if (pEntity->v.team == 4) sprintf(team, "Green");


}

void AvGetClass( edict_t *pEntity , char *team)
{
	// return the team
	
	sprintf(team, "Unknown");
	if (pEntity->v.playerclass == 1) sprintf(team, "Scout");
	if (pEntity->v.playerclass == 2) sprintf(team, "Sniper");
	if (pEntity->v.playerclass == 3) sprintf(team, "Soldier");
	if (pEntity->v.playerclass == 4) sprintf(team, "Demoman");
	if (pEntity->v.playerclass == 5) sprintf(team, "Medic");
	if (pEntity->v.playerclass == 6) sprintf(team, "HWGuy");
	if (pEntity->v.playerclass == 7) sprintf(team, "Pyro");
	if (pEntity->v.playerclass == 8) sprintf(team, "Spy");
	if (pEntity->v.playerclass == 9) sprintf(team, "Engineer");
	if (pEntity->v.playerclass == 11) sprintf(team, "Civilian");


}
edict_t *AvFindPlayerByUserID(int usernum) 
{
	
	int i = 0;
	edict_t *pPlayerEdict;
	
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		if ( i > 0 && i <= gpGlobals->maxClients )
		{
			pPlayerEdict = INDEXENT( i );
			if ( pPlayerEdict && !pPlayerEdict->free )
			{
				// Check USERID here
				if ( GETPLAYERUSERID( pPlayerEdict ) == usernum ) {
					// UserID matches, perform commands here.
	
					return pPlayerEdict;
				}
			}
		}
	}
	
	return NULL;


}

void AvDoTeleport() 
{

	// handles these 4 CVARs:

	//sa_teleport_z
	//sa_teleport_y
	//sa_teleport_x
	//sa_teleport_usernum

	int usernum = (int)CVAR_GET_FLOAT( "sa_teleport_usernum" );

	if (usernum == 0) return;

	// Find this player.

	edict_t *pPlayerEdict;
	Vector pTeleport;

	
	pPlayerEdict = AvFindPlayerByUserID( usernum );

	if (pPlayerEdict != NULL) {
		pTeleport.x = (int)CVAR_GET_FLOAT( "sa_teleport_x" );
		pTeleport.y = (int)CVAR_GET_FLOAT( "sa_teleport_y" );
		pTeleport.z = (int)CVAR_GET_FLOAT( "sa_teleport_z" );
		
		// Set the origin.

		SET_ORIGIN(pPlayerEdict, pTeleport );
	}
	// Clear the CVARS

	CVAR_SET_STRING( "sa_teleport_x", "0" );
	CVAR_SET_STRING( "sa_teleport_y", "0" );
	CVAR_SET_STRING( "sa_teleport_z", "0" );
	CVAR_SET_STRING( "sa_teleport_usernum", "0" );
}



void AvDoTalk( const char *CVarName )
{
	
	char *saywhat = (char *)CVAR_GET_STRING( CVarName );

	if (FStrEq(saywhat, "0")) return;

	// convert from HEX

	AvHexToString ( saywhat );

	// send to all clients
	sprintf( saywhat, "%s\n", saywhat );

	UTIL_ClientPrintAll( HUD_PRINTTALK, saywhat );

	// Clear the CVAR

	CVAR_SET_STRING(  CVarName, "0" );
	//UTIL_LogPrintf( "Admin Say: %s", saywhat) ;
}

void AvDoMessage() 
{

	char *saywhat = (char *)CVAR_GET_STRING( "sa_message" );

	if (FStrEq(saywhat, "0")) return;

	// convert from HEX

	AvHexToString ( saywhat );

	hudtextparms_t m_textParms;

	m_textParms.r1 = CVAR_GET_FLOAT("sa_message_red1");
	m_textParms.g1 = CVAR_GET_FLOAT("sa_message_green1");
	m_textParms.b1 = CVAR_GET_FLOAT("sa_message_blue1");
	m_textParms.a1 = 0;

	m_textParms.r2 = CVAR_GET_FLOAT("sa_message_red2");
	m_textParms.g2 = CVAR_GET_FLOAT("sa_message_green2");
	m_textParms.b2 = CVAR_GET_FLOAT("sa_message_blue2");
	m_textParms.a2 = 0;

	m_textParms.fadeinTime = CVAR_GET_FLOAT("sa_message_fadein");
	m_textParms.fadeoutTime = CVAR_GET_FLOAT("sa_message_fadeout");
	m_textParms.holdTime = CVAR_GET_FLOAT("sa_message_holdtime");
	m_textParms.fxTime = CVAR_GET_FLOAT("sa_message_fxtime");
	m_textParms.x = CVAR_GET_FLOAT("sa_message_position_x");
	m_textParms.y = CVAR_GET_FLOAT("sa_message_position_y");

	m_textParms.effect= CVAR_GET_FLOAT("sa_message_effect");
	m_textParms.channel= CVAR_GET_FLOAT("sa_message_channel");

	UTIL_HudMessageAll( m_textParms, saywhat );

	sprintf( saywhat, "%s\n", saywhat );
	
	UTIL_ClientPrintAll( HUD_PRINTCONSOLE, saywhat );


	// Clear the CVAR
	CVAR_SET_STRING(  "sa_message" , "0" );

}



void SaPrecache() 
{
	client_update_time = 0;
	next_checktime = 300;
	logwritetimer = 0;
	next_timeleft = 1;


	/*
	char *mapname = AvGetMapName();
	
	fp=fopen("commandlog.txt","a");
	fprintf(fp, "Command by %s: %s\n", person, command);
	fclose(fp);
	*/
	
	// AT THIS POINT WRITE THE CURRENT MAP ARRAY AND THEN CLEAR IT.
	// UNLESS THE FILE ALREADY EXISTS

	// clear map array
	for (int x = 0; x < 65; x++)
	{
		for (int y = 0; y < 65; y++)
		{
			MapArray[x][y] = 0;
		}
	}
}

void AvPrePlayerThink( edict_t *pEntity) 
{

	// Set this value in the maparray.

	if (pEntity->v.groundentity && pEntity->v.flags & FL_ONGROUND)
	{

//		ALERT(at_console, "Ground Ent of Player %s is a %s\n", (char*)STRING(pEntity->v.netname), (char*)STRING(pEntity->v.groundentity->v.classname));

		
		int i;
		edict_t *pPlayerEdict;
		
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			if ( INDEXENT( i ) == pEntity->v.groundentity )
			{
				pPlayerEdict = INDEXENT( i );

				if (FStrEq((char*)STRING(pPlayerEdict->v.classname), "player")) // make sure its actually a player.
				{
					// Set my velocity, etc

					pEntity->v.velocity.x = pPlayerEdict->v.velocity.x;
					pEntity->v.velocity.y = pPlayerEdict->v.velocity.y;
					pEntity->v.velocity.z = pPlayerEdict->v.velocity.z;

				}
			}
		}	

	}



	// make sure its a valid player (not a spec and not clipping)
	if (pEntity->v.iuser1 != 0) return;
	if (pEntity->v.movetype == MOVETYPE_NOCLIP) return;
	
	int x = (int)(pEntity->v.origin.x/128)+32;
	int y = (int)(pEntity->v.origin.y/128)+32;
	int z = (int)(pEntity->v.origin.z);

	x--;
	y++;

	if (x < 0 || x > 64) return;
	if (y < 0 || y > 64) return;

    int t1 = 0;
    if (pEntity->v.team == 1) t1 = 8192;
    if (pEntity->v.team == 2) t1 = 16384;
    if (pEntity->v.team == 3) t1 = -8192;
    if (pEntity->v.team == 4) t1 = -16384;
                      
    //See if this is a dual-team point.
    int t2 = AvGetMapArrayTeam(MapArray[x][y]);
    
    if (t2 == 5) t1 = 0;
    if (t2 != 5 && t2 != 0 && t2 != pEntity->v.team) t1 = 24675; //This point is NOT a one-team point.

    z = z + t1;
	
	
    MapArray[x][y] = z;

}

int AvGetMapArrayTeam(int z)
{

	// 20481   to  28972 -> more than one team    (norm: -24576)
	if (z >= 4097 && z <= 12288) return 1;
	if (z >= 12289 && z <= 20480) return 2;
	if (z >= 2048) return 5;
	if (z <= -4097 && z >= -12288) return 3;
	if (z <= -12289) return 4;
	
	return 0;

}

void AvWriteMapArray()
{
	// Writes my version of the map array to a file.
	char *mapname = AvGetMapName();
	char filename[80];

	sprintf(filename, "%s.ama", mapname);
	
	// see if it exists...
	
	fp=fopen(filename,"r");
	if (fp != NULL) {
		fclose(fp);
		return;
	}
	
	//fclose(fp);

	fp=fopen(filename,"w");
	//ok we are all set.
	// start writing.
	int x;
	int y;
	
	for (y=0; y<65; y++)
	{
		for (x=0; x<65; x++)
		{
			// Write to file.
			fprintf(fp, "%i,", MapArray[x][y]);
	
		}
		//new line
		fprintf(fp, "\n", MapArray[x][y]);
	}

	fclose(fp);

}

char *AvGetMapName()
{
	char *pMapname = (char *)STRING(gpGlobals->mapname);
	return pMapname;
}

void AvGameInit() 
{

	pfnAddServerCommand( "checktime", AvCheckTime );

	// Register the CVARS
		

	CVAR_REGISTER (&sa_disablenewmodels);
	CVAR_REGISTER (&sa_reloadcensorfile);
	CVAR_REGISTER (&sa_censorenabled);
	CVAR_REGISTER (&sa_censorfile);
	CVAR_REGISTER (&sa_allowshot);
	CVAR_REGISTER (&sa_allowgrenades);
	CVAR_REGISTER (&sa_rkillid);

	CVAR_REGISTER (&sa_option9);
	CVAR_REGISTER (&sa_option8);
	CVAR_REGISTER (&sa_option7);
	CVAR_REGISTER (&sa_option6);
	CVAR_REGISTER (&sa_option5);
	CVAR_REGISTER (&sa_option4);
	CVAR_REGISTER (&sa_option3);
	CVAR_REGISTER (&sa_option2);
	CVAR_REGISTER (&sa_option1);
	CVAR_REGISTER (&sa_question);
	CVAR_REGISTER (&sa_duration);
	CVAR_REGISTER (&sa_abort);

	CVAR_REGISTER (&sa_sendinfo);
	CVAR_REGISTER (&sa_setclassid);
	CVAR_REGISTER (&sa_setclass);
	CVAR_REGISTER (&sa_shutupoff);
	CVAR_REGISTER (&sa_shutupon);
	CVAR_REGISTER (&sa_logwritefreq);

	CVAR_REGISTER (&sa_teleport_z);
	CVAR_REGISTER (&sa_teleport_y);
	CVAR_REGISTER (&sa_teleport_x);
	CVAR_REGISTER (&sa_teleport_usernum);

	CVAR_REGISTER (&sa_nameset);
	CVAR_REGISTER (&sa_namesetid);

	CVAR_REGISTER (&sa_talkto);
	CVAR_REGISTER (&sa_talktoid);
	//conmsg
	CVAR_REGISTER (&sa_conmsg);
	CVAR_REGISTER (&sa_conmsgid);

	CVAR_REGISTER (&sa_setpoints);
	CVAR_REGISTER (&sa_setpointsid);


	CVAR_REGISTER (&sa_message_fadeout);
	CVAR_REGISTER (&sa_message_fadein);
	CVAR_REGISTER (&sa_message_channel);
	CVAR_REGISTER (&sa_message_effect);
	CVAR_REGISTER (&sa_message_position_y );
	CVAR_REGISTER (&sa_message_position_x );
	CVAR_REGISTER (&sa_message_fxtime);
	CVAR_REGISTER (&sa_message_holdtime);
	CVAR_REGISTER (&sa_message_green2);
	CVAR_REGISTER (&sa_message_green1);
	CVAR_REGISTER (&sa_message_blue2);
	CVAR_REGISTER (&sa_message_blue1);
	CVAR_REGISTER (&sa_message_red2);
	CVAR_REGISTER (&sa_message_red1);

	CVAR_REGISTER (&sa_talk4);
	CVAR_REGISTER (&sa_talk3);
	CVAR_REGISTER (&sa_talk2);
	CVAR_REGISTER (&sa_talk1);
	CVAR_REGISTER (&sa_message);
	CVAR_REGISTER (&sa_popwindowfile);

	CVAR_REGISTER (&sa_ccommand);
	CVAR_REGISTER (&sa_ccommandid);
	

}

void AvHexToString(char *hexstring)
{
	const char *hexchar = "0123456789abcdef";
    
	int length = (int)strlen(hexstring);
    int counter;
    int c2;
    // if the string's length is even, somthing is wrong and it's not a proper hex string, so return
    if((!fmod(length,2))||length<3) return;

    for(counter=0;counter<length/2;counter++)
	{
    	for(c2 = 0; c2 < (int)(int)strlen(hexchar); c2++)
       	{
        if(tolower(hexstring[counter*2+1])==hexchar[c2])
          	hexstring[counter]=(int)strlen(hexchar)*c2;
        }
    	for(c2=0;c2 < (int)(int)strlen(hexchar);c2++)
      	{
        if(tolower(hexstring[counter*2+2])==hexchar[c2])
          	hexstring[counter]+=c2;
        }
    }

	// Set last character to 0?

	hexstring[(int)length/2]=0;
	hexstring[((int)length/2) + 1]=0;

}


void AvDoTell(edict_t *pEntity)
{
	
	// Print this occurence of TELL to the log file.

	char temp[1024];

	sprintf(temp,"\"%s<%i>\" tell %s\n", (char *)STRING(pEntity->v.netname), (int)GETPLAYERUSERID( pEntity ),  (char *)Cmd_Args());
	
	UTIL_LogPrintf( temp );

	// Format
	//"[BD]Avatar-X [DRP]<119>" tell "[BD]Avatar-X" yo
	

}



void AvDoAdm(edict_t *pEntity)
{
	
	// SACMD: Player "<id>" used "<command>"

	char temp[1024];
	sprintf(temp,"SACMD: Player \"%i\" used \"%s\"\n", (int)GETPLAYERUSERID( pEntity ),  (char *)Cmd_Args());

	UTIL_LogPrintf( temp );
}




void AvCheckMessage() 
{

	// See if it's a SAY or SAY_TEAM


	if(Cmd_Argc()>=2)
	{
		//if(FStrEq(Cmd_Argv(0),"say") || FStrEq(Cmd_Argv(0),"say_team"))
		if(FStrEq(Cmd_Argv(0),"say"))
		{
			char temp[1024];

			sprintf(temp,"%s",(char *)Cmd_Args());
			
//			UTIL_ClientPrintAll(HUD_PRINTTALK, temp);
			
			// UNFINISHED DUE TO CRASH

			//StringSearchReplace("fuck","foo",temp);
			
//			sprintf((char *)Cmd_Args(),"%s",temp);



		}
	}
}


void AvGetCensorList()
{
	// Reads the censor list from the file

}

// Bills STRING REPLACEMENT STUFF


int StringSearchReplace(char Search[128], char Replace[128], char Text[1024])
 {
 char newSearch[128], newText[1024], tempText[2048], tempstring[1024] = "\0";
 //TextIndex contains an array where TextIndex[x] is the position of the character newText[x] in Text
 //ReplaceIndex contains an array where ReplaceIndex[x] is the position of the character to replace newText[x] with in Replace
 int TextIndex[1024], ReplaceIndex[1024], ShortenIndex[1024];
 int c1, c2, c3, matchflag;
 int numberofmatches = 0;
 //First, we need to convert the newSearch and newText strings to lowercase letters only.
 for(c1=0;c1<=(int)strlen(Search);c1++)
  newSearch[c1]=tolower(Search[c1]);
 for(c1=0;c1<=(int)strlen(Text);c1++)
  newText[c1]=tolower(Text[c1]);
 for(c1=0,c2=0;c1<(int)strlen(newSearch);c1++)
  {
  if(isalpha(newSearch[c1]))
	{
	tempstring[c2]=newSearch[c1];
	tempstring[c2+1]=*"\0";
	c2++;
	}
  }
 strcpy(newSearch, tempstring);
 if((int)strlen(newSearch)<1)
  return -1;
 //Index every point in the stripped string with each point in the original string.
 for(c1=0, c2=0;c1<(int)strlen(newText);c1++)
  if(isalpha(newText[c1]))
	{
	TextIndex[c2++] = c1;
	}
 for(c1=0,c2=0;c1<(int)strlen(newText);c1++)
  {
  if(isalpha(newText[c1]))
	{
	tempstring[c2] = newText[c1];
	tempstring[c2+1]=*"\0";
	c2++;
	}
  }
 strcpy(newText, tempstring);

 //Now to search the string an create the ReplaceIndex array with -1 for not a substring otherwise the subscript of the char in the Replace string.
 for(c1=0;c1<((int)strlen(newText)-((int)strlen(newSearch)-1));c1++)
  {
  matchflag=0;
  ReplaceIndex[c1]=-1;
  if(newText[c1]==newSearch[0])
	{
	matchflag=1;
	ReplaceIndex[c1]=0;
	for(c2=1;(c2<(int)strlen(newSearch))&&matchflag;c2++)
	 if(newText[c1+c2]!=newSearch[c2])
	  {
	  matchflag=0;
	  ReplaceIndex[c1]=-1;
	  }
	 else
	  ReplaceIndex[c1+c2]=c2;
	if(matchflag)
	 {
	 numberofmatches++;
	 c1+=((int)strlen(newSearch)-1);
	 }
	}
  }

 //make sure the last few indexes are -1
 while(c1<(int)strlen(newText))
  ReplaceIndex[c1++]=-1;
 //Now to actually replace the characters in the original string
 strcpy(tempstring,Text);
 for(c1=0;c1<(int)strlen(newText);c1++)
  if(ReplaceIndex[c1]==-1)
	tempstring[TextIndex[c1]]=Text[TextIndex[c1]];
  else
	if((int)strlen(Replace)>ReplaceIndex[c1])
	 tempstring[TextIndex[c1]]=Replace[ReplaceIndex[c1]];
 strcpy(Text,tempstring);
 //Now go through the string inserting the rest of the Replace string wherever it is cut short
 if((int)strlen(Replace)>(int)strlen(newSearch))
  {
  strcpy(tempText,Text);
  for(c1=0;c1<(int)strlen(newText);c1++)
	if(ReplaceIndex[c1]==(int)strlen(newSearch)-1)
	 {
	 strcpy(tempstring,&tempText[TextIndex[c1]+1]);
	 for(c2=TextIndex[c1],c3=0;c2<=(TextIndex[c1]+((int)strlen(Replace)-(int)strlen(newSearch)));c2++,c3++)
	  {
	  tempText[c2]=Replace[(int)strlen(newSearch)+c3-1];
	  }
	 strcpy(&tempText[TextIndex[c1]+c3],tempstring);
	 for(c2=c1;c2<(int)strlen(newText);c2++)
	  TextIndex[c2]+=(c3-1);
	 }
	for(c1=0;c1<1024;c1++)
	 Text[c1]=tempText[c1];
	Text[c1]=*"\0";
	}
 //Now if (int)strlen(Replace) < (int)strlen(newSearch), the string must be shortened
 if((int)strlen(Replace)<(int)strlen(newSearch))
  {
  for(c1=0;c1<1024;c1++)
	ShortenIndex[c1]=0;
  for(c1=0;c1<(int)strlen(newText);c1++)
	if((ReplaceIndex[c1]>=0)&&(ReplaceIndex[c1]>=(int)strlen(Replace)))
	 ShortenIndex[TextIndex[c1]]=1;
  for(c1=0,c2=0;c1<1024;c1++,c2++)
	if(ShortenIndex[c1])
	 c2--;
	else
	 tempText[c2]=Text[c1];
  for(c1=0;c1<1024;c1++)
	Text[c1]=tempText[c1];
  Text[c1]=*"\0";
  };




 return numberofmatches;
 };



char *AvGetClassname( edict_t *pEntity ) 
{

	char *pClassname = (char *)STRING(pEntity->v.classname);
	return pClassname;

}



/*
int StringSearchReplace(char Search[128], char Replace[128], char Text[1024])
 {
 char newSearch[128], newText[1024], tempText[2048], tempstring[1024] = "\0";
 //TextIndex contains an array where TextIndex[x] is the position of the character newText[x] in Text
 //ReplaceIndex contains an array where ReplaceIndex[x] is the position of the character to replace newText[x] with in Replace
 int TextIndex[1024], ReplaceIndex[1024], ShortenIndex[1024];
 int c1, c2, c3, matchflag;
 int numberofmatches = 0;
 if((int)strlen(Text)==0)
  return 0;
 //First, we need to convert the newSearch and newText strings to lowercase letters only.
 for(c1=0;c1<=(int)(int)strlen(Search);c1++)
  newSearch[c1]=tolower(Search[c1]);
 for(c1=0;c1<=(int)(int)strlen(Text);c1++)
  if(isalpha(Text[c1]))
   numberofmatches++;
 if(!numberofmatches)
  return 0;
 numberofmatches = 0;
 for(c1=0;c1<=(int)(int)strlen(Text);c1++)
  newText[c1]=tolower(Text[c1]);
 for(c1=0,c2=0;c1<(int)(int)strlen(newSearch);c1++)
  {
  if(isalpha(newSearch[c1]))
	{
	tempstring[c2]=newSearch[c1];
	tempstring[c2+1]=*"\0";
	c2++;
	}
  }
 sprintf(newSearch, "%s",tempstring);
 if((int)strlen(newSearch)<1)
  return -1;
 //Index every point in the stripped string with each point in the original string.
 for(c1=0, c2=0;c1<(int)(int)strlen(newText);c1++)
  if(isalpha(newText[c1]))
	{
	TextIndex[c2++] = c1;
	}
 for(c1=0,c2=0;c1<(int)(int)strlen(newText);c1++)
  {
  if(isalpha(newText[c1]))
	{
	tempstring[c2] = newText[c1];
	tempstring[c2+1]=*"\0";
	c2++;
	}
  }
 sprintf(newText,"%s", tempstring);

 //Now to search the string an create the ReplaceIndex array with -1 for not a substring otherwise the subscript of the char in the Replace string.
 for(c1=0;c1<((int)(int)strlen(newText)-((int)(int)strlen(newSearch)-1));c1++)
  {
  matchflag=0;
  ReplaceIndex[c1]=-1;
  if(newText[c1]==newSearch[0])
	{
	matchflag=1;
	ReplaceIndex[c1]=0;
	for(c2=1;(c2<(int)(int)strlen(newSearch))&&matchflag;c2++)
	 if(newText[c1+c2]!=newSearch[c2])
	  {
	  matchflag=0;
	  ReplaceIndex[c1]=-1;
	  }
	 else
	  ReplaceIndex[c1+c2]=c2;
	if(matchflag)
	 {
	 numberofmatches++;
	 c1+=((int)strlen(newSearch)-1);
	 }
	}
  }

 //make sure the last few indexes are -1
 while(c1<(int)(int)strlen(newText))
  ReplaceIndex[c1++]=-1;
 //Now to actually replace the characters in the original string
 sprintf(tempstring,"%s",Text);
 for(c1=0;c1<(int)(int)strlen(newText);c1++)
  if(ReplaceIndex[c1]==-1)
	tempstring[TextIndex[c1]]=Text[TextIndex[c1]];
  else
	if((int)(int)strlen(Replace)>ReplaceIndex[c1])
	 tempstring[TextIndex[c1]]=Replace[ReplaceIndex[c1]];
 sprintf(Text,"%s",tempstring);
 //Now go through the string inserting the rest of the Replace string wherever it is cut short
 if((int)strlen(Replace)>(int)strlen(newSearch))
  {
  sprintf(tempText,"%s",Text);
  for(c1=0;c1<(int)(int)strlen(newText);c1++)
	if(ReplaceIndex[c1]==(int)(int)strlen(newSearch)-1)
	 {
	 sprintf(tempstring,"%s",&tempText[TextIndex[c1]+1]);
	 for(c2=TextIndex[c1],c3=0;c2<=(TextIndex[c1]+((int)(int)strlen(Replace)-(int)(int)strlen(newSearch)));c2++,c3++)
	  tempText[c2]=Replace[(int)strlen(newSearch)+c3-1];
	 sprintf(&tempText[TextIndex[c1]+c3],"%s",tempstring);
	 for(c2=c1;c2<(int)(int)strlen(newText);c2++)
	  TextIndex[c2]+=(c3-1);
	 }
	for(c1=0;c1<1024;c1++)
	 Text[c1]=tempText[c1];
	Text[c1]=*"\0";
	}
 //Now if (int)strlen(Replace) < (int)strlen(newSearch), the string must be shortened
 if((int)strlen(Replace)<(int)strlen(newSearch))
  {
  for(c1=0;c1<1024;c1++)
	ShortenIndex[c1]=0;
  for(c1=0;c1<(int)(int)strlen(newText);c1++)
	if((ReplaceIndex[c1]>=0)&&(ReplaceIndex[c1]>=(int)(int)strlen(Replace)))
	 ShortenIndex[TextIndex[c1]]=1;
  for(c1=0,c2=0;c1<1024;c1++,c2++)
	if(ShortenIndex[c1])
	 c2--;
	else
	 tempText[c2]=Text[c1];
  for(c1=0;c1<1024;c1++)
	Text[c1]=tempText[c1];
  Text[c1]=*"\0";
  };




 return numberofmatches;
 };

*/
