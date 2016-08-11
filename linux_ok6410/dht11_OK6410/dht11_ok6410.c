#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/device.h>

#include <linux/gpio.h>
#include <plat/gpio-cfg.h>


#define GPC_7 S3C64XX_GPC(7)

static volatile unsigned char values[4]={0,0,0,0};

static struct class * dhtdrv_class;
static struct device * dht11_drv_device;


volatile unsigned long *gpccon = NULL;
volatile unsigned long *gpcdat = NULL;


int major;

static int dht11_drv_open(struct inode *inode,struct file * file)
{
	/*设置GPC7为输出引脚*/
	*gpccon &= ~( 0xf << (4*7));
	/*GPC7输出高电平*/
	*gpcdat |= (1<<7);

	return 0;
}


static char dht11_drv_read_byte ( void )
{
	char dht11_byte ;
	unsigned char i ;
	unsigned char temp ;
	
	dht11_byte = 0 ;
	/*接收数据，并利用|和&存储到dht11_byte*/
	for ( i = 0 ; i < 8 ; i ++ )
	{
		temp = 0 ;
		/*50us传输间隔时间*/
		while ( ! (gpio_get_value ( GPC_7 ) ) )
		{
			temp ++ ;
			if ( temp > 12 )
				return 1 ;
			udelay ( 5 ) ;
		}
		temp = 0 ;
		
		/*根据接收到高电平持续的时长判断数据是0还是1*/
		while ( gpio_get_value ( GPC_7 ) )
		{
			temp ++ ;
			if ( temp > 20 )
				return 1 ;
			udelay ( 5 ) ;
		}
		/*大于6的话，也就是大于35us，数据为1，否则数据为0*/
		if ( temp > 6 )
		{
			dht11_byte <<= 1 ;
			dht11_byte |= 1 ;
		} 
		else
		{
			dht11_byte <<= 1 ;
			dht11_byte |= 0 ;
		}
	}
	return dht11_byte ;
}

static ssize_t dht11_drv_read ( struct file* filp, char __user* buf, size_t count, loff_t* f_pos )
{
	unsigned char DataTemp;
	unsigned char i;
	unsigned char err;
	char tempBuf[5];
	err = 0 ;

	/*设置GPC7为输出引脚*/
	*gpccon &= ~( 0xf << ( 4*7) );
	*gpccon |= ( 0x1 << (4*7));	
	
	/*GPC7输出低电平*/
	*gpcdat &= ~( 1<<7 );
	
	/*
	等同于
	s3c_gpio_cfgpin ( GPC_7 , S3C_GPIO_OUTPUT );
	gpio_set_value ( GPC_7 , 0 );
	*/
	
	/*延时18毫秒，用于DHT11从待机状态进入工作状态*/
	msleep ( 18 );//msleep()休眠延时函数，并不精确，实际延时时间会大于设置值
	
	/*GPC7输出高电平*/
	*gpcdat |= ( 1<<7 );
	//gpio_set_value ( GPC_7 , 1 );
	udelay ( 40 );	//udelay()忙等待函数,延迟过程中无法运行其他任务．这个延迟的时间是准确,但是浪费CPU，不适用鱼长时间延时。
	
	/*设置GPC7为输入引脚*/
	*gpccon &= ~( 0xf << ( 4*7 ));
	/*
	等同于
	s3c_gpio_cfgpin ( GPC_7 , S3C_GPIO_INPUT );
	*/
	
	/*
		这时候DHT会发出80us的低电平信号，利用while等待信号结束。
		下面这个函数在此间拔出 DHT11的话，将会进入死循环，
		所以需要添加一个判断。
			while ( !( gpio_get_value ( GPC_7 ) ) )
				;
	*/

		while ( !( gpio_get_value ( GPC_7 ) ) && DataTemp )
		{
			DataTemp --;
			udelay ( 10 );
		}
		if ( !DataTemp )
		{
			err = 1;
			count = -EFAULT;
		}

/*
		同上，消掉80us的高电平信号。
		while ( ( gpio_get_value ( GPC_7 ) ) )
			;
*/
			
		if ( !err )
		{
			DataTemp = 10 ;
			while ( ( gpio_get_value ( GPC_7 ) ) && DataTemp )
			{
				DataTemp --;
				udelay ( 10 );
			}
			if ( !DataTemp )
			{
				err = 1;
				count = -EFAULT;
			}
		}


		if( !err )
		{

			/*利用数组接收数据
			分别是：
			8bit湿度整数数据
			8bit湿度小数数据
			8bi温度整数数据
			8bit温度小数数据
			8bit校验和
			*/
			for ( i = 0; i < 5; i ++ )
			{
				tempBuf[i] = dht11_drv_read_byte () ;
			}

			
			DataTemp = 0 ;
			
			/*校验数据是否准确*/
			for ( i = 0; i < 4; i ++ )
			{
				DataTemp += tempBuf[i] ;
			}
			if ( DataTemp != tempBuf[4] )
			{
				count = -EFAULT;
			}
			
			
			if ( count > 5 )
			{
				count = 5 ;
			}

			/*发送给用户空间*/
			if ( copy_to_user ( buf , tempBuf , count ) )
			{
				count = -EFAULT ;
			}

		}
		
	/*一次数据读取完成*/
	s3c_gpio_cfgpin ( GPC_7 , S3C_GPIO_OUTPUT );
	gpio_set_value ( GPC_7 , 1 );

	return count;
	
}

static struct  file_operations dht_drv_fops ={
        .owner = THIS_MODULE,
        .open = dht11_drv_open,
		.read = dht11_drv_read,
};

/*初始化设备*/
int __init dht11_drv_init(void)
{
	major = register_chrdev(0,"dht11",&dht_drv_fops);
	dhtdrv_class = class_create(THIS_MODULE,"dht_drv");
	dht11_drv_device = device_create(dhtdrv_class, NULL, MKDEV(major,0),NULL, "dht11");
	gpccon = (volatile unsigned long *)ioremap(0x7F008040,16);
	gpcdat = gpccon + 1;
	return 0;

}

/*卸载设备*/
void __exit dht11_drv_exit(void)
{
	unregister_chrdev(major,"dht11");
	device_unregister(dht11_drv_device);
	class_destroy(dhtdrv_class);
	iounmap(gpccon);

}

module_init(dht11_drv_init);
module_exit(dht11_drv_exit);
MODULE_LICENSE("GPL");
