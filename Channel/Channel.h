#ifndef Channel_H
#define Channel_H
/////////////////////////////////////////////////////////////////////////////
// Channel.h - Demo for CSE687 Project #4, Spring 2015                     //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - Channel reads from sendQ and writes to recvQ                          //
//                                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015               //
// Changed by Syed Yaser Ahmed Syed										   //
/////////////////////////////////////////////////////////////////////////////

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
using Message = std::string;
struct ISendr
{
	virtual void postMessage(const Message& msg) = 0;
};

struct IRecvr
{
	virtual std::string getMessage() = 0;
};

struct IChannel
{
public:
	virtual void start() = 0;
	virtual void stop() = 0;
};

extern "C" {
	struct ObjectFactory
	{
		DLL_DECL ISendr* createSendr();
		DLL_DECL IRecvr* createRecvr();
		DLL_DECL IChannel* createChannel(ISendr* pISendr, IRecvr* pIRecvr);
	};
}

#endif


