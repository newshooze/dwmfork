#include <string.h>
#include <stdio.h>

#include "pavolume.h"

//#define PAV_USEASLIBRARY /* set inside makefile */

struct pulseaudio_t pulseaudioobject = {0};

//static int pulseinitialized = 0;

void pulse_server_info_callback(pa_context UNUSED *c, const pa_server_info *i, void *data) 
{
	struct pulseaudio_t *pulse = (struct pulseaudio_t *)data;
	pulse->default_sink = (char*)malloc(strlen(i->default_sink_name)+1);
	strcpy(pulse->default_sink, i->default_sink_name);
}

void pulse_connect_state_callback(pa_context *cxt, void *data) 
{
	enum pa_context_state *state = (enum pa_context_state *)data;
	*state = pa_context_get_state(cxt);
}

void pulse_async_wait(struct pulseaudio_t *pulse, pa_operation *op) 
{
	while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
		pa_mainloop_iterate(pulse->mainloop, 1, NULL);
}

void pulse_get_default_sink_volume_callback(pa_context UNUSED *c, const pa_sink_info *i, int eol, void *data) 
{
	if (eol)
		return;
	struct pulseaudio_t *pulse = (struct pulseaudio_t*)data;
	pulse->muted = i->mute;
	pulse->cvolume = i->volume;
	pulse->volume = pa_cvolume_avg(&(i->volume));
}

void pulse_get_default_sink_volume(struct pulseaudio_t *pulse) 
{
	pa_operation *op;
	op = pa_context_get_sink_info_by_name(pulse->cxt, pulse->default_sink, pulse_get_default_sink_volume_callback, pulse);
	pulse_async_wait(pulse, op);
	pa_operation_unref(op);
}

void pulse_set_default_sink_volume(struct pulseaudio_t *pulse, pa_volume_t volume) 
{
	pa_operation *op;
	pa_cvolume_set(&(pulse->cvolume), 2, volume);
	op = pa_context_set_sink_volume_by_name(pulse->cxt, pulse->default_sink, &(pulse->cvolume), NULL, pulse);
	pulse_async_wait(pulse, op);
	pa_operation_unref(op);
}

float pulse_get_default_sink_volume_percent(struct pulseaudio_t *pulse)
{
	float ret;
	pulse_get_default_sink_volume(pulse);
	ret = pulse->volume * 100.0 / PA_VOLUME_NORM;
	return ret;	
}
void pulse_set_default_sink_volume_percent(struct pulseaudio_t *pulse,float percent)
{
	pa_volume_t newvolume = PA_VOLUME_NORM * percent / 100;
	pa_volume_t maxv = PA_VOLUME_NORM;
	if(newvolume < 0)
		newvolume = 0;
	if(newvolume > maxv)
		newvolume = maxv;
	pulse_set_default_sink_volume(pulse,newvolume);
}
void pulse_change_default_sink_volume_by_percent(struct pulseaudio_t *pulse,float percent)
{
	float currentpercent = pulse_get_default_sink_volume_percent(pulse);
	float newpercent = currentpercent + percent;
	if(newpercent > 100)
		newpercent = 100;
	if(newpercent < 0)
		newpercent = 0;	
	pulse_set_default_sink_volume_percent(pulse,newpercent);
}

void pulse_set_default_sink_mute(struct pulseaudio_t *pulse, int muted) 
{
	pa_operation *op;
	pulse->muted = muted;
	op = pa_context_set_sink_mute_by_name(pulse->cxt, pulse->default_sink, pulse->muted, NULL, pulse);
	pulse_async_wait(pulse, op);
	pa_operation_unref(op);
}

int pulse_get_mute(struct pulseaudio_t *pulse)
{
	return pulse->muted;
}
void pulse_toggle_default_sink_mute(struct pulseaudio_t *pulse)
{
	pulse_set_default_sink_mute(pulse,!pulse->muted);
}

void pulse_get_default_sink(struct pulseaudio_t *pulse) 
{
	pa_operation *op;
	free(pulse->default_sink);
	op = pa_context_get_server_info(pulse->cxt, pulse_server_info_callback, pulse);
	pulse_async_wait(pulse, op);
	pa_operation_unref(op);
}

int pulse_init(struct pulseaudio_t *pulse) 
{
	enum pa_context_state state = PA_CONTEXT_CONNECTING;
	pulse->mainloop = pa_mainloop_new();
	pulse->cxt = pa_context_new(pa_mainloop_get_api(pulse->mainloop), "pavolume");
	pulse->default_sink = NULL;
	pulse->muted = 0;
	pulse->volume = 0;
	pa_context_set_state_callback(pulse->cxt, pulse_connect_state_callback, &state);
	pa_context_connect(pulse->cxt, NULL, PA_CONTEXT_NOFLAGS, NULL);
	while (state != PA_CONTEXT_READY && state != PA_CONTEXT_FAILED)
		pa_mainloop_iterate(pulse->mainloop, 1, NULL);

	if (state != PA_CONTEXT_READY) {
		fprintf(stderr, "Connection to pulseaudio failed: %s\n",
				pa_strerror(pa_context_errno(pulse->cxt)));
		return 1;
	}
	pulse_get_default_sink(pulse);
	pulse_get_default_sink_volume(pulse);

	return 0;
}

void pulse_deinit(struct pulseaudio_t *pulse) 
{
	enum pa_context_state state = PA_CONTEXT_CONNECTING;
	pa_context_set_state_callback(pulse->cxt, pulse_connect_state_callback, &state);
	pa_context_disconnect(pulse->cxt);
	pa_mainloop_free(pulse->mainloop);
	free(pulse->default_sink);
}
#ifdef PAV_USE_AS_LIBRARY 
void pulseinit()
{
	pulse_init(&pulseaudioobject);
}
void pulsedeinit()
{
	pulse_deinit(&pulseaudioobject);
}
void pulsesetvolumebypercent(uint32_t n)
{
	pulse_set_default_sink_volume_percent(&pulseaudioobject,n);
}
void pulsechangevolumebypercent(int n)
{
	pulse_change_default_sink_volume_by_percent(&pulseaudioobject,n);
}
void pulseincreasevolumepercent(uint32_t n)
{
	pulse_change_default_sink_volume_by_percent(&pulseaudioobject,n);	
}
void pulsedecreasevolumepercent(uint32_t n)
{
	pulse_change_default_sink_volume_by_percent(&pulseaudioobject,-n);
}
int pulsegetmute()
{
	return pulse_get_mute(&pulseaudioobject);	
}
int pulsemuted()
{
	return pulse_get_mute(&pulseaudioobject);
}
void pulsemute()
{
	pulse_set_default_sink_mute(&pulseaudioobject,1);
}
void pulseunmute()
{
	pulse_set_default_sink_mute(&pulseaudioobject,0);
}
int pulsegetvolumepercent()
{
	float f = pulse_get_default_sink_volume_percent(&pulseaudioobject);
	return (int)f;
}

#endif

#ifndef PAV_USE_AS_LIBRARY
void printusage()
{
	printf("usage: pavolume [command] [args]\n");	
	printf("      -h,--help           Print this help\n");
	printf("      -i,--increase [n]   Increase volume by n (0-100)\n");      
	printf("      -d,--decrease [n]   Decrease volume by n (0-100)\n");
	printf("      -m,--mute           Mute audio\n");
	printf("      -u,--unmute         Unmute audio\n");
	printf("      -t,--toggle-mute    Toggle mute\n");
	printf("      -g,--get-volume     Print current volume (0-100)\n"); 
	printf("      -s,--set-volume [n] Set current volume to n (0-100)\n"); 
}
int main(int argc, char * argv[]) 
{
	pulse_init(&pulseaudioobject);
	double pulsevolume = (double)pulseaudioobject.volume;
	double volumepercent = (double)pulseaudioobject.volume * 100 / PA_VOLUME_NORM;
	printf("PA_VOLUME_MAX: %d\n",PA_VOLUME_MAX);
	printf("PA_VOLUME_MUTED: %d\n",PA_VOLUME_MUTED);
	printf("PA_VOLUME_NORM: %d\n",PA_VOLUME_NORM);
	printf("Default sink: %s\n",pulseaudioobject.default_sink);
	printf("Current volume for default sink: %f\n",pulsevolume);
	printf("Current volume %% for default sink: %f\n",volumepercent);
	//pulse_toggle_default_sink_mute(pulseaudioobject);
	pulse_change_default_sink_volume_by_percent(&pulseaudioobject,1);
	int newvolume = pulse_get_default_sink_volume_percent(&pulseaudioobject);
	printf("newvolume: %d%%\n",newvolume);
	//pulse_set_default_sink_volume_percent(pulseaudioobject,24);
	pulse_deinit(&pulseaudioobject);
	return 0;
}
#endif
