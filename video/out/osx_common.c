/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "config.h"

// only to get keycode definitions from HIToolbox/Events.h
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include "osx_common.h"
#include "video/out/vo.h"
#include "core/input/keycodes.h"
#include "core/input/input.h"
#include "core/mp_msg.h"

/*
 * Define keycodes only found in OSX >= 10.5 for older versions
 */
#if MAC_OS_X_VERSION_MAX_ALLOWED <= 1040
#define kVK_ANSI_Keypad0 0x52
#define kVK_ANSI_Keypad1 0x53
#define kVK_ANSI_Keypad2 0x54
#define kVK_ANSI_Keypad3 0x55
#define kVK_ANSI_Keypad4 0x56
#define kVK_ANSI_Keypad5 0x57
#define kVK_ANSI_Keypad6 0x58
#define kVK_ANSI_Keypad7 0x59
#define kVK_ANSI_Keypad8 0x5b
#define kVK_ANSI_Keypad9 0x5c
#define kVK_ANSI_KeypadDecimal 0x41
#define kVK_ANSI_KeypadDivide 0x4b
#define kVK_ANSI_KeypadEnter 0x4c
#define kVK_ANSI_KeypadMinus 0x4e
#define kVK_ANSI_KeypadMultiply 0x43
#define kVK_ANSI_KeypadPlus 0x45
#define kVK_Control 0x3b
#define kVK_Delete 0x33
#define kVK_DownArrow 0x7d
#define kVK_End 0x77
#define kVK_Escape 0x35
#define kVK_F1 0x7a
#define kVK_F10 0x6d
#define kVK_F11 0x67
#define kVK_F12 0x6f
#define kVK_F2 0x78
#define kVK_F3 0x63
#define kVK_F4 0x76
#define kVK_F5 0x60
#define kVK_F6 0x61
#define kVK_F7 0x62
#define kVK_F8 0x64
#define kVK_F9 0x65
#define kVK_ForwardDelete 0x75
#define kVK_Help 0x72
#define kVK_Home 0x73
#define kVK_LeftArrow 0x7b
#define kVK_Option 0x3a
#define kVK_PageDown 0x79
#define kVK_PageUp 0x74
#define kVK_Return 0x24
#define kVK_RightArrow 0x7c
#define kVK_Shift 0x38
#define kVK_Tab 0x30
#define kVK_UpArrow 0x7e
#endif /* MAC_OS_X_VERSION_MAX_ALLOWED <= 1040 */

static const struct mp_keymap keymap[] = {
    // special keys
    {0x34, MP_KEY_ENTER}, // Enter key on some iBooks?
    {kVK_Return, MP_KEY_ENTER},
    {kVK_Escape, MP_KEY_ESC},
    {kVK_Delete, MP_KEY_BACKSPACE}, {kVK_Option, MP_KEY_BACKSPACE}, {kVK_Control, MP_KEY_BACKSPACE}, {kVK_Shift, MP_KEY_BACKSPACE},
    {kVK_Tab, MP_KEY_TAB},

    // cursor keys
    {kVK_UpArrow, MP_KEY_UP}, {kVK_DownArrow, MP_KEY_DOWN}, {kVK_LeftArrow, MP_KEY_LEFT}, {kVK_RightArrow, MP_KEY_RIGHT},

    // navigation block
    {kVK_Help, MP_KEY_INSERT}, {kVK_ForwardDelete, MP_KEY_DELETE}, {kVK_Home, MP_KEY_HOME},
    {kVK_End, MP_KEY_END}, {kVK_PageUp, MP_KEY_PAGE_UP}, {kVK_PageDown, MP_KEY_PAGE_DOWN},

    // F-keys
    {kVK_F1, MP_KEY_F + 1}, {kVK_F2, MP_KEY_F + 2}, {kVK_F3, MP_KEY_F + 3}, {kVK_F4, MP_KEY_F + 4},
    {kVK_F5, MP_KEY_F + 5}, {kVK_F6, MP_KEY_F + 6}, {kVK_F7, MP_KEY_F + 7}, {kVK_F8, MP_KEY_F + 8},
    {kVK_F9, MP_KEY_F + 9}, {kVK_F10, MP_KEY_F + 10}, {kVK_F11, MP_KEY_F + 11}, {kVK_F12, MP_KEY_F + 12},

    // numpad
    {kVK_ANSI_KeypadPlus, '+'}, {kVK_ANSI_KeypadMinus, '-'}, {kVK_ANSI_KeypadMultiply, '*'},
    {kVK_ANSI_KeypadDivide, '/'}, {kVK_ANSI_KeypadEnter, MP_KEY_KPENTER}, {kVK_ANSI_KeypadDecimal, MP_KEY_KPDEC},
    {kVK_ANSI_Keypad0, MP_KEY_KP0}, {kVK_ANSI_Keypad1, MP_KEY_KP1}, {kVK_ANSI_Keypad2, MP_KEY_KP2}, {kVK_ANSI_Keypad3, MP_KEY_KP3},
    {kVK_ANSI_Keypad4, MP_KEY_KP4}, {kVK_ANSI_Keypad5, MP_KEY_KP5}, {kVK_ANSI_Keypad6, MP_KEY_KP6}, {kVK_ANSI_Keypad7, MP_KEY_KP7},
    {kVK_ANSI_Keypad8, MP_KEY_KP8}, {kVK_ANSI_Keypad9, MP_KEY_KP9},

    {0, 0}
};

int convert_key(unsigned key, unsigned charcode)
{
    int mpkey = lookup_keymap_table(keymap, key);
    if (mpkey)
        return mpkey;
    return charcode;
}