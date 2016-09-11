/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

#include <obs-internal.h>
#include "ts3plugin.h"
#include "ts3plugin_settings.h"

char* adrs;
int iname;
bool bname;
bool bright;
bool btalker;
int italker;

char* cluid;
char* prefix;
bool bsuffix;
bool ball;
int iMnD = 0;

char* nameList;

bool needToRun = false;

static const char *ts3_plugin_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("TeamSpeak 3 Overlay");
}

static void *ts3_plugin_create(obs_data_t *settings, obs_source_t *source)
{
	struct ts3_overlay *overlay = bzalloc(sizeof(struct ts3_overlay));
	overlay->source = source;

	const char *text_source_id = "text_ft2_source\0";
	overlay->textSource = obs_source_create(text_source_id, text_source_id, settings, NULL);

	obs_source_add_active_child(overlay->source, overlay->textSource);

	needToRun = true;
	
	return overlay;
}

static void ts3_plugin_destroy(void *data)
{
	needToRun = false;

	struct ts3_overlay *overlay = data;

	obs_source_remove(overlay->textSource);
	obs_source_release(overlay->textSource);
	overlay->textSource = NULL;
	
	bfree(overlay);
}

static void ts3_plugin_update(void *data, obs_data_t *settings)
{
	struct ts3_overlay *overlay = data;
	obs_source_update(overlay->textSource, settings);

	obs_data_set_bool(overlay->textSource, "from_file", false);
}

static uint32_t ts3_plugin_get_width(void *data)
{
	struct ts3_overlay *overlay = data;
	return obs_source_get_width(overlay->textSource);
}

static uint32_t ts3_plugin_get_height(void *data)
{
	struct ts3_overlay *overlay = data;
	return obs_source_get_height(overlay->textSource);
}

static void ts3_plugin_render(void *data, gs_effect_t *effect)
{
	struct ts3_overlay *overlay = data;

	obs_source_video_render(overlay->textSource);

	adrs = obs_data_get_string(overlay->textSource, "ts3ip");
	iname = obs_data_get_int(overlay->textSource, "num_name");
	bname = obs_data_get_bool(overlay->textSource, "no_self");
	bright = obs_data_get_bool(overlay->textSource, "right_of_name");
	italker = obs_data_get_int(overlay->textSource, "hide_after");
	btalker = obs_data_get_bool(overlay->textSource, "speaking_only");

	cluid = obs_data_get_string(overlay->textSource, "cluid");
	prefix = obs_data_get_string(overlay->textSource, "prefix");
	bsuffix = obs_data_get_bool(overlay->textSource, "suffix");
	ball = obs_data_get_bool(overlay->textSource, "all_servers");

	bool bM = obs_data_get_bool(overlay->textSource, "mute");
	bool bD = obs_data_get_bool(overlay->textSource, "deafen");
	if(bM && !bD)
	{
		iMnD = 1;
	}
	else if(!bM && bD)
	{
		iMnD = 2;
	}
	else if(bM && bD)
	{
		iMnD = 3;
	}
	else
	{
		iMnD = 0;
	}
}

static void ts3_plugin_video_tick(void *data, float seconds)
{
	struct ts3_overlay *overlay = data;

	if (!obs_source_showing(overlay->source))
	{
		return;
	}

	obs_data_set_string(overlay->textSource->context.settings, "text", nameList);
	obs_source_update(overlay->textSource, overlay->textSource->context.settings);
}

static obs_properties_t *ts3_plugin_properties(void *data)
{
	struct ts3_overlay *overlay = data;

	obs_properties_t *props = obs_source_properties(overlay->textSource);

	//Needed TS3 Stuff
	obs_properties_add_text(props, "ts3ip",
		obs_module_text("IPV4"), OBS_TEXT_DEFAULT);
	obs_data_set_default_string(overlay->textSource, "ts3ip", "127.0.0.1");

	obs_properties_add_text(props, "cluid",
		obs_module_text("UniqueID"), OBS_TEXT_PASSWORD);
	//TS3 Overlay
	obs_properties_add_int(props, "num_name",
		obs_module_text("NamesToShow"), 0, 100, 1);
	obs_data_set_default_int(overlay->textSource, "num_name", 10);

	obs_properties_add_bool(props, "speaking_only",
		obs_module_text("OnlyShowSpeakers"));

	obs_properties_add_int(props, "hide_after",
		obs_module_text("HideAfter"), 0, 2000, 1);

	obs_properties_add_bool(props, "no_self",
		obs_module_text("HideOwnName"));

	obs_properties_add_bool(props, "right_of_name",
		obs_module_text("SymbolRightOfNames"));
	//TS3 Name Changing
	obs_properties_add_text(props, "prefix",
		obs_module_text("Modifier"), OBS_TEXT_DEFAULT);
	obs_data_set_default_string(overlay->textSource, "prefix", "*R*");

	obs_properties_add_bool(props, "suffix",
		obs_module_text("ModifierAsSuffix"));

	obs_properties_add_bool(props, "mute",
		obs_module_text("MuteTS3"));

	obs_properties_add_bool(props, "deafen",
		obs_module_text("DeafenTS3"));

	obs_properties_add_bool(props, "all_servers",
		obs_module_text("ChangeAllServers"));

	return props;
}

void enum_active_sources(void *data, obs_source_enum_proc_t enum_callback, void *param) {
	struct ts3_overlay *context = data;

	enum_callback(context->source, context->textSource, param);
}

struct obs_source_info getInfo()
{
	struct obs_source_info ts3_plugin_info = {
		.id = "ts3plugin",
		.type = OBS_SOURCE_TYPE_INPUT,
		.output_flags = OBS_SOURCE_VIDEO,
		.get_name = ts3_plugin_get_name,
		.create = ts3_plugin_create,
		.destroy = ts3_plugin_destroy,
		.update = ts3_plugin_update,
		.get_width = ts3_plugin_get_width,
		.get_height = ts3_plugin_get_height,
		.video_render = ts3_plugin_render,
		.video_tick = ts3_plugin_video_tick,
		.get_properties = ts3_plugin_properties,
		.enum_active_sources = enum_active_sources
	};
	return ts3_plugin_info;
}

bool getNeedToRun()
{
	return needToRun;
}

char* getAddress()
{
	return adrs;
}

int getNumberOfNames()
{
	return iname;
}

bool getHideSelf()
{
	return bname;
}

bool getRightOfSymbol()
{
	return bright;
}

bool getOnlyShowTalker()
{
	return btalker;
}

int getHideNameAfter()
{
	return italker;
}

void sendOverlay(const char* names)
{
	nameList = names;
	return;
}

char* getCluid()
{
	return cluid;
}

char* getPrefix()
{
	return prefix;
}

bool getSuffix()
{
	return bsuffix;
}

bool getAllServers()
{
	return ball;
}

int getMuteAndDeafen()
{
	return iMnD;
}