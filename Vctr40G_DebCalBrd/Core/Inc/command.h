/*
 * command.h
 *
 *  Created on: Mar 20, 2026
 *      Author: koptubenko_b
 */

#ifndef INC_COMMAND_H_
#define INC_COMMAND_H_

#include "main.h"

// === COMMANDS =====================================================

typedef void (*NaCommandFunc_t)(uint8_t objState);

void NA_Cmd00_NULL(uint8_t objState);
void NA_Cmd01_CAL1(uint8_t objState);
void NA_Cmd02_CAL2(uint8_t objState);
void NA_Cmd03_Refl(uint8_t objState);
void NA_Cmd04_Mix(uint8_t objState);
void NA_Cmd05_IF(uint8_t objState);
void NA_Cmd06_IFAtt(uint8_t objState);
void NA_Cmd07_SsynthAtt(uint8_t objState);
void NA_Cmd08_SVernier(uint8_t objState);
void NA_Cmd09_Lsynth(uint8_t objState);
void NA_Cmd10_Ssynth(uint8_t objState);
void NA_Cmd11_Port1(uint8_t objState);
void NA_Cmd12_Port2(uint8_t objState);
void NA_Cmd13_CalSignalSA(uint8_t objState);
void NA_Cmd14_PulseMOD(uint8_t objState);
void NA_Cmd15_NATest(uint8_t objState);
void NA_Cmd16_LSEn(uint8_t objState);
void NA_Cmd17_Ldiv10G(uint8_t objState);
void NA_Cmd18_Sdiv10G(uint8_t objState);
void NA_Cmd19_LS(uint8_t objState);
void NA_Cmd20_NA_S_Rfout(uint8_t objState);
void NA_Cmd21_NA_L_Rfout(uint8_t objState);

typedef void (*SaCommandFunc_t)(uint8_t objState);

void SA_Cmd00_NULL(uint8_t objState);
void SA_Cmd01_PrAmp(uint8_t objState);
void SA_Cmd02_PrAtt(uint8_t objState);
void SA_Cmd03_PrslCFsw(uint8_t objState);
void SA_Cmd04_PrslCFen1(uint8_t objState);
void SA_Cmd05_PrslCFen2(uint8_t objState);
void SA_Cmd06_CF17Mixen(uint8_t objState);
void SA_Cmd07_Mix1(uint8_t objState);
void SA_Cmd08_Mix2(uint8_t objState);
void SA_Cmd09_IF1(uint8_t objState);
void SA_Cmd10_IF2(uint8_t objState);
void SA_Cmd11_IF3(uint8_t objState);
void SA_Cmd12_AttIF2(uint8_t objState);
void SA_Cmd13_AttIF3(uint8_t objState);
void SA_Cmd14_AttDC(uint8_t objState);
void SA_Cmd15_LO3(uint8_t objState);
void SA_Cmd16_LO2(uint8_t objState);
void SA_Cmd17_LO1(uint8_t objState);
void SA_Cmd18_SAout(uint8_t objState);
void SA_Cmd19_SATest(uint8_t objState);
void SA_Cmd20_SAEn1(uint8_t objState);
void SA_Cmd21_SAEn2(uint8_t objState);
void SA_Cmd22_LO_Rfout(uint8_t objState);

#define NA_MAX_COMMANDS 		22
#define SA_MAX_COMMANDS 		23

extern NaCommandFunc_t NaCommandTable[NA_MAX_COMMANDS];
extern SaCommandFunc_t SaCommandTable[SA_MAX_COMMANDS];

#define RorW_BIT_NUM_CMD 		7
#define NAorSA_BIT_NUM_CMD 		6
#define OBJ_NUM_CMD 			0x3F
/**
 *****************************************************
 * !! VARIABLES FOR STORAGE OF RECEIVED COMMAND !!
 * cmd number for command service > 0 if not
 *****************************************************
 */
extern volatile uint8_t pending_cmd;	// first byte cmd
extern volatile uint8_t objState_cmd;	// second byte cmd
// start command
extern void ProcessCommandTask(uint8_t pendingCmd, uint8_t objStateCmd);

// === CONTINUOUS PROCESS FLAGS ======================================

#define PROC_NONE 0x00
// bit 0: PulseMOD process automatic operation  (signal ON/OFF fast switching)
#define PROC_PULSEMOD        (1 << 0)
#define PROC_NA_DYNAMIC_TEST (1 << 1) // bit 1: dynamic test NA latchs
#define PROC_SA_DYNAMIC_TEST (1 << 2) // bit 2: dynamic test SA latchs
#define PROC_3               (1 << 3) // bit 3: reserved
#define PROC_4               (1 << 4) // bit 4: reserved
#define PROC_5               (1 << 5) // bit 5: reserved
#define PROC_6               (1 << 6) // bit 6: reserved
#define PROC_7               (1 << 7) // bit 7: reserved

extern volatile uint8_t processFlags; // only one bit active at a time

// PD4 = NA_S_MOD1orCAL, PD5 = NA_S_MOD2
// 00 = signal ON, 11 = signal OFF
#define MOD_PINS_MASK (GPIO_PIN_4 | GPIO_PIN_5) // bits 4,5

extern void PulseMOD_Process(void); // call from while(1)

// === LATCHES =====================================================

#define NA_LATCH_STROBE_Port		GPIOH
#define NA_LATCH_STROBE_Pin 		GPIO_PIN_11
#define SA_LATCH_STROBE_Port		GPIOG
#define SA_LATCH_STROBE_Pin 		GPIO_PIN_6

#define NA_LATCH_COUNT 10
#define SA_LATCH_COUNT 13
extern volatile uint8_t naTest_latchIdx;
extern volatile uint8_t saTest_latchIdx;
extern void NATest_Process(void); // call from while(1)
extern void SATest_Process(void); // call from while(1)
/**
 *************************************************************
 * !! VARIABLES FOR STORAGE OF SCH LATCH REGISTER's STATE !!
 * cmd number for command service > 0 if not
 *************************************************************
 */
extern volatile uint8_t na_latch0_CAL;  			
extern volatile uint8_t na_latch1_SwMix_SwRefl;
extern volatile uint8_t na_latch2_MixEn_SwSAin_NaEn;
extern volatile uint8_t na_latch3_IF;
extern volatile uint8_t na_latch4_IFAtt;
extern volatile uint8_t na_latch5_LEn_SEn_MOD;
extern volatile uint8_t na_latch6_L_S;
extern volatile uint8_t na_latch7_LDiv_SDiv;
extern volatile uint8_t na_latch8_SAtt;
extern volatile uint8_t na_latch9_SVerAtt_SwP;

extern volatile uint8_t sa_latch0_PrAmp;
extern volatile uint8_t sa_latch1_PrAtt;
extern volatile uint8_t sa_latch2_PrslSw1;
extern volatile uint8_t sa_latch3_PrslSw2_CF17En_Mix;
extern volatile uint8_t sa_latch4_CF01to08En;
extern volatile uint8_t sa_latch5_CF09to16En;
extern volatile uint8_t sa_latch6_CnvrSw;
extern volatile uint8_t sa_latch7_IF2Att;
extern volatile uint8_t sa_latch8_IF3Att;
extern volatile uint8_t sa_latch9_DCAtt;
extern volatile uint8_t sa_latch10_LO2;
extern volatile uint8_t sa_latch11_IF2_IF3;
extern volatile uint8_t sa_latch12_LO1_LO3;

// 	latch byte
extern void na_write_to_latch(uint8_t address, uint8_t data);
extern void sa_write_to_latch(uint8_t address, uint8_t data);


// === SA Canal Filters =====================================================
typedef enum {
    CF01,
    CF02,
    CF03,
    CF04,
    CF05,
    CF06,
    CF07,
    CF08,
    CF09,
    CF10,
    CF11,
    CF12,
    CF13,
    CF14,
    CF15,
    CF16,
    CF17
} SaCF;

// === ADF4368 synthesizer programming ======================================
typedef enum {
    SYNTH_NA_S_SPI2 = 0,
    SYNTH_NA_L_SPI3 = 1,
    SYNTH_SA_SPI6   = 2
} Adf4368Synth_t;

HAL_StatusTypeDef ADF4368_SetRfoutHz(Adf4368Synth_t synth, uint32_t rfoutHz);
HAL_StatusTypeDef ADF4368_SetAllRfoutHz(uint32_t spi2RfoutHz, uint32_t spi3RfoutHz, uint32_t spi6RfoutHz);

extern void MyCmdUserTask(void); // function declaration TEMPLATE

#endif /* INC_COMMAND_H_ */