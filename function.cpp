#include <iostream>
#include <fstream>
#include <string>
#include <ios>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "md5.h"
#include "function.h"

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::string;
using std::puts;
using std::ifstream;
using std::ofstream;
using std::to_string;
using std::ios;
using std::vector;

MD5 md5;
vector<string> buffer;

bool checkExists(string username)
{
	string str = "";
	ifstream file;
	file.open("salt.txt");

	//So Case Sensitivity Is Not Affected
	transform(username.begin(), username.end(), username.begin(), ::tolower);
	
	while(file.good())
	{
		file >> str;
		int colon = str.find(":");
		string user = str.substr(0, colon);

		if (strcmp(username.c_str(), user.c_str()) == 0)
		{
			return true;
		}
	}
	
	return false;

	file.close();

}

int createUser(string username)
{
	string password = ""; 
	string confirm = "";
	int clearance;

	if(checkExists(username) == true)
	{
		cerr << "\nUsername Already Exists. Please Try Another Username." << endl;
	} else{
		cout << "Password: ";
		cin >> password;
		cout << "Confirm Password: ";
		cin >> confirm;

		if (password != confirm)
		{
			cout << "\nThe Passwords You Entered Don't Match. Please Try Again..." << endl;
			createUser(username);
		} else if (password.length() < 6)
		{
			cout << "\nThe Password Must Be Atleast 6 Characters Long. Please Try Again..." << endl;
			createUser(username);
		} else{
			cout << "User Clearance (0 or 1 or 2 or 3): ";
			cin >> clearance;
			if(clearance < 0 || clearance > 3)
			{
				cerr << "You Entered An Invalid Clearance. Please Start Again." << endl;
			}else{

				// Generating Salt and Hash

				string saltValue = generateSalt();
				string passSalt = password + saltValue;

				char *cstr = &passSalt[0];
				string hash = md5.digestString(cstr);

				ofstream saltFile;
				saltFile.open("salt.txt",ios::out|ios::binary|ios::app);
				saltFile << "Username:Salt" << endl;
				saltFile << username << ":" << saltValue << endl;
				saltFile.close();

				ofstream shadowFile;
				shadowFile.open("shadow.txt", ios::out|ios::binary|ios::app);
				shadowFile << "Username:PassSaltHash:SecurityClearance" << endl;
				shadowFile << username << ":" << hash << ":" << clearance << endl;
				shadowFile.close();

				cout << "User Successfully Created." << endl;
			}
			
		}
	}

	return 0;

}

string generateSalt()
{
	int random;
	string randomStr;
	srand(time(NULL));
	random = (double)rand()/(RAND_MAX + 1) * (99999999 - 1) + 1;
	randomStr = to_string(random);
	return randomStr;
}

string getSaltValue(string username)
{
	string str = "";
	string salt = "";
	ifstream file;
	file.open("salt.txt");

	//So Case Sensitivity Is Not Affected
	transform(username.begin(), username.end(), username.begin(), ::tolower);
	
	while(file.good())
	{
		file >> str;

		int colon = str.find(":");
		string user = str.substr(0, colon);
		if(user == username)
		{
			int position = str.find(username);

			//Getting index of Salt from username
			salt = str.substr(position + username.size() + 1);
			
		}
	}

	return salt;

	file.close();
}

int loggingIn(string user)
{

	string password = "";
	string salt = "";
	ifstream saltFile;
	transform(user.begin(), user.end(), user.begin(), ::tolower);

	if(checkExists(user) == false)
	{
		cerr << "The Username Provided Does Not Exist." << endl;
	}else {
		cout << "Password: ";
		cin >> password;
		cout << user << " Found In The Salt.txt" << endl;

		salt = getSaltValue(user);

		cout << "Salt Retrieved: " << salt << endl;

		//Generating PassSaltHash
		string passSalt = password + salt;
		char *cstr = &passSalt[0];
		string hash = md5.digestString(cstr);

		cout << "Hash Value: " << hash << endl;

		if(checkPassSaltHash(hash, user) == true)
		{
			cout << "Authenication For User " << user << " Complete." << endl;
			cout << "The Clearance For " << user << " Is " << getClearance(user) << endl;
			optionsMenu(user);
		}else {
			cerr << "Incorrect Password. FileSystem Has Terminated." << endl;
		}

	}
	return 0;
}

bool checkPassSaltHash(string passSaltHash, string username)
{
	string str = "";
	string textFileHash = "";
	ifstream file;
	file.open("shadow.txt");
	
	while(file.good())
	{
		file >> str;
		int colon = str.find(":");
		string user = str.substr(0, colon);
		if(user == username)
		{
			int position = str.find(username);
			textFileHash = str.substr(position + username.size() + 1, +32);	
		}

		if(textFileHash == passSaltHash)
		{
			return true;
		}
	}
	
	return false;

	file.close();

}

string getClearance(string username)
{
	string str = "";
	string clearance = "";
	ifstream file;
	file.open("shadow.txt");
	
	while(file.good())
	{
		file >> str;
		int colon = str.find(":");
		string user = str.substr(0, colon);
		if(user == username)
		{
			int position = str.find(username);
			clearance = str.substr(position + username.size() + 34);	
		}
	}
	
	return clearance;
	
	file.close();
}

int optionsMenu(string username)
{	
	//Files Store Load
	ifstream fileStore;
	fileStore.open("Files.store");

	cout << "\nOptions: (C)reate, (A)ppend, (R)ead, (W)rite, (L)ist, (S)ave or (E)xit." << endl;
	string user = username;
	string filename = "";
	string input = "";
	string fileClearance = "";
	char choice;
	cin >> input;

	//Prevents Default From Looping When Input Is Larger Than One Character.
	if(input.size() > 1)
	{
		cout << "Please Select Only One Option..." << endl;
		optionsMenu(user);
	}

	choice = input[0];

	switch(choice)
	{
		case 'C':
		case 'c':  
		{
			filename = requestFilename();
			createFile(username, getClearance(username), filename);
			optionsMenu(user);
			break;
		}

		case 'A':
		case 'a': 
		{
			filename = requestFilename();
			if(checkFileExists(filename) == false)
			{
				cout << "File " << filename << " Does Not Exist..." << endl;
				optionsMenu(user);
			}else {
				fileClearance = getFileClearance(filename);
				if(fileClearance >= getClearance(user))
				{
					cout << "Success. You Have The Rights." << endl;
					optionsMenu(user);
				}else {
					cout << "Failure. You Don't Have Authorisation." << endl;
					optionsMenu(user);
				}
			}
			break;
		}

		case 'R':
		case 'r': 
		{
			filename = requestFilename();
			if(checkFileExists(filename) == false)
			{
				cout << "File " << filename << " Does Not Exist..." << endl;
				optionsMenu(user);
			}else {
				fileClearance = getFileClearance(filename);
				if(fileClearance <= getClearance(user))
				{
					cout << "Success. You Have The Rights." << endl;
					optionsMenu(user);
				}else {
					cout << "Failure. You Don't Have Authorisation." << endl;
					optionsMenu(user);
				}
			}
			break;
		}

		case 'W':
		case 'w': 
		{
			filename = requestFilename();
			if(checkFileExists(filename) == false)
			{
				cout << "File " << filename << " Does Not Exist..." << endl;
				optionsMenu(user);
			}else {
				fileClearance = getFileClearance(filename);
				if(fileClearance == getClearance(user))
				{
					cout << "Success. You Have The Rights." << endl;
					optionsMenu(user);
				}else {
					cout << "Failure. You Don't Have Authorisation." << endl;
					optionsMenu(user);
				}
			}
			break;
		}

		case 'L':
		case 'l': 
		{
			listAllFiles();
			optionsMenu(user);
			break;
		}

		case 'S':
		case 's': 
		{
			ofstream FileStore;
			FileStore.open("Files.store",ios::out|ios::binary|ios::app);
			
			for(int i = 0; i < buffer.size(); i++)
			{
				FileStore << endl;
				FileStore << buffer[i];
			}
			FileStore.close();
			cout << "File/s Saved" << endl;
			buffer.clear();
			optionsMenu(user);
			break;
		}

		case 'E':
		case 'e': 
		{
			cout << "Shut Down FileSystem? (Y)es or (N)o ";
			string shutDownOption;
			char yORn = '\0';
			cin >> shutDownOption;
			if(shutDownOption.size() > 1)
			{
				cout << "Please Select One Option..." << endl;
			}
			yORn = shutDownOption[0];
			if(yORn == 'Y' || yORn =='y')
			{
				buffer.clear();
				fileStore.close();
				cerr << "Shut Down Complete." << endl;
				break;
			}else if(yORn == 'N' || yORn =='n')
			{
				optionsMenu(user);
			}else {
				cout << "Invalid Option..." << endl;
				optionsMenu(user);
			}
			break;
		}

		default:
		{
			cout << "Invalid Option. Please Try Again..." << endl;
			optionsMenu(user);
		}
	}

	return 0;
}

string requestFilename()
{
	string filename = "";
	cout << "Filename: ";
	cin >> filename;

	return filename;
}

int createFile(string user, string clearance, string fileName)
{
	//Loading Files Store.
	ifstream fileStore;
	fileStore.open("Files.store");

	string toBePushed = "";
	string storeFile = "";
	string str = "";
	while(fileStore.good())
	{
		fileStore >> str;
		int colon = str.find(":");
		string storeFile = str.substr(0, colon);
		if(storeFile == fileName)
		{
			cout << "File " << fileName << " Already Exists. Please Try A New File Name..." << endl;	
			return 1;
		}else{
			//File Store format is FileName:Owner:Clearance
			toBePushed = fileName + ":" + user + ":" + clearance;
		}
	}

	if(toBePushed != "")
	{
		buffer.push_back(toBePushed);
		cout << fileName << " Added To List" << endl;
	}

	fileStore.close();
	return 0;
}

int listAllFiles()
{
	cout << "\nAll Files In The File System..." << endl;
	ifstream fileStore;
	fileStore.open("Files.store");

	string str = "";
	while(fileStore.good())
	{
		fileStore >> str;
		int colon = str.find(":");
		string file = str.substr(0, colon);
		cout << file << endl;
	}

	fileStore.close();
	return 0;
}

bool checkFileExists(string fileName)
{
	ifstream fileStore;
	fileStore.open("Files.store");

	string storeFile = "";
	string str = "";

	while(fileStore.good())
	{
		fileStore >> str;
		int colon = str.find(":");
		string storeFile = str.substr(0, colon);

		if(storeFile == fileName)
		{
			cout << "File Found." << endl;
			return true;
		}
	}

	fileStore.close();
	return false;

	
}

string getFileClearance(string fileName)
{
	ifstream fileStore;
	fileStore.open("Files.store");

	string storeFile = "";
	string str = "";
	string clearance = "";
	string buffer = "";
	string buffer2 = "";

	while(fileStore.good())
	{
		fileStore >> str;
		int colon = str.find(":");
		storeFile = str.substr(0, colon);
		if(storeFile == fileName)
		{
			buffer = str;
		}
	}
		int colonOut = buffer.find(":");
		buffer2 = buffer.substr(colonOut +1);
		int colonOut2 = buffer2.find(":");
		clearance = buffer2.substr(colonOut2 +1);

	fileStore.close();
	return clearance;

}

/*
0- write 0 read 0 append 0,1,2,3 
1- write 1 read 0,1 append 1,2,3
2- write 2 read 0,1,2 append 2,3
3- write 3 read 0,1,2,3 append 3
*/