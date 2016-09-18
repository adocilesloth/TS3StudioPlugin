/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

#include "slothSock.hpp"
#include <obs-module.h>
#include <sstream>

#if _WIN32
	bool slothSock::connectToHost(int port, char* adrs)
	{
		WSADATA wsadata;
		int error = WSAStartup(0x0202, &wsadata);	//error on startup?

		if(error)
		{
			blog(LOG_WARNING, "TS3: Socket: error");
			return false;
		}
		if(wsadata.wVersion != 0x0202)	//error check winsock version
		{
			blog(LOG_WARNING, "TS3: Socket: != 0x0202");
			WSACleanup(); //Clean up Winsock
			return false;
		}
		SOCKADDR_IN target;							//Socket address information
		target.sin_family = AF_INET;				//address family Internet
		target.sin_port = htons (port);				//Port to connect on
		target.sin_addr.s_addr = inet_addr (adrs);	//Target IP
		sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
		if(sock == INVALID_SOCKET)
		{
			blog(LOG_WARNING, "TS3: Socket: INVALID_SOCKET");
			return false; //Couldn't create the socket
		}  
		if(connect(sock, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR) //connect
		{
			blog(LOG_WARNING, "TS3: Socket: Failed to connect");
			std::stringstream code;
			code << WSAGetLastError();
			blog(LOG_WARNING, code.str().c_str());
			return false; //Couldn't connect
		}
		else
		{
			return true; //Success
		}
	}
		
	void slothSock::closeConnection()
	{
		//Close the socket if it exists
		if(sock)
		{
		   closesocket(sock);
		}

		WSACleanup(); //Clean up Winsock
	}

	bool slothSock::sendAll(char *buffer, int length, int flag)
	{
		char *ptr = (char*)buffer;
		while(length > 0)
		{
			int i = send(sock, ptr, length, flag);
			if(i < 0)
			{
				return false;
			}
			ptr += i;
			length -= i;
		}
		return true;
	}

	bool slothSock::sendAll(const char *buffer, int length, int flag)
	{
		const char *ptr = (const char*)buffer;
		while(length > 0)
		{
			int i = send(sock, ptr, length, flag);
			if(i < 0)
			{
				return false;
			}
			ptr += i;
			length -= i;
		}
		return true;
	}

	bool slothSock::recvAll(char *buffer, int length, int flag)
	{
		char *ptr = (char*)buffer;
		while(length > 0)
		{
			int i = recv(sock, ptr, length, flag);
			if(i < 0)
			{
				return false;
			}
			else if(i == 0)
			{
				break;
			}
			ptr += i;
			length -= i;
		}
		return true;
	}
	
	bool slothSock::recvAll(std::vector<char> &vbuffer, int length, int flag)
	{
		std::vector<char> vtemp;
		int lastPlace = 0;
		while(length > 0)
		{
			vtemp.resize(length);
			int i = recv(sock, &vtemp[0], length, flag);
			if(i < 0)
			{
				return false;
			}
			else if(i == 0)
			{
				break;
			}
		
			for(int j = 0; j < i; j++)
			{
				vbuffer[j + lastPlace] = vtemp[j];
			}
							lastPlace += i;
			length -= i;
			vtemp.clear();
		}
		return true;
	}

	bool slothSock::recvAll(char *buffer, int length, int flag, std::string endid)
	{
		std::stringstream incomming;
		int endpos = -1;
		char *ptr = (char*)buffer;
		while(length > 0)
		{
			int i = recv(sock, ptr, length, flag);
			if(i < 0)
			{
				return false;
			}
			else if(i == 0)
			{
				break;
			}
			ptr += i;
			length -= i;
			
			incomming << buffer;
			endpos = incomming.str().find(endid);
			if(endpos > 0)
			{
				break;
			}
			incomming.str("");
		}
		return true;
	}
#else	//Unix
	bool slothSock::connectToHost(int port, char* adrs)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(sock < 0)
		{
			blog(LOG_WARNING, "TS3: Socket: Failed to create socket");
			return false;
		}

		struct sockaddr_in remoteaddr;
		remoteaddr.sin_family = AF_INET;
		remoteaddr.sin_addr.s_addr = inet_addr(adrs);
		remoteaddr.sin_port = htons(port);
		if(connect(sock, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) < 0)
		{
			//blog(LOG_WARNING, "TS3: Socket: Failed to connect");
			return false;
		}
		else
		{
			return true;
		}
	}

	void slothSock::closeConnection()
	{
		//Close the socket if it exists
		if(sock)
		{
		   close(sock);
		}
	}
	/* Note, 'int flag' isn't used, it's just there to be consistant with the Windows calls*/
	bool slothSock::sendAll(char *buffer, int length, int flag)
	{
		char *ptr = (char*)buffer;
		while(length > 0)
		{
			int i = write(sock, ptr, length);
			if(i < 0)
			{
				return false;
			}
			ptr += i;
			length -= i;
		}
		return true;
	}

	bool slothSock::sendAll(const char *buffer, int length, int flag)
	{
		const char *ptr = (const char*)buffer;
		while(length > 0)
		{
			int i = write(sock, ptr, length);
			if(i < 0)
			{
				return false;
			}
			ptr += i;
			length -= i;
		}
		return true;
	}

	bool slothSock::recvAll(char *buffer, int length, int flag)
	{
		char *ptr = (char*)buffer;
		while(length > 0)
		{
			int i = read(sock, ptr, length);
			if(i < 0)
			{
				return false;
			}
			else if(i == 0)
			{
				break;
			}
			ptr += i;
			length -= i;
		}
		return true;
	}
	
	bool slothSock::recvAll(std::vector<char> &vbuffer, int length, int flag)
	{
		std::vector<char> vtemp;
		int lastPlace = 0;
		while(length > 0)
		{
			vtemp.resize(length);
			int i = read(sock, &vtemp[0], length);
			if(i < 0)
			{
				return false;
			}
			else if(i == 0)
			{
				break;
			}
		
			for(int j = 0; j < i; j++)
			{
				vbuffer[j + lastPlace] = vtemp[j];
			}
			lastPlace += i;
			length -= i;
			vtemp.clear();
		}
		return true;
	}

	bool slothSock::recvAll(char *buffer, int length, int flag, std::string endid)
	{
		std::stringstream incomming;
		int endpos = -1;
		char *ptr = (char*)buffer;
		while(length > 0)
		{
			int i = read(sock, ptr, length);
			if(i < 0)
			{
				return false;
			}
			else if(i == 0)
			{
				break;
			}
			ptr += i;
			length -= i;
			
			incomming << buffer;
			endpos = incomming.str().find(endid);
			if(endpos > 0)
			{
				break;
			}
			incomming.str("");
		}
		return true;
	}
#endif
