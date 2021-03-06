#include "stm32f1xx_hal.h"
#include "main.h"
#include <stdlib.h>
#include "lcd.h"
#include "display.h"
#include "Brick.h"

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */ 

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim8;
UART_HandleTypeDef huart1;
ADC_HandleTypeDef hadc1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_BUTTON_Init(void);
static void MX_LED_Init(void);
static void MX_JOY_Init(void);
static void MX_TIM1_Init(uint32_t Frequence);
static void MX_TIM8_Init(uint32_t Frequence);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
void Draw(uint8_t *string);
void ASCIItoLCD(uint8_t *string);
void printffff(uint8_t *string);
void LCD_FullDrew_8_16(uint8_t *string);
void LCD_FullDrew_8_8(uint8_t *string);
void new_brick();

uint8_t UARTBuffer[100];
uint8_t UARTCount = 0;

uint8_t tamperButton, keyButton, wakeupButton, upButton, downButton, leftButton, rightButton, selButton;
uint8_t R_button,L_button,U_button,SAVE_button,PUT_button;
int8_t type,angle,x,y,save_type=0;
uint8_t delay_flag = 0;
int8_t temp_type=0;
uint8_t save_f=0;
uint8_t score = 0;

uint8_t screen[16][8] = 
{
	0
};
//===========================================================================//
//            						     main                       			     			   //
//===========================================================================//
int main(void)
{
	x=2;
	y=-1;
	save_type=-1;
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
	MX_BUTTON_Init();
  MX_LED_Init();
	MX_JOY_Init();
  MX_TIM1_Init(2);
  MX_TIM8_Init(20);
  MX_ADC1_Init();
  MX_USART1_UART_Init();
	LCD_Init();
	LCD_Clear();
	
	HAL_ADC_Start(&hadc1);
	uint16_t adc = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	
	srand(adc);
	
  HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim8);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	
  while (1)
  {
		if(delay_flag)
		{
			HAL_Delay(500);
			delay_flag = 0;
		}
		LCD_Display(&screen[0]);
  }
}

//==========================================================================//
//            						     TIM1_UP_IRQHandler                      		 	//
//==========================================================================//
void TIM1_UP_IRQHandler(void)
{
	clean_old(&brick[type][angle][0],&screen[0],x,y);
	
	if(canPut(&brick[type][angle][0],&screen[0],x,y+1)==0)
	{
		y=y+1;
	}
	else if(canPut(&brick[type][angle][0],&screen[0],x,y+1)==3)
	{
		new_brick();	
	}
	else if(canPut(&brick[type][angle][0],&screen[0],x,y+1)==4)
	{
		new_brick();
	}
	
	set_new(&brick[type][angle][0],&screen[0],x,y);	
	
	HAL_GPIO_TogglePin(LED1_GPIO_PORT,LED1_PIN);
	
  HAL_TIM_IRQHandler(&htim1);
}

//==========================================================================//
//            						     TIM8_UP_IRQHandler                     	   	//
//==========================================================================//
void TIM8_UP_IRQHandler(void)
{
	HAL_TIM_Base_Stop_IT(&htim1);
//=======================================================================R
	if((R_button == 1)&&(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 0))
	{
		clean_old(&brick[type][angle][0],&screen[0],x,y);
		if(canPut(&brick[type][angle][0],&screen[0],x+1,y)==0)
		{
			x++;
		}
	}
//=======================================================================L
	if((L_button == 1)&&(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == 0))
	{	
		clean_old(&brick[type][angle][0],&screen[0],x,y);
		if(canPut(&brick[type][angle][0],&screen[0],x - 1,y)==0)
		{
			x--;
		}
	}			
//=======================================================================PUT
	if((PUT_button == 1)&&(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == 0))
	{
		clean_old(&brick[type][angle][0],&screen[0],x,y);
		while(canPut(&brick[type][angle][0],&screen[0],x,y+1)==0)	
		{
			y++;
		}
		new_brick();
	}
//=======================================================================SAVE
	if((SAVE_button == 1)&&(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == 0))
	{
		if(save_f==0)
		{
			clean_old(&brick[type][angle][0],&screen[0],x,y);
		
			if(save_type==-1)		
			{
				save_type =type ;
				x=2;y=-1;angle=0;
				type=rand()%7;
			}
			else
			{
				temp_type=type;
				x=2;y=-1;angle=0;
				type=save_type;
				save_type=temp_type;
			}
		}
		save_f=1;
	}
//=======================================================================U
	if((U_button == 1)&&(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 0))
	{
		clean_old(&brick[type][angle][0],&screen[0],x,y);
		uint8_t tempAngle = (angle + 1)%4;
		if(canPut(&brick[type][tempAngle][0],&screen[0],x,y)==0)
		{
			angle = (angle + 1)%4;
		}	
		else if(canPut(&brick[type][tempAngle][0],&screen[0],x,y)==1)
		{
			if(canPut(&brick[type][tempAngle][0],&screen[0],x+1,y)==0)
			{
				x++;
				angle = (angle + 1)%4;
			}	
			else if(canPut(&brick[type][tempAngle][0],&screen[0],x+1,y)==1)
			{
				if(canPut(&brick[type][tempAngle][0],&screen[0],x+2,y)==0)
				{
					x=x+2;
					angle = (angle + 1)%4;
				}	
				else if(canPut(&brick[type][tempAngle][0],&screen[0],x+2,y)==1)
				{
					if(canPut(&brick[type][tempAngle][0],&screen[0],x+3,y)==0)
					{
						x=x+3;
						angle = (angle + 1)%4;
					}	
				}
			}
		}	
		else if(canPut(&brick[type][tempAngle][0],&screen[0],x,y)==2)
		{
			if(canPut(&brick[type][tempAngle][0],&screen[0],x-1,y)==0)
			{
				x--;
				angle = (angle + 1)%4;
			}	
			else if(canPut(&brick[type][tempAngle][0],&screen[0],x-1,y)==2)
			{
				if(canPut(&brick[type][tempAngle][0],&screen[0],x-2,y)==0)
				{
					x=x-2;
					angle = (angle + 1)%4;
				}	
				else if(canPut(&brick[type][tempAngle][0],&screen[0],x-2,y)==2)
				{
					if(canPut(&brick[type][tempAngle][0],&screen[0],x-3,y)==0)
					{
						x=x-3;
						angle = (angle + 1)%4;
					}	
				}
			}
		}	
	}
//=======================================================================Tamper
	if((tamperButton == 1)&&(HAL_GPIO_ReadPin(TAMPER_BUTTON_GPIO_PORT, TAMPER_BUTTON_PIN) == 0))
	{
		
	}
//=======================================================================Wakeup
	if((wakeupButton == 1)&&(HAL_GPIO_ReadPin(WAKEUP_BUTTON_GPIO_PORT, WAKEUP_BUTTON_PIN) == 0))
	{
		
	}
	tamperButton = HAL_GPIO_ReadPin(TAMPER_BUTTON_GPIO_PORT, TAMPER_BUTTON_PIN);
	wakeupButton = HAL_GPIO_ReadPin(WAKEUP_BUTTON_GPIO_PORT, WAKEUP_BUTTON_PIN);
	R_button=HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);
	U_button=HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8);
	L_button=HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12);
	PUT_button=HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2);
	SAVE_button=HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11); 	
	keyButton = HAL_GPIO_ReadPin(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN);
	
	set_new(&brick[type][angle][0],&screen[0],x,y);
		 
  HAL_TIM_IRQHandler(&htim8);
	HAL_TIM_Base_Start_IT(&htim1);
}
//==========================================================================//
//            						     USART1_IRQHandler                      	  	//
//==========================================================================//
void USART1_IRQHandler(void)
{
	uint8_t temp;
	HAL_UART_Receive(&huart1, (uint8_t *)&temp, 1, 0xffff);
	if(temp != '\n')
	{
		UARTBuffer[UARTCount++] = temp;
		if(UARTCount >= 100)
		{
			UARTCount = 99;
		}
	}
	else
	{
		UARTBuffer[UARTCount] = 0x00;
		
		printffff(UARTBuffer);
		LCD_Clear();
		Draw(UARTBuffer);
		
		UARTCount = 0;
		UARTBuffer[UARTCount] = 0x00;
	}
	HAL_UART_IRQHandler(&huart1);
}

//==========================================================================//
//            						     new_brick                              	  	//
//==========================================================================//
void new_brick()
{
	save_f=0;
	set_new(&brick[type][angle][0],&screen[0],x,y);	
	x=2;y=-1;angle=0;
	while(1)
	{
		uint8_t temp;
		temp=rand()%7;
		if(type!=temp)
		{
			type=temp;
			break;
		}
	}
	
	for(int i = 0; i < 16; i ++)
	{	
		for(int j = 0; j < 8; j ++)
		{
			if(screen[i][j]==0)
			{
				break;
			}
			if((screen[i][j]!=0)&&(j==7))
			{
				LCD_Display(&screen[0]);
				for(int ii = 15; ii > 0; ii --)
				{
					for(int jj = 0; jj < 8; jj ++)
					{
						if(ii<=i)
							screen[ii][jj]=screen[ii-1][jj];
					}
				}
				for(int jj = 0; jj < 8; jj ++)
				{
					screen[0][jj]=0;
				}
				delay_flag = 1;
				score += 10;
			}
		}
	}
	if((screen[1][3]!=0)||(screen[1][4]!=0)||(screen[1][2]!=0)||(screen[1][5]!=0))
		for(int ii = 0; ii <16; ii ++)
			for(int jj = 0; jj < 8; jj ++)
				screen[ii][jj]=0;
}

//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//            						     SystemClock_Config                     	   	//
//==========================================================================//
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

//==========================================================================//
//            						     MX_GPIO_Init                             		//
//==========================================================================//
void MX_GPIO_Init(void)
{
  __GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_9;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP ;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10,0);
	 
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

//==========================================================================//
//            						     MX_LED_Init                           		   	//
//==========================================================================//
void MX_LED_Init(void)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
	
  LED1_GPIO_CLK_ENABLE();
  LED2_GPIO_CLK_ENABLE();
  LED3_GPIO_CLK_ENABLE();
  LED4_GPIO_CLK_ENABLE();
	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStruct.Pin = LED1_PIN;
  HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED2_PIN;
  HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED3_PIN;
  HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED4_PIN;
  HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(LED4_GPIO_PORT, LED1_PIN|LED2_PIN|LED3_PIN|LED4_PIN,0);
}

//==========================================================================//
//            						     MX_BUTTON_Init                         	  	//
//==========================================================================//
void MX_BUTTON_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
	
	TAMPER_BUTTON_GPIO_CLK_ENABLE();
	KEY_BUTTON_GPIO_CLK_ENABLE();
	WAKEUP_BUTTON_GPIO_CLK_ENABLE();
	
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
  GPIO_InitStruct.Pin = TAMPER_BUTTON_PIN;
  HAL_GPIO_Init(TAMPER_BUTTON_GPIO_PORT, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = KEY_BUTTON_PIN;
  HAL_GPIO_Init(KEY_BUTTON_GPIO_PORT, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = WAKEUP_BUTTON_PIN;
  HAL_GPIO_Init(WAKEUP_BUTTON_GPIO_PORT, &GPIO_InitStruct);
}

//==========================================================================//
//            						     MX_JOY_Init                             		 	//
//==========================================================================//
void MX_JOY_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
	
	UP_JOY_GPIO_CLK_ENABLE();
	DOWN_JOY_GPIO_CLK_ENABLE();
	LEFT_JOY_GPIO_CLK_ENABLE();
	RIGHT_JOY_GPIO_CLK_ENABLE();
	SEL_JOY_GPIO_CLK_ENABLE();
	
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStruct.Pin = UP_JOY_PIN;
  HAL_GPIO_Init(UP_JOY_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DOWN_JOY_PIN;
  HAL_GPIO_Init(DOWN_JOY_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LEFT_JOY_PIN;
  HAL_GPIO_Init(LEFT_JOY_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RIGHT_JOY_PIN;
  HAL_GPIO_Init(RIGHT_JOY_GPIO_PORT, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = SEL_JOY_PIN;
  HAL_GPIO_Init(SEL_JOY_GPIO_PORT, &GPIO_InitStruct);
}

//==========================================================================//
//            						     MX_TIM1_Init                            	  	//
//==========================================================================//
void MX_TIM1_Init(uint32_t Frequence)
{
	uint16_t prescaler;
	uint16_t period;
	
	Frequence = 72000000 / Frequence;
	
	if (Frequence > 720000)
	{
		prescaler = 7200;
		period = Frequence / prescaler;
	}
	else
	{
		if (Frequence > 7200)
		{
			prescaler = 72;
			period = Frequence / prescaler;
		}
		else
		{
			prescaler = 1;
			period = Frequence;
		}
	}
	
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = prescaler;
  htim1.Init.Period = period;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  HAL_TIM_SlaveConfigSynchronization(&htim1, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

}

//==========================================================================//
//            						     MX_TIM8_Init                              		//
//==========================================================================//
void MX_TIM8_Init(uint32_t Frequence)
{	
	uint16_t prescaler;
	uint16_t period;
	
	Frequence = 72000000 / Frequence;
	
	if (Frequence > 720000)
	{
		prescaler = 7200;
		period = Frequence / prescaler;
	}
	else
	{
		if (Frequence > 7200)
		{
			prescaler = 72;
			period = Frequence / prescaler;
		}
		else
		{
			prescaler = 1;
			period = Frequence;
		}
	}
	
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = prescaler;
  htim8.Init.Period = period;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim8);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR1;
  HAL_TIM_SlaveConfigSynchronization(&htim8, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig);

}

//==========================================================================//
//            						     MX_ADC1_Init                             	 	//
//==========================================================================//
void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;

  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadc1);

  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

//==========================================================================//
//            						     MX_USART1_UART_Init                      	 	//
//==========================================================================//
void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	
  HAL_UART_Init(&huart1);
}

//==========================================================================//
//            						     printffff                              	  	//
//==========================================================================//
void printffff(uint8_t *string)
{
	while(*string)
	{
		printf("%c", *string++);
	}
	printf("\r\n");
}
	
//==========================================================================//
//            						     Draw                                  		   	//
//==========================================================================//
void Draw(uint8_t *string)
{
	uint8_t i, j = 0;
	while(*string)
	{
		ASCIItoLCD(string);
		LCD_DrawString(i, j, string, 1);
		j += 8;
		if(j >= 128)
		{
			i += 2;
			j = 0;
		}
		string++;
	}
}

//==========================================================================//
//            						     ASCIItoLCD                             	   	//
//==========================================================================//
void ASCIItoLCD(uint8_t *string)
{
	if((48 <= *string) && (*string <= 57))
	{
		*string -= 48;
	}
	else if((65 <= *string) && (*string <= 90))
	{
		*string -= 9;
	}
	else if((97 <= *string) && (*string <= 122))
	{
		*string -= 67;
	}
	else if(*string == 32)
	{
		*string = 22;
	}
	else if(*string == 45)
	{
		*string = 20;
	}
	else if(*string == 46)
	{
		*string = 10;
	}
	else if(*string == 58)
	{
		*string = 21;
	}
}

//==========================================================================//
//            						     LCD_FullDrew                            	  	//
//==========================================================================//
void LCD_FullDrew_8_16(uint8_t *string)
{
	uint8_t i, j, k = 0;
	for(i = 0;i<= 6;i+= 2)
	{
		for(j = 0;j < 128;j+= 8)
		{
			k = 8;
			unsigned char coll = j & 0x0f;
			unsigned char colh = j >> 4;
  
			LCD_Command = Set_Start_Line_X|0x0;   delay();
			LCD_Command = Set_Page_Addr_X|i;  		delay();
			LCD_Command = Set_ColH_Addr_X|colh;   delay();
			LCD_Command = Set_ColL_Addr_X|coll;   delay();
			while (k--)
			{
				LCD_Data = *string++;
				delay();
			}	
			k = 8;
			LCD_Command = Set_Page_Addr_X|(i+1);	delay();
			LCD_Command = Set_ColH_Addr_X|colh;		delay();
			LCD_Command = Set_ColL_Addr_X|coll;		delay();
			while(k--)
			{
				LCD_Data = *string++;
				delay();
			}
		}
	}
}

//==========================================================================//
//            						     LCD_FullDrew                            	  	//
//==========================================================================//
void LCD_FullDrew_8_8(uint8_t *string)
{
	uint8_t i, j, k = 0;
	for(i = 0;i<= 6;i++)
	{
		for(j = 0;j < 128;j+= 8)
		{
			k = 8;
			unsigned char coll = j & 0x0f;
			unsigned char colh = j >> 4;
  
			LCD_Command = Set_Start_Line_X|0x0;   delay();
			LCD_Command = Set_Page_Addr_X|i;  		delay();
			LCD_Command = Set_ColH_Addr_X|colh;   delay();
			LCD_Command = Set_ColL_Addr_X|coll;   delay();
			while (k--)
			{
				LCD_Data = *string++;
				delay();
			}
		}
	}
}

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);

  return ch;
}

#ifdef USE_FULL_ASSERT

 /**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */ 
void assert_failed(uint8_t* file, uint32_t line)
{
 /* USER CODE BEGIN 6 */ 
 /* User can add his own implementation to report the file name and line number, 
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */ 
 /* USER CODE END 6 */ 

}

#endif

 /**
  * @}
  */ 

 /**
  * @}
*/ 

 /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
