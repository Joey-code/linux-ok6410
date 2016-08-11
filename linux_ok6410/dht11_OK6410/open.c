#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main ( void )
{
	int fd ,fd1;
	int retval ;
	char buf[5] ;
	char buf1[0];
	
		{
			fd = open ( "/dev/dht11" , O_RDONLY);
			if ( fd == -1 )
			{
				perror ( "open dht11 error\n" ) ;
				exit ( -1 ) ;
			}		
		}
		
		
			{
	 	   	fd1 = open("/dev/button_irq",0);
			if ( fd == -1 )
				{
				perror ( "open dht11 error\n" ) ;
				exit ( -1 ) ;
				}		
			}

	sleep ( 1 ) ;
	while ( 1 )
	{
		sleep ( 1 ) ;
		{
			retval = read ( fd , buf , 5 );
		if ( retval == -1 )
		{
			perror ( "read dht11 error" ) ;
			printf ( "read dht11 error" ) ;
			exit ( -1 ) ;
		}
		printf ( "\nHumidity:%d.%d\%\nTemperature:%d.%d C\n", buf[0], buf[1], buf[2], buf[3] ) ;
		if(buf[0]>80) 
			printf("window is close !\n");
		else
			printf("window is open !\n");	
		printf("===================\n");	
		}
	
	
			{
			 retval = read(fd1,buf1,1);
      		if(retval<0)
      		 {
       			 printf("read hongwai error\n");
       			 continue;
        		}
        	if(buf1[0]==1)	
       		  printf("Someone is invading !\n");
        	else 
        	  printf("All going well !\n");
			}
		
		
	}
	close ( fd ) ;
	close ( fd1) ;
}

