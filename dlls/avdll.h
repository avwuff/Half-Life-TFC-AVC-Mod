//H file

void AvCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9);
void AvEntCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd );
int AvRunEnt( edict_t *pEntity , entvars_t *pPev, int MyNumber, edict_t *pRunOn, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int Cmd);
void AvRunPlayer( edict_t *pEntity , entvars_t *pPev, int MyNumber, edict_t *pRunOn, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int Cmd, int FirstArg);
void AvPlayerCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd );
void AvRunMe( edict_t *pEntity , entvars_t *pPev, int MyNumber, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int Cmd);
void AvSelfCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd );
void AvRunPrecache();
void AvPutInServer( edict_t *pEntity );
void AvMakeAutoEnt(int n);
void AvBeforeTouch( edict_t *pEntity );
void AvAfterTouch( );
void AvStartFrame();
void AvModelChange( edict_t *pEntity, entvars_t *pPev, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, int MyNumber);
void AvMakeModels();
void AvSpawnOneModel(int GoalNum, char *Netname, char *Message, int GroupNum, char *ModelFile, char *Items, char *SpeedReduc, int Body, int Skin);
void AvSpawnRemove();
void AvSpawnTakegun();
void AvWriteEntsToFile(char *filename);
void AvReadEntsFromFile(entvars_t *pPev, char *filename);
void OldAvReadEntsFromFile(entvars_t *pPev, char *filename);
void AvWriteColor( const char *arg1, const char *arg2, const char *arg3);
void AvRemoveEye (edict_t *pEntity, int MyNumber);
void AvAttachEye (edict_t *pEntity, int MyNumber);
void AvAfterCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6);
void AvTouch (edict_t *pentUsed, edict_t *pentOther);
void AvShowMem( entvars_t *pRunOnPev, int memnum ) ;
void AvPathCommands( edict_t *pEntity, entvars_t *pPev, int MyNumber, const char *arg1, const char *arg2, const char *arg3, const char *arg4, int Cmd);
int AvCountPathNums( int MyNumber ) ;
void AvAddClockPoints() ;
void AvDrawClock( edict_t *pEntity );
void AvMakeBeam( Vector vecP1, Vector vecP2, int life, int width, int r, int g, int b);
void AvMakeHand(float a, int rad, Vector vecEnd, int r, int g, int b);
void AvMakeBeam2( int x1, int y1, int z1, int x2, int y2, int z2, int life, int width, int r, int g, int b);
void AvPrePlayerThink2 ( edict_t *pEntity );
bool AvCheckIfInField(  edict_t *pEntity );
void AvThink( edict_t *pEntity );
void AvLinearMove( edict_t *pEntity , Vector vecDest, float flSpeed );
int AvValue( const char *arg1, int deflt);
void AvMakeBeam3( int x1, int y1, int z1, int x2, int y2, int z2);
bool FStrLowEq( const char *sz1, const char *sz2 );
void AvSetDigit( edict_t *pEntity, int currdig, int setvalue );
void AvSetAlphaDigit( edict_t *pEntity, int currdig, int setvalue );
void AvSpawnMazeParts();
edict_t *AvMazeDup( edict_t *dupFrom, int orix, int oriy, int oriz);
void AvSwitchFront( edict_t *pEntity );
void AvRotFront( edict_t *pEntity );
void AvNonCommand( edict_t *pEntity , entvars_t *pPev, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, int Cmd );
void Dismount();
void PrivateChat(edict_t *pEntity);


int AddEntKey(char *keyname, char *keyvalue);
bool ClearKeys();
char *Av_GetKeyName ( int usekey );
char *Av_GetKeyValue ( int usekey ) ;
bool SetKeyValue(int usekey, char *keyvalue);
void AvUpdateShowMem();
void checkMapSpecificModels();

#define stristr(a, b) strstr(strupr(_strdup(a)), strupr(strdup(b)))


// Defines for the various commands.


// ent commands

#define AV_ENT_MOVE			1	//Move ent. Requires X Y Z
#define AV_ENT_ROT			2	//Rotate ent. Requires P Y R
#define AV_ENT_USE			3	//Use ent. Requires Activator
#define AV_ENT_TOUCH		4	//Touch ent. Requires Activator
#define AV_ENT_KILL			5	//Kill ent. Kinda stupid in a memory as it only works once
#define AV_ENT_HIDE			6	//Hide ent.
#define AV_ENT_SHOW			7	//Show ent.
#define AV_ENT_SETRENDER	8	//Set render properties. Requires Mode, Effect, and Amount
#define AV_ENT_SETSOLID		9	//Set solidity properties. Requires Mode.
#define AV_ENT_ADDTOMEM		10	//Add to ent memory
#define AV_ENT_REMFROMMEM	11	//Remove from ent memory
#define AV_ENT_CLEARMEM 	12	//Clear memory
#define AV_ENT_SHOWMEM  	13	//Show ents that are memorized
#define AV_ENT_HIDEMEM  	14	//Hide ents that are memorized
#define AV_ENT_LISTMEM  	15	//List ents that are memorized
#define AV_ENT_PRINTMEMNUM  16	//Prints the number of ents in memory to the screen
#define AV_ENT_MOVETOME		17	//Prints the number of ents in memory to the screen
#define AV_ENT_INFO			18	//Prints the number of ents in memory to the screen

#define AV_ENT_SETCOLOR		19	//Prints the number of ents in memory to the screen
#define AV_ENT_SETGOD		20	//Prints the number of ents in memory to the screen
#define AV_ENT_DAMAGE		21	
#define AV_ENT_SPAWN		22	
#define AV_ENT_DUPLICATE	23	
#define AV_ENT_VELOCITY		27
#define AV_ENT_AVELOCITY	28
#define AV_ENT_STOP			29
#define AV_ENT_WRITE		30
#define AV_ENT_READ			31

#define AV_ENT_KEYVALUE		32
#define AV_ENT_SWAPMEM		33
#define AV_ENT_FREE			34
#define AV_ENT_MAKETRAIN	44		// Make a train that follows a certain path.
#define AV_ENT_MORTARFIELD	45
#define AV_ENT_LIST			46
#define AV_ENT_MOVEREL		47
#define AV_ENT_MOVEVEL		48

#define AV_ENT_MAKECOPY		49
#define AV_ENT_COPYCLASS	50
#define AV_ENT_COPYKEY		51
#define AV_ENT_COPYSPAWN	52

#define AV_ENT_SHOWBOX		53
#define AV_ENT_SPAWNAGAIN	54
#define AV_ENT_SHOWTRIGGER	55
#define AV_ENT_SHOWPOS		56
#define AV_ENT_MOVEMETO		57
#define AV_ENT_MOREINFO		58	
#define AV_ENT_MAKEDOOR		59	

#define AV_ENT_MUSIC		60
#define AV_ENT_MSTOP		61	
#define AV_ENT_READOLD		62
#define AV_ENT_ALWAYS		63
#define AV_ENT_CIRCLE		64
#define AV_ENT_RADIOSONG	65
#define AV_ENT_RADIOPITCH	66

//#define AV_ENT_	58


// Path stuff
#define AV_ENT_PATHADD		35
#define AV_ENT_PATHMOVE		36
#define AV_ENT_PATHREM		37
#define AV_ENT_PATHSHOW		38
#define AV_ENT_PATHCLEAR	39
#define AV_ENT_PATHMAKE		40

// AutoSpawners
#define AV_ENT_STARTAUTO	24	
#define AV_ENT_LISTAUTO		25
#define AV_ENT_STOPAUTO		26

// Spawn crap


//int numAutoSpawners = 0; // max of 20

//Vector AutoSpawnOrigin[21];
//char AutoSpawnClassname[21];
//int AutoSpawnCapacity[21];
//int AutoSpawnCurrEnt[21];

// Player commands

#define AV_PLY_TELEPORT		151	//Teleports em to me
#define AV_PLY_METELEPORT	152	//Teleports me to em
#define AV_PLY_KILL			153	//Kills em
#define AV_PLY_COMMAND		154	//Runs a command on em
#define AV_PLY_GOD			155	//Sets god mode
#define AV_PLY_INVIS		156	//Sets invis mode
#define AV_PLY_GRAV			157	//Sets grav mode
#define AV_PLY_FLY			158	//Sets fly mode
#define AV_PLY_TARGET		159	//Sets target mode
#define AV_PLY_FRIC			160	//Sets friction
#define AV_PLY_SOLID		161	//Sets solidity
#define AV_PLY_ADDMEM		162	//Sets solidity
#define AV_PLY_TELELAST		163	//Sets solidity
#define AV_PLY_GLOWSHELL	164	//Sets solidity
#define AV_PLY_FUNNEL		165	//Sets solidity
#define AV_PLY_SPAWNPRES	166	//Sets solidity
#define AV_PLY_SPRITETRAIL	167	//Sets solidity
#define AV_PLY_BEAMTORUS	168	//Sets solidity
#define AV_PLY_BEAMFOLLOW	169	//Sets solidity
#define AV_PLY_BUBBLETRAIL	170	//Sets solidity
#define AV_PLY_PROJECTILE	171	//Sets solidity
#define AV_PLY_ATTACH		172	//Sets solidity
#define AV_PLY_REMATTACH	173	//Sets solidity
#define AV_PLY_FIREFIELD	174	//Sets solidity
#define AV_PLY_LOCATE		175	//Sets solidity
#define AV_PLY_TELETEST		176	//Sets solidity
#define AV_PLY_BEAMRING		177	//Sets solidity
#define AV_PLY_SENTRYGOD	178	//Sets solidity
#define AV_PLY_IUSER		179	//Sets solidity
#define AV_PLY_DRAWCLOCK	180	//Sets solidity
#define AV_PLY_VELUP		181
#define AV_PLY_TEAM			182
#define AV_PLY_CLASS		183
#define AV_PLY_MOVETYPE		184
#define AV_PLY_BEAM			185
#define AV_PLY_WEAPONS		186
#define AV_PLY_FRAGS		187
#define AV_PLY_HEALTH		188
#define AV_PLY_MAXHEALTH	189
#define AV_PLY_GIVEAIR		190
#define AV_PLY_SPEED		191
#define AV_PLY_FOV			192
#define AV_PLY_LONGBEAM		193
#define AV_PLY_MORTAR		194
#define AV_PLY_GIVE			195
#define AV_PLY_FLASHLIGHT	196

#define AV_PLY_SETVIEW		197
#define AV_PLY_RESETVIEW	198
#define AV_PLY_POPUP		199
#define AV_PLY_DISCO		200
#define AV_PLY_NOTGROUND	201
#define AV_PLY_CHANGEMODEL	202
#define AV_PLY_SPECIAL		203
#define AV_PLY_LEASH		204
#define AV_PLY_SPRITEBOOM	205
#define AV_PLY_WORDS		206

#define AV_PLY_SETVIEWANGLE	207

#define AV_PLY_COLORMAP		208
#define AV_PLY_SETPOINTS	209



// Self Commands

#define AV_ME_GOD			301	//Sets god mode
#define AV_ME_GRAV			302	//Sets god mode
#define AV_ME_FRIC			303	//Sets god mode
#define AV_ME_INVIS			304	//Sets god mode
#define AV_ME_FLY			305	//Sets god mode
#define AV_ME_TARGET		306	//Sets god mode
#define AV_ME_SOLID			307	//Sets god mode
#define AV_ME_GLOWSHELL		308	//Sets god mode
#define AV_ME_TEAM			309	//Sets god mode

// Non Me commands

#define AV_NON_TELEPORT		451	//Teleports em to me
#define AV_NON_LOCATE		475	//Sets solidity
#define AV_NON_BEAMRING		477	//Sets solidity


// Admin Flags 




//Command Ranges

#define AV_RANGE_ENT_START	1		- 1
#define AV_RANGE_ENT_END	100		+ 1

#define AV_RANGE_PLY_START	151		- 1
#define AV_RANGE_PLY_END	250		+ 1

#define AV_RANGE_ME_START	301		- 1
#define AV_RANGE_ME_END		350		+ 1

#define AV_RANGE_NON_START		451		- 1
#define AV_RANGE_NON_END		550		+ 1


// Other hardcoded numbers

#define AV_WONID_AV			392640
#define AV_WONID_BILL		261665
#define AV_WONID_FREAKY		18913
#define AV_WONID_INVIC		389568
#define AV_WONID_JEFFR		297467
#define AV_WONID_SKY		321251
#define AV_WONID_RADISH		660943
#define AV_WONID_CROWN  	553476


#define AV_POINTS_MODELS	8000


// Flags

#define AV_FLAG_SHOWMEM0	1		// Show memory as we add it!
#define AV_FLAG_SHOWMEM1	2		// Show memory as we add it!
#define AV_FLAG_SHOWMEM2	4		// Show memory as we add it!
#define AV_FLAG_SHOWMEM3	8		// Show memory as we add it!
#define AV_FLAG_SHOWMEM4	16		// Show memory as we add it!
#define AV_FLAG_SHOWMEM5	32		// Show memory as we add it!

// UniqueID's for these models:

#define AV_MODEL_AV			392640
#define AV_MODEL_FOX		140487
#define AV_MODEL_GMAN		251930
#define AV_MODEL_FREAKY		18913
#define AV_MODEL_INVIC		389568
#define AV_MODEL_TOAST		58890
#define AV_MODEL_COW		117658
#define AV_MODEL_SCUD		93301
#define AV_MODEL_RONALD		109239
#define AV_MODEL_SCIBAR		445468
#define AV_MODEL_BARNEY		115657

#define FL_ALWAYS				(1<<20) // always send



struct keydata_s
{
	char value[20];
};
typedef keydata_s keydata_t;

struct origentdata_s
{
	int rendermode;
	float renderamt;
    int renderfx;
    int rendercolorx;
    int rendercolory;
    int rendercolorz;
	bool alreadydone;
	bool justduped;
	bool donttouchme;	// dont modify this ent in any way, totally ignore it, as if it aint there, foo
	char digitgroup[20];
	edict_t *leash_master;
	int currpos;
	int mazex;
	int mazey;
	int mazeid;
	
	// storage of keyvalues
	int numkeys;
	
	int keyindex[128];

	bool isnotspawned;
	bool addingkey;
	bool hasorigin;
};
typedef origentdata_s origentdata_t;


struct recordframe_s
{
	char value[256];
};
typedef recordframe_s recordframe_t;


struct recordframe2_s
{
	char key[128];
	char value[256];
	bool inuse;
};
typedef recordframe2_s recordframe2_t;


struct phrases_s
{
	char phrase[128];
};

typedef phrases_s phrases_t;




struct pathdata_s // Data on an admin path currently under construction.
{
	int x;
	int y;
    int z;
	int flags;	// Spawnflags for things like teleport or wait for retrigger.
};

typedef pathdata_s pathdata_t;

struct clockdata_s 
{
	int x;
	int y;
	int connectto;
};

typedef clockdata_s clockdata_t;

struct playerinfo_s
{
	bool hasSpecialMode;
	bool superBright;
	bool noPlayers;
	bool privchat;
	bool racecam;
	bool racer;

	// tripmine color sets
	int tripmineR;
	int tripmineG;
	int tripmineB;

	// tripmine modes
	int tripMode;


	// number of points
	int numPoints;

};

typedef playerinfo_s playerinfo_t;



struct commandlog_s
{
	char value[512];
	char playername[40];
	float runtime;
};
typedef commandlog_s commandlog_t;



// DLL.CPP FUNCTIONS


void UpdateClientData(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd);
void AddLogEntry(char *entry);
void AvLinkUserMessages();
void fixSGS();
char *UTIL_StringSearchReplace(const char *Search, const char *Replace, const char *Input);
void addCommandToLog(const char *cmd, const char *args, const char *player);
void AvAddDebug( char* szFmt, ...);

