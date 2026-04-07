/*
 * button_led.h
 *
 *  Created on: Mar 20, 2026
 *      Author: koptubenko_b
 */

#ifndef INC_BUTTON_LED_H_
#define INC_BUTTON_LED_H_

#include "main.h"

// === BUTTON ===========================================================================

#define GPIO_BTN_PRESSED	     		(GPIO_PIN_SET)	 			// "1" for this STM32F407G kit sch
#define GPIO_BTN_NOT_PRESSED          	(!GPIO_BTN_PRESSED)


#define BTN_NUM         	8		// 1 for SCH STM32F407G kit
#define HOLD_TICKS          500		// 50 =< t < 500 msec - short press
#define LONG_HOLD_TICKS     3000	// 500 =< t < 3000  - long press, t >3000 - very long press
#define DEBOUNCE_TICKS      50

typedef enum {
	BTN_NOT_PRESSED,
	BTN_PRESSED,					// DEBOUNCE_TICKS < BTN_PRESSED =< HOLD_TICKS
    BTN_HOLD,						// HOLD_TICKS < BTN_HOLD =< LONG_HOLD_TICKS
	BTN_LONG_HOLD
} ButtonEvent;

// point to handler, button index & buttonEvent (state)
typedef void (*ButtonCallBack) (uint8_t index, ButtonEvent buttonEvent);

typedef struct {
	GPIO_TypeDef* port;
	    uint16_t pin;
	    uint32_t timer;         									// debounce counter and Hold
	    GPIO_PinState lastState;
	    uint8_t is_held;        									// flag of Hold
	    ButtonCallBack callback;									// CallBack function
} McuPin;

// button handler
extern void OnButtonEvent(uint8_t index, ButtonEvent buttonEvent);

#define B1_PortF			GPIOF
#define B2_PortF			GPIOF
#define B3_PortF			GPIOF
#define B4_PortF			GPIOF
#define B5_PortF			GPIOF
#define B6_PortH			GPIOH
#define B7_PortH			GPIOH
#define B8_PortF			GPIOF

// mcuPin_Init()
extern McuPin mcuPin[BTN_NUM];

// === LED ===========================================================================

#define LED_NUM         	BTN_NUM
//#define LED_TIMER_MASK     	7		// 0111
#define SLOW_BLINK_PERIOD   1000		// ms
#define FAST_BLINK_PERIOD	250
#define LED_MODE_COUNT      4 			// 0 - OFF, 1 - ON, 2 - SLOW_BLINK, 3 - FAST_BLINK

typedef enum {
    LED_OFF,
    LED_ON,
    LED_SLOW_BLINK,
    LED_FAST_BLINK,
} LedEvent;

typedef struct {
	GPIO_TypeDef* led_port;
	uint16_t led_pin;
	uint32_t led_timer;
	uint8_t led_is_held;			// flag of Hold
} LedPin;

// led handler
extern void LED_Handler(uint8_t index);

#define L1_PortC			GPIOC
#define L2_PortC			GPIOC
#define L3_PortI			GPIOI
#define L4_PortC			GPIOC
#define L5_PortI			GPIOI
#define L6_PortI			GPIOI
#define L7_PortI			GPIOI
#define L8_PortI			GPIOI

// ledPin_Init()
extern LedPin ledPin[LED_NUM];

void MyUserTask(void);			// function declaration - TEMPLATE


#endif /* INC_BUTTON_LED_H_ */
