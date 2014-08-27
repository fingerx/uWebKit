/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#include <map>
#include "UWKCommon/uwk_keyboard.h"
#include "UWKCommon/uwk_message.h"
#include "uwk_unity_keys.h"

// ifdef mac

static uint32_t TranslateUnityKeyCode(uint32_t uKeyCode)
{
    switch (uKeyCode)
    {

    case UnityKeyCode_None:
        return UWKInvalidKeyCode;

    case UnityKeyCode_Backspace:
        return UWKKeyboard::Key_Backspace;

    case UnityKeyCode_Delete:
        return UWKKeyboard::Key_Delete;

    case UnityKeyCode_Tab:
        return UWKKeyboard::Key_Tab;

    case UnityKeyCode_Clear:
        return UWKKeyboard::Key_Clear;

    case UnityKeyCode_Return:
        return UWKKeyboard::Key_Return;

    case UnityKeyCode_Pause:
        return UWKInvalidKeyCode;

    case UnityKeyCode_Escape:
        return UWKKeyboard::Key_Escape;

    case UnityKeyCode_Space:
        return UWKKeyboard::Key_Space;

    case UnityKeyCode_Keypad0:
    case UnityKeyCode_Keypad1:
    case UnityKeyCode_Keypad2:
    case UnityKeyCode_Keypad3:
    case UnityKeyCode_Keypad4:
    case UnityKeyCode_Keypad5:
    case UnityKeyCode_Keypad6:
    case UnityKeyCode_Keypad7:
    case UnityKeyCode_Keypad8:
    case UnityKeyCode_Keypad9:
    case UnityKeyCode_KeypadPeriod:
    case UnityKeyCode_KeypadDivide:
    case UnityKeyCode_KeypadMultiply:
    case UnityKeyCode_KeypadMinus:
    case UnityKeyCode_KeypadPlus:
    case UnityKeyCode_KeypadEnter:
    case UnityKeyCode_KeypadEquals:
        break;

    case UnityKeyCode_UpArrow:
        return UWKKeyboard::Key_Up;
    case UnityKeyCode_DownArrow:
        return UWKKeyboard::Key_Down;
    case UnityKeyCode_RightArrow:
        return UWKKeyboard::Key_Right;
    case UnityKeyCode_LeftArrow:
        return UWKKeyboard::Key_Left;

    case UnityKeyCode_Insert:
    case UnityKeyCode_Home:
    case UnityKeyCode_End:
    case UnityKeyCode_PageUp:
    case UnityKeyCode_PageDown:
        break;

    case UnityKeyCode_F1:
        return UWKKeyboard::Key_F1;
    case UnityKeyCode_F2:
        return UWKKeyboard::Key_F2;
    case UnityKeyCode_F3:
        return UWKKeyboard::Key_F3;
    case UnityKeyCode_F4:
        return UWKKeyboard::Key_F4;
    case UnityKeyCode_F5:
        return UWKKeyboard::Key_F5;
    case UnityKeyCode_F6:
        return UWKKeyboard::Key_F6;
    case UnityKeyCode_F7:
        return UWKKeyboard::Key_F7;
    case UnityKeyCode_F8:
        return UWKKeyboard::Key_F8;
    case UnityKeyCode_F9:
        return UWKKeyboard::Key_F9;
    case UnityKeyCode_F10:
        return UWKKeyboard::Key_F10;
    case UnityKeyCode_F11:
        return UWKKeyboard::Key_F11;
    case UnityKeyCode_F12:
        return UWKKeyboard::Key_F12;
    case UnityKeyCode_F13:
        return UWKKeyboard::Key_F13;
    case UnityKeyCode_F14:
        return UWKKeyboard::Key_F14;
    case UnityKeyCode_F15:
        return UWKKeyboard::Key_F15;
        break;

    case UnityKeyCode_Alpha0:
        return UWKKeyboard::Key_0;
    case UnityKeyCode_Alpha1:
        return UWKKeyboard::Key_1;
    case UnityKeyCode_Alpha2:
        return UWKKeyboard::Key_2;
    case UnityKeyCode_Alpha3:
        return UWKKeyboard::Key_3;
    case UnityKeyCode_Alpha4:
        return UWKKeyboard::Key_4;
    case UnityKeyCode_Alpha5:
        return UWKKeyboard::Key_5;
    case UnityKeyCode_Alpha6:
        return UWKKeyboard::Key_6;
    case UnityKeyCode_Alpha7:
        return UWKKeyboard::Key_7;
    case UnityKeyCode_Alpha8:
        return UWKKeyboard::Key_8;
    case UnityKeyCode_Alpha9:
        return UWKKeyboard::Key_9;

    case UnityKeyCode_Exclaim:
        return UWKKeyboard::Key_1; // translated to ! with shift modifier

    case UnityKeyCode_DoubleQuote:
        return UWKKeyboard::Key_QuoteDbl;

    case UnityKeyCode_Hash:
        return UWKKeyboard::Key_3; // translated to # with shift modifier
    case UnityKeyCode_Dollar:
        return UWKKeyboard::Key_4; // translated to $ with shift modifier
    case UnityKeyCode_Ampersand:
        return UWKKeyboard::Key_7; // translated to & with shift modifier
    case UnityKeyCode_Quote:
        return UWKKeyboard::Key_QuoteLeft;

    case UnityKeyCode_LeftParen:
        return UWKKeyboard::Key_9; // translated to ( with shift modifier

    case UnityKeyCode_RightParen:
        return UWKKeyboard::Key_0; // translated to ) with shift modifier

    case UnityKeyCode_Asterisk:
        return UWKKeyboard::Key_8; // translated to * with shift modifier

    case UnityKeyCode_Plus:
        return UWKKeyboard::Key_Equal; // translated to + with shift modifier
    case UnityKeyCode_Comma:
        return UWKKeyboard::Key_Comma;
    case UnityKeyCode_Minus:
        return UWKKeyboard::Key_Minus;
    case UnityKeyCode_Period:
        return UWKKeyboard::Key_Period;
    case UnityKeyCode_Slash:
        return UWKKeyboard::Key_Slash;
    case UnityKeyCode_Colon:
        return UWKKeyboard::Key_Semicolon; // translated to : with shift modifer
    case UnityKeyCode_Semicolon:
        return UWKKeyboard::Key_Semicolon;

    case UnityKeyCode_Less:
        return UWKKeyboard::Key_Comma; // translated to < with shift modifier
    case UnityKeyCode_Equals:
        return UWKKeyboard::Key_Equal;
    case UnityKeyCode_Greater:
        return UWKKeyboard::Key_Period; // translated to > with shift modifier

    case UnityKeyCode_Question:
        return UWKKeyboard::Key_Slash; // translated to ? with shift modifier

    case UnityKeyCode_At:
        return UWKKeyboard::Key_2; // translated to @ with shift modifier

    case UnityKeyCode_LeftBracket:
        return UWKKeyboard::Key_BracketLeft;
    case UnityKeyCode_Backslash:
        return UWKKeyboard::Key_Backslash; // translated to { with shift modifier
    case UnityKeyCode_RightBracket:
        return UWKKeyboard::Key_BracketRight; // translated to } with shift modifier
    case UnityKeyCode_Caret:
        return UWKKeyboard::Key_6; // translated to ^ with shift modifier
    case UnityKeyCode_Underscore:
        return UWKKeyboard::Key_Minus; // translated to _ with shift modifier

    case UnityKeyCode_BackQuote:
        return UWKKeyboard::Key_Agrave;

    case UnityKeyCode_A:
        return UWKKeyboard::Key_A;
    case UnityKeyCode_B:
        return UWKKeyboard::Key_B;
    case UnityKeyCode_C:
        return UWKKeyboard::Key_C;
    case UnityKeyCode_D:
        return UWKKeyboard::Key_D;
    case UnityKeyCode_E:
        return UWKKeyboard::Key_E;
    case UnityKeyCode_F:
        return UWKKeyboard::Key_F;
    case UnityKeyCode_G:
        return UWKKeyboard::Key_G;
    case UnityKeyCode_H:
        return UWKKeyboard::Key_H;
    case UnityKeyCode_I:
        return UWKKeyboard::Key_I;
    case UnityKeyCode_J:
        return UWKKeyboard::Key_J;
    case UnityKeyCode_K:
        return UWKKeyboard::Key_K;
    case UnityKeyCode_L:
        return UWKKeyboard::Key_L;
    case UnityKeyCode_M:
        return UWKKeyboard::Key_M;
    case UnityKeyCode_N:
        return UWKKeyboard::Key_N;
    case UnityKeyCode_O:
        return UWKKeyboard::Key_O;
    case UnityKeyCode_P:
        return UWKKeyboard::Key_P;
    case UnityKeyCode_Q:
        return UWKKeyboard::Key_Q;
    case UnityKeyCode_R:\
        return UWKKeyboard::Key_R;
    case UnityKeyCode_S:
        return UWKKeyboard::Key_S;
    case UnityKeyCode_T:
        return UWKKeyboard::Key_T;
    case UnityKeyCode_U:
        return UWKKeyboard::Key_U;
    case UnityKeyCode_V:
        return UWKKeyboard::Key_V;
    case UnityKeyCode_W:
        return UWKKeyboard::Key_W;
    case UnityKeyCode_X:
        return UWKKeyboard::Key_X;
    case UnityKeyCode_Y:
        return UWKKeyboard::Key_Y;
    case UnityKeyCode_Z:
        return UWKKeyboard::Key_Z;

    case UnityKeyCode_Numlock:
    case UnityKeyCode_CapsLock:
    case UnityKeyCode_ScrollLock:
    case UnityKeyCode_RightShift:
    case UnityKeyCode_LeftShift:
    case UnityKeyCode_RightControl:
    case UnityKeyCode_LeftControl:
    case UnityKeyCode_RightAlt:
    case UnityKeyCode_LeftAlt:
    case UnityKeyCode_LeftCommand:
        //case UnityKeyCode_LeftApple: same as UnityKeyCode_LeftCommand
    case UnityKeyCode_LeftWindows:
    case UnityKeyCode_RightCommand:
        //case UnityKeyCode_RightApple: same sas UnityKeyCode_RightCommand
    case UnityKeyCode_RightWindows:
    case UnityKeyCode_AltGr:
    case UnityKeyCode_Help:
    case UnityKeyCode_Print:
    case UnityKeyCode_SysReq:
    case UnityKeyCode_Break:
    case UnityKeyCode_Menu:
        break;
    }

    return UWKInvalidKeyCode;

}

bool UnityKeyHasCharacter(uint32_t unityKeyCode)
{
    switch (unityKeyCode)
    {
    case UnityKeyCode_Space:
    case UnityKeyCode_Keypad0:
    case UnityKeyCode_Keypad1:
    case UnityKeyCode_Keypad2:
    case UnityKeyCode_Keypad3:
    case UnityKeyCode_Keypad4:
    case UnityKeyCode_Keypad5:
    case UnityKeyCode_Keypad6:
    case UnityKeyCode_Keypad7:
    case UnityKeyCode_Keypad8:
    case UnityKeyCode_Keypad9:
    case UnityKeyCode_KeypadPeriod:
    case UnityKeyCode_KeypadDivide:
    case UnityKeyCode_KeypadMultiply:
    case UnityKeyCode_KeypadMinus:
    case UnityKeyCode_KeypadPlus:
    case UnityKeyCode_KeypadEnter:
    case UnityKeyCode_KeypadEquals:
    case UnityKeyCode_Alpha0:
    case UnityKeyCode_Alpha1:
    case UnityKeyCode_Alpha2:
    case UnityKeyCode_Alpha3:
    case UnityKeyCode_Alpha4:
    case UnityKeyCode_Alpha5:
    case UnityKeyCode_Alpha6:
    case UnityKeyCode_Alpha7:
    case UnityKeyCode_Alpha8:
    case UnityKeyCode_Alpha9:
    case UnityKeyCode_Exclaim:
    case UnityKeyCode_DoubleQuote:
    case UnityKeyCode_Hash:
    case UnityKeyCode_Dollar:
    case UnityKeyCode_Ampersand:
    case UnityKeyCode_Quote:
    case UnityKeyCode_LeftParen:
    case UnityKeyCode_RightParen:
    case UnityKeyCode_Asterisk:
    case UnityKeyCode_Plus:
    case UnityKeyCode_Comma:
    case UnityKeyCode_Minus:
    case UnityKeyCode_Period:
    case UnityKeyCode_Slash:
    case UnityKeyCode_Colon:
    case UnityKeyCode_Semicolon:
    case UnityKeyCode_Less:
    case UnityKeyCode_Equals:
    case UnityKeyCode_Greater:
    case UnityKeyCode_Question:
    case UnityKeyCode_At:
    case UnityKeyCode_LeftBracket:
    case UnityKeyCode_Backslash:
    case UnityKeyCode_RightBracket:
    case UnityKeyCode_Caret:
    case UnityKeyCode_Underscore:
    case UnityKeyCode_BackQuote:
    case UnityKeyCode_A:
    case UnityKeyCode_B:
    case UnityKeyCode_C:
    case UnityKeyCode_D:
    case UnityKeyCode_E:
    case UnityKeyCode_F:
    case UnityKeyCode_G:
    case UnityKeyCode_H:
    case UnityKeyCode_I:
    case UnityKeyCode_J:
    case UnityKeyCode_K:
    case UnityKeyCode_L:
    case UnityKeyCode_M:
    case UnityKeyCode_N:
    case UnityKeyCode_O:
    case UnityKeyCode_P:
    case UnityKeyCode_Q:
    case UnityKeyCode_R:
    case UnityKeyCode_S:
    case UnityKeyCode_T:
    case UnityKeyCode_U:
    case UnityKeyCode_V:
    case UnityKeyCode_W:
    case UnityKeyCode_X:
    case UnityKeyCode_Y:
    case UnityKeyCode_Z:
        //case UnityKeyCode_Return:
        return true;
    default:
        return false;
    }

    return false;
}

// end if

static uint32_t lastEventKeyCode = 0;
static std::map<uint32_t, uint32_t> keyCodes;

static void SendKeyMessage(UWKMessage& msg, UnityKeyEvent* keyEvent, int modifiers)
{
    msg.type = keyEvent->Type ? UMSG_KEY_DOWN : UMSG_KEY_UP;
    msg.iParams[0] = keyEvent->KeyCode;
    msg.iParams[1] = modifiers;
    msg.iParams[2] = keyEvent->Character;

    //UWKLog::LogVerbose("Web Key Event, Type:%i Modifiers:%i KeyCode:%i Character:%i",
    //                   msg.type, msg.iParams[1], msg.iParams[0], msg.iParams[2]);

}

bool EncodeUnityKeyEvent(UnityKeyEvent* keyEvent, UWKMessage& msg)
{
    // Unity splits up key down/up into multiple events some with keycodes, some with characters
    // these can come in different order depending on what keys are down as well (for instance cmd-c cmd-v)

    // We can get a keycode with no character followed by a character with no keycode, in this case
    // we will only get a key up with keycode, no character keyup

    // We can get a single keydown, with character and keycode, and a corresponding single key up with
    // keycode and character

    // UWKLog::LogVerbose("Unity Key Event, Type:%u Modifiers:%u KeyCode:%u Character:%u",
    //                    keyEvent->Type, keyEvent->Modifiers, keyEvent->KeyCode, keyEvent->Character);

#ifdef _MSC_VER

    if (keyEvent->Modifiers & UnityKeyModifier_Control &&
            ((keyEvent->KeyCode >= 'a' && keyEvent->KeyCode <= 'z') ||
             (keyEvent->KeyCode >= 'A' && keyEvent->KeyCode <= 'Z')))
    {
        keyEvent->KeyCode = toupper(keyEvent->KeyCode);
        keyEvent->Character = keyEvent->KeyCode;
        SendKeyMessage(msg, keyEvent, UWKKeyboard::Control);
        // select only works with down
        if (keyEvent->KeyCode == 65)
            msg.type = UMSG_KEY_DOWN;
        return true;
    }

#endif


    // Modifiers
    uint32_t modifiers = 0;

    if (keyEvent->Modifiers & UnityKeyModifier_Alt)
        modifiers |= UWKKeyboard::Alt;

    if (keyEvent->Modifiers & UnityKeyModifier_Control)
        modifiers |= UWKKeyboard::Control;

    if (keyEvent->Modifiers & UnityKeyModifier_CommandWin)
        modifiers |= UWKKeyboard::CommandWin;

    if (keyEvent->Modifiers & UnityKeyModifier_Shift)
        modifiers |= UWKKeyboard::Shift;


    // if we have a character and keycode coming in
    if (keyEvent->Character && keyEvent->KeyCode)
    {
        keyEvent->KeyCode = TranslateUnityKeyCode(keyEvent->KeyCode);
        if ( keyEvent->KeyCode == UWKInvalidKeyCode)
            return false;

        SendKeyMessage(msg, keyEvent, modifiers);
        return true;
    }

    std::map<uint32_t, uint32_t>::iterator itr;

    if (keyEvent->Character)
    {
        itr = keyCodes.find(lastEventKeyCode);

        if (itr == keyCodes.end())
        {
            keyCodes.insert(std::make_pair(lastEventKeyCode, keyEvent->Character));
        }
    }

    // check for ascii keycode
    if (keyEvent->KeyCode && UnityKeyHasCharacter(keyEvent->KeyCode))
    {
        uint32_t keyCode = keyEvent->KeyCode;

        if (keyEvent->Type)
        {
            // key down
            lastEventKeyCode = keyCode;

            if (modifiers & UWKKeyboard::Shift)
                lastEventKeyCode += 0xffff;

            return false;
        }
        else
        {
            // key up
            if (modifiers & UWKKeyboard::Shift)
                keyCode += 0xffff;

            itr = keyCodes.find(keyCode);
            if (itr == keyCodes.end())
                return false;

            keyEvent->Character = itr->second;

        }
    }

    if (!keyEvent->Character && keyEvent->KeyCode)
    {
        keyEvent->KeyCode = TranslateUnityKeyCode(keyEvent->KeyCode);
        if ( keyEvent->KeyCode == UWKInvalidKeyCode)
            return false;
    }

    if (keyEvent->Character)
    {
        modifiers &= ~UWKKeyboard::Shift;
        keyEvent->KeyCode = keyEvent->Character;
    }

    SendKeyMessage(msg, keyEvent, modifiers);

    return true;
}
