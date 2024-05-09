void	initAudio(int32 samprate);
void	startAudio(void);
void	stopAudio(void);
void	holdAudio(u32 reason);
void	unholdAudio(u32 reason);
u32		getAudioTag(void);
bool	stuffAudio(char *audbits,u32 audcnt,u32 tag);
void	flushAudio(void);
void	showbug(void);

/*
	Good reasons for holding audio:
*/
#define reasonQueue		1
#define reasonDX		2
#define reasonOther		4