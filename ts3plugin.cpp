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
#include <atomic>
#include <sstream>

std::thread overlayThread;
std::thread listenThread;
//#if _WIN32
	std::atomic<bool> close(false);
//#else
//	bool close = false;
//#endif
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
				#if _WIN32
					close = false;
				#endif
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
				#if _WIN32
					close = true;
					overlayThread.join();
				#endif
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
	blog(LOG_WARNING, "TS3: Load");
	return true;
}

void obs_module_unload(void)
{
	stopListening = true;
	#if _WIN32
		close = true;
	#endif
	return;
}

const char *obs_module_author(void)
{
	blog(LOG_WARNING, "TS3: Author");
	return "A Docile Sloth";
}

const char *obs_module_name(void)
{
	blog(LOG_WARNING, "TS3: Name");
	return "TeamSpeak 3 Plugin";
}

const char *obs_module_description(void)
{
	blog(LOG_WARNING, "TS3: Description");
	return "Adds an overlay for TeamSpeak 3";
}
