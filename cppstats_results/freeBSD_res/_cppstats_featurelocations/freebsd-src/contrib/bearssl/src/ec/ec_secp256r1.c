























#include "inner.h"

static const unsigned char P256_N[] = {
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84,
0xF3, 0xB9, 0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51
};

static const unsigned char P256_G[] = {
0x04, 0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42,
0x47, 0xF8, 0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40,
0xF2, 0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33,
0xA0, 0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2,
0x96, 0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F,
0x9B, 0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E,
0x16, 0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E,
0xCE, 0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51,
0xF5
};


const br_ec_curve_def br_secp256r1 = {
BR_EC_secp256r1,
P256_N, sizeof P256_N,
P256_G, sizeof P256_G
};
