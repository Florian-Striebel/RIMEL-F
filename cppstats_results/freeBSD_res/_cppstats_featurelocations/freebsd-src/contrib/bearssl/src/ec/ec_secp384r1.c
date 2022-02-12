























#include "inner.h"

static const unsigned char P384_N[] = {
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xC7, 0x63, 0x4D, 0x81, 0xF4, 0x37, 0x2D, 0xDF,
0x58, 0x1A, 0x0D, 0xB2, 0x48, 0xB0, 0xA7, 0x7A,
0xEC, 0xEC, 0x19, 0x6A, 0xCC, 0xC5, 0x29, 0x73
};

static const unsigned char P384_G[] = {
0x04, 0xAA, 0x87, 0xCA, 0x22, 0xBE, 0x8B, 0x05,
0x37, 0x8E, 0xB1, 0xC7, 0x1E, 0xF3, 0x20, 0xAD,
0x74, 0x6E, 0x1D, 0x3B, 0x62, 0x8B, 0xA7, 0x9B,
0x98, 0x59, 0xF7, 0x41, 0xE0, 0x82, 0x54, 0x2A,
0x38, 0x55, 0x02, 0xF2, 0x5D, 0xBF, 0x55, 0x29,
0x6C, 0x3A, 0x54, 0x5E, 0x38, 0x72, 0x76, 0x0A,
0xB7, 0x36, 0x17, 0xDE, 0x4A, 0x96, 0x26, 0x2C,
0x6F, 0x5D, 0x9E, 0x98, 0xBF, 0x92, 0x92, 0xDC,
0x29, 0xF8, 0xF4, 0x1D, 0xBD, 0x28, 0x9A, 0x14,
0x7C, 0xE9, 0xDA, 0x31, 0x13, 0xB5, 0xF0, 0xB8,
0xC0, 0x0A, 0x60, 0xB1, 0xCE, 0x1D, 0x7E, 0x81,
0x9D, 0x7A, 0x43, 0x1D, 0x7C, 0x90, 0xEA, 0x0E,
0x5F
};


const br_ec_curve_def br_secp384r1 = {
BR_EC_secp384r1,
P384_N, sizeof P384_N,
P384_G, sizeof P384_G
};
