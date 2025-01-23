/*
 * dac904.c
 *
 *  Created on: Dec 27, 2024
 *      Author: JA
 */
#include "main.h"
#include "gpio.h"
#include "dac.h"

#define DAC904_BASE_ADDR  (0x60000000) // Bank1起始地址
volatile uint16_t *dac904 = (volatile uint16_t*)DAC904_BASE_ADDR;
//volatile uint16_t dac_value[10] = {8192, 8055, 7918, 7781, 7644, 7507, 7370, 7233, 7096, 6959}; // 0~500mV
//volatile uint16_t dac_value[10] = {8192, 7919, 7646, 7373, 7100, 6827, 6554, 6281, 6008, 5735}; // 0~1000mV
volatile uint16_t dac_value[10] = {8192, 7782, 7372, 6962, 6552, 6142, 5732, 5322, 4912, 4502}; // 0~1500mV
uint16_t dac_value2[10] = {8192, 7646, 7096, 6554, 6008, 5462, 4912, 4370, 3824, 3278}; //0~2000mV
WAVE_STRUCT gWave = {.Enable = 1, .LengthType = LENGTH_532P, .Type = 0, .Freq = 100, .Delay = 100};

// 将电压值(mV)转换为DAC输出值
uint16_t Voltage_To_DAC(float voltage_mv) {
    // 8192对应0V，0对应3000mV
    // DAC值 = 8192 - (voltage_mv * 8192 / 3000)
    uint16_t dac_val = 8192 - (uint16_t)((voltage_mv * 8192.0f) / 3000.0f);
    
    // 范围保护
    if(dac_val > 8192) dac_val = 8192;
    //if(dac_val < 0) dac_val = 0;
    
    return dac_val;
}

// 根据输入电压范围更新DAC数组
void DAC_UpdateArray(float max_voltage_mv) {
    float step = max_voltage_mv / 9.0f;  // 分9步
    
    for(int i = 0; i < 10; i++) {
        float current_mv = step * i;
        dac_value[i] = Voltage_To_DAC(current_mv);
    }
}

// 发送数据到DAC904
void DAC904_WriteData(uint16_t data)
{
    *dac904 = data; // 写入数据，触发FMC写时序
	// GPIOC->BSRR = (0x1 << (3 + 16));
	// GPIOE->ODR = data;
	// if (data & 0x0004) {
	// 	GPIOD->BSRR = (0x1 << 7);
	// } else {
	// 	GPIOD->BSRR = (0x1 << (7 + 16));
	// }
	// __NOP();__NOP();
	// GPIOC->BSRR = (0x1 << 3);
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
		case TEST_LINEARITY:  // 线性度测试
			gWave.Freq = 100;
			gWave.Delay = 100;
			break;
		case TEST_CROSSTALK:  // 串扰测试
			gWave.Freq = 100;
			gWave.Delay = 100;
			break;
		case TEST_GAINRATIO:  // 增益比测试
			gWave.Freq = 100;
			gWave.Delay = 100;
			break;
		case TEST_DYNAMICRANGE:  // 动态范围测试
			gWave.Freq = 100;
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
        __NOP();__NOP();__NOP();__NOP();
    }
}

void DAC_OutputWave(void) {
	static uint32_t cnt = 0;
	uint16_t output = 8192;  // Default output value

	if (gWave.Enable) {
		switch (gWave.Type) {
			case TEST_LINEARITY: // 线性度测试
				if (cnt < gWave.Freq/10) output = dac_value[cnt];
				break;
			case TEST_CROSSTALK: // 串扰测试
				if (cnt == 0) output = 5735;
				break;
			case TEST_GAINRATIO: // 增益比测试
				if (cnt == 0) output = 5735;
				break;
			case TEST_DYNAMICRANGE: // 动态范围测试
				if (cnt < gWave.Freq/10) output = dac_value2[cnt];
				break;
		}
	}

	DAC904_WriteData(output);
	cnt = (cnt + 1) >= gWave.Freq ? 0 : cnt + 1;
	delay_us(1);
}

