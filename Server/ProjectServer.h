#ifndef PROJECTSERVER_H
#define PROJECTSERVER_H

/////////////////////////////////////////////////////////////////////////
// ProjectServer.h									 				   
//																	      
// Package for parsing request from MsgClient and sending Http Messages 
// to MsgClient														    	
// Ver 1.0															    
// Application: can store a key and a pair of string values			   
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015     
// Author:      Jim Fawcett											 
//Changed by:	Syed Yaser Ahmed Syed								
//              sysyed@syr.edu										
//////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This is the server application that runs endlessly unless closed
* Used for parsing requests from MsgClient and sends HTTP Messages to Client
* All messages are POST as we are using ASynchronous one-way messaging
*
*/
/*
* Required Files:
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../CodeAnalyzer/FileSystem.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../CodeAnalyzer/Logger.h"
#include "../Utilities/Utilities.h"
#include "../CodeAnalyzer/Executive.h"
#include "../CodeAnalyzer/Executive.cpp"
#include <string>
#include <iostream>
using namespace Async;
class ClientHandler
{
public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	using EndPoint = std::string;
	using IISPath = std::string;
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
};
class ServerSender
{
public:
	using EndPoint = std::string;
	void sendCategories(std::string);
	void sendmessage(std::string);
	int setArguments();
	void sendcfiles(std::string);
	void downloadfiles(std::string);
	void deleteFile(std::string);
	void setupIIS(std::string);
	void sendpfiles(std::string);
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket);
	void truncateMap();
};

#endif