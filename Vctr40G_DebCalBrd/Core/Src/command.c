/*
 * command.c
 *
 *  Created on: Mar 20, 2026
 *      Author: koptubenko_b
 */

#include "main.h"
#include "command.h"

volatile uint8_t pending_NA_cmd = -1;
volatile uint8_t pending_SA_cmd = -1;

volatile uint8_t na_latch0_CAL               = 0;
volatile uint8_t na_latch1_SwMix_SwRefl      = 0;
volatile uint8_t na_latch2_MixEn_SwSAin_NaEn = 0;
volatile uint8_t na_latch3_IF                = 0;
volatile uint8_t na_latch4_IFAtt             = 0;
volatile uint8_t na_latch5_LEn_SEn_MOD       = 0;
volatile uint8_t na_latch6_L_S               = 0;
volatile uint8_t na_latch7_LDiv_SDiv         = 0;
volatile uint8_t na_latch8_SAtt              = 0;
volatile uint8_t na_latch9_SVerAtt_SwP       = 0;

volatile uint8_t sa_latch0_PrAmp              = 0;
volatile uint8_t sa_latch1_PrAtt              = 0;
volatile uint8_t sa_latch2_PrslSw1            = 0;
volatile uint8_t sa_latch3_PrslSw2_CF17En_Mix = 0;
volatile uint8_t sa_latch4_CF01to08En         = 0;
volatile uint8_t sa_latch5_CF09to16En         = 0;
volatile uint8_t sa_latch6_CnvrSw             = 0;
volatile uint8_t sa_latch7_IF2Att             = 0;
volatile uint8_t sa_latch8_IF3Att             = 0;
volatile uint8_t sa_latch9_DCAtt              = 0;
volatile uint8_t sa_latch10_LO2               = 0;
volatile uint8_t sa_latch11_IF2_IF3           = 0;
volatile uint8_t sa_latch12_LO1_LO3           = 0;

extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi6;

#define ADF4368_REF_HZ            100000000UL
#define ADF4368_R_COUNTER         1U
#define ADF4368_PRESCALER         8U
#define ADF4368_SPI_TIMEOUT_MS    10U

static SPI_HandleTypeDef *adf4368_get_spi(Adf4368Synth_t synth)
{
    switch (synth) {
    case SYNTH_NA_S_SPI2:
        return &hspi2;
    case SYNTH_NA_L_SPI3:
        return &hspi3;
    case SYNTH_SA_SPI6:
        return &hspi6;
    default:
        return NULL;
    }
}

static HAL_StatusTypeDef adf4368_write_latch(SPI_HandleTypeDef *hspi, uint32_t word24)
{
    uint8_t tx[3];
    tx[0] = (uint8_t)((word24 >> 16) & 0xFFU);
    tx[1] = (uint8_t)((word24 >> 8) & 0xFFU);
    tx[2] = (uint8_t)(word24 & 0xFFU);
    return HAL_SPI_Transmit(hspi, tx, 3, ADF4368_SPI_TIMEOUT_MS);
}

HAL_StatusTypeDef ADF4368_SetRfoutHz(Adf4368Synth_t synth, uint32_t rfoutHz)
{
    SPI_HandleTypeDef *hspi = adf4368_get_spi(synth);
    if (hspi == NULL || rfoutHz == 0U) {
        return HAL_ERROR;
    }

    const uint32_t fpfdHz = ADF4368_REF_HZ / ADF4368_R_COUNTER;
    const uint32_t n      = (rfoutHz + (fpfdHz / 2U)) / fpfdHz;

    if (n < ADF4368_PRESCALER) {
        return HAL_ERROR;
    }

    const uint32_t bCounter = n / ADF4368_PRESCALER;
    const uint32_t aCounter = n % ADF4368_PRESCALER;

    // ADF4368 24-bit latches: [DATA.. | C2 C1], where latch select is in bits [1:0].
    // NOTE: Control field defaults below are safe placeholders and can be tuned per board.
    const uint32_t rLatch = ((ADF4368_R_COUNTER & 0x3FFFU) << 2) | 0x0U; // R-counter latch
    const uint32_t nLatch = ((bCounter & 0x1FFFU) << 8) | ((aCounter & 0x1FU) << 2) | 0x1U;
    const uint32_t cLatch = (1UL << 21) | (1UL << 20) | 0x2U; // Control latch (CP gain/polarity defaults)

    HAL_StatusTypeDef st;
    st = adf4368_write_latch(hspi, cLatch);
    if (st != HAL_OK) {
        return st;
    }
    st = adf4368_write_latch(hspi, rLatch);
    if (st != HAL_OK) {
        return st;
    }
    st = adf4368_write_latch(hspi, nLatch);
    return st;
}

HAL_StatusTypeDef ADF4368_SetAllRfoutHz(uint32_t spi2RfoutHz, uint32_t spi3RfoutHz, uint32_t spi6RfoutHz)
{
    HAL_StatusTypeDef st;

    st = ADF4368_SetRfoutHz(SYNTH_NA_S_SPI2, spi2RfoutHz);
    if (st != HAL_OK) {
        return st;
    }

    st = ADF4368_SetRfoutHz(SYNTH_NA_L_SPI3, spi3RfoutHz);
    if (st != HAL_OK) {
        return st;
    }

    return ADF4368_SetRfoutHz(SYNTH_SA_SPI6, spi6RfoutHz);
}

static uint32_t adf4368_decode_rfout_from_cmd(uint8_t freqCode)
{
    // One-byte frequency coding: RFOUT = code * 100 MHz
    // Valid for ADF4368 integer-N setup here: code >= 8 (N >= prescaler 8)
    if (freqCode < ADF4368_PRESCALER) {
        return 0U;
    }
    return ((uint32_t)freqCode) * ADF4368_REF_HZ;
}

void na_write_to_latch(uint8_t address, uint8_t data)
{
    // data (PE8-PE15)
    GPIOE->ODR = (GPIOE->ODR & 0x00FF) | ((uint16_t)data << 8);
    // address (PH7-PH10), strobe (PH11 High)
    GPIOH->ODR = (GPIOH->ODR & ~0xF80) | ((uint16_t)(address & 0x0F) << 7) | (1 << 11);
    // strobe PH11 (High -> Low -> High)
    GPIOH->BSRR = GPIO_BSRR_BR11;
    __NOP();
    GPIOH->BSRR = GPIO_BSRR_BS11;
}
void sa_write_to_latch(uint8_t address, uint8_t data)
{
    // data (PD8-PD15)
    GPIOD->ODR = (GPIOD->ODR & 0x00FF) | ((uint16_t)data << 8);
    // address (PG2-PG6), strobe (PG7 High)
    GPIOG->ODR = (GPIOG->ODR & ~0x1FC) | ((uint16_t)(address & 0x1F) << 2) | (1 << 7);
    // strobe PG7 (High -> Low -> High)
    GPIOG->BSRR = GPIO_BSRR_BR7;
    __NOP();
    GPIOG->BSRR = GPIO_BSRR_BS7;
}

// === NA handlers ======================================
void NA_Cmd00_NULL(uint8_t objState)	{ /* a dummy for idle indexes */ }
void NA_Cmd01_CAL1(uint8_t objState)
{
    switch (objState & 0x3) {
    case 0: { // Through
        na_latch0_CAL &= ~(1 << 1);
        na_latch0_CAL &= ~(1 << 2);
        break;
    }
    case 1: { // Math
        na_latch0_CAL |= (1 << 1);
        na_latch0_CAL &= ~(1 << 2);
        break;
    }
    case 2: { // Open
        na_latch0_CAL &= ~(1 << 1);
        na_latch0_CAL |= (1 << 2);
        break;
    }
    default: // Short
        na_latch0_CAL |= (1 << 1);
        na_latch0_CAL |= (1 << 2);
        break;
    }
    if (objState & 0x04) {
        // Port1 & NA -> CAL1: NA_Sw_Cal1 = 1; CAL1 -> Out1 (Port1)
        na_latch0_CAL |= (1 << 0) | (1 << 3);
    } else {
        // Port1 & NA -> CAL1: NA_Sw_Cal1 = 1; CAL1 -> Out2 (NA)
        na_latch0_CAL |= (1 << 0);
        na_latch0_CAL &= ~(1 << 3);
    }
    na_write_to_latch(0, na_latch0_CAL); // write to latch A=0
}
void NA_Cmd02_CAL2(uint8_t objState)
{
    switch (objState & 0x3) {
    case 0: { // Through
        na_latch0_CAL &= ~(1 << 5);
        na_latch0_CAL &= ~(1 << 6);
        break;
    }
    case 1: { // Match
        na_latch0_CAL |= (1 << 5);
        na_latch0_CAL &= ~(1 << 6);
        break;
    }
    case 2: { // Open
        na_latch0_CAL &= ~(1 << 5);
        na_latch0_CAL |= (1 << 6);
        break;
    }
    default: // Short
        na_latch0_CAL |= (1 << 5);
        na_latch0_CAL |= (1 << 6);
        break;
    }
    if (objState & 0x04) {
        // Port2 & NA -> CAL2: NA_Sw_Cal2 = 1; CAL2 -> Out1 (Port2)
        na_latch0_CAL |= (1 << 4) | (1 << 7);
    } else {
        // Port2 & NA -> CAL2: NA_Sw_Cal2 = 1; CAL2 -> Out2 (NA)
        na_latch0_CAL |= (1 << 4);
        na_latch0_CAL &= ~(1 << 7);
    }
    na_write_to_latch(0, na_latch0_CAL); // write to latch A=0
}
void NA_Cmd03_Refl(uint8_t objState)
{
    switch (objState & 0x3) {
    case 0: { // Coupler1(40G)
        na_latch1_SwMix_SwRefl &= ~(1 << 4);
        na_latch1_SwMix_SwRefl &= ~(1 << 5);
        na_latch1_SwMix_SwRefl &= ~(1 << 6);
        na_latch1_SwMix_SwRefl &= ~(1 << 7);
        break;
    }
    case 1: { // Coupler2(20G)
        na_latch1_SwMix_SwRefl &= ~(1 << 4);
        na_latch1_SwMix_SwRefl |= (1 << 5);
        na_latch1_SwMix_SwRefl &= ~(1 << 6);
        na_latch1_SwMix_SwRefl |= (1 << 7);
        break;
    }
    case 2: { // Coupler3(4G)
        na_latch1_SwMix_SwRefl |= (1 << 4);
        na_latch1_SwMix_SwRefl &= ~(1 << 5);
        na_latch1_SwMix_SwRefl |= (1 << 6);
        na_latch1_SwMix_SwRefl &= ~(1 << 7);
        break;
    }
    default: // Coupler4(1G)
        na_latch1_SwMix_SwRefl |= (1 << 4);
        na_latch1_SwMix_SwRefl |= (1 << 5);
        na_latch1_SwMix_SwRefl |= (1 << 6);
        na_latch1_SwMix_SwRefl |= (1 << 7);
        break;
    }
    na_write_to_latch(1, na_latch1_SwMix_SwRefl); // write to latch A=1
}
void NA_Cmd04_Mix(uint8_t objState)
{
    switch (objState & 0x3) {
    case 0: { // Pass(MixBypass)
        na_latch1_SwMix_SwRefl &= ~(1 << 0);
        na_latch1_SwMix_SwRefl &= ~(1 << 1);
        break;
    }
    case 1: { // 10G(Mix2)
        na_latch1_SwMix_SwRefl |= (1 << 0);
        na_latch1_SwMix_SwRefl &= ~(1 << 1);
        break;
    }
    case 2: { // 40G(Mix1)
        na_latch1_SwMix_SwRefl &= ~(1 << 0);
        na_latch1_SwMix_SwRefl |= (1 << 1);
        break;
    }
    default: // 4G(Mix3)
        na_latch1_SwMix_SwRefl |= (1 << 0);
        na_latch1_SwMix_SwRefl |= (1 << 1);
        break;
    }
    na_write_to_latch(1, na_latch1_SwMix_SwRefl);       // write to latch A=1
    na_latch2_MixEn_SwSAin_NaEn |= (objState >> 2);     // En Mix1...Mix4 and
    na_write_to_latch(2, na_latch2_MixEn_SwSAin_NaEn);  // write to latch A=2
}
void NA_Cmd05_IF(uint8_t objState)
{
    na_latch3_IF &= 0xF0;
    na_latch3_IF |= (objState & 0x0F);
    na_write_to_latch(3, na_latch3_IF);
}
void NA_Cmd06_IFAtt(uint8_t objState) { 
    na_latch4_IFAtt &= 0xC0;
    na_latch4_IFAtt |= (objState & 0x3F);
    na_write_to_latch(4, na_latch4_IFAtt);
  }
void NA_Cmd07_SsynthAtt(uint8_t objState)
{
    na_latch8_SAtt &= 0x40;
    na_latch8_SAtt |= ((objState & 0x3F)<<1);   // Att data
    na_write_to_latch(8, na_latch8_SAtt);       // 0->1->0 strobe
    na_latch8_SAtt |= (1 << 0);
    na_write_to_latch(8, na_latch8_SAtt);
    na_latch8_SAtt &= ~(1 << 0);
    na_write_to_latch(8, na_latch8_SAtt);
}
void NA_Cmd08_SVernier(uint8_t objState)
{
    na_latch9_SVerAtt_SwP &= 0xC0;
    na_latch9_SVerAtt_SwP |= (objState & 0x3F);
    na_write_to_latch(9, na_latch9_SVerAtt_SwP);
}
void NA_Cmd09_Lsynth(uint8_t objState)
{
    na_latch6_L_S &= 0xFC;
    na_latch6_L_S |= (objState & 0x03);
    na_write_to_latch(6, na_latch6_L_S);
}
void NA_Cmd10_Ssynth(uint8_t objState)
{
     na_latch6_L_S &= 0xCF;
     na_latch6_L_S |= ((objState & 0x03) << 4);
     na_write_to_latch(6, na_latch6_L_S);
}
void NA_Cmd11_Port1(uint8_t objState)
{
     na_latch0_CAL &= 0xFE; // 0 Port1->NAin, 1 Port1->CAL1
     na_latch0_CAL |= objState & 0x01;
     na_write_to_latch(0, na_latch0_CAL);
}
void NA_Cmd12_Port2(uint8_t objState)
{
     na_latch2_MixEn_SwSAin_NaEn &= 0xDF; // NA_SwSA_IN: 00 Port2->NAin, 01 Port2->SAin
     na_latch2_MixEn_SwSAin_NaEn |= ((objState & 0x01) << 5);
     na_write_to_latch(2, na_latch2_MixEn_SwSAin_NaEn);

     na_latch0_CAL &= 0xEF; // NA_Sw_Cal2: 1x Port2->CAL2
     na_latch0_CAL |= ((objState & 0x02) << 3);
     na_write_to_latch(0, na_latch0_CAL);
}
void NA_Cmd13_CalSignalSA(uint8_t objState)
{
     na_latch2_MixEn_SwSAin_NaEn &= 0xBF; // 1: NA_En
     na_latch2_MixEn_SwSAin_NaEn |= ((objState & 0x02) << 5);
     na_write_to_latch(2, na_latch2_MixEn_SwSAin_NaEn);

     na_latch5_LEn_SEn_MOD &= 0xBF; // S_MOD3=1: CalSignal->SA
     na_latch5_LEn_SEn_MOD |= ((objState & 0x01) << 6);
     na_write_to_latch(5, na_latch5_LEn_SEn_MOD);
}
void NA_Cmd14_PulseMOD(uint8_t objState)
{
     if (objState & 0x02) { // Start: activate PulseMOD
        processFlags |= (objState & 0x01);
        na_latch5_LEn_SEn_MOD &= ~(1 << 6); // S_MOD3 = 0: S PulseMOD
        na_write_to_latch(5, na_latch5_LEn_SEn_MOD);
     } else {
        // Stop: deactivate PulseMOD
        processFlags &= ~PROC_PULSEMOD;
        if (objState & 0x01) {
            // Signal OFF: PD4=1, PD5=1 (set both via lower halfword)
            GPIOD->BSRR = MOD_PINS_MASK;
        }
     }
}
void NA_Cmd15_NATest(uint8_t objState)
{
     if (objState) {
        naTest_latchIdx = 0; // reset to first latch
        processFlags |= PROC_NA_DYNAMIC_TEST;
     } else {
        processFlags &= ~PROC_NA_DYNAMIC_TEST;
        // Clear all latches to 0x00 on stop
        for (uint8_t i = 0; i < NA_LATCH_COUNT; i++) {
            na_write_to_latch(i, 0x00);
        }
     }
}
void NA_Cmd16_LSEn(uint8_t objState)
{
     na_latch5_LEn_SEn_MOD &= 0xC0; // L&S_En
     na_latch5_LEn_SEn_MOD |= (objState & 0x3F);
     na_write_to_latch(5, na_latch5_LEn_SEn_MOD);
}
void NA_Cmd17_Ldiv10G(uint8_t objState)
{
     na_latch7_LDiv_SDiv &= 0xF8; // Ldiv 10G: 00 1x, 01 1/2, 10 1/4, 11 1/8
     na_latch7_LDiv_SDiv |= (objState & 0x07);
     na_write_to_latch(7, na_latch7_LDiv_SDiv);
}
void NA_Cmd18_Sdiv10G(uint8_t objState)
{
     na_latch7_LDiv_SDiv &= 0xC7; // Sdiv 10G: 00 1x, 01 1/2, 10 1/4, 11 1/8
     na_latch7_LDiv_SDiv |= ((objState & 0x07) << 3);
     na_write_to_latch(7, na_latch7_LDiv_SDiv);
}
void NA_Cmd19_LS(uint8_t objState)
{
     na_latch6_L_S &= 0xB3; // Lsynth: 10/40, SA/NA; Ssynth: 10/40
     na_latch6_L_S |= ((objState & 0x03) << 2);
     na_latch6_L_S |= ((objState & 0x40) << 4);
     na_write_to_latch(6, na_latch6_L_S);
}
void NA_Cmd20_NA_S_Rfout(uint8_t objState)
{
     uint32_t rfoutHz = adf4368_decode_rfout_from_cmd(objState);
     if (rfoutHz != 0U) {
         (void)ADF4368_SetRfoutHz(SYNTH_NA_S_SPI2, rfoutHz);
     }
}
void NA_Cmd21_NA_L_Rfout(uint8_t objState)
{
     uint32_t rfoutHz = adf4368_decode_rfout_from_cmd(objState);
     if (rfoutHz != 0U) {
         (void)ADF4368_SetRfoutHz(SYNTH_NA_L_SPI3, rfoutHz);
     }
}

// === SA handlers ======================================

volatile SaCF saCF;
void SA_Cmd00_NULL(uint8_t objState) { /* a dummy for idle indexes */ }
void SA_Cmd01_PrAmp(uint8_t objState)
{
     sa_latch0_PrAmp &= 0xF0;
     sa_latch0_PrAmp |= (objState & 0x0F);

     sa_write_to_latch(0, sa_latch0_PrAmp); // write to latch A=0
}
void SA_Cmd02_PrAtt(uint8_t objState)
{
     sa_latch1_PrAtt &= 0xC0;
     sa_latch1_PrAtt |= (objState & 0x3F);

     sa_write_to_latch(1, sa_latch1_PrAtt); // write to latch A=1
}
void SA_Cmd03_PrslCFsw(uint8_t objState)
{
     // write to A=0: SIN_V2, SIN_V1
     if ((objState >= CF01 && objState <= CF04) || (objState >= CF06 && objState <= CF08)) {
        sa_latch0_PrAmp &= ~(1 << 5); // 01
        sa_latch0_PrAmp |= (1 << 4);
     } else if (objState >= CF10 && objState <= CF16) {
        sa_latch0_PrAmp |= (1 << 5); // 11
        sa_latch0_PrAmp |= (1 << 4);
     } else if (objState == CF05 || objState == CF09) {
        sa_latch0_PrAmp |= (1 << 5); // 10
        sa_latch0_PrAmp &= ~(1 << 4);
     } else if (objState == CF17) {
        sa_latch0_PrAmp &= ~(1 << 5); // 00
        sa_latch0_PrAmp &= ~(1 << 4);
     }
     sa_write_to_latch(0, sa_latch0_PrAmp);

     // write to A=2: SHB_V2, SHB_V1
     if (objState == CF02) {
        sa_latch2_PrslSw1 &= ~(1 << 5); // 01
        sa_latch2_PrslSw1 |= (1 << 4);
     } else if (objState == CF04 || (objState >= CF06 && objState <= CF08)) {
        sa_latch2_PrslSw1 |= (1 << 5); // 11
        sa_latch2_PrslSw1 |= (1 << 4);
     } else if (objState == CF01) {
        sa_latch2_PrslSw1 |= (1 << 5); // 10
        sa_latch2_PrslSw1 &= ~(1 << 4);
     } else if (objState == CF03) {
        sa_latch2_PrslSw1 &= ~(1 << 5); // 00
        sa_latch2_PrslSw1 &= ~(1 << 4);
     }

     // write to A=2: SLB_V2, SLB_V1
     if (objState == CF12) {
        sa_latch2_PrslSw1 &= ~(1 << 1); // 01
        sa_latch2_PrslSw1 |= (1 << 0);
     } else if (objState == CF10 || (objState >= CF14 && objState <= CF16)) {
        sa_latch2_PrslSw1 |= (1 << 1); // 11
        sa_latch2_PrslSw1 |= (1 << 0);
     } else if (objState == CF11) {
        sa_latch2_PrslSw1 |= (1 << 1); // 10
        sa_latch2_PrslSw1 &= ~(1 << 0);
     } else if (objState == CF13) {
        sa_latch2_PrslSw1 &= ~(1 << 1); // 00
        sa_latch2_PrslSw1 &= ~(1 << 0);
     }
     // write to A=2: SLB1_V2, SLB1_V1
     if (objState == CF10) {
        sa_latch2_PrslSw1 &= ~(1 << 3); // 01
        sa_latch2_PrslSw1 |= (1 << 2);
     } else if (objState == CF16) {
        sa_latch2_PrslSw1 |= (1 << 3); // 11
        sa_latch2_PrslSw1 |= (1 << 2);
     } else if (objState == CF15) {
        sa_latch2_PrslSw1 |= (1 << 3); // 10
        sa_latch2_PrslSw1 &= ~(1 << 2);
     } else if (objState == CF14) {
        sa_latch2_PrslSw1 &= ~(1 << 3); // 00
        sa_latch2_PrslSw1 &= ~(1 << 2);
     }
     // write to A=2: SHB2
     if (objState == CF05) {
        sa_latch2_PrslSw1 &= ~(1 << 6); // 0
     } else if (objState == CF09) {
        sa_latch2_PrslSw1 |= (1 << 6); // 1
     }

     sa_write_to_latch(2, sa_latch2_PrslSw1);
     
     // write to A=3: SHB1_V2, SHB1_V2
     if (objState == CF06) {
        sa_latch3_PrslSw2_CF17En_Mix &= ~(1 << 1); // 01
        sa_latch3_PrslSw2_CF17En_Mix |= (1 << 0);
     } else if (objState == CF08) {
        sa_latch3_PrslSw2_CF17En_Mix |= (1 << 1); // 11
        sa_latch3_PrslSw2_CF17En_Mix |= (1 << 0);
     } else if (objState == CF07) {
        sa_latch3_PrslSw2_CF17En_Mix |= (1 << 1); // 10
        sa_latch3_PrslSw2_CF17En_Mix &= ~(1 << 0);
     } else if (objState == CF04) {
        sa_latch3_PrslSw2_CF17En_Mix &= ~(1 << 1); // 00
        sa_latch3_PrslSw2_CF17En_Mix &= ~(1 << 0);
     } 
     
     // write to A=3: SHB3
     if (objState == CF05) {
        sa_latch3_PrslSw2_CF17En_Mix &= ~(1 << 2); // 0
     } else if (objState == CF07) {
        sa_latch3_PrslSw2_CF17En_Mix |= (1 << 2); // 1
     }

     sa_write_to_latch(3, sa_latch3_PrslSw2_CF17En_Mix);
}
void SA_Cmd04_PrslCFen1(uint8_t objState)
{
     sa_latch4_CF01to08En = 0;
     sa_latch4_CF01to08En |= objState;

     sa_write_to_latch(4, sa_latch4_CF01to08En); // write to latch A=4
}
void SA_Cmd05_PrslCFen2(uint8_t objState)
{
     sa_latch5_CF09to16En = 0;
     sa_latch5_CF09to16En |= objState;

     sa_write_to_latch(5, sa_latch5_CF09to16En); // write to latch A=5
}
void SA_Cmd06_CF17Mixen(uint8_t objState)
{
     sa_latch3_PrslSw2_CF17En_Mix &= 0xF7; // CF17 En
     sa_latch3_PrslSw2_CF17En_Mix |= ((objState & 1) << 3);

     sa_latch3_PrslSw2_CF17En_Mix &= 0x9F; // Mix1.1 & Miv1.2 En
     sa_latch3_PrslSw2_CF17En_Mix |= ((objState & 6) << 4);

     sa_write_to_latch(3, sa_latch3_PrslSw2_CF17En_Mix);

     sa_latch6_CnvrSw &= 0xFD; // Mix2.1 En
     sa_latch6_CnvrSw |= ((objState & 8) >> 2);

     sa_write_to_latch(6, sa_latch6_CnvrSw);

     sa_latch11_IF2_IF3 &= 0x7F; // IF3 En
     sa_latch11_IF2_IF3 |= ((objState & 0x10) << 3);
     sa_latch11_IF2_IF3 &= 0xF7; // IF2 En
     sa_latch11_IF2_IF3 |= ((objState & 0x20) >> 2);

     sa_write_to_latch(11, sa_latch11_IF2_IF3);
}
void SA_Cmd07_Mix1(uint8_t objState)
{
     sa_latch3_PrslSw2_CF17En_Mix &= 0xEF; // Mix1 switch
     sa_latch3_PrslSw2_CF17En_Mix |= ((objState & 0x01) << 4);

     sa_write_to_latch(3, sa_latch3_PrslSw2_CF17En_Mix);
}
void SA_Cmd08_Mix2(uint8_t objState)
{
     sa_latch3_PrslSw2_CF17En_Mix &= 0x7F; // Mix2 switch
     sa_latch3_PrslSw2_CF17En_Mix |= ((objState & 0x01) << 7);
     sa_write_to_latch(3, sa_latch3_PrslSw2_CF17En_Mix);

     sa_latch6_CnvrSw &= 0xFE; // Mix2.1 In
     sa_latch6_CnvrSw |= ((objState & 0x02)>>1);
     sa_write_to_latch(6, sa_latch6_CnvrSw);
}
void SA_Cmd09_IF1(uint8_t objState)
{
     sa_latch6_CnvrSw &= 0xF3; // IF1 filters switch
     sa_latch6_CnvrSw |= ((objState & 0x03) << 2);

     sa_write_to_latch(6, sa_latch6_CnvrSw);
}
void SA_Cmd10_IF2(uint8_t objState)
{
     sa_latch6_CnvrSw &= 0xCF; // IF2 filters switch
     sa_latch6_CnvrSw |= ((objState & 0x03) << 4);

     sa_write_to_latch(6, sa_latch6_CnvrSw);
}
void SA_Cmd11_IF3(uint8_t objState)
{
     sa_latch9_DCAtt &= 0x7F; // 37.5 / DC
     sa_latch9_DCAtt |= ((objState & 0x01) << 7);

     sa_write_to_latch(9, sa_latch9_DCAtt);
}
void SA_Cmd12_AttIF2(uint8_t objState)
{
     sa_latch7_IF2Att &= 0x80; // IF2 Att
     sa_latch7_IF2Att |= (objState & 0x7F);

     sa_write_to_latch(7, sa_latch7_IF2Att);
}
void SA_Cmd13_AttIF3(uint8_t objState)
{
     sa_latch8_IF3Att &= 0x80; // IF3 Att
     sa_latch8_IF3Att |= (objState & 0x7F);

     sa_write_to_latch(8, sa_latch8_IF3Att);
}
void SA_Cmd14_AttDC(uint8_t objState)
{
     sa_latch9_DCAtt &= 0x80; // DC Att
     sa_latch9_DCAtt |= (objState & 0x7F);

     sa_write_to_latch(9, sa_latch9_DCAtt);
}
void SA_Cmd15_LO3(uint8_t objState)
{
     sa_latch12_LO1_LO3 &= 0xF8; // LO3 divider 1/2(/4/8)
     sa_latch12_LO1_LO3 |= (objState & 0x07);

     sa_write_to_latch(12, sa_latch12_LO1_LO3);
}
void SA_Cmd16_LO2(uint8_t objState)
{
     sa_latch10_LO2 &= 0xF8; // LO2 divider 1/2(/4/8)
     sa_latch10_LO2 |= (objState & 0x07);

     sa_write_to_latch(10, sa_latch10_LO2);
}
void SA_Cmd17_LO1(uint8_t objState)
{
     sa_latch12_LO1_LO3 &= 0xDF; // LO1 switch: 0 1xLO1, 1 4xLO1
     sa_latch12_LO1_LO3 |= ((objState & 0x01) << 5);

     sa_write_to_latch(12, sa_latch12_LO1_LO3);
}
void SA_Cmd18_SAout(uint8_t objState)
{
     // IF3_SOut: 0 IF3, 1 IF3->TP (A=8 D7)
     // IF2_SOut: 0 IF2, 1 IF2->TP (A=7 D7)
     // IF2_ST: 0 IF2->TP, 1 IF3->TP (A=0 D6)

     sa_latch8_IF3Att &= 0x7F;
     sa_latch8_IF3Att |= ((objState & 0x01) << 7);
     sa_write_to_latch(8, sa_latch8_IF3Att);

     sa_latch7_IF2Att &= 0x7F;
     sa_latch7_IF2Att |= ((objState & 0x02) << 6);
     sa_write_to_latch(7, sa_latch7_IF2Att);

     sa_latch0_PrAmp &= 0xBF;
     sa_latch0_PrAmp |= ((objState & 0x04) << 4);
     sa_write_to_latch(0, sa_latch0_PrAmp);
}
void SA_Cmd19_SATest(uint8_t objState)
{
     if (objState) {
        saTest_latchIdx = 0; // reset to first latch
        processFlags |= PROC_SA_DYNAMIC_TEST;
     } else {
        processFlags &= ~PROC_SA_DYNAMIC_TEST;
        // Clear all latches to 0x00 on stop
        for (uint8_t i = 0; i < SA_LATCH_COUNT; i++) {
            sa_write_to_latch(i, 0x00);
        }
     }
}
void SA_Cmd20_SAEn1(uint8_t objState)
{
     sa_latch11_IF2_IF3 &= 0xEF; // F412 En
     sa_latch11_IF2_IF3 |= ((objState & 0x01) << 4);
     sa_latch11_IF2_IF3 &= 0xDF; // F487 En
     sa_latch11_IF2_IF3 |= ((objState & 0x02) << 4);
     sa_latch11_IF2_IF3 &= 0xBF; // F862 En
     sa_latch11_IF2_IF3 |= ((objState & 0x04) << 4);
     sa_write_to_latch(11, sa_latch11_IF2_IF3);

     sa_latch6_CnvrSw &= 0xBF; // F40M (IF3_SF)
     sa_latch6_CnvrSw |= ((objState & 0x08) << 3);
     sa_latch6_CnvrSw &= 0x7F; // F40M En
     sa_latch6_CnvrSw |= ((objState & 0x10) << 3);
     sa_write_to_latch(6, sa_latch6_CnvrSw);

     sa_latch10_LO2 &= 0xEF; // LO2 En
     sa_latch10_LO2 |= ((objState & 0x20) >> 1);
     sa_write_to_latch(10, sa_latch10_LO2);

     sa_latch12_LO1_LO3 &= 0xF7; // LO3 En
     sa_latch12_LO1_LO3 |= ((objState & 0x40) >> 3);
     sa_latch12_LO1_LO3 &= 0xEF; // LO1 En
     sa_latch12_LO1_LO3 |= ((objState & 0x80) >> 4);
     sa_write_to_latch(12, sa_latch12_LO1_LO3);
}
void SA_Cmd21_SAEn2(uint8_t objState)
{
     sa_latch10_LO2 &= 0xDF; // DetLO2 En
     sa_latch10_LO2 |= ((objState & 0x01) << 5);
     sa_write_to_latch(10, sa_latch10_LO2);

     sa_latch12_LO1_LO3 &= 0xBF; // DetLO3 En
     sa_latch12_LO1_LO3 |= ((objState & 0x02) << 5);
     sa_latch12_LO1_LO3 &= 0x7F; // DeIF1 En
     sa_latch12_LO1_LO3 |= ((objState & 0x04) << 5);
     sa_write_to_latch(12, sa_latch12_LO1_LO3);
}
void SA_Cmd22_LO_Rfout(uint8_t objState)
{
     uint32_t rfoutHz = adf4368_decode_rfout_from_cmd(objState);
     if (rfoutHz != 0U) {
         (void)ADF4368_SetRfoutHz(SYNTH_SA_SPI6, rfoutHz);
     }
}


// === jump tables ======================================
NaCommandFunc_t NaCommandTable[NA_MAX_COMMANDS] = {
    [0] = NA_Cmd00_NULL,     [1] = NA_Cmd01_CAL1,         [2] = NA_Cmd02_CAL2,
    [3] = NA_Cmd03_Refl,     [4] = NA_Cmd04_Mix,          [5] = NA_Cmd05_IF,
    [6] = NA_Cmd06_IFAtt,    [7] = NA_Cmd07_SsynthAtt,    [8] = NA_Cmd08_SVernier,
    [9] = NA_Cmd09_Lsynth,   [10] = NA_Cmd10_Ssynth,      [11] = NA_Cmd11_Port1,
    [12] = NA_Cmd12_Port2,   [13] = NA_Cmd13_CalSignalSA, [14] = NA_Cmd14_PulseMOD,
    [15] = NA_Cmd15_NATest,  [16] = NA_Cmd16_LSEn,        [17] = NA_Cmd17_Ldiv10G,
    [18] = NA_Cmd18_Sdiv10G, [19] = NA_Cmd19_LS,          [20] = NA_Cmd20_NA_S_Rfout,
    [21] = NA_Cmd21_NA_L_Rfout};
SaCommandFunc_t SaCommandTable[SA_MAX_COMMANDS] = {
    [0] = SA_Cmd00_NULL,      [1] = SA_Cmd01_PrAmp,     [2] = SA_Cmd02_PrAtt,
    [3] = SA_Cmd03_PrslCFsw,  [4] = SA_Cmd04_PrslCFen1, [5] = SA_Cmd05_PrslCFen2,
    [6] = SA_Cmd06_CF17Mixen, [7] = SA_Cmd07_Mix1,      [8] = SA_Cmd08_Mix2,
    [9] = SA_Cmd09_IF1,       [10] = SA_Cmd10_IF2,      [11] = SA_Cmd11_IF3,
    [12] = SA_Cmd12_AttIF2,   [13] = SA_Cmd13_AttIF3,   [14] = SA_Cmd14_AttDC,
    [15] = SA_Cmd15_LO3,      [16] = SA_Cmd16_LO2,      [17] = SA_Cmd17_LO1,
    [18] = SA_Cmd18_SAout,    [19] = SA_Cmd19_SATest,   [20] = SA_Cmd20_SAEn1,
    [21] = SA_Cmd21_SAEn2,    [22] = SA_Cmd22_LO_Rfout};

void ProcessCommandTask(uint8_t pendingCmd, uint8_t objStateCmd)
{
    uint8_t cmd_idx = (pendingCmd & OBJ_NUM_CMD);
    pending_cmd     = 0; // reset for permit a new interrupt
    if (pendingCmd & (1 << NAorSA_BIT_NUM_CMD)) {
        if (SaCommandTable[cmd_idx] != NULL) {
            SaCommandTable[cmd_idx](objStateCmd); // SA command process
        }
    } else {
        if (NaCommandTable[cmd_idx] != NULL) {
            NaCommandTable[cmd_idx](objStateCmd); // NA command process
        }
    }
}

// === CONTINUOUS PROCESSES ======================================
volatile uint8_t processFlags = PROC_NONE; // only one bit active at a time

void PulseMOD_Process(void)
{
    // Signal ON: PD4=0, PD5=0 (reset both via upper halfword)
    GPIOD->BSRR = (uint32_t)MOD_PINS_MASK << 16;
    __NOP();
    // Signal OFF: PD4=1, PD5=1 (set both via lower halfword)
    GPIOD->BSRR = MOD_PINS_MASK;
}

volatile uint8_t naTest_latchIdx = 0;
void NATest_Process(void)
{
    uint8_t addr = naTest_latchIdx;

    // Fast toggle: 0xFF -> 0x00 -> 0xFF -> 0x00 on current latch
    na_write_to_latch(addr, 0xFF);
    na_write_to_latch(addr, 0x00);
    na_write_to_latch(addr, 0xFF);
    na_write_to_latch(addr, 0x00);

    // Advance to next latch in circle
    naTest_latchIdx++;
    if (naTest_latchIdx >= NA_LATCH_COUNT) {
        naTest_latchIdx = 0;
    }
}

volatile uint8_t saTest_latchIdx = 0;
void SATest_Process(void)
{
    uint8_t addr = saTest_latchIdx;

    // Fast toggle: 0xFF -> 0x00 -> 0xFF -> 0x00 on current latch
    sa_write_to_latch(addr, 0xFF);
    sa_write_to_latch(addr, 0x00);
    sa_write_to_latch(addr, 0xFF);
    sa_write_to_latch(addr, 0x00);

    // Advance to next latch in circle
    saTest_latchIdx++;
    if (saTest_latchIdx >= SA_LATCH_COUNT) {
        saTest_latchIdx = 0;
    }
}


void MyCmdUserTask(void) {			// function definition TEMPLATE
	__NOP();
	__NOP();
}

