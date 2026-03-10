//=====[#include guards - begin]===============================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

//=====[Declaration of public defines]=========================================
#define DISPLAY_CHAR_WIDTH 16
#define DISPLAY_ROWS		2


#define DISPLAY_DEL_37US	37ul
#define DISPLAY_DEL_01US	01ul

//=====[Declaration of public data types]======================================

typedef enum {
     DISPLAY_CONNECTION_GPIO_4BITS,
     DISPLAY_CONNECTION_GPIO_8BITS,
} displayConnection_t;

typedef struct {
   displayConnection_t connection;
} display_t;

//=====[Declarations (prototypes) of public functions]=========================

void displayInit( displayConnection_t connection );

void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY );

void displayStringWrite( const char * str );

void displayCharWrite(const char c);

void displayClearScreen(void);

void display_delay_us(uint32_t delay_us);

//=====[#include guards - end]=================================================

#endif // _DISPLAY_H_
