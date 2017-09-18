/////////////////////////////////////////////////////////////////////////////
// Channel.cpp -										                   //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - Channel reads from sendQ and writes to recvQ                          //
//                                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015               //
// Changed By: Syed Yaser Ahmed Syed									  //
/////////////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "Channel.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../MsgClient/MsgClient.h"
#include <string>
#include <thread>
#include <iostream>

using namespace Async;
using BQueue = BlockingQueue < Message >;

/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts messages from client for consumption by Channel
//
class Sendr : public ISendr
{
public:
	void postMessage(const Message& msg);
	BQueue& queue();
private:
	BQueue sendQ_;
};

void Sendr::postMessage(const Message& msg)
{
	sendQ_.enQ(msg);
}

BQueue& Sendr::queue() { return sendQ_; }

/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts messages from MockChanel for consumption by client
//
class Recvr : public IRecvr
{
public:
	Message getMessage();
	BQueue& queue();
private:
	BQueue recvQ_;
};

Message Recvr::getMessage()
{
	return recvQ_.deQ();
}

BQueue& Recvr::queue()
{
	return recvQ_;
}
/////////////////////////////////////////////////////////////////////////////
// Channel class
// - reads messages from Sendr and writes messages to Recvr
//
class Channel : public IChannel
{
public:
	Channel(ISendr* pSendr, IRecvr* pRecvr);
	void start();
	void setFolderPath(std::string);
	void setupFolder(std::string msg, MsgClient& c1, Sendr* pSendr);
	void getRepo(std::string msg, MsgClient& c1, Sendr* pSendr);
	void getFilesCategory(std::string msg, MsgClient& c1, Sendr* pSendr);
	void downloadfile(std::string msg, MsgClient& c1, Sendr* pSendr);
	void LazyDownload(std::string msg, MsgClient& c1, Sendr* pSendr);
	void deleteFile(std::string msg, MsgClient& c1, Sendr* pSendr);
	void publishCategory(std::string msg, MsgClient& c1, Sendr* pSendr);
	void getNoParent(std::string msg, MsgClient& c1, Sendr* pSendr);
	void OpenHTMLFile(std::string msg, MsgClient& c1, Sendr* pSendr);
	void stop();
	MsgClient c1;
private:
	std::string folderPath;
	std::thread thread_;
	ISendr* pISendr_;
	IRecvr* pIRecvr_;
	bool stop_ = false;
};

//----< pass pointers to Sender and Receiver >-------------------------------

Channel::Channel(ISendr* pSendr, IRecvr* pRecvr) : pISendr_(pSendr), pIRecvr_(pRecvr) {}

//-----< Function for IIS setup, Sends folder name given in IIS Setup tab of Client window >---------------//

void Channel::setupFolder(std::string msg,MsgClient& c1,Sendr* pSendr)
{
	std::string ms12 = c1.sendMessage(msg);
	pSendr->postMessage(ms12);
}

// ------< Function for Getting Repository Structure > ----------------------//
void Channel::getRepo(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	Message ms = c1.sendMessage(msg);
	pSendr->postMessage(ms);
}
// ------< Function for Getting File list of selected category > ------------//

void Channel::getFilesCategory(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	Message ms = c1.sendMessage(msg);
	pSendr->postMessage(ms);
}
// ------< Function for Download file of selected category> ------------//
void Channel::downloadfile(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	Message ms = c1.sendMessage(msg);
	pSendr->postMessage("File downloaded");
}
// ------< Function for Lazy Downloading dependencies of selected file in selected category> ------------//
void Channel::LazyDownload(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	Message ms = c1.sendMessage(msg);
	pSendr->postMessage("ms");
}

// ------< Function for Deleting selected file > ------------//

void Channel::deleteFile(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	std::string deleteReturn = c1.sendMessage(msg);
	pSendr->postMessage(deleteReturn);
}

// ------< Function for Publishing selected category > ------------//
void Channel::publishCategory(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	Message ms = c1.sendMessage(msg);
	pSendr->postMessage(ms);
}
// ------< Function for Getting File with no parent in selected category> ------------//

void Channel::getNoParent(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	Message ms = c1.sendMessage(msg);
	pSendr->postMessage(ms);
}

// ------< Function for Opening File selected> ------------//

void Channel::OpenHTMLFile(std::string msg, MsgClient& c1, Sendr* pSendr)
{
	Message ms = c1.sendMessage(msg);
	pSendr->postMessage(ms);
}

//----< creates thread to read from sendQ and echo back to the recvQ >-------

void Channel::start()
{
	thread_ = std::thread(
		[this] {
		Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
		Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
		if (pSendr == nullptr || pRecvr == nullptr){
			std::cout << "\n  failed to start Channel\n\n";
			return;
		}
		BQueue& sendQ = pSendr->queue();
		BQueue& recvQ = pRecvr->queue();
		std::string uCategory;
		while (!stop_){
			Message msg = sendQ.deQ();
			recvQ.enQ(msg);
			if (msg.find("setupfolder") != std::string::npos)
				setupFolder(msg, c1, pSendr);
			if (msg.find("DeleteFl") != std::string::npos)
				deleteFile(msg, c1, pSendr);
			if (msg.find("Category;") != std::string::npos){
				std::stringstream commaSep(msg);
				std::string substr;
				while (commaSep.good()){
					getline(commaSep, substr, ';');
					if (substr != "Category")
						uCategory = substr;
			}}
			if (msg.find("Upload,") != std::string::npos){
				std::vector<std::string> filesVector=Utilities::StringHelper::split(msg);
				c1.sendFiles(filesVector, folderPath, uCategory);
				::Sleep(100);}
			if (msg.find("Publish:")!=std::string::npos)
				publishCategory(msg, c1, pSendr);
			if (msg == "GetStructure")
				getRepo(msg, c1, pSendr);
			if (msg.find("CategoryF,") != std::string::npos)
				getFilesCategory(msg, c1, pSendr);
			if (msg.find("DownloadFile,") != std::string::npos){
					Message ms = c1.sendMessage(msg);
					pSendr->postMessage("File downloaded");}
			if (msg.find("DownloadF,") != std::string::npos)
				OpenHTMLFile(msg, c1, pSendr);
			if (msg.find("LazyFile,") != std::string::npos)
				LazyDownload(msg, c1, pSendr);
			if (msg.find("GetNoParent") != std::string::npos)
				getNoParent(msg, c1, pSendr);
		}});
	stop_ = false;
}
// ------------< Function to set folder path which is a private member of Class Channel >------------- 
void Channel::setFolderPath(std::string path)
{
	folderPath = path;
}
//----< signal server thread to stop >---------------------------------------

void Channel::stop() { stop_ = true; }

//----< factory functions >--------------------------------------------------

ISendr* ObjectFactory::createSendr() { return new Sendr; }

IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IChannel* ObjectFactory::createChannel(ISendr* pISendr, IRecvr* pIRecvr)
{
	return new Channel(pISendr, pIRecvr);
}

#ifdef TEST_Channel

//----< test stub >----------------------------------------------------------

int main()
{
	ObjectFactory objFact;
	ISendr* pSendr = objFact.createSendr();
	IRecvr* pRecvr = objFact.createRecvr();
	IChannel* pChannel = objFact.createChannel(pSendr, pRecvr);
	pChannel->start();
	pSendr->postMessage("Hello World");
	pSendr->postMessage("CSE687 - Object Oriented Design");
	Message msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pSendr->postMessage("stopping");
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pChannel->stop();
	pSendr->postMessage("quit");
	std::cin.get();
}
#endif
