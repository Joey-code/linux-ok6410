#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(void)
{
    int fd;
    int val;
    int i;
    char buf[1];
   // int press_cnt[6];

    fd = open("/dev/button_irq",0);
    if(fd<0)
     {
      printf("open devie error\n");
      return -1;
    }
    while(1) 
    {
      val = read(fd,buf,1);
      if(val<0)
       {
        printf("read error\n");
        continue;
      	}

         printf("count%d pressed\n",buf[0]);
        
      }  
      
    
    close(fd);
    return 0;

}
