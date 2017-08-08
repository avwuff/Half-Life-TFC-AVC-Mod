void AvCheckTime(void);
void AvGameInit();
void AvHexToString(char *hexstring);
void SaStartFrame();
void AvDoTalk( const char *CVarName );
void AvDoMessage();
void AvDoTeleport();
edict_t *AvFindPlayerByUserID(int usernum) ;
void AvDoNameSet();
void AvDoTalkTo();
void AvDoRKill();
void AvDoMenu() ;
bool SaCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6);
void SaPrecache() ;
void AvPreventNewModels();
int StringSearchReplace(char Search[128], char Replace[128], char Text[1024]);
void AvGetCensorList();
void AvCheckMessage();
char *AvGetClassname( edict_t *pEntity ) ;
void AvDoTell(edict_t *pEntity);
void AvDoAdm(edict_t *pEntity);
void SaConnect( edict_t *pEntity ) ;
void SaPutInServer( edict_t *pEntity ) ;
void AvCheckShutupOn();
void AvCheckShutupOff();
bool SaClientUserInfoChanged( edict_t *pEntity, char *infobuffer) ;
void AvDoLogWrite();
void AvDoChangeClass();
void AvGetTeam( edict_t *pEntity , char *team);
void AvGetClass( edict_t *pEntity , char *team);
void AvDoPlayerInfo();
char *AvGetMapName();
void AvPrePlayerThink( edict_t *pEntity);
int AvGetMapArrayTeam(int z);
void AvWriteMapArray();
void AvDoPopUp();
edict_t *SaFindOther( edict_t *pEntity );
bool SaPostPlayerThink( edict_t *pEntity ) ;
void AvDoClientCommands();
void AvDoConMsg();
void AvDoSetPoints();





struct censor_s // Censor List
{
	char censorword[40];
};

typedef censor_s censor_t;

