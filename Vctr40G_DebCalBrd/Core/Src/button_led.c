/*
 * button_led.c
 *
 *  Created on: Mar 20, 2026
 *      Author: koptubenko_b
 */

#include "main.h"
#include "button_led.h"

// mcuPin_Init()
McuPin mcuPin[BTN_NUM] = {
    {B1_PortF, B1_PF11_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent},
    {B2_PortF, B2_PF12_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent},
    {B3_PortF, B3_PF13_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent},
    {B4_PortF, B4_PF14_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent},
    {B5_PortF, B5_PF15_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent},
    {B6_PortH, B6_PH2_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent},
    {B7_PortH, B7_PH3_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent},
    {B8_PortF, B8_PF10_Pin, 0, GPIO_BTN_NOT_PRESSED, BTN_NOT_PRESSED, OnButtonEvent}
};

// ledPin_Init()
LedPin ledPin[LED_NUM] = {
    {L1_PortC, L1_PC4_Pin, 0, LED_OFF},
	{L2_PortC, L2_PC5_Pin, 0, LED_OFF},
	{L3_PortI, L3_PI4_Pin, 0, LED_OFF},
	{L4_PortC, L4_PC13_Pin, 0, LED_OFF},
	{L5_PortI, L5_PI8_Pin, 0, LED_OFF},
	{L6_PortI, L6_PI9_Pin, 0, LED_OFF},
	{L7_PortI, L7_PI10_Pin, 0, LED_OFF},
	{L8_PortI, L8_PI11_Pin, 0, LED_OFF}
};

void OnButtonEvent(uint8_t index, ButtonEvent buttonEvent)
{
    if (buttonEvent == BTN_PRESSED) {
        ledPin[index].led_is_held++;
        if (ledPin[index].led_is_held > LED_FAST_BLINK) {
            ledPin[index].led_is_held = LED_ON;
        }
        // switched LED mode in first ON state
    } else if (buttonEvent == BTN_HOLD || buttonEvent == BTN_LONG_HOLD) {
        ledPin[index].led_is_held = LED_OFF;
    }
}

void LED_Handler(uint8_t i) {
    uint8_t ledMode = ledPin[i].led_is_held;
    ledPin[i].led_timer++;
    switch (ledMode) {
    case LED_OFF:
        HAL_GPIO_WritePin(ledPin[i].led_port, ledPin[i].led_pin, GPIO_PIN_RESET); // LED OFF
        break;
    case LED_ON:
        HAL_GPIO_WritePin(ledPin[i].led_port, ledPin[i].led_pin, GPIO_PIN_SET); // LED ON
        break;
    case LED_SLOW_BLINK:
        if (ledPin[i].led_timer % SLOW_BLINK_PERIOD < SLOW_BLINK_PERIOD / 2) {
              HAL_GPIO_WritePin(ledPin[i].led_port, ledPin[i].led_pin, GPIO_PIN_SET);
        } else {
              HAL_GPIO_WritePin(ledPin[i].led_port, ledPin[i].led_pin, GPIO_PIN_RESET);
        }
        break;
    case LED_FAST_BLINK:
        if (ledPin[i].led_timer % FAST_BLINK_PERIOD < FAST_BLINK_PERIOD / 2) {
              HAL_GPIO_WritePin(ledPin[i].led_port, ledPin[i].led_pin, GPIO_PIN_SET);
        } else {
              HAL_GPIO_WritePin(ledPin[i].led_port, ledPin[i].led_pin, GPIO_PIN_RESET);
        }
        break;
    }
}

void MyBtnUserTask(void) {			// function definition - TEMPLATE
	__NOP();
	__NOP();
}


