// trackplayer.h
// 11/04/93 - (THUR) - added PauseTrack(), ResumeTrack(), RewindTrack().

#ifndef __TRACKPLAYER_H__
#define __TRACKPLAYER_H__ (1)

#include "types.h"

// *************** T Y P E S *****************


// ********** F U N C T I O N  P R O T O T Y P E S **************

extern int32	OpenTrackPlayer(
	int32	priority,
	int32	buffer_count,
	int32	buffer_size,
	void	*buffer_to_use,
	Item	MixerIns
);
extern int32	LinearTrackPlayer (void);
extern int32	LoopedTrackPlayer (void);
extern int32	SpliceTrack(char *filespec);
extern int32	AdvanceTrack(int32 left_volume, int32 right_volume);
extern int32	PauseTrack(void);
extern int32	ResumeTrack(void);
extern int32	RewindTrack(void);
extern int32	VolumeTrack(int32 left_volume, int32 right_volume);
extern void	CloseTrackPlayer(void);

#endif
