// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

char *stristr (const char * str1, const char * str2);

#define TOOLS_MAX_CONSOLE_CMDS	32

typedef struct threadInfo_st {
	char name[128];
	uint32 tID;
} threadInfo_t;

extern const char *namestealText[NAMESTEAL_MAX];

class CTools
{
public:
	CTools();
	void Init(int Game);
	void Shutdown();
	uint32 crc32FromFile(const char *filename);
	void initCrc32();
	unsigned int ClientNumForAddr(uint32 dwCentAddr);
	int ScanCrosshairEntity(void);	// Returns entNum if crosshair is over Ent, else returns -1 (even through walls)
	pack_t *getPack(char *pk3filename);

	void CharToVec3(const char *txt, vec3_t v);
	void CharToVec4(const char *txt, vec4_t v);

	void SaveSettings(void);
	void LoadSettings(bool loadGui);
	void DataAsHex(char *out, void *in, size_t size);

	void SetUserCvars();
	void SaveUserCvars();
	void AddUserCvar(char *cvarName, char *cvarValue);
	void LoadUserCvars();
	
	void PBNamespoof(char *name);
	void NameSteal(int recurse);

	// this seems out of place here
	void VectorAngles(const vec3_t forward, vec3_t angles);

	char *CleanString(char *string);
	void UrlString(char *string, int size);
	void RemoveUrlSpaces(char *string);

	const char *GetRandomName(void);

	const char *GetEtVersionString(int version);
	const char *GetModVersionString(int version);

	void RegisterThread(uint32 tID, const char *name);
	threadInfo_t *nThreads;
	int sThreads;

	cvarInfo_t *userCvarList;

	uint32 GetPrivateProfileString(const char* lpAppName,const char* lpKeyName,const char* lpDefault, char* lpReturnedString,uint32 nSize,const char* lpFileName);
	bool WritePrivateProfileString(const char *lpAppName, const char *lpKeyName, const char *lpString, const char *lpFileName);

	void Print(const char *txtFmt, ...);
	
	void *FindPattern(void *haystack, int haystacklen, const char *pattern, const char *mask);

	int			lastNamestealTime;
	char		(*nameList)[256];
	int			nNames;	

	
private:
	void getOffsetFromINI( const char *key, int *offset );
	void getOffsetFromINI( const char *key, uint32 *offset );
	bool loadOffsets();
	char offsetFile[MAX_PATH];
	char __section[64];

	char errorMessage[1024];
	uint32 block_chksum(uchar *block, uint32 length);

	uint32 crc_tab[256];
	pthread_mutex_t *fp_mutex;
	pthread_mutex_t *cc_mutex;
	pthread_mutex_t *print_mutex;
	
	int			targetID;
	
	bool 		getAmmo();
	int 		ammo;
	int 		clip;	
};

extern CTools Tools;
