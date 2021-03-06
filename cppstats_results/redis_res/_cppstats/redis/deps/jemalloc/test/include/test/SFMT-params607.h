#define POS1 2
#define SL1 15
#define SL2 3
#define SR1 13
#define SR2 3
#define MSK1 0xfdff37ffU
#define MSK2 0xef7f3f7dU
#define MSK3 0xff777b7dU
#define MSK4 0x7ff7fb2fU
#define PARITY1 0x00000001U
#define PARITY2 0x00000000U
#define PARITY3 0x00000000U
#define PARITY4 0x5986f054U
#if defined(__APPLE__)
#define ALTI_SL1 (vector unsigned int)(SL1, SL1, SL1, SL1)
#define ALTI_SR1 (vector unsigned int)(SR1, SR1, SR1, SR1)
#define ALTI_MSK (vector unsigned int)(MSK1, MSK2, MSK3, MSK4)
#define ALTI_MSK64 (vector unsigned int)(MSK2, MSK1, MSK4, MSK3)
#define ALTI_SL2_PERM (vector unsigned char)(3,21,21,21,7,0,1,2,11,4,5,6,15,8,9,10)
#define ALTI_SL2_PERM64 (vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define ALTI_SR2_PERM (vector unsigned char)(5,6,7,0,9,10,11,4,13,14,15,8,19,19,19,12)
#define ALTI_SR2_PERM64 (vector unsigned char)(13,14,15,0,1,2,3,4,19,19,19,8,9,10,11,12)
#else
#define ALTI_SL1 {SL1, SL1, SL1, SL1}
#define ALTI_SR1 {SR1, SR1, SR1, SR1}
#define ALTI_MSK {MSK1, MSK2, MSK3, MSK4}
#define ALTI_MSK64 {MSK2, MSK1, MSK4, MSK3}
#define ALTI_SL2_PERM {3,21,21,21,7,0,1,2,11,4,5,6,15,8,9,10}
#define ALTI_SL2_PERM64 {3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2}
#define ALTI_SR2_PERM {5,6,7,0,9,10,11,4,13,14,15,8,19,19,19,12}
#define ALTI_SR2_PERM64 {13,14,15,0,1,2,3,4,19,19,19,8,9,10,11,12}
#endif
#define IDSTR "SFMT-607:2-15-3-13-3:fdff37ff-ef7f3f7d-ff777b7d-7ff7fb2f"
