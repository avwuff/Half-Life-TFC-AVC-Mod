bool SignCreate( edict_t *pEntity );
void SignPrecache();
void SignThink ( edict_t *pent );
void SignTouch ( edict_t *pent , edict_t *pentTouched);
bool SignRemove( edict_t *pEntity );

struct signtext_s 
{
	char text[80];
};

typedef signtext_s signtext_t;