/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

extern "C"
{
	#include "ts3plugin.h"
	#include "ts3plugin_settings.h"
};
#include "overlayComs.hpp"
#include "utility.hpp"

#include <thread>

std::thread overlayThread;
std::thread listenThread;
std::atomic<bool> close(false);
std::atomic<bool> stopListening(false);

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("TS3Plugin", "en-US")

/*extern "C" static struct obs_source_info ts3_plugin_info = {
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
};*/

static struct obs_source_info ts3_plugin_info = getInfo();

void listen(void)
{
	bool go = false;
	bool stop = false;
	bool toggle = false;

	while(!stopListening)
	{
		if(!toggle)
		{
			go = getNeedToRun();
			if(go)
			{
				overlayThread = std::thread(runOverlay, std::ref(close));
				go = false;
				toggle = true;
			}
		}
		else
		{
			stop = !getNeedToRun();
			if(stop)
			{
				close = true;
				stop = false;
				toggle = false;
			}
		}
		psleep(100);
	}
	return;
}

bool obs_module_load(void)
{
	obs_register_source(&ts3_plugin_info);
	listenThread = std::thread(listen);
	return true;
}

void obs_module_unload(void)
{
	stopListening = true;
	close = true;
	return;
}

const char *obs_module_author(void)
{
	return "A Docile Sloth";
}

const char *obs_module_name(void)
{
	return "TeamSpeak 3 Plugin";
}

const char *obs_module_description(void)
{
	return "Adds an overlay for TeamSpeak 3";
}