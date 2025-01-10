#include "niubkb.h"
#include "timer1.h"

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QW 0
#define _RS 1
#define _LW 2

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QW] = KEYMAP( /* Colemak */
  KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,                                               KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN ,
  KC_A,    KC_R,    KC_S,    KC_T,    KC_D,                                               KC_H,    KC_N,    KC_E,    KC_I,    KC_O    ,
  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   LGUI(KC_S),           KC_DEL,               KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH ,
  KC_ESC, KC_TAB, KC_LGUI,  KC_LSFT, KC_BSPC, MT(MOD_LCTL, KC_TAB), MT(MOD_LALT, KC_ENT), KC_SPC,  MO(_RS), KC_MINS, KC_QUOT, KC_ENT  ),
/*
 *  !       @     up     {    }        ||     pgup    7     8     9    *
 *  #     left   down  right  $        ||     pgdn    4     5     6    +
 *  [       ]      (     )    &        ||       `     1     2     3    \
 * lower  insert super shift bksp ctrl || alt space   fn    .     0    =
 */
[_RS] = KEYMAP( /* [> RAISE <] */
  KC_EXLM, KC_AT,   KC_UP,   KC_LCBR, KC_RCBR,                   KC_PGUP, KC_7,    KC_8,   KC_9, KC_ASTR ,
  KC_HASH, KC_LEFT, KC_DOWN, KC_RGHT, KC_DLR,                    KC_PGDN, KC_4,    KC_5,   KC_6, KC_PLUS ,
  KC_LBRC, KC_RBRC, KC_LPRN, KC_RPRN, KC_AMPR, KC_CAPS, KC_DEL,  KC_GRV,  KC_1,    KC_2,   KC_3, KC_BSLS ,
  TG(_LW), KC_INS,  KC_LGUI, KC_LSFT, KC_BSPC, KC_LCTL, KC_LALT, KC_SPC,  KC_TRNS, KC_DOT, KC_0, KC_EQL  ),
/*
 * insert home   up  end   pgup       ||      up     F7    F8    F9   F10
 *  del   left  down right pgdn       ||     down    F4    F5    F6   F11
 *       volup             reset      ||             F1    F2    F3   F12
 *       voldn  super shift bksp ctrl || alt space   L0  prtsc scroll pause
 */
[_LW] = KEYMAP( /* [> LOWER <] */
  KC_INS,  KC_HOME, KC_UP,   KC_END,  KC_PGUP,                   KC_UP,   KC_F7,   KC_F8,   KC_F9,   KC_F10  ,
  KC_DELT, KC_LEFT, KC_DOWN, KC_RGHT, KC_DOWN,                   KC_DOWN, KC_F4,   KC_F5,   KC_F6,   KC_F11  ,
  KC_NO,   KC_VOLU, KC_NO,   KC_NO,   RESET,   KC_CAPS, KC_DEL,  KC_NO,   KC_F1,   KC_F2,   KC_F3,   KC_F12  ,
  KC_NO,   KC_VOLD, KC_LGUI, KC_LSFT, KC_BSPC, KC_LCTL, KC_LALT, KC_SPC,  TO(_QW), KC_PSCR, KC_SLCK, KC_PAUS )
};

int idle_mode = 0;
int timer_loop = 0;

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
    // Exit idle mode and stop timer once it detects key press
    if ((record->event.pressed) && (idle_mode == 1))
    {
        idle_mode = 0;
        timer_loop = 0;
        timer1_stop();
        PORTB &= ~(1 << PINB0); // disable LED
    }

    switch (keycode) {
    case KC_F13: // toggle idle mode
        if (record->event.pressed) {
            idle_mode = 1;
            PORTB |= (1 << PINB0); // enable LED
        }
        break;

    default:
        return true;
    }

    return true;
}

void send_random_benign_keystroke(void)
{
    uint8_t key = rand() % 5;
    uint8_t key_list[5] = {KC_F13, KC_F14, KC_F15, KC_F16, KC_F17};
    register_code(key_list[key]);
    unregister_code(key_list[key]);
}

void matrix_init_user(void)
{
    timer1_init();
    return;
}

void matrix_scan_user(void)
{
    if (idle_mode == 1)
    {
        // Timer overflow when Output Compare flag (OCF1A) of Timer1
        // Interrupt Flag Register (TIFR1) is set
        if ((TIFR1 & (1 << OCF1A)) != 0)
        {
            // Send benign keystroke after 60 instances of time up (60 seconds)
            if (timer_loop >= 60)
            {
                timer_loop = 0;
                send_random_benign_keystroke();
            }
            else
            {
                timer_loop += 1;
            }
            // Reset Timer
            TCNT1 = 0;             // Reset timer register
            TIFR1 |= (1 << OCF1A); // Clear overflow flag
        }
    }
    else
    {
        timer1_stop();
        timer_loop = 0;
    }

    return;
}
