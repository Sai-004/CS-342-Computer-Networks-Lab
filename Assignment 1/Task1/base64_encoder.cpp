#include <iostream>
#include <map>
#include <string>
using namespace std;

string base64_encode(const string &input) {
    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    string encoded_string;

    size_t input_length = input.length();
    const unsigned char *input_data = reinterpret_cast<const unsigned char *>(input.c_str());

    size_t i = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (input_length--) {
        char_array_3[i++] = *(input_data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xFC) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xF0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0F) << 2) + ((char_array_3[2] & 0xC0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3F;

            for (i = 0; i < 4; i++) {
                encoded_string += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (size_t j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xFC) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xF0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0F) << 2) + ((char_array_3[2] & 0xC0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3F;

        for (size_t j = 0; j < i + 1; j++) {
            encoded_string += base64_chars[char_array_4[j]];
        }

        while (i++ < 3) {
            encoded_string += '=';
        }
    }

    return encoded_string;
}