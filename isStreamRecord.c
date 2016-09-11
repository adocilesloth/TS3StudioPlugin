/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

#include "isStreamRecord.h"

bool isStreaming(void)
{
	if(obs_output_active(obs_get_output_by_name("simple_stream")))
	{
		return true;
	}
	else if(obs_output_active(obs_get_output_by_name("adv_stream")))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isRecording(void)
{
	if(obs_output_active(obs_get_output_by_name("simple_file_output")))
	{
		return true;
	}
	else if(obs_output_active(obs_get_output_by_name("adv_file_output")))
	{
		return true;
	}
	else if(obs_output_active(obs_get_output_by_name("adv_ffmpeg_output")))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isStreamingOrRecording(void)
{
	if(isStreaming() || isRecording())
	{
		return true;
	}
	else
	{
		return false;
	}
}