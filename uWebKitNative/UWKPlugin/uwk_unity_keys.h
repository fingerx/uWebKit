/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <stdlib.h>

struct UWKMessage;

// Marshalled from C#
struct UnityKeyEvent
{
    uint32_t Type; // 1 for down 0 for up
    uint32_t Modifiers;
    uint32_t KeyCode;
    uint32_t Character;
};

bool EncodeUnityKeyEvent(UnityKeyEvent* keyEvent, UWKMessage& msg);

bool UnityKeyHasCharacter(uint32_t unityKeyCode);

enum UnityKeyCode
{
    UnityKeyCode_None,
    UnityKeyCode_Backspace = 8,
    UnityKeyCode_Delete = 127,
    UnityKeyCode_Tab = 9,
    UnityKeyCode_Clear = 12,
    UnityKeyCode_Return,
    UnityKeyCode_Pause = 19,
    UnityKeyCode_Escape = 27,
    UnityKeyCode_Space = 32,
    UnityKeyCode_Keypad0 = 256,
    UnityKeyCode_Keypad1,
    UnityKeyCode_Keypad2,
    UnityKeyCode_Keypad3,
    UnityKeyCode_Keypad4,
    UnityKeyCode_Keypad5,
    UnityKeyCode_Keypad6,
    UnityKeyCode_Keypad7,
    UnityKeyCode_Keypad8,
    UnityKeyCode_Keypad9,
    UnityKeyCode_KeypadPeriod,
    UnityKeyCode_KeypadDivide,
    UnityKeyCode_KeypadMultiply,
    UnityKeyCode_KeypadMinus,
    UnityKeyCode_KeypadPlus,
    UnityKeyCode_KeypadEnter,
    UnityKeyCode_KeypadEquals,
    UnityKeyCode_UpArrow,
    UnityKeyCode_DownArrow,
    UnityKeyCode_RightArrow,
    UnityKeyCode_LeftArrow,
    UnityKeyCode_Insert,
    UnityKeyCode_Home,
    UnityKeyCode_End,
    UnityKeyCode_PageUp,
    UnityKeyCode_PageDown,
    UnityKeyCode_F1,
    UnityKeyCode_F2,
    UnityKeyCode_F3,
    UnityKeyCode_F4,
    UnityKeyCode_F5,
    UnityKeyCode_F6,
    UnityKeyCode_F7,
    UnityKeyCode_F8,
    UnityKeyCode_F9,
    UnityKeyCode_F10,
    UnityKeyCode_F11,
    UnityKeyCode_F12,
    UnityKeyCode_F13,
    UnityKeyCode_F14,
    UnityKeyCode_F15,
    UnityKeyCode_Alpha0 = 48,
    UnityKeyCode_Alpha1,
    UnityKeyCode_Alpha2,
    UnityKeyCode_Alpha3,
    UnityKeyCode_Alpha4,
    UnityKeyCode_Alpha5,
    UnityKeyCode_Alpha6,
    UnityKeyCode_Alpha7,
    UnityKeyCode_Alpha8,
    UnityKeyCode_Alpha9,
    UnityKeyCode_Exclaim = 33,
    UnityKeyCode_DoubleQuote,
    UnityKeyCode_Hash,
    UnityKeyCode_Dollar,
    UnityKeyCode_Ampersand = 38,
    UnityKeyCode_Quote,
    UnityKeyCode_LeftParen,
    UnityKeyCode_RightParen,
    UnityKeyCode_Asterisk,
    UnityKeyCode_Plus,
    UnityKeyCode_Comma,
    UnityKeyCode_Minus,
    UnityKeyCode_Period,
    UnityKeyCode_Slash,
    UnityKeyCode_Colon = 58,
    UnityKeyCode_Semicolon,
    UnityKeyCode_Less,
    UnityKeyCode_Equals,
    UnityKeyCode_Greater,
    UnityKeyCode_Question,
    UnityKeyCode_At,
    UnityKeyCode_LeftBracket = 91,
    UnityKeyCode_Backslash,
    UnityKeyCode_RightBracket,
    UnityKeyCode_Caret,
    UnityKeyCode_Underscore,
    UnityKeyCode_BackQuote,
    UnityKeyCode_A,
    UnityKeyCode_B,
    UnityKeyCode_C,
    UnityKeyCode_D,
    UnityKeyCode_E,
    UnityKeyCode_F,
    UnityKeyCode_G,
    UnityKeyCode_H,
    UnityKeyCode_I,
    UnityKeyCode_J,
    UnityKeyCode_K,
    UnityKeyCode_L,
    UnityKeyCode_M,
    UnityKeyCode_N,
    UnityKeyCode_O,
    UnityKeyCode_P,
    UnityKeyCode_Q,
    UnityKeyCode_R,
    UnityKeyCode_S,
    UnityKeyCode_T,
    UnityKeyCode_U,
    UnityKeyCode_V,
    UnityKeyCode_W,
    UnityKeyCode_X,
    UnityKeyCode_Y,
    UnityKeyCode_Z,
    UnityKeyCode_Numlock = 300,
    UnityKeyCode_CapsLock,
    UnityKeyCode_ScrollLock,
    UnityKeyCode_RightShift,
    UnityKeyCode_LeftShift,
    UnityKeyCode_RightControl,
    UnityKeyCode_LeftControl,
    UnityKeyCode_RightAlt,
    UnityKeyCode_LeftAlt,
    UnityKeyCode_LeftCommand = 310,
    UnityKeyCode_LeftApple = 310,
    UnityKeyCode_LeftWindows,
    UnityKeyCode_RightCommand = 309,
    UnityKeyCode_RightApple = 309,
    UnityKeyCode_RightWindows = 312,
    UnityKeyCode_AltGr,
    UnityKeyCode_Help = 315,
    UnityKeyCode_Print,
    UnityKeyCode_SysReq,
    UnityKeyCode_Break,
    UnityKeyCode_Menu
};

// these must be the same as the C# representation
enum UnityKeyModifiers
{
    UnityKeyModifier_Shift =      0x1,
    UnityKeyModifier_Control =    0x2,
    UnityKeyModifier_Alt =        0x4,
    UnityKeyModifier_CommandWin = 0x8, // windows or command key
    UnityKeyModifier_Numeric =    0x10,
    UnityKeyModifier_CapsLock =   0x20,
    UnityKeyModifier_FunctionKey = 0x40
};




