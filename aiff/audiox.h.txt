// audiox.h
// 11/03/93 (WED) - 1.0 finished. (RGM)
// 11/03/93 (WED) - Added SetVolumeSpooledAudio() (RGM)
// 11/04/93 (THUR) - added PauseSpooledAudio(), ResumeSpooledAudio(), RewindSpooledAudio()
// 11/09/93 (TUES) - Added pre-allocated buffer support in OpenSpooledAudio(). (RGM)

#ifndef __AUDIOX_H__
#define __AUDIOX_H__ (1)

#include "types.h"

//************ F U N C T I O N   H E A D E R S ***********************

extern void	*OpenSpooledAudio (int32 buffer_count, int32 buffer_size,
			void *pre_allocated_buffer, Item MixerIns);

extern int32	LoopSpooledAudio (void *spool);
extern int32	LinearSpooledAudio (void *spool);

extern int32	LoadSpooledAudioTrack	(char *filename, void *spool);
extern int32	SpliceSpooledAudioTrack	(char *filename, void *spool);

extern int32	PlaySpooledAudio (void	*spool_player, int32 left_volume, int32 right_volume);
extern int32	AdvanceSpooledAudio (void *spool_player, int32 left_volume, int32 right_volume);

extern int32	PauseSpooledAudio (void *spool_player);

extern int32	ServiceSpooledAudio (void *spool_player, int32 load_signals, int32 *wait_signals);

extern int32	SetVolumeSpooledAudio (void *spool_player, int32 left_volume, int32 right_volume);

extern int32	PauseSpooledAudio (void *spool_player);

extern int32	ResumeSpooledAudio (void *spool_player);

extern int32	RewindSpooledAudio (void *spool_player);

extern void	CloseSpooledAudio (void	*audio_spool);

extern void	JumpSpooledAudio (void *spool_player);

extern void	FastForwardSpooledAudio (void *spool_player);

extern void	FastReverseSpooledAudio (void *spool_player);

extern int32	StopSpooledAudio (void *spool_player);

extern int32	SpooledAudioTrackPlaying (void *spool_player);

extern int32	SpooledAudioData (void *spool_player, char *data);

extern int32	SpooledAudioInfo (void *spool_player, int32 *len, int32 *offs);

#endif

