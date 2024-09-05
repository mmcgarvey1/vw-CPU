#include <string.h>
#include <SDL2/SDL.h>
#include <ctype.h>
#include "execute.h"

unsigned int get_key_code(SDL_Keycode key) {
    switch (key) {
        case 1073741906: // up
            return 249;
        case 1073741905: // down
            return 248;
        case 1073741904: // left
            return 247;
        case 1073741903: // right
            return 246;
    }
    const char *characters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";
    const char *ptr = strchr(characters, key);
    
    if (ptr != NULL) {
        return (unsigned int)(ptr - characters);
    } else if (key == 13) { // enter key
        return 254;
    } else if (key == 8) { // backspace key
        return 253;
    } else if (key == 27) { // esc key
        return 252;
    } else {
        return 255; // key not found
    }
}

char shift_convert(char key) {
    if (isalpha(key)) {
        return toupper(key);  // Use toUpper for alphabetic characters
    }
    switch (key) {
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        case '`': return '~';
        default: return key;
    }
}

unsigned int ctrl_convert(SDL_Keycode key) {
    switch (key) {
        case 114: // run
            return 243;
        case 99:  // clear
            return 244;
        case 111: // open
            return 245;
        case 115: // save
            return 251;
        case 108: // load
            return 250;
        case 1073742048:
            return 255;
    }
    printf("key: %d", key);
    return 255; // key not found
}