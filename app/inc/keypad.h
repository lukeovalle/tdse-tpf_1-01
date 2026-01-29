#ifndef KEYPAD_H
#define KEYPAD_H

typedef enum {
    KEY_1, KEY_2, KEY_3, KEY_A,
    KEY_4, KEY_5, KEY_6, KEY_B,
    KEY_7, KEY_8, KEY_9, KEY_C,
    KEY_STAR, KEY_0, KEY_HASH, KEY_D,
    KEY_NONE
} keypad_key_t;

keypad_key_t keypad_scan(void);

#endif