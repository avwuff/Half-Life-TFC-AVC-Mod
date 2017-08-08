#define MAX_MOTD_CHUNK	  60
#define MAX_MOTD_LENGTH 1024
void SendMOTDToOne( edict_t *client, char *file );
void SendPopupToOne( edict_t *client, char *msg );
void SendPopupToAll( char *msg );
void SendFileToAll( char *file );
