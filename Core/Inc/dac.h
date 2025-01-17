/*
 * dac904.h
 *
 *  Created on: Dec 27, 2024
 *      Author: JA
 */

#ifndef INC_DAC904_H_
#define INC_DAC904_H_

#define MAX_US_PER_S  1000000
/*
 * length   LengthType
 * 355P      0
 * 355S      1
 * 386       2
 * 407       3
 * 532P      4
 * 532S      5
 * 607       6
 * 1064      7
 */
typedef enum {
    LENGTH_355P = 0,
    LENGTH_355S = 1,
    LENGTH_386 = 2,
    LENGTH_407 = 3,
    LENGTH_532P = 4,
    LENGTH_532S = 5,
    LENGTH_607 = 6,
    LENGTH_1064 = 7,
    LENGTH_UNKNOWN = 255  // 未知类型
} LengthType;

typedef struct
{
	uint8_t Enable;
	LengthType LengthType;
	uint8_t Type;
	uint32_t Freq;
	uint32_t Delay;
	}WAVE_STRUCT;
extern WAVE_STRUCT gWave;

void DAC904_WriteData(uint16_t data);
void DAC904_OutputValue(uint16_t value);
void DAC_CreateWave(LengthType length, uint8_t type);
void DAC_OutputWave(void);

#endif /* INC_DAC904_H_ */
