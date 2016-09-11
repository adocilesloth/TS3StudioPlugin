#ifndef TS3PLUGIN_SETTINGS_H
#define TS3PLUGIN_SETTINGS_H

struct ts3_overlay
{
	obs_source_t *source;
	obs_source_t *textSource;
};

static const char *ts3_plugin_get_name(void *unused);
static void *ts3_plugin_create(obs_data_t *settings, obs_source_t *source);
static void ts3_plugin_destroy(void *data);
static void ts3_plugin_update(void *data, obs_data_t *settings);
static uint32_t ts3_plugin_get_width(void *data);
static uint32_t ts3_plugin_get_height(void *data);
static void ts3_plugin_render(void *data, gs_effect_t *effect);
static void ts3_plugin_video_tick(void *data, float seconds);
static obs_properties_t *ts3_plugin_properties(void *data);
struct obs_source_info getInfo();

#endif