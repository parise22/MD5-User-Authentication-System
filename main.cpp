#include <iostream>
#include <fstream>
#include <string>
#include "md5.h"
#include "function.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

int main(int argc, char *argv[])
{
	MD5 md5;
	string testString = "This is a test";
	char *cstr = &testString[0];
	cout << "***********************************************************" << endl;
	cout << "MD5 ('This is a test') = " << md5.digestString(cstr) << endl;
	cout << "***********************************************************" << endl;

	if(argv[1])
	{	
		string user = "";
		cout << "\nCreate User" << endl;
		cout << "Username: ";
		cin >> user;
		createUser(user);
	}else{
		string user = "";
		cout << "\nLog In" << endl;
		cout << "Username: ";
		cin >> user;
		loggingIn(user);
	} 


	return 0;
}
