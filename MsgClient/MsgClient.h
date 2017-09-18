//////////////////////////////////////////////////////////////////////
// MsgClient  .h									 				//
//																	//
// Package for sending Http Messages to Server   					//
// Ver 1.0															//
// Application: can store a key and a pair of string values			//
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015	//
// Author:      Jim Fawcett
//Changed by:	Syed Yaser Ahmed Syed								//
//              sysyed@syr.edu										//
//////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* Can be called by Channel dll to parse messages from Client WPF and sending requests in the form of HTTP messages
* To the server. Uses Socket, HttpMessage, FileSystem, Logger, Utilities and BlockingQueue for sending and receiving messages
* to and from client/server
*
*/

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../CodeAnalyzer/FileSystem.h"
#include "../CodeAnalyzer/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include <string>
#include <iostream>
#include <thread>
#include "../Channel/Channel.h"

using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

class ServerHandler
{
public:
	ServerHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	using EndPoint = std::string;
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	Async::BlockingQueue<HttpMessage>& msgQ_;
};
/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void sendFiles(std::vector<std::string>,std::string iisPath_,std::string category);
	std::string sendMessage(std::string);
	std::string downloadFunction(std::string MsgString,SocketConnecter& si);
	std::string LazyDownloadFunction(std::string MsgString, SocketConnecter& si);
	std::string DownloadFileList(std::string MsgString, SocketConnecter& si);
	std::string PublishFunction(std::string MsgString, SocketConnecter& si);
	std::string CategoryFunction(std::string MsgString, SocketConnecter& si);
	std::string StructureFunction(std::string MsgString, SocketConnecter& si);
	std::string deleteFunction(std::string MsgString, SocketConnecter& si);
	std::string noParentFunction(std::string MsgString, SocketConnecter& si);
	void SendIISPath(std::string MsgString, SocketConnecter& si);
	void downloadWrapper(std::string MsgString);
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket);
	std::string getCategory(std::string);
	
};
static std::string returnmsg = "empty";
static std::string ucategory = "";
//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
inline HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

inline void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
inline bool MsgClient::sendFile(const std::string& filename, Socket& socket)
{
	// assumes that socket is connected
	FileSystem::FileInfo fi(filename);
	//FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(filename);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8080");
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addAttribute(HttpMessage::Attribute("category", ucategory));

	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
//----< this defines the behavior of the client >--------------------
inline void MsgClient::sendFiles(std::vector<std::string> files,std::string iisPath_,std::string category)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}

		for (size_t i = 0; i < files.size(); ++i)
		{
			Show::write("\n\n  sending file " + files[i]);
			std::size_t foundh = files[i].find(".h");
			std::size_t foundcpp = files[i].find(".cpp");
			if (foundh != std::string::npos || foundcpp != std::string::npos)
			{
				std::cout << "\n Sending File: " << files[i] << std::endl;
				ucategory = category;
				sendFile(files[i], si);
				::Sleep(100);
			}
		}

		HttpMessage msg;
		std::string msgBody = "<msg>" + iisPath_ + "</msg>";
		msgBody ="<msg>quit</msg>";
		msg = makeMessage(1, msgBody, "localhost:8080");
		sendMessage(msg, si);
		::Sleep(100);
	}
	catch (std::exception& ex)
	{
		std::cout << "Error occured while sending files " << ex.what();
	}
}
// --------------<<Download Wrapper function for reading msgstring and downloading files >------------------
void MsgClient::downloadWrapper(std::string MsgString)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
			::Sleep(100);

		std::vector<std::string> dvector = Utilities::StringHelper::split(MsgString);
		std::string resultString = "Downloaded,";
		if (dvector.at(1) != "DownloadFile")
		{
			HttpMessage filemsg = makeMessage(1, dvector.at(1), "localhost:8080");
			filemsg.addAttribute(HttpMessage::Attribute("command", "Download"));
			sendMessage(filemsg, si);
			::Sleep(100);
			SocketSystem ss;
			SocketListener sl(8070, Socket::IP6);
			Async::BlockingQueue<HttpMessage> msgQ;
			ServerHandler cp(msgQ);
			sl.start(cp);
			while (true)
			{
				std::cout << "Listener running";
				::Sleep(100);
				HttpMessage msg = msgQ.deQ();
				//std::cout << "\n\n  return msg Client recvd message contents:\n" << msg.findValue("file") << std::endl;
				if (msg.findValue("file") != "")
				{
					resultString += msg.findValue("file");
					std::cout << "\n printing resultString " << resultString << std::endl;
				}
				if (msg.bodyString() == "<msg>quit</msg>")
				{
					break;
				}
			}
		}
	}
	catch (std::exception& ex) {
		std::cout << "Error occured while sending files " << ex.what();
	}
}
// ---------------< Function for deleting a file, gets arguments from the Operator() >----------------------
std::string MsgClient::deleteFunction(std::string MsgString, SocketConnecter & si)
{
	std::vector<std::string> dvector = Utilities::StringHelper::split(MsgString);
	std::string resultString = "Deleted->";
	if (dvector.at(1) != "DeleteFl" && dvector.at(1) != "")
	{
		HttpMessage filemsg = makeMessage(1, dvector.at(1), "localhost:8080");
		filemsg.addAttribute(HttpMessage::Attribute("command", "Delete"));
		sendMessage(filemsg, si);
		resultString += dvector.at(1);
	}
	return resultString;
}
// -------< Function for sending independent file download request, algorithm will be called from the server >-------------------
inline std::string MsgClient::noParentFunction(std::string MsgString, SocketConnecter & si)
{
	std::vector<std::string> categoryV = Utilities::StringHelper::split(MsgString);
	HttpMessage cmsg;
	cmsg = makeMessage(1, categoryV.at(1), "localhost::8080");
	cmsg.addAttribute(HttpMessage::Attribute("GetNoParent", "Yes"));
	sendMessage(cmsg, si);
	::Sleep(100);
	SocketSystem ss;
	SocketListener sl(8070, Socket::IP6);
	Async::BlockingQueue<HttpMessage> msgQ;
	ServerHandler cp(msgQ);
	sl.start(cp);
	while (true)
	{
		HttpMessage msg = msgQ.deQ();
		returnmsg = msg.bodyString();
		//std::cout << "\n\n  return msg Client recvd message contents:\n" << returnmsg << std::endl;
		if (msg.findValue("GetNoParent?") == "Yes" && returnmsg != "empty")
		{
			returnmsg = "ParentFiles : " + returnmsg;
			return returnmsg;
		}
	}
	sl.stop();
	return returnmsg;
}
static bool lazyDownloadStop_ = true;
static bool downloadStop_ = true;
// -------< Function for getting files in a category from Server-Repository >-------------------
std::string MsgClient::downloadFunction(std::string MsgString,SocketConnecter& si)
{
	std::vector<std::string> dvector = Utilities::StringHelper::split(MsgString);
	std::string resultString="Downloaded file";
	if (dvector.at(1) != "DownloadFile" && dvector.at(1) != "")
	{
		HttpMessage filemsg = makeMessage(1, dvector.at(1), "localhost:8080");
		filemsg.addAttribute(HttpMessage::Attribute("command", "Download"));
		sendMessage(filemsg, si);
		::Sleep(100);
		SocketSystem ss;
		SocketListener sl(8070, Socket::IP6);
		Async::BlockingQueue<HttpMessage> msgQ;
		ServerHandler cp(msgQ);
		sl.start(cp);
		while (downloadStop_)
		{}
		downloadStop_ = true;
		sl.stop();
	}
	return resultString;
}
// -------< Function for executing Lazy Download, which downloads children and grandchildren of a file, only works if published first >-------------------
std::string MsgClient::LazyDownloadFunction(std::string MsgString, SocketConnecter& si) 
{
	bool downstop_ = true;
	std::vector<std::string> dvector = Utilities::StringHelper::split(MsgString);
	if (dvector.at(1) != "LazyFile" && dvector.at(1) !="")
	{
		HttpMessage filemsg = makeMessage(1, dvector.at(1), "localhost:8080");
		filemsg.addAttribute(HttpMessage::Attribute("command", "Download"));
		filemsg.addAttribute(HttpMessage::Attribute("lazydownload", "Yes"));
		sendMessage(filemsg, si);
		::Sleep(100);
		SocketSystem ss;
		SocketListener sl(8070, Socket::IP6);
		Async::BlockingQueue<HttpMessage> msgQ;
		ServerHandler cp(msgQ);
		sl.start(cp);
		while (lazyDownloadStop_)
		{}
		lazyDownloadStop_ = true;
		return "LazyFile,";
		sl.stop();
	}
	else
	{
		return "Please select a file to download";
	}
}
// -------< Function for opening a html file in the server category >-------------------
std::string MsgClient::DownloadFileList(std::string MsgString, SocketConnecter& si)
{
	std::vector<std::string> dvector = Utilities::StringHelper::split(MsgString);
	for each(std::string dfiles in dvector)
	{
		if (dfiles != "DownloadF" && dfiles != dvector.at(1) && dfiles != ":")
		{
			HttpMessage filemsg = makeMessage(1, dfiles, "localhost:8080");
			filemsg.addAttribute(HttpMessage::Attribute("command", "Launch"));
			filemsg.addAttribute(HttpMessage::Attribute("category", dvector.at(1)));
			sendMessage(filemsg, si);
		}
	}
	return "Getting File List";
}
// -------< Function for sending publish command >-------------------
std::string MsgClient::PublishFunction(std::string MsgString, SocketConnecter& si)
{
	std::vector<std::string> catVector = Utilities::StringHelper::split(MsgString);
	std::string resultStr = "Could not publish";
	bool listenerstatus_ = false;
	for each(std::string category in catVector)
	{
		if (category != "Publish:" && category != ":" && category != "Publish")
		{
			HttpMessage cmsg;
			cmsg = makeMessage(1, "start", "localhost::8080");
			cmsg.addAttribute(HttpMessage::Attribute("publish-category", category));
			sendMessage(cmsg, si);
			::Sleep(100);
			cmsg = makeMessage(1, "quit", "localhost:8080");
			sendMessage(cmsg, si);
			::Sleep(100);
		}
	}
	SocketSystem ss;
	SocketListener sl(8070, Socket::IP6);
	Async::BlockingQueue<HttpMessage> msgQ;
	ServerHandler cp(msgQ);
	sl.start(cp);
	while (listenerstatus_)
	{
		HttpMessage msg = msgQ.deQ();
		returnmsg = msg.bodyString();
		//std::cout << "\n\n  return msg Client recvd message contents:\n" << returnmsg << std::endl;
		if (returnmsg != "empty")
			return returnmsg;
		if (msg.findValue("server-categories") == "No")
		{
			return returnmsg;
		}
	}
	sl.stop();
	listenerstatus_ = true;
	return returnmsg;
}
// -------------< Function for getting all the files in a category >---------------
std::string MsgClient::CategoryFunction(std::string MsgString, SocketConnecter& si) 
{
	bool listenerstatus_ = true;
	std::vector<std::string> categoryV = Utilities::StringHelper::split(MsgString);
	HttpMessage cmsg;
	cmsg = makeMessage(1, categoryV.at(1), "localhost::8080");
	cmsg.addAttribute(HttpMessage::Attribute("getCfiles", "Yes"));
	sendMessage(cmsg, si);
	::Sleep(100);
	SocketSystem ss;
	SocketListener sl(8070, Socket::IP6);
	Async::BlockingQueue<HttpMessage> msgQ;
	ServerHandler cp(msgQ);
	sl.start(cp);
	while (true)
	{
		HttpMessage msg = msgQ.deQ();
		returnmsg = msg.bodyString();
		//std::cout << "\n\n  return msg Client recvd message contents:\n" << returnmsg << std::endl;
		if (msg.findValue("categoryfiles?") == "Yes" && returnmsg != "empty")
		{
			returnmsg = "CategoryFiles : " + returnmsg;
			return returnmsg;
		}
	}
	sl.stop();
}
// -------< Function for getting structure of Server-Repository >-------------------
std::string MsgClient::StructureFunction(std::string MsgString, SocketConnecter& si) 
{
	HttpMessage msg;
	std::string msgBody = "<msg>" + MsgString + "</msg>";
	msg = makeMessage(1, msgBody, "localhost:8080");
	sendMessage(msg, si);
	::Sleep(100);
	SocketSystem ss;
	SocketListener sl(8070, Socket::IP6);
	Async::BlockingQueue<HttpMessage> msgQ;
	ServerHandler cp(msgQ);
	sl.start(cp);
	while (true)
	{
		HttpMessage msg = msgQ.deQ();
		returnmsg = msg.bodyString();
		//std::cout << "\n\n  return msg Client recvd message contents:\n" << returnmsg << std::endl;
		if (returnmsg != "empty")
			return returnmsg;
		else
			return "empty";
	}
	sl.stop();
}
// -------< Function for sending IIS Server path, it should be absolute or it won't work >-------------------
void MsgClient::SendIISPath(std::string MsgString, SocketConnecter& si)
{
	std::vector<std::string> folderPath = Utilities::StringHelper::split(MsgString);
	HttpMessage foldermsg = makeMessage(1, folderPath.at(1), "localhost:8080");
	foldermsg.addAttribute(HttpMessage::Attribute("save_path", folderPath.at(1)));
	sendMessage(foldermsg, si);
}
// -------< Utility Function for replacing sub-string in a string >-------------------
void replaceAll(std::string &s, const std::string &search, const std::string &replace) {
	for (size_t pos = 0; ; pos += replace.length()) {
		pos = s.find(search, pos);
		if (pos == std::string::npos) break;
		s.erase(pos, search.length());
		s.insert(pos, replace);
	}
}
// -------< Function for sending string messages to server, used by various functions in the project >-------------------
inline std::string MsgClient::sendMessage(std::string MsgString)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
			::Sleep(100);
		if (MsgString.find("DownloadFile,") != std::string::npos){
			downloadStop_ = true;
			std::string result = downloadFunction(MsgString, si);
			return result;
		}
		if (MsgString.find("DeleteFl,") != std::string::npos){
			std::string result=deleteFunction(MsgString,si);
			return result;
		}
		if (MsgString.find("LazyFile,") != std::string::npos){
			lazyDownloadStop_ = true;
			std::string result = LazyDownloadFunction(MsgString, si);
			return result;
		}
		if (MsgString.find("GetNoParent") != std::string::npos){
			std::string result = noParentFunction(MsgString, si);
			return result;
		}
		if (MsgString.find("setupfolder") != std::string::npos){
			SendIISPath(MsgString, si);
			return "IIS setup";
		}
		if (MsgString.find("DownloadF") != std::string::npos){
			std::string result = DownloadFileList(MsgString, si);
			return result;
		}
		if (MsgString.find("Publish:")!=std::string::npos){
			std::string result = PublishFunction(MsgString, si);
			return result;
		}
		if (MsgString.find("CategoryF") != std::string::npos){
			std::string result = CategoryFunction(MsgString, si);
			return result;
		}
		if (MsgString == "GetStructure"){
			std::string result = StructureFunction(MsgString, si);
			return result;
		}}
	catch (std::exception& ex) {
		std::cout << "Error occured while sending files " << ex.what();
	}
	return returnmsg;
}

//----< entry point - runs two clients each on its own thread >------
inline void ServerHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		if (msg.findValue("server-categories") == "Yes")
		{
			std::string stringmsg = msg.bodyString();
		}
		if (msg.findValue("file") != "")
		{
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);

				std::string fileq = msg.findValue("file");
				replaceAll(fileq, "./Server-Repository/", "");
				readFile(fileq, contentSize, socket);
		}
		msgQ_.enQ(msg);
	}
}
// ----------< Function to replace content-length attribute to accomodate new message structure >------------
HttpMessage removeAFunction(std::string filename, HttpMessage& msg)
{
	msg.removeAttribute("content-length");
	std::string bodyString = "<file>" + filename + "</file>";
	std::string sizeString = Converter<size_t>::toString(bodyString.size());
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addBody(bodyString);
	return msg;
}
// -------< This function is called by the MsgClient Entry point, based on type of message >-------------------
inline HttpMessage ServerHandler::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;
	while (true){
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}
	if (msg.attributes().size() == 0){
		connectionClosed_ = true;
		return msg;
	}
	if (msg.findValue("lazydownload") == "Yes") {
		lazyDownloadStop_ = false;
		downloadStop_ = false;
		return msg;
	}
	if (msg.attributes()[0].first == "POST"){
		std::string filename = msg.findValue("file");
		if (filename != ""){
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			::Sleep(100);
			std::string fileq = msg.findValue("file");
			replaceAll(fileq, "./Server-Repository/", "");
			readFile(fileq, contentSize, socket);
		}
		if (filename != "")
			msg = removeAFunction(filename, msg);
		else{
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size()){
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}}}
	return msg;
}
//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
inline bool ServerHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = "./Client-Repository/" + filename;
	replaceAll(fqname, "./Server-Repository/", "");
	
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);
		socket.sendString("\nGot file", '\000');
		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//Function to get category from a filename received from the Client WPF
inline std::string MsgClient::getCategory(std::string filename)
{
	std::string category = filename.substr(filename.find_last_of('\\') + 1);
	size_t found;
	found = category.find_last_of(".");
	category = category.substr(0, found);
	return category;
}

