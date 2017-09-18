#ifndef WINDOW_H
#define WINDOW_H
///////////////////////////////////////////////////////////////////////////
// Client1.h - C++\CLI implementation of WPF Application                 //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //  
// Changed  by Syed Yaser Ahmed Syed								     //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*		Upload a folder of files with a Browser dialog box to any cateogory selected in the Combo box
*		Publish html files for the selected category in server-repository
*		Get repository structure of the server
*		Get Files present in selected category
*		Download selected file
*		Lazy Download Selected file - gets its dependencies as well
*		Delete a file
*		Open file from server-repository in Chrome
*		Display file which has no parent or no dependencies
*		Setup Server-Repository in the path given in IIS tab's text box
*		Open selected categories index in Chrome from IIS path
*
*
*
*  Required Files:
*  ---------------
*  Client1.h, Client1.cpp, Channel.h, Channel.cpp,
*  Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp
*
*  Build Command:
*  --------------
*  devenv CppCli-WPF-App.sln
*  - this builds C++\CLI client application and native channel DLL
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 2 May 2017
*	- WPF specific to Project#4. It transmits messages to Server using Channel dll
*/

using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;

#include "../Channel/Channel.h"
#include "../Utilities/Utilities.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace CppCliWindows
{
	ref class ClientWPF : Window
	{
		// Channel references

		ISendr* pSendr_;
		IRecvr* pRecvr_;
		IChannel* pChann_;

		// Controls for Window

		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hPublishFilesTab = gcnew TabItem();
		TabItem^ hFileListTab = gcnew TabItem();
		TabItem^ hDownloadTab = gcnew TabItem();
		TabItem^ hIISTab = gcnew TabItem();
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

		// Controls for PublishFiles View
		Grid^ hPublishFilesGrid = gcnew Grid();
		Button^ hGetFilesBtn = gcnew Button();
		Button^ hPublishBtn = gcnew Button();
		TextBlock^ hPublishTextBlock = gcnew TextBlock();
		ListBox^ hpListBox = gcnew ListBox();
		Forms::FolderBrowserDialog^ hPFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		StackPanel^ hStackPanel1 = gcnew StackPanel();
		RowDefinition^ hPRowDef = gcnew RowDefinition();
		RowDefinition^ hPRowDef2 = gcnew RowDefinition();
		ComboBox^ hCategory2 = gcnew ComboBox();


		// Controls for FileListView View
		Grid^ hFileListGrid = gcnew Grid();
		TextBox^ hSavePathtxt = gcnew TextBox();
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		ListBox^ hListBox = gcnew ListBox();
		ListBox^ hListBox2 = gcnew ListBox();
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hShowItemsButton = gcnew Button();
		Grid^ hGrid2 = gcnew Grid();
		ComboBox^ hCategoriesBox = gcnew ComboBox();
		StackPanel^ hStackPanel2 = gcnew StackPanel();

		Grid^ hIISGrid = gcnew Grid();
		TextBox^ hIISSavePathtxt = gcnew TextBox();
		Button^ hPublishIISBtn = gcnew Button();
		StackPanel^ hIISStack = gcnew StackPanel();
		StackPanel^ iStack2 = gcnew StackPanel();
		ComboBox^ iCombo = gcnew ComboBox();

		//Controls for Download View
		Grid^ hDownloadFilesGrid = gcnew Grid();
		Button^ hDGetFilesBtn = gcnew Button();
		Button^ hDownloadBtn = gcnew Button();
		Button^ hGetRepositoryBtn = gcnew Button();
		Button^ hDisplayPageBtn = gcnew Button();
		Button^ deleteBtn = gcnew Button();
		Button^ noParentBtn = gcnew Button();
		
		CheckBox^ hLDownload = gcnew CheckBox();
		CheckBox^ LazyCheck = gcnew CheckBox();
		//Button^ publishInIIS = gcnew Button();

		TextBlock^ hDownloadTextBlock = gcnew TextBlock();
		ListBox^ hdListBox = gcnew ListBox();
		StackPanel^ hStackPanel3 = gcnew StackPanel();
		RowDefinition^ hDRowDef = gcnew RowDefinition();
		RowDefinition^ hDRowDef2 = gcnew RowDefinition();
		ComboBox^ hDCategory = gcnew ComboBox();

		String^ UploadCategory;
		array<String^>^  UploadPatterns;
		String^ PublishCategory;
		array<String^>^  PublishPatterns;
		String^ DownloadCategory;
		array<String^>^  DownloadPatterns;
		String^ iCategory;
		String^ folderPath_;
		// receive thread

		Thread^ recvThread;

	public:
		ClientWPF();
		~ClientWPF();
		void setupEverything();
		void setUpStatusBar();
		void setUpTabControl();
		void setUpPublishFilesView();
		void setUpFileListView();
		void setUpConnectionView();
		void setUpIISView();

		void PublishFiles(Object^ obj, RoutedEventArgs^ args);
		void addText(String^ msg);
		void addtoList(String^ msg);
		void getMessage();
		void SendPublishList(Object^ sender, RoutedEventArgs^ args);
		void getItemsFromList(Object^ sender, RoutedEventArgs^ args);
		void browseForFolder(Object^ sender, RoutedEventArgs^ args);
		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
		void selectCategories(Object^ sender, RoutedEventArgs^ args);
		void selectiCategories(Object^ snder, RoutedEventArgs^ args);
		void iCategories(Object^ sender, SelectionChangedEventArgs^ args);
		void uploadCategories(Object^ sender, SelectionChangedEventArgs^ args);
		void selectPCategories(Object^ sender, RoutedEventArgs^ args);
		void PublishCategories(Object^ sender, SelectionChangedEventArgs^ args);
		void GetStructure(Object^ sender, RoutedEventArgs^ args);
		void DownloadFile(Object^ sender, RoutedEventArgs^ args);
		void DeleteFile(Object^ sender, RoutedEventArgs^ args);
		void GetCFiles(Object^ sender, RoutedEventArgs^ args);
		void DisplayPage(Object^ sender, RoutedEventArgs^ args);
		void setupIIS(Object^ sender, RoutedEventArgs^ args);
		void selectDCategories(Object^ sender, RoutedEventArgs^ args);
		void DownloadCategories(Object^ sender, SelectionChangedEventArgs^ args);
		void getnoParents(Object^ sender, RoutedEventArgs^ args);
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
	private:
		std::string toStdString(String^ pStr);
		String^ toSystemString(std::string& str);
		std::string createMsg(array<String^>^ files,String^ CMD);
		void setTextBlockProperties();
		void setButtonsProperties();
		void setButtonsProperties2();
		void setFileListRows();
		String^ iCategorystr;
		void readCategory(std::string& msg);
		void readCategoryFiles(std::string& msg);
		void FileDownload(std::string& msg);
		void NoParent(std::string& msg);
	};
}


#endif
