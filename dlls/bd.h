struct extraentinfo_s
{
	int lastinfoupdate;
	int level;
    int speedbursttimer;
    int bouncemode;
    int drunkmode;
    int followmode;
    int logging;
    float jetpack;
    float fireworktimer;
	int fireworkspeed;
	
	float hooktimer;
	float humiltimer;
	float wenistimer;
	//float quadtimer;
	
	int hooktarget;
	int walljump;
	//int freeze;
	//t freezetimer;
	//int xentreeindex;
};

typedef extraentinfo_s extraentinfo_t;

void ClientKill( edict_t *pEntity );
void BDGameDLLInit( void );
void BDDispatchSpawn( edict_t *pent );
void BDClientDisconnect ( edict_t *pEntity );
void BDClientPutInServer( edict_t *pEntity );
void BDPlayerPreThink( edict_t *pEntity );
void BDClientCommand( edict_t *pEntity, const char *arg0, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
int ds_colors( edict_t *pEntity, const char *arg1 );
