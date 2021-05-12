#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>


using namespace std;

#define MAXCHUNK 8192

char * mapfile;

//this function will use mmap as opposed to read - to get the answer
int mmapHelp(int file, int size)
{
   int printCount=0;
   mapfile = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, file, 0);
   for (int i = 0; i < size;i++)
   {
   //mmap mpas the enitirety of the file so we must iterate through each 
   	if (isspace(mapfile[i]) || isprint(mapfile[i]))
   	{
   	//if not a space and it is printable then increment
   		printCount++; 
   	}
   }
  
   return printCount;
}

//this function uses read to iterate through bytes
int readHelp(int file, int chunkSize)
{
   int printCount=0;
   char buff[chunkSize]; //create char array with the size of the chunks
   int readResult=0;
  
   while((readResult = read(file, buff, chunkSize))>0)
   {
      for(int i=0;i<readResult;i++)
      {
         if(isspace(buff[i]) || isprint(buff[i]))
         {
         //go through each element of the chunked array
            printCount++;
         }
         
      }
   }
   return printCount;
}


int main(int argc, char* argv[])
{
   
   int chunkSize = 1024; //initial chunk size
   bool mmapTest = false;
   int file = open(argv[1], O_RDONLY);
   int printCount=0;
   
   if(argc<2)
   {
   //if we have less than 2 arguments - not valid
      cout << "Please enter the correct number of arguments\n";
      cout << "It should be the form './proj4 srcfile optional:[size|mmap]"<<endl;
      exit(1);
   }
   
   if(file < 0) //file error
   {
      cerr << "Error opening file, please make sure the file name is written correctly"<<endl;
      exit(1);
   }
   
   if(argc>2) //if we have greater than 2 arguments - mmap and chunk
   {
  
      if(strcmp(argv[2],"mmap")==0)
      {
         mmapTest=true; // set mmap to true
      }
      else
        {
         chunkSize = atoi(argv[2]); //we have chunks, set chunkSize
      }
   }
   

   struct stat stats; //this will give us statistics for our file
   
   if(fstat(file, &stats)<0) //file size less than 0
   {
      cerr<<"Error getting file size"<<endl;
      exit(1);
   }
   
   if(chunkSize>MAXCHUNK) //exceeding max chunk size
   {
      cout << "The chunk size limit is " << MAXCHUNK <<", setting chunk size to "<<MAXCHUNK<<endl;
      chunkSize = MAXCHUNK;
   }
   if(chunkSize<1) //negative chunk size
   {
      cerr << "Please enter a positive chunk size. You entered: "<<argv[2]<<endl;
      exit(1);
   }
   if(mmapTest==true)
   {
   //use mmap instead of read
      printCount = mmapHelp(file, stats.st_size);
   }
   else
    {
      //use read instead of mmap;
      printCount = readHelp(file, chunkSize);
   }

   int resulting = ((double)printCount/stats.st_size)*100;
   cout<<printCount<<" printable characters out of "<<stats.st_size<<" bytes, "<<resulting<<"%"<<endl;
   close(file);

   exit(0);
}
