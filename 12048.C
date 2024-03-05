


#include <reg51.h>
#define  uint  unsigned int
#define  uchar unsigned char
#include "chinese_cha.h"
#include "assii.h"

#define data_port P1

//sbit 	cs1 = P3^4;
//sbit  rs = P3^2;
//sbit	rw =P3^6;
//sbit	rd =P3^7;
//sbit	sclk =P3^1;
//sbit	sda =P3^0;
//sbit	res =P3^3;

//ZHANGRU 20160909
sbit 	cs1 = P3^0;
sbit	res =P3^1;
sbit    rs = P3^2;
sbit	rw =P3^3;
sbit	rd =P3^4;


sbit	sclk =P3^3;
sbit	sda =P3^4;
sbit	sclk1 =P1^0;
sbit	sda1 =P1^1;
sbit	sclk2 =P1^6;
sbit	sda2 =P1^7;



void lcd_init(void);
void lcd_rest(void);

void n_ms(uchar x);
void delay_n_s(uchar x);
void delay_n_100ms(uchar x);			//��ʱN-S�ӳ���

void key_scan();
void key_scan1(uint dtime);//���޸ĵȴ��İ���
uint key_scan2(uint dtime);//��ʱ������⹦�ܣ�ʹ�ö�P2�����趨ʵ�֣����鿪����P2λ���ķ���

void write_command(uchar command);
void write_data(uchar dis_data);
void set_page_address(uchar x);
void set_column_address(uchar x);

//��4��������ʵ�ʹ��ܣ��Ʋ�Ϊ���ڿ���ʹ��
void prt_one_char_sub(uchar start_column,start_page,uchar *i);
void disp_timer(uchar start_column,start_page,uchar second);
void prt_one_char(uchar start_column,start_page,uchar *char_pointer);
void read_data_tab(uchar *pp);

void lcd_disp_tab(void);//д�ı߿����ֶ��趨�߽�
void lcd_disp_full(uchar x);//д����ʵ�ʳ���������
void lcd_disp_test_icon_2();
void lcd_disp_test_icon(void);

void disp_single_char(uchar column,page,uchar *text_pointer);//д����
void disp_full_picture(uchar column,page,uchar *pic_pointer);//дȫ��ͼƬ
void disp_full_picture_9616(uchar column,page,uchar *pic_pointer,uchar cha_num,uchar cha_width);//д96*16��ͼƬ

void disp_single_square1(uchar column,page,uchar data_write_in);//д4*4�������ڿ����汾��
void disp_single_square2(uchar head_column,uchar head_line);//д4*4���񣬿��Զ�ʶ���ϰ�page���°�page
	//̰����д��4*4������disp_89()���
	void disp_single_area(uchar column,page,uchar *pic_pointer,uchar cha_num,uchar cha_width);

	//��disp_single_area�Ŀ���
	void switch_case0(uchar icolumn_t,uchar typ_t);
	void switch_case1(uchar icolumn_t,uchar typ_t);
	void switch_case2(uchar icolumn_t,uchar typ_t);
	void disp_89(uchar icolumn_temp2,uchar area,uchar typ);

void pin_detect_write(uchar icolumn_temp,uint *loc_d);//̽�����������޸Ĺ���
uint loc_process1(uchar iline_t);//Ŀ���޸������Ԥ����
void snake_program();//̰����������











//LCD��ʼ���ӳ���*********************************************************************
void lcd_init(void)
{
        write_command(0xe2);//����λ
        n_ms(5);
        write_command(0x2f);//��Դ��
        write_command(0xaf);//��ʾ��
        write_command(0x23);//�����Աȶȣ�0x21-0x27
        write_command(0x81);//˫�ֽ�ָ�΢���Աȶȣ�
        write_command(45);       //38=9.0v  30=8.5v  //�Աȶȵ��ڣ�0-63
        write_command(0xc8);	 //����,c0,c8
        write_command(0xa0);	 //����,a0,a1

//write_command(0x60);
}

// LCD��λ�ӳ���*********************************************************************
void lcd_rest(void)
{
	res=0;			//res =P3^1;
	n_ms(200);
	res=1;
	n_ms(200);
//	n_ms(200);
}


//��ʱN-MS�ӳ���********************************************************************
void n_ms(uchar x)			//��ʱN-MS�ӳ���
{
	uchar    i,d=1;
        for(i=0;i<x;++i)
        {
			for(d=0;d<50;d++);
        }
}
//��ʱN-S�ӳ���********************************************************************
void delay_n_s(uchar x)			//��ʱN-S�ӳ���
{
        uint i;
        uchar y;
        for(y=0;y<x;y++)
        {
        	for(i=0;i<33000;i++);
        }
}

//��ʱN-S�ӳ���********************************************************************
void delay_n_100ms(uchar x)			//��ʱN-S�ӳ��� (12mhz)
{
        uint i;
        uchar y;
        for(y=0;y<x;y++)
        {
        	for(i=0;i<7690;i++);
        }
}

//������������*************************************************************************************

void key_scan(void)						//��������
{
	uchar	i,h;
	P2=0Xff;
key_in:
	n_ms(10);
	i=P2;
	i=i&0x01;					//P3.5Ϊ��������;ok=0x01,down=0x02,up=0x04
	if(i>0)
	goto key_in;
key_hold:
	n_ms(10);
    i=P2;
	i=i&0x01;
	if(i<1)
	goto key_hold;
}


//��ʱ������������********************************************************************************
void key_scan1(uint dtime)						//dtime=5000>>>>time<=8s
{
	uchar i;
	uint j;
	P2=0Xff;
	j=0;
key_in:
	n_ms(10);
	i=P2;						//P2.0=ok,P2.1=down,P2.2=up
	i=i&0x01;					//P3.5Ϊ��������;ok=0x01,down=0x02,up=0x04
	if(i>0)
	{
		if(j<dtime)
		{
			j++;
			goto key_in;
		}
	}
key_hold:
	n_ms(10);
  	i=P2;
	i=i&0x01;
	if(i<1)
	{
		if(j<dtime)
		{
			j++;
			goto key_hold;
		}
	}

}

//��ʱ��ⰴ������********************************************************************************

//��д��2024�꺮�٣���ʱ��֪��IO�ڿ���λ���������������֮ǰ�İ���������ֽڲ����ˣ�����
uint key_scan2(uint dtime)
{
	uchar i;
	uint j,pin_address;
	j=0;
	pin_address=0;//ɨ������ʼ��
	P2=0Xff;//�˿ڳ�ʼ��

scan_start://�˿�ɨ�裬�����ⰴ��������ǰ�ظ���⣬ֱ����ʱ����
	n_ms(10);
	i=P2;
	P1=0x0c;//������P1����LED
	i=i&0x01;	
	if (i<1) 
		{
			pin_address=1;
			goto scan_check;
		}
	i=P2;
	i=i&0x02;	
	if (i<1) 
		{
			pin_address=2;
			goto scan_check;
		}
	i=P2;
	i=i&0x04;	
	if (i<1) 
		{
			pin_address=3;
			goto scan_check;
		}
	if (j<dtime) 
		{
			j++;
			goto scan_start;
		}
	goto scan_finish;
scan_check://��⵽�������º�ȷ�ϰ����ɿ���ֱ����ʱ����
	switch (pin_address)
	{
		case 1:
			n_ms(10);
  		i=P2;
			i=i&0x01;
			if(i<1&&j<dtime)
			{
				j++;
				goto scan_check;
			}
			goto scan_start;
			break;
		case 2:
			n_ms(10);
  		i=P2;
			i=i&0x02;
			if(i<1&&j<dtime)
			{
				j++;
				goto scan_check;
			}
			goto scan_start;
			break;
		case 3:
			n_ms(10);
  		i=P2;
			i=i&0x04;
			if(i<1&&j<dtime)
			{
				j++;
				goto scan_check;
			}
			goto scan_start;
			break;
		default:
			goto scan_start;//���ȷ����ɣ��ٴν����ⰴ������
	}
scan_finish://������⣬���ؼ����
	P1=0x00;
	return pin_address;//�޲���ʱΪ0����0��ʱΪ1����1��ʱΪ2����2��ʱΪ3
}




// д�����ӳ���*********************************************************************
void write_command(uchar command)  //д�����ӳ���
{
    uchar x;                //�����ݴ���
    rs=0;                   //rs = P3^2;     //RS=0
    cs1=0;          		//cs1 = P3^0;    //cs1=0
    for(x=0;x<8;x++)        	//ѭ��8��
    {
        sclk=0;				//sclk =P3^3;
        sclk1=0;			//sclk1 =P1^0;
       	sclk2=0;			//sclk2 =P1^6;
        if((command&0x80)==0x80)
        	{
           	sda=1;          //sda =P3^4;	//ȡSDA��������4�����λ
            sda1=1;			//sda1 =P1^1;
            sda2=1;			//sda2 =P1^7;
        	}
        else
        	{
            sda=0;
        	sda1=0;
            sda2=0;
            }
        command=command<<1;        //����һλ
        {
    	sclk=1;                    //SCKL=1
        sclk1=1;
        sclk2=1;
        }
    }
    cs1=1;                      //cs1=1
}


// д�����ӳ���*********************************************************************
void write_data(uchar dis_data)  //д�����ӳ���
{
    uchar x;                   //�����ݴ���
    rs=1;                      //RS=1
    cs1=0;                     //cs1=0
    for(x=0;x<8;x++)          		 //ѭ��8��
    {
        sclk=0;
        sclk1=0;
        sclk2=0;
        if ((dis_data&0x80)==0x80)                    //SCLK=0
        	{ 
			sda=1;
           	sda1=1;
           	sda2=1;
           	}
        else
        	{
			sda=0;
        	sda1=0;
            sda2=0;
            }
        dis_data=dis_data<<1;      //����һλ
        {
		sclk=1;                    //SCKL=1
        sclk1=1;
        sclk2=1;
        }
    }
    cs1=1;                      //cs1=1
}




/*
void write_command(uchar command)
{
	rs=0;
	cs1=0;
	rd=1;			//rd =P3^4;
	rw=0;			//rw =P3^3;
	data_port=command;
	rw=1;
}
void write_data(uchar disp_data)
{
	rs=1;
	cs1=0;
	rd=1;
	rw=0;
	data_port=disp_data;
	rw=1;
}
*/



// дҳ���ַ�ӳ���*********************************************************************
void set_page_address(uchar x)
{
    uchar  page_temp;
    page_temp=x|0xb0;
    write_command(page_temp);
}


// д�е�ַ�ӳ���*********************************************************************
void set_column_address(uchar x)
{
	uchar  column_temp;


	column_temp=x%16;		//ȡ��λ�е�ַ
	write_command(column_temp);

	column_temp=x/16;		//ȡ��λ�е�ַ
	column_temp=column_temp|0x10;
	write_command(column_temp);
}


void prt_one_char_sub(uchar start_column,start_page,uchar *i)
{
        uchar   x,l,k;
        x=start_column;
		set_page_address(start_page);
        set_column_address(start_column);

        for(k=0;k<5;k++)
        {
        l=*i;
        write_data(*i);
        *i++;
        }

}

void disp_timer(uchar start_column,start_page,uchar second)
{
	prt_one_char_sub(start_column,start_page,uchr_Number_Set+second*5);
}





void prt_one_char(uchar start_column,start_page,uchar *char_pointer)
{
	uchar		x,k;
      	x=*char_pointer;
        while(*char_pointer!='\0')

        {
        if(*char_pointer>=0x30&&*char_pointer<=0x39)	//����0-9
	prt_one_char_sub(start_column,start_page,&uchr_Number_Set+(*char_pointer-0x30)*5);

        if(*char_pointer>=65&&*char_pointer<=90)	//��д��ĸA-Z
	prt_one_char_sub(start_column,start_page,&uchr_Capitalization_Char_Set+(*char_pointer-65)*5);

        if(*char_pointer>=97&&*char_pointer<=122)	//Сд��ĸa-z
	prt_one_char_sub(start_column,start_page,&uchr_lowercase_Char_Set+(*char_pointer-97)*5);


        *char_pointer++;
		start_column+=6;
	}
}

void read_data_tab(uchar *pp)
{
        uchar i,x;
        for(x=0;x<10;x++)
        {
        i=*pp  ;
        *pp++;
        }
}





// д��ʾ�ı߿��ӳ���*********************************************************************
//�ó���ǰ�趨ֵΪ120*48
void lcd_disp_tab(void)
{
	uchar	i,page_address_temp=0;

	set_page_address(0);
	set_column_address(0);
	
	//д����
	for(i=0;i<132;i++)
	{
	write_data(0x01);

	
	//page_address_temp++;
	}
	set_page_address(5);
	set_column_address(0);
	
	//д�ױ�
	for(i=0;i<132;i++)
	{
	write_data(0x80);

	
	//page_address_temp++;
	}
	
	//д���
	set_page_address(0);
	
	for(page_address_temp=0;page_address_temp<7;page_address_temp++)
	{
		set_column_address(0);
		write_data(0xFF);
		set_page_address(page_address_temp);

	
	//page_address_temp++;
	}
	
	//д�ұ�
	set_page_address(0);
	
	for(page_address_temp=0;page_address_temp<7;page_address_temp++)
	{
		set_column_address(119);
		write_data(0xFF);
		set_page_address(page_address_temp);

	
	//page_address_temp++;
	}
}


// дȫ����ʾ�����ӳ���*********************************************************************

//�˲��ֹ���ʵ�ֵ�����ʾ����
		//a=x<<7;
		//b=x>>1;
		//x=a|b;

void lcd_disp_full(uchar x)
{
	uchar	a,b,i,t,page_address_temp=0;

	for(t=0;t<9;t++)
   {
	set_page_address(page_address_temp);
	set_column_address(0);
	for(i=0;i<64;i++)
	{
		a=x<<7;
		b=x>>1;
		x=a|b;
		write_data(x);
  	P0=x;
  }
  x= x>>3;
  for(i=0;i<64;i++)
	{
		a=x<<7;
		b=x>>1;
  	write_data(x);
  	P0=x;
  }
        
  x = x<<3;

            
        
        
  
  for(i=0;i<128;i++)
	{
	a=x<<7;
	b=x>>1;
	x=a|b;

	write_data(x);
        P0=x;
        x= x>>3;
        write_data(x);
        P0=x;
        x = x<<3;
        }
        page_address_temp++;
   }
}

// д����ͼ���ӳ���*********************************************************************
void lcd_disp_test_icon_2()
{
	uchar *tab_pointer ;

	uchar	temp,i;
  tab_pointer=&ICON_TAB;
	set_page_address(8);

	for(i=0;i<28;i++)
	{

	set_page_address(8);
	set_column_address(ICON_TAB[i]);
	write_data(0xff);
	set_column_address(ICON_TAB[i]);
	set_page_address(0);
	write_data(0xff);
	key_scan();

        }
}


// д����ͼ���ӳ���*********************************************************************
void lcd_disp_test_icon(void)
{
	uchar	i;

	set_page_address(0x08);
	set_column_address(0);
	for(i=0;i<132;i++)
	{
	write_data(0xff);
	key_scan();
    }
}




// д���������ӳ���*********************************************************************
void disp_single_char(uchar column,page,uchar *text_pointer)
{
        uchar x,y,z;
        for(y=0;y<2;y++)
     {
        set_column_address(column);
        set_page_address(page);
        for(x=0;x<16;x++)
        {
        //z=*text_pointer;
        write_data(*text_pointer);
        text_pointer++;
        }
        page++;

     }
}

// дȫ��ͼƬ�ӳ���*********************************************************************
void disp_full_picture(uchar column,page,uchar *pic_pointer)
{
        uchar x,y,z;
        for(y=0;y<2;y++)
     {
        set_column_address(column);
        set_page_address(page);
        for(x=0;x<56;x++)
        {
        //z=*text_pointer;
        write_data(*pic_pointer);
        pic_pointer++;
        }
        page++;

     }
}

// д��������ӳ���*********************************************************************
//ʹ�÷���Ϊ���ַ��������ֱ��ƴ�Ӷ������
//column=��ʼд���У�page=��ʼд��page��*pic_pointer=�������ƣ�cha_um=ƴ�ӵ��ַ�������cha_width=�����ַ�����

void disp_full_picture_9616(uchar column,page,uchar *pic_pointer,uchar cha_num,uchar cha_width)
{
        uchar x,y,z;
        uchar page_temp;
        page_temp=page;
        for(z=0;z<cha_num;z++)
        {
        	
		        for(y=0;y<2;y++)
		     {
		        set_column_address(column);
		        set_page_address(page);
		        for(x=0;x<cha_width;x++)
		        {
		        
		        write_data(*pic_pointer);
		        pic_pointer++;
		        }
		        page++;
		        
		
		     }
		     page=page_temp;
		     column=column+cha_width;
		    }
}




//д����4*4ɫ���ӳ���**************************************************************************
//disp_single_square()������ѧϰ�汾

void disp_single_square1(uchar column,page,uchar data_write_in)
{
	uchar x,y;
	set_column_address(column);
	set_page_address(page);
	   for(y=0;y<1;y++)
		     {
		        set_column_address(column);
		        set_page_address(page);
		        for(x=0;x<4;x++)
		        {
		        
		        	write_data(data_write_in);
		        
		        }

				}
}

//д����4*4ɫ���ӳ���**************************************************************************

//data_write_in = 0xf0,��������
//data_write_in = 0x0f,ż������
void disp_single_square2(uchar head_column,uchar head_line)
{
	uchar x,y;
	set_column_address(5*head_column-4);
	set_page_address(head_line/2-1);
	if(head_line%2 == 1)
		{
	   	for(y=0;y<1;y++)
		     {
		        set_column_address(5*head_column-4);
		        set_page_address(head_line/2-1);
		        for(x=0;x<4;x++)
		        {
		        
		        	write_data(0xf0);
		        
		        }

				}
		}
	else
		{
			
			for(y=0;y<1;y++)
		     {
		        set_column_address(5*head_column-4);
		        set_page_address(head_line/2-1);
		        for(x=0;x<4;x++)
		        {
		        	write_data(0x0f);
		        }
			}
		}
}

//д����̰����area�ӳ���,�޸���д��������ӳ���
void disp_single_area(uchar column,page,uchar *pic_pointer,uchar cha_num,uchar cha_width)
{
        uchar x,y,z;
        uchar page_temp;
        page_temp=page;
        for(z=0;z<cha_num;z++)
        {
        	
		        for(y=0;y<2;y++)
		     {
		        set_column_address(column);
		        set_page_address(page);
		        for(x=0;x<cha_width;x++)
		        {
		        
		        write_data(*pic_pointer);
		        pic_pointer++;
		        }
		        page++;
		        
		
		     }
		     page=page_temp;
		     column=column+cha_width;
		    }
}


//���ڽ��disp_89()��switch�޷�Ƕ�׵�����******************************************************

//case0��ʾareaΪ0
void switch_case0(uchar icolumn_t,uchar typ_t)
{
	switch(typ_t)
			{
				case 0:
					disp_single_area(5*icolumn_t-2,0,s10000,1,4);
					break;
				case 1: //001=1
					disp_single_area(5*icolumn_t-2,0,s12001,1,4);
					break;
					//(uchar column,page,uchar *pic_pointer,uchar cha_num,uchar cha_width)
				case 2://010=2
					disp_single_area(5*icolumn_t-2,0,s12010,1,4);
					break;
				case 3://011=3
					disp_single_area(5*icolumn_t-2,0,s12011,1,4);
					break;
				case 4://100=4
					disp_single_area(5*icolumn_t-2,0,s12100,1,4);
					break;
				case 5://101=5
					disp_single_area(5*icolumn_t-2,0,s12101,1,4);
					break;
				case 6://110=6
					disp_single_area(5*icolumn_t-2,0,s12110,1,4);
					break;
				case 7://111=7
					disp_single_area(5*icolumn_t-2,0,s12111,1,4);
					break;
			}
}

//case1��ʾareaΪ1
void switch_case1(uchar icolumn_t,uchar typ_t)
{
	switch(typ_t)
			{
				case 0:
					disp_single_area(5*icolumn_t-2,2,s00000,1,4);
					break;
				case 1: //001=1
					disp_single_area(5*icolumn_t-2,2,s34001,1,4);
					break;
				case 2://010=2
					disp_single_area(5*icolumn_t-2,2,s34010,1,4);
					break;
				case 3://011=3
					disp_single_area(5*icolumn_t-2,2,s34011,1,4);
					break;
				case 4://10=4
					disp_single_area(5*icolumn_t-2,2,s34100,1,4);
					break;
				case 5://101=5
					disp_single_area(5*icolumn_t-2,2,s34101,1,4);
					break;
				case 6://110=6
					disp_single_area(5*icolumn_t-2,2,s34110,1,4);
					break;
				case 7://111=7
					disp_single_area(5*icolumn_t-2,2,s34111,1,4);
					break;
			}
}

//case2��ʾarea2
void switch_case2(uchar icolumn_t,uchar typ_t)
{
	switch(typ_t)
			{
				case 0:
					disp_single_area(5*icolumn_t-2,4,s00001,1,4);
					break;
				case 1: //001=1
					disp_single_area(5*icolumn_t-2,4,s56001,1,4);
					break;
				case 2://010=2
					disp_single_area(5*icolumn_t-2,4,s56010,1,4);
					break;
				case 3://011=3
					disp_single_area(5*icolumn_t-2,4,s56011,1,4);
					break;
				case 4://100=4
					disp_single_area(5*icolumn_t-2,4,s56100,1,4);
					break;
				case 5://101=5
					disp_single_area(5*icolumn_t-2,4,s56101,1,4);
					break;
				case 6://110=6
					disp_single_area(5*icolumn_t-2,4,s56110,1,4);
					break;
				case 7://111=7
					disp_single_area(5*icolumn_t-2,4,s56111,1,4);
					break;
			}
}





//8page��9��ʽ������߿���ʾ����***************************************************************
//�����޷�Ƕ��switch�����ú��������ڶ���switch

void disp_89(uchar icolumn_temp2,uchar area,uchar typ)
{
	switch(area)
	{
		case 0:
		{
			switch_case0(icolumn_temp2,typ);//���ڲ���ԭ���޷�Ƕ��switch���ʸ����ӳ���
			break;
			//switch_case0()��Դ
			/*
			switch(typ)
			{
				case 1: //001=1
					disp_single_area(5*icolumn_temp2-2,area,s12001,1,4);
					break;
					//(uchar column,page,uchar *pic_pointer,uchar cha_num,uchar cha_width)
				case 2://010=2
					disp_single_area(5*icolumn_temp2-2,area,s12010,1,4);
					break;
				case 3://011=3
					disp_single_area(5*icolumn_temp2-2,area,s12011,1,4);
					break;
				case 4://100=4
					disp_single_area(5*icolumn_temp2-2,area,s12100,1,4);
					break;
				case 5://101=5
					disp_single_area(5*icolumn_temp2-2,area,s12101,1,4);
					break;
				case 6://110=6
					disp_single_area(5*icolumn_temp2-2,area,s12110,1,4);
					break;
				case 7://111=7
					disp_single_area(5*icolumn_temp2-2,area,s12111,1,4);
					break;
			}
			break;
			*/
		}
		case 1:
		{
			switch_case1(icolumn_temp2,typ);//���ڲ���ԭ���޷�Ƕ��switch���ʸ����ӳ���
			break;
			//switch_case1()��Դ
			/*
			switch(typ)
			{
				case 1: //001=1
					disp_single_area(5*icolumn_temp2-2,area,s34001,1,4);
					break;
				case 2://010=2
					disp_single_area(5*icolumn_temp2-2,area,s34010,1,4);
					break;
				case 3://011=3
					disp_single_area(5*icolumn_temp2-2,area,s34011,1,4);
					break;
				case 4://100=4
					disp_single_area(5*icolumn_temp2-2,area,s34100,1,4);
					break;
				case 5://101=5
					disp_single_area(5*icolumn_temp2-2,area,s34101,1,4);
					break;
				case 6://110=6
					disp_single_area(5*icolumn_temp2-2,area,s34110,1,4);
					break;
				case 7://111=7
					disp_single_area(5*icolumn_temp2-2,area,s34111,1,4);
					break;
				default:
					key_scan();
			}
			break;
			*/
		}
		case 2:
		{
			switch_case2(icolumn_temp2,typ);//���ڲ���ԭ���޷�Ƕ��switch���ʸ����ӳ���
			break;
			//switch_case2()��Դ
			/*
			switch(typ)
			{
				case 1: //001=1
					disp_single_area(5*icolumn_temp2-2,area,s56001,1,4);
					break;
				case 2://010=2
					disp_single_area(5*icolumn_temp2-2,area,s56010,1,4);
					break;
				case 3://011=3
					disp_single_area(5*icolumn_temp2-2,area,s56011,1,4);
					break;
				case 4://100=4
					disp_single_area(5*icolumn_temp2-2,area,s56100,1,4);
					break;
				case 5://101=5
					disp_single_area(5*icolumn_temp2-2,area,s56101,1,4);
					break;
				case 6://110=6
					disp_single_area(5*icolumn_temp2-2,area,s56110,1,4);
					break;
				case 7://111=7
					disp_single_area(5*icolumn_temp2-2,area,s56111,1,4);
					break;
			}
			break;
			*/
		}
	}
}


//̽���Ⲣ������ʾд�루������*******************************************************************
//�ó���̽��Ϊ3λ111��̽�룬���ڶ��������ÿ��area��״̬�ļ��

void pin_detect_write(uchar icolumn_temp,uint *loc_d)//icolumnΪ������λ�ã�loc_dΪ����������
{	
	uint loc_point=0xE000;//��ʼ��̽��
	uint loc_point_low=*loc_d;//����������
	uchar i;
	for(i=0;i<3;i++)
	{
		if((loc_point&loc_point_low)>0)
		{
			loc_point_low=loc_point_low<<3*i;
			loc_point_low=loc_point_low>>13;//�ƶ�����������λ
			//loc_point=loc_point>>3;
			disp_89(icolumn_temp,i,loc_point_low);
			loc_point_low=*loc_d;
		}
		else
		{
			disp_89(icolumn_temp,i,0);
		}
		loc_point=loc_point>>3;
	}
}


//�����д��Ԥ����
//���ڵ�������д��ʱ���滻λ����
uint loc_process1(uchar iline_t)
{
	uint iloc=0x8000;
	uchar i;
	for(i=0;i<iline_t-1;i++)
	{
		iloc=iloc>>1;
	}
	return iloc;
}






//̰����������****************************

//ע����ʾӦ�������� ��4�� ��ʼ���Ա�֤��ʾ���ҶԳ�

void snake_program()
{
	
	uchar length=3;//��ʼ������,1�ֽ�
	
	uchar direction=1;//����,1Ϊ�ҡ�2Ϊ�ϡ�3Ϊ��4Ϊ�£�1�ֽ�

	uchar xdata act_rec[20];//��Ϊ�⣬20�ֽڣ��ɳ���ʹ��xdata
	uchar rec_temp;//��Ϊ���棬1�ֽ�
		
	uint xdata loc1[23];//1������⣬46�ֽڣ��ɳ���ʹ��xdata
	uint loc1_temp;//���껺�棬2�ֽ�
	uint loc_point=0xE000;//����̽��1110000000000000��2�ֽ�
	uint loc_point_low;//��λ̽������2�ֽ�

	uchar iline=4;//iline=1~9
	uchar icolumn=11;//icolumn=0~22,��ʼͷ���꣬2�ֽ�
	uchar iline_temp,icolumn_temp;//���껺�棬2�ֽ�

	uchar i;//ѭ���ݴ�����1�ֽڣ�Ŀǰ��ʹ��80�ֽ�
	
	for(i=0;i<23;i++)//��ʼ�������
	{
		loc1[i]=0x0000;
	}

	for(i=0;i<20;i++)//��ʼ����Ϊ��
	{
		act_rec[i]=0;
	}

	for(i=0;i<length;i++)//��ʼ�ж���¼����
	{
		act_rec[i]=direction;
	}
	for(i=0;i<3;i++)//��ʼ�������
	{
		loc1[icolumn-i]=0x1000;
	}
	for(i=0;i<3;i++)//��ʼ��ʾ
	{
		disp_89(icolumn-i,1,0x0004);//disp_89(uchar icolumn_temp2,uchar area,uint typ)
	}


	while(1)
	{

	//��ȡ�������
	switch(key_scan2(1000)-1)//uint key_scan2(uint dtime),����ֵΪ1~3,1Ϊok��
	{
		case 1:
			if(direction>=4)
			{
				direction=0;
			}
			direction++;
			break;
		case 2:
			if(direction<=1)
			{
				direction=5;
			}
			direction--;
			break;
		default:
			break;

	}

	//�����ж�ѹ����Ϊ��
	for(i=0;i<length;i++)
	{
		act_rec[i]=act_rec[i+1];
	}
	act_rec[length-1]=direction;

	//��д�µ�ͷ����
	switch(direction)
	{
		case 1://����
			icolumn++;
			break;
		case 2://����
			iline--;
			break;
		case 3://����
			icolumn--;
			break;
		case 4://����
			iline++;
			break;
	}


	
	
	
	//д��������
	loc1_temp = loc1[icolumn];
	loc1[icolumn]=loc1_temp | loc_process1(iline);
	
	pin_detect_write(icolumn,&loc1[icolumn]);



	//̽��������ʾд��,void pin_detect_write(uint *loc_d)��Դ
	
	/*
	loc_point=0xE000;
	for(i=0;i<3;i++)
	{
		if(loc_point&loc1[icolumn]>0)
		{
			loc_point_low=loc_point;
				loc_point_low=loc_point_low>>13-3*i;
			disp_89(i,loc_point_low);
		}
		loc_point=loc_point>>3;
	}
	*/


	//̽��������ʾ������д�룩
	//1������β����

	iline_temp=iline;
	icolumn_temp=icolumn;
	for(i=0;i<20 && act_rec[i]!=0;i++)
	{
		switch(act_rec[i])
		{
			case 1:
				icolumn_temp--;
				break;
			case 2:
				iline_temp++;
				break;
			case 3:
				icolumn_temp++;
				break;
			case 4:
				iline_temp--;
				break;
			default:
				break;
		}
	}

	//2����д������

	loc1_temp = ~loc1[icolumn_temp];
	loc1_temp = loc1_temp | loc_process1(iline_temp);
	loc1[icolumn_temp]=~loc1_temp;
	


	//3����ʼ������д�룩

	pin_detect_write(icolumn_temp,&loc1[icolumn_temp]);
	//key_scan();

	}



	//loc1_temp = loc1[icolumn_process(icolumn)];
	//loc1[icolumn_process(icolumn)]=loc_process2(iline_process(iline));//����������

}






// ������****************************************************************************
//key_scan1()������ʱ���
//key_scan2()������ʱ���
void main(void)
{
	uint g;
	uint i_temp,g_temp;
	while(1)
	{
		lcd_rest();
   		lcd_init();

		lcd_disp_full(0x00);
		delay_n_100ms(20);
		lcd_disp_tab();//д�ı߿�



		key_scan1(500);


//lcd_disp_full(0xaa);
//delay_n_100ms(20);
////key_scan();
//lcd_disp_full(0x00);
//delay_n_100ms(20);
//	//key_scan();
////lcd_disp_test_icon_2();
////	key_scan();
//lcd_disp_full(0xFF);
//delay_n_100ms(20);
// // key_scan();
////lcd_disp_full(0x00);
//

//delay_n_100ms(10);
//		key_scan();


//��ʾ������ֹ��ܲ���

//lcd_disp_full(0x00);
//disp_single_char(1,1,CHA_CU_1);
//disp_single_char(17,1,CHA_CU_2);
//disp_single_char(33,1,CHA_CU_3);
//key_scan();
//disp_89(11,1,0004);
//key_scan();
//lcd_disp_full(0x00);
//disp_full_picture_9616(1,1,CHA_MU_3,9,8);
//key_scan();

/*
g_temp=key_scan2(2500);
switch (g_temp)
{
	case 1:
		for(g=8;g<=16;g++)
		{
			//lcd_disp_full(0x00);
			disp_single_square2(g,2);
			key_scan1(800);
		}
		break;
	case 2:
		for(g=8;g<=16;g++)
		{
			//lcd_disp_full(0x00);
			disp_single_square2(g,3);
			key_scan1(800);
		}
		break;
	case 3:
		for(g=8;g<=16;g++)
		{
			//lcd_disp_full(0x00);
			disp_single_square2(g,4);
			key_scan1(800);
		}
		break;
	default:
		for(g=8;g<=16;g++)
		{
			//lcd_disp_full(0x00);
			disp_single_square2(g,5);
			key_scan1(800);
		}
		
}

*/



/*
if (g_temp==2) 
{
	for(g=8;g<=16;g++)
		{
			lcd_disp_full(0x00);
			disp_single_square2(g,2);
			key_scan1(500);
		}


}
*/
//lcd_disp_full(0x00);
//key_scan();



disp_full_picture_9616(1,1,CHA_MU_3,9,8);

key_scan1(1000);
lcd_disp_full(0x00);
lcd_disp_tab();
snake_program();

		//snake_program();



//lcd_disp_full(0xaa);
//key_scan();
//
//    	  write_command(0x81);
//        write_command(33);
//        key_scan();
//
//
//       write_command(0x81);
//        write_command(48);
//        key_scan();


//    	   	disp_full_picture(4,0,PIC_12864_meinu);
//    	   	delay_n_s(1);
//
//    	   	disp_full_picture(4,0,PIC_12864_TV);
//    	   	delay_n_s(1);
//
//    	   	disp_full_picture(4,0,PIC_12864_TIMER);
//    	   	delay_n_s(1);
//
//    	   	disp_full_picture(4,0,PIC_12864_MIQI2);
//    	   	delay_n_s(1);
//
//    	   	disp_full_picture(4,0,PIC_12864_CHINA_MAP);
//   	      delay_n_s(1);






	}
}