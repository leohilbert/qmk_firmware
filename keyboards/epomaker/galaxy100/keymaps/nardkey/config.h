// Copyright 2026 Leo Hilbert
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// keyboard.json ships debounce 1 ms -> chattering switches register double
// presses. 10 ms fixed it for others in Epomaker/galaxy100#2.
#undef DEBOUNCE
#define DEBOUNCE 10

// LEDs off after 10 min without input (PC on but nobody at it); next key turns them on
#define RGB_MATRIX_TIMEOUT 600000

// Key overrides release Ctrl/Alt before sending the umlaut scancode. A bare
// Alt press+release would open the menu bar in Windows apps, so tap a dummy
// key first. Exact-match list: every umlaut chord, with and without Shift.
#define DUMMY_MOD_NEUTRALIZER_KEYCODE KC_RIGHT_CTRL

#define NARD_L  (MOD_BIT(KC_LCTL) | MOD_BIT(KC_LALT))
#define NARD_R  MOD_BIT(KC_RALT)
#define NARD_RL (MOD_BIT(KC_RALT) | MOD_BIT(KC_LCTL))
#define NARD_RR (MOD_BIT(KC_RALT) | MOD_BIT(KC_RCTL))
#define NARD_WITH_SHIFTS(m) (m), (m) | MOD_BIT(KC_LSFT), (m) | MOD_BIT(KC_RSFT)

#define MODS_TO_NEUTRALIZE { \
    MOD_BIT(KC_LEFT_ALT), MOD_BIT(KC_LEFT_GUI), \
    NARD_WITH_SHIFTS(NARD_L), NARD_WITH_SHIFTS(NARD_R), \
    NARD_WITH_SHIFTS(NARD_RL), NARD_WITH_SHIFTS(NARD_RR) \
}
