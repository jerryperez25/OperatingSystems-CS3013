#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <ctime>
#include <sstream>
#include <vector>
#include <string.h>

string prompt = "==>";

/*struct helpme
{
	string arguments;
	struct timeval newStartTime;
	int pid;
};*/

void enterShell(int shellpid)
{

	bool trigger = true;
	// this is going to control our while loop
	bool backgroundProc; //no functionality yet
	//vector <helpme> background;
	while(trigger)
	{
	backgroundProc = false;
	/*for(int i=0; i<background.size();i++)
	{
		int hold;
		int finstate;
		finstate = waitpid(background.at(i).pid, &hold, WNOHANG);
		if (finstate != 0)
		{
			cout << "[" << i+1 << "] " << background[i].pid << "has been completed." << endl;
			//print stats
			background.erase(background.begin()+i);
		}
	}*/
	//create new variables for statistics
    	struct timeval newstart;  
    	struct timeval newend;
    	struct rusage totalusageshell;
    	
    	//Storing the user response
    	char* response = (char*) malloc(128*sizeof(char));
	cout << prompt;
	cin.getline(response,128);
	
	//Detecting endline 
		
		if(!cin)
		{
			if(cin.eof())
			{
				cout << "End of file has been reached. Now exiting." << endl;
				exit(0);
			}
		}
		
		//Strip the response array
		char* responseCollec[32];
		char* responses;
		int j = 1;
		responses = strtok(response, " ");
		responseCollec[0] = responses;
		
		//while stripping insert into our array
		while ((responses = strtok(NULL, " ")))
		{
			responseCollec[j] = responses;
			j++;
		}
		//set the last element to be null
		responseCollec[j] = NULL;
			
		//trying to get exit to work properly	
		//string exitvar = responseCollec[0];
		
		//use str comp to match the first element to that of exit
		if(strncmp(responseCollec[0],"exit",4)==0)
		{
			exit(0);
		}	
		
		//functionality for set prompt
		else if ((strncmp(responseCollec[0], "set", 3) == 0) && (strncmp(responseCollec[1], "prompt",6) == 0))
    		{
    			string newPrompt;
    			newPrompt = responseCollec[3];
    			prompt = newPrompt;
    				
    		}
		
		//functionality for changing the directory
		else if(strncmp(responseCollec[0], "cd", 2) == 0)
		{
			if (strncmp(responseCollec[1], "..", 2) == 0)
			{
				cout<<"You are moving back a directory."<<endl;
				chdir(responseCollec[1]);
			}
			else
			{
				chdir(responseCollec[1]);
				cout << "Working directory is now: " << 	responseCollec[1]<<endl;
			}
		}
		
	else
	{
	//get time of day
	gettimeofday(&newstart,NULL);	
	
	if ((shellpid = fork()) < 0) 
    	{
        	cerr << "Fork error\n";
        	exit(1);
    	}
    	else if (shellpid == 0) 
    	{	
    		 if(strncmp(responseCollec[j-1],"&",1)==0)
    		{
    		
    			cout << "we have a background task and it is not fully functional" <<endl;
    			char* ampstripper[32];
    			for (int i =0; i < j-1;i++)
    			{
    				ampstripper[i] = responseCollec[i];
    			}
    			ampstripper[j-1] = NULL;
    			
    			if (execvp(ampstripper[0],ampstripper) < 0) 
		{
          	  cerr << "Execvp error\n";
          	  exit(1);
        	} 
    		} 	

		
		else if (execvp(responseCollec[0],responseCollec) < 0) 
		{
          	  cerr << "Execvp error\n";
          	  exit(1);
        	}
        			
	}
	else
	{
		//wait to finish
		wait(0);
		gettimeofday (&newend, NULL);
        	struct rusage shellusage;
        	
    //Statistic printing
    
    getrusage(RUSAGE_CHILDREN, &shellusage);
    
    cout << "CPU System Time: " << (shellusage.ru_stime.tv_sec * 1000.0 
    + shellusage.ru_stime.tv_usec/1000.0)<< " milliseconds"<< endl;
    
    cout << "CPU User Time: " << (shellusage.ru_utime.tv_sec * 1000.0 
    + shellusage.ru_utime.tv_usec/1000.0) << " milliseconds"<<endl;
    
    cout << "Wall Clock Time: " << ((newend.tv_sec - newstart.tv_sec)*1000.0) + 
    ((newend.tv_usec - newstart.tv_usec)/1000.0) << " millseconds"<< endl;
    
    cout << "Number of times the process was preempted involuntarily: " << shellusage.ru_nivcsw << endl;
    
    cout << "Number of times process gave up the CPU voluntarily: " << shellusage.ru_nvcsw << endl;
    
    cout << "Number of major page faults: " << shellusage.ru_majflt << endl;
    
    cout << "Number of minor page faults: " << shellusage.ru_minflt << endl;
    
    totalusageshell = shellusage;
		
	}
	
}
}
}


int main(int argc, char **argv)
     /* argc -- number of arguments */
     /* argv -- an array of strings */
{
    char *argvNew[5];
    int pid;
    int status;
    
    struct timeval start;
    struct timeval end;
    struct rusage totalusage;
    
    getrusage(RUSAGE_CHILDREN, &totalusage);
    gettimeofday(&start, NULL);

	if(argc==1)
	{
		enterShell(pid);
	}
    

    	if ((pid = fork()) < 0) 
   	 {
      	  cerr << "Fork error\n";
       	 exit(1);
    	}
    	else if (pid == 0) 
   	 {
        	/* child process */
       	 argvNew[0] = argv[1];
	   	 argvNew[1] = argv[2];
       	 if (execvp(argvNew[0],argvNew) < 0) 
		{
           		 cerr << "Execvp error\n";
           		 exit(1);
       	 }
	
    	}
    	else 
    	{
    
        	wait(0);	/* wait for the child to finish */
            /* parent */
        	gettimeofday (&end, NULL);
        	struct rusage myusage;
    
    		getrusage(RUSAGE_CHILDREN, &myusage);
    
    cout << "CPU System Time: " << (myusage.ru_stime.tv_sec * 1000.0 
    + myusage.ru_stime.tv_usec/1000.0)<< "milliseconds"<< endl;
    
    cout << "CPU User Time: " << (myusage.ru_utime.tv_sec * 1000.0 
    + myusage.ru_utime.tv_usec/1000.0) << "milliseconds"<<endl;
    
    cout << "Wall Clock Time: " << ((end.tv_sec - start.tv_sec)*1000.0) + 
    ((end.tv_usec - start.tv_usec)/1000.0) << " milliseconds"<< endl;
    
    cout << "Number of times the process was preempted involuntarily: " << myusage.ru_nivcsw
    - totalusage.ru_nivcsw << endl;
    
    cout << "Number of times process gave up the CPU voluntarily: " << myusage.ru_nvcsw -
    totalusage.ru_nvcsw << endl;
    
    cout << "Number of major page faults: " << myusage.ru_majflt - totalusage.ru_majflt<< endl;
    
    cout << "Number of minor page faults: " << myusage.ru_minflt - totalusage.ru_minflt<< endl;
    
    totalusage = myusage;
    }

}
