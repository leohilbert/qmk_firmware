// Copyright 2024 Epomaker (@Epomaker)
// SPDX-License-Identifier: GPL-2.0-or-later

// Default keymap + German umlauts on A/O/U/S with either
//   left Ctrl + left Alt   or   right Alt (with or without Ctrl).
// The chord is replaced by a scancode the NardKey Windows layout maps to
// ä/ö/ü/ß (https://github.com/leohilbert/nardkey), so the host never sees
// Ctrl/Alt and app shortcuts can't swallow it. Shift passes through -> Ä/Ö/Ü/ẞ.
// Left Alt alone stays a normal Alt (Alt+A etc. still reach apps).

#include QMK_KEYBOARD_H

enum layers {
    _BL = 0,
    _FL,
    _MBL,
    _MFL,
    _FBL,
};

#define LEFT_CA (MOD_BIT(KC_LCTL) | MOD_BIT(KC_LALT))
#define RIGHT_A MOD_BIT(KC_RALT)

// Like ko_make_basic, but always strips every Ctrl/Alt, so Ctrl + right Alt works too.
#define UMLAUT(trigger_mods_, trigger_key, replacement_key) \
    ((const key_override_t){                                \
        .trigger_mods      = (trigger_mods_),               \
        .layers            = ~0,                            \
        .suppressed_mods   = MOD_MASK_CA,                   \
        .options           = ko_options_default,            \
        .negative_mod_mask = 0,                             \
        .custom_action     = NULL,                          \
        .context           = NULL,                          \
        .trigger           = (trigger_key),                 \
        .replacement       = (replacement_key),             \
        .enabled           = NULL                           \
    })

const key_override_t l_ae = UMLAUT(LEFT_CA, KC_A, KC_NUBS);     // SC 56 -> ä
const key_override_t l_oe = UMLAUT(LEFT_CA, KC_O, KC_INT1);     // SC 73 -> ö
const key_override_t l_ue = UMLAUT(LEFT_CA, KC_U, KC_INT3);     // SC 7D -> ü
const key_override_t l_ss = UMLAUT(LEFT_CA, KC_S, KC_KP_COMMA); // SC 7E -> ß
const key_override_t r_ae = UMLAUT(RIGHT_A, KC_A, KC_NUBS);
const key_override_t r_oe = UMLAUT(RIGHT_A, KC_O, KC_INT1);
const key_override_t r_ue = UMLAUT(RIGHT_A, KC_U, KC_INT3);
const key_override_t r_ss = UMLAUT(RIGHT_A, KC_S, KC_KP_COMMA);

const key_override_t *key_overrides[] = {
    &l_ae, &l_oe, &l_ue, &l_ss,
    &r_ae, &r_oe, &r_ue, &r_ss,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BL] = LAYOUT( /* Base */
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11 ,  KC_F12,   KC_DEL,    KC_HOME,   KC_END ,   KC_PSCR, KC_PAUSE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,    KC_NUM,    KC_PSLS,   KC_PAST, KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_PGUP,   KC_P7,     KC_P8,     KC_P9,   KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_ENT ,            KC_PGDN,   KC_P4,     KC_P5,     KC_P6,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,                         KC_P1,     KC_P2,     KC_P3,   KC_PENT,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(_FL),  KC_LEFT,  KC_DOWN,            KC_RGHT,              KC_P0,     KC_PDOT),

    [_FL] = LAYOUT( /* Base */
        KC_ESC,             KC_MYCM,  KC_WHOM,  KC_CALC,  KC_MAIL,  KC_MSEL,  KC_MSTP,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_DEL,    KC_HOME,   KC_END ,   KC_PSCR, KC_PAUSE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   EE_CLR,   KC_INS,    KC_NUM,    KC_PSLS,   KC_PAST, KC_PMNS,
        KC_TAB,   KC_BT1,   KC_BT2,   KC_BT3,   KC_2G4,   RM_TOGG,  KC_Y,     KC_U,     KC_I,     MO(_FBL), KC_P,     KC_LBRC,  KC_RBRC,  RM_HUEU,  KC_PGUP,   KC_P7,     KC_P8,     KC_P9,   KC_PPLS,
        KC_CAPS,  KC_NO,    TO(_MBL), KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     MO(_FBL), KC_SCLN,  KC_QUOT,  RM_NEXT,            KC_PGDN,   KC_P4,     KC_P5,     KC_P6,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  RM_VALU,                       KC_P1,     KC_P2,     KC_P3,   KC_PENT,
        KC_LCTL,  GU_TOGG,  KC_LALT,                      HS_BATQ,                                 KC_RALT,  _______,  RM_SPDD,  RM_VALD,            RM_SPDU,              KC_P0,     KC_PDOT),

    [_MBL] = LAYOUT( /* Base */
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11 ,  KC_F12,   KC_DEL,    KC_HOME,   KC_END ,   KC_PSCR, KC_PAUSE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,    KC_NUM,    KC_PSLS,   KC_PAST, KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_PGUP,   KC_P7,     KC_P8,     KC_P9,   KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_ENT ,            KC_PGDN,   KC_P4,     KC_P5,     KC_P6,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,                         KC_P1,     KC_P2,     KC_P3,   KC_PENT,
        KC_LCTL,  KC_LALT,  KC_LGUI,                      KC_SPC,                                 KC_RGUI,  MO(_MFL), KC_LEFT,  KC_DOWN,            KC_RGHT,              KC_P0,     KC_PDOT),

    [_MFL] = LAYOUT( /* Base */
        KC_ESC,             KC_BRID,  KC_BRIU,  USE_TASK, USE_PRO,  KC_F5,    KC_F6,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_DEL,    KC_HOME,   KC_END ,   KC_PSCR, KC_PAUSE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   EE_CLR,   KC_INS,    KC_NUM,    KC_PSLS,   KC_PAST, KC_PMNS,
        KC_TAB,   KC_BT1,   KC_BT2,   KC_BT3,   KC_2G4,   RM_TOGG,  KC_Y,     KC_U,     KC_I,     MO(_FBL), KC_P,     KC_LBRC,  KC_RBRC,  RM_HUEU,  KC_PGUP,   KC_P7,     KC_P8,     KC_P9,   KC_PPLS,
        KC_CAPS,  TO(_BL),  KC_NO,    KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     MO(_FBL), KC_SCLN,  KC_QUOT,  RM_NEXT,            KC_PGDN,   KC_P4,     KC_P5,     KC_P6,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  RM_VALU,                       KC_P1,     KC_P2,     KC_P3,   KC_PENT,
        KC_LCTL,  KC_LALT,  KC_LGUI,                      HS_BATQ,                                 KC_RGUI,  _______,  RM_SPDD,  RM_VALD,            RM_SPDU,              KC_P0,     KC_PDOT ),
    [_FBL] = LAYOUT( /* Base */
        QK_BOOT,             _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______, _______,  _______, _______,
        _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______, _______,  _______, _______,
        _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______, _______,  _______, _______,
        _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,             _______,  _______, _______,  _______,
        _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                       _______, _______,  _______, _______,
        _______,  _______,   _______,                      _______,                                _______,  _______,  _______,  _______,             _______,           _______,  _______)
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [1] = {ENCODER_CCW_CW(_______, _______)},
    [2] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [3] = {ENCODER_CCW_CW(_______, _______)},
    [4] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
};
#endif
// clang-format on
