/////////////////////////////////////////////////////////////////////////
// ProjectServer.cpp - Demonstrates simple one-way HTTP style messaging//
//                 and file transfer                                   //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Changed by Syed Yaser Ahmed Syed									   //
// Application: OOD Project #4                                         //
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015      //
/////////////////////////////////////////////////////////////////////////


#include"ProjectServer.h"
#include "windows.h"
#include <set>
#include "../CodeAnalyzer/ScopeStack.h"

using Show1 = Logging::StaticLogger<1>;
using namespace Utilities;

#pragma warning(disable:4996)
/////////////////////////////////////////////////////////////////////
// ClientHandler class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You need to be careful using data members of this class
//   because each client handler thread gets a reference to this 
//   instance so you may get unwanted sharing.
// - I may change the SocketListener semantics (this summer) to pass
//   instances of this class by value.
// - that would mean that all ClientHandlers would need either copy or
//   move semantics.
//

//----< this defines processing to frame messages >------------------
static std::string Fpath_="./Server-Repository/";
static std::string ServerPath_;
static bool connectionStatus_ = true;
static bool sendquit = true;
static std::string fileCategory_;
std::string CategoryTable_="Category:";

//-----< Function to add content-length by checking filename length as the message received does not have <file> tags >------------
HttpMessage removeAFunction(std::string filename, HttpMessage& msg)
{
	msg.removeAttribute("content-length");
	std::string bodyString = "<file>" + filename + "</file>";
	std::string sizeString = Converter<size_t>::toString(bodyString.size());
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addBody(bodyString);
	return msg;
}
//------< Function to read messages from socket, called by Operator() >--------------------------
HttpMessage ClientHandler::readMessage(Socket& socket)
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
	if (msg.attributes()[0].first == "POST")
	{
		std::string filename = msg.findValue("file");
		if (filename != "")
		{
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			fileCategory_ = msg.findValue("category");
			readFile(filename, contentSize, socket);
		}

		if (filename != "")
			msg = removeAFunction(filename, msg);
		else
		{
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size()) {
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
		}
	}
	return msg;
}

//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/


bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string newPath = "./Server-Repository/" + fileCategory_;
	if (!CreateDirectory(newPath.c_str(), NULL))
	{
		//std::cout << "\nCreated:" << newPath;
	}
	CategoryTable_ += fileCategory_ + "-" + filename.substr(filename.find_last_of('\\') + 1) +",";
	std::string fqname = "./Server-Repository/" + fileCategory_ + "/" + filename.substr(filename.find_last_of('\\') + 1);
	//std::string fqname = filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		Show1::write("\n\n  can't open file " + fqname);
		return false;
	}
	std::cout << "\n Received File from Client as streams of bytes: " << fqname << std::endl;
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
//----< receiver functionality is defined by this function >---------
HttpMessage ServerSender::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
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

void ServerSender::sendMessage(HttpMessage& msg, Socket& socket)
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
bool ServerSender::sendFile(const std::string& filename, Socket& socket)
{
	std::string fqname = filename;
	std::string tempcategory = filename.substr(0, filename.find_last_of("\\/"));
	std::stringstream ss(tempcategory);
	std::string subs2;
	std::string subs;
	while (ss.good()){
		getline(ss, subs, '/');
		if (subs.find("Category") != std::string::npos)
			subs2 = subs;
	}
	if (filename == "./Server-Repository/quit.html")
	{
		HttpMessage lmsg = makeMessage(1, "lazyquitmessage", "localhost::8070");
		lmsg.addAttribute(HttpMessage::Attribute("lazydownload", "Yes"));
		sendMessage(lmsg, socket);
		return false;
	}
	else {
		FileSystem::FileInfo fi(fqname);
		std::cout << fqname << std::endl;
		size_t fileSize = fi.size();
		std::string sizeString = Converter<size_t>::toString(fileSize);
		FileSystem::File file(fqname);
		file.open(FileSystem::File::in, FileSystem::File::binary);
		if (!file.isGood())
			return false;
		HttpMessage msg = makeMessage(1, "file", "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("file", fqname.substr(fqname.find_last_of('\\') + 1)));
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addAttribute(HttpMessage::Attribute("category", subs2));
		sendMessage(msg, socket);
		::Sleep(100);
		const size_t BlockSize = 2048;
		Socket::byte buffer[BlockSize];
		while (true){
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
}
std::unordered_map<std::string, std::vector<std::string>> dependencyTable_;
std::unordered_map<std::string, std::vector<std::string>> truncatedTab_;

// --------------------< Function to call code analyzer by setting arguments and calls publisher for publishing file >-------------------- //
int ServerSender::setArguments()
{
	int argc=6;
	char* argv[6];
	std::string filearg = "./Server-Repository/" + fileCategory_;
	std::vector<std::string> htmlfiles = FileSystem::Directory::getFiles(filearg, "*.html");
	for (std::string fi : htmlfiles)
		FileSystem::File::remove(fi);
	if (!FileSystem::Directory::exists(filearg))
	{
		return 1;
	}
	char * stringtochar = new char[filearg.length() + 1];
	std::strcpy(stringtochar, filearg.c_str());
	argv[1] = stringtochar;
	argv[2] = "*.h";
	argv[3] = "*.cpp";
	argv[4] = "a";
	argv[5] = "index.html";
	CodeAnalysis::CodeAnalysisExecutive exec;
	bool succeeded = exec.ProcessCommandLine(argc, argv);
	if (!succeeded)
		return 1;
	exec.getSourceFiles();
	exec.processSourceCode(true);
	TypeAnalysis ta;
	std::unordered_map<std::string, _pair> newtable = ta.doTypeAnal();
	std::vector<std::string> vec = exec.cppHeaderFiles(), vec2 = exec.cppImplemFiles();
	vec.insert(vec.end(), vec2.begin(), vec2.end());
	DependencyAnalyzer da;
	dependencyTable_ = da.findDependency(newtable, vec);
	std::unordered_map <std::string, std::vector<std::pair<int, int>>> tab = exec.getnamespace();
	//Use db for dependency creation
	std::cout << "\n Project 4- Requirement 4: Satisfies the requirements of CodePublisher developed in Project #3. These will be displayed shortly \n";
	Publisher pr;
	pr.generateHTML(dependencyTable_, tab, Fpath_);
	std::cout << "\n Project 4- Requirement 3: Files Published in Repository as Linked HTML pages \n";
	truncateMap();
	return 0;
}
// -------------< Function to get independent files i.e. Files that have no parent >----------------
void ServerSender::sendpfiles(std::string CFiles)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8070))
		{
			Show1::write("\n client waiting to connect");
			::Sleep(100);
		}

		HttpMessage msg;
		std::string msgBody = "NoPrnt,"+CFiles;

		msg = makeMessage(1, msgBody, "localhost:8070");
		msg.addAttribute(HttpMessage::Attribute("GetNoParent?", "Yes"));
		sendMessage(msg, si);
		Show1::write("\n\n  sent to Client\n" + msg.toIndentedString());
		::Sleep(100);
	}
	catch (std::exception& exc)
	{
		Show1::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show1::write(exMsg);
	}
}
// -------------< Function to get files present in a category >----------------
void ServerSender::sendcfiles(std::string CFiles)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8070))
		{
			Show1::write("\n client waiting to connect");
			::Sleep(100);
		}

		HttpMessage msg;
		std::string msgBody = "CategoryFiles : "+CFiles;

		msg = makeMessage(1, msgBody, "localhost:8070");
		msg.addAttribute(HttpMessage::Attribute("categoryfiles?", "Yes"));
		sendMessage(msg, si);
		Show1::write("\n\n  sent to Client\n" + msg.toIndentedString());
		::Sleep(100);
	}
	catch (std::exception& exc)
	{
		Show1::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show1::write(exMsg);
	}
}
// -------------< Function to Delete Given file from path provided by client >----------------
void ServerSender::deleteFile(std::string msg)
{
	std::cout << "\n This is the current Fpath_ ../Server-Repository/" << Fpath_;
	std::string filePath = Fpath_ + msg;
	FileSystem::File::remove(filePath);
	std::cout << "\n Attempting to remove file : " + filePath;
}
// -------------< Function to download selected file from Server>----------------
void ServerSender::downloadfiles(std::string filename)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8070))
		{
			Show1::write("\n client waiting to connect");
			//std::cout << "Not connected\n";
			::Sleep(100);
		}

		std::cout << "\n\n  sending file " + filename << std::endl;
		//sendFile(filename, si);
		if (filename.find("./Server-Repository") == std::string::npos){
			sendFile("./Server-Repository/" + filename, si);
			::Sleep(100);
		}
		else {
			sendFile(filename, si);
			::Sleep(100);
		}

		Show1::write("\n");
		Show1::write("\n  All done folks");
	}
	catch (std::exception& exc)
	{
		Show1::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show1::write(exMsg);
	}
}
// ----------< function for setting up IIS >----------------
void ServerSender::setupIIS(std::string path)
{
	std::stringstream ss(path);
	std::string subs2;
	std::string subs;
	while (ss.good())
	{
		getline(ss, subs, '\\');
		if (subs.find("Category") != std::string::npos)
		{
			subs2 = subs;
		}
	}
	std::string newpath = Fpath_+subs2;
	std::cout << "\n This is the current path " << newpath<< std::endl;
	std::vector<std::string> files = FileSystem::Directory::getFiles(newpath, "*.*");
	for (std::string fi : files)
	{
		FileSystem::File::copy(newpath+"\\"+fi, path +"\\"+fi);
	}
	//For getting resources
	std::string resourceFolder = newpath + "/Resources/";
	std::vector<std::string> resourcefiles = FileSystem::Directory::getFiles(resourceFolder, "*.*");
	for (std::string rfi : resourcefiles)
	{
		FileSystem::Directory::create(path+"\\Resources\\");
		FileSystem::File::copy(resourceFolder + rfi, path + "\\Resources\\" + rfi);
	}
	std::string startchr = "start chrome @localhost:8080/" + subs2;
	std::system(startchr.c_str());
}
// -------< Function for getting files in a category from Server-Repository >-------------------
void ServerSender::sendCategories(std::string categoryString)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8070))
		{
			Show1::write("\n client waiting to connect");
			::Sleep(100);
		}
		std::vector<std::string> files = FileSystem::Directory::getDirectories(Fpath_, "*Category*");
		HttpMessage msg;
		std::string msgBody = "Category : ";
		for each(std::string file in files)
			msgBody += file+",";

		msg = makeMessage(1, msgBody, "localhost:8070");
		msg.addAttribute(HttpMessage::Attribute("server-categories", "Yes"));
		sendMessage(msg, si);
		Show1::write("\n\n  sent to Client\n" + msg.toIndentedString());
		::Sleep(100);

		Show1::write("\n");
		Show1::write("\n  All done folks");
	}
	catch (std::exception& exc)
	{
		Show1::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show1::write(exMsg);
	}
}
// -------< Function for sending string messages from Server to client>-------------------
void ServerSender::sendmessage(std::string)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8070))
		{
			Show1::write("\n client waiting to connect");
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;
		msg = makeMessage(1, "No files present in selected category!", "localhost:8070");
		msg.addAttribute(HttpMessage::Attribute("server-categories", "No"));
		sendMessage(msg, si);
		Show1::write("\n\n  sent to Client\n" + msg.toIndentedString());
		::Sleep(100);


		// shut down server's client handler

		msg = makeMessage(1, "quit", "toAddr:localhost:8080");
		sendMessage(msg, si);

	}
	catch (std::exception& exc)
	{
		Show1::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show1::write(exMsg);
	}
}
// -------< Entry point for the server, messages are read here and processes as per structure >-------------------
void ClientHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show1::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}
// -------------< function for getting only file names in the dependency table >----------------------
void ServerSender::truncateMap()
{
	for (auto fileSpec : dependencyTable_)
	{
		std::string files = (fileSpec.first).substr((fileSpec.first).find_last_of("\\/") + 1)+".html";
		for (std::string vec : fileSpec.second)
		{
			truncatedTab_[files].push_back((vec).substr((vec).find_last_of("\\/") + 1)+".html");
		}
		if (fileSpec.second.size() == 0)
		{
			std::string nodepfile= (fileSpec.first).substr((fileSpec.first).find_last_of("\\/") + 1) + ".html";
			std::vector<std::string> nodepVec;
			truncatedTab_[nodepfile] = nodepVec;
		}
	}
}
// -------< Function for sending files to the client based on path received in request messages >-------------------
void downloadFunction(ServerSender& send, HttpMessage msg)
{
	std::string filepath = msg.bodyString();
	std::string getfilename = filepath.substr(filepath.find_last_of("\\/") + 1);
	std::string category = filepath.substr(0, filepath.find_last_of("\\/"));
	if (msg.findValue("lazydownload") == "Yes")
	{
		std::set<std::string> fileList;
		ScopeStack<std::string> sstack;
		sstack.push(getfilename);
		while (sstack.size() > 0)
		{
			std::string poppedFile = sstack.pop();
			fileList.insert(poppedFile);
			std::vector<std::string> sv = truncatedTab_[poppedFile];
			for (std::string c : sv)
			{
				const bool is_in = fileList.find(c) != fileList.end();
				if (!is_in)
					sstack.push(c);
			}
		}
		size_t lcount = 0;
		for (std::string lazy : fileList)
		{
			send.downloadfiles("./Server-Repository/"+category+lazy);
			lcount = lcount + 1;
			if (lcount == fileList.size())
				send.downloadfiles("quit.html");
		}
	}
	else
	{
		send.downloadfiles(filepath);
		send.downloadfiles("quit.html");
	}
}
// -------< Function for starting Code Publisher, takes category to publish and sets arguments >-------------------
void startFunction(ServerSender& send, HttpMessage msg)
{
	std::string pcategory = msg.findValue("publish-category");
	if (pcategory != "")
		fileCategory_ = pcategory;
	std::string Fpath2 = Fpath_;
	Fpath_ = Fpath_ + fileCategory_ + "\\";
	int failure = send.setArguments();
	Fpath_ = Fpath2;
	if (failure == 1)
		send.sendmessage("No files present in selected category!");
}
// -------< Function for getting files in a category from Server-Repository >-------------------
void categoryFunction(ServerSender& send, HttpMessage msg)
{
	std::vector<std::string> files = FileSystem::Directory::getFiles(Fpath_ + "//" + msg.bodyString(), "*.html");
	std::string cfilestr;
	for each(std::string file in files)
		cfilestr += file + ",";
	send.sendcfiles(cfilestr);
}
// -------< Function for getting files that have no parent >-------------------
void NoParentFunction(ServerSender& send, HttpMessage msg)
{
	std::vector<std::string> listofFiles;
	std::set<std::string> noParentFiles;
	for (auto& i : truncatedTab_)
		listofFiles.push_back(i.first);
    
	for (std::string i : listofFiles){
		int pcount = 0;
		for (auto& j : truncatedTab_)
		{
			if (i != j.first)
			{
				for (std::string k : j.second)
					if (k == i)
						pcount++;
			}
		}
		if (pcount == 0)
		{
			const bool is_in = noParentFiles.find(i) != noParentFiles.end();
			if (!is_in)
				noParentFiles.insert(i);
		}
	}

	std::string cfilestr;
	for each(std::string file in noParentFiles)
		cfilestr += file + ",";

	send.sendpfiles(cfilestr);
}
//----< test stub >--------------------------------------------------

int main()
{
	::SetConsoleTitle("Server");
	Show1::attach(&std::cout);
	Show1::start();
	BlockingQueue<HttpMessage> msgQ;
	try
	{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP4);
		ClientHandler cp(msgQ);
		sl.start(cp);
		ServerSender send;
		while (connectionStatus_)
		{
			HttpMessage msg = msgQ.deQ();
			if (msg.findValue("getCfiles") != "")
				categoryFunction(send, msg);
			if (msg.findValue("save_path") != "")
			{
				ClientHandler::IISPath path= msg.findValue("save_path");
				if (!CreateDirectory(path.c_str(), NULL))
					std::cout << "\n Folder "<< path <<" did not exist, hence created" << std::endl;
				send.setupIIS(path);
			}
			if (msg.findValue("command") == "Download")
				downloadFunction(send, msg);
			if (msg.findValue("command") == "Delete")
				send.deleteFile(msg.bodyString());
			if (msg.bodyString() == "start")
				startFunction(send, msg);
			if (msg.findValue("GetNoParent") == "Yes")
				NoParentFunction(send, msg);
			if (msg.findValue("command") == "Launch" && msg.bodyString().find("DownloadFile")==std::string::npos){
				Publisher pr;//ServerPath_ changed to Fpath_
				pr.OpenInBrowser(Fpath_+ msg.findValue("category") + "\\" +msg.bodyString(), "chrome");
			}
			if (msg.bodyString() == "<msg>GetStructure</msg>"){
				std::cout << "Sending Categories to client" << std::endl;
				send.sendCategories(""); 
				::Sleep(100);
			}
		}
		connectionStatus_ = false;
	}
	catch (std::exception& exc)
	{
		std::cout << "Exception caught: " << exc.what() << std::endl;
	}
}