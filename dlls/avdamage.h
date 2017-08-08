void AvTakeDamage( entvars_t *pev, entvars_t *pevAttacker, entvars_t *pevInflictor, float flDamage, int bitsDamageType );
float AvDamageForce( entvars_t *pev, float damage );
void AvLinkUserMessages();
void AvKilled ( entvars_t *pev, entvars_t *pevAttacker, entvars_t *pevInflictor, int iGib );
void AvPlayerKilled( entvars_t *pev, entvars_t *pKiller, entvars_t *pInflictor );
void AvDeathNotice( entvars_t *pev, entvars_t *pKiller, entvars_t *pInflictor);
void AvSetActivity ( entvars_t *pev, char *szSequence );
int AvLookupSequence( entvars_t *pev, const char *label );
void AvRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int bitsDamageType );
