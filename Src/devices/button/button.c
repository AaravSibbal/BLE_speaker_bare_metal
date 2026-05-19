#include "button.h"

volatile uint8_t button_history = ((uint8_t)0x00);
// okay here are the things that a button needs 
/***
* a button needs to inturrupt and all
	- this is handled by exti but it doesn't need 
	exti it need the things that exti config needs tho

*/
