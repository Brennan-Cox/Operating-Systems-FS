/**
 * @author Brennan Cox
 * 
 * This program is intended to simulate a filesystem when you pass in a filename to this program
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <algorithm>

using namespace std;

/**
 * method returns a copy of a string without the leading or following space
*/
string trim(string input) {
    //finds first non whitespace and erase all before
    input.erase(input.begin(), find_if(input.begin(), input.end(), [](int ch) {
        return !isspace(ch);
    }));
    //does the same thing but in reverse using a reverse itterative approach
    input.erase(find_if(input.rbegin(), input.rend(), [](int ch) {
        return !isspace(ch);
    }).base(), input.end());
    return input;
}

/**
 * base class to be extended from all children have a filename
 */
class File
{
protected:
    char fileName[11];

public:
    File(char *fileName)
    {
        for (int i = 0; i < 11; i++)
        {
            this->fileName[i] = fileName[i];
        }
    }
};

/**
 * All program files will have a filename cpureq and memreq
 */
class ProgramFile : public File
{
private:
    int cpuReq;
    int memReq;

public:
    
    /**
     * stores cpuReq and memReq in program attributes
     */
    ProgramFile(char *fileName, int cpuReq, int memReq)
        : File(fileName)
    {
        this->cpuReq = cpuReq;
        this->memReq = memReq;
    }

    string toString() {
        stringstream ss;
        ss << "Text File"
           << ":\t";
        for (int i = 0; i < 11; i++)
        {
            ss << fileName[i];
        }
        ss << "contents: cpu req: " << cpuReq << ", mem req: " << memReq << endl;
        return ss.str();
    }

    void write(fstream &streamIO, stringstream &ss) {
        ss << streamIO.tellp() << ":\t";
        ss << "Filename:\t";
        stringstream name;
        for (int i = 0; i < 11; i++) {
            name << fileName[i];
        }
        ss << name.str() << endl;
        streamIO.write(fileName, 11);
        ss << "\tType: Program" << endl;
        ss << "\tContents: CPU Requirement: " << cpuReq;
        ss << ", Memory Requirement " << memReq << endl;
        streamIO.write((char*)&cpuReq, sizeof(cpuReq));
        streamIO.write((char*)&memReq, sizeof(memReq));
    }
};

/**
 * All TestFiles will have a filename, num of chars, and file contents
 */
class TextFile : public File
{
private:
    int numOfChars;
    string fileContents;
public:
    TextFile(char *fileName, int numOfChars, string fileContents)
        : File(fileName)
    {
        this->numOfChars = numOfChars;
        this->fileContents = fileContents;
    }

    string toString() {
        stringstream ss;
        ss << "Text File"
           << ":\t";
        for (int i = 0; i < 11; i++)
        {
            ss << fileName[i];
        }
        ss << endl << "size: " << numOfChars << endl << "contents: ";
        ss << fileContents << endl;
        return ss.str();
    }

    void write(fstream &streamIO, stringstream &ss) {
        ss << streamIO.tellp() << ":\t";
        ss << "Filename:\t";
        stringstream name;
        for (int i = 0; i < 11; i++) {
            name << fileName[i];
        }
        ss << name.str() << endl;
        ss << "\tType: Text file" << endl;
        streamIO.write(fileName, 11);
        ss << streamIO.tellp() << ":\t";
        ss << "Size of text file: " << numOfChars << " byte" << endl;
        streamIO.write((char*)&numOfChars, sizeof(numOfChars));
        ss << streamIO.tellp() << ":\t";
        ss << "Contents of text file: " << fileContents << endl;
        streamIO.write((char*)&fileContents, numOfChars);
    }
};

/**
 * All Directories will have a filename, num of items contained,
 * and lists of other directories program files and text files
 */
class Directory : public File
{
private:

    Directory *prev;
    int manyItems;
    vector<ProgramFile*> programFiles;
    vector<TextFile*> textFiles;
    vector<Directory*> directories;
public:

    Directory(char *fileName, int manyItems)
        : File(fileName)
    {
        this->manyItems = manyItems;
        prev = NULL;
    }

    Directory(Directory *prev, char *fileName, int manyItems)
        : File(fileName)
    {
        this->manyItems = manyItems;
        this->prev = prev;
    }

    /**
     * toString method for testing purposes
    */
    string toString()
    {
        stringstream ss;
        ss << "Directory"
           << ":\t";
        for (int i = 0; i < 11; i++)
        {
            ss << fileName[i];
        }
        ss << " Contains " << manyItems << endl;
        for (TextFile* file : textFiles) {
            ss << file->toString();
        }
        for (ProgramFile* file : programFiles) {
            ss << file->toString();
        }
        return ss.str();
    }

    /**
     * Method that writes a Directory
     * utilizes recursion for subdirectories
    */
    void write(fstream &streamIO, stringstream &ss) {

        //write name to file and give to ss
        ss << streamIO.tellp() << ":\t";
        ss << "Directory:\t";
        stringstream name;
        for (int i = 0; i < 11; i++) {
            name << fileName[i];
        }
        ss << name.str();
        streamIO.write(fileName, 11);

        //ex -> 11: Directory root contains 2 files/directories
        ss << endl << streamIO.tellp() << ":\t";
        ss << "Directory " << getName() << " contains " << manyItems;
        ss << " files/directories" << endl;
        streamIO.write((char*)&manyItems, sizeof(manyItems));

        //call recursive for each subdirectory
        for (TextFile* file : textFiles) {
            file->write(streamIO, ss);
        }
        for (ProgramFile* file : programFiles) {
            file->write(streamIO, ss);
        }
        for (Directory* directory : directories) {
            directory->write(streamIO, ss);
        }

        //ex -> 65: End of directory first.d
        ss << streamIO.tellp() << ":\t";
        ss << "End of directory " << name.str() << endl;
        streamIO.write("End", 3);
        streamIO.write(fileName, 11);
    }

    /**
     * Returns a string consisting of just the name 
     * portion of file name
    */
    string getName() {
        stringstream ss;
        for (int i = 0; i < 8; i++) {
            ss << fileName[i];
        }
        return ss.str();
    }

    /**
     * Method adds the given directory to
     * the list of directories
     * then it increments the number of items in directory
    */
    void addDirectory(Directory* toAdd) {
        manyItems++;
        directories.push_back(toAdd);
    }

    void addTextFile(TextFile* toAdd) {
        manyItems++;
        textFiles.push_back(toAdd);
    }

    void addProgramFile(ProgramFile* toAdd) {
        manyItems++;
        programFiles.push_back(toAdd);
    }

    bool hasPrev() {
        return prev != NULL;
    }

    Directory* getPrev() {
        if (hasPrev()) {
            return prev;
        } else {
            throw invalid_argument("current node " + toString() + " has no prev");
        }
    }
};

/**
 * takes a desired name and a file extension character
 * then formats that information as a fileName
 * makes the given name fit to 8 characters
 */
void fileNameFormat(string name, char ex, char *formatted)
{
    int i = 0;
    while (i < 8 && i < name.length())
    {
        formatted[i] = name[i];
        i++;
    }
    while (i < 8)
    {
        formatted[i] = '\0';
        i++;
    }
    formatted[i++] = '.';
    formatted[i++] = ex;
    formatted[i++] = '\0';
}

/**
 * Prints a string in command prompt format
 */
void prompt(string in)
{
    cout << in + ">";
}

/**
 * returns command from console
 */
string takeCommand(string workingDir)
{
    prompt("Command(" + workingDir + ")");
    string str;
    getline(cin, str);
    str = trim(str);
    return str;
}

/**
 * Prints a string in error format
 */
void error(string in)
{
    cout << "***" << in << "***" << endl;
}

/**
 * method gets and formats a file name then
 * returns the pointer to the name
*/
void getFileName(char* name, string type, char ex) {
    prompt("Enter " + type + " name");
    string input;
    getline(cin, input);
    input = trim(input);
    fileNameFormat(input, ex, name);
}

/**
 * method gets a filename from user and formatts it properly if possible
 * this overloaded method is for a filename specifically
*/
void getFileName(char* name) {

    //gets filename to a char* format that can be broken up using strtok
    prompt("Enter filename");
    string input;
    getline(cin, input);
    input = trim(input);
    char* inputCA = new char[input.length()];
    for (int i = 0; i < input.length(); i++) {

        inputCA[i] = input[i];
    }

    //trys to tokenize on delimiter '.'
    try {
        string inputName = strtok(inputCA, ".");
        string aft = strtok(NULL, ".");
        char ex = aft[0];
        fileNameFormat(inputName, ex, name);
    } catch (const exception& e) {
        error("INCORRECT FILENAME FORMAT");
    }
    delete[] inputCA;
}

/**
 * method that from a pointer to a given length will null out part of memory
 * (eliminates the likelyhood that a mistype will result in file creation)
*/
void nullCharArray(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        arr[i] = '\0';
    }
}

int main(int argc, char **argv)
{
    //Project spec didn't say to read data just write
    fstream binaryFile;
    binaryFile.open(argv[1], ios::binary | ios::out);

    // const strings and file name
    const string welcome = "Welcome to RUFS. Enter one of the following commands:";
    const string validOptions = "CreateDir or CreateFile or EndDir or quit";
    cout << welcome << endl
         << validOptions << endl;

    //initialize root and working directory
    char rootName[11];
    fileNameFormat("root", 'd', rootName);
    Directory* working = new Directory(rootName, 0);
    Directory* root = working;

    //begin commands
    string command = takeCommand(working->getName());
    while (command.compare("quit") != 0)
    {

        if (command == "CreateDir")
        {
            //get name of directory
            char name[11]; getFileName(name, "Directory", 'd');

            //create directory
            Directory* newDir = new Directory(working, name, 0);

            //position directoy
            working->addDirectory(newDir);

            //move to new directory
            working = newDir;
        }
        else if (command == "print") 
        {
            //testing method to print some meta data about a sub tree
            cout << working->toString();
        }
        else if (command == "CreateFile")
        {
            //get fileName of size 11
            char name[11]; nullCharArray(name, 11); getFileName(name);

            if (name[9] == 't')
            {
                prompt("Please enter file contents");
                string input;
                getline(cin, input);

                int size = input.length();
                TextFile *newFile = new TextFile(name, size, input);

                //poistion file
                working->addTextFile(newFile);

            } else if (name[9] == 'p') {

                
                prompt("Enter CPU requirements");
                int cpuReq; cin >> cpuReq;

                string dummy;
                //consume newline
                getline(cin, dummy);

                prompt("Enter memory requirements");
                int memReq; cin >> memReq;

                //consume newline
                getline(cin, dummy);

                ProgramFile *newFile = new ProgramFile(name, cpuReq, memReq);

                //position file
                working->addProgramFile(newFile);

            } else {

                /*
                if the file doesn't have a .p or .t extenstion
                or the name of the file is of invalid format
                */
                error("INVALID FILE EX");
            }
        }
        else if (command == "EndDir")
        {
            if (working->hasPrev()) {
                working = working->getPrev();
            } else {
                error("HAS NO PREV");
            }
        }
        else if (command != "")
        {
            error(command + " INPUT DOES NOT MATCH A VALID COMMAND");
            cout << validOptions << endl;
        }
        command = takeCommand(working->getName());
    }
    stringstream ss;
    root->write(binaryFile, ss);
    cout << ss.str();
    binaryFile.close();
    return 0;
}