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
	/*����GPC7Ϊ�������*/
	*gpccon &= ~( 0xf << (4*7));
	/*GPC7����ߵ�ƽ*/
	*gpcdat |= (1<<7);

	return 0;
}


static char dht11_drv_read_byte ( void )
{
	char dht11_byte ;
	unsigned char i ;
	unsigned char temp ;
	
	dht11_byte = 0 ;
	/*�������ݣ�������|��&�洢��dht11_byte*/
	for ( i = 0 ; i < 8 ; i ++ )
	{
		temp = 0 ;
		/*50us������ʱ��*/
		while ( ! (gpio_get_value ( GPC_7 ) ) )
		{
			temp ++ ;
			if ( temp > 12 )
				return 1 ;
			udelay ( 5 ) ;
		}
		temp = 0 ;
		
		/*���ݽ��յ��ߵ�ƽ������ʱ���ж�������0����1*/
		while ( gpio_get_value ( GPC_7 ) )
		{
			temp ++ ;
			if ( temp > 20 )
				return 1 ;
			udelay ( 5 ) ;
		}
		/*����6�Ļ���Ҳ���Ǵ���35us������Ϊ1����������Ϊ0*/
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

	/*����GPC7Ϊ�������*/
	*gpccon &= ~( 0xf << ( 4*7) );
	*gpccon |= ( 0x1 << (4*7));	
	
	/*GPC7����͵�ƽ*/
	*gpcdat &= ~( 1<<7 );
	
	/*
	��ͬ��
	s3c_gpio_cfgpin ( GPC_7 , S3C_GPIO_OUTPUT );
	gpio_set_value ( GPC_7 , 0 );
	*/
	
	/*��ʱ18���룬����DHT11�Ӵ���״̬���빤��״̬*/
	msleep ( 18 );//msleep()������ʱ������������ȷ��ʵ����ʱʱ����������ֵ
	
	/*GPC7����ߵ�ƽ*/
	*gpcdat |= ( 1<<7 );
	//gpio_set_value ( GPC_7 , 1 );
	udelay ( 40 );	//udelay()æ�ȴ�����,�ӳٹ������޷�����������������ӳٵ�ʱ����׼ȷ,�����˷�CPU���������㳤ʱ����ʱ��
	
	/*����GPC7Ϊ��������*/
	*gpccon &= ~( 0xf << ( 4*7 ));
	/*
	��ͬ��
	s3c_gpio_cfgpin ( GPC_7 , S3C_GPIO_INPUT );
	*/
	
	/*
		��ʱ��DHT�ᷢ��80us�ĵ͵�ƽ�źţ�����while�ȴ��źŽ�����
		������������ڴ˼�γ� DHT11�Ļ������������ѭ����
		������Ҫ���һ���жϡ�
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
		ͬ�ϣ�����80us�ĸߵ�ƽ�źš�
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

			/*���������������
			�ֱ��ǣ�
			8bitʪ����������
			8bitʪ��С������
			8bi�¶���������
			8bit�¶�С������
			8bitУ���
			*/
			for ( i = 0; i < 5; i ++ )
			{
				tempBuf[i] = dht11_drv_read_byte () ;
			}

			
			DataTemp = 0 ;
			
			/*У�������Ƿ�׼ȷ*/
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

			/*���͸��û��ռ�*/
			if ( copy_to_user ( buf , tempBuf , count ) )
			{
				count = -EFAULT ;
			}

		}
		
	/*һ�����ݶ�ȡ���*/
	s3c_gpio_cfgpin ( GPC_7 , S3C_GPIO_OUTPUT );
	gpio_set_value ( GPC_7 , 1 );

	return count;
	
}

static struct  file_operations dht_drv_fops ={
        .owner = THIS_MODULE,
        .open = dht11_drv_open,
		.read = dht11_drv_read,
};

/*��ʼ���豸*/
int __init dht11_drv_init(void)
{
	major = register_chrdev(0,"dht11",&dht_drv_fops);
	dhtdrv_class = class_create(THIS_MODULE,"dht_drv");
	dht11_drv_device = device_create(dhtdrv_class, NULL, MKDEV(major,0),NULL, "dht11");
	gpccon = (volatile unsigned long *)ioremap(0x7F008040,16);
	gpcdat = gpccon + 1;
	return 0;

}

/*ж���豸*/
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
