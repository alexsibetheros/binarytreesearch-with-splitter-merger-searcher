#define Max_buff_size 256

#include 	<stdlib.h>
#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include 	<sys/times.h> 
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<ctype.h>
#include	<fcntl.h>
#include 	"record.h"
#include 	"timeRec.h"

int main(int argc, char *argv[])
{
	/*
	1-depth
	2-binary file
	3-start
	4-finish	
	5-pipe
	6-query
	*/
	
	timeRec timing,timing2,timetemp;/*Structs for times, 2 searchers and Average*/
	record rec;
	
 	int status,fd,fd2,FD,fd3,fd4, ret_val, count;
 	int rec_num,rec_num2,temp,start1=0,start2=0;	
	char depthstring[Max_buff_size];	
	char pipe1[20],pipe2[20],pipe3[20],pipe4[20],start[20],stop[20],start_2[20],stop_2[20];
	
	
	/*Data from Command Line*/
	int depth= atoi(argv[1]);
	int tempstart= atoi( argv[3]);
	int tempfinish=atoi(argv[4]);
	FD=open(argv[5],O_WRONLY);
	
	snprintf(depthstring, Max_buff_size, "%d", depth-1);/*convert depth to string to give as argument*/
	
	/*Calculate where merger/searchers should look*/
	temp=tempfinish-tempstart;
	if( temp % 2 ==1)
	{
		rec_num= temp/2+1 ;
		rec_num2= temp/2 ;
	}
	else
	{
		rec_num=temp/2;
		rec_num2=temp/2;
	}	
	start1=tempstart;
	start2=tempstart+rec_num;
	snprintf(start, 20, "%d", start1);
	snprintf(stop, 20, "%d", rec_num+start1);
	snprintf(start_2, 20, "%d", start2);
	snprintf(stop_2, 20, "%d", rec_num2+start2);			
	/*Also convert to string to give as arguments*/
	
	if(depth>1){	
		
		sprintf(pipe3, "left%d", getpid());
		ret_val= mkfifo(pipe3,0666);
		if( (ret_val== -1) && (errno != EEXIST)){ perror("error creating pipe"); exit (1);}
		
		switch(fork()){
   			case -1: 
            			perror("fork"); exit(1);
   				break;
   			case 0 :
          			execlp("./merger","merger",depthstring,argv[2],start,stop,pipe3,argv[6],(char*)NULL );
           			
				break;           
   			default: /*Double Switch allows processes to create a processes and then continue creating a another process*/
			
				sprintf(pipe4, "right%d", getpid());
				ret_val= mkfifo(pipe4,0666);
				if( (ret_val== -1) && (errno != EEXIST)){ perror("error creating pipe"); exit (1);}
				
   				switch(fork()){
   					case -1: 
            					perror("fork"); exit(1);
   						break;
   					case 0 : 
          					execlp("./merger","merger",depthstring,argv[2],start_2,stop_2,pipe4,argv[6],(char*)NULL );	
						break;           
   					default: 
					
																
						fd3=open(pipe3,O_RDONLY);				
						read(fd3,&rec, sizeof(record));
						while((rec).id!=-1)
						{
						
							write(FD,&rec,sizeof(record));
							read(fd3,&rec, sizeof(record));
						
						}
						read(fd3,&timing, sizeof(timeRec));/*After structs, 1 extra record with Times is sent inside pipe from searcher*/
						
						fd4=open(pipe4,O_RDONLY);				
						read(fd4,&rec, sizeof(record));
						while((rec).id!=-1)
						{
						
							write(FD,&rec,sizeof(record));
							read(fd4,&rec, sizeof(record));
					
						}
						read(fd4,&timing2, sizeof(timeRec));/*Get time from second searcher*/
						
						(rec).id=-1;
						write(FD,&rec,sizeof(record));	/*Sent end signal for records through pipe*/
												
						timetemp.Cpu_time=(double)((timing.Cpu_time+timing2.Cpu_time) /2);
						timetemp.Real_time=(double)((timing.Real_time+timing2.Real_time) /2);
						write(FD,&timetemp,sizeof(timeRec));	/*Send Average Time of searchers*/	
						
						wait(&status);
						wait(&status);						
						close(fd4);
						remove(pipe3);
						
   				}
   				close(fd3);
   				remove(pipe4);		
   		}
	}
	else
	{
		/*Create Pipe*/
		sprintf(pipe1, "left%d", getpid());
		ret_val= mkfifo(pipe1,0666);
		if( (ret_val== -1) && (errno != EEXIST)){ perror("error creating pipe"); exit (1);}
		
		switch(fork()){
   			case -1: 
            			perror("fork"); exit(1);
   				break;
   			case 0 :
          			execlp("./searcher","searcher",argv[2],pipe1,start,stop,argv[6],(char*)NULL );		
				break;           
   			default:			
				/*Create pipe to communicate between Merge Processes and Searcher Processes*/
				sprintf(pipe2, "right%d", getpid());
				ret_val= mkfifo(pipe2,0666);
				if( (ret_val== -1) && (errno != EEXIST)){ perror("error creating pipe"); exit (1);}
				
				switch(fork()){
   					case -1: 
            					perror("fork"); exit(1);
   						break;
   					case 0 :
          					execlp("./searcher","searcher",argv[2],pipe2,start_2,stop_2,argv[6],(char*)NULL );		
						break;           
   					default:		
																
						fd=open(pipe1,O_RDONLY);				
						read(fd,&rec, sizeof(record));

						while((rec).id!=-1)/*Read all the structs inside Pipe*/
						{							
							write(FD,&rec,sizeof(record));
							read(fd,&rec, sizeof(record));						
						}
						read(fd,&timing, sizeof(timeRec));/*After structs, 1 extra record with Times is sent inside pipe*/
					
						fd2=open(pipe2,O_RDONLY);				
						read(fd2,&rec, sizeof(record));
						while((rec).id!=-1)
						{							
							write(FD,&rec,sizeof(record));
							read(fd2,&rec, sizeof(record));						
						}
						read(fd2,&timing2, sizeof(timeRec)); /*Get time from second searcher*/
						(rec).id=-1;
						write(FD,&rec,sizeof(record)); /*Sent end signal for records through pipe*/
						
						timetemp.Cpu_time=(double)((timing.Cpu_time+timing2.Cpu_time) /2);
						timetemp.Real_time=(double)((timing.Real_time+timing2.Real_time) /2);
						write(FD,&timetemp,sizeof(timeRec)); /*Send Average Time of searchers*/
					
						wait(&status);	
						wait(&status);
						close(fd);					
						remove(pipe1);		
				}
				close(fd2);
				remove(pipe2);
		}	
	}
return 0;
}



