/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

extern "C"
{
	#include "ts3plugin.h"
	//#include "isStreamRecord.h"
	#include "obs-frontend-api\obs-frontend-api.h"
};
#include <sstream>
#include "overlayComs.hpp"
#include "slothSock.hpp"
#include "utility.hpp"
#include "originalFeatures.hpp"

using namespace std;

void runOverlay(atomic<bool> &close)
{
	int iname;
	bool bname;
	bool bright;
	bool btalker;
	int italker;
	vector<wstring> vname;
	vector<int> vtime;
	bool bsaid = false;

	bool allOK = false;
	
	slothSock sock;
	//char* adrs;

	bool iResult;
	char reci1[253]; //181
	char reci2[64];
	vector<char> reci3;
	int reci3Size;
	char reci4[256];
	//char reci5[256];
	char reciA[64];

	char* whoami = "whoami\n";
	char* schandlerlist = "serverconnectionhandlerlist\n";

	//string location
	string identstart;
	wstring widentstart;
	string identend;
	wstring widentend;
	wstring talkstart = L"client_flag_talking=";
	size_t startpos;
	size_t endpos;
	string tempstr;
	wstring wtempstr;

	//useful things
	string cid;
	string list;
	wstring wlist;
	const char *cllist;
	vector<wstring> name;
	int namesize;
	vector<bool> talk;
	bool discon = true;
	bool noserv = true;
	bool add = true;
	bool remove = false;
	bool bCom = false;
	wstring userName;
	bool bMnD = false;
	vector<string> schandlerid;

	psleep(1000);

	char* apikey = getAPIKey();
	//blog(LOG_WARNING, apikey);
	tempstr = "auth apikey=";
	tempstr.append(apikey);
	tempstr.append("\n");
	const char* auth = tempstr.c_str();

	wstringstream overlay;
	string overlayString;

	blog(LOG_INFO, "TS3: Overlay: Started");
	#if _WIN32
	while(!close)
	#else
	while(close)
	#endif
	{
		//connect
		//Only ned to connect once as long as everything is okay.
		blog(LOG_WARNING, "TS3: Starting Connection");
		char* adrs = getAddress();
		if(!sock.connectToHost(25639, adrs))
		{
			if(discon)
			{
				blog(LOG_WARNING, "TS3: Connection Failure: Check TS3 is running and ClientQuery Plugin is enabled");
				
				discon = false;
				//force Communicate, MuteandDeafen and ChannelSwitch
				bCom = false;
				bMnD = false;
				add = true;
			}
			overlay.str(L" ");	//empty client list
			allOK = false;
			goto skip;
		}
		//delete adrs;
		blog(LOG_INFO, "TS3: Connected");
		discon = true;
		iResult = sock.recvAll(reci1, 253, 0);	//get TS3 Client... 181
		if(!iResult)
		{
			blog(LOG_WARNING, "TS3: Overlay: First Recieve Failure");
			overlay.str(L" ");	//empty client list
			allOK = false;
			goto skip;
		}
		//authorise
		iResult = sock.sendAll(auth, (int)strlen(auth), 0);
		if(!iResult)
		{
			blog(LOG_WARNING, "TS3: Overlay: auth Send Failure");
			overlay.str(L" ");	//empty client list
			allOK = false;
			goto skip;
		}
		iResult = sock.recvAll(reciA, 64, 0, "msg=");
		if(!iResult)
		{
			blog(LOG_WARNING, "TS3: Overlay: auth Recieve Failure");
			overlay.str(L" ");	//empty client list
			allOK = false;
			goto skip;
		}

		tempstr = reciA;
		identstart = "id=";
		startpos = tempstr.find(identstart);
		if(startpos == -1)
		{
			if(noserv)	//if initial disconnection
			{
				blog(LOG_WARNING, "TS3: Overlay: Invalid API Key Return");
				noserv = false;
				bCom = false;
				bMnD = false;
				add = false;
			}
			memset(reciA, 0, 64);
			overlay.str(L" ");	//empty client list
			allOK = false;
			goto skip;
		}
		else if(tempstr.substr(startpos, 7) == "id=1538")
		{
			//blog(LOG_WARNING, "id=1538");
			if(noserv)	//if initial disconnection
			{
				blog(LOG_WARNING, "TS3: Overlay: Invalid API Key");
				noserv = false;
				bCom = false;
				bMnD = false;
				add = false;
			}
			memset(reciA, 0, 64);
			overlay.str(L" ");	//empty client list
			allOK = false;
			goto skip;
		}

		allOK = true;
		
		while(allOK)
		{
			if(obs_frontend_streaming_active() || obs_frontend_recording_active())
			{
				if(add)
				{
					iResult = sock.sendAll(schandlerlist, (int)strlen(schandlerlist), 0);	//send serverconnectionhandlerlist
					if(!iResult)
					{
						blog(LOG_WARNING, "TS3: Overlay: schandlerlist Send Failure");
						overlay.str(L" ");	//empty client list
						allOK = false;
						goto skip;
					}
					iResult = sock.recvAll(reci4, 256, 0, "msg=");	//get serverconnectionhandlerlist
					if (!iResult)
					{
						blog(LOG_WARNING, "TS3: Overlay: schandlerlist Recieve Failure");
						overlay.str(L" ");	//empty client list
						allOK = false;
						goto skip;
					}
					tempstr = reci4;
					memset(reci4, 0, 256);

					while(tempstr.size() > 12)
					{
						identstart = "sch";
						startpos = tempstr.find(identstart);
						if(startpos >= 0 && startpos < tempstr.size())
						{
							schandlerid.push_back(tempstr.substr(startpos, 13));
						}
						else
						{
							break;
						}
						tempstr = tempstr.substr(startpos + 13, tempstr.size() - (startpos + 13));
					}

					if(!bCom)
					{
						userName = Communicate(1, sock, schandlerid);
						if(userName != L"poop")
						{
							wReplaceAll(userName, L"\\s", L" ");
							bCom = true;
						}
						else
						{
							bCom = false;
						}
					}
					if(!bMnD)
					{
						bMnD = MuteandDeafen(1, sock, schandlerid);
					}
					if(bCom && bMnD)
					{
						bCom = false;
						bMnD = false;
						add = false;
						remove = true;
					}
					schandlerid.clear();
				}
			}
			else
			{
				if(remove)
				{
					iResult = sock.sendAll(schandlerlist, (int)strlen(schandlerlist), 0);	//send serverconnectionhandlerlist
					if(!iResult)
					{
						blog(LOG_WARNING, "TS3: Overlay: schandlerlist Send Failure");
						overlay.str(L" ");	//empty client list
						allOK = false;
						goto skip;
					}
					iResult = sock.recvAll(reci4, 256, 0, "msg=");	//get serverconnectionhandlerlist
					if (!iResult)
					{
						blog(LOG_WARNING, "TS3: Overlay: schandlerlist Recieve Failure");
						overlay.str(L" ");	//empty client list
						allOK = false;
						goto skip;
					}
					tempstr = reci4;
					memset(reci4, 0, 256);

					while(tempstr.size() > 12)
					{
						identstart = "sch";
						startpos = tempstr.find(identstart);
						if(startpos > 0 && startpos < tempstr.size())
						{
							schandlerid.push_back(tempstr.substr(startpos, 13));
						}
						else
						{
							break;
						}
						tempstr = tempstr.substr(startpos + 13, tempstr.size() - (startpos + 13));
					}

					if(!bCom)
					{
						userName = Communicate(0, sock, schandlerid);
						if(userName != L"poop")
						{
							wReplaceAll(userName, L"\\s", L" ");
							bCom = true;
						}
						else
						{
							bCom = false;
						}
					}
					if(!bMnD)
					{
						bMnD = MuteandDeafen(0, sock, schandlerid);
					}
					if(bCom && bMnD)
					{
						bCom = false;
						bMnD = false;
						add = true;
						remove = false;
					}
					schandlerid.clear();
				}
			}
			//get cid
			iResult = sock.sendAll(whoami, (int)strlen(whoami), 0);	//send whoami
			if (!iResult)
			{
				blog(LOG_WARNING, "TS3: Overlay: whoami Send Failure");
				overlay.str(L" ");	//empty client list
				allOK = false;
				goto skip;
			}
			//psleep(50);
			iResult = sock.recvAll(reci2, 64 ,0 ,"msg=");	//get whoami
			if (!iResult)
			{
				blog(LOG_WARNING, "TS3: Overlay: whoami Recieve Failure");
				overlay.str(L" ");	//empty client list
				allOK = false;
				goto skip;
			}
			tempstr = reci2;
			identstart = "msg=";
			startpos = tempstr.find(identstart);
			if(tempstr.substr(startpos, 5) == "msg=n")
			{
				if(noserv)	//if initial disconnection
				{
					blog(LOG_WARNING, "TS3: Overlay: Not Connected to TS3 Server");
					noserv = false;
					//force Communicate, MuteandDeafen and ChannelSwitch
					bCom = false;
					bMnD = false;
					add = false;
				}
				memset(reci2, 0, 64);
				overlay.str(L" ");	//empty client list
				allOK = false;
				goto skip;
			}
			tempstr = " ";

			if(!noserv)
			{
				blog(LOG_INFO, "TS3: Overlay: Now Connected to TS3 Server");
				add = true;
				noserv = true;
			}

			identstart = "cid=";
			identend = "\n";
			cid = reci2;
			startpos = cid.find(identstart);
			//blog(LOG_WARNING, reci2); //--------------------------------------------------------------------------
			//break;
			if(startpos == -1)
			{
				blog(LOG_WARNING, "TS3: Overlay: cid startpos == -1");
				overlay.str(L" ");	//empty client list
				allOK = false;
				//clear buffer;
				///iResult = sock.recvAll(reci5, 256, 0);
				goto skip;
			}
			endpos = cid.find(identend);
			if(endpos < 0)
			{
				blog(LOG_WARNING, "TS3: Overlay: cid endpos < 0");
				overlay.str(L" ");	//empty client list
				allOK = false;
				goto skip;
			}
			cid = cid.substr(startpos, endpos - startpos);

			//set up channelclientlist
			tempstr = "channelclientlist ";
			tempstr.append(cid);
			tempstr.append(" -voice\n");
			cllist = tempstr.c_str();
		
			//recieve setup
			iname = getNumberOfNames();
			reci3Size = iname * 360;
			reci3.resize(reci3Size);

			//get channelclientlist
			iResult = sock.sendAll(cllist, (int)strlen(cllist), 0);	//send channelcli...
			if (!iResult)
			{
				blog(LOG_WARNING, "TS3: Overlay: channelclientlist Send Failure");
				overlay.str(L" ");	//empty client list
				allOK = false;
				goto skip;
			}
			iResult = sock.recvAll(&reci3[0], reci3.size(), 0, "msg=");					//recieve channelcli...
			if (!iResult)
			{
				blog(LOG_WARNING, "TS3: Overlay: channelclientlist Recieve Failure");
				overlay.str(L" ");	//empty client list
				allOK = false;
				goto skip;
			}

			widentstart = L"name=";
			widentend = L"client_type";
			list.assign(reci3.begin(), reci3.end());
			//blog(LOG_WARNING, list.c_str()); //----------------------------------------------------------------------
			wlist = s2ws(list);

			//generate client list
			for(int i = 0; i < iname; i++)
			{
				startpos = wlist.find(widentstart);
				if(startpos == -1)
				{
					break;
				}
				endpos = wlist.find(widentend);
				if(endpos < 0)
				{
					break;
				}

				wtempstr = wlist.substr(startpos + 5, endpos-startpos-6);
				name.push_back(wtempstr);

				startpos = wlist.find(talkstart);
				if(startpos == -1)
				{
					talk.push_back(false);
					break;
				}
				else if(wlist.substr(startpos + 20, 1) == L"0")
				{
					talk.push_back(false);
				}
				else if(wlist.substr(startpos + 20, 1) == L"1")
				{
					talk.push_back(true);
				}
				wlist = wlist.substr(startpos+25);
			}

			//only show talker time keeping
			btalker = getOnlyShowTalker();
			if(btalker)
			{
				italker = getHideNameAfter()/100;
				if(static_cast<int>(vtime.size()) > 0)
				{
					for(int i = static_cast<int>(vtime.size()) - 1; i > -1; i--)
					{
						if(vtime[i] > italker)
						{
							vtime.erase(vtime.begin() + i);
							vname.erase(vname.begin() + i);
						}
						else
						{
							vtime[i]++;
						}
					}
				}
			}

			//print client list
			bname = getHideSelf();
			bright = getRightOfSymbol();

			namesize = name.size();
			if(namesize < 1)
			{
				overlay.str(L" ");	//empty client list
				allOK = false;
				goto skip;
			}
			else if(namesize < iname)
			{
				iname = namesize;
			}

			overlay.str(L" ");	//empty client list

			//normal
			if(!btalker)
			{
				for(int i = 0; i < iname; i++)
				{
					if(name[i].empty())
					{
						break;
					}
					wReplaceAll(name[i], L"\\s", L" ");
					if(bname && name[i] == userName)
					{
						//do nothing
					}
					else
					{
						wstring wsTmp(name[i].begin(), name[i].end());
						if(!bright)
						{
							if(talk[i])
							{
								overlay << L"\u25CF" << wsTmp << L"\n";
							}
							else
							{
								overlay << L"\u25CB" << wsTmp << L"\n";
							}
						}
						else
						{
							if(talk[i])
							{
								overlay << wsTmp << L"\u25CF" << L"\n";
							}
							else
							{
								overlay << wsTmp << L"\u25CB" << L"\n";
							}
						}
					}
				}//end for(int i...
			}
			//Only show talkers
			else
			{
				for(int i = 0; i < iname; i++)
				{
					if(name[i].empty())
					{
						break;
					}
					wReplaceAll(name[i], L"\\s", L" ");
					if(bname && name[i] == userName)
					{
						//do nothing
					}
					else
					{
						wstring wsTmp(name[i].begin(), name[i].end());
						if(talk[i])
						{
							bsaid = false;
							if(static_cast<int>(vname.size()) > 0)
							{
								for(int j = 0; j < static_cast<int>(vname.size()); j++)
								{
									if(wsTmp == vname[j])
									{
										vtime[j] = 0;
										bsaid = true;
										break;
									}
								}
							}
							if(!bsaid)
							{
								vname.push_back(wsTmp);
								vtime.push_back(0);
							}
							else
							{
								bsaid = false;
							}
						}//end if(talk[i])
					}
				}//end for(int i...

				//Print
				if(static_cast<int>(vname.size()) > 0)
				{
					for(int j = 0; j < static_cast<int>(vname.size()); j++)
					{
						overlay << vname[j] << L"\n";
					}
				}
			}//end Only Show talkers
			//reset variables
			memset(reci1, 0, 253); //181
			memset(reci2, 0, 64);
			memset(reciA, 0, 64);
			reci3.clear();
			cid.clear();
			list.clear();
			name.clear();
			talk.clear();
			identstart.clear();
			identend.clear();
			startpos = 0;
			endpos = 0;
			tempstr.clear();

		skip:

			if(overlay.str() == L" ")
			{
				overlayString = " ";
			}
			else
			{
				overlayString = ws2s(overlay.str());
			}
			sendOverlay(overlayString.c_str());
			psleep(100);
		}
		blog(LOG_INFO, "TS3: Closing Connection");
		sock.closeConnection();
		psleep(1000);
	}//end while(!close)

//shutdown:
	sock.closeConnection();
	delete whoami;
	delete schandlerlist;
	blog(LOG_INFO, "TS3: Overlay: Stopped");	
	return;

}
