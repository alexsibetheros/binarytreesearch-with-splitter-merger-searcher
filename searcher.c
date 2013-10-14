#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<ctype.h>
#include	<fcntl.h>
#include 	<sys/times.h> 
#include 	"record.h"
#include 	"timeRec.h"

int compare_record(record rec, char query[]);


int main(int argc, char *argv[])
{ 
	/*
	Argv Arguments:
	1)Binary File
	2)Pipe Name
	3)Start
	4)Stop
	5)QUery
	*/
	double t1, t2, cpu_time;
	struct tms tb1, tb2;
	double ticspersec;
	timeRec timing;
	int i,j;
   	FILE *fpb;
   	int fd;
   	int start,stop;
   	char query[20];	
   	record rec;
   	
   	
	/*Necessary for calculating running times*/
	ticspersec = (double) sysconf(_SC_CLK_TCK);
	t1 = (double) times(&tb1);
	/*Running times for CPU and Real Time*/
	
   	
   	/*Extract Data from Argv*/
   	fpb = fopen (argv[1],"rb"); /*Open File*/
   	if (fpb==NULL){printf("Cannot open binary file\n");return 1;}
   	fd=open(argv[2],O_WRONLY); /*Open Pipe*/
  	start=atoi(argv[3]);
   	stop=atoi(argv[4]);
 	strcpy(query,argv[5]);
 	
 	
   
 	
 	/*Skip records until reached correct start*/
   	for (i=0; i<start ; i++)
   	{
       		fread(&rec, sizeof(record), 1, fpb);
   	}
  	/*Start checking records*/
   	for (j=i; j<(stop) ; j++)
 	{     
 		fread(&rec, sizeof(record), 1, fpb);
 		//printf("%d %s %s %d %s %d\n",rec.id, rec.firstName, rec.lastName, rec.age, rec.job, rec.salary);	
      		if( compare_record(rec, query)==0 )/*Check if current record fits the criteria*/
      			write(fd,&rec,sizeof(record));
   	}
   	rec.id=-1; /*After all records read, send an modified previous record to signal end*/
   	write(fd,&rec,sizeof(record));
   
   	fclose (fpb);
	

	/*Calculate running time*/
	t2 = (double) times(&tb2);
	cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) -(tb1.tms_utime + tb1.tms_stime));

	timing.Cpu_time=(double)(cpu_time / ticspersec);
	timing.Real_time=(double)((t2 - t1) / ticspersec);
	write(fd,&timing,sizeof(timeRec));
	/*After finding time, fill struct with times and send through pipe*/
	
	close(fd);
	return 0;
}


int compare_record(record rec, char query[])
{
	char str1[20],str2[10];
   	int column;
   	int i=0;
   	int temp,temp1,temp2;
   	sscanf(query,"%d %s %s ",&column,str1,str2); /*Extract Data from query*/
   	
   	if( strcmp(">",str1) ==0 )  /*Find what kind of comparison is asked for*/  
   	{	
   		switch(column){
   			case(1): /*For given comparison, use on column1*/
   				temp=atoi(str2);
   				temp1=rec.id;
   				if( temp1 > temp)
   					return 0; /*Return 0 if record fits criteria*/					
   				break;
   			case(2):
   				if( strcmp(rec.firstName, str2) > 0)
   					return 0;
   				break;
   			case(3):
   				if( strcmp(rec.lastName, str2) > 0)
   					return 0;
   				break;
   			case(4):
   				temp=atoi(str2);
   				temp1=rec.age;
   				if( temp1 > temp)
   					return 0;
   				break;
   			case(5):
   				if( strcmp(rec.job, str2) > 0)
   					return 0;
   				break;	
   			case(6):
   				temp=atoi(str2);
   				temp1=rec.salary;
   				if( temp1 > temp)
   					return 0;
   				break;	
   				
   			default: 
   				return 1;
   				break;  		
   		}
   		return 1;
			   		
   	}
   	else if( strcmp("<",str1) ==0 )    
   	{	
   		switch(column){
   			case(1):
   				temp=atoi(str2);
   				temp1=rec.id;
   				if( temp1 < temp)
   					return 0;
   				break;
   			case(2):
   				if( strcmp(rec.firstName, str2) < 0)
   					return 0;
   				break;
   			case(3):
   				if( strcmp(rec.lastName, str2) < 0)
   					return 0;
   				break;
   			case(4):
   				temp=atoi(str2);
   				temp1=rec.age;
   				if( temp1 < temp)
   					return 0;
   				break;
   			case(5):
   				if( strcmp(rec.job, str2) < 0)
   					return 0;
   				break;	
   			case(6):
   				temp=atoi(str2);
   				temp1=rec.salary;
   				if( temp1 < temp)
   					return 0;
   				break;	
   				
   			default:
   				return 1;
   				break;
   		
   		}
   		return 1;

   	}
   	else if( strcmp("<=",str1) ==0 )    
   	{	
   	switch(column){
   			case(1):
   				temp=atoi(str2);
   				temp1=rec.id;
   				if( temp1 <= temp)
   					return 0;
   				break;
   			case(2):
   				if( strcmp(rec.firstName, str2) <= 0)
   					return 0;
   				break;
   			case(3):
   				if( strcmp(rec.lastName, str2) <= 0)
   					return 0;
   				break;
   			case(4):
   				temp=atoi(str2);
   				temp1=rec.age;
   				if( temp1 <= temp)
   					return 0;
   				break;
   			case(5):
   				if( strcmp(rec.job, str2) <= 0)
   					return 0;
   				break;	
   			case(6):
   				temp=atoi(str2);
   				temp1=rec.salary;
   				if( temp1 <= temp)
   					return 0;
   				break;	
   				
   			default:
   				return 1;
   				break;		
   		}
   		return 1;
   	}
   	else if( strcmp(">=",str1) ==0 )    
   	{	
   		switch(column){
   			case(1):
   				temp=atoi(str2);
   				temp1=rec.id;
   				if( temp1 >= temp)
   					return 0;					
   				break;
   			case(2):
   				if( strcmp(rec.firstName, str2) >= 0)
   					return 0;
   				break;
   			case(3):
   				if( strcmp(rec.lastName, str2) >= 0)
   					return 0;
   				break;
   			case(4):
   				temp=atoi(str2);
   				temp1=rec.age;
   				if( temp1 >= temp)
   					return 0;
   				break;
   			case(5):
   				if( strcmp(rec.job, str2) >= 0)
   					return 0;
   				break;	
   			case(6):
   				temp=atoi(str2);
   				temp1=rec.salary;
   				if( temp1 >= temp)
   					return 0;
   				break;	
   				
   			default: 
   				return 1;
   				break;  		
   		}
   		return 1;
	
   	}
   	else if( strcmp("=",str1) ==0 )    
   	{	
   		switch(column){
   			case(1):
   				temp=atoi(str2);
   				temp1=rec.id;
   				if( temp1 == temp)
   					return 0;					
   				break;
   			case(2):
   				if( strcmp(rec.firstName, str2) == 0)
   					return 0;
   				break;
   			case(3):
   				if( strcmp(rec.lastName, str2) == 0)
   					return 0;
   				break;
   			case(4):
   				temp=atoi(str2);
   				temp1=rec.age;
   				if( temp1 == temp)
   					return 0;
   				break;
   			case(5):
   				if( strcmp(rec.job, str2) == 0)
   					return 0;
   				break;	
   			case(6):
   				temp=atoi(str2);
   				temp1=rec.salary;
   				if( temp1 == temp)
   					return 0;
   				break;	
   				
   			default: 
   				return 1;
   				break;  		
   		}
   		return 1;
   		
   	}
   	else if( strcmp("!=",str1) ==0 )    
   	{	
   		switch(column){
   			case(1):
   				temp=atoi(str2);
   				temp1=rec.id;
   				if( temp1 != temp)
   					return 0;					
   				break;
   			case(2):
   				if( strcmp(rec.firstName, str2) != 0)
   					return 0;
   				break;
   			case(3):
   				if( strcmp(rec.lastName, str2) != 0)
   					return 0;
   				break;
   			case(4):
   				temp=atoi(str2);
   				temp1=rec.age;
   				if( temp1 != temp)
   					return 0;
   				break;
   			case(5):
   				if( strcmp(rec.job, str2) != 0)
   					return 0;
   				break;	
   			case(6):
   				temp=atoi(str2);
   				temp1=rec.salary;
   				if( temp1 != temp)
   					return 0;
   				break;	
   				
   			default: 
   				return 1;
   				break;  		
   		}
   		return 1;
   	}  
   return 1;
}
