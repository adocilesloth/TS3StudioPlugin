/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

extern "C"
{
	#include "ts3plugin.h" 
};
#include <sstream>
#include <vector>
#include "slothSock.hpp"
#include "utility.hpp"

std::wstring Communicate(int cont, slothSock &obs, std::vector<std::string> &schandlerid)
{
	blog(LOG_INFO, "Communicate");

	std::wstring poop = L"poop";	//failure return

	bool iResult;
	//char *notify = "clientnotifyregister schandlerid=1 event=notifyclientnamefromuid\n";
	const char *notify;
	std::string snotify;
	std::wstringstream wnewname;
	char reci2[256];
	char reci3[256];
	char reci4[256];
	std::string space = "\\s";
	std::wstring wspace = L"\\s";

	std::wstring temp;
	bool multiChange;
	std::string sendschandlerid;

	std::string tmp;
	const char* recname;

	//get cluid and recording prefix
	std::wstring cluid = s2ws(getCluid());
	std::wstring rec = s2ws(getPrefix());
	if(rec.length() > 10)
	{
		rec = rec.substr(0, 10);
	}
	wReplaceAll(rec, L" ", wspace);		//replace spaces with \s
	int modcount = wcountSubstring(rec, wspace);	//number of \s
	multiChange = getAllServers();

	//set up the getname call
	std::wstring wtempgetname = L"clientgetnamefromuid cluid=";
	wtempgetname.append(cluid);
	wtempgetname.append(L"\n");
	std::string tempgetname(wtempgetname.begin(), wtempgetname.end());
	const char *getname = tempgetname.c_str();

	//return string
	std::wstring rname;

	int numServer;
	if(multiChange)
	{
		numServer = schandlerid.size();
	}
	else
	{
		numServer = 1;
	}

	for(int i = 0; i < numServer; i++)
	{
		wnewname << L"clientupdate client_nickname=";
		snotify = "clientnotifyregister " + schandlerid[i] + " event=notifyclientnamefromuid\n";
		notify = snotify.c_str();
		sendschandlerid = "use " + schandlerid[i] + "\n";

		//set server to notify on
		iResult = obs.sendAll(sendschandlerid.c_str(), sendschandlerid.size(), 0);	//use schandlerid=i
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: useschandlerid=i Send Failure");
			return poop;
		}
		iResult = obs.recvAll(reci2, 256, 0, "msg=");	//recieve result: error id=0...
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: useschandlerid=i Recieve Failure");
			return poop;
		}
		memset(reci2, 0, 256);

		//notifyregister
		iResult = obs.sendAll(notify, (int)strlen(notify), 0);	//request notifyregister...
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: notifyregister Send Failure");
			return poop;
		}
		iResult = obs.recvAll(reci2, 256, 0, "msg=");	//recieve result: error id=0...
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: notifyregister Recieve Failure");
			return poop;
		}

		//clientnamefromuid
		iResult = obs.sendAll(getname, (int)strlen(getname), 0);	//request clientnamefromuid...
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: clientnamefromuid Send Failure");
			return poop;
		}
		iResult = obs.recvAll(reci3, 256, 0, "msg=");	//recieve name
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: clientnamefromuid Recieve Failure");
			return poop;
		}

		//get name
		std::wstring identstart = L"name=";
		std::wstring identend = L"\n";
		std::string name = reci3;
		std::wstring wname = s2ws(name);

		size_t startpos = wname.find(identstart);	//start of name
		if(startpos == -1)
		{
			blog(LOG_WARNING, "Communicate: startpos == -1");
			blog(LOG_WARNING, name.c_str());
			//goto endofif;
			return poop;
		}
		size_t endpos = wname.find(identend);
		if(endpos < startpos)
		{
			blog(LOG_WARNING, "Communicate: endpos < startpos");
			blog(LOG_WARNING, name.c_str());
			//goto endofif;
			return poop;
		}
		wname = wname.substr(startpos+5, endpos-startpos-5);
		int count = wcountSubstring(wname, wspace);	//number of \s
		//wname = wname.substr(startpos+5 , 30 + count);
		//get name end

		bool bprefix = getSuffix();
		if(bprefix)	//if using suffix
		{
			size_t spc = wname.find(L"\n");
			wname = wname.substr(0, spc);
			int nstrt = wname.length();
			nstrt = nstrt - rec.length();
			if(nstrt < 0)
			{
			nstrt = 0;
			}
			int nlen = wname.length() - count;
	
			if(cont == 1)			//adding modifier
			{
				if(wname.substr(nstrt) != rec)
				{
					if(nlen > 30)
					{
						wname = wname.substr(0, 30 + count - rec.length());
					}
					wnewname << wname
							 << rec << L"\n";		//finish name set string
				}
			}
			else if(cont == 0)		//removing modifier
			{
				if(wname.substr(nstrt) == rec)
				{
					wname = wname.substr(0, nlen + count - rec.length());
				}
				wnewname << wname << L"\n";		//finish name set string
			}

			rname = wname;
			if(!rec.empty())
			{
				rname.append(rec);
			}
		}
		else	//if using prefix
		{
			if(cont == 1)			//adding modifier
			{
				if(wname.substr(0, rec.length()) != rec)
				{
					wname = wname.substr(0, 30 + count - rec.length());
					wnewname << rec;	//finish name set string
				}
			}
			else if(cont == 0)		//removing modifier
			{
				if(wname.substr(0, rec.length()) == rec)
				{
					wname = wname.substr(rec.length(), 30 + count - modcount - rec.length());
				}
			}
			wnewname << wname << L"\n";			//finish name set string

			if(!rec.empty())
			{
				rname = rec;
				rname.append(wname);
			}
			else
			{
				rname = wname;
			}
		}

		//no need to send new name if name is not updated
		if(rec.empty())
		{
			blog(LOG_WARNING, "Communicate: rec is empty");
			//goto endofif;
			return poop;
		}

		//AppWarning(wnewname.str().c_str());		//print name being sent

		tmp = ws2s(wnewname.str());	//set name to string
		recname = tmp.c_str();	//set name to char* so it can be sent

		//clientupdate
		iResult = obs.sendAll(recname, (int)strlen(recname), 0);
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: clientupdate Send Failure");
			return poop;
		}
		iResult = obs.recvAll(reci4, 256 ,0, "msg=");
		if (!iResult)
		{
			blog(LOG_WARNING, "Communicate: clientupdate Recieve Failure");
			return poop;
		}

//endofif:
		wnewname.str(L"");
		memset(reci2, 0, 256);
		memset(reci3, 0, 256);
		memset(reci4, 0, 256);
	}

	//return to default server
	sendschandlerid = "use " + schandlerid[0] + "\n";
	iResult = obs.sendAll(sendschandlerid.c_str(), sendschandlerid.size(), 0);	//use schandlerid=i
	if (!iResult)
	{
		blog(LOG_WARNING, "Communicate: useschandlerid=0 Send Failure");
		return poop;
	}
	iResult = obs.recvAll(reci2, 256, 0, "msg=");	//recieve result: error id=0...
	if (!iResult)
	{
		blog(LOG_WARNING, "Communicate: useschandlerid=0r Recieve Failure");
		return poop;
	}

	return rname;
}

bool MuteandDeafen(int state, slothSock &obs, std::vector<std::string> &schandlerid)
{
	blog(LOG_INFO, "MuteandDeafen");

	int mnd = getMuteAndDeafen();
	if(mnd != 1 && mnd != 2 && mnd != 3)	//if not set to mute or deafen
	{
		return true;
	}

	bool iResult;
	std::stringstream sstate;
	sstate << state << "\n";

	bool multiChange = getAllServers();

	int numServer;
	if(multiChange)
	{
		numServer = schandlerid.size();
	}
	else
	{
		numServer = 1;
	}

	char reci3[256];
	std::string sendschandlerid;

	for(int i = 0; i < numServer; i++)
	{
		sendschandlerid = "use " + schandlerid[i] + "\n";

		//set server to notify on
		iResult = obs.sendAll(sendschandlerid.c_str(), sendschandlerid.size(), 0);	//use schandlerid=i
		if (!iResult)
		{
			blog(LOG_WARNING, "MuteandDeafen: useschandlerid=i Send Failure");
			return false;
		}
		iResult = obs.recvAll(reci3, 256, 0, "msg=");	//recieve result: error id=0...
		if (!iResult)
		{
			blog(LOG_WARNING, "MuteandDeafen: useschandlerid=i Recieve Failure");
			return false;
		}
		memset(reci3, 0, 256);

		if(mnd == 1 || mnd == 3)	//if set to mute
		{
			char reci1[256];;

			std::string tempmute = "clientupdate client_input_muted=";
			tempmute.append(sstate.str());
			const char *mute = tempmute.c_str();

			iResult = obs.sendAll(mute, (int)strlen(mute), 0);	//set mute
			if (!iResult)
			{
				blog(LOG_WARNING, "MuteandDeafen: Mute Send Failure");
				return false;
			}
			iResult = obs.recvAll(reci1, 256, 0, "msg=");	//recieve result: error id=0...
			if (!iResult)
			{
				blog(LOG_WARNING, "MuteandDeafen: Mute Recieve Failure");
				return false;
			}
		}

		if(mnd == 2 || mnd == 3)
		{
			char reci2[256];
	
			std::string tempdeaf = "clientupdate client_output_muted=";
			tempdeaf.append(sstate.str());
			const char *deaf = tempdeaf.c_str();
			iResult = obs.sendAll(deaf, (int)strlen(deaf), 0);	//set deafen
			if (!iResult)
			{
				blog(LOG_WARNING, "MuteandDeafen: Deaf Send Failure");
				return false;
			}
			iResult = obs.recvAll(reci2, 256, 0, "msg=");	//recieve result: error id=0...
			if (!iResult)
			{
				blog(LOG_WARNING, "MuteandDeafen: Deaf Recieve Failure");
				return false;
			}
		}
	}

	//return to default server
	sendschandlerid = "use " + schandlerid[0] + "\n";
	iResult = obs.sendAll(sendschandlerid.c_str(), sendschandlerid.size(), 0);	//use schandlerid=0
	if (!iResult)
	{
		blog(LOG_WARNING, "MuteandDeafen: useschandlerid=0 Send Failure");
		return false;
	}
	iResult = obs.recvAll(reci3, 256, 0, "msg=");	//recieve result: error id=0...
	if (!iResult)
	{
		blog(LOG_WARNING, "MuteandDeafen: useschandlerid=0 Recieve Failure");
		return false;
	}

	return true;
}
