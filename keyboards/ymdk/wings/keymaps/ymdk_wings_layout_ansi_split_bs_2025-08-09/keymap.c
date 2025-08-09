#include QMK_KEYBOARD_H
#if __has_include("keymap.h")
#    include "keymap.h"
#endif

// State for the mode:
static bool sponge_on = false;
static bool sponge_upper_next = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Toggle our mock mode
    if (keycode == SPONGE && record->event.pressed) {
        sponge_on = !sponge_on;
        sponge_upper_next = false;
        // ensure we don't collide with Caps Word if it was left on
        #ifdef CAPS_WORD_ENABLE
        if (sponge_on && is_caps_word_on()) { caps_word_off(); }
        #endif
        return false; // don't send a normal key
    }

    // While mock mode is on, alternate case for letters; end at word break
    if (sponge_on && record->event.pressed) {
        switch (keycode) {
            // Letters: alternate case by adding Shift on every other press
            case KC_A ... KC_Z:
                if (sponge_upper_next) { add_weak_mods(MOD_BIT(KC_LSFT)); }
                sponge_upper_next = !sponge_upper_next;
                return true; // let the letter go through (with shift if applied)

            // Keys that may appear inside a "word" but shouldn't end the mode
            case KC_1 ... KC_0:   // digits
            case KC_UNDS:         // underscore
            case KC_MINS:         // hyphen
            case KC_BSPC:
            case KC_DEL:
                return true;

            default:
                // Any other key (space, enter, punctuation, etc.) ends the one-word mode
                sponge_on = false;
                sponge_upper_next = false;
                return true;
        }
    }

    return true; // default processing
}
