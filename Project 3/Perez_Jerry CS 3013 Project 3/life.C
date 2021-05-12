#include <cstdio>
#include <fstream>
#include <iostream>
#include <semaphore.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


using namespace std;


#define MAXGRID 40
#define EVEN 0
#define ODD 1

#define RANGE 1
#define ALLDONE 2
#define GO 3
#define GENDONE 4
#define STOP 5


struct msg 
{
int iSender;   //sender of the message
int type; //type of message
int value1; //first value
int value2; //second value
};

struct msg ** mailboxes;
struct msg* stop = new msg;

//array of semaphores for sending and receiving
sem_t * sendSem;
sem_t * recSem;

//this is a 2d int array to hold 0's and their position
//same goes for 1's
int** zeroGen;
int** oneGen;

//rows and columns
int rows;
int columns;

//contains a count of all generations
int allGen;


void SendMsg(int iTo, struct msg *Msg) 
{
	sem_wait(&sendSem[iTo]);
	mailboxes[iTo] = Msg;
	sem_post(&recSem[iTo]);
}


void RecvMsg(int iFrom, struct msg *Msg) 
{
	sem_wait(&recSem[iFrom]);
	*Msg = *mailboxes[iFrom];
	delete mailboxes[iFrom];
	sem_post(&sendSem[iFrom]);
}

bool zerogenerationTest (int zeroGen) 
{
	if (zeroGen == EVEN)
	{
		return true;
	}
	
	return false;
}
bool onegenerationTest (int oneGen) 
{
	if (oneGen == ODD)
	{
		return true;
	}
	
	return false;
}
bool stopChecker (int checker) 
{
	if (checker == STOP)
	{
		return true;
	}
	return false;
}
bool doneChecker (int check)
{
	if (check == ALLDONE)
	{
		return true;
	}
	
	return false;
}

int setToZero(int val) 
{
	val = 0;
	return val;
}

bool checkZero(int val)
{
	if (val == 0)
	{
		return true;
	}
	return false;
}

//this function is used to print desired output for initial generation
void printOut(int generation) 
{
	for(int i = 0; i<rows; i++)
	{
	for(int j=0;j<columns;j++)
	{

		if(onegenerationTest(generation))
		{
			cout << oneGen[i][j] << " ";
		}
		else if (zerogenerationTest(generation))
		{
			cout << zeroGen[i][j] << " ";
		}
		}
	std::cout << endl;
	std::cout.flush();
	}
}

//this will count the neigboring cells, being careful not to double count and that we stay within the boundaries of the initial grid

int countNeighboring(int row, int column, int generation)  
{
	int neighborCount = 0; 
	int tempRow = row;
	int tempCol = column;
	int tempGen = generation;
	for(int i=tempRow-1;i<tempRow+2; i++)
	{
		for(int j=tempCol-1;j<tempCol+2;j++)
		{
			if(i>-1 && j>-1 && i<rows && j<columns)
			{
				if(!(i==tempRow && tempCol==j))
				{
					if(zerogenerationTest(tempGen))
					{
						neighborCount= neighborCount +zeroGen[i][j];
					}
					else if (onegenerationTest(tempGen))
					{
						neighborCount=neighborCount + oneGen[i][j];
					}
				}
			}
		}
	}
	return neighborCount;
}


void* rowProcess(void* Id) 
{
	int constSt=0;
	int zeroD=0;
	int surviveTester = 0;
	surviveTester = -1;
	int divideIt = 0;
	bool tester = true;
	int threadIdent = (int)(intptr_t)Id;
	struct msg recMes;
	RecvMsg(threadIdent, &recMes);
	int firstR=recMes.value1;
	int lastR =recMes.value2;
	
	
if (tester) //can remove later, just make sure it works
{
	//cout<<"testing"<<endl;
	for(int i=1;i<=allGen;i++)
	{
		int checkT;
		divideIt = i%2;
		RecvMsg(threadIdent, &recMes);
		checkT = recMes.type;
		if(stopChecker(checkT))
		{
			break;
			//exit(0);
		}
	constSt=1;
	zeroD=1;
	for(int row=firstR;row<lastR;row++)
	{
		for(int col=0;col<columns;col++)
		{
			int survive = -1;
			int neighbors = countNeighboring(row, col, (i-1)%2);
			
			if(neighbors==3)
			{
				survive=1;
			}
			else if(neighbors<2 || neighbors>3)
			{
				survive=setToZero(survive);
			}
			if(divideIt==1)
			{
				if(survive==surviveTester)
				{
					oneGen[row][col]=zeroGen[row][col];
				}
				else
				{
					oneGen[row][col]=survive;
				}
				if(oneGen[row][col]!=0)
				{
					zeroD=setToZero(zeroD);
				}
			}
			else
			{
				if(survive==surviveTester)
				{
					zeroGen[row][col]=oneGen[row][col];
				}
				else
				{
					zeroGen[row][col]=survive;
				}
				if(zeroGen[row][col]!=0)
				{
					zeroD=setToZero(zeroD);
				}
			}
			if(oneGen[row][col]!=zeroGen[row][col])
			{
				constSt=setToZero(constSt);
			}
		}
	}

	struct msg* doneG = new struct msg;
	doneG->iSender = threadIdent;
	doneG->type=GENDONE;
	doneG->value1=zeroD;
	doneG->value2=constSt;
	SendMsg(0, doneG);
	}
}

struct msg* returnM = new struct msg;
returnM->iSender = threadIdent;
returnM->type=ALLDONE;
SendMsg(0, returnM);
return 0;
}


int main(int argc, char* argv[]) //--------------------------------
{
	int totalT=0;
	allGen=0;
	int arg1 = atoi(argv[1]);
	string arg0 = argv[0];
	int arg3 = atoi(argv[3]);
	string arg2 = argv[2];
	string arg4 = argv[4];
	string arg5 = argv[4];
	bool printAlert = false;
	bool inputAlert = false;
	bool testHere = true;
	int generation;
	string exitCode = "the maximum number of generations being reached";
if(testHere)
{
	if(argc<4) 
	{
		cout << "Please enter the correct number of arguments"<<endl;
		cout << "The correct entry form looks like: ./life #ofthreads filename #ofgenerations 'print' 'input'"<<endl;
		exit(1);
	}
	else if(arg1 < 1) 
	{
		cout << "The number of threads must be positive"<<endl;
		exit(1);
	}
	else
	{
		totalT = arg1;
	}
	if(arg3 < 1)
	{
		cout << "The number of generations must be positive"<<endl;
		exit(1);
	}
	else
	{
		allGen = arg3;
	}
	if(argc > 4)
	{
		if(strcmp(argv[4],"y")==0)
		{
			printAlert=true;
		}
	}
	else if(argc>5)
	{
		if(strcmp(argv[5],"y")==0)
		{
			inputAlert=true;
		}
	}
}

	ifstream fileIn;
	string reader;
	const char* file = argv[2];

	fileIn.open(file);
	rows=0;
	columns=0;
	if(!fileIn)
	{
		cerr << "File opening error."<<endl;
		exit(1);
	}
	while(getline(fileIn, reader)) //parse
	{
		istringstream ss (reader);
		int z = 0;
		int count =0;
		while(ss>>z)
		{
			count++;
		}
		if(checkZero(columns))
		{
			columns = count;
		}
		if(!checkZero(count))
		{
			rows++;
		}
	}
	fileIn.close();

	if(rows>MAXGRID || columns>MAXGRID)
	{
		cerr << "The grid size exceeds the max size."<<endl;
		exit(1);
	}
	zeroGen = new int* [rows];
	oneGen = new int* [rows];
	for(int i=0;i<rows;i++)
	{
		zeroGen[i]=new int[columns];
		oneGen[i]=new int[columns];
	}

	fileIn.open(file);
	for(int i=0; i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			int z;
			fileIn >> z;
			zeroGen[i][j]=z;
			oneGen[i][j]=0;
		}
	}
	fileIn.close();

	if(totalT > rows)
	{
		totalT = rows;
	}
	
	//add one for parent

	mailboxes = new struct msg * [totalT+1];
	sendSem = new sem_t[totalT+1];
	recSem = new sem_t[totalT+1];
	for(int i = 0; i<totalT+1; i++)
	{
		sem_init(&sendSem[i], 0, 1);
		sem_init(&recSem[i], 0, 0);
	}

	pthread_t children [totalT];

	int counter = 0;
	int range = rows/totalT;
	int remainder = rows % totalT;
	
	for(int i=0; i<totalT; i++)
	{
		int start = counter;
		int end = start + range;
		bool anotherTest = true;
	if (anotherTest) //remember to eliminate if needed
	{
		if(remainder > 0)
		{
			end++;
			remainder--;
		}
		int res = i+1;
		void* vall=(void*) res; //try to get rid of cast error
		pthread_create(&children[i], NULL, rowProcess, vall);
	
	}
	struct msg* sendMess = new struct msg;
	sendMess->iSender = 0;
	sendMess->type = RANGE;
	sendMess->value1 = start;
	sendMess->value2 = end;
	SendMsg(i+1, sendMess);

	counter=end;
	}

	int threadOver = 0;
	int lastGen = 0;
	for(int i=0;i<allGen;i++)
	{
		if(checkZero(i) || printAlert )
		{
			if(checkZero(i))
			{
				cout<<"This is how we started: "<<endl;
			}
			else
			{
				cout<<"Generation "<< i << ":\n";
			}
			
			
		printOut(i%2);
		cout<<endl;
		}
		if(printAlert && inputAlert )
		{
			cin.get();
		}

		for(int thread=0;thread<totalT;thread++)
		{
			struct msg* sendIt = new msg;
			sendIt->iSender=0;
			sendIt->type = GO;
			SendMsg(thread+1, sendIt);
		}

		int dead=1;
		int conS=1;
		for(int thread=0;thread<totalT;thread++)
		{
			struct msg recv;
			int type;
			RecvMsg(0,&recv);
			type=recv.type;
			if(checkZero(recv.value1))
			{
				dead=0;
			}
			if(doneChecker(type))
			{
				threadOver++;
			}
			if(checkZero(recv.value2))
			{
				conS=0;
			}
		}
	lastGen = i+1;

	if(conS==1)
	{
		for(int thread=0;thread<totalT;thread++)
		{
			stop->iSender = 0;
			stop->type=STOP;
			SendMsg(thread+1, stop);
		}
	exitCode = "the pattern of organisms being unchanged from the previous generation ";
	lastGen = i;
	break;
	}
	if(dead == 1)
	{
		for(int thread=0;thread<totalT;thread++)
		{
			stop->iSender = 0;
			stop->type=STOP;
			SendMsg(thread+1, stop);
		}
	exitCode = "all organisms being dead ";
	break;
	}
	}


	for(int i=0;i<totalT-threadOver;i++)
	{
		struct msg recievedMessage;
		int type;
		while(!doneChecker(type))
		{
			RecvMsg(0,&recievedMessage);
			type=recievedMessage.type;
		}
	}

cout<<"The game ended due to: "<<exitCode<<"over the span of "<<lastGen<<" generations with an end state of: "<<endl;
printOut(lastGen%2);


exit(0);
}
