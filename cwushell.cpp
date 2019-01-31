/*
  Author: Al Timofeyev
	Date:		1/13/2019
	Desc:		A simple shell written in C++.
					The code was written, complied, and executed on
					Linux Ubuntu 18.04.1 using the g++ compiler in shell.
					To compile code, in shell run command:
					g++ cwushell.cpp
					To execute code, in shell run command:
					./a.out
					A ReadMe.txt file will also be provided with the same information.
*/

#include <iostream>
#include <fstream>			// For reading cpuinfo/meminfo
#include <string.h>			// For strtok() and strcmp()
#include <sstream>			// For stringstream conversion
#include <vector>
#include <unistd.h>			// For fork()
#include <cstdlib>			// For exit()
#include <stdio.h>
#include <sys/types.h>	// For pid_t
#include <sys/wait.h>		// For waitpid()

using namespace std;

// Function Prototypes.
void tockenizeCommand(char *command, int &argc, char **argv);
void executeCommand(char **argv);
void printHelp();
void printManual();

int main(int argc, char* argv[])
{
	char input[400];
	string prompt = "cwushell";
	long totalMem = 0;
	long freeMem = 0;

	while(true)
	{
		cout << prompt << "> ";

		// Get the command from the user and tokenize it int argv[].
		cin.getline(input, 400);
		tockenizeCommand(input, argc, argv);

		// Check to see if the user only pressed the Return/Enter key.
		if(argc == 0)
			continue;

		// ** Execute Commands **
		// Exit Command.
		if(strcmp(input, "exit") == 0)
		{
			// If there was an exit value provided.
			if(argc >= 2)
			{
				// Convert the exit value into an int.
				string sNum = argv[1];
				stringstream sSNum(sNum);
				int num = 0;
				sSNum >> num;

				return num;
			}

			return 0;
		}

		// Print-Manual Command.
		else if(strcmp(input, "manual") == 0)
		{
			printManual();
		}

		// Print-Help Command.
		else if(strcmp(input, "shellhelp") == 0)
		{
			printHelp();
		}

		// Prompt-Change Command.
		else if(strcmp(input, "prompt") == 0)
		{
			// If new prompt name was provided.
			if(argc >= 2)
			{
				prompt = argv[1];
				continue;
			}

			// Reset prompt if no new prompt name was provided.
			prompt = "cwushell";
		}

		// Print-CPU Info Command.
		else if(strcmp(input, "cpuinfo") == 0)
		{
			string line;
			ifstream cpu("/proc/cpuinfo");

			// Print CPU Type.
			if(argc >= 2 && (strcmp(argv[1], "-t") == 0))
			{
				for(int i = 0; i < 5 && !cpu.eof(); i++)
				{
					getline(cpu, line);
				}
				cout << line << endl;
			}

			// Print CPU Clock Speed.
			else if(argc >= 2 && (strcmp(argv[1], "-c") == 0))
			{
				for(int i = 0; i < 7 && !cpu.eof(); i++)
				{
					getline(cpu, line);
				}
				cout << line << endl;
			}

			// Print Number of CPU Cores.
			else if(argc >= 2 && (strcmp(argv[1], "-n") == 0))
			{
				for(int i = 0; i < 12 && !cpu.eof(); i++)
				{
					getline(cpu, line);
				}
				cout << line << endl;
			}

			// Print cpuinfo help.
			else
			{
				cout << "Incorrect usage of command: cpuinfo -switch\n" << endl;

				cout << "cpuinfo has the following three switches:" << endl;
				cout << "-t\t" << "Prints the CPU Type." << endl;
				cout << "-c\t" << "Prints the CPU Clock Speed." << endl;
				cout << "-n\t" << "Prints the Number of CPU Cores.\n" << endl;

				cout << "For a brief list of defined commands please type: shellhelp" << endl;
				cout << "For a full list of defined commands and usage examples please type: manual" << endl;
			}
		}

		// Print-Memory Info Command.
		else if(strcmp(input, "meminfo") == 0)
		{
			string line;
			ifstream mem("/proc/meminfo");
			long usedMem;

			// Print Total ammount of RAM Available.
			if(argc >= 2 && (strcmp(argv[1], "-t") == 0))
			{
				for(int i = 0; i < 1 && !mem.eof(); i++)
				{
					getline(mem, line);
				}

				// Tokenize the line.
				vector<string> lineTokens;
				stringstream lineStream(line);
				string temp;
				while(getline(lineStream, temp, ' '))
					lineTokens.push_back(temp);

				totalMem = stoi(lineTokens[lineTokens.size()-2]);
				line = "Total RAM Available:\t" + to_string(totalMem * 1000) + " Bytes";
				cout << line << endl;
			}

			// Print Used RAM Memory.
			else if(argc >= 2 && (strcmp(argv[1], "-u") == 0))
			{
				for(int i = 0; i < 2 && !mem.eof(); i++)
				{
					getline(mem, line);
				}

				// Tokenize the line.
				vector<string> lineTokens;
				stringstream lineStream(line);
				string temp;
				while(getline(lineStream, temp, ' '))
					lineTokens.push_back(temp);

				freeMem = stoi(lineTokens[lineTokens.size()-2]);
				usedMem = totalMem - freeMem;
				line = "Used RAM:\t" + to_string(usedMem * 1000) + " Bytes";
				cout << line << endl;
			}

			// Print Size of Cache.
			else if(argc >= 2 && (strcmp(argv[1], "-c") == 0))
			{
				ifstream cpu("/proc/cpuinfo");
				for(int i = 0; i < 8 && !cpu.eof(); i++)
				{
					getline(cpu, line);
				}

				// Tokenize the line.
				vector<string> lineTokens;
				stringstream lineStream(line);
				string temp;
				while(getline(lineStream, temp, ' '))
					lineTokens.push_back(temp);

				int cache = stoi(lineTokens[lineTokens.size()-2]);
				line = "Cache Size:\t" + to_string(cache * 1000) + " Bytes";
				cout << line << endl;
			}

			// Print meminfo help.
			else
			{
				cout << "Incorrect usage of command: meminfo -switch\n" << endl;

				cout << "meminfo has the following three switches:" << endl;
				cout << "-t\t" << "Prints the total available RAM." << endl;
				cout << "-u\t" << "Prints the used ammount of RAM." << endl;
				cout << "-c\t" << "Prints the size of Cache.\n" << endl;

				cout << "For a brief list of defined commands please type: shellhelp" << endl;
				cout << "For a full list of defined commands and usage examples please type: manual" << endl;
			}
		}

		// Else if the command is unknown or internal.
		else
		{
			executeCommand(argv);
		}
	}
}

/*
	This function tockenizes the command provided by the user and
	stores the command parameters in argv[] using the " " delimiter.

	@param *command		The character array of the user command.
	@param **argv			The argv[] matrix used by the system for
										command line arguments.
*/
void tockenizeCommand(char *command, int &argc, char **argv)
{
	// Reset the total number of arguments to 0.
	argc = 0;

	char *tokenizedCMD = strtok(command, " \t");
	while(tokenizedCMD != NULL)
	{
		// Iterate through program arguments and add user commands/parameters.
		*argv++ = tokenizedCMD;

		// Increment the number of commands/parameters added.
		argc++;

		// Tokenize the next user command/parameter.
		tokenizedCMD = strtok(NULL, " \t");
	}

	*argv = NULL;
}

/*
	This function executes the command provided by the user.
	@param **argv			The argv[] matrix used by the system for
										command line arguments.
*/
void executeCommand(char **argv)
{
	int status;

	// Fork a child process.
	pid_t pid = fork();

	// If forked child process failed.
	if(pid < 0)
	{
		cout << "*ERROR: Forking child process failed" << endl;
		exit(1);
	}

	// Else try executing the child process.
	else if(pid == 0)
	{
		if(execvp(*argv, argv) < 0)
		{
			// If execution of the child process fails.
			cout << "Unknown Command: " << argv[0] << endl;
			cout << "For a list of commands, please type: shellhelp" << endl;
			exit(1);
		}
	}

	// Wait for any child process to finish.
	else
		waitpid(pid, &status, 0);
}

/*
	A simple help functionn that prints a brief list of commands.
*/
void printHelp()
{
	cout << "\n**** Brief List of All Commands ****\n";
	cout << "shellhelp" << endl;
	cout << "\t\t\t- Prints a brief list of all commands to the screen.\n" << endl;

	cout << "manual" << endl;
	cout << "\t\t\t- Prints a full list of all commands included with this" << endl;
	cout << "\t\t\t- cwushell as well as usage examples.\n" << endl;

	cout << "exit [n]" << endl;
	cout << "\t\t\t- Terminates the shell. [n] is an optional parameter.\n" << endl;

	cout << "prompt <new_prompt>" << endl;
	cout << "\t\t\t- Changes the prompt name if one is provided.\n" << endl;

	cout << "cpuinfo -switch" << endl;
	cout << "\t\t\t- Shows the cpu information based on the switch provided." << endl;
	cout << "\t\t\t- There are three possible switches: -t, -c, -n\n" << endl;

	cout << "meminfo -switch" << endl;
	cout << "\t\t\t- Show the memory information based on the switch provided." << endl;
	cout << "\t\t\t- There are three possible swithces: -t, -u, -c\n" << endl;
}

/*
	This function prints out the manual for the cwushell, which includes
	a full list of all commands as well as usage examples.
*/
void printManual()
{
	cout << "\n****************************************************************\n";
	cout << "********* Full List of All Commands and Usage Examples *********\n";
	cout << "shellhelp" << endl;
	cout << "\t\t\t- Prints out a brief list of commands to the screen." << endl;
	cout << "\t\t\t- Examples: cwushell> shellhelp\n" << endl;

	cout << "manual" << endl;
	cout << "\t\t\t- Prints out a full list of commands included with this" << endl;
	cout << "\t\t\t- cwushell as well as usage examples." << endl;
	cout << "\t\t\t- Examples: cwushell> manual\n" << endl;

	cout << "exit [n]" << endl;
	cout << "\t\t\t- Terminates the shell. [n] is an optional parameter." << endl;
	cout << "\t\t\t- Examples: cwushell> exit" << endl;
	cout << "\t\t\t-           cwushell> exit 12\n" << endl;

	cout << "prompt <new_prompt>" << endl;
	cout << "\t\t\t- Changes the prompt name if one is provided." << endl;
	cout << "\t\t\t- Examples: cwushell> prompt" << endl;
	cout << "\t\t\t-           cwushell> prompt blub\n" << endl;

	cout << "cpuinfo -switch" << endl;
	cout << "\t\t\t- Shows the cpu information based on the switch provided." << endl;
	cout << "\t\t\t- There are three possible switches: -t, -c, -n" << endl;
	cout << "\t\t\t- Examples: cwushell> cpuinfo -t" << endl;
	cout << "\t\t\t-           cwushell> cpuinfo -c" << endl;
	cout << "\t\t\t-           cwushell> cpuinfo -n\n" << endl;

	cout << "meminfo -switch" << endl;
	cout << "\t\t\t- Show the memory information based on the switch provided." << endl;
	cout << "\t\t\t- There are three possible swithces: -t, -u, -c" << endl;
	cout << "\t\t\t- Examples: cwushell> meminfo -t" << endl;
	cout << "\t\t\t-           cwushell> meminfo -u" << endl;
	cout << "\t\t\t-           cwushell> meminfo -c\n" << endl;

	cout << "All other existing shell commands are available with this cwushell." << endl;
	cout << "Some Example Commands: man, ls, cat, pwd, etc." << endl;
	cout << "Please Note: The command    man    is used to print the manual of" << endl;
	cout << "already existing commands." << endl;
	cout << "\t\t\t- Examples: cwushell> man ls" << endl;
	cout << "\t\t\t            cwushell> man cat" << endl;
	cout << "\t\t\t            cwushell> man pwd\n" << endl;

	cout << "\n****************************************************************\n";
}
