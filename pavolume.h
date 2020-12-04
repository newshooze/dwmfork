#ifndef PAVOLUME_H
#define PAVOLUME_H

#include <pulse/pulseaudio.h>

#define UNUSED __attribute__((unused))

struct pulseaudio_t 
{
	pa_context *cxt;
	pa_mainloop *mainloop;
	pa_cvolume cvolume;
	pa_volume_t volume; // average
	int muted; // 0 or 1
	char *default_sink;
};

extern struct pulseaudio_t pulseaudioobject;

void pulse_server_info_callback(pa_context UNUSED *c, const pa_server_info *i, void *data);
void pulse_connect_state_callback(pa_context *cxt, void *data);
void pulse_async_wait(struct pulseaudio_t *pulse, pa_operation *op);
void pulse_get_default_sink_volume_callback(pa_context UNUSED *c, const pa_sink_info *i, int eol, void *data);
void pulse_get_default_sink_volume(struct pulseaudio_t *pulse);
float pulse_get_default_sink_volume_percent(struct pulseaudio_t *pulse);
void pulse_set_default_sink_volume(struct pulseaudio_t *pulse, pa_volume_t volume);
void pulse_set_default_sink_volume_percent(struct pulseaudio_t *pulse,float percent);
void pulse_change_default_sink_volume_by_percent(struct pulseaudio_t *pulse,float percent);
void pulse_set_default_sink_mute(struct pulseaudio_t *pulse, int muted);
void pulse_toggle_default_sink_mute(struct pulseaudio_t *pulse);
void pulse_get_default_sink(struct pulseaudio_t *pulse);
int pulse_init(struct pulseaudio_t *pulse);
void pulse_deinit(struct pulseaudio_t *pulse);

void pulseinit();
void pulsedeinit();
void pulsechangevolumebypercent(int n);
void pulseincreasevolumepercent(uint32_t n);
void pulsedecreasevolumepercent(uint32_t n);
void pulsesetvolumebypercent(uint32_t n);
/* pulsegetmute and pulsemuted are the same */
int pulsegetmute();
int pulsemuted();
void pulsemute();
void pulseunmute();
int pulsegetvolumepercent();
#endif // PAVOLUME_H
