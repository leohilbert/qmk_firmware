# nardkey

Default keymap plus German umlauts without host-visible modifiers:

Chord: **left Ctrl + left Alt** or **right Alt** (Ctrl optional) plus

| Key | Sends | With [NardKey](https://github.com/leohilbert/nardkey) layout |
|---|---|---|
| A | `KC_NUBS` | ä (Shift: Ä) |
| O | `KC_INT1` | ö (Shift: Ö) |
| U | `KC_INT3` | ü (Shift: Ü) |
| S | `KC_KP_COMMA` | ß (Shift: ẞ) |

Implemented with QMK [key overrides](https://docs.qmk.fm/features/key_overrides):
Ctrl/Alt are released before the replacement key is sent, so apps never see
Ctrl+Alt+A. A dummy key tap (`DUMMY_MOD_NEUTRALIZER_KEYCODE`) keeps the
released Alt from opening app menu bars. Left Alt alone stays a normal Alt.

Without the NardKey layout installed these keys produce whatever the active
layout maps them to (e.g. `\|` for `KC_NUBS` on US).

    qmk compile -kb epomaker/galaxy100 -km nardkey
