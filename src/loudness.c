/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
 * Copyright (C) 2007  The OpenTyrian Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "opentyr.h"
#include "lds_play.h"
#include "fm_synth.h"

#define NO_EXTERNS
#include "loudness.h"
#undef NO_EXTERNS

/* TODO: Audio isn't really "working" yet. It makes noises but I still neeed to build
   the mixer and full audio system back here. Also need to add the music. So yeah, I
   know this code sucks right now. Ignore it, please. */


/* SYN: These are externally accessible variables: */
JE_MusicType musicData;
JE_boolean repeated;
JE_boolean playing;

/* SYN: These shouldn't be used outside this file. Hands off! */
SAMPLE_TYPE *channel_buffer [SFX_CHANNELS]; /* SYN: I'm not sure what Tyrian actually does for sound effect channels... */
SAMPLE_TYPE *channel_pos [SFX_CHANNELS];
SAMPLE_TYPE *music_buffer = NULL;
Uint32 channel_len [SFX_CHANNELS];
int sound_init_state = FALSE;
int freq = 11025 * OUTPUT_QUALITY;

/* SYN: TODO: Okay, some sound issues and what I'm going to do about them:
	- sfx are garbled when music is playing. Something is wrong with my mixing. Fix it.
	- music speed is wrong. This seems to be corrected by having a larger music buffer; for latency issues, 
	  this means I need a seperate buffer to play music into that is refilled as needed and copied in smaller
	  pieces to the main output buffer.
*/



void audio_cb(void *userdata, unsigned char *feedme, int howmuch);

/* SYN: The arguments to this function are probably meaningless now */
void JE_initialize(JE_word soundblaster, JE_word midi, JE_boolean mixenable, JE_byte sberror, JE_byte midierror)
{
    SDL_AudioSpec plz;
	int i = 0;
	
	sound_init_state = TRUE;
	
	/*final_audio_buffer = NULL;
	audio_pos = NULL;*/
	for (i = 0; i < SFX_CHANNELS + 1; i++)
	{
		channel_buffer[i] = channel_pos[i] = NULL;
		channel_len[i] = 0;
	}
	
    plz.freq = freq;
	plz.format = AUDIO_S16SYS;
    plz.channels = 1;
    plz.samples = 512 * 16;
    plz.callback = audio_cb;
    plz.userdata = NULL;

    if ( SDL_OpenAudio(&plz, NULL) < 0 ) 
	{
        printf("WARNING: Failed to initialize SDL audio. Bailing out.\n");
        exit(1);
    }
    SDL_PauseAudio(0);
}

void audio_cb(void *userdata, unsigned char *sdl_buffer, int howmuch)
{
	long ch, smp, qu, i;
	long ct = 0;
	long remaining = howmuch / BYTES_PER_SAMPLE;
	SAMPLE_TYPE *music_pos;
	long music_samples = howmuch * 1.1;
	SAMPLE_TYPE *feedme = (SAMPLE_TYPE*) sdl_buffer;
	int extend;
	int clip;
	
	music_buffer = malloc(BYTES_PER_SAMPLE * music_samples); /* SYN: A little extra because I don't trust the adplug code to be exact */
	music_pos = (SAMPLE_TYPE*) sdl_buffer;
	
	/* SYN: Simulate the fm synth chip */
	while(remaining > 0) 
	{
		while(ct < 0) 
		{
			ct += freq;
			lds_update(); /* SYN: Do I need to use the return value for anything here? */
		}
		
		/* set i to smaller of data requested by SDL and some stuff. Dunno. */
		i = ((long) ((ct / REFRESH) + 4) & ~3); /* SYN: I have no idea what those numbers mean. */
		i = (i > remaining) ? remaining : i; /* i should now equal the number of samples we get */
		opl_update((short*) music_pos, i);
		music_pos += (i * BYTES_PER_SAMPLE);
		remaining -= i;
		ct -= (long)(REFRESH * i); /* SYN: I have no idea what this calculation means... */
	}
	
	/* SYN: And now we mix in the audio. */
	/* Actually I'm dumping audio straight into the sdl buffer right now, so this isn't currently needed. */
	qu = howmuch;
	for (smp = 0; smp < qu; smp++) 
	{
		/* printf("> %d\n", music_buffer[smp]); */
		feedme[smp] = feedme[smp] * 0.75; /* + (music_buffer[smp] * 0.75);*/
	}
	
	/* Bail out, don't mix in sound at the moment. It's broked. */
	return;
	
	/* SYN: Mix sound channels and shove into audio buffer */
	for (ch = 0; ch < SFX_CHANNELS; ch++) 
	{
		/* SYN: Don't copy more data than is in the channel! */
		qu = ( (Uint32) howmuch > channel_len[ch] ? (int) channel_len[ch] : howmuch); /* How many bytes to copy */
		for (smp = 0; smp < (qu / BYTES_PER_SAMPLE); smp++)
		{
			clip = ((int) feedme[smp] + (int) (((SAMPLE_TYPE) channel_pos[ch][smp]) / VOLUME_SCALING ));
			feedme[smp] = (clip >= 128) ? 127 : (clip <= -128) ? -127 : clip;			
		}
		
		channel_pos[ch] += qu;
		channel_len[ch] -= qu;
		
		/* SYN: If we've emptied a channel buffer, let's free the memory and clear the channel. */
		if (channel_len == 0)
		{
			free(channel_buffer[ch]);
			channel_buffer[ch] = channel_pos[ch] = NULL;
		}
	}
}

void JE_deinitialize( void )
{
	/* SYN: TODO: Clean up any other audio stuff, if necessary. This should only be called when we're quitting. */
	SDL_CloseAudio();
}

void JE_play( void )
{
	/* SYN: This proc isn't necessary, because filling the buffer is handled in the SDL callback function.*/
}

/* SYN: selectSong is called with 0 to disable the current song. Calling it with 1 will start the current song if not playing,
   or restart it if it is. */
void JE_selectSong( JE_word value )
{
	/* TODO: Finish this function! */
	
	/* TODO: Stop currently playing song  */
	if (value != 0)
	{
		lds_load((JE_byte*) musicData); /* Load song */
		/* TODO: Start playing song */
	}
}

void JE_samplePlay(JE_word addlo, JE_word addhi, JE_word size, JE_word freq)
{
	/* SYN: I don't think this function is used. */
	STUB(JE_samplePlay);
}

void JE_bigSamplePlay(JE_word addlo, JE_word addhi, JE_word size, JE_word freq)
{
	/* SYN: I don't think this function is used. */
	STUB(JE_bigSamplePlay);
}

/* Call with 0x1-0x100 for music volume, and 0x10 to 0xf0 for sample volume. */
/* SYN: Either I'm misunderstanding Andreas's comments, or the information in them is inaccurate. */
void JE_setVol(JE_word volume, JE_word sample)
{
	STUB(JE_setVol);
}

JE_word JE_getVol( void )
{
	STUB(JE_getVol);
	return 0;
}

JE_word JE_getSampleVol( void )
{
	STUB(JE_getSampleVol);
	return 0;
}

void JE_multiSampleInit(JE_word addlo, JE_word addhi, JE_word dmalo, JE_word dmahi)
{
	/* SYN: I don't know if this function should do anything else. For now, it just checks to see if sound has
	   been initialized and, if not, calls the main initialize function. */
	
	if (!sound_init_state)
	{
		JE_initialize(0, 0, 0, 0, 0);
	}
}

void JE_multiSampleMix( void )
{
	/* SYN: This proc isn't necessary, because the mixing is handled in the SDL callback function.*/
}

void JE_multiSamplePlay(JE_byte *buffer, JE_word size, JE_byte chan, JE_byte vol)
{
	int i, ex;
	double v = 1;
	/* v = (vol - 0x100) / ((double) 0xe00); */ /* SYN: Convert Loudness vol to fraction) */
	
	if (channel_buffer[chan] != NULL)
	{
		/* SYN: Something is already playing on this channel, so remove it */
		free(channel_buffer[chan]);
		channel_buffer[chan] = channel_pos[chan] = NULL;
		channel_len[chan] = 0;
	}
	
	channel_len[chan] = size * SAMPLE_SCALING;
	channel_buffer[chan] = malloc(channel_len[chan]);
	channel_pos[chan] = channel_buffer[chan];
	
	for (i = 0; i < size; i++)
	{
		for (ex = 0; ex < SAMPLE_SCALING; ex++)
		{
			channel_buffer[chan][(i * SAMPLE_SCALING) + ex] = (SAMPLE_TYPE) buffer[i]; 
			/* Should adjust for volume here? */
		}
	}
}
