/*# Test vectors for brainpoolP384r1 with SHA384
 *# generated by SAGE from message and private key coming from CAVS 11.2 */

#include "config.h"
#include "CSE_ext_hash.h"
#include "CSE_ext_ECC_ECDSA_SignGenVerif_TV.h"

/* Test Vector for ECDSA Signature generation and verification in the same test.
 * Generate signature and verify it in the same function.  */
#ifdef INCLUDE_BRAINPOOL_P256R1
const signVerify_test_vect_stt signVerify_test_vect_array_brainpool256r1[NB_OF_ECDSA_SIGN_VERIFY_TV_brainpoolP256r1_SHA256] =
{
  { /* TV 3-1*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x59,0x05,0x23,0x88,0x77,0xc7,0x74,0x21,0xf7,0x3e,0x43,0xee,0x3d,0xa6,0xf2,0xd9,
     0xe2,0xcc,0xad,0x5f,0xc9,0x42,0xdc,0xec,0x0c,0xbd,0x25,0x48,0x29,0x35,0xfa,0xaf,
     0x41,0x69,0x83,0xfe,0x16,0x5b,0x1a,0x04,0x5e,0xe2,0xbc,0xd2,0xe6,0xdc,0xa3,0xbd,
     0xf4,0x6c,0x43,0x10,0xa7,0x46,0x1f,0x9a,0x37,0x96,0x0c,0xa6,0x72,0xd3,0xfe,0xb5,
     0x47,0x3e,0x25,0x36,0x05,0xfb,0x1d,0xdf,0xd2,0x80,0x65,0xb5,0x3c,0xb5,0x85,0x8a,
     0x8a,0xd2,0x81,0x75,0xbf,0x9b,0xd3,0x86,0xa5,0xe4,0x71,0xea,0x7a,0x65,0xc1,0x7c,
     0xc9,0x34,0xa9,0xd7,0x91,0xe9,0x14,0x91,0xeb,0x37,0x54,0xd0,0x37,0x99,0x79,0x0f,
     0xe2,0xd3,0x08,0xd1,0x61,0x46,0xd5,0xc9,0xb0,0xd0,0xde,0xbd,0x97,0xd7,0x9c,0xe8 },
    /* d */
    {0x51,0x9b,0x42,0x3d,0x71,0x5f,0x8b,0x58,0x1f,0x4f,0xa8,0xee,0x59,0xf4,0x77,0x1a,
     0x5b,0x44,0xc8,0x13,0x0b,0x4e,0x3e,0xac,0xca,0x54,0xa5,0x6d,0xda,0x72,0xb4,0x64 },
    /* Qx */
    {0x8A,0x89,0x17,0x02,0x53,0x70,0xF4,0x05,0x67,0xCA,0xF3,0xF3,0x72,0x81,0x46,0x22,
     0x4D,0x33,0x48,0xD3,0x3F,0xE1,0x2A,0xA2,0x16,0x66,0x64,0x5B,0xD0,0x9B,0x0B,0x29 },
    /* Qy */
    {0x50,0x30,0xC0,0xB8,0xDD,0x6B,0x62,0x1C,0xFC,0x4A,0xF9,0xBD,0x73,0xE8,0x0D,0xB9,
     0xF6,0x02,0x53,0xEA,0x86,0x92,0xB1,0xB4,0x49,0xCE,0x19,0xC4,0xF7,0xFC,0xE2,0x00 },
  },

  { /* TV 3-2*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0xc3,0x5e,0x2f,0x09,0x25,0x53,0xc5,0x57,0x72,0x92,0x6b,0xdb,0xe8,0x7c,0x97,0x96,
     0x82,0x7d,0x17,0x02,0x4d,0xbb,0x92,0x33,0xa5,0x45,0x36,0x6e,0x2e,0x59,0x87,0xdd,
     0x34,0x4d,0xeb,0x72,0xdf,0x98,0x71,0x44,0xb8,0xc6,0xc4,0x3b,0xc4,0x1b,0x65,0x4b,
     0x94,0xcc,0x85,0x6e,0x16,0xb9,0x6d,0x7a,0x82,0x1c,0x8e,0xc0,0x39,0xb5,0x03,0xe3,
     0xd8,0x67,0x28,0xc4,0x94,0xa9,0x67,0xd8,0x30,0x11,0xa0,0xe0,0x90,0xb5,0xd5,0x4c,
     0xd4,0x7f,0x4e,0x36,0x6c,0x09,0x12,0xbc,0x80,0x8f,0xbb,0x2e,0xa9,0x6e,0xfa,0xc8,
     0x8f,0xb3,0xeb,0xec,0x93,0x42,0x73,0x8e,0x22,0x5f,0x7c,0x7c,0x2b,0x01,0x1c,0xe3,
     0x75,0xb5,0x66,0x21,0xa2,0x06,0x42,0xb4,0xd3,0x6e,0x06,0x0d,0xb4,0x52,0x4a,0xf1 },
    /* d */
    {0x0f,0x56,0xdb,0x78,0xca,0x46,0x0b,0x05,0x5c,0x50,0x00,0x64,0x82,0x4b,0xed,0x99,
     0x9a,0x25,0xaa,0xf4,0x8e,0xbb,0x51,0x9a,0xc2,0x01,0x53,0x7b,0x85,0x47,0x98,0x13 },
    /* Qx */
    {0x29,0x70,0xBA,0xAC,0x06,0x94,0xE4,0xA2,0x79,0x81,0x73,0xD0,0x4E,0x77,0xF3,0xEB,
     0xE8,0xD2,0xB2,0x53,0xF6,0xB4,0x4A,0x56,0xE0,0x12,0x93,0x7A,0x1E,0xBF,0x40,0x20 },
    /* Qy */
    {0x02,0xA2,0xCF,0x27,0xC3,0xA1,0xAF,0xCA,0xE3,0x4E,0x94,0x75,0x80,0x0B,0x41,0x2B,
     0x39,0x9C,0x5A,0x11,0xAF,0xC6,0x0B,0xAD,0xC3,0xBD,0x74,0x83,0x4A,0x89,0x39,0x4D },
  },

  { /* TV 3-3*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x3c,0x05,0x4e,0x33,0x3a,0x94,0x25,0x9c,0x36,0xaf,0x09,0xab,0x5b,0x4f,0xf9,0xbe,
     0xb3,0x49,0x2f,0x8d,0x5b,0x42,0x82,0xd1,0x68,0x01,0xda,0xcc,0xb2,0x9f,0x70,0xfe,
     0x61,0xa0,0xb3,0x7f,0xfe,0xf5,0xc0,0x4c,0xd1,0xb7,0x0e,0x85,0xb1,0xf5,0x49,0xa1,
     0xc4,0xdc,0x67,0x29,0x85,0xe5,0x0f,0x43,0xea,0x03,0x7e,0xfa,0x99,0x64,0xf0,0x96,
     0xb5,0xf6,0x2f,0x7f,0xfd,0xf8,0xd6,0xbf,0xb2,0xcc,0x85,0x95,0x58,0xf5,0xa3,0x93,
     0xcb,0x94,0x9d,0xbd,0x48,0xf2,0x69,0x34,0x3b,0x52,0x63,0xdc,0xdb,0x9c,0x55,0x6e,
     0xca,0x07,0x4f,0x2e,0x98,0xe6,0xd9,0x4c,0x2c,0x29,0xa6,0x77,0xaf,0xaf,0x80,0x6e,
     0xdf,0x79,0xb1,0x5a,0x3f,0xcd,0x46,0xe7,0x06,0x7b,0x76,0x69,0xf8,0x31,0x88,0xee },
    /* d */
    {0xa2,0x83,0x87,0x12,0x39,0x83,0x7e,0x13,0xb9,0x5f,0x78,0x9e,0x6e,0x1a,0xf6,0x3b,
     0xf6,0x1c,0x91,0x8c,0x99,0x2e,0x62,0xbc,0xa0,0x40,0xd6,0x4c,0xad,0x1f,0xc2,0xef },
    /* Qx */
    {0x2B,0x40,0xED,0x3F,0x96,0x33,0xA3,0x55,0x97,0x1F,0x0D,0xCB,0xD3,0xC9,0x0D,0xEA,
     0x84,0x78,0xC7,0x1C,0x93,0x2D,0x26,0x7F,0x8E,0xAC,0x97,0x1D,0xDE,0xA0,0x51,0xC5 },
    /* Qy */
    {0x14,0xD1,0x80,0xC8,0x6E,0x87,0x06,0x5D,0x03,0x22,0xAA,0x38,0xD3,0xE7,0x89,0x94,
     0x6C,0x37,0xD6,0x46,0xD2,0x55,0x06,0x71,0xBB,0x21,0x56,0x77,0x68,0xD8,0x57,0x24 },
  },

  { /* TV 3-4*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x09,0x89,0x12,0x24,0x10,0xd5,0x22,0xaf,0x64,0xce,0xb0,0x7d,0xa2,0xc8,0x65,0x21,
     0x90,0x46,0xb4,0xc3,0xd9,0xd9,0x9b,0x01,0x27,0x8c,0x07,0xff,0x63,0xea,0xf1,0x03,
     0x9c,0xb7,0x87,0xae,0x9e,0x2d,0xd4,0x64,0x36,0xcc,0x04,0x15,0xf2,0x80,0xc5,0x62,
     0xbe,0xbb,0x83,0xa2,0x3e,0x63,0x9e,0x47,0x6a,0x02,0xec,0x8c,0xff,0x7e,0xa0,0x6c,
     0xd1,0x2c,0x86,0xdc,0xc3,0xad,0xef,0xbf,0x1a,0x9e,0x9a,0x9b,0x66,0x46,0xc7,0x59,
     0x9e,0xc6,0x31,0xb0,0xda,0x9a,0x60,0xde,0xbe,0xb9,0xb3,0xe1,0x93,0x24,0x97,0x7f,
     0x3b,0x4f,0x36,0x89,0x2c,0x8a,0x38,0x67,0x1c,0x8e,0x1c,0xc8,0xe5,0x0f,0xcd,0x50,
     0xf9,0xe5,0x1d,0xea,0xf9,0x82,0x72,0xf9,0x26,0x6f,0xc7,0x02,0xe4,0xe5,0x7c,0x30 },
    /* d */
    {0xa3,0xd2,0xd3,0xb7,0x59,0x6f,0x65,0x92,0xce,0x98,0xb4,0xbf,0xe1,0x0d,0x41,0x83,
     0x7f,0x10,0x02,0x7a,0x90,0xd7,0xbb,0x75,0x34,0x94,0x90,0x01,0x8c,0xf7,0x2d,0x07 },
    /* Qx */
    {0x8C,0xE4,0xA2,0xF1,0xE4,0xD7,0x94,0xBB,0x34,0xF4,0x9B,0xFC,0x07,0xC4,0x19,0x21,
     0xF6,0xAE,0xB7,0xB5,0xDB,0xD0,0x11,0x54,0x4D,0x4E,0x91,0xB9,0xEA,0x0E,0xD4,0xA0 },
    /* Qy */
    {0x4C,0x7A,0x3C,0xB6,0xE2,0x0F,0x7F,0xD6,0x13,0xA6,0x9E,0x47,0x59,0x38,0x80,0x8D,
     0x19,0xD3,0x34,0x0C,0x65,0x66,0x8D,0x31,0x28,0xB5,0x42,0xB6,0xA1,0x8E,0x16,0xE9 },
  },

  { /* TV 3-5*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0xdc,0x66,0xe3,0x9f,0x9b,0xbf,0xd9,0x86,0x53,0x18,0x53,0x1f,0xfe,0x92,0x07,0xf9,
     0x34,0xfa,0x61,0x5a,0x5b,0x28,0x57,0x08,0xa5,0xe9,0xc4,0x6b,0x77,0x75,0x15,0x0e,
     0x81,0x8d,0x7f,0x24,0xd2,0xa1,0x23,0xdf,0x36,0x72,0xff,0xf2,0x09,0x4e,0x3f,0xd3,
     0xdf,0x6f,0xbe,0x25,0x9e,0x39,0x89,0xdd,0x5e,0xdf,0xcc,0xcb,0xe7,0xd4,0x5e,0x26,
     0xa7,0x75,0xa5,0xc4,0x32,0x9a,0x08,0x4f,0x05,0x7c,0x42,0xc1,0x3f,0x32,0x48,0xe3,
     0xfd,0x6f,0x0c,0x76,0x67,0x8f,0x89,0x0f,0x51,0x3c,0x32,0x29,0x2d,0xd3,0x06,0xea,
     0xa8,0x4a,0x59,0xab,0xe3,0x4b,0x16,0xcb,0x5e,0x38,0xd0,0xe8,0x85,0x52,0x5d,0x10,
     0x33,0x6c,0xa4,0x43,0xe1,0x68,0x2a,0xa0,0x4a,0x7a,0xf8,0x32,0xb0,0xee,0xe4,0xe7 },
    /* d */
    {0x53,0xa0,0xe8,0xa8,0xfe,0x93,0xdb,0x01,0xe7,0xae,0x94,0xe1,0xa9,0x88,0x2a,0x10,
     0x2e,0xbd,0x07,0x9b,0x3a,0x53,0x58,0x27,0xd5,0x83,0x62,0x6c,0x27,0x2d,0x28,0x0d },
    /* Qx */
    {0x6D,0x73,0x69,0x72,0xAC,0xBE,0x11,0xD9,0xA7,0xA6,0x2A,0x71,0xC2,0x8D,0x5E,0x1A,
     0xBA,0xCA,0xA7,0xCC,0x4F,0xC2,0xFB,0xDD,0x54,0xE6,0xF4,0x2D,0x05,0x6C,0xDA,0xDB },
    /* Qy */
    {0x7F,0x1D,0xAB,0xA9,0x5C,0xC8,0x11,0xFB,0xBA,0x45,0xFD,0x2F,0x45,0x66,0xAA,0xF9,
     0xCA,0x44,0x51,0xDE,0xF5,0xF7,0x95,0xD6,0x43,0x23,0x6F,0x3C,0x60,0xB3,0xE7,0xA3 },
  },

  { /* TV 3-6*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x60,0x09,0x74,0xe7,0xd8,0xc5,0x50,0x8e,0x2c,0x1a,0xab,0x07,0x83,0xad,0x0d,0x7c,
     0x44,0x94,0xab,0x2b,0x4d,0xa2,0x65,0xc2,0xfe,0x49,0x64,0x21,0xc4,0xdf,0x23,0x8b,
     0x0b,0xe2,0x5f,0x25,0x65,0x91,0x57,0xc8,0xa2,0x25,0xfb,0x03,0x95,0x36,0x07,0xf7,
     0xdf,0x99,0x6a,0xcf,0xd4,0x02,0xf1,0x47,0xe3,0x7a,0xee,0x2f,0x16,0x93,0xe3,0xbf,
     0x1c,0x35,0xea,0xb3,0xae,0x36,0x0a,0x2b,0xd9,0x1d,0x04,0x62,0x2e,0xa4,0x7f,0x83,
     0xd8,0x63,0xd2,0xdf,0xec,0xb6,0x18,0xe8,0xb8,0xbd,0xc3,0x9e,0x17,0xd1,0x5d,0x67,
     0x2e,0xee,0x03,0xbb,0x4c,0xe2,0xcc,0x5c,0xf6,0xb2,0x17,0xe5,0xfa,0xf3,0xf3,0x36,
     0xfd,0xd8,0x7d,0x97,0x2d,0x3a,0x8b,0x8a,0x59,0x3b,0xa8,0x59,0x55,0xcc,0x9d,0x71 },
    /* d */
    {0x4a,0xf1,0x07,0xe8,0xe2,0x19,0x4c,0x83,0x0f,0xfb,0x71,0x2a,0x65,0x51,0x1b,0xc9,
     0x18,0x6a,0x13,0x30,0x07,0x85,0x5b,0x49,0xab,0x4b,0x38,0x33,0xae,0xfc,0x4a,0x1d },
    /* Qx */
    {0x73,0xF3,0x07,0xE4,0x8F,0x4F,0xDA,0x79,0xA1,0xD5,0x7B,0x0E,0xA6,0x59,0x3E,0xE5,
     0x71,0xCD,0xE9,0x1A,0x39,0xD9,0x75,0xCF,0x70,0x9B,0xFD,0x1C,0xF1,0x0F,0xC6,0x6D },
    /* Qy */
    {0x90,0x97,0xCE,0xE2,0x07,0x7D,0x2F,0xB7,0x09,0x2D,0xAC,0x98,0x90,0x36,0x0C,0x82,
     0x76,0x4D,0xDB,0xB4,0x8D,0x16,0xA5,0x8E,0x68,0xD6,0x60,0x61,0x13,0x32,0x0B,0x9B },
  },

  { /* TV 3-7*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0xdf,0xa6,0xcb,0x9b,0x39,0xad,0xda,0x6c,0x74,0xcc,0x8b,0x2a,0x8b,0x53,0xa1,0x2c,
     0x49,0x9a,0xb9,0xde,0xe0,0x1b,0x41,0x23,0x64,0x2b,0x4f,0x11,0xaf,0x33,0x6a,0x91,
     0xa5,0xc9,0xce,0x05,0x20,0xeb,0x23,0x95,0xa6,0x19,0x0e,0xcb,0xf6,0x16,0x9c,0x4c,
     0xba,0x81,0x94,0x1d,0xe8,0xe7,0x6c,0x9c,0x90,0x8e,0xb8,0x43,0xb9,0x8c,0xe9,0x5e,
     0x0d,0xa2,0x9c,0x5d,0x43,0x88,0x04,0x02,0x64,0xe0,0x5e,0x07,0x03,0x0a,0x57,0x7c,
     0xc5,0xd1,0x76,0x38,0x71,0x54,0xea,0xba,0xe2,0xaf,0x52,0xa8,0x3e,0x85,0xc6,0x1c,
     0x7c,0x61,0xda,0x93,0x0c,0x9b,0x19,0xe4,0x5d,0x7e,0x34,0xc8,0x51,0x6d,0xc3,0xc2,
     0x38,0xfd,0xdd,0x6e,0x45,0x0a,0x77,0x45,0x5d,0x53,0x4c,0x48,0xa1,0x52,0x01,0x0b },
    /* d */
    {0x78,0xdf,0xaa,0x09,0xf1,0x07,0x68,0x50,0xb3,0xe2,0x06,0xe4,0x77,0x49,0x4c,0xdd,
     0xcf,0xb8,0x22,0xaa,0xa0,0x12,0x84,0x75,0x05,0x35,0x92,0xc4,0x8e,0xba,0xf4,0xab },
    /* Qx */
    {0x6C,0x72,0xC3,0x2A,0x05,0x47,0xED,0x1B,0x6E,0x61,0xD2,0x8F,0x07,0xC7,0x1F,0x79,
     0x63,0x62,0xAB,0xF1,0xE7,0xAF,0x59,0xC2,0x62,0x91,0x4D,0x92,0x12,0x3B,0x79,0xF3 },
    /* Qy */
    {0x4E,0x40,0xE7,0xD7,0x1E,0x13,0x66,0x13,0xAF,0x71,0x65,0x1B,0xF8,0x5F,0xA1,0x55,
     0x7C,0x8D,0xA6,0x96,0x0B,0x44,0xB0,0x4A,0x11,0xD0,0xFC,0xDF,0xE4,0x91,0x29,0xF4 },
  },

  { /* TV 3-8*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x51,0xd2,0x54,0x7c,0xbf,0xf9,0x24,0x31,0x17,0x4a,0xa7,0xfc,0x73,0x02,0x13,0x95,
     0x19,0xd9,0x80,0x71,0xc7,0x55,0xff,0x1c,0x92,0xe4,0x69,0x4b,0x58,0x58,0x7e,0xa5,
     0x60,0xf7,0x2f,0x32,0xfc,0x6d,0xd4,0xde,0xe7,0xd2,0x2b,0xb7,0x38,0x73,0x81,0xd0,
     0x25,0x6e,0x28,0x62,0xd0,0x64,0x4c,0xdf,0x2c,0x27,0x7c,0x5d,0x74,0x0f,0xa0,0x89,
     0x83,0x0e,0xb5,0x2b,0xf7,0x9d,0x1e,0x75,0xb8,0x59,0x6e,0xcf,0x0e,0xa5,0x8a,0x0b,
     0x9d,0xf6,0x1e,0x0c,0x97,0x54,0xbf,0xcd,0x62,0xef,0xab,0x6e,0xa1,0xbd,0x21,0x6b,
     0xf1,0x81,0xc5,0x59,0x3d,0xa7,0x9f,0x10,0x13,0x5a,0x9b,0xc6,0xe1,0x64,0xf1,0x85,
     0x4b,0xc8,0x85,0x97,0x34,0x34,0x1a,0xad,0x23,0x7b,0xa2,0x9a,0x81,0xa3,0xfc,0x8b },
    /* d */
    {0x80,0xe6,0x92,0xe3,0xeb,0x9f,0xcd,0x8c,0x7d,0x44,0xe7,0xde,0x9f,0x7a,0x59,0x52,
     0x68,0x64,0x07,0xf9,0x00,0x25,0xa1,0xd8,0x7e,0x52,0xc7,0x09,0x6a,0x62,0x61,0x8a },
    /* Qx */
    {0x9A,0xB8,0x34,0x8D,0x25,0xBE,0x3C,0xDC,0x94,0x1F,0x2A,0x6A,0x28,0x41,0x8D,0x33,
     0xA1,0xD8,0xC6,0xC4,0xCE,0x61,0x96,0x10,0x16,0x19,0x51,0x83,0x14,0xFA,0x97,0xB5 },
    /* Qy */
    {0x2E,0x1D,0xC8,0x20,0xC2,0x7C,0x65,0x79,0x93,0x46,0x1C,0x7C,0xE3,0xEF,0xC9,0x15,
     0x7B,0x1C,0x97,0xEC,0xF8,0xB8,0x0F,0x25,0xCA,0x02,0x49,0x55,0xD1,0x37,0x5F,0xA4 },
  },

  { /* TV 3-9*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x55,0x8c,0x2a,0xc1,0x30,0x26,0x40,0x2b,0xad,0x4a,0x0a,0x83,0xeb,0xc9,0x46,0x8e,
     0x50,0xf7,0xff,0xab,0x06,0xd6,0xf9,0x81,0xe5,0xdb,0x1d,0x08,0x20,0x98,0x06,0x5b,
     0xcf,0xf6,0xf2,0x1a,0x7a,0x74,0x55,0x8b,0x1e,0x86,0x12,0x91,0x4b,0x8b,0x5a,0x0a,
     0xa2,0x8e,0xd5,0xb5,0x74,0xc3,0x6a,0xc4,0xea,0x58,0x68,0x43,0x2a,0x62,0xbb,0x8e,
     0xf0,0x69,0x5d,0x27,0xc1,0xe3,0xce,0xaf,0x75,0xc7,0xb2,0x51,0xc6,0x5d,0xdb,0x26,
     0x86,0x96,0xf0,0x7c,0x16,0xd2,0x76,0x79,0x73,0xd8,0x5b,0xeb,0x44,0x3f,0x21,0x1e,
     0x64,0x45,0xe7,0xfe,0x5d,0x46,0xf0,0xdc,0xe7,0x0d,0x58,0xa4,0xcd,0x9f,0xe7,0x06,
     0x88,0xc0,0x35,0x68,0x8e,0xa8,0xc6,0xba,0xec,0x65,0xa5,0xfc,0x7e,0x2c,0x93,0xe8 },
    /* d */
    {0x5e,0x66,0x6c,0x0d,0xb0,0x21,0x4c,0x3b,0x62,0x7a,0x8e,0x48,0x54,0x1c,0xc8,0x4a,
     0x8b,0x6f,0xd1,0x5f,0x30,0x0d,0xa4,0xdf,0xf5,0xd1,0x8a,0xec,0x6c,0x55,0xb8,0x81 },
    /* Qx */
    {0x8E,0x17,0x85,0x62,0x43,0x83,0x4D,0x3C,0x6C,0x8D,0x0D,0xDA,0xDF,0x26,0xD1,0xE7,
     0xB4,0x7E,0x99,0x69,0x59,0x01,0x10,0x4B,0xC4,0xF8,0x3E,0x8E,0xB3,0x70,0x6C,0x4C },
    /* Qy */
    {0x1D,0x20,0x57,0xEE,0xC5,0x22,0x2B,0xA5,0x37,0xE8,0xF4,0x34,0xE1,0xEA,0xA5,0xE4,
     0x6D,0x36,0xF7,0x24,0x2C,0x3A,0x0F,0xEA,0x9E,0x1C,0xF7,0xF9,0x6B,0xE1,0xB3,0x56},
  },

  { /* TV 3-10*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x4d,0x55,0xc9,0x9e,0xf6,0xbd,0x54,0x62,0x16,0x62,0xc3,0xd1,0x10,0xc3,0xcb,0x62,
     0x7c,0x03,0xd6,0x31,0x13,0x93,0xb2,0x64,0xab,0x97,0xb9,0x0a,0x4b,0x15,0x21,0x4a,
     0x55,0x93,0xba,0x25,0x10,0xa5,0x3d,0x63,0xfb,0x34,0xbe,0x25,0x1f,0xac,0xb6,0x97,
     0xc9,0x73,0xe1,0x1b,0x66,0x5c,0xb7,0x92,0x0f,0x16,0x84,0xb0,0x03,0x1b,0x4d,0xd3,
     0x70,0xcb,0x92,0x7c,0xa7,0x16,0x8b,0x0b,0xf8,0xad,0x28,0x5e,0x05,0xe9,0xe3,0x1e,
     0x34,0xbc,0x24,0x02,0x47,0x39,0xfd,0xc1,0x0b,0x78,0x58,0x6f,0x29,0xef,0xf9,0x44,
     0x12,0x03,0x4e,0x3b,0x60,0x6e,0xd8,0x50,0xec,0x2c,0x19,0x00,0xe8,0xe6,0x81,0x51,
     0xfc,0x4a,0xee,0x5a,0xde,0xbb,0x06,0x6e,0xb6,0xda,0x4e,0xaa,0x56,0x81,0x37,0x8e },
    /* d */
    {0xa9,0x3f,0x45,0x52,0x71,0xc8,0x77,0xc4,0xd5,0x33,0x46,0x27,0xe3,0x7c,0x27,0x8f,
     0x68,0xd1,0x43,0x01,0x4b,0x0a,0x05,0xaa,0x62,0xf3,0x08,0xb2,0x10,0x1c,0x53,0x08 },
    /* Qx */
    {0x47,0x3A,0x19,0x41,0xB2,0x47,0x5A,0x9A,0x4C,0xE1,0x7B,0x6C,0x97,0x3C,0x6A,0x0B,
     0x3B,0x12,0xE3,0x48,0xE4,0x57,0x62,0x94,0xD0,0xAF,0x26,0xD3,0x25,0x94,0x6A,0xA3 },
    /* Qy */
    {0xA0,0x7F,0xDF,0xE7,0x0E,0x76,0x91,0x5A,0xD7,0x56,0x86,0xB7,0x88,0x76,0xA3,0x40,
     0x09,0x38,0xBB,0x89,0x25,0x97,0x68,0xCE,0x87,0xE0,0x9B,0x15,0xAF,0xF8,0x88,0x8C },
  },

  { /* TV 3-11*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0xf8,0x24,0x8a,0xd4,0x7d,0x97,0xc1,0x8c,0x98,0x4f,0x1f,0x5c,0x10,0x95,0x0d,0xc1,
     0x40,0x47,0x13,0xc5,0x6b,0x6e,0xa3,0x97,0xe0,0x1e,0x6d,0xd9,0x25,0xe9,0x03,0xb4,
     0xfa,0xdf,0xe2,0xc9,0xe8,0x77,0x16,0x9e,0x71,0xce,0x3c,0x7f,0xe5,0xce,0x70,0xee,
     0x42,0x55,0xd9,0xcd,0xc2,0x6f,0x69,0x43,0xbf,0x48,0x68,0x78,0x74,0xde,0x64,0xf6,
     0xcf,0x30,0xa0,0x12,0x51,0x2e,0x78,0x7b,0x88,0x05,0x9b,0xbf,0x56,0x11,0x62,0xbd,
     0xcc,0x23,0xa3,0x74,0x2c,0x83,0x5a,0xc1,0x44,0xcc,0x14,0x16,0x7b,0x1b,0xd6,0x72,
     0x7e,0x94,0x05,0x40,0xa9,0xc9,0x9f,0x3c,0xbb,0x41,0xfb,0x1d,0xcb,0x00,0xd7,0x6d,
     0xda,0x04,0x99,0x58,0x47,0xc6,0x57,0xf4,0xc1,0x9d,0x30,0x3e,0xb0,0x9e,0xb4,0x8a },
    /* d */
    {0xa2,0x0d,0x70,0x5d,0x9b,0xd7,0xc2,0xb8,0xdc,0x60,0x39,0x3a,0x53,0x57,0xf6,0x32,
     0x99,0x0e,0x59,0x9a,0x09,0x75,0x57,0x3a,0xc6,0x7f,0xd8,0x9b,0x49,0x18,0x79,0x06 },
    /* Qx */
    {0x82,0x76,0x17,0xAA,0xBD,0x6B,0x08,0x32,0x65,0x60,0x2C,0x79,0x19,0x24,0xE9,0x76,
     0x40,0xC5,0xE2,0x28,0xA2,0x0E,0xEA,0x3B,0xCB,0x4C,0x7B,0xE0,0xEF,0x41,0xD5,0x84 },
    /* Qy */
    {0x2B,0xE9,0x1D,0x2C,0x11,0xDA,0xF8,0x86,0x27,0xD8,0xB2,0x20,0xD8,0xD0,0xFA,0xFB,
     0x97,0x0D,0x6D,0x54,0x13,0x56,0x27,0x3D,0x75,0xDC,0xF6,0xBE,0x5F,0xAC,0x68,0xBC },
  },

  { /* TV 3-12*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x3b,0x6e,0xe2,0x42,0x59,0x40,0xb3,0xd2,0x40,0xd3,0x5b,0x97,0xb6,0xdc,0xd6,0x1e,
     0xd3,0x42,0x3d,0x8e,0x71,0xa0,0xad,0xa3,0x5d,0x47,0xb3,0x22,0xd1,0x7b,0x35,0xea,
     0x04,0x72,0xf3,0x5e,0xdd,0x1d,0x25,0x2f,0x87,0xb8,0xb6,0x5e,0xf4,0xb7,0x16,0x66,
     0x9f,0xc9,0xac,0x28,0xb0,0x0d,0x34,0xa9,0xd6,0x6a,0xd1,0x18,0xc9,0xd9,0x4e,0x7f,
     0x46,0xd0,0xb4,0xf6,0xc2,0xb2,0xd3,0x39,0xfd,0x6b,0xcd,0x35,0x12,0x41,0xa3,0x87,
     0xcc,0x82,0x60,0x90,0x57,0x04,0x8c,0x12,0xc4,0xec,0x3d,0x85,0xc6,0x61,0x97,0x5c,
     0x45,0xb3,0x00,0xcb,0x96,0x93,0x0d,0x89,0x37,0x0a,0x32,0x7c,0x98,0xb6,0x7d,0xef,
     0xaa,0x89,0x49,0x7a,0xa8,0xef,0x99,0x4c,0x77,0xf1,0x13,0x0f,0x75,0x2f,0x94,0xa4 },
    /* d */
    {0xa9,0x23,0x4b,0xeb,0xfb,0xc8,0x21,0x05,0x03,0x41,0xa3,0x7e,0x12,0x40,0xef,0xe5,
     0xe3,0x37,0x63,0xcb,0xbb,0x2e,0xf7,0x6a,0x1c,0x79,0xe2,0x47,0x24,0xe5,0xa5,0xe7 },
    /* Qx */
    {0x00,0x20,0x9F,0xE4,0x31,0x86,0x99,0xD0,0xE7,0xD1,0x29,0x34,0xF0,0xF0,0x57,0xAC,
     0xC1,0xB9,0x61,0x04,0xA0,0xC5,0x90,0xE6,0x89,0x97,0x22,0x06,0xBB,0xB1,0x02,0x4B },
    /* Qy */
    {0x5F,0x3C,0x97,0x1C,0x2A,0xC1,0x35,0xB5,0x11,0xC8,0xF7,0x45,0xDC,0x52,0x93,0xA2,
     0x75,0xD8,0xE5,0xF8,0x75,0x33,0xD1,0xB7,0x36,0xFA,0x50,0x98,0x5F,0x4F,0x25,0xFE },
  },

  { /* TV 3-13*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0xc5,0x20,0x4b,0x81,0xec,0x0a,0x4d,0xf5,0xb7,0xe9,0xfd,0xa3,0xdc,0x24,0x5f,0x98,
     0x08,0x2a,0xe7,0xf4,0xef,0xe8,0x19,0x98,0xdc,0xaa,0x28,0x6b,0xd4,0x50,0x7c,0xa8,
     0x40,0xa5,0x3d,0x21,0xb0,0x1e,0x90,0x4f,0x55,0xe3,0x8f,0x78,0xc3,0x75,0x7d,0x5a,
     0x5a,0x4a,0x44,0xb1,0xd5,0xd4,0xe4,0x80,0xbe,0x3a,0xfb,0x5b,0x39,0x4a,0x5d,0x28,
     0x40,0xaf,0x42,0xb1,0xb4,0x08,0x3d,0x40,0xaf,0xbf,0xe2,0x2d,0x70,0x2f,0x37,0x0d,
     0x32,0xdb,0xfd,0x39,0x2e,0x12,0x8e,0xa4,0x72,0x4d,0x66,0xa3,0x70,0x1d,0xa4,0x1a,
     0xe2,0xf0,0x3b,0xb4,0xd9,0x1b,0xb9,0x46,0xc7,0x96,0x94,0x04,0xcb,0x54,0x4f,0x71,
     0xeb,0x7a,0x49,0xeb,0x4c,0x4e,0xc5,0x57,0x99,0xbd,0xa1,0xeb,0x54,0x51,0x43,0xa7 },
    /* d */
    {0xa5,0x8f,0x52,0x11,0xdf,0xf4,0x40,0x62,0x6b,0xb5,0x6d,0x0a,0xd4,0x83,0x19,0x3d,
     0x60,0x6c,0xf2,0x1f,0x36,0xd9,0x83,0x05,0x43,0x32,0x72,0x92,0xf4,0xd2,0x5d,0x8c },
    /* Qx */
    {0x5C,0xAF,0xCD,0x21,0x90,0xFE,0x64,0x94,0xC6,0xFE,0x1B,0xF1,0x13,0xBF,0x62,0x88,
     0xAC,0xFB,0x71,0x44,0xA4,0xF4,0xEB,0xAB,0x1B,0x8E,0xE8,0x63,0x43,0x8A,0xD5,0x2D },
    /* Qy */
    {0x87,0xD8,0x89,0xB8,0x86,0x1B,0xE1,0x29,0x07,0xF0,0xFA,0x09,0xE9,0x3C,0x5B,0x79,
     0x39,0xB9,0x85,0x27,0x1A,0x38,0x6C,0xFF,0x1E,0xE4,0x50,0x88,0x04,0xF8,0xA5,0xCE },
  },

  { /* TV 3-14*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x72,0xe8,0x1f,0xe2,0x21,0xfb,0x40,0x21,0x48,0xd8,0xb7,0xab,0x03,0x54,0x9f,0x11,
     0x80,0xbc,0xc0,0x3d,0x41,0xca,0x59,0xd7,0x65,0x38,0x01,0xf0,0xba,0x85,0x3a,0xdd,
     0x1f,0x6d,0x29,0xed,0xd7,0xf9,0xab,0xc6,0x21,0xb2,0xd5,0x48,0xf8,0xdb,0xf8,0x97,
     0x9b,0xd1,0x66,0x08,0xd2,0xd8,0xfc,0x32,0x60,0xb4,0xeb,0xc0,0xdd,0x42,0x48,0x24,
     0x81,0xd5,0x48,0xc7,0x07,0x57,0x11,0xb5,0x75,0x96,0x49,0xc4,0x1f,0x43,0x9f,0xad,
     0x69,0x95,0x49,0x56,0xc9,0x32,0x68,0x41,0xea,0x64,0x92,0x95,0x68,0x29,0xf9,0xe0,
     0xdc,0x78,0x9f,0x73,0x63,0x3b,0x40,0xf6,0xac,0x77,0xbc,0xae,0x6d,0xfc,0x79,0x30,
     0xcf,0xe8,0x9e,0x52,0x6d,0x16,0x84,0x36,0x5c,0x5b,0x0b,0xe2,0x43,0x7f,0xdb,0x01 },
    /* d */
    {0x54,0xc0,0x66,0x71,0x1c,0xdb,0x06,0x1e,0xda,0x07,0xe5,0x27,0x5f,0x7e,0x95,0xa9,
     0x96,0x2c,0x67,0x64,0xb8,0x4f,0x6f,0x1f,0x3a,0xb5,0xa5,0x88,0xe0,0xa2,0xaf,0xb1 },
    /* Qx */
    {0x80,0x65,0x47,0x12,0x7D,0x6C,0x1D,0x7C,0x8A,0x60,0x9E,0x10,0xFD,0x2D,0xC9,0x9F,
     0x20,0x84,0xFD,0x22,0xCE,0x6E,0x6F,0x32,0xD3,0x69,0xFF,0x86,0x24,0xD6,0xBF,0xF4 },
    /* Qy */
    {0x4F,0x58,0x9C,0xEE,0x31,0xED,0xFD,0x5A,0x1A,0xB8,0x13,0x56,0xA4,0x11,0x7A,0x6E,
     0xEA,0x2D,0xC1,0x3F,0xA6,0x65,0x67,0x00,0x14,0x73,0xE3,0x93,0xFE,0x9F,0xDD,0x67 },
  },

  { /* TV 3-15*/
    C_BRAINPOOL_P256R1,
    E_SHA256,
    /* Msg */
    {0x21,0x18,0x8c,0x3e,0xdd,0x5d,0xe0,0x88,0xda,0xcc,0x10,0x76,0xb9,0xe1,0xbc,0xec,
     0xd7,0x9d,0xe1,0x00,0x3c,0x24,0x14,0xc3,0x86,0x61,0x73,0x05,0x4d,0xc8,0x2d,0xde,
     0x85,0x16,0x9b,0xaa,0x77,0x99,0x3a,0xdb,0x20,0xc2,0x69,0xf6,0x0a,0x52,0x26,0x11,
     0x18,0x28,0x57,0x8b,0xcc,0x7c,0x29,0xe6,0xe8,0xd2,0xda,0xe8,0x18,0x06,0x15,0x2c,
     0x8b,0xa0,0xc6,0xad,0xa1,0x98,0x6a,0x19,0x83,0xeb,0xee,0xc1,0x47,0x3a,0x73,0xa0,
     0x47,0x95,0xb6,0x31,0x9d,0x48,0x66,0x2d,0x40,0x88,0x1c,0x17,0x23,0xa7,0x06,0xf5,
     0x16,0xfe,0x75,0x30,0x0f,0x92,0x40,0x8a,0xa1,0xdc,0x6a,0xe4,0x28,0x8d,0x20,0x46,
     0xf2,0x3c,0x1a,0xa2,0xe5,0x4b,0x7f,0xb6,0x44,0x8a,0x0d,0xa9,0x22,0xbd,0x7f,0x34 },
    /* d */
    {0x34,0xfa,0x46,0x82,0xbf,0x6c,0xb5,0xb1,0x67,0x83,0xad,0xcd,0x18,0xf0,0xe6,0x87,
     0x9b,0x92,0x18,0x5f,0x76,0xd7,0xc9,0x20,0x40,0x9f,0x90,0x4f,0x52,0x2d,0xb4,0xb1 },
    /* Qx */
    {0x15,0xE1,0x52,0x8B,0xB2,0x58,0x95,0xD4,0x0C,0xB1,0x61,0xFD,0xB4,0x0C,0x0C,0x99,
     0x4C,0xC6,0x95,0x2A,0xCA,0xB0,0x98,0x52,0x30,0xD3,0x91,0xD1,0x7C,0x37,0xB6,0x3F },
    /* Qy */
    {0x5D,0x73,0x5F,0xF0,0x8A,0xBC,0xC0,0x19,0x7A,0x76,0x7A,0x02,0x7C,0x6A,0xBB,0x0D,
     0x9C,0xDF,0x9C,0xCF,0xF5,0xE0,0xDD,0x73,0x02,0x9F,0xE5,0xFD,0x6E,0xD8,0x1B,0x0D },
  }
};
#endif

