#include "lcd.h"
uint8_t oldPoint[16][8] = {0};
void LCD_Display(uint8_t point[][8])
{
	uint8_t i, j, k = 0;
	for(i = 0;i < 8;i++)
	{
		for(j = 0;j < 16;j++)
		{
			if(oldPoint[j][i] != point[j][i])
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
						if((point[j][i] != point[j + 1][i])||(j == 15))
						{
							temp += 0x3c;
						}
					}
					if((k == 2)||(k == 3)||(k == 4)||(k == 5))
					{
						temp = 0x00;
						if((point[j][i] != point[j][i - 1])||(i == 0))
						{
							temp += 0xc0;
						}
						if((point[j][i] != point[j][i + 1])||(i == 7))
						{
							temp += 0x03;
						}
					}
					if((k == 6)||(k == 7))
					{
						temp = 0xc3;
						if((point[j][i] != point[j - 1][i])||(j == 0))
						{
							temp += 0x3c;
						}
					}
					LCD_Data = (point[j][i] * temp);
					delay();
				}
				oldPoint[j][i] = point[j][i];
			}
		}
	}
}