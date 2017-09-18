///////////////////////////////////////////////////////////////////////////
// Client1.cpp - C++\CLI implementation of WPF Application               //
//										                                 //
// ver 2.0                                                               //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //
// Changed By: Syed Yaser Ahmed Syed									 //
///////////////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*/
#include "Client1.h"

using namespace CppCliWindows;
std::string categoryStr = "NoCategory";
std::string pcategoryStr = "NoCategory";
std::string dcategoryStr = "NoCategory";

// ----< ClientWPF constructor calls for the setting up GUI window >-----------
ClientWPF::ClientWPF()
{
	std::cout << "\n Starting Channel" << std::endl;
	ObjectFactory* pObjFact = new ObjectFactory;
	pSendr_ = pObjFact->createSendr();
	pRecvr_ = pObjFact->createRecvr();
	pChann_ = pObjFact->createChannel(pSendr_, pRecvr_);
	pChann_->start();
	delete pObjFact;
	recvThread = gcnew Thread(gcnew ThreadStart(this, &ClientWPF::getMessage));
	recvThread->Start();
	this->Loaded +=gcnew System::Windows::RoutedEventHandler(this, &ClientWPF::OnLoaded);
	this->Closing +=gcnew CancelEventHandler(this, &ClientWPF::Unloading);
	hGetFilesBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::PublishFiles);
	hPublishBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::SendPublishList);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::browseForFolder);
	hShowItemsButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::getItemsFromList);
	hCategoriesBox->SelectionChanged += gcnew SelectionChangedEventHandler(this, &ClientWPF::uploadCategories);
	hCategoriesBox->Loaded += gcnew RoutedEventHandler(this, &ClientWPF::selectCategories);
	iCombo->SelectionChanged += gcnew SelectionChangedEventHandler(this, &ClientWPF::iCategories);
	iCombo->Loaded += gcnew RoutedEventHandler(this, &ClientWPF::selectiCategories);
	hCategory2->SelectionChanged += gcnew SelectionChangedEventHandler(this, &ClientWPF::PublishCategories);
	hCategory2->Loaded += gcnew RoutedEventHandler(this, &ClientWPF::selectPCategories);
	hDCategory->SelectionChanged += gcnew SelectionChangedEventHandler(this, &ClientWPF::DownloadCategories);
	hDCategory->Loaded += gcnew RoutedEventHandler(this, &ClientWPF::selectDCategories);
	hDGetFilesBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::GetStructure);
	hDownloadBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::DownloadFile);
	hGetRepositoryBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::GetCFiles);
	hDisplayPageBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::DisplayPage);
	hPublishIISBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::setupIIS);
	deleteBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::DeleteFile);
	noParentBtn->Click += gcnew RoutedEventHandler(this, &ClientWPF::getnoParents);
	this->Title = "Client1";
	this->Width = 800;
	this->Height = 600;
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);
	setupEverything();
}
//--------------< Destructor removes channel, sender and receiver thread >-----------
ClientWPF::~ClientWPF()
{
	delete pChann_;
	delete pSendr_;
	delete pRecvr_;
}
//----------------< Calls functions to setup every tab and status bar >--------------
void CppCliWindows::ClientWPF::setupEverything()
{
	setUpTabControl();
	setUpStatusBar();
	setUpPublishFilesView();
	setUpFileListView();
	setUpConnectionView();
	setUpIISView();
}
//----------------< function for setting up IIS tab, setting properties for its various buttons >--------------
void ClientWPF::setUpIISView()
{
	hIISGrid->Margin = Thickness(20);
	hIISGrid->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hIISTab->Content = hIISGrid;
	hIISStack->Height = 100;
	hIISStack->Width = 400;
	RowDefinition^ iRowDef = gcnew RowDefinition(); //File list row
	hIISGrid->RowDefinitions->Add(iRowDef);
	RowDefinition^ iRowDef2 = gcnew RowDefinition();
	hIISGrid->RowDefinitions->Add(iRowDef2);
	TextBlock^ pathBlock = gcnew TextBlock();
	pathBlock->Padding = Thickness(5);
	pathBlock->Text = "Note: IIS server Absolute Path (Changeable), for TAs to setup";
	pathBlock->FontWeight = FontWeights::Bold;
	hIISStack->Children->Add(pathBlock);
	
	hSavePathtxt->Padding = Thickness(5);
	hIISStack->Children->Add(hSavePathtxt);

	TextBlock^ spaceri = gcnew TextBlock();
	spaceri->Height = 10;
	hIISStack->Children->Add(spaceri);

	iStack2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	iStack2->Height = 100;
	iStack2->Width = 400;
	iCombo->Width = 100;
	iStack2->Children->Add(iCombo);
	TextBlock^ spaceri2 = gcnew TextBlock();
	spaceri2->Width = 5;
	iStack2->Children->Add(spaceri2);
	hPublishIISBtn->Content = "Publish in IIS";
	hPublishIISBtn->Padding = Thickness(5);
	hPublishIISBtn->Width = 100;
	iStack2->Children->Add(hPublishIISBtn);

	hIISGrid->SetRow(hIISStack, 0);
	hIISGrid->Children->Add(hIISStack);
	hIISGrid->SetRow(iStack2, 1);
	hIISGrid->Children->Add(iStack2);
	hSavePathtxt->Text = "C:\\SU\\temp\\CodePages\\";
}
//----------------< function for setting up Status bar which will be changed to display messages >--------------
void ClientWPF::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "Please check here for Status messages";
	//status->FontWeight = FontWeights::Bold;
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}
//----------------< function for setting up tabs and their headers >--------------
void ClientWPF::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hPublishFilesTab->Header = "Publish";
	hFileListTab->Header = "Upload";
	hDownloadTab->Header = "Download";
	hIISTab->Header = "IIS Setup";
	hTabControl->Items->Add(hFileListTab);
	hTabControl->Items->Add(hPublishFilesTab);
	hTabControl->Items->Add(hDownloadTab);
	hTabControl->Items->Add(hIISTab);
}
//----------------< function for setting up ListBox for Download Tab >--------------
void ClientWPF::setTextBlockProperties()
{
	hStackPanel1->Children->Add(hGetFilesBtn);

	hPublishFilesGrid->SetRow(hStackPanel1, 1);

	array<String^>^ plist = { "Category1", "Category2","Category3","NoCategory" };
	for (int i = 0; i < plist->Length; ++i)
		hpListBox->Items->Add(plist[i]);
}

//----------------< function for setting up Download, setting properties for its various buttons >--------------
void CppCliWindows::ClientWPF::setFileListRows()
{
	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 10;
	hStackPanel2->Children->Add(hFolderBrowseButton);
	hStackPanel2->Children->Add(hSpacer1);

	hCategoriesBox->Height = 30;
	hCategoriesBox->Width = 120;
	hCategoriesBox->Padding = Thickness(5);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 10;
	hStackPanel2->Children->Add(hCategoriesBox);
	hStackPanel2->Children->Add(hSpacer2);

	// Show selected items button.
	hShowItemsButton->Content = "Upload Folder";
	hShowItemsButton->Height = 30;
	hShowItemsButton->Width = 120;
	hShowItemsButton->BorderThickness = Thickness(2);
	hShowItemsButton->BorderBrush = Brushes::Black;
	hStackPanel2->Children->Add(hShowItemsButton);

	hFileListGrid->SetRow(hStackPanel2, 4);
	hFileListGrid->Children->Add(hStackPanel2);

	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}
//----------------< function to change listbox >--------------
void CppCliWindows::ClientWPF::readCategory(std::string& msg)
{
	Console::Write("\n Categories received \n");
	String^ sMsg = toSystemString(msg);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	std::vector<std::string> files = Utilities::StringHelper::split(msg);
	for each(std::string substr in files)
	{
		args[0] = toSystemString(substr);
		Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addtoList);
		Dispatcher->Invoke(act, args);
	}
}
//----------------< This reads incoming category files request and adds to the ListBox in download tab >--------------
void CppCliWindows::ClientWPF::readCategoryFiles(std::string& msg)
{
	Console::Write("\n Category Files received \n");
	String^ sMsg = toSystemString(msg);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	std::vector<std::string> files = Utilities::StringHelper::split(msg);
	for each(std::string substr in files)
	{
		if (substr != "CategoryFiles" && substr != ":")
		{
			args[0] = toSystemString(substr);
			Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addtoList);
			Dispatcher->Invoke(act, args);
		}
	}
}
//----------------< To read incoming file downloaded message and changing status bar to reflect status >--------------
void CppCliWindows::ClientWPF::FileDownload(std::string& msg)
{
	String^ sMsg = toSystemString(msg);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	args[0] = toSystemString(msg);
	Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addText);
	Dispatcher->Invoke(act, args);
}
//--------------< Adds No parent files in the listbox >----------------
void CppCliWindows::ClientWPF::NoParent(std::string& msg)
{
	Console::Write("\n Files with no parents received \n");
	String^ sMsg = toSystemString(msg);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	std::vector<std::string> files = Utilities::StringHelper::split(msg);
	
	for each(std::string substr in files)
	{
		if (substr != "NoPrnt" && substr != "NoParent" && substr != "" && substr!="ParentFiles")
		{
			args[0] = toSystemString(substr);
			Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addtoList);
			Dispatcher->Invoke(act, args);
		}
	}
}
//----------------< function for setting up Publish tab, setting properties for its various buttons >--------------
void ClientWPF::setUpPublishFilesView()
{
	Console::Write("\n  setting up PublishFiles view \n");
	hPRowDef->Height = GridLength(300);
	hPRowDef2->Height = GridLength(100);

	hPublishFilesGrid->Margin = Thickness(20);
	hPublishFilesTab->Content = hPublishFilesGrid;
	hPublishFilesGrid->Children->Add(hStackPanel1);
	hPublishFilesGrid->RowDefinitions->Add(hPRowDef);
	hPublishFilesGrid->RowDefinitions->Add(hPRowDef2);

	Border^ hPBorder1 = gcnew Border();
	hPBorder1->BorderThickness = Thickness(1);
	hPBorder1->BorderBrush = Brushes::Black;
	hPBorder1->Height = 300;
	hPBorder1->Width = 500;
	hpListBox->SelectionMode = SelectionMode::Single;
	hpListBox->Height = 300;
	hpListBox->Width = 500;
	hPBorder1->Child = hpListBox;
	hPublishFilesGrid->SetRow(hPBorder1, 0);
	hPublishFilesGrid->Children->Add(hPBorder1);

	hGetFilesBtn->Content = "Publish Selected Category";
	hGetFilesBtn->Height = 30;
	hGetFilesBtn->Width = 200;
	hGetFilesBtn->BorderThickness = Thickness(2);
	hGetFilesBtn->BorderBrush = Brushes::Black;
	
	hStackPanel1->Orientation = Orientation::Horizontal;
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;//::Center for centralizing

	setTextBlockProperties();
}
//----------------< This converts System String^ to std::string >--------------
std::string ClientWPF::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}
//----------------< function for sending publish message >--------------
void ClientWPF::PublishFiles(Object^ obj, RoutedEventArgs^ args)
{
	std::cout << "\n  Publishing Selected Category";

	int index = 0;
	int count = hpListBox->SelectedItems->Count;

	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hpListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}
	std::string publishmsg = createMsg(items, "Publish:");
	pSendr_->postMessage(publishmsg);
}
//----------------< This converts std::string to System String^ >--------------
String^ ClientWPF::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}
//----------------< function for creating a comma separated file >--------------
std::string CppCliWindows::ClientWPF::createMsg(array<String^>^ files,String^ command)
{
	std::string publish = toStdString(command) + ",";
	for each(String^ item in files)
	{
		publish += toStdString(item) +",";
	}
	return publish;
}
//----------------< function for adding given text to the status bar >--------------
void ClientWPF::addText(String^ msg)
{
	hStatus->Text = msg;
}
//----------------< Adds Items to ListBox in Download tab >--------------
void CppCliWindows::ClientWPF::addtoList(String ^ msg)
{
	if (msg != "Category" && msg != ":" && msg!="No Parent")
	{
		hdListBox->Items->Add(msg);
	}
}
static std::string somrt="";
//----------------< receive thread runs this function, and will parse any incoming messages to act accordingly >--------------
void ClientWPF::getMessage()
{
	while (true)
	{
		std::string msg = pRecvr_->getMessage();
		String^ sMsg = toSystemString(msg);
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = sMsg;
		if (msg.find("Category : ") != std::string::npos)
			readCategory(msg);
		if (msg.find("CategoryFiles : ") != std::string::npos)
			readCategoryFiles(msg);
		if (msg == "File downloaded")
			FileDownload(msg);
		if (msg.find("NoPrnt") != std::string::npos)
			NoParent(msg);
		if (msg == "ms")
		{
			std::string m = "Relevant Files Downloaded, please check Client-Repository";
			args[0] = toSystemString(m);
			Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addText);
			Dispatcher->Invoke(act, args);
		}
		if (msg == "IIS setup")
		{
			args[0] = toSystemString(msg);
			Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addText);
			Dispatcher->Invoke(act, args);
		}
		if (msg.find("Downloaded,") != std::string::npos)
		{
			std::cout << "\n File Downloaded\n";
			std::vector<std::string> files = Utilities::StringHelper::split(msg);
			for each(std::string substr in files)
			{
				if (substr != "Downloaded")
				{
					args[0] = toSystemString(substr);
					Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addText);
					Dispatcher->Invoke(act, args);
				}
			}
		}
		if (msg == "No files present in selected category!")
		{
			Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addText);
			Dispatcher->Invoke(act, args);
		}
	}
}
//----------------< function for clearing Publish text block, not used >--------------
void ClientWPF::SendPublishList(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  cleared message text");
	hStatus->Text = "Cleared message";
	hPublishTextBlock->Text = "";
}

//----------------< function for sending category file and upload request >--------------
void ClientWPF::getItemsFromList(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n Project 4- Requirement 5: provided a Client program that can upload files and can view repository contents \n";
	std::cout << "\n Project 4- Requirement 6: Message from MsgClient to Project Server are sent through sockets \n ";
	std::cout << "\n Project 4- Requirement 7: Messages sent are in the form of HttpMessages as you can see in Server application \n ";
	std::cout << "\n Project 4 - Requirement 8: Server and client received/Send files as bytes of stream with the help of communication system";
	int index = 0;
	int count = hListBox->SelectedItems->Count;
	hStatus->Text = "Sending files in folder";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}
	if (folderPath_ != nullptr && folderPath_ !="")
	{
		array<String^>^ hfiles = System::IO::Directory::GetFiles(folderPath_, L"*.*");
		std::string categoryMsg = "Category;" + categoryStr;
		pSendr_->postMessage(categoryMsg);
		std::string Message = createMsg(hfiles, "Upload");
		pSendr_->postMessage(Message);
	}
	else
	{
		std::cout << "Sending Files Failed: Please select folder to send!\n";
	}
}
//----------------< function for setting up File List Tab >--------------
void ClientWPF::setUpFileListView()
{
	Console::Write("\n  setting up FileList view");
	
	hFileListGrid->Margin = Thickness(20);

	hFileListGrid->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	//Adding content to Tab
	hGrid2->Children->Add(hFileListGrid);

	hFileListTab->Content = hGrid2;

	RowDefinition^ hRow1Def = gcnew RowDefinition(); //File list row
	hFileListGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Height = 300;
	hBorder1->Width = 500;
	hListBox->SelectionMode = SelectionMode::Multiple;
	hListBox->Height =300;
	hListBox->Width = 500;
	hBorder1->Child = hListBox;
	hFileListGrid->SetRow(hBorder1, 0);
	hFileListGrid->Children->Add(hBorder1);
	hStackPanel2->Orientation = Orientation::Horizontal;
	hStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;//::Center for centralizing

	RowDefinition^ hRow2Def = gcnew RowDefinition(); //Text block row
	hRow2Def->Height = GridLength(30);
	RowDefinition^ hRow2Def3 = gcnew RowDefinition(); //button row
	hRow2Def3->Height = GridLength(30);
	RowDefinition^ hRow2Def2 = gcnew RowDefinition(); //Text box row
	hRow2Def2->Height = GridLength(30);
	RowDefinition^ hRow2Def5 = gcnew RowDefinition();
	hRow2Def5->Height = GridLength(30);
	hFileListGrid->RowDefinitions->Add(hRow2Def);
	hFileListGrid->RowDefinitions->Add(hRow2Def2);
	hFileListGrid->RowDefinitions->Add(hRow2Def3);
	hFileListGrid->RowDefinitions->Add(hRow2Def5);

	hFolderBrowseButton->Content = "Select Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	setFileListRows();
}
//----------------< function for handling browse dialog box and getting path from which to upload >--------------
void ClientWPF::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	hListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		//Add uploadPatterns in argument to select categories
		array<String^>^ files= System::IO::Directory::GetFiles(path, L"*.*");

		folderPath_ = hFolderBrowserDialog->SelectedPath;

		for (int i = 0; i < files->Length; ++i)
			hListBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hListBox->Items->Add(L"<> " + dirs[i]);
	}
}
//----------------< function for setting up Download Tab >--------------
void ClientWPF::setUpConnectionView()
{
	Console::Write("\n  setting up Download view");
	hDRowDef->Height = GridLength(300);
	hDRowDef2->Height = GridLength(100);

	hDownloadFilesGrid->Margin = Thickness(20);
	hDownloadTab->Content = hDownloadFilesGrid;
	hDownloadFilesGrid->Children->Add(hStackPanel3);
	hDownloadFilesGrid->RowDefinitions->Add(hDRowDef);
	hDownloadFilesGrid->RowDefinitions->Add(hDRowDef2);

	Border^ hDBorder1 = gcnew Border();
	hDBorder1->BorderThickness = Thickness(1);
	hDBorder1->BorderBrush = Brushes::Black;
	hDBorder1->Height = 300;
	hDBorder1->Width = 500;
	hdListBox->SelectionMode = SelectionMode::Single;
	hdListBox->Height = 300;
	hdListBox->Width = 500;
	hDBorder1->Child = hdListBox;
	hDownloadFilesGrid->SetRow(hDBorder1, 0);
	hDownloadFilesGrid->Children->Add(hDBorder1);

	hDGetFilesBtn->Content = "Get List";
	hDGetFilesBtn->Height = 30;
	hDGetFilesBtn->Width = 120;
	hDGetFilesBtn->BorderThickness = Thickness(2);
	hDGetFilesBtn->BorderBrush = Brushes::Black;

	hStackPanel3->Orientation = Orientation::Horizontal;
	hStackPanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	setButtonsProperties();
}
//----------------< function that is called to setup button properties by the Download Tab>--------------
void ClientWPF::setButtonsProperties()
{
	TextBlock^ hDSpacer1 = gcnew TextBlock();
	hDSpacer1->Width = 5;
	hStackPanel3->Children->Add(hDGetFilesBtn);
	hStackPanel3->Children->Add(hDSpacer1);
	hDCategory->Height = 30;
	hDCategory->Width = 120;
	hDCategory->Padding = Thickness(5);
	TextBlock^ hdSpacer2 = gcnew TextBlock();
	hdSpacer2->Width = 5;
	hStackPanel3->Children->Add(hDCategory);
	hStackPanel3->Children->Add(hdSpacer2);
	hDownloadBtn->Content = "Download Selected";
	hDownloadBtn->Height = 30;
	hDownloadBtn->Width = 120;
	hDownloadBtn->BorderThickness = Thickness(2);
	hDownloadBtn->BorderBrush = Brushes::Black;
	hStackPanel3->Children->Add(hDownloadBtn);
	hdSpacer2 = gcnew TextBlock();
	hdSpacer2->Width = 5;
	hStackPanel3->Children->Add(hdSpacer2);
	LazyCheck->Content = "Lazy Download";
	LazyCheck->VerticalAlignment = System::Windows::VerticalAlignment::Center;
	hStackPanel3->Children->Add(LazyCheck);
	hdSpacer2 = gcnew TextBlock();
	hdSpacer2->Width = 5;
	hStackPanel3->Children->Add(hdSpacer2);
	hGetRepositoryBtn->Content = "Display Files";
	hGetRepositoryBtn->Height = 30;
	hGetRepositoryBtn->Width = 120;
	hGetRepositoryBtn->BorderThickness = Thickness(2);
	hGetRepositoryBtn->BorderBrush = Brushes::Black;
	hStackPanel3->Children->Add(hGetRepositoryBtn);
	hdSpacer2 = gcnew TextBlock();
	hdSpacer2->Width = 5;
	hStackPanel3->Children->Add(hdSpacer2);
	hDisplayPageBtn->Content = "Open selected File";
	hDisplayPageBtn->Height = 30;
	hDisplayPageBtn->Width = 120;
	hDisplayPageBtn->BorderThickness = Thickness(2);
	hDisplayPageBtn->BorderBrush = Brushes::Black;
	hStackPanel3->Children->Add(hDisplayPageBtn);
	hDownloadFilesGrid->SetRow(hStackPanel3, 1);
	RowDefinition^ deleteRow = gcnew RowDefinition();
	hDownloadFilesGrid->RowDefinitions->Add(deleteRow);
	setButtonsProperties2();
}
//----------------< function for setting buttons for Download View >--------------
void CppCliWindows::ClientWPF::setButtonsProperties2()
{
	StackPanel^ lastStack = gcnew StackPanel();
	lastStack->Orientation = Orientation::Horizontal;
	lastStack->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	noParentBtn->Content = "Display No Parent";
	noParentBtn->Width = 100;
	noParentBtn->Height = 30;
	TextBlock^ hdSpacer2 = gcnew TextBlock();
	hdSpacer2->Width = 15;
	deleteBtn->Content = "Delete Selected";
	deleteBtn->Height = 30;
	deleteBtn->Width = 100;
	lastStack->Children->Add(noParentBtn);
	lastStack->Children->Add(hdSpacer2);
	lastStack->Children->Add(deleteBtn);
	hDownloadFilesGrid->SetRow(lastStack, 2);
	hDownloadFilesGrid->Children->Add(lastStack);
}
//----------------< Defines what to print during Window loading >--------------
void ClientWPF::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}
//----------------< function for category select in Upload tab >--------------
void CppCliWindows::ClientWPF::selectCategories(Object ^ sender, RoutedEventArgs ^ args)
{
	array<String^>^ Categories = {"Category 1", "Category 2","Category 3","No Category"};
	hCategoriesBox->ItemsSource = Categories;
	hCategoriesBox->SelectedIndex = 0;
}
//----------------< function for category select in IIS tab >--------------
void CppCliWindows::ClientWPF::selectiCategories(Object ^ snder, RoutedEventArgs ^ args)
{
	array<String^>^ Categories = { "Category 1", "Category 2","Category 3","No Category" };
	iCombo->ItemsSource = Categories;
	iCombo->SelectedIndex = 0;
}
//---------------< fucntion that defines what happens when Category is changed in IIS tab >---------------
void CppCliWindows::ClientWPF::iCategories(Object ^ sender, SelectionChangedEventArgs ^ args)
{
	iCategory = iCombo->SelectedItem->ToString();
	if (iCategory == "Category 1") {
		iCategorystr = "Category1";
		iCombo->SelectedIndex = 0;
	}
	else if (iCategory == "Category 2") {
		iCategorystr = "Category2";
		iCombo->SelectedIndex = 1;
	}
	else if (iCategory == "Category 3") {
		iCategorystr = "Category3";
		iCombo->SelectedIndex = 2;
	}
	else
	{
		iCategorystr = "NoCategory";
		iCombo->SelectedIndex = 3;
	}
}
//---------------< fucntion that defines what happens when Category is changed in Upload tab >---------------
void CppCliWindows::ClientWPF::uploadCategories(Object ^ sender, SelectionChangedEventArgs ^ args)
{
	UploadCategory = hCategoriesBox->SelectedItem->ToString();
	if (UploadCategory == "Category 1") {
		categoryStr = "Category1";
		hCategoriesBox->SelectedIndex = 0;
	}
	else if (UploadCategory == "Category 2") {
		categoryStr = "Category2";
		hCategoriesBox->SelectedIndex = 1;
	}
	else if (UploadCategory == "Category 3") {
		categoryStr = "Category3";
		hCategoriesBox->SelectedIndex = 2;
	}
	else
	{
		categoryStr = "NoCategory";
		hCategoriesBox->SelectedIndex = 3;
	}
}
//----------------< function for category select in Publish tab >--------------
void CppCliWindows::ClientWPF::selectPCategories(Object ^ sender, RoutedEventArgs ^ args)
{
	array<String^>^ Categories = { "Category 1", "Category 2","Category 3","No Category" };
	hCategory2->ItemsSource = Categories;
	hCategory2->SelectedIndex = 0;
}
//---------------< fucntion that defines what happens when Category is changed in Publish tab >---------------
void CppCliWindows::ClientWPF::PublishCategories(Object ^ sender, SelectionChangedEventArgs ^ args)
{
	PublishCategory = hCategoriesBox->SelectedItem->ToString();
	if (PublishCategory == "Category 1") {
		pcategoryStr = "Category1";
		hCategory2->SelectedIndex = 0;
	}
	else if (PublishCategory == "Category 2") {
		pcategoryStr = "Category2";
		hCategory2->SelectedIndex = 1;
	}
	else if (PublishCategory == "Category 3") {
		pcategoryStr = "Category3";
		hCategory2->SelectedIndex = 2;
	}
	else
	{
		pcategoryStr = "NoCategory";
		hCategory2->SelectedIndex = 3;
	}
}
//---------------< fucntion that posts get repository structure message to the channel >---------------
void CppCliWindows::ClientWPF::GetStructure(Object ^ sender, RoutedEventArgs ^ args)
{
	//Get Directory structure
	std::string msg = "GetStructure";
	pSendr_->postMessage(msg);

	hdListBox->Items->Clear();

}
//---------------< fucntion that posts get download file message to the channel >---------------
void CppCliWindows::ClientWPF::DownloadFile(Object ^ sender, RoutedEventArgs ^ args)
{
	std::string msg="DownloadFile,";

	if (LazyCheck->IsChecked)
	{
		msg = "LazyFile,";
		std::cout << "\n Project 4 - Bonus Requirement 10: Lazy Downloading files with their children and their grandchildren \n";
	}

	int index = 0;
	int count = hdListBox->SelectedItems->Count;
	hStatus->Text = "Downloading Selected files";
	if (count > 0) {
		for each (String^ item in hdListBox->SelectedItems)
		{
			msg += dcategoryStr + "//" + toStdString(item);
		}
	}
	pSendr_->postMessage(msg);
}
//---------------< fucntion that posts delete file message to the channel >---------------
void CppCliWindows::ClientWPF::DeleteFile(Object ^ sender, RoutedEventArgs ^ args)
{
	std::string msg="DeleteFl,";
	int index = 0;
	int count = hdListBox->SelectedItems->Count;
	hStatus->Text = "Deleting Selected files";
	if (count > 0) {
		for each (String^ item in hdListBox->SelectedItems)
		{
			msg += dcategoryStr + "//" + toStdString(item);
		}
	}
	pSendr_->postMessage(msg);
}
//---------------< fucntion that posts Get category files message to the channel >---------------
void CppCliWindows::ClientWPF::GetCFiles(Object ^ sender, RoutedEventArgs ^ args)
{
	hdListBox->Items->Clear();
	std::string msg = "CategoryF," + dcategoryStr;
	pSendr_->postMessage(msg);
}
//---------------< fucntion that posts open web page message to the channel >---------------
void CppCliWindows::ClientWPF::DisplayPage(Object ^ sender, RoutedEventArgs ^ args)
{
	int index = 0;
	int count = hdListBox->SelectedItems->Count;
	hStatus->Text = "Opening Selected files";
	if (count > 0) {
		for each (String^ item in hdListBox->SelectedItems)
		{
			//items[index++] = item;
			pSendr_->postMessage("DownloadF," + dcategoryStr+" : "+toStdString(item));
		}
	}
}
//---------------< fucntion that posts IIS setup message to the channel >---------------
void CppCliWindows::ClientWPF::setupIIS(Object ^ sender, RoutedEventArgs ^ args)
{
	std::cout << "\n Bonus Requirement 11: Server will publish Selected Category in the path given in Publish tab text box \n";
	std::cout << " Please provide absolute path \n";
	std::string IISFolder = "setupfolder,"+toStdString(hSavePathtxt->Text)+toStdString(iCategorystr) + "\\";
	pSendr_->postMessage(IISFolder);
}
//---------------< fucntion that fills combo box in Download tab >---------------
void CppCliWindows::ClientWPF::selectDCategories(Object ^ sender, RoutedEventArgs ^ args)
{
	array<String^>^ Categories = { "Category 1", "Category 2","Category 3","No Category" };
	hDCategory->ItemsSource = Categories;
	hDCategory->SelectedIndex = 0;
}
//---------------< fucntion that defines what happens when Category is changed in Download tab >---------------
void CppCliWindows::ClientWPF::DownloadCategories(Object ^ sender, SelectionChangedEventArgs ^ args)
{
	DownloadCategory = hDCategory->SelectedItem->ToString();
	if (DownloadCategory == "Category 1") {
		dcategoryStr = "Category1";
		hDCategory->SelectedIndex = 0;
	}
	else if (DownloadCategory == "Category 2") {
		dcategoryStr = "Category2";
		hDCategory->SelectedIndex = 1;
	}
	else if (DownloadCategory == "Category 3") {
		dcategoryStr = "Category3";
		hDCategory->SelectedIndex = 2;
	}
	else
	{
		dcategoryStr = "NoCategory";
		hDCategory->SelectedIndex = 3;
	}
}
//---------------< fucntion that posts get files with no parent message to the channel >---------------
void CppCliWindows::ClientWPF::getnoParents(Object ^ sender, RoutedEventArgs ^ args)
{
	hdListBox->Items->Clear();
	std::string msg = "GetNoParent,"+dcategoryStr;
	pSendr_->postMessage(msg);
}
//------------------< Defines what happens when window is closed >-------------------------------
void ClientWPF::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	Console::WriteLine(L"\n Starting ClientWPF");

	Application^ app = gcnew Application();
	app->Run(gcnew ClientWPF());
	Console::WriteLine(L"\n\n");
}