// must run with -lpthread or -pthread

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define RANGE 1
#define ALLDONE 2
#define MAXTHREAD 10

using namespace std;


struct msg
{

    int iSender; //sender of message
    int type; //message type
    int value1; //first value
    int value2; //second value
};

struct msg mailboxes[MAXTHREAD+1];

sem_t sendsem[MAXTHREAD+1];
sem_t recsem[MAXTHREAD+1];

pthread_t Pthread[MAXTHREAD+1];


int threadCount = 0;
int total = 1;
int mailIdentifiers[MAXTHREAD+1] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};


void SendMsg(int iTo, struct msg *pMsg)
{

    //cout<<"we are in sendMsg"<<endl;
    sem_wait(&sendsem[iTo]);
    mailboxes[iTo] = *pMsg;
    sem_post(&recsem[iTo]);
}

void RecvMsg(int iFrom, struct msg *pMsg)
{
    //cout<<"we are in recvMsg"<<endl;
    sem_wait(&recsem[iFrom]);
    *pMsg = mailboxes[iFrom];
    sem_post(&sendsem[iFrom]);
}

//thread routines
void* threadHelp(void *arg)
{

    msg message;
    int mailID = *(int*)arg;

    RecvMsg(mailID, &message);

    int total = 0;
    bool tester = true;
    int i = 0;
    i = message.value2; // might not need
    int val = message.value2;

    for (i = message.value1; i <= val; i++)
    {
        total = total + i;
    }
    if (tester) 
    {
    //cout << "testing" << endl;
    message.iSender = mailID;
    message.type = ALLDONE;
    message.value1 = total;
    message.value2 = total;
    SendMsg(0, &message);
    }

    return 0;
}

void *parentHelp(void *arg)
{
    //send messages to all thread mailboxes
    int sum = 0;
    int starting = 1;
    bool test = true;
    int i = 0;
    int holder = threadCount;
    int end = 0;
    int remainder = 0;
    //cout<<"testing before arbitrary"<<endl;
if(test) //try to remove if program doesnt break without it
{
	//cout<<"testing after the arbitrary"<<endl;
    for (i=1; i <= holder; i++)
    {
        
        if (i > 1)
        {
            starting = end + 1; 
        }

	remainder = total % holder;
        end = (starting - 1) + (total/holder);
       
        
        if (i <= remainder)
        {
            end++; // increment ending
        }

        struct msg messageO;
        messageO.iSender = 0;
        messageO.type = RANGE;
        messageO.value1 = starting;
        messageO.value2 = end;
        SendMsg(i, &messageO);
    }
   }

    
    for (int i = 1; i <= holder; i++)
    {
    //wait for messages 
        struct msg message;
        bool tester = true; // arbitrary tester to make sure i can get into loops bc c++ is weird, try to remove after
        RecvMsg(0, &message);
        
        if (message.type == ALLDONE && tester == true)
        {
            sum = sum + message.value1;
        }
    }

    cout << "The total for 1 to " << total << " using " << holder <<
         " thread(s) is " << sum << endl;
    return 0;
}

void semThreadWork()
{
int valueTest = 0;
sem_t send[MAXTHREAD+1];
sem_t rec[MAXTHREAD+1];
pthread_t Pthr[MAXTHREAD+1];
if(valueTest ==0)
{
	for (int i = 0; i <= threadCount; i++)
    {
        sem_init(&recsem[i], 0, 0);
    }
    
    for (int i = 0; i <= threadCount; i++)
    {
        sem_init(&sendsem[i], 0, 1);
    }

    pthread_create(&Pthread[0], NULL, parentHelp, &mailIdentifiers[0]);

    for (int i = 1; i <= threadCount; i++)
    {
        pthread_create(&Pthread[i], NULL, threadHelp, &mailIdentifiers[i]);
        
    }

    for (int i = 0; i <= threadCount; i++)
    {
        int var;
        void* sendBack;
        var = pthread_join(Pthread[i], &sendBack);
    }

    for (int i = 0; i <= threadCount; i++)
    {
        sem_destroy(&sendsem[i]);
        sem_destroy(&recsem[i]);
    }
    
   }
   valueTest++; 

}

int main(int argc, char *argv[])
{
    // if there are 3 arguments
    int numofarguments = 0;
    int arguments = argc;
    int threader = 1;
    int holder = threadCount;
    int argone = atoi(argv[1]);
    int argtwo = atoi(argv[2]);
    if (argc == 3)
    {
    	if (threader == 1)
    	{
        	total = argtwo; 
        	if ((argone < MAXTHREAD)&&(argone > 0)&&(threader ==1)) 
        	{
           	 
            		threadCount = argone;
        	}
        	else if (argone <= 0) // negative thread
        	{
           	 	std::cout << "Cannot enter a negative thread count. "<<endl;
           	 	return 1;
                
        	}
        	else
        	{
            	
            		threadCount = MAXTHREAD;
        	}
        }
    }
    else 
    {
    	cout<<"You must enter all arguments"<<endl<<"It should look like ./addem #ofthreads #tocountto"<<endl;
    }
    
    
    semThreadWork();

    return 0;
}
