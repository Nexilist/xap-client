#pragma once

#include <linux/input.h>

#define InputKeyTypeCount 61

typedef unsigned char byte;
typedef unsigned short int ushort;

enum class InputKeyType : int {
    INPUT_UNKNOWN = 0,
    KEYBOARD_1 = 1,
    KEYBOARD_2 = 2,
    KEYBOARD_3 = 3,
    KEYBOARD_4 = 4,
    KEYBOARD_5 = 5,
    KEYBOARD_6 = 6,
    KEYBOARD_7 = 7,
    KEYBOARD_8 = 8,
    KEYBOARD_9 = 9,
    KEYBOARD_0 = 10,
    KEYBOARD_A = 11,
    KEYBOARD_B = 12,
    KEYBOARD_C = 13,
    KEYBOARD_D = 14,
    KEYBOARD_E = 15,
    KEYBOARD_F = 16,
    KEYBOARD_G = 17,
    KEYBOARD_H = 18,
    KEYBOARD_I = 19,
    KEYBOARD_J = 20,
    KEYBOARD_K = 21,
    KEYBOARD_L = 22,
    KEYBOARD_M = 23,
    KEYBOARD_N = 24,
    KEYBOARD_O = 25,
    KEYBOARD_P = 26,
    KEYBOARD_Q = 27,
    KEYBOARD_R = 28,
    KEYBOARD_S = 29,
    KEYBOARD_T = 30,
    KEYBOARD_U = 31,
    KEYBOARD_V = 32,
    KEYBOARD_W = 33,
    KEYBOARD_X = 34,
    KEYBOARD_Y = 35,
    KEYBOARD_Z = 36,
    KEYBOARD_F1 = 37,
    KEYBOARD_F2 = 38,
    KEYBOARD_F3 = 39,
    KEYBOARD_F4 = 40,
    KEYBOARD_F5 = 41,
    KEYBOARD_F6 = 42,
    KEYBOARD_F7 = 43,
    KEYBOARD_F8 = 44,
    KEYBOARD_F9 = 45,
    KEYBOARD_F10 = 46,
    KEYBOARD_F11 = 47,
    KEYBOARD_F12 = 48,
    KEYBOARD_CAPS_LOCK = 49,
    KEYBOARD_LEFT_SHIFT = 50,
    KEYBOARD_LEFT_CTRL = 51,
    KEYBOARD_LEFT_ALT = 52,
    KEYBOARD_INSERT = 53,
    KEYBOARD_DELETE = 54,
    KEYBOARD_SPACE = 55,
    MOUSE_Left = 56,
    MOUSE_Right = 57,
    MOUSE_Middle = 58,
    MOUSE_X1 = 59,
    MOUSE_X2 = 60
};

const char* InputKeyTypeNames[] = { "UNKNOWN", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "CAPS_LOCK", "LEFT_SHIFT", "LEFT_CTRL", "LEFT_ALT", "INSERT", "DELETE", "SPACE", "Mouse_Left", "Mouse_Right", "Mouse_Middle", "Mouse_X1", "Mouse_X2" };
    

InputKeyType mapMouseButtonType(ushort code) {
    switch (code)
    {
        case BTN_LEFT://272:
            return InputKeyType::MOUSE_Left;
        case BTN_RIGHT://273:
            return InputKeyType::MOUSE_Right;
        case BTN_MIDDLE://274:
            return InputKeyType::MOUSE_Middle;
        case BTN_SIDE://275:
            return InputKeyType::MOUSE_X1;
        case BTN_EXTRA://276:
            return InputKeyType::MOUSE_X2;
        default:
            return InputKeyType::INPUT_UNKNOWN;
    }

    return InputKeyType::INPUT_UNKNOWN;
}

InputKeyType mapKeyboardKeyType(int keyCode) {
    switch (keyCode) {
        case KEY_1: return InputKeyType::KEYBOARD_1;
        case KEY_2: return InputKeyType::KEYBOARD_2;
        case KEY_3: return InputKeyType::KEYBOARD_3;
        case KEY_4: return InputKeyType::KEYBOARD_4;
        case KEY_5: return InputKeyType::KEYBOARD_5;
        case KEY_6: return InputKeyType::KEYBOARD_6;
        case KEY_7: return InputKeyType::KEYBOARD_7;
        case KEY_8: return InputKeyType::KEYBOARD_8;
        case KEY_9: return InputKeyType::KEYBOARD_9;
        case KEY_0: return InputKeyType::KEYBOARD_0;
        case KEY_A: return InputKeyType::KEYBOARD_A;
        case KEY_B: return InputKeyType::KEYBOARD_B;
        case KEY_C: return InputKeyType::KEYBOARD_C;
        case KEY_D: return InputKeyType::KEYBOARD_D;
        case KEY_E: return InputKeyType::KEYBOARD_E;
        case KEY_F: return InputKeyType::KEYBOARD_F;
        case KEY_G: return InputKeyType::KEYBOARD_G;
        case KEY_H: return InputKeyType::KEYBOARD_H;
        case KEY_I: return InputKeyType::KEYBOARD_I;
        case KEY_J: return InputKeyType::KEYBOARD_J;
        case KEY_K: return InputKeyType::KEYBOARD_K;
        case KEY_L: return InputKeyType::KEYBOARD_L;
        case KEY_M: return InputKeyType::KEYBOARD_M;
        case KEY_N: return InputKeyType::KEYBOARD_N;
        case KEY_O: return InputKeyType::KEYBOARD_O;
        case KEY_P: return InputKeyType::KEYBOARD_P;
        case KEY_Q: return InputKeyType::KEYBOARD_Q;
        case KEY_R: return InputKeyType::KEYBOARD_R;
        case KEY_S: return InputKeyType::KEYBOARD_S;
        case KEY_T: return InputKeyType::KEYBOARD_T;
        case KEY_U: return InputKeyType::KEYBOARD_U;
        case KEY_V: return InputKeyType::KEYBOARD_V;
        case KEY_W: return InputKeyType::KEYBOARD_W;
        case KEY_X: return InputKeyType::KEYBOARD_X;
        case KEY_Y: return InputKeyType::KEYBOARD_Y;
        case KEY_Z: return InputKeyType::KEYBOARD_Z;
        case KEY_F1: return InputKeyType::KEYBOARD_F1;
        case KEY_F2: return InputKeyType::KEYBOARD_F2;
        case KEY_F3: return InputKeyType::KEYBOARD_F3;
        case KEY_F4: return InputKeyType::KEYBOARD_F4;
        case KEY_F5: return InputKeyType::KEYBOARD_F5;
        case KEY_F6: return InputKeyType::KEYBOARD_F6;
        case KEY_F7: return InputKeyType::KEYBOARD_F7;
        case KEY_F8: return InputKeyType::KEYBOARD_F8;
        case KEY_F9: return InputKeyType::KEYBOARD_F9;
        case KEY_F10: return InputKeyType::KEYBOARD_F10;
        case KEY_F11: return InputKeyType::KEYBOARD_F11;
        case KEY_F12: return InputKeyType::KEYBOARD_F12;
        case KEY_CAPSLOCK: return InputKeyType::KEYBOARD_CAPS_LOCK;
        case KEY_LEFTSHIFT: return InputKeyType::KEYBOARD_LEFT_SHIFT;
        case KEY_LEFTCTRL: return InputKeyType::KEYBOARD_LEFT_CTRL;
        case KEY_LEFTALT: return InputKeyType::KEYBOARD_LEFT_ALT;
        case KEY_SPACE: return InputKeyType::KEYBOARD_SPACE;
        default: return InputKeyType::INPUT_UNKNOWN;
    }
}