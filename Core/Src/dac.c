/*
 * dac904.c
 *
 *  Created on: Dec 27, 2024
 *      Author: JA
 */
#include "main.h"
#include "gpio.h"
#include "dac.h"

//volatile uint16_t dac_value[10] = {8192, 8055, 7918, 7781, 7644, 7507, 7370, 7233, 7096, 6959};
volatile uint16_t dac_value[10] = {8192, 7919, 7646, 7373, 7100, 6827, 6554, 6281, 6008, 5735};
uint16_t dac_value2[10] = {8192, 7646, 7096, 6554, 6008, 5462, 4912, 4370, 3824, 3278};
WAVE_STRUCT gWave = {.Enable = 0, .LengthType = LENGTH_532P, .Type = 0, .Freq = 1000, .Delay = 100};

// 发送数据到DAC904
void DAC904_WriteData(uint16_t data)
{
    //*(__IO uint16_t *)(0x60000000) = data;
	GPIOC->BSRR = (0x1 << (3 + 16));
	GPIOE->ODR = data;
	if (data & 0x0004) {
		GPIOD->BSRR = (0x1 << 7);
	} else {
		GPIOD->BSRR = (0x1 << (7 + 16));
	}
	__NOP();__NOP();
	GPIOC->BSRR = (0x1 << 3);
}

// DAC904输出电压
void DAC904_OutputValue(uint16_t value)
{
    value = 16384 - (uint32_t)value*16384/6000;
    DAC904_WriteData(value);
}

void DAC_CreateWave(LengthType length, uint8_t type) {
	gWave.LengthType = length;
	gWave.Type = type;
	switch(gWave.Type) {
		case 0:
			gWave.Freq = 1000;
			gWave.Delay = 100;
			break;
		case 1:
			gWave.Freq = 1000;
			gWave.Delay = 100;
			break;
		case 2:
			gWave.Freq = 1000;
			gWave.Delay = 100;
			break;
		case 3:
			gWave.Freq = 1000;
			gWave.Delay = 100;
			break;
		default:
			break;
	}
}

void delay_us(uint32_t delay)
{
    uint32_t i;
    for(i = 0; i < delay; i++) {
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
    }
}

void DAC_OutputWave(void) {
	static uint32_t cnt = 0;
	uint16_t output = 8192;  // Default output value

	if (gWave.Enable) {
		switch (gWave.Type) {
			case 0:
				if (cnt < gWave.Delay/10) output = dac_value[cnt];
				break;
			case 1:
				if (cnt == 0) output = 5735;
				break;
			case 2:
				if (cnt < gWave.Delay/2) output = 5735;
				break;
			case 3:
				if (cnt < gWave.Delay/10) output = dac_value2[cnt];
				break;
		}
	}

	DAC904_WriteData(output);
	cnt = (cnt + 1) >= gWave.Delay ? 0 : cnt + 1;
}

