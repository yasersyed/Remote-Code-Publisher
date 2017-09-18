--------------------------------------------------------------------------
Remote Code Publisher
--------------------------------------------------------------------------

To Compile - execute compile.bat
To Run - execute run.bat

The following are the functions that the client performs, separated by Tab in which the functions are present.

Upload Tab
-----------
*		Upload a folder of files with a Browser dialog box to any cateogory selected in the Combo box
*		Combo Box provided is by default set to Category1. It can be changed to match Users preference on 
		where to store.
Publish Tab
------------
*		Publish html files for the selected category and uploaded files in server-repository

Download Tab
-------------
*		Get repository structure of the server 		- "Get List" button
*		Get Files present in selected category 		- "Display Files" button
*		Download selected file 		       		- "Download Selected"
*		Lazy Download Selected file 			- If check box is ticked, lazy download will be performed
*		Delete a file					- "Delete Selected" file button
*		Open file from server-repository in Chrome	- "Open Selected file" button
*		Display file which has no parent or no dependencies - "Display No Parent" button

IIS Setup Tab
-------------
*		Setup Server-Repository in the path given in IIS tab's text box - Please provide absolute path of the virtual directory. 
										  This is where the folders and files will be saved

*		Open selected categories index in Chrome from IIS path		- "Publish in IIS"

Again category needs to be selected to send the files to required category folder in IIS, by default it is set to Category1