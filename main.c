#define Max_buff_size 256

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include 	<sys/times.h> 
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<ctype.h>
#include	<fcntl.h>
#include	"record.h"
#include 	"timeRec.h"
#include 	<stdlib.h>

int main(int argc, char *argv[])
{

		
	int fd; /*For open pipes*/
	int status; /*Used for waiting for processes*/
	timeRec timing; /*Struct used for moving time through processes*/
	record rec; /*Struct used from moving records through the processes*/
	int depth; /*From Command Line*/
	char filename[20]; /*From Command Line*/
	int ret_val;
	char pipe[20];	
	long lSize;
   	int records;
   	char finish[Max_buff_size];
   	char start[Max_buff_size];
   	char query[20];
   	int k;
   	char reading;	
   	char str1[20],str2[10];
	char* column_para;
   	int column;
   	int num;
   	char line[500];
   	char string[5];
   	int flagR;
   	FILE *fpb;
   	FILE * pFile, *pFile2;
	int choice;
	if(argc == 5)
    	{
        	if( strcmp(argv[1], "-d")==0 )
		{	
			depth=atoi(argv[2]);
			if(depth<0){printf("Invalid depth\n");return 0;}
			else if(depth>5){printf("Depth too large");return 0;}
		}
		else{printf("Sorry, command line argument 1 wrong, please try again\n");return 0;}	
		if( strcmp(argv[3], "-r")==0 )
		{
			strcpy(filename,argv[4]);			
		}
		else{printf("Sorry, command line argument 3 wrong, please try again\n");return 0;}
	}
	else{ printf("Sorry, wrong command line arguments, please run again\n");return 0;}	
	printf("------Start----\n");
	
	
	
	printf("\nPlease Give Action: 1)Query 2)Exit \n--");
   	scanf("%1d",&choice);
   	while(getchar() !='\n');
   	
   	while(choice==1){
   	
   	/*Create Pipe with name of Process+ string*/		
	sprintf(pipe, "original%d", getpid());
	ret_val= mkfifo(pipe,0666);
	if( (ret_val== -1) && (errno != EEXIST)){ perror("error creating pipe"); exit (1);}	
	/*Pipe Created*/
	
   	/*Open Binary File, retrieve total number of records*/
   	
   	
   	fpb = fopen (filename,"rb");
   	if (fpb==NULL){printf("Cannot open binary file\n");return 1;}
   	fseek (fpb , 0 , SEEK_END);
   	lSize = ftell (fpb);
   	rewind (fpb);
   	records = lSize/sizeof(record);
   	fclose (fpb);
	/*Binary File Closed*/   
   	
   	/*Create starting parameters for Merge Process, start at 0 end at total Records*/
   
	snprintf(finish, Max_buff_size, "%d", records);
		
	snprintf(start, Max_buff_size, "%d", 0);
   	/*Start, stop now converted to string for arguments into Merge Process*/
   
   	
   	/*Open Output file*/
	
	pFile = fopen ( "output" , "w" );
	if (pFile==NULL){printf("Cannot create output\n");return 1;}
   	/*Output file in binary coding*/
   	
   	/*Read query*/
   	
   	printf("Please Give your query: \n");
   	k=0;
   	do{
   	reading=getchar();
   	query[k]=reading;
   	k++;
   	}while ( reading!= '\n');
	query[k-1]='\0';
	/*Store query as string*/

	switch(fork()){
   			case -1: /*If fork fails*/
            			perror("fork"); exit(1);
   				break;
   			case 0 :/*Fork succesfull, call Initial Merger*/
				execlp("./merger","merger",argv[2],filename,start,finish,pipe,query,(char*)NULL );
				break;           
   			default: /*Parent's Code*/
				fd=open(pipe,O_RDONLY);	/*Pipe that communicates with Initial Merger*/			
				read(fd,&rec, sizeof(record)); /*Read first record*/
				while((rec).id!=-1) /*Read records from pipe until a special record with id=-1*/
				{						
					fprintf (pFile, "%d %s %s %d %s %d\n",rec.id, rec.firstName, rec.lastName, rec.age, rec.job, rec.salary); /*Write records to a fail in txt format*/				//printf("%d %s %s %d %s %d\n",rec.id, rec.firstName, rec.lastName, rec.age, rec.job, rec.salary);		
					read(fd,&rec, sizeof(record));
				}
				read(fd,&timing, sizeof(timeRec));/*Retrieve total time of execution from Merge Process*/		
				printf("Run time: %lf sec (REAL time) -  %lf sec (CPU time).\n",timing.Real_time, timing.Cpu_time);		
				break;
   		}
	wait(&status); /*Wait till initial Merge completes to continue*/
   	close(fd); /*Close Pipe*/
   	remove(pipe);  	
	fclose (pFile); /*Close output file*/
	
	/*Begging procedure for sort*/
	
   	sscanf(query,"%d %s %s ",&column,str1,str2); /*extract from query, column, comparison, price*/
   	switch(column){
   			case(1):
   				column_para="-nk1"; /*Parameter for Sort, -nk1 is for integers in the first column*/
   				break;
   			case(2):
   				column_para="-k2"; /*Parameter for column2, for strings*/
   				break;
   			case(3):
   				column_para="-k3";
   				break;
   			case(4):
   				column_para="-nk4";
   				break;
   			case(5):
   				column_para="-k5";
   				break;	
   			case(6):
   				column_para="-nk6";
   				break;	 				
   			default:   				
   				break;		
   		}
   		
	switch(fork()){ /*Finally call sort*/
   			case -1: 
            			perror("fork"); exit(1);
   				break;
   			case 0 :
				execlp("sort","sort",column_para,"output","-o","sorted_txt",(char*)NULL );/*sort called with column parameter, source, destination*/
				break;           
   			default: 
				
				wait(&status);/*Wait until sort finished*/	
				break;
   		}	
	
	remove("output");
	/*Open sorted file to print to stdout*/
	pFile2 = fopen ("sorted_txt","r");
   	if (pFile2==NULL){printf("Cannot open binary file\n");return 1;}
   	
   	printf("Would you like to see results?{Total records will stll be shown} \n");
   	scanf("%5s",string);
   	flagR=0;
   	if( (strcmp(string, "yes")==0) || (strcmp(string, "y")==0) || (strcmp(string, "Y")==0) || (strcmp(string, "Yes")==0) || (strcmp(string, "Nai")==0) || (strcmp(string, "NAI")==0) || (strcmp(string, "YES")==0)|| (strcmp(string, "OK")==0)|| (strcmp(string, "Ok")==0)|| (strcmp(string, "ok")==0))
   		flagR=1;
   	num=0;
	while (fgets(line, 500, pFile2) != NULL) {
		if(flagR==1)
			printf("%s",line); 
		num++;
	}
	printf("Total Records: %d\n",num);
	fclose(pFile2);
	/*After asking if user wants results printed, close file and finish*/
	
	printf("\nPlease Give Action: 1)Query 2)Exit \n--");
   	scanf("%1d",&choice);
   	while(getchar() !='\n');
	}
	printf("----END----\n");
	return 0;
}
