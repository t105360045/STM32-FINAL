#include "lcd.h"

uint8_t specialNum = 1;
void LCD_Display(uint8_t screen[][8])
{
	uint8_t i, j, k = 0;
	for(i = 0;i < 8;i++)
	{
		for(j = 0;j < 16;j++)
		{
			k = 8;
			unsigned char coll = ((15 - j) * 8) & 0x0f;
			unsigned char colh = ((15 - j) * 8) >> 4;
		
			LCD_Command = Set_Start_Line_X|0x0;   delay();
			LCD_Command = Set_Page_Addr_X|i;  		delay();
			LCD_Command = Set_ColH_Addr_X|colh;   delay();
			LCD_Command = Set_ColL_Addr_X|coll;   delay();

			while (k--)
			{
				uint8_t temp = 0;
				if((k == 0)||(k == 1))
				{
					temp = 0xc3;
					if((screen[j][i] != screen[j - 1][i])||(j == 0))
					{
						temp += 0x3c;
					}
				}
				if((k == 2)||(k == 3)||(k == 4)||(k == 5))
<<<<<<< HEAD
				{
=======
			{
>>>>>>> 20171127_0051
					temp = 0x00;
					if((screen[j][i] != screen[j][i + 1])||(i == 7))
					{
						temp += 0xc0;
					}
					if((screen[j][i] != screen[j][i - 1])||(i == 0))
					{
						temp += 0x03;
					}
				}
				if((k == 6)||(k == 7))
				{
					temp = 0xc3;
					if((screen[j][i] != screen[j + 1][i])||(j == 15))
					{
						temp += 0x3c;
					}
				}
				if(screen[j][i] == 0)
				{
					temp = 0x00;
				}
				if(k == 7)
				{
					temp |= 0x01;
					if(i == 7)
					{
						temp |= 0x80;
					}
				}
				LCD_Data = temp;
				delay();
			}
		}
	}
}
 
void clean_old(uint8_t array[4][4],uint8_t screen[16][8],int8_t x,int8_t y)
{
	uint8_t i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if((array[i][j])&&((y+i)>=0)&&((x+j)>=0)&&((y+i)<=15)&&((x+j)<=7))
			{					
				screen[i+y][j+x]=0;
			}				
		}
	}
}

void set_new(uint8_t array[4][4],uint8_t screen[16][8],int8_t x,int8_t y)
{
	uint8_t i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if((array[i][j])&&((y+i)>=0)&&((x+j)>=0)&&((y+i)<=15)&&((x+j)<=7))
			{					
				screen[i+y][j+x]=specialNum;
			}				
		}
	}
	if(++specialNum == 0)
	{
		specialNum = 1;
	}
}

int8_t canPut(uint8_t array[4][4], uint8_t screen[16][8], int8_t x, int8_t y)
{
	int8_t i,j;
	for(i = x; i < (x + 4); i++)
	{
		for(j = y; j < (y + 4); j++)
		{
			if((i < 0)&&(array[j-y][i-x]!= 0))
				return 1;
			if((i > 7)&&(array[j-y][i-x]!= 0))
				return 2;
			if((j > 15)&&(array[j-y][i-x]!= 0))
				return 3;
			if((i >= 0)&&(i <= 7)&&(j >= 0)&&(j <= 15))
			{
				if((screen[j][i] != 0)&&(array[j-y][i-x] != 0))
				{
					return 4;
				}
			}
		}
	}
	return 0;
}