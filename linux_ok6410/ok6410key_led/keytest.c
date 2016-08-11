 #include <stdio.h>
 #include <stdlib.h>
#include <stdint.h>
 #include <unistd.h>
 #include <sys/ioctl.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <sys/select.h>
 #include <sys/time.h>
 #include <errno.h>
#include <getopt.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
//#include<char/led_modules.c>

#define LED_ON 1
#define LED_OFF 0

 int main(void)
 {
	int buttons_fd;
	int on;
	unsigned int led_no;
     char buttons[6] = {'0', '0', '0', '0', '0', '0'};
     buttons_fd = open("/dev/keyset", 0);
     if (buttons_fd < 0) {
    perror("open device buttons");
    exit(1);
     }

     for (;;) 
{
    char current_buttons[6];
    int count_of_changed_key;
    int i;
	if (read(buttons_fd, current_buttons, sizeof current_buttons) != sizeof current_buttons) {
         		  perror("read buttons:");
         		  exit(1);
   			 }
    for (i = 0, count_of_changed_key = 0; i < sizeof buttons / sizeof buttons[0]; i++) {
           if (buttons[i] != current_buttons[i])
		 {
               	  buttons[i] = current_buttons[i];
                 printf("%skey %d is %s", count_of_changed_key? ", ": "", i+1, buttons[i] == '0' ? "up" : "down");
		
                 count_of_changed_key++;
		ioctl(buttons_fd,buttons[i]-0x30,i);
		printf("\n The key %d type is %d",i,buttons[i]-0x30);
	}
    }
    if (count_of_changed_key)
	 {
           printf("\n");
  	 }
     }
    close(buttons_fd);
    return 0;
 }
