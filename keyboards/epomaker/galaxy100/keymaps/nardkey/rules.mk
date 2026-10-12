ENCODER_MAP_ENABLE = yes
VIA_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes

# VIA marks its EEPROM data valid via a magic derived from the build date, so every
# build from a new day wipes the VIA keymap. --skip-all pins the date to 1970-01-01.
# ponytail: relies on build_keyboard.mk internals (SKIP_VERSION is read before keymap
# rules.mk, VERSION_H_FLAGS only after). Changes to keymaps[] then need a VIA reset.
VERSION_H_FLAGS += --skip-all
