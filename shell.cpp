#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include <vector>

#define N_HISTORY 100
#define MAX_LEN 120
#define MAX_ARGUMENTS 100

// #define UNAME "test"
// #define SNAME "customShell"

// For color outPut of the console terminal 

/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST



void printConsoleBaseString(char* currentDir,char* UNAME,char* SNAME)
{
    printf(FYEL("[Custom Shell] "));
    printf(FGRN("%s@%s"),UNAME,SNAME);
    printf(FWHT(":"));
    printf(FBLU("%s"),currentDir);
    printf(FWHT("$ "));
    fflush(stdout);
}



// Prints the welcome string
void printWelcomeString()
{
    printf(FYEL("************************************************************************************************\n"));
    printf(FYEL("**----------------------- Welcome to Custom Linux Terminal -----------------------------------**\n"));
    printf(FYEL("**-------- ##   With Great powers comes great Responsibilty ##--------------------------------**\n"));
    printf(FYEL("**-------- ##   For help , enter help                           ------------------------------**\n"));
    printf(FYEL("************************************************************************************************\n"));
    fflush(stdout);
}


// Get the input from the user 
char** commandTokenizer(char* userCommand, char** tokenString,char* delimiterStrings,int& num)
{
    int k = 0;
    // Use the Tokenize command to get the string arguments
    tokenString[k] = strtok(userCommand, delimiterStrings); 
    // Keep Extracting all the arguments 
    while(tokenString[k] != NULL){
        k++;
        tokenString[k] = strtok(NULL, delimiterStrings);
    }
    num = k-1;
    return tokenString;
}


void printHelpString()
{
    printf(FBLU("-- Custom Shell Help Menu ---- \n"));
    printf(FGRN("Command : history\n"));
    printf(("\t Prints the History of commands (if available), else prints an error \n"));
    printf(FGRN("Command : !!\n"));
    printf(("\t Prints the Last command in history \n"));
    printf(FGRN("Command : !x\n"));
    printf(("\t Prints the xth command in history \n"));
}


int printExitMessage()
{
        printf(FRED("Exiting Process..!\n"));

}


int isForkFailed(int pid)
{
    int fail = 0;
    if(pid < 0)
    {
        fail = 1;
    }
        
    
    return fail;
}

void printBgProcess(int bg_count,pid_t* bg_ProcessIds)
{
    std::cout << "S.No"<< "\t"<< "pid"<<std::endl;
    for ( int i = 0 ; i < bg_count; i++)
    {
        std::cout << i << "\t" << bg_ProcessIds[i]  <<std::endl;
    }
}


int historyFunction(std::vector<std::string> hist )
{
    if(hist.size() == 0)
    {
        printf(FRED("No commands in history\n"));
    }
    std::cout << hist.size()<<std::endl;
    for ( int i = hist.size() ; i > 0;i--)
    {
        std::cout << i << "\t" << hist[i-1]<<std::endl;
    }
}

using namespace std;
int main()
{

    int runProcess = 1;
    int status = 0;
    char currentDir[MAX_LEN];
    char userCommand[MAX_LEN];
    int firstRun = 0;
    char USERNAME[MAX_LEN];
    char SYSTEMNAME[MAX_LEN];
    int numParam;
    int bg_count = 0;
    int isCommandFromHistory = 0;
    

    // Save the delimters for Extracting the tokens from the Given command
    char delimiterStrings[] = " \f\v\t\r\n"; // delimiter to remove all spaces and extraneous chars
    

    char** tokenString =new char*[MAX_ARGUMENTS];    // TO store the tokens
    // Assign Memory to Token string 
    for ( int i = 0 ; i < MAX_ARGUMENTS; i++)
        tokenString[i] = new char[MAX_LEN];

    char** BgProcessString =new char*[MAX_ARGUMENTS];    // TO store the tokens
    // Assign Memory to Token string 
    for ( int i = 0 ; i < MAX_ARGUMENTS; i++)
        BgProcessString[i] = new char[MAX_LEN];
    
    std::vector< std::string> histCommandLists;

    pid_t bg_ProcessIds[50]; // pid's of background process are stored.

    //GEt username 
    getlogin_r(USERNAME, sizeof(USERNAME));
    gethostname(SYSTEMNAME, sizeof(SYSTEMNAME));


    // Get the current Workign Directory 
    if (getcwd(currentDir, sizeof(currentDir)) == NULL)
        cerr << "Current Working Directory Cannot be Obtained " <<endl;
    
    // Prints the welcome string
    printWelcomeString();

    //Print Console base string
    printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);


    while(runProcess)
    {
        // Get the user input as command using fgets 
        if(fgets(userCommand, sizeof(userCommand), stdin));

        if(strcmp(userCommand,"\n") == 0)
        {
            printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
            continue;
        }

        if(strcmp(userCommand,"!!\n") == 0)
        {
            if(histCommandLists.size() == 0)
            {
                printf(FRED("No commands in history\n"));
                printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
                continue;
            }
                
            else
                {
                    strcpy(userCommand,histCommandLists[histCommandLists.size()-1].c_str());
                    isCommandFromHistory = 1;
                }
        }

        else if(strchr(userCommand,'!')!= NULL)
        {
            const char* ptr  = strchr(userCommand,'!');
            int index  = ptr - userCommand;
            // printf("Index : %d\n",index);
            if(index != 0)
            {
                printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
                continue;
            }
            char* a = (char*)malloc(strlen(userCommand)-1);
            strncpy(a,userCommand+1,sizeof(char)*strlen(userCommand)-1);
            int num = atoi(a);
            // printf("%d",num);
            if(num <= histCommandLists.size())
            {
                strcpy(userCommand,histCommandLists[num-1].c_str());
                isCommandFromHistory = 1;
               
            }
            else{
                printf(FRED("No such command in history.\n"));
                printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
                continue;
            }
            
            
        }

        else {} // Do nothing

        // Tokenize the Given String using 
        tokenString = commandTokenizer(userCommand,tokenString,delimiterStrings,numParam);
        

        // Write all the commands for unique process. 
        // Such as "cd" , "exit" , "&", "Help"

        if(strcmp(tokenString[0],"exit") == 0)
        {
            printExitMessage();
            runProcess = 0;
            return(1);
        }

        

        
        
        if(strcmp(tokenString[0], "cd") == 0)
        {
            int result;
            // Change directory to the given directory
            if(numParam > 0) 
            {

                result = chdir(tokenString[1]);
                
                if(result != 0)
                    printf(FRED("%s Directory does not exists\n"),tokenString[1]);
                
                getcwd(currentDir, sizeof(currentDir));
                
            }
                
            else
            {
                std::string k = "/home/" + std::string(USERNAME);
                result = chdir(k.c_str());
                getcwd(currentDir, sizeof(currentDir));
            }
            
            
            if(!isCommandFromHistory) histCommandLists.push_back(std::string(userCommand));
            printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
            continue;
                
        }

        if(strcmp(tokenString[0], "help") == 0)
        {
            if(!isCommandFromHistory) histCommandLists.push_back(std::string(userCommand));
            printHelpString();
            printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
            continue;

        }
            

        if(strcmp(tokenString[0], "history") == 0)
        {

            historyFunction(histCommandLists);  
            printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
            if(!isCommandFromHistory) histCommandLists.push_back(std::string(userCommand));
            continue;
        }
                  

        // if(strcmp(tokenString[0],"bgprocess") == 0)        
        //     printBgProcess(bg_count,bg_ProcessIds);
        
        int BackGroundFlag = 0;
        if (strcmp(tokenString[numParam], "&") == 0) {
			BackGroundFlag = 1; // background set to true
			tokenString[numParam] = NULL; // remove & arg
            numParam --;
		}

        
        // FORK THE Actual process
        pid_t ProcessId = fork(); // fork!
        
        // Check the fork 
        if(isForkFailed(ProcessId) == 1)
        {
            printf(FRED("Fork Failed.!\n"));
            return 1;
        }
        // Execute the process
        if (ProcessId == 0) 
        { 
            execvp(tokenString[0], tokenString); // exec on path, with commands	
            fflush(stdout);
        }
        // parent process
		else 
        { 
			// if background flag, parent won't wait
			if (BackGroundFlag) 
            {
                printf("BACKGROUND");
				bg_ProcessIds[bg_count] = ProcessId;
				bg_count++; // increment background process number
				printf("ProcessId: %d %s is running in Background \n", ProcessId, userCommand); // print pid info
                fflush(stdout);
                // printf("Use bgprocess to list all Background Process \n"); // print pid info
			} 
            else 
            {
				waitpid(ProcessId, NULL, 0); // make parent wait
			}
		}

        if(!isCommandFromHistory) histCommandLists.push_back(std::string(userCommand));
        printConsoleBaseString(currentDir,USERNAME,SYSTEMNAME);
        isCommandFromHistory = 0;
    }

    cout <<endl;
    flush(cout);
    return 0;
}