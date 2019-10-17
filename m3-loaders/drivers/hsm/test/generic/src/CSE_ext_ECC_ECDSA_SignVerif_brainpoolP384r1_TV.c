/*
 * CSE_ext_ECC_ECDSA_SignVerif_brainpoolP384r1_TV.c
 *
 */

#include "config.h"
#include "CSE_ext_hash.h"
#include "CSE_ext_ECC_ECDSA_SignGenVerif_TV.h"

#ifdef INCLUDE_BRAINPOOL_P384R1
/* Test vectors with error introduced to generate wrong verification */
const verify_test_vect_stt verify_test_vect_array_brainpoolP384r1[NB_OF_ECDSA_SIGN_VERIF_TV_brainpoolP384r1_SHA384] =
{
#ifdef INCLUDE_SHA384
/*[P-384,SHA-384]*/

  { /* TV 19-1*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x41,0x32,0x83,0x3a,0x52,0x5a,0xec,0xc8,0xa1,0xa6,0xde,0xa9,0xf4,0x07,0x5f,0x44,
     0xfe,0xef,0xce,0x81,0x0c,0x46,0x68,0x42,0x3b,0x38,0x58,0x04,0x17,0xf7,0xbd,0xca,
     0x5b,0x21,0x06,0x1a,0x45,0xea,0xa3,0xcb,0xe2,0xa7,0x03,0x5e,0xd1,0x89,0x52,0x3a,
     0xf8,0x00,0x2d,0x65,0xc2,0x89,0x9e,0x65,0x73,0x5e,0x4d,0x93,0xa1,0x65,0x03,0xc1,
     0x45,0x05,0x9f,0x36,0x5c,0x32,0xb3,0xac,0xc6,0x27,0x0e,0x29,0xa0,0x92,0x31,0x29,	/* 0x92 was 0x91 */
     0x91,0x81,0xc9,0x8b,0x3c,0x76,0x76,0x9a,0x18,0xfa,0xf2,0x1f,0x6b,0x4a,0x8f,0x27,
     0x1e,0x6b,0xf9,0x08,0xe2,0x38,0xaf,0xe8,0x00,0x2e,0x27,0xc6,0x34,0x17,0xbd,0xa7,
     0x58,0xf8,0x46,0xe1,0xe3,0xb8,0xe6,0x2d,0x7f,0x05,0xeb,0xd9,0x8f,0x1f,0x91,0x54 },
    /* Qx */
    {0x23,0x43,0x3A,0x45,0x59,0xE4,0xB0,0xE2,0xD4,0xE2,0x71,0xDE,0x3F,0xC5,0x96,0x93,
     0x2B,0x39,0x93,0x97,0x82,0xF0,0x7A,0x9E,0xA7,0x7D,0x6C,0x91,0xB6,0x3F,0x95,0xD2,
     0xF7,0x6D,0x7C,0x85,0x38,0x21,0x57,0x31,0x93,0x93,0x9F,0xC9,0x39,0xD5,0x34,0x37 },
    /* Qy */
    {0x75,0x8E,0x5F,0x1F,0xAC,0xB9,0xE9,0x8B,0x12,0xAD,0xE0,0xE2,0xC8,0xC9,0x6B,0x21,
     0x67,0x03,0xBD,0x7D,0x4C,0xFF,0xC6,0xCE,0xD4,0x75,0xEB,0x6E,0xF8,0x3B,0xBD,0x42,
     0x52,0x63,0x5D,0x55,0x1B,0x1D,0x26,0xAB,0x38,0xC2,0x07,0x8F,0x34,0xB5,0xAE,0x6D },
    /* R */
    {0x8A,0x98,0x39,0x02,0xBA,0xC4,0xF5,0xE0,0xC3,0x4C,0x6A,0xAD,0x08,0xE8,0xC9,0x74,
     0x3D,0xEB,0xEC,0xE3,0xC2,0x04,0xEB,0xF6,0xE0,0xA1,0x8E,0xB3,0x33,0xFC,0xDD,0x3F,
     0x78,0x71,0xD6,0x12,0xA1,0xEE,0xFA,0x47,0x61,0xCE,0x1F,0x16,0x37,0xB9,0xB9,0x01 },
    /* S */
    {0x67,0x10,0x96,0x77,0xBF,0x5D,0xE9,0xE3,0x3E,0x93,0x10,0xB1,0xA4,0x8C,0x2E,0x5B,
     0x5D,0xB7,0x06,0x1F,0x15,0x30,0xEE,0x23,0x10,0x9F,0xA3,0x64,0x20,0xB5,0xC7,0xC2,
     0x7F,0x07,0x6E,0x98,0xA1,0x9A,0xE2,0x7F,0xB9,0x92,0x2D,0xDC,0x2E,0x11,0x3B,0x09 },
    /* Result */
    TEST_FAIL /* Message modified */
  },

  { /* TV 19-2*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x9d,0xd7,0x89,0xea,0x25,0xc0,0x47,0x45,0xd5,0x7a,0x38,0x1f,0x22,0xde,0x01,0xfb,
     0x0a,0xbd,0x3c,0x72,0xdb,0xde,0xfd,0x44,0xe4,0x32,0x13,0xc1,0x89,0x58,0x3e,0xef,
     0x85,0xba,0x66,0x20,0x44,0xda,0x3d,0xe2,0xdd,0x86,0x70,0xe6,0x32,0x51,0x54,0x48,
     0x01,0x55,0xbb,0xee,0xbb,0x70,0x2c,0x75,0x78,0x1a,0xc3,0x2e,0x13,0x94,0x18,0x60,
     0xcb,0x57,0x6f,0xe3,0x7a,0x05,0xb7,0x57,0xda,0x5b,0x5b,0x41,0x8f,0x6d,0xd7,0xc3,
     0x0b,0x04,0x2e,0x40,0xf4,0x39,0x5a,0x34,0x2a,0xe4,0xdc,0xe0,0x56,0x34,0xc3,0x36,
     0x25,0xe2,0xbc,0x52,0x43,0x45,0x48,0x1f,0x7e,0x25,0x3d,0x95,0x51,0x26,0x68,0x23,
     0x77,0x1b,0x25,0x17,0x05,0xb4,0xa8,0x51,0x66,0x02,0x2a,0x37,0xac,0x28,0xf1,0xbd },
    /* Qx */
    {0x6F,0xCD,0x4C,0x62,0x48,0x98,0x80,0xF3,0xE0,0x59,0xB6,0x5B,0x86,0xA2,0xBD,0x9E,
     0x92,0x02,0x52,0x7A,0x71,0xC6,0x5C,0xFE,0xED,0xE0,0xE3,0x99,0xE5,0x02,0x8C,0x8B,	/* 0x8B was 0x9B*/
     0xFF,0x2C,0x39,0xA5,0xD4,0x74,0x0E,0x31,0x3B,0xBA,0x1B,0x74,0xC5,0x37,0x97,0x63 },
    /* Qy */
    {0x20,0xE3,0x27,0x21,0x41,0xDE,0x36,0x6C,0x40,0xEA,0x92,0xBB,0xAD,0xF0,0xC5,0x65,
     0x18,0xBD,0xBC,0x6D,0xF6,0x86,0xFD,0xD4,0x8A,0x19,0x81,0xB2,0x51,0xBD,0x0A,0xD5,
     0x1E,0xC4,0x1B,0x3D,0xDA,0x20,0xD9,0x3E,0xC6,0x08,0x60,0x87,0x0D,0x2A,0x8D,0x44 },
    /* R */
    {0x0A,0x72,0xF5,0x07,0xA4,0x07,0xE3,0xAB,0xFD,0x56,0x0F,0x33,0xA8,0x37,0x9B,0xF9,
     0xDF,0xA1,0x74,0x28,0x28,0x79,0x68,0x13,0xDE,0x88,0xE9,0x2B,0x9D,0x84,0x81,0x63,
     0x0C,0x6E,0xFB,0xC8,0x0B,0x9F,0xD5,0x9D,0x81,0xED,0xB5,0x5D,0x9B,0x42,0xB1,0xFC },
    /* S */
    {0x4B,0xF3,0xDE,0x7F,0xD4,0xCC,0x61,0xE0,0x99,0x22,0x03,0xEB,0x1F,0x00,0x18,0x24,
     0x02,0x5F,0xC7,0x2C,0xFA,0x4F,0x0F,0x85,0xBD,0x5D,0xA2,0x3D,0xE9,0x16,0xF4,0x5D,
     0x36,0x1B,0x9B,0xA2,0xD9,0x39,0x38,0x8A,0x8F,0x07,0x2B,0x39,0x79,0x64,0x8E,0x31 },
    /* Result */
    TEST_FAIL /* Qx modified */
  },

  { /* TV 19-3*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x9c,0x44,0x79,0x97,0x7e,0xd3,0x77,0xe7,0x5f,0x5c,0xc0,0x47,0xed,0xfa,0x68,0x9e,
     0xf2,0x32,0x79,0x95,0x13,0xa2,0xe7,0x02,0x80,0xe9,0xb1,0x24,0xb6,0xc8,0xd1,0x66,
     0xe1,0x07,0xf5,0x49,0x4b,0x40,0x68,0x53,0xae,0xc4,0xcf,0xf0,0xf2,0xca,0x00,0xc6,
     0xf8,0x9f,0x0f,0x4a,0x2d,0x4a,0xb0,0x26,0x7f,0x44,0x51,0x2d,0xff,0xf1,0x10,0xd1,
     0xb1,0xb2,0xe5,0xe7,0x88,0x32,0x02,0x2c,0x14,0xac,0x06,0xa4,0x93,0xab,0x78,0x9e,
     0x69,0x6f,0x7f,0x0f,0x06,0x08,0x77,0x02,0x9c,0x27,0x15,0x7c,0xe4,0x0f,0x81,0x25,
     0x87,0x29,0xca,0xa4,0xd9,0x77,0x8b,0xae,0x48,0x9d,0x3a,0xb0,0x25,0x9f,0x67,0x33,
     0x08,0xae,0x1e,0xc1,0xb1,0x94,0x8a,0xd2,0x84,0x5f,0x86,0x3b,0x36,0xae,0xdf,0xfb },
    /* Qx */
    {0x7E,0x2E,0x41,0x94,0x47,0x26,0xEB,0x1C,0xDC,0xB3,0x38,0x1F,0x03,0xB9,0x1C,0x1C,
     0x7C,0x26,0x33,0x06,0x36,0xF5,0x61,0xD9,0xF8,0xFC,0x2D,0x45,0xF3,0x43,0x74,0x97,
     0x6A,0x91,0x82,0x68,0xCA,0x51,0x49,0x3A,0xDE,0x1C,0xFA,0xB9,0xC7,0xA1,0x7D,0x2B },
    /* Qy */
    {0x24,0xAD,0x7A,0xEE,0x36,0xAF,0x9A,0xD1,0x09,0x8B,0x32,0x81,0x99,0x05,0xBF,0x56,	/* 0x36 was 0x35 */
     0x0F,0xF9,0xBB,0x3D,0x19,0x2D,0x3A,0x14,0x78,0x3A,0x28,0xEF,0x8F,0x51,0xA0,0xA0,
     0x39,0x86,0x4B,0xDE,0xDB,0x4C,0x77,0xB9,0x8F,0x24,0xC8,0xDA,0xF8,0x95,0x59,0x37 },
    /* R */
    {0x62,0xF0,0x56,0xB1,0x44,0xBA,0x24,0x41,0xA3,0x62,0x51,0xC0,0xB9,0x13,0x64,0x8E,
     0x61,0x9D,0x68,0x31,0x8B,0x77,0xDE,0xAE,0x52,0x6A,0x28,0xB6,0xA8,0x4A,0xE8,0x9B,
     0xC4,0x55,0x0D,0xFA,0x4B,0x99,0x4D,0x53,0xA5,0x1C,0x0F,0x59,0x39,0xFB,0x01,0xE7 },
    /* S */
    {0x18,0x8C,0x45,0xA5,0xBB,0xEE,0x44,0x16,0xDB,0x6F,0x29,0x3A,0x53,0xDC,0x0A,0xA0,
     0x07,0xEE,0x90,0x8D,0x59,0xD9,0xDC,0xFC,0x38,0x6E,0x44,0xB9,0xBD,0x75,0x1F,0x6A,
     0x32,0xAF,0xFF,0x06,0xFF,0x85,0xEF,0x88,0x07,0x1B,0x82,0xDF,0xA1,0xA9,0x9C,0x8A },
    /* Result */
    TEST_FAIL /* Qy modified */
  },

  { /* TV 19-4*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x21,0xeb,0x31,0xf2,0xb3,0x4e,0x4d,0xde,0x8d,0x6c,0x70,0x1e,0x97,0x6d,0x3f,0xbb,
     0xf4,0xde,0x6a,0x33,0x84,0x32,0x91,0x18,0xd4,0xdd,0xb4,0x9a,0xdb,0x2b,0xb4,0x44,
     0x65,0x59,0x8a,0xbf,0x6d,0xf2,0x58,0x58,0xb4,0x50,0xc7,0x76,0x7e,0x28,0x2c,0xca,
     0xca,0x49,0x40,0x88,0x27,0x4e,0x37,0x35,0x36,0x74,0xee,0xf5,0x8f,0x58,0x39,0x37,
     0xd3,0xd1,0x84,0xef,0x72,0x73,0x17,0xd3,0x67,0x23,0x97,0xa7,0x4c,0x8f,0xe3,0x27,
     0x91,0x9a,0x3d,0xf8,0xfd,0x65,0xaf,0x0b,0xc8,0xce,0xbb,0xc4,0x00,0x95,0xad,0xf8,
     0x9f,0x1b,0xf2,0xc5,0xe6,0xdc,0x6b,0xa4,0x46,0x33,0xfd,0x84,0x33,0xb2,0x5f,0x06,
     0x5f,0x5e,0x3e,0xb4,0x84,0x0a,0xf2,0x3c,0xc5,0x34,0x41,0x54,0x06,0x74,0x5a,0x31 },
    /* Qx */
    {0x87,0x72,0x9D,0x90,0xBC,0x99,0x66,0x31,0x27,0x10,0x5F,0xA0,0x00,0x4F,0xBB,0xAA,
     0x8C,0xF4,0x5D,0x4C,0xA4,0x76,0x8A,0x7C,0xC8,0xDE,0xD4,0x09,0x8C,0x89,0x80,0x6B,
     0x5F,0x5F,0xF0,0xB0,0x95,0x7D,0x14,0x9A,0xB3,0x8A,0xFB,0x59,0x9B,0x04,0x85,0x75 },
    /* Qy */
    {0x6B,0x64,0x96,0x7D,0x14,0x54,0x2E,0x01,0x89,0xF0,0xC9,0x96,0x2D,0xF8,0x0D,0x7C,
     0x00,0x2E,0x46,0xB1,0x4B,0xA4,0x3F,0x80,0x1A,0xDA,0x95,0x65,0xBB,0x3F,0xB3,0xE9,
     0x6A,0x0F,0x63,0x31,0x9E,0xEE,0xBC,0x4B,0xE1,0x94,0x84,0x47,0xC9,0xE0,0xE4,0x8B },
    /* R */
    {0x2F,0xCF,0x1A,0x95,0x1C,0x89,0x48,0x38,0x50,0xE1,0x8B,0x05,0xB4,0x58,0x5C,0xC4,
     0x83,0xD0,0x47,0x00,0x08,0x4D,0x62,0xA2,0x2E,0xCC,0xCB,0xB4,0x33,0x2F,0x22,0x7E,
     0x6C,0x8B,0x2E,0xF6,0x29,0x4C,0x37,0xE4,0x70,0xDA,0xC4,0xDD,0xCB,0xE8,0x87,0x8D }, /* 0x8D was 0x8C */
    /* S */
    {0x0D,0xC7,0x01,0x6D,0x02,0xC7,0x11,0x72,0x7B,0x71,0x17,0xAE,0xFB,0xBD,0xF2,0x57,
     0x48,0x82,0xB6,0x13,0xD0,0xC7,0x88,0xC6,0x3D,0x42,0x4F,0xF9,0xBE,0xE3,0x97,0x0B,
     0xF6,0x1B,0x33,0x4C,0x31,0x58,0x0C,0x32,0x8F,0x9E,0x42,0x59,0x02,0xC5,0xDB,0x02 },
    /* Result */
    TEST_FAIL /* R modified */
  },

  { /* TV 19-5*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x58,0xea,0x3b,0x1e,0x82,0xf9,0x77,0x08,0x05,0x3d,0x0b,0x41,0x44,0x1d,0x0a,0xa9,
     0x61,0x90,0x50,0xe8,0x6a,0xc6,0xc4,0xf7,0x78,0x11,0x64,0xe5,0xda,0x30,0x19,0xc4,
     0x7a,0x83,0x93,0x66,0x50,0x9f,0xa9,0x58,0x12,0xe4,0xf6,0x4a,0xfd,0xc6,0x2b,0x62,
     0x7c,0x7a,0x98,0xf6,0x33,0xdd,0x05,0xdb,0x45,0xc1,0xd8,0x95,0x4f,0xc8,0x3b,0xdb,
     0x50,0x42,0x67,0x93,0x78,0xbb,0x7e,0x4c,0x78,0x63,0xaa,0xcf,0x20,0x26,0x36,0x0c,
     0xa5,0x83,0x14,0x98,0x3e,0x6c,0x72,0x6c,0xf0,0x2b,0xb3,0x47,0x70,0x6b,0x84,0x4d,
     0xdc,0x66,0xae,0xe4,0x17,0x7c,0x30,0x9c,0xb7,0x00,0x76,0x95,0x53,0x48,0x0c,0xdd,
     0x6b,0x1c,0xd7,0x73,0x41,0xc9,0xa8,0x1c,0x05,0xfb,0xb8,0x08,0x19,0xbc,0x62,0x3f },
    /* Qx */
    {0x5A,0x7B,0xA8,0x77,0xE7,0xD3,0x54,0x0B,0xC0,0xB7,0x67,0xD1,0x58,0xBC,0xEA,0xF9,
     0x88,0xBA,0xC8,0x64,0x90,0x6E,0x64,0xAD,0xC5,0x7C,0x2E,0x0C,0xB3,0x64,0xDF,0x6F,
     0x97,0xE2,0x6F,0xD4,0xDA,0xD8,0x39,0x51,0x00,0x81,0xB9,0x14,0x88,0x62,0x9B,0x47 },
    /* Qy */
    {0x41,0x34,0x76,0x2A,0x94,0x75,0xC3,0x05,0x07,0x8E,0x55,0x74,0x58,0x8C,0x73,0xCB,
     0x58,0xBA,0x20,0x03,0x4D,0x5B,0xCF,0x8D,0x3E,0xAF,0xE8,0xFB,0xD6,0x5D,0x1E,0x7E,
     0x0C,0x7F,0x49,0xB1,0x0B,0x7F,0x9F,0x80,0x6C,0xE7,0x40,0xF0,0xAA,0x9E,0x16,0xAF },
    /* R */
    {0x2A,0x0B,0xD9,0x0D,0xA9,0x2F,0x0E,0x2A,0xCF,0xC3,0xD3,0x9B,0xFB,0x8E,0xAA,0xEC,
     0xB1,0x29,0x3C,0x93,0xE9,0x63,0xEA,0xE7,0x3A,0xA2,0xAA,0x01,0x9A,0xA2,0x04,0x13,
     0x9A,0xA0,0xD4,0x37,0x1C,0x6C,0x11,0xBD,0xF3,0xEB,0x3A,0x78,0x3A,0xB0,0xC1,0x12 },
    /* S */
    {0x06,0x81,0x50,0xB6,0x16,0x3C,0x91,0x7C,0x2C,0xBF,0x27,0xF2,0xEE,0xA1,0xB9,0xCE,	/* 0xCE was 0xCF */
     0x04,0x1F,0x51,0xBA,0x02,0xE3,0x03,0x60,0xE6,0x9F,0x99,0xD2,0x3C,0x43,0x46,0x86,
     0x77,0xB3,0xBA,0x7A,0x8A,0x89,0xF6,0x8A,0xB4,0x36,0xAB,0xAF,0xF4,0x8A,0xE5,0x08 },
    /* Result */
    TEST_FAIL /* S modified */
  },

  { /* TV 19-6*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x18,0x8c,0xd5,0x30,0x97,0xef,0x3e,0x64,0xb7,0x8b,0x92,0x60,0xbf,0x46,0x17,0x08,
     0xc8,0x36,0xf2,0x5f,0x2b,0xcc,0x98,0xb5,0x34,0xaf,0x98,0xb9,0x6e,0xe4,0xb3,0x24,
     0xe2,0x20,0x3a,0x7e,0x62,0xdb,0xc3,0x96,0x96,0x6f,0x56,0x41,0x9f,0xb5,0x13,0x5c,
     0xb1,0x24,0x36,0x9a,0xaa,0x02,0x5f,0x39,0x6e,0xac,0x72,0xf0,0x5a,0xb4,0x59,0x50,
     0xd9,0xe0,0x2c,0xd5,0xa2,0x35,0x7e,0xaf,0xab,0x9f,0x81,0x61,0x17,0xb7,0xf1,0xde,
     0x19,0x24,0x68,0x89,0x53,0x27,0x80,0x2e,0xc7,0x9f,0x5d,0x6b,0x5a,0x3d,0x44,0xd7,
     0xaf,0xbe,0xd7,0xb4,0xa3,0x08,0xe3,0x65,0x65,0x5b,0x8d,0xb2,0xbd,0xe7,0x5e,0x14,
     0x30,0x62,0xee,0x48,0xb7,0xc5,0x16,0x88,0xac,0x5d,0xb0,0xbc,0x7c,0x83,0xec,0x9c },
    /* Qx */
    {0x24,0xAA,0x56,0x15,0x29,0x20,0x82,0xFF,0xCC,0xEE,0xCF,0x46,0x2C,0x6A,0x3C,0x5D,
     0xD6,0x2B,0x3F,0x8A,0x9E,0x7A,0x26,0xAE,0xD8,0x71,0xA0,0xC0,0xFC,0x13,0x79,0xC5,
     0x09,0x1B,0x96,0x88,0xDD,0x5A,0xB8,0x58,0x88,0xEA,0xB0,0x51,0xE6,0xD8,0xB2,0x45 },
    /* Qy */
    {0x7C,0xD4,0xDF,0x80,0xC6,0x89,0x17,0x08,0x64,0xA6,0x92,0xF8,0x11,0x3F,0xDD,0x2A,
     0x0F,0x04,0xC2,0x7F,0x4F,0x2F,0xBE,0x5B,0xA8,0x55,0x02,0xE6,0xB2,0x2C,0x75,0x33,
     0xDB,0xFE,0x02,0x00,0x01,0xA6,0x92,0x9D,0x91,0x3F,0xC4,0xEB,0xF1,0x82,0x4B,0xD1 },
    /* R */
    {0x36,0x11,0x92,0x0C,0x18,0xDC,0x4A,0xAD,0xDE,0xEB,0xDE,0x82,0x4F,0xC8,0x46,0xDF,
     0xB0,0xC3,0xB2,0x1B,0xA5,0xEA,0x0A,0xA0,0x06,0xC5,0x80,0x62,0x47,0x96,0x97,0x86,
     0x19,0x79,0x4C,0xB5,0x97,0xE8,0xDA,0xF2,0x15,0x52,0x0B,0x5D,0x52,0x0A,0x27,0xA8 },
    /* S */
    {0x4A,0x28,0xFE,0x3B,0xF2,0x54,0xCA,0x1A,0x37,0x93,0x3B,0xFF,0xF9,0xD9,0xF7,0xE0,
     0x06,0x42,0x1F,0xE0,0x7D,0xEB,0x9E,0xF9,0x60,0x9A,0xC4,0x92,0x82,0xD7,0x81,0xCE,
     0x53,0x7C,0x8D,0x89,0x6A,0xB1,0x7F,0xC5,0xA9,0xBF,0xE4,0x51,0x67,0xAF,0x17,0x6D },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-7*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x64,0x62,0xbc,0x8c,0x01,0x81,0xdb,0x7d,0x59,0x6a,0x35,0xaa,0x25,0xd5,0xd3,0x23,
     0xdd,0x3b,0x27,0x98,0x05,0x4c,0x2a,0xf6,0xc2,0x2e,0x84,0x1b,0x1c,0xcf,0x3d,0xc3,
     0xee,0x51,0x4f,0x86,0xd4,0xa0,0xce,0xf7,0xa6,0xf7,0xf5,0x66,0xae,0x44,0x8b,0x24,
     0xdc,0xc8,0xd1,0x1e,0xb7,0xa5,0x85,0xd4,0x49,0x23,0xea,0x1a,0x06,0xc7,0x74,0xa2,
     0xb3,0xeb,0x74,0x09,0xab,0x17,0xa0,0x06,0x5d,0x58,0x34,0xab,0x00,0x30,0x9a,0xd4,
     0x43,0x12,0xa7,0x31,0x72,0x59,0x21,0x95,0x43,0xe8,0x0d,0xdb,0x0c,0xc2,0xa4,0x38,
     0x1b,0xf6,0xe5,0x3c,0xd1,0xbb,0x35,0x7e,0xba,0x82,0xe1,0x1c,0x59,0xf8,0x2e,0x44,
     0x6c,0x4b,0x79,0x31,0x41,0x19,0x18,0x2c,0x0d,0xe9,0x6a,0x1b,0x5b,0xae,0x0b,0x08 },
    /* Qx */
    {0x73,0x3A,0xF3,0x78,0xD3,0x68,0x0B,0xA1,0xA1,0x6B,0x5F,0xBA,0xDD,0xD6,0x4E,0xB5,
     0x6B,0xF1,0x22,0x23,0x8B,0x8E,0x84,0x1E,0xC1,0x10,0xFE,0x15,0xD6,0x65,0xC0,0xC6,
     0xC6,0x58,0x1E,0xD2,0x3B,0x9D,0xCF,0x86,0xA5,0xAD,0x38,0x99,0x70,0x14,0x48,0xE8 },
    /* Qy */
    {0x3B,0x1B,0x6A,0xEF,0xF6,0xB1,0xA5,0x7A,0x8A,0x62,0x5B,0xED,0xA1,0x86,0x8E,0xDB,
     0x09,0xFB,0xB0,0xFA,0xDC,0xB3,0xE2,0xA4,0x76,0xAA,0x18,0x08,0xAB,0x1F,0xC2,0x6A,
     0x94,0xEC,0x95,0xD7,0x89,0x3E,0x33,0x33,0xB1,0xC9,0xE6,0x4F,0x8B,0xB6,0xEF,0xC8 },
    /* R */
    {0x57,0xBA,0x36,0xA4,0x13,0xB7,0x46,0x13,0x13,0xD5,0xBA,0x69,0x9B,0x4C,0xDF,0xAD,
     0xFC,0x0D,0xFD,0x45,0x54,0x91,0x09,0x50,0xF2,0x40,0xAC,0x6C,0xF4,0x24,0xD9,0x13,
     0xDB,0xA1,0xED,0xFF,0x54,0x85,0xD2,0xD5,0x3E,0x50,0x7C,0x44,0xCC,0x77,0xC2,0x91 },
    /* S */
    {0x7B,0xA4,0xD4,0xC2,0xEA,0x25,0xAB,0xC8,0x3F,0x1F,0x6F,0xA9,0x09,0x23,0xB3,0xFB,
     0x92,0x2B,0xF6,0xA6,0xA6,0x46,0xA7,0x36,0xD8,0xD3,0x91,0x7F,0xF3,0xC1,0x2D,0xDD,
     0x5A,0xDD,0x5F,0xEB,0x57,0xFF,0x7E,0x8B,0x25,0x81,0xC3,0x48,0xF6,0xF5,0x06,0xEE },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-8*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x13,0xc6,0x3a,0x3c,0xb6,0x1f,0x15,0xc6,0x59,0x72,0x06,0x58,0xa7,0x78,0x69,0x14,
     0x5a,0xe8,0xa1,0x76,0xc6,0xd9,0x3d,0x3a,0x8a,0xa9,0x94,0x62,0x36,0xd9,0xfb,0x04,
     0x63,0xdb,0x9e,0x48,0xc6,0x67,0xcb,0xa7,0x31,0xaf,0xaa,0x81,0x4b,0xa0,0xd5,0x83,
     0x57,0x52,0x4f,0x8d,0xe2,0x8d,0x4c,0x4b,0xbe,0x26,0x91,0xda,0xc9,0xb3,0x26,0x32,
     0xa7,0xdd,0x0f,0x99,0xfd,0x4c,0xb2,0x40,0x29,0x08,0x78,0x30,0x50,0x11,0xf7,0xd3,
     0xe3,0x7e,0xcc,0x41,0x0c,0xc1,0xfe,0xd6,0x01,0xe7,0x90,0x1e,0x8b,0xe6,0x41,0x4e,
     0xa4,0x43,0x17,0x58,0x48,0x43,0xa2,0xd2,0xca,0x2e,0x15,0x10,0x3e,0x1e,0xa4,0x93,
     0x65,0xbc,0x38,0x43,0x55,0xb3,0xc6,0xfa,0x6c,0xcd,0xd4,0x52,0x54,0x3e,0x97,0x69 },
    /* Qx */
    {0x71,0xB4,0xF9,0xFD,0x89,0xF6,0xC6,0x42,0x6F,0x86,0x4E,0x9A,0x2D,0x20,0xC9,0x43,
     0xF9,0x2E,0xAD,0xD3,0xAD,0x40,0x45,0x27,0x24,0xC0,0x9C,0xF2,0xCD,0x96,0x71,0x10,
     0xF4,0x5B,0xB4,0x72,0x9A,0xEC,0x8E,0x5D,0x3B,0x67,0xEF,0xA4,0x89,0x52,0xF0,0x91 },
    /* Qy */
    {0x44,0xE6,0xF5,0xC2,0xC0,0x59,0x7B,0x24,0x1C,0x2B,0xE4,0x67,0x17,0x39,0xBF,0xAA,
     0x81,0x69,0xB4,0xC0,0x65,0x72,0xE7,0x00,0x2E,0x7D,0x7D,0xCD,0x3E,0xB8,0xAB,0x7E,
     0x36,0x08,0x79,0x3A,0xF1,0xEE,0x19,0x93,0x14,0xE7,0x17,0xE0,0x48,0x2D,0xEE,0x4B },
    /* R */
    {0x05,0x89,0x8F,0x17,0x2B,0xF3,0xFD,0x20,0x17,0x06,0x8F,0x8A,0x9C,0xBF,0x46,0x6F,
     0x80,0xA4,0xB6,0x60,0x8A,0x2E,0x41,0xF9,0x60,0x58,0x31,0x41,0x26,0x3E,0xE5,0x22,
     0x56,0x29,0x6B,0x53,0xA7,0x5B,0x5A,0xC0,0x0F,0x27,0x48,0x94,0x49,0x25,0x41,0xD0 },
    /* S */
    {0x63,0xA7,0xE1,0xC1,0x34,0xF5,0xE0,0x17,0xE1,0x91,0x14,0xEC,0x5D,0x62,0xB5,0x79,
     0xAA,0x04,0xB3,0x60,0xBD,0xB6,0x0D,0xC3,0x1E,0xDC,0x4F,0xDB,0xA4,0x43,0x33,0xBA,
     0xEE,0xD5,0xA4,0x46,0xAE,0xA9,0x8D,0x9A,0x99,0x2A,0x75,0x0D,0xA8,0x78,0x66,0x14 },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-9*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x69,0x39,0xa9,0x11,0x8a,0xdc,0x30,0x71,0x07,0xaa,0x6b,0x00,0x57,0xc2,0x80,0xd1,
     0x0f,0xa4,0x4a,0x64,0x70,0x0c,0x7b,0xd2,0x3e,0x1f,0x33,0xa4,0x78,0xad,0x2c,0xfe,
     0x59,0x6c,0x05,0xf7,0x2b,0x54,0x0c,0xbd,0xb6,0x96,0xaa,0xc6,0xab,0x98,0xd9,0xca,
     0x8c,0x62,0xf3,0x3e,0x18,0x26,0x57,0x13,0x0b,0x83,0x17,0xa7,0x62,0x75,0xa5,0x99,
     0x63,0x33,0xa5,0xd3,0x54,0x7e,0x22,0x93,0xb4,0x01,0xd0,0xad,0xf6,0x0f,0x91,0xe9,
     0x1d,0x21,0x37,0xe3,0x4f,0x33,0x36,0xe0,0x17,0xc3,0xc6,0xdb,0xa6,0xbf,0x5b,0x13,
     0xdd,0x0d,0xe2,0x88,0xf9,0xb2,0x0a,0x89,0x6a,0x92,0xc4,0x8e,0x98,0x4f,0xbc,0x09,
     0xf9,0x20,0xfa,0xb8,0x2f,0x3f,0x91,0x5d,0x65,0x24,0xb0,0xc1,0x12,0x36,0xac,0xa4 },
    /* Qx */
    {0x1E,0xAA,0xF6,0xDA,0x96,0xCB,0x1E,0x08,0xB0,0xDF,0x74,0x91,0x44,0xD0,0x9F,0xCE,
     0x68,0x00,0x9D,0x54,0x5F,0xC1,0x60,0x19,0xD2,0xB1,0x3A,0x1E,0x9A,0x77,0x27,0x88,
     0x76,0x7D,0xF6,0xC7,0xB9,0x1A,0xA2,0x76,0x07,0xD4,0xD2,0xFF,0x8E,0x7F,0x1F,0x2F },
    /* Qy */
    {0x77,0xDC,0x80,0xE8,0x6E,0xD4,0x46,0x78,0x1E,0xBB,0x9B,0x1F,0x9E,0x2F,0x19,0x4F,
     0x0C,0xF5,0xFC,0x0F,0xC3,0x70,0xBF,0x43,0x3D,0xEB,0xA4,0x7B,0x32,0x94,0x9B,0xAA,
     0xAD,0xEA,0xDF,0x7E,0x39,0xC4,0xB6,0x44,0x98,0xB5,0x93,0xCE,0xD5,0x1D,0xF0,0xC6 },
    /* R */
    {0x81,0x73,0xFB,0x7A,0x9A,0xE7,0x3F,0x7A,0x63,0xD7,0xB8,0x5B,0xE2,0xB6,0xEF,0x98,
     0xEF,0xD5,0xFD,0x98,0x50,0x82,0x4E,0xAD,0x4C,0xE7,0x53,0x72,0xCE,0x72,0x3C,0xDA,
     0xA9,0x8D,0xE3,0x38,0x9E,0xCA,0xF2,0x24,0x0F,0x3E,0x26,0xBB,0x45,0x11,0x01,0x93 },
    /* S */
    {0x6E,0x85,0xC4,0x1B,0x90,0x8B,0x78,0x3C,0x7E,0x8A,0x46,0x84,0x24,0x3E,0xF9,0x54,
     0x73,0xBF,0xE8,0x16,0xB2,0xEA,0x2D,0x36,0xF9,0x23,0x27,0x09,0x3F,0xAA,0x94,0x34,
     0x77,0xE9,0xE3,0x33,0x98,0x86,0x0D,0xE5,0x6E,0x18,0x7B,0xDB,0x65,0x6C,0xA7,0x8D },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-10*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0xc8,0x20,0x71,0xe4,0x2c,0x45,0xac,0x35,0x97,0xf2,0x55,0xba,0x27,0x76,0x6a,0xfe,
     0x36,0x6e,0x31,0xa5,0x53,0xa4,0xd2,0x19,0x13,0x60,0xb8,0x8a,0x2a,0x34,0x9e,0xe0,
     0x77,0x29,0x14,0x54,0xbf,0x7b,0x32,0x3c,0xb3,0xc9,0xd7,0xfe,0xc5,0x53,0x3e,0x4e,
     0x4b,0xf4,0xfb,0x5b,0xc2,0xeb,0x16,0xc6,0x31,0x9e,0x93,0x78,0xa3,0xd8,0xa4,0x44,
     0xb2,0xd7,0x58,0x12,0x34,0x38,0xdb,0xb4,0x57,0xb2,0x6b,0x14,0xb6,0x54,0xb3,0xc8,
     0x8d,0x66,0x83,0x8a,0xdf,0xa6,0x73,0x06,0x7c,0x05,0x52,0xd1,0xb8,0xa3,0xad,0xe3,
     0xa9,0xcb,0x77,0x79,0x86,0xc0,0x0f,0x65,0xca,0xce,0x53,0xf8,0x52,0xc1,0x12,0x1a,
     0xcf,0x19,0x51,0x6a,0x7c,0xf0,0xba,0x38,0x20,0xb5,0xf5,0x1f,0x31,0xc5,0x39,0xa2 },
    /* Qx */
    {0x49,0x20,0xD2,0xDE,0xF7,0xAD,0xEC,0xAE,0x74,0x25,0x98,0x46,0x2D,0xE7,0xDB,0x0C,
     0xF6,0x32,0x47,0xEC,0xA2,0x28,0xA8,0xE6,0x47,0xEC,0xE2,0x8D,0x97,0x71,0x47,0xF5,
     0x51,0xE9,0xF9,0xBD,0x63,0x63,0xB6,0x6F,0x84,0x23,0xF7,0xB5,0xA8,0xDE,0x5A,0x3E },
    /* Qy */
    {0x57,0x46,0x61,0xD6,0x8A,0x38,0x23,0xE0,0x5B,0xAD,0x71,0x26,0x40,0x59,0xBD,0x6B,
     0xF0,0x1B,0x4A,0x95,0xB8,0x28,0xF4,0x80,0x8E,0x3B,0x74,0xB6,0xD5,0x45,0x72,0xDB,
     0xB1,0xFB,0x76,0x13,0xDD,0xE1,0x4F,0xD4,0xF3,0x59,0x5E,0xEF,0xC7,0x11,0xE5,0x56 },
    /* R */
    {0x4C,0x4A,0x42,0xE1,0xA6,0xB0,0xD6,0x51,0x7D,0x2A,0xA1,0xC0,0xB7,0xAB,0x4C,0xBC,
     0xC9,0x51,0x10,0xDF,0x04,0x40,0xD6,0x32,0x79,0xD5,0x38,0xE0,0x50,0xD1,0xE5,0xFB,
     0x7A,0xBA,0x69,0x42,0x11,0x93,0xF3,0x15,0x23,0x38,0xB8,0x12,0xAB,0x20,0xA2,0x61 },
    /* S */
    {0x7E,0x27,0x91,0xCB,0x3D,0x6B,0xC6,0x4F,0xBE,0xE2,0xB1,0xDF,0x3E,0x2B,0x26,0xB4,
     0x1B,0xF0,0xAE,0x06,0xA7,0x99,0xD6,0x7A,0x8E,0x17,0xB9,0x6A,0x00,0x60,0x41,0x5C,
     0x4A,0xFE,0xF3,0x8A,0xB8,0xAD,0x38,0x77,0xA8,0xE2,0x9B,0xD4,0x34,0x8A,0x5A,0xD0 },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-11*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x13,0x7b,0x21,0x5c,0x01,0x50,0xee,0x95,0xe8,0x49,0x4b,0x79,0x17,0x3d,0x7a,0xe3,
     0xc3,0xe7,0x1e,0xfc,0xc7,0xc7,0x5a,0xd9,0x2f,0x75,0x65,0x9c,0xe1,0xb2,0xd7,0xeb,
     0x55,0x5a,0xad,0x80,0x26,0x27,0x7a,0xe3,0x70,0x9f,0x46,0xe8,0x96,0x96,0x39,0x64,
     0x48,0x69,0x46,0xb9,0xfe,0x26,0x9d,0xf4,0x44,0xa6,0xea,0x28,0x9e,0xc2,0x28,0x5e,
     0x79,0x46,0xdb,0x57,0xff,0x18,0xf7,0x22,0xa5,0x83,0x19,0x4a,0x96,0x44,0xe8,0x63,
     0xae,0x45,0x2d,0x14,0x57,0xdc,0x5d,0xb7,0x2e,0xe2,0x0c,0x48,0x64,0x75,0xf3,0x58,
     0xdc,0x57,0x5c,0x62,0x1b,0x5a,0xb8,0x65,0xc6,0x62,0xe4,0x83,0x25,0x8c,0x71,0x91,
     0xb4,0xcc,0x21,0x8e,0x1f,0x9a,0xfe,0xeb,0x3e,0x1c,0xb9,0x78,0xce,0x96,0x57,0xdc },
    /* Qx */
    {0x44,0xAB,0x6E,0x04,0x53,0xF5,0x63,0xDC,0x83,0x32,0xDC,0x4E,0xCE,0xD7,0x6A,0x58,
     0xF6,0x61,0xA2,0x7A,0x3B,0xAB,0x8D,0x26,0x9A,0x53,0xE8,0xCF,0x01,0xFB,0xD0,0x42,
     0x85,0x69,0xAE,0xA8,0x19,0x67,0xEB,0xEF,0x7A,0xEC,0xDA,0xD3,0xED,0xD1,0xB7,0x3F },
    /* Qy */
    {0x0E,0x31,0xEB,0xFF,0xDC,0x7B,0x83,0x19,0xFC,0xCB,0x4A,0xFD,0x42,0x92,0xD6,0xDA,
     0x43,0xCC,0x74,0x19,0x2B,0xB1,0xC0,0x97,0x14,0x43,0x14,0x28,0xDE,0x90,0xEB,0x95,
     0x56,0x84,0x4A,0xBD,0xF7,0x1C,0xF0,0x08,0x06,0x39,0x80,0x12,0x72,0xF1,0x2B,0x42 },
    /* R */
    {0x00,0x30,0xDF,0xE5,0x84,0xFF,0x85,0x56,0xB8,0x67,0x69,0x70,0x4E,0x69,0x0C,0xE6,
     0xA8,0x87,0x3A,0xE6,0xA7,0x4F,0x11,0x36,0xD0,0x9F,0xB7,0xDE,0xE3,0xFE,0x77,0xC5,
     0x40,0xCB,0x9E,0xA8,0x90,0x0C,0x3F,0xC7,0x89,0xD6,0xA3,0xEC,0x20,0xBA,0x56,0xFD },
    /* S */
    {0x1F,0xCF,0x58,0xBE,0x5D,0xFC,0x01,0x5A,0x9D,0x04,0x18,0xF0,0x27,0x33,0x83,0x94,
     0x2D,0x77,0x95,0x60,0xD8,0xDE,0x3E,0x3E,0xFC,0xA3,0x8E,0x89,0x09,0x5D,0xE1,0x12,
     0xCB,0x97,0x4E,0xA5,0xBF,0x82,0x85,0xFA,0xF6,0x71,0xC8,0x94,0x0A,0xF2,0xC7,0xB8 },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-12*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x93,0xe7,0xe7,0x5c,0xfa,0xf3,0xfa,0x4e,0x71,0xdf,0x80,0xf7,0xf8,0xc0,0xef,0x66,
     0x72,0xa6,0x30,0xd2,0xdb,0xeb,0xa1,0xd6,0x13,0x49,0xac,0xba,0xaa,0x47,0x6f,0x5f,
     0x0e,0x34,0xdc,0xcb,0xd8,0x5b,0x9a,0x81,0x5d,0x90,0x82,0x03,0x31,0x3a,0x22,0xfe,
     0x3e,0x91,0x95,0x04,0xcb,0x22,0x2d,0x62,0x3a,0xd9,0x56,0x62,0xea,0x4a,0x90,0x09,
     0x97,0x42,0xc0,0x48,0x34,0x1f,0xe3,0xa7,0xa5,0x11,0x10,0xd3,0x0a,0xd3,0xa4,0x8a,
     0x77,0x7c,0x63,0x47,0xea,0x8b,0x71,0x74,0x93,0x16,0xe0,0xdd,0x19,0x02,0xfa,0xcb,
     0x30,0x4a,0x76,0x32,0x4b,0x71,0xf3,0x88,0x2e,0x6e,0x70,0x31,0x9e,0x13,0xfc,0x2b,
     0xb9,0xf3,0xf5,0xdb,0xb9,0xbd,0x2c,0xc7,0x26,0x5f,0x52,0xdf,0xc0,0xa3,0xbb,0x91 },
    /* Qx */
    {0x1C,0xB6,0x1C,0x3F,0xA8,0x1E,0x50,0x58,0xB7,0xD1,0x0B,0x72,0x97,0xD8,0xBD,0x68,
     0xC0,0x6B,0x62,0x52,0x2B,0x6E,0x6A,0xD3,0x74,0x7D,0x00,0x6E,0xA4,0x5F,0x5E,0x53,
     0x21,0x6B,0x71,0x76,0x28,0xA8,0x51,0x5A,0x6E,0x85,0x95,0x26,0xAF,0x1F,0x41,0x17 },
    /* Qy */
    {0x2A,0x6F,0xB1,0xD5,0x5E,0xC2,0x8D,0xBD,0xC2,0x35,0x88,0xBC,0x29,0x8F,0x99,0x12,
     0x82,0x1E,0xE3,0x62,0x3B,0x87,0xB9,0x41,0x91,0x87,0x08,0x2A,0x70,0xF2,0x92,0x4B,
     0xAF,0x34,0x0E,0xDE,0xFF,0x7F,0x26,0x5F,0x5A,0xFF,0xD0,0x4E,0x2E,0x38,0x0D,0xDA },
    /* R */
    {0x7C,0xC9,0x07,0xCC,0x3B,0xE3,0x38,0x0F,0x83,0xB0,0x90,0x11,0x56,0x76,0x28,0x5E,
     0xEF,0x8E,0x80,0x37,0xDF,0x2E,0xFC,0x7A,0xD1,0xC7,0xD6,0xF0,0x28,0x2B,0xD9,0x83,
     0x76,0x9D,0x20,0x91,0xBE,0x01,0xF8,0x31,0x43,0x2B,0x99,0x54,0xED,0xD7,0x5B,0x3C },
    /* S */
    {0x54,0x46,0x2C,0x35,0xBD,0x4D,0x4F,0x81,0xAE,0x9B,0xBD,0xBA,0x93,0x43,0xA9,0x65,
     0x5D,0x26,0x17,0xE6,0x91,0x22,0x75,0xCA,0xA5,0xB1,0x0F,0x72,0x8B,0xA1,0x94,0x4D,
     0x43,0x51,0x40,0x72,0xDF,0x8F,0x1E,0x27,0xAF,0xEA,0xA3,0x22,0xF9,0xDE,0x97,0xF2 },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-13*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x15,0x49,0x3a,0xa1,0x0c,0xfb,0x80,0x4b,0x3d,0x80,0x70,0x3c,0xa0,0x2a,0xf7,0xe2,
     0xcf,0xdc,0x67,0x14,0x47,0xd9,0xa1,0x71,0xb4,0x18,0xec,0xf6,0xca,0x48,0xb4,0x50,
     0x41,0x4a,0x28,0xe7,0xa0,0x58,0xa7,0x8a,0xb0,0x94,0x61,0x86,0xad,0x2f,0xe2,0x97,
     0xe1,0xb7,0xe2,0x0e,0x40,0x54,0x7c,0x74,0xf9,0x48,0x87,0xa0,0x0f,0x27,0xdd,0xe7,
     0xf7,0x8a,0x3c,0x15,0xeb,0x11,0x15,0xd7,0x04,0x97,0x2b,0x35,0xa2,0x7c,0xaf,0x8f,
     0x7c,0xdc,0xce,0x02,0xb9,0x6f,0x8a,0x72,0xd7,0x7f,0x36,0xa2,0x0d,0x3f,0x82,0x9e,
     0x91,0x5c,0xd3,0xbb,0x81,0xf9,0xc2,0x99,0x77,0x87,0xa7,0x36,0x16,0xed,0x5c,0xb0,
     0xe8,0x64,0x23,0x19,0x59,0xe0,0xb6,0x23,0xf1,0x2a,0x18,0xf7,0x79,0x59,0x9d,0x65 },
    /* Qx */
    {0x80,0x3F,0xC6,0x6A,0x2B,0x21,0x97,0xB5,0x72,0x4B,0xE6,0x86,0x23,0x91,0xBF,0xAF,
     0x6B,0x2A,0x80,0xA1,0x4C,0xF8,0x4F,0xD8,0xEF,0x79,0x76,0xC2,0xD8,0x76,0xAF,0x0B,
     0x86,0xF7,0x80,0x46,0xF1,0x5C,0x0F,0x01,0xC6,0x28,0x9E,0x1E,0x68,0x74,0x82,0x94 },
    /* Qy */
    {0x22,0xAF,0xAF,0xB1,0x5B,0x53,0xBE,0x87,0xE5,0xA2,0x1D,0x06,0x2E,0x1D,0xAF,0x85,
     0x77,0x95,0x56,0xD6,0x7A,0x87,0x69,0x98,0x1D,0x34,0x83,0x99,0x8A,0xEF,0xE9,0x99,
     0xD2,0x18,0x4D,0xEF,0xBC,0x44,0x98,0xD3,0xD7,0xE9,0xCF,0xEB,0x1A,0x55,0x62,0x46 },
    /* R */
    {0x2F,0xE9,0xB3,0xCF,0x04,0xFE,0x9B,0xAA,0x36,0xBD,0x78,0xCC,0x2D,0x7F,0x8A,0x52,
     0xA4,0xD6,0x02,0xE8,0x4F,0xEB,0x1C,0xDF,0x4F,0x5B,0x7D,0xE6,0x8C,0xFB,0xE7,0x6B,
     0xB4,0x6B,0x83,0xDD,0x6B,0xB1,0x06,0x57,0x01,0x54,0xC5,0xC6,0xFD,0xEC,0x35,0xBA },
    /* S */
    {0x31,0x5A,0x73,0xAC,0x1C,0x34,0x19,0xDA,0x61,0xFC,0x47,0x17,0x80,0x9E,0x4B,0x7C,
     0xDB,0x6A,0x92,0xB5,0xD3,0xCE,0x0E,0x2A,0x84,0xB5,0x37,0xBD,0xEB,0x12,0x88,0xDF,
     0x64,0x8C,0xCF,0xFF,0xC1,0xE0,0x93,0xFB,0x3D,0x92,0x32,0x3F,0x6D,0xC4,0x19,0x65 },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-14*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0xbc,0x55,0x82,0x96,0x78,0x88,0xa4,0x25,0xfb,0x75,0x7b,0xd4,0x96,0x59,0x00,0xf0,
     0x1e,0x66,0x95,0xd1,0x54,0x7e,0xd9,0x67,0xc1,0xd4,0xf6,0x7b,0x1b,0x1d,0xe3,0x65,
     0xd2,0x03,0xf4,0x07,0x69,0x87,0x61,0x69,0x9f,0xec,0x5f,0x5a,0x61,0x4c,0x21,0xe3,
     0x6a,0x9f,0x57,0xa8,0xaa,0xf8,0x52,0xe9,0x55,0x38,0xf5,0x61,0x57,0x85,0x53,0x45,
     0x68,0x81,0x1a,0x9a,0x9c,0xcc,0x34,0x98,0x43,0xf6,0xc1,0x6d,0xc9,0x0a,0x4a,0xc9,
     0x6a,0x8f,0x72,0xc3,0x3d,0x95,0x89,0xa8,0x60,0xf4,0x98,0x1d,0x7b,0x4e,0xe7,0x17,
     0x3d,0x1d,0xb5,0xd4,0x9c,0x43,0x61,0x36,0x85,0x04,0xc9,0xa6,0xcb,0xba,0xed,0xc2,
     0xc9,0xbf,0xf2,0xb1,0x28,0x84,0x37,0x9b,0xa9,0x04,0x33,0x69,0x8c,0xeb,0x88,0x1d },
    /* Qx */
    {0x8B,0xE6,0xA6,0x8F,0x9D,0x25,0x5F,0xFF,0x5E,0x14,0xCF,0xCE,0xBB,0x4A,0xDC,0xAB,
     0x12,0x1A,0x87,0x63,0x75,0x65,0x21,0x77,0x73,0x6D,0xAB,0xAB,0x77,0x75,0xA3,0x26,
     0x79,0xEF,0x2A,0xE3,0x8F,0x44,0xD9,0xA1,0x6D,0xD3,0x0A,0x01,0x58,0x49,0xD6,0xA6 },
    /* Qy */
    {0x4C,0x28,0x75,0x91,0x89,0x92,0xAB,0x32,0x09,0xF9,0x98,0x6E,0xA6,0x80,0xEA,0xD3,
     0x03,0x1D,0x8B,0xA8,0x58,0x72,0x25,0x96,0x8E,0x82,0x79,0x31,0xAA,0xBD,0xF8,0x39,
     0x2B,0xBA,0x1E,0x36,0x21,0x6B,0xB3,0xF8,0x96,0xC0,0x28,0x82,0x8F,0xE6,0xFB,0x08 },
    /* R */
    {0x2B,0x09,0x31,0x67,0x54,0x37,0xCA,0xDD,0xEB,0xE0,0x12,0x02,0x9B,0x53,0x57,0x29,
     0x23,0x5B,0x3B,0xF4,0xC3,0x7C,0xC0,0xC4,0x8F,0x6C,0xA3,0xBC,0xC7,0x29,0x4A,0xC8,
     0x62,0x9A,0xA8,0x52,0xFF,0x37,0x8A,0xF1,0xBA,0x8B,0x3E,0x09,0x5F,0x05,0xEB,0x9A },
    /* S */
    {0x50,0xB5,0xEA,0xD5,0xE9,0x54,0x44,0x81,0xD1,0x13,0xCA,0x10,0x40,0xA3,0xBB,0xAD,
     0x05,0xCF,0x09,0x0A,0x2F,0xC9,0xD0,0x08,0xF3,0xC7,0xEA,0xBF,0x8B,0x36,0xD2,0x7C,
     0x6E,0xC4,0xEB,0x09,0xD9,0x8D,0xC9,0x3D,0xEF,0xC3,0x0C,0xDB,0xC3,0xD4,0x53,0x6E },
    /* Result */
    TEST_PASS
  },

  { /* TV 19-15*/
    C_BRAINPOOL_P384R1, E_SHA384,
    /* Msg */
    {0x4f,0x31,0x33,0x1e,0x20,0xa3,0x27,0x3d,0xa8,0xfc,0xe6,0xb0,0x3f,0x2a,0x86,0x71,
     0x2e,0xd5,0xdf,0x41,0x12,0x0a,0x81,0xe9,0x94,0xd2,0xb2,0xf3,0x70,0xe9,0x8e,0xf3,
     0x5b,0x84,0x7f,0x30,0x47,0xd3,0xcf,0x57,0xe8,0x83,0x50,0xe2,0x7b,0x9a,0xc3,0xf0,
     0x20,0x73,0xac,0x18,0x38,0xdb,0x25,0xb5,0xad,0x47,0x7a,0xee,0x68,0x93,0x08,0x82,
     0x30,0x4f,0xc0,0x52,0xf2,0x73,0x82,0x10,0x56,0xdf,0x75,0x00,0xdc,0x9e,0xab,0x03,
     0x7e,0xd3,0xac,0x3c,0x75,0x39,0x6e,0x31,0x3b,0xf0,0xf4,0xb8,0x9b,0x26,0x67,0x5a,
     0xf5,0x5f,0x33,0x78,0xcf,0x09,0x9d,0x9d,0x9a,0x25,0xa4,0x88,0x7c,0x1c,0xfd,0x24,
     0x48,0xf5,0xb2,0x18,0x8c,0x41,0xd6,0xfa,0x26,0x04,0x5c,0x5e,0x97,0x4b,0xf3,0xe4 },
    /* Qx */
    {0x3E,0x1F,0xA9,0x0A,0xF6,0xFE,0x05,0xDE,0x0A,0x61,0xBE,0xA7,0x0F,0xC0,0xBE,0x32,
     0x94,0xC3,0x0B,0x91,0x81,0x0A,0xD5,0x0E,0xF3,0x12,0xD2,0xD2,0xAC,0x80,0x29,0x5C,
     0x4F,0x23,0xCE,0xBE,0x2A,0x89,0x7F,0x93,0xC7,0x8C,0x17,0x8F,0xFE,0x66,0xFF,0xA2 },
    /* Qy */
    {0x75,0xAB,0xD9,0x46,0x3A,0x2E,0xD7,0x34,0xD2,0x22,0xA5,0xAE,0xDF,0xF7,0xCC,0xD3,
     0xF9,0xC4,0xC0,0xE4,0x3C,0x88,0x37,0x43,0xED,0xED,0x29,0xC4,0xBE,0x81,0x4D,0xCB,
     0xEE,0x0F,0x2E,0x60,0xFB,0x5D,0x25,0x71,0x4E,0x10,0xAE,0xE9,0x2C,0xE1,0xD0,0x60 },
    /* R */
    {0x48,0x22,0xA9,0x24,0x72,0x14,0xCD,0x2D,0x63,0xEC,0x0B,0xB6,0xB7,0xE8,0x3F,0xAD,
     0x17,0x39,0x6D,0xDF,0xB7,0xBB,0x82,0x7E,0x3B,0x64,0x59,0x15,0x3E,0xED,0xF2,0x71,
     0x82,0xD3,0x3F,0x87,0x26,0x90,0x3E,0xAF,0x55,0x21,0xDA,0x8D,0x9E,0x0F,0x1B,0x3A },
    /* S */
    {0x69,0x5E,0xD2,0x8A,0x34,0x1D,0xB4,0x4F,0xDB,0x2C,0xB3,0xF8,0xD7,0xEB,0x79,0x3B,
     0x48,0x47,0xCD,0xDA,0x86,0x1E,0x5B,0x1B,0x9F,0xF3,0xF3,0xDC,0xA9,0x08,0xB3,0x1B,
     0x7A,0xFB,0xB5,0xE0,0xB5,0x70,0xFE,0xE9,0x40,0xE5,0x91,0x8F,0xD4,0x8E,0xF1,0xD4 },
    /* Result */
    TEST_PASS
  }
,
#endif
};
#endif
