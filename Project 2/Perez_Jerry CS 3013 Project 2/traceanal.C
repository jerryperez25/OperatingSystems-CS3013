#include <stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include<map> // wont work correctly
#include<array> // attempted - more work, vectors?
#include<vector>

using namespace std;

struct afterCallReader
{
	//string testing; making sure that this populates values correctly
	string current;
	int count;
	
};

struct totalCallReader
{
	// this will have name and count 
	string previous;
	int counterT = 0;
	vector<afterCallReader> afterCallRead;
	//vector<afterCallReader> testing; make sure that this populates values correctly.
};

struct callReader
{
	// this will have name and count 
	string sysName;
	int counter;
};


int main(int argc, char **argv)
{
	// think about forking off here:
	// parent process responsible for part 1/2
	// child process responsible for other part?
	if ((argc != 1) && (strncmp(argv[1], "seq",3)==0))
	{
	//try to eliminate if possible
	bool trythis = true;
	//set up an arbitrary string so that it is overrided after the first argument
	string previousR = "arbitrary";
	
	string lineRead;
	int n = 0;
	bool test = false;
	vector<totalCallReader> reader;
	
	while (getline(cin, lineRead))
	{
	
		//read up until the first parenthesis in order to get the command

		if(lineRead.find("(") != -1)
		{
			string currentCall; 
			currentCall = lineRead.substr(0, lineRead.find("("));

			
			if (trythis)
			{
				previousR = currentCall;
				trythis = false;
			}

			else if((trythis = false) && reader.empty())
			{

				vector<afterCallReader> vec;
				vec.push_back({currentCall,1});
				reader.push_back({previousR,1,vec});
				previousR = currentCall;
			}
			
			else
			{
			for (int firstFor = 0; firstFor < reader.size(); firstFor++)
			{
			if (reader.at(firstFor).previous == previousR)
			{
				
				test = true;
				int secondFor = 0;
				if((reader.at(firstFor).previous == previousR) && test)
				{
					reader.at(firstFor).counterT++;
				}
				
				for (secondFor = 0; secondFor < reader.at(firstFor).afterCallRead.size(); secondFor++)
				{
				if ((test = true) && reader.at(firstFor).afterCallRead.at(secondFor).current == currentCall)
				{		
			reader.at(firstFor).afterCallRead.at(secondFor).count++;
			// we want to get out of here after this is done
				break;
				}
				}
					if(secondFor == reader.at(firstFor).afterCallRead.size())
				{

				reader.at(firstFor).afterCallRead.push_back({currentCall,1});
				}
				previousR = currentCall;
				break;
			}
			
			}
			if (!test)
			{
			vector<afterCallReader> lastvec;
			lastvec.push_back({currentCall,1});
			reader.push_back({previousR,1,lastvec});
			previousR = currentCall;
			}
			
			test = false;
			
		}
		
	}
	n++;
	}
	
	
	int firstTotal;
	int secondTotal;
	
	for (int iterator = 0; iterator < reader.size();iterator++)
	{
		
		firstTotal = reader.at(iterator).counterT;
		for(int second = 0; second < reader.at(iterator).afterCallRead.size(); second++)
		{
			secondTotal = firstTotal + reader.at(iterator).afterCallRead.at(second).count;
		}
	}
	cout << "AAA: " << n-1 << " " << "invoked system call instances from "<< reader.size() + 1<< " unique system calls" << endl;
	
	for (int i = 0; i < reader.size(); i++)
	{
		cout<<reader.at(i).previous <<" "<< reader.at(i).counterT<<endl;
		for(int j = 0; j < reader.at(i).afterCallRead.size(); j++)
		{
			cout<<"\t"<<reader.at(i).previous<<": " << reader.at(i).afterCallRead.at(j).current<<" "<<reader.at(i).afterCallRead.at(j).count<<endl;
		}
	}
	
}		
else
{
	map<string, int> holder;
	int systCallT = 0;
	string lineRead;
	while(getline(cin, lineRead))
	{
	
		if(lineRead.find("(") != -1)
		{
			string calls;
			calls = lineRead.substr(0, lineRead.find("("));
			if (holder.count(calls))
			{
				holder[calls]++;
			}
			else
			{
				holder.insert({calls,1});
			}
			
			systCallT = systCallT + 1;
		}
	}
	
	cout << "AAA: " << systCallT << " " << "invoked system call instances from "<< holder.size() << " unique system calls" << endl;
	
	for (auto i = holder.begin(); i != holder.end(); ++i)
	{
		cout << i->first<< " "<< i->second<<endl;
	}
	}
}

