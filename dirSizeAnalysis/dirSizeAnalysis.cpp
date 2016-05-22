// dirSizeAnalysis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <io.h>
#include <iomanip>

using namespace std;

class file // class for holding name and size information about a file
{
public:
	file(string path) // constructor - requires path to the file to be analyzed
	{
		struct _finddata_t files; // create find struct
		intptr_t hFile; // create file index
		fileSize = 0;
		
		if ( (hFile = _findfirst(path.c_str(), &files)) == -1L) // set ref to file path passed, fail if file doesn't exist
		{
			cout << "File not found!" << endl;
		}
		else
		{
			fileSize = files.size; // set fileSize to the size from the file at the location passed
			//cout << "File \"" << files.name << "\" is: " << files.size << " bytes." << endl; // debug print
		}
		_findclose(hFile); // close file index handle
		return;
	}
	unsigned long long size() { return fileSize; }
	string name() { return fileName; }
private:
	string fileName;
	unsigned long long fileSize;
};

class dir // class for holding name and size information about directory, as well as pointers to its children
{
public:
	dir(string path) // constructor for the root directory - set path to the passed arg
	{
		dirName = path;
		dirSize = 0;
		return;
	}
	void populate() // Recursively builds the directory tree starting from the current objects path, calculating the size of each directory on the fly
	{
		unsigned long long sizeTemp = 0; // create temporary size variable, to be used while calculating directory sizes
		struct _finddata_t files; // create find struct
		string fileName = dirName + "\\*.*"; // get ready to find the first file in current directory
		intptr_t hFile; // create file index

		if( (hFile = _findfirst(fileName.c_str(), &files)) == -1L) // find the first file or directory in the current dir
		{
			cout << "Directory not found, or access denied." << endl;
		}
		else
		{
			do
			{
				if((files.attrib & _A_SUBDIR)) // if true, this is a subdirectory and we must recursively populate
				{
					if(strcmp(files.name,".") != 0 && strcmp(files.name,"..") != 0 /*&& strcmp(files.name,"$Recycle.Bin") != 0*/ ) // we need to skip "." and ".." as these are symbols for the current and previous directories
					{
						string pathTemp;
						pathTemp = dirName + "\\" + files.name;
						
						//cout << "Added directory: " << files.name << endl; // noisy debug output
						dir childDir(pathTemp);
						childDir.populate(); // recursive!! populates the next step in the tree
						childDirs.push_back(&childDir); // adds a reference to the current object, to be followed later
						sizeTemp = sizeTemp + childDir.size(); // adds to the current directory size
					}
				}
				else // otherwise, this is a normal file, and we just need to add the file and its size to the current directory
				{
					string pathTemp;
					pathTemp = dirName + "\\" + files.name; // path to pass to the file constructor - pad with a trailing / in case the original path was missing it
					
					//cout << "Adding file: " << files.name << endl; // noisy debug output
					file childFile(pathTemp); // create a child file
					childFiles.push_back(&childFile); // add a reference to the child to the parent directory
					sizeTemp = sizeTemp + childFile.size(); // add the size of the child to the parent directory
				}
			}
			while(_findnext(hFile, &files) == 0);
		}			
		dirSize = sizeTemp;
		_findclose(hFile); // close the file index handle
		return;
	}
	void printSubSize(long total) // Traverses directory tree and prints the size of all directories below this one to std output
	{
		cout << total << endl; // TODO: Print subdirectory sizes
		return;
	}
	unsigned long long size() { return dirSize; }
	string name() { return dirName; }
private:
	string dirName; // path to represented directory
	unsigned long long dirSize; // size of represented directory and all of its child directories and files
	vector<dir*> childDirs; // links to all child directories
	vector<file*> childFiles; // links to all child files
};

int main(int argc, char* argv[])
{
	string path;
	
	if (argc == 2)
	{
		path = argv[1];
		cout << "You entered: " << path << endl;
	}
	else if (argc == 1)
	{
		cout << "Please enter a path to begin at: ";
		getline(cin, path);
		cout << "You entered: " << path << endl;
	}
	else
	{
		cout << "Invalid number of arguments" << endl;
		return 0;
	}

	dir root(path);

	root.populate();
	cout << "Specified directory is: " << fixed << setprecision(0) << root.size() << " bytes" << endl;
	//root.printSubSize(root.size());
	
	return 0;
};