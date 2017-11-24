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
					if((k == 2)||(k == 3)||(k == 4)||(k == 5))
					{
						LCD_Data = (point[j][i] * 0xc3);
					}
					else
					{
						LCD_Data = (point[j][i] * 0xff);
					}
					delay();
				}
				oldPoint[j][i] = point[j][i];
			}
		}
	}
}