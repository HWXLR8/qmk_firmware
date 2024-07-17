#include "irene.h"
#include "i2c.h"
#include "lcd.h"
#include "timer1.h"

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QW 0
#define _RS 1
#define _LW 2
static bool lcd_off = false;
int i = 0;
int idle_mode = 0;
int timer_loop = 0;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QW] = KEYMAP( /* Qwerty */
  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,		    		KC_6,    KC_7,    KC_8,    KC_9,    KC_0    ,
  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                 KC_Y,    KC_U,    KC_I,    KC_O,    KC_P    ,
  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                 KC_H,    KC_J,    KC_K,    KC_L,    KC_SLSH ,
  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                 KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_ENT  ,
  KC_ESC,  KC_LGUI, KC_LSFT, KC_BSPC, SCMD(KC_S),           KC_DEL, KC_SPC,  MO(_RS),  KC_SCLN, KC_QUOT , 
                                      MT(MOD_LCTL, KC_TAB), MT(MOD_LALT, KC_CAPS)                                                     ),

[_RS] = KEYMAP( /* [> RAISE <] */
  KC_GRV,     KC_F10,    KC_F11,    KC_F12,    KC_PSCR,    KC_NO,    KC_NO,    KC_NO,     KC_MINS, KC_EQL,
  KC_VOLU,    KC_F7,     KC_F8,     KC_F9,     KC_SLCK,    KC_PGUP,  KC_INS,   KC_UP,     KC_LBRC, KC_RBRC ,
  KC_VOLD,    KC_F4,     KC_F5,     KC_F6,     KC_PAUS,    KC_PGDN,  KC_LEFT,  KC_DOWN,   KC_RGHT, KC_SLSH,
  KC_MUTE,    KC_F1,     KC_F2,     KC_F3,     KC_CAPS,    KC_HOME,  KC_END,   KC_COMM,   KC_DOT,  KC_BSLS ,
  TG(_LW),    KC_LGUI,   KC_LSFT,   KC_BSPC,   KC_RCTL,    KC_DEL,   KC_SPC,   KC_TRNS,   KC_SCLN, KC_QUOT ,         
                                               KC_LCTL,    KC_LALT                        ),
/*
 * insert home   up  end   pgup       ||      up     F7    F8    F9   F10
 *  del   left  down right pgdn       ||     down    F4    F5    F6   F11
 *       volup             reset      ||             F1    F2    F3   F12
 *       voldn  super shift bksp ctrl || alt space   L0  prtsc scroll pause
 */
[_LW] = KEYMAP( /* [> LOWER <] */
  KC_F13,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_BSLS,    KC_ASTR    ,
  KC_NO,    KC_NO,    KC_W,     KC_NO,    KC_NO,    KC_NO,    KC_7,     KC_8,     KC_9,       KC_MINS  ,
  KC_NO,    KC_A,     KC_S,     KC_D,     KC_NO,    KC_NO,    KC_4,     KC_5,     KC_6,       KC_PLUS  ,
  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_1,     KC_2,     KC_3,       KC_ENT  ,
  KC_NO,    KC_LGUI,  KC_LSFT,  KC_BSPC,  RESET,    KC_NO,    KC_SPC,   TO(_QW),  KC_0,       KC_DOT ,
                                          KC_LCTL,  KC_LALT         )
};

const uint16_t PROGMEM fn_actions[] = {

};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
  // MACRODOWN only works in this function
      switch(id) {
        case 0:
          if (record->event.pressed) {
            register_code(KC_RSFT);
          } else {
            unregister_code(KC_RSFT);
          }
        break;
      }
    return MACRO_NONE;
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	if ((record->event.pressed) && (idle_mode == 1)) {
		idle_mode = 0;
		timer_loop = 0;
		lcd_clrscr();
		timer1_stop();
	}

    switch (keycode) {
        case MO(_RS):
		case TO(_QW):
			if (record->event.pressed) {
			if (i == 2)
				i = 0;
			else
				i++;
			}
			else {
			if ((i != 2) && (i != 0))
				i--;
			}
			break;
		case TG(_LW):
			if (record->event.pressed) {
				i++;
			}
			break;
		case KC_NO:
			if (record->event.pressed) {
				if (lcd_off) {
					lcd_init(LCD_DISP_ON);
				lcd_clrscr();
				lcd_puts("LCD ON");
					lcd_off = false;
				}
				else {
				lcd_clrscr();
				lcd_sleep(YES);
				lcd_off = true;
				}
			}
			return true;
		case KC_F13:
			if (record->event.pressed) {
				if (idle_mode == 0) {
					idle_mode = 1;
				}
			}
			break;
		default:
			return true;
    }
    if (!lcd_off){	
		if (idle_mode == 1) {
			lcd_clrscr();
			lcd_puts("Fishing...");
			lcd_gotoxy(0,2);
			lcd_puts_p(PSTR("     ><(((*>"));
			lcd_gotoxy(0,4);
			lcd_puts_p(PSTR("          <*)))><"));
			return true;
		}
    	switch(i){
			case 0:
				lcd_clrscr();
				lcd_puts("Layer 1");
				lcd_gotoxy(0,2);
				lcd_puts_p(PSTR("QWERTY"));
				lcd_gotoxy(0,4);
				lcd_puts_p(PSTR("(> v <)"));
				break;
			case 1:
				lcd_clrscr();
				lcd_puts("Layer 2");
				lcd_gotoxy(0,2);
				lcd_puts_p(PSTR("Functions & Symbols"));
				lcd_gotoxy(0,4);
				lcd_puts_p(PSTR("(O_o)?"));
				break;
			case 2:
				lcd_clrscr();
				lcd_puts("Layer 3");
				lcd_gotoxy(0,2);
				lcd_puts_p(PSTR("Number Pad"));
				lcd_gotoxy(0,4);
				lcd_puts_p(PSTR("(-_-;)"));
				break;
			default:
				break;
        }
    }
    return true;
}

void press_random(void) {
	uint8_t key = rand() % 5;
	uint8_t key_list[5] = {KC_F13, KC_F14, KC_F15, KC_F16, KC_F17};
	register_code(key_list[key]);
	unregister_code(key_list[key]);
}

void matrix_init_user(void){
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts("Irene's keyboard");
    lcd_gotoxy(0,2);
    lcd_puts_p(PSTR("(~^o^)~"));
    #ifdef GRAPHICMODE
    lcd_drawCircle(64,32,7,WHITE);
    lcd_display();
    #endif
   
    timer1_init();
    return;
}

void matrix_scan_user(void){
	if (idle_mode == 1) {
		if ((TIFR1 & (1<<OCF1A)) != 0) {
			if (timer_loop >= 60) {
				timer_loop = 0;
				PORTB ^= (1 << PINB0);
				press_random();
			}
			else{
				timer_loop += 1;
			}
			TCNT1 = 0;
			TIFR1 |= (1<<OCF1A); // Clear overflow flag
		}
	}
	else {
		timer1_stop();
		timer_loop = 0;
	}

   	return;
}
