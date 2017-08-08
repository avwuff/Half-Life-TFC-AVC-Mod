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
#include "avrecord.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

edict_t *man = NULL;

bool recording = 0;
bool playing = 0;




recordframe_t RecFrames[5000][3];
float RecTimes[5000][3];
Vector vangles[5000][3];

float startTime[2] ;
float startTimePlay[2];

int CurrFrame[2];
int CurrPlayFrame[2];

int CurrTrack = 0;
int PlayTrack = 0;

char lastSend[256];


void EvClientCommand( edict_t *pEntity)
{

	const char *pcmd = Cmd_Argv(0);
	const char *arg1 = Cmd_Argv(1);
	const char *arg2 = Cmd_Argv(2);

	if (FStrEq(pcmd, "startrec"))
	{
		StartRecording(pEntity);

		if (atoi(arg1) == 1) { // play the other track
		
			PlayTrack = 1 - CurrTrack;

			man = pEntity;
			StartPlayback();
			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "STARTED PLAYING ANOTHER TRACK!");

		}
	}
	else if (FStrEq(pcmd, "stoprec"))
	{
		StopRecording();
	}
	else if (FStrEq(pcmd, "stopplay"))
	{
		playing = 0;
	}
	else if (FStrEq(pcmd, "startplay"))
	{

		PlayTrack = CurrTrack;

		man = pEntity;
		StartPlayback();

	}
	else if (FStrEq(pcmd, "settrack"))
	{

		CurrTrack = atoi(arg1);
	}

	else if (FStrEq(pcmd, "saverec"))
	{
		// save the recording

		char filename[20];
		sprintf(filename, "%s.rec", arg1);



		fp=fopen(filename,"w");
	
		fprintf(fp, "Total Events: %i\n", CurrFrame[CurrTrack] - 1);

		for (int i = 0; i < CurrFrame[CurrTrack]; i++)
		{

			fprintf(fp, "%f - ~%s~ %f %f %f\n", RecTimes[i][CurrTrack], RecFrames[i][CurrTrack].value, vangles[i][CurrTrack].x, vangles[i][CurrTrack].y, vangles[i][CurrTrack].z    );
		}
	
		fclose(fp);	
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "FILE SAVED!!\n");

	}
	else if (FStrEq(pcmd, "merge"))
	{	
		Merge();

	}
	else if (FStrEq(pcmd, "addtime"))
	{	
		
		startTimePlay[PlayTrack] += atof(arg1);

	}
	else if (FStrEq(pcmd, "loadrec"))
	{
		char filename[20];
		sprintf(filename, "%s.rec", arg1);

		fp = fopen(filename,"r");
	
		if (fp == NULL) return;
		
		// read file
		int test = 0;

			
		test = fscanf(fp, "Total Events: %i\n", &CurrFrame[CurrTrack]);
		CurrFrame[CurrTrack]++;

		// start reading

		for (int i = 0; i < CurrFrame[CurrTrack]; i++)
		{
			//char read1[280];
			test = fscanf(fp, "%f - ~%[^~]~ %f %f %f\n", & RecTimes[i][CurrTrack], &RecFrames[i][CurrTrack].value, &vangles[i][CurrTrack].x, &vangles[i][CurrTrack].y, &vangles[i][CurrTrack].z  );
			ALERT(at_console, "LOADED FRAME: %i : %f : %s\n", i, RecTimes[i][CurrTrack],  RecFrames[i][CurrTrack].value );
		}

		ALERT(at_console, "FILE LOADED: %i frames\n", CurrFrame[CurrTrack] );
		fclose(fp);	
	}
	else if (recording == 1 && man == pEntity)
	{
		RecordEvent();
		
	}

}
void Merge()
{

		fp=fopen("merge.txt","a");
		fprintf(fp, "point 1");
		fclose(fp);


		// merge track 0 and 1 into track 0

		int count1 = 0;
		int count2 = 0;
		int do1 = 1;
		int do2 = 1;
		float lasttime = 0;
		float time1 = 0;
		float time2 = 0;

		
		int countt = 0;

		fp=fopen("merge.txt","a");
		fprintf(fp, "point 2");
		fclose(fp);
		int mode = 0;
		int totalcount = 0;

		while (do1 + do2 > 0 && totalcount < 100000)
		{

			
			time1 = RecTimes[count1][0];
			time2 = RecTimes[count2][1];
			
			mode = 0;

			if (time1 - lasttime < time2 - lasttime) mode = 1;


			if (mode == 1 && do1)
			{
				// this one goes first
				sprintf(RecFrames[countt][2].value , "%s", RecFrames[count1][0].value );
				RecTimes[countt][2] = time1;
				vangles[countt][2] = vangles[count1][0];

				//increase
				count1++;
				countt++;

				lasttime = time1;

			}
			else if (do2)
			{
				// this one goes first
				sprintf(RecFrames[countt][2].value , "%s", RecFrames[count2][1].value );
				RecTimes[countt][2] = time2;
				vangles[countt][2] = vangles[count2][1];

				//increase
				count2++;
				countt++;
				lasttime = time2;
			}

			if (count1 >= CurrFrame[0]) do1 = 0;
			if (count2 >= CurrFrame[1]) do2 = 0;
			totalcount++;

		}

		fp=fopen("merge.txt","a");
		fprintf(fp, "point 3");
		fclose(fp);

		// done, now copy to track 1
		for (int i = 0; i < countt; i++)
		{
				sprintf(RecFrames[i][0].value , "%s", RecFrames[i][2].value );
				RecTimes[i][0] = RecTimes[i][2];
				vangles[i][0] = vangles[i][2];
		}

		CurrFrame[0] = countt;
		fp=fopen("merge.txt","a");
		fprintf(fp, "point 4");
		fclose(fp);
		
		ALERT(at_console, "MERGED!: %i frames total: %i\n", CurrFrame[0], totalcount );

}
void StartRecording( edict_t *pEntity )
{
	sprintf(lastSend, "ACH NEIN!");
	man = pEntity;
	recording = 1;

	// clear the recording array
	CurrFrame[CurrTrack] = 0;
	startTime[CurrTrack] = gpGlobals->time;

	ALERT(at_console, "RECORDING STARTED\n");
}
void StopRecording( )
{
	recording = 0;

	ALERT(at_console, "RECORDING STOPPED\n");
}
void RecordEvent()
{

	float eventTime = gpGlobals->time;

	// record this event
	char Event[256];

	sprintf( Event , "%s %s", Cmd_Argv(0), Cmd_Args() );
	if (FStrEq(lastSend, Event))
	{
		sprintf(lastSend, "ACH NEIN!");
		return; 
	}
	
	sprintf( RecFrames[CurrFrame[CurrTrack]][CurrTrack].value , "%s", Event );
	RecTimes[CurrFrame[CurrTrack]][CurrTrack] = eventTime - startTime[CurrTrack];
	vangles[CurrFrame[CurrTrack]][CurrTrack] = man->v.v_angle;

	CurrFrame[CurrTrack]++;

	if (CurrFrame[CurrTrack] >= 5000)
	{
		StopRecording();
	}
	ALERT(at_console, "RECORDED EVENT: %i   - %f\n", CurrFrame[CurrTrack], eventTime );
}
void StartPlayback()
{

	playing = 1;
	CurrPlayFrame[PlayTrack] = 0;
	startTimePlay[PlayTrack] = gpGlobals->time;

	ALERT(at_console, "STARTED PLAYING\n");

}
void PlaybackFrame()
{

	
	// Find out when its time to play this frame
	if (playing)
	{

		float playTime = startTimePlay[PlayTrack] + RecTimes[CurrPlayFrame[PlayTrack]][PlayTrack];

		if (playTime <= gpGlobals->time)
		{
			// play this event

			char playFrame[280];

			sprintf( playFrame, "%s\n", RecFrames[CurrPlayFrame[PlayTrack]][PlayTrack].value );
			sprintf( lastSend, "%s", RecFrames[CurrPlayFrame[PlayTrack]][PlayTrack].value);


			ALERT(at_console, "TRACK: %i PLAYING FRAME: %i  : %s\n", PlayTrack, CurrPlayFrame[PlayTrack], RecFrames[CurrPlayFrame[PlayTrack]][PlayTrack].value );
			
			man->v.angles = vangles[CurrPlayFrame[PlayTrack]][PlayTrack];
			man->v.v_angle = vangles[CurrPlayFrame[PlayTrack]][PlayTrack];
			man->v.fixangle = TRUE;

			CLIENT_COMMAND( man, playFrame );

			CurrPlayFrame[PlayTrack]++;

			if (CurrPlayFrame[PlayTrack] >= CurrFrame[PlayTrack])
			{

				playing = 0;
				ALERT(at_console, "STOPPED PLAYING\n");
			}
		}
	}
}