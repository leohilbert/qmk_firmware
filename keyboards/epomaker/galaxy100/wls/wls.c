#include "wls.h"

static ioline_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
// static ioline_t row_pins[MATRIX_COLS] = MATRIX_ROW_PINS;

// Light sleep for wireless modes:
// - Turns off RGB/indicators after timeout
// - Keeps MCU running (no deep STOP), so any keypress can "wake" immediately
static bool hs_wls_idle_sleep = false;
#ifdef RGB_MATRIX_ENABLE
static bool hs_wls_idle_rgb_enabled = false;
static void hs_wls_idle_sleep_enter(void) {
    // Never enter "wireless idle sleep" while in USB mode
    if (wireless_get_current_devs() == DEVS_USB) {
        return;
    }
    if (hs_wls_idle_sleep) {
        return;
    }
    hs_wls_idle_sleep = true;
    hs_wls_idle_rgb_enabled = rgb_matrix_is_enabled();
    if (hs_wls_idle_rgb_enabled) {
        // Turn LEDs fully off, but keep RGB enabled so it can resume immediately.
        rgb_matrix_set_suspend_state(true);
    }
}
static void hs_wls_idle_sleep_exit(void) {
    // If we switched to USB mode, ensure LEDs are not stuck suspended
    // (this can happen if we entered idle sleep in wireless mode).
    if (wireless_get_current_devs() == DEVS_USB) {
        hs_wls_idle_sleep = false;
    }
    if (!hs_wls_idle_sleep) {
        return;
    }
    hs_wls_idle_sleep = false;
    if (hs_wls_idle_rgb_enabled) {
        rgb_matrix_set_suspend_state(false);
        // Force a restart of the RGB task so LEDs come back immediately
        rgb_matrix_mode_noeeprom(rgb_matrix_get_mode());
    }
    hs_rgb_blink_set_timer(timer_read32());
}
#else
static void hs_wls_idle_sleep_enter(void) { hs_wls_idle_sleep = true; }
static void hs_wls_idle_sleep_exit(void)  { hs_wls_idle_sleep = false; hs_rgb_blink_set_timer(timer_read32()); }
#endif

void hs_wls_user_activity(void) {
    // Only meaningful in wireless modes
    if (wireless_get_current_devs() == DEVS_USB) {
        return;
    }

    // If we're in keyboard-side idle sleep, wake visuals first
    hs_wls_idle_sleep_exit();

    // Reassert current device selection to the wireless module so it starts scanning/reconnecting.
    uint8_t dev = wireless_get_current_devs();
    wireless_devs_change(dev, dev, false);
}

bool hs_modeio_detection(bool update, uint8_t *mode, uint8_t lsat_btdev) {
    static uint32_t scan_timer = 0x00;

    if ((update != true) && (timer_elapsed32(scan_timer) <= (HS_MODEIO_DETECTION_TIME))) {
        return false;
    }
    scan_timer = timer_read32();
#if defined(HS_BT_DEF_PIN) && defined(HS_2G4_DEF_PIN)
    uint8_t now_mode         = 0x00;
    uint8_t hs_mode          = 0x00;
    static uint8_t last_mode = 0x00;
    bool sw_mode             = false;
    now_mode                 = (HS_GET_MODE_PIN(HS_USB_PIN_STATE) ? 3 : (HS_GET_MODE_PIN(HS_BT_PIN_STATE) ? 1 : ((HS_GET_MODE_PIN(HS_2G4_PIN_STATE) ? 2 : 0))));
    hs_mode                  = (*mode >= DEVS_BT1 && *mode <= DEVS_BT5) ? 1 : ((*mode == DEVS_2G4) ? 2 : ((*mode == DEVS_USB) ? 3 : 0));
    sw_mode                  = ((update || (last_mode == now_mode)) && (hs_mode != now_mode)) ? true : false;
    last_mode                = now_mode;

    switch (now_mode) {
        case 1:
            *mode = hs_bt;
            if (sw_mode) {
                wireless_devs_change(wireless_get_current_devs(), lsat_btdev, false);
                md_send_devctrl(0x80+30);
            }
            break;
        case 2:
            *mode = hs_2g4;
            if (sw_mode) {
                wireless_devs_change(wireless_get_current_devs(), DEVS_2G4, false);
                md_send_devctrl(0xFF);
            }
            break;
        case 3:
            *mode = hs_usb;
            if (sw_mode)
                wireless_devs_change(wireless_get_current_devs(), DEVS_USB, false);

            break;
        default:
            break;
    }

    if (sw_mode) {
        hs_rgb_blink_set_timer(timer_read32());
        hs_wls_idle_sleep_exit();
        suspend_wakeup_init();
        return true;
    }
#else
    *mode = hs_none;
#endif

    return false;
}

static uint32_t hs_linker_rgb_timer = 0x00;

bool hs_mode_scan(bool update, uint8_t moude, uint8_t lsat_btdev) {

    if (hs_modeio_detection(update, &moude, lsat_btdev)) {

        return true;
    }
    hs_rgb_blink_hook();
    return false;
}

void hs_rgb_blink_set_timer(uint32_t time) {
    hs_linker_rgb_timer = time;
}

uint32_t hs_rgb_blink_get_timer(void) {
    return hs_linker_rgb_timer;
}

bool hs_rgb_blink_hook() {
    static uint8_t last_status;
    uint32_t timeout = HS_SLEEP_TIMEOUT;
  
    // USB mode: never keep LEDs in wireless idle sleep
    if (wireless_get_current_devs() == DEVS_USB && hs_wls_idle_sleep) {
        hs_wls_idle_sleep_exit();
    }

    if (last_status != *md_getp_state()) {
        last_status = *md_getp_state();
        hs_rgb_blink_set_timer(0x00);
        // If we reconnect, exit idle sleep so indicators can run again
        if (*md_getp_state() == MD_STATE_CONNECTED) {
            hs_wls_idle_sleep_exit();
        }
    }

    // While idling in wireless sleep, keep everything off until user interaction or reconnect
    if (hs_wls_idle_sleep) {
        if (*md_getp_state() == MD_STATE_CONNECTED) {
            hs_wls_idle_sleep_exit();
        }
        return true;
    }

    switch (*md_getp_state()) {
        case MD_STATE_NONE: {
            hs_rgb_blink_set_timer(0x00);
        } break;

        case MD_STATE_DISCONNECTED:{
            if (hs_rgb_blink_get_timer() == 0x00) {
                hs_rgb_blink_set_timer(timer_read32());
                extern void wireless_devs_change_kb(uint8_t old_devs, uint8_t new_devs, bool reset);
                wireless_devs_change_kb(wireless_get_current_devs(), wireless_get_current_devs(), false);
            } else {
                if ((timer_elapsed32(hs_rgb_blink_get_timer()) >= HS_LBACK_TIMEOUT) && !rgbrec_is_started()) {
                    hs_rgb_blink_set_timer(timer_read32());
                    // Enter light idle sleep instead of deep STOP (keeps keypress wake working)
                    hs_wls_idle_sleep_enter();
                }
            }
        } break;
        case MD_STATE_CONNECTED:{
            if (hs_rgb_blink_get_timer() == 0x00) {
                hs_rgb_blink_set_timer(timer_read32());
                // extern bool hs_wlr_succeed;
                // extern uint32_t hs_usb_succeed_time;
                // hs_wlr_succeed = true;
                // hs_usb_succeed_time = timer_read32();
            } else {
                if (timer_elapsed32(hs_rgb_blink_get_timer()) >= timeout && !rgbrec_is_started()) {
                    hs_rgb_blink_set_timer(timer_read32());
                    clear_keyboard();
                    // Enter light idle sleep instead of deep STOP (keeps keypress wake working)
                    hs_wls_idle_sleep_enter();
                }
            }
        } break;

        case MD_STATE_PAIRING:{
            if (hs_rgb_blink_get_timer() == 0x00) {
                hs_rgb_blink_set_timer(timer_read32());
    
            } else {
                if (timer_elapsed32(hs_rgb_blink_get_timer()) >= HS_PAIR_TIMEOUT && !rgbrec_is_started()) {
                    hs_rgb_blink_set_timer(timer_read32());
                    // Enter light idle sleep instead of deep STOP (keeps keypress wake working)
                    hs_wls_idle_sleep_enter();
                }
            }
        } break;
        default:break;
    }
    return true;
}

void lpwr_exti_init_hook(void) {

#ifdef HS_BT_DEF_PIN
    setPinInputHigh(HS_BT_DEF_PIN);
    waitInputPinDelay();
    palEnableLineEvent(HS_BT_DEF_PIN, PAL_EVENT_MODE_BOTH_EDGES);
#endif

#ifdef HS_2G4_DEF_PIN
    setPinInputHigh(HS_2G4_DEF_PIN);
    waitInputPinDelay();
    palEnableLineEvent(HS_2G4_DEF_PIN, PAL_EVENT_MODE_BOTH_EDGES);
#endif

    if (lower_sleep) {
#if DIODE_DIRECTION == ROW2COL
        for (uint8_t i = 0; i < ARRAY_SIZE(col_pins); i++) {
            if (col_pins[i] != NO_PIN) {
                setPinOutput(col_pins[i]);
                writePinHigh(col_pins[i]);
            }
        }
#endif
    }

    setPinInput(HS_BAT_CABLE_PIN);
    waitInputPinDelay();
    palEnableLineEvent(HS_BAT_CABLE_PIN, PAL_EVENT_MODE_RISING_EDGE);
}

void palcallback_cb(uint8_t line) {
    switch (line) {
        case PAL_PAD(HS_BAT_CABLE_PIN): {
            lpwr_set_sleep_wakeupcd(LPWR_WAKEUP_CABLE);
        } break;
#ifdef HS_BT_DEF_PIN
        case PAL_PAD(HS_2G4_DEF_PIN): {
            lpwr_set_sleep_wakeupcd(LPWR_WAKEUP_SWITCH);
        } break;
#endif

#ifdef HS_2G4_DEF_PIN
        case PAL_PAD(HS_BT_DEF_PIN): {
            lpwr_set_sleep_wakeupcd(LPWR_WAKEUP_SWITCH);
        } break;
#endif
        default: {

        } break;
    }
}

void lpwr_stop_hook_pre(void) {

    gpio_write_pin_low(LED_POWER_EN_PIN);
    gpio_write_pin_low(B9);
    gpio_write_pin_low(HS_LED_BOOSTING_PIN);
    if (lower_sleep) {
        md_send_devctrl(MD_SND_CMD_DEVCTRL_USB);
        wait_ms(200);
        lpwr_set_sleep_wakeupcd(LPWR_WAKEUP_UART);
    }
}

void lpwr_stop_hook_post(void) {
    if (lower_sleep) {
        switch (lpwr_get_sleep_wakeupcd()) {
            case LPWR_WAKEUP_USB:
            case LPWR_WAKEUP_CABLE: {
                lower_sleep = false;
                lpwr_set_state(LPWR_WAKEUP);
            } break;
            default: {
                lpwr_set_state(LPWR_STOP);
            } break;
        }
    }
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id = &(data[0]);


    hs_rgb_blink_set_timer(timer_read32());

    switch (*command_id) {
        case id_eeprom_reset: {
            hs_reset_settings();
        } break;
        default:{
            extern bool usb_wd_via_command(uint8_t *data);
            if (!usb_wd_via_command(data)) *command_id = id_unhandled;
        }break;
    }
}

bool via_command_kb(uint8_t *data, uint8_t length) {
    
    hs_rgb_blink_set_timer(timer_read32());
    
    // if (hs_via_custom_value_command_kb(data, length) != false){
    //     replaced_hid_send(data, length);
    //     return true;
    // }
    return false;
}
