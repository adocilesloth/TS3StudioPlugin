/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

#ifndef TS3PLUGIN_H
#define TS3PLUGIN_H

#include <obs-module.h>

const char* getAbsolutePath();

bool getNeedToRun();

char* getAddress();
int getNumberOfNames();
bool getHideSelf();
bool getRightOfSymbol();
bool getOnlyShowTalker();
int getHideNameAfter();

void sendOverlay(const char* names);

char* getCluid();
char* getAPIKey();
char* getPrefix();
bool getSuffix();
bool getAllServers();
int getMuteAndDeafen();

#endif
