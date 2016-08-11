#include <linux/fs.h>    
#include <linux/init.h>    
#include <linux/module.h>    
#include <linux/kernel.h>    
#include <linux/irq.h>    
#include <asm/irq.h>    
#include <asm/io.h>    
 #include <linux/interrupt.h>    
#include<linux/uaccess.h>
#include<linux/gpio.h>
#include<linux/device.h>
#include<linux/io.h>
    #include <linux/miscdevice.h>    
    #include <linux/cdev.h>    
    #include <asm/uaccess.h>    
    #include <mach/hardware.h>    
    #include <linux/platform_device.h>    
  //  #include <mach/regs-gpio.h>    
  //  #include <mach/gpio-bank-n.h>    
    #include <plat/gpio-cfg.h>    
        
    #define DEVICE_NAME "button_irq"    
  volatile unsigned long*gpccon=NULL;
   volatile unsigned long*gpcdat=NULL;
   volatile unsigned long*gpcpud=NULL;
           
    static int button_irq_open(struct inode *inode,struct file *filp)    
    {    
    
     /*设置GPC6为输出引脚*/
	*gpccon &= ~( 0xf << (4*6));
	/*GPC6输出di电平*/
	*gpcdat &=~ (1<<6);
	*gpcpud|=(1<<12);
	*gpcpud&=~(1<<13);     
        return 0;    
    }    
    static int button_irq_release(struct inode *inode,struct file *filp)    
    {    
        return 0;    
    }    
    static irqreturn_t  button_irq_inter(int nr,void *devid)  //中断处理函数  
    {    
        static int count=0;    
        count ++;    
        printk("The button1 is sucessed! count= %d\n",count);    
        return IRQ_RETVAL(IRQ_HANDLED);    
    }    
    static int button_irq_init()    
    {    
        unsigned int ret;    
        ret = request_irq(IRQ_EINT(0),button_irq_inter,IRQF_TRIGGER_HIGH,"KEY0",NULL);  //中断请求  
    if(ret==0)  
        printk("request_irq was sucessed!\n");  
        return 0;    
    }   
    
    
static ssize_t hw_read ( struct file* filp, char __user* buf, size_t count, loff_t* f_pos )
{

			/*发送给用户空间*/
		 copy_to_user (buf,&count,1);
		 count=0;			
	return 1;
	
}


     
    static struct file_operations dev_fops =    
    {    
        .owner = THIS_MODULE,    
        .open = button_irq_open,
        .read=hw_read,    
        .release = button_irq_release,    
        
    };  
      
    static struct miscdevice misc =    
    {    
        .minor = MISC_DYNAMIC_MINOR,    
        .name = DEVICE_NAME,    
        .fops = &dev_fops,    
    };    
    static int __init dev_init()    
    {    
        int ret=0;    
        ret = misc_register(&misc);    
        printk("dev_init return ret:%d\n",ret);  
        gpccon=(volatile unsigned long*)ioremap(0x7F008040,16);  
        gpcdat=gpccon+1;  
        gpcpud=gpccon+2; 
        
        button_irq_init();    
        return 0;    
    }    
    static void __exit dev_exit()    
    {    
        misc_deregister(&misc);    
        free_irq(IRQ_EINT(0),NULL);    
    }    
        
    module_init(dev_init);    
    module_exit(dev_exit);    
        
    MODULE_LICENSE("GPL");    
    MODULE_AUTHOR("MUGE0913");    
