/*****************************************************************************
T5tic.h :    T5 Interpreted Code

DO NOT ALTER THIS !

(c) COPALP 2010
*****************************************************************************/

#ifndef _T5TIC_H_INCLUDED_
#define _T5TIC_H_INCLUDED_

/****************************************************************************/
/* data types */

#define T5C_BOOL            0x01
#define T5C_SINT            0x0a
#define T5C_USINT           0x0e
#define T5C_INT             0x0b
#define T5C_UINT            0x0f
#define T5C_DINT            0x02
#define T5C_UDINT           0x10
#define T5C_LINT            0x0c
#define T5C_ULINT           0x11
#define T5C_REAL            0x03
#define T5C_LREAL           0x0d
#define T5C_TIME            0x04
#define T5C_STRING          0x05
#define T5C_COMPLEX         0x7f    /* something else in CT                 */

#define T5C_EXTERN          0x80    /* mask for extern variables            */

/****************************************************************************/
/* BOOL */

#define T5C_NOTBOOL         0x03    /* BOOL: a := not b                     */
#define T5C_EQBOOL          0x23    /* BOOL: a := b == c                    */
#define T5C_NEBOOL          0x28    /* BOOL: a := b != c                    */
#define T5C_ORBOOL          0x29    /* BOOL: a := b OR c                    */
#define T5C_ANDBOOL         0x2a    /* BOOL: a := b AND c                   */
#define T5C_XORBOOL         0x2b    /* BOOL: a := b XOR c                   */
#define T5C_COPYBOOL        0x2c    /* BOOL/SINT: a := b                    */
#define T5C_ARGETBOOL       0x37    /* BOOL: a := b [ c ]                   */
#define T5C_ARPUTBOOL       0x3c    /* BOOL: a [ b ] := c                   */
#define T5C_REDGE           0xc5    /* BOOL: REDGE: a := b & !c; c := b     */
#define T5C_FEDGE           0xc4    /* BOOL: FEDGE: a := !b & c; c := b     */
#define T5C_SETBOOL         0x105   /* set bool to TRUE                     */
#define T5C_RESETBOOL       0x106   /* reset bool to FALSE                  */
#define T5C_LOCKBOOL        0x109   /* lock bool                            */
#define T5C_UNLOCKBOOL      0x10a   /* unlock bool                          */

#define T5C_O_NOTBOOL       0x70    /* <ifnolock> BOOL: a := not b          */
#define T5C_O_EQBOOL        0x8d    /* <ifnolock> BOOL: a := b == c         */
#define T5C_O_NEBOOL        0x92    /* <ifnolock> BOOL: a := b != c         */
#define T5C_O_ORBOOL        0x94    /* <ifnolock> BOOL: a := b OR c         */
#define T5C_O_ANDBOOL       0x95    /* <ifnolock> BOOL: a := b AND c        */
#define T5C_O_XORBOOL       0x96    /* <ifnolock> BOOL: a := b XOR c        */
#define T5C_O_COPYBOOL      0x97    /* <ifnolock> BOOL/SINT: a := b         */
#define T5C_O_AGETBOOL      0xa1    /* <ifnolock> BOOL: a := b [ c ]        */
#define T5C_O_APUTBOOL      0xa6    /* <ifnolock> BOOL: a [ b ] := c        */
#define T5C_O_REDGE         0xcc    /* <ifnolock> BOOL: REGDE               */
#define T5C_O_FEDGE         0xcb    /* <ifnolock> BOOL: FEDGE               */
#define T5C_O_SETBOOL       0x107   /* set bool to TRUE                     */
#define T5C_O_RESETBOOL     0x108   /* reset bool to FALSE                  */

/****************************************************************************/
/* SINT */

#define T5C_NEGSINT         0x179   /* SINT: a := - b                       */
#define T5C_MULSINT         0x17b   /* SINT: a := b * c                     */
#define T5C_DIVSINT         0x17d   /* SINT: a := b / c                     */
#define T5C_ADDSINT         0x17f   /* SINT: a := b + c                     */
#define T5C_SUBSINT         0x181   /* SINT: a := b - c                     */
#define T5C_GTSINT          0x185   /* SINT: a := b > c                     */
#define T5C_GESINT          0x189   /* SINT: a := b >= c                    */
#define T5C_EQSINT          0x18b   /* SINT: a := b == c                    */
#define T5C_NESINT          0x18d   /* SINT: a := b != c                    */
#define T5C_COPYSINT        0x18f   /* SINT: a := b                         */
#define T5C_ARGETSINT       0x191   /* SINT: a := b [ c ]                   */
#define T5C_ARPUTSINT       0x193   /* SINT: a [ b ] := c                   */
#define T5C_SETSINT         0x194   /* force 8 bit value                    */
#define T5C_LOCKSINT        0x196   /* lock 8 bit variable                  */
#define T5C_UNLOCKSINT      0x197   /* unlock 8 bit variable                */

#define T5C_O_NEGSINT       0x178   /* <ifnolock> SINT: a := - b            */
#define T5C_O_MULSINT       0x17a   /* <ifnolock> SINT: a := b * c          */
#define T5C_O_DIVSINT       0x17c   /* <ifnolock> SINT: a := b / c          */
#define T5C_O_ADDSINT       0x17e   /* <ifnolock> SINT: a := b + c          */
#define T5C_O_SUBSINT       0x180   /* <ifnolock> SINT: a := b - c          */
#define T5C_O_GTSINT        0x184   /* <ifnolock> SINT: a := b > c          */
#define T5C_O_GESINT        0x188   /* <ifnolock> SINT: a := b >= c         */
#define T5C_O_EQSINT        0x18a   /* <ifnolock> SINT: a := b == c         */
#define T5C_O_NESINT        0x18c   /* <ifnolock> SINT: a := b != c         */
#define T5C_O_COPYSINT      0x18e   /* <ifnolock> SINT: a := b              */
#define T5C_O_ARGETSINT     0x190   /* <ifnolock> SINT: a := b [ c ]        */
#define T5C_O_ARPUTSINT     0x192   /* <ifnolock> SINT: a [ b ] := c        */
#define T5C_O_SETSINT       0x195   /* force 8 bit value                    */

/****************************************************************************/
/* USINT */

#define T5C_GTUSINT         0x1e2   /* USINT: a := b > c                    */
#define T5C_GEUSINT         0x1e3   /* USINT: a := b >= c                   */
#define T5C_MULUSINT        0x1e4   /* USINT: a := b * c                    */
#define T5C_DIVUSINT        0x1e5   /* USINT: a := b / c                    */

#define T5C_O_GTUSINT       0x1e6   /* <ifnolock> USINT: a := b > c         */
#define T5C_O_GEUSINT       0x1e7   /* <ifnolock> USINT: a := b >= c        */
#define T5C_O_MULUSINT      0x1e8   /* <ifnolock> USINT: a := b * c         */
#define T5C_O_DIVUSINT      0x1e9   /* <ifnolock> USINT: a := b / c         */

/****************************************************************************/
/* INT */

#define T5C_NEGINT          0x15d   /* INT: a := - b                        */
#define T5C_MULINT          0x15f   /* INT: a := b * c                      */
#define T5C_DIVINT          0x161   /* INT: a := b / c                      */
#define T5C_ADDINT          0x163   /* INT: a := b + c                      */
#define T5C_SUBINT          0x165   /* INT: a := b - c                      */
#define T5C_GTINT           0x169   /* INT: a := b > c                      */
#define T5C_GEINT           0x16d   /* INT: a := b >= c                     */
#define T5C_EQINT           0x16f   /* INT: a := b == c                     */
#define T5C_NEINT           0x171   /* INT: a := b != c                     */
#define T5C_COPYINT         0x173   /* INT: a := b                          */
#define T5C_ARGETINT        0x175   /* INT: a := b [ c ]                    */
#define T5C_ARPUTINT        0x177   /* INT: a [ b ] := c                    */
#define T5C_SETINT          0x10b   /* force 16 bit value                   */
#define T5C_LOCKINT         0x10d   /* lock 16 bit variable                 */
#define T5C_UNLOCKINT       0x10e   /* unlock 16 bit variable               */

#define T5C_O_NEGINT        0x15c   /* <ifnolock> INT: a := - b             */
#define T5C_O_MULINT        0x15e   /* <ifnolock> INT: a := b * c           */
#define T5C_O_DIVINT        0x160   /* <ifnolock> INT: a := b / c           */
#define T5C_O_ADDINT        0x162   /* <ifnolock> INT: a := b + c           */
#define T5C_O_SUBINT        0x164   /* <ifnolock> INT: a := b - c           */
#define T5C_O_GTINT         0x168   /* <ifnolock> INT: a := b > c           */
#define T5C_O_GEINT         0x16c   /* <ifnolock> INT: a := b >= c          */
#define T5C_O_EQINT         0x16e   /* <ifnolock> INT: a := b == c          */
#define T5C_O_NEINT         0x170   /* <ifnolock> INT: a := b != c          */
#define T5C_O_COPYINT       0x172   /* <ifnolock> INT: a := b               */
#define T5C_O_ARGETINT      0x174   /* <ifnolock> INT: a := b [ c ]         */
#define T5C_O_ARPUTINT      0x176   /* <ifnolock> INT: a [ b ] := c         */
#define T5C_O_SETINT        0x10c   /* force 16 bit value                   */

/****************************************************************************/
/* UINT */

#define T5C_GTUINT          0x1ea   /* UINT: a := b > c                     */
#define T5C_GEUINT          0x1eb   /* UINT: a := b >= c                    */
#define T5C_MULUINT         0x1ec   /* UINT: a := b * c                     */
#define T5C_DIVUINT         0x1ed   /* UINT: a := b / c                     */

#define T5C_O_GTUINT        0x1ee   /* <ifnolock> UINT: a := b > c          */
#define T5C_O_GEUINT        0x1ef   /* <ifnolock> UINT: a := b >= c         */
#define T5C_O_MULUINT       0x1f0   /* <ifnolock> UINT: a := b * c          */
#define T5C_O_DIBUINT       0x1f1   /* <ifnolock> UINT: a := b / c          */

/****************************************************************************/
/* DINT */

#define T5C_NEGDINT         0x01    /* DINT: a := - b                       */
#define T5C_MULDINT         0x04    /* DINT: a := b * c                     */
#define T5C_DIVDINT         0x06    /* DINT: a := b / c                     */
#define T5C_ADDDINT         0x08    /* DINT: a := b + c                     */
#define T5C_SUBDINT         0x0c    /* DINT: a := b - c                     */
#define T5C_GTDINT          0x13    /* DINT: a := b > c                     */
#define T5C_GEDINT          0x1b    /* DINT: a := b >= c                    */
#define T5C_EQDINT          0x1f    /* DINT: a := b == c                    */
#define T5C_NEDINT          0x24    /* DINT: a := b != c                    */
#define T5C_COPYDINT        0x2d    /* DINT/REAL: a := b                    */
#define T5C_ARGETDINT       0x38    /* DINT: a := b [ c ]                   */
#define T5C_ARPUTDINT       0x3d    /* DINT: a [ b ] := c                   */
#define T5C_SETDINT         0x10f   /* force 32 bit value                   */
#define T5C_LOCKDINT        0x111   /* lock 32 bit variable                 */
#define T5C_UNLOCKDINT      0x112   /* unlock 32 bit variable               */

#define T5C_O_NEGDINT       0x6e    /* <ifnolock> DINT: a := - b            */
#define T5C_O_MULDINT       0x71    /* <ifnolock> DINT: a := b * c          */
#define T5C_O_DIVDINT       0x73    /* <ifnolock> DINT: a := b / c          */
#define T5C_O_ADDDINT       0x75    /* <ifnolock> DINT: a := b + c          */
#define T5C_O_SUBDINT       0x78    /* <ifnolock> DINT: a := b - c          */
#define T5C_O_GTDINT        0x7e    /* <ifnolock> DINT: a := b > c          */
#define T5C_O_GEDINT        0x86    /* <ifnolock> DINT: a := b >= c         */
#define T5C_O_EQDINT        0x8a    /* <ifnolock> DINT: a := b == c         */
#define T5C_O_NEDINT        0x8f    /* <ifnolock> DINT: a := b != c         */
#define T5C_O_COPYDINT      0x98    /* <ifnolock> DINT: a := b              */
#define T5C_O_AGETDINT      0xa2    /* <ifnolock> DINT: a := b [ c ]        */
#define T5C_O_APUTDINT      0xa7    /* <ifnolock> DINT: a [ b ] := c        */
#define T5C_O_SETDINT       0x110   /* force 32 bit value                   */

/****************************************************************************/
/* UDINT */

#define T5C_GTUDINT         0x1f2   /* UDINT: a := b > c                    */
#define T5C_GEUDINT         0x1f3   /* UDINT: a := b >= c                   */
#define T5C_MULUDINT        0x1f4   /* UDINT: a := b * c                    */
#define T5C_DIVUDINT        0x1f5   /* UDINT: a := b / c                    */

#define T5C_0_GTUDINT       0x1f6   /* <ifnolock> UDINT: a := b > c         */
#define T5C_0_GEUDINT       0x1f7   /* <ifnolock> UDINT: a := b >= c        */
#define T5C_0_MULUDINT      0x1f8   /* <ifnolock> UDINT: a := b * c         */
#define T5C_0_DIVUDINT      0x1f9   /* <ifnolock> UDINT: a := b / c         */

/****************************************************************************/
/* LINT */

#define T5C_NEGLINT         0x121   /* LINT: a := - b                       */
#define T5C_MULLINT         0x123   /* LINT: a := b * c                     */
#define T5C_DIVLINT         0x125   /* LINT: a := b / c                     */
#define T5C_ADDLINT         0x127   /* LINT: a := b + c                     */
#define T5C_SUBLINT         0x129   /* LINT: a := b - c                     */
#define T5C_GTLINT          0x12d   /* LINT: a := b > c                     */
#define T5C_GELINT          0x131   /* LINT: a := b >= c                    */
#define T5C_EQLINT          0x133   /* LINT: a := b == c                    */
#define T5C_NELINT          0x135   /* LINT: a := b != c                    */
#define T5C_COPYLINT        0x137   /* LINT: a := b                         */
#define T5C_ARGETLINT       0x139   /* LINT: a := b [ c ]                   */
#define T5C_ARPUTLINT       0x13b   /* LINT: a [ b ] := c                   */
#define T5C_SETLINT         0x13c   /* force 32 bit value                   */
#define T5C_LOCKLINT        0x13e   /* lock 32 bit variable                 */
#define T5C_UNLOCKLINT      0x13f   /* unlock 32 bit variable               */

#define T5C_O_NEGLINT       0x120   /* <ifnolock> LINT: a := - b            */
#define T5C_O_MULLINT       0x122   /* <ifnolock> LINT: a := b * c          */
#define T5C_O_DIVLINT       0x124   /* <ifnolock> LINT: a := b / c          */
#define T5C_O_ADDLINT       0x126   /* <ifnolock> LINT: a := b + c          */
#define T5C_O_SUBLINT       0x128   /* <ifnolock> LINT: a := b - c          */
#define T5C_O_GTLINT        0x12c   /* <ifnolock> LINT: a := b > c          */
#define T5C_O_GELINT        0x130   /* <ifnolock> LINT: a := b >= c         */
#define T5C_O_EQLINT        0x132   /* <ifnolock> LINT: a := b == c         */
#define T5C_O_NELINT        0x134   /* <ifnolock> LINT: a := b != c         */
#define T5C_O_COPYLINT      0x136   /* <ifnolock> LINT: a := b              */
#define T5C_O_ARGETLINT     0x138   /* <ifnolock> LINT: a := b [ c ]        */
#define T5C_O_ARPUTLINT     0x13a   /* <ifnolock> LINT: a [ b ] := c        */
#define T5C_O_SETLINT       0x13d   /* <ifnolock> force 32 bit value        */

/****************************************************************************/
/* ULINT */

#define T5C_GTULINT         0x22c   /* ULINT: a := b > c                    */
#define T5C_GEULINT         0x22d   /* ULINT: a := b >= c                   */
#define T5C_MULULINT        0x22e   /* ULINT: a := b * c                    */
#define T5C_DIVULINT        0x22f   /* ULINT: a := b / c                    */

#define T5C_0_GTULINT       0x230   /* <ifnolock> ULINT: a := b > c         */
#define T5C_0_GEULINT       0x231   /* <ifnolock> ULINT: a := b >= c        */
#define T5C_0_MULULINT      0x232   /* <ifnolock> ULINT: a := b * c         */
#define T5C_0_DIVULINT      0x233   /* <ifnolock> ULINT: a := b / c         */

/****************************************************************************/
/* REAL */

#define T5C_NEGREAL         0x02    /* REAL: a := - b                       */
#define T5C_MULREAL         0x05    /* REAL: a := b * c                     */
#define T5C_DIVREAL         0x07    /* REAL: a := b / c                     */
#define T5C_ADDREAL         0x09    /* REAL: a := b + c                     */
#define T5C_SUBREAL         0x0d    /* REAL: a := b - c                     */
#define T5C_GTREAL          0x14    /* REAL: a := b > c                     */
#define T5C_GEREAL          0x1c    /* REAL: a := b >= c                    */
#define T5C_EQREAL          0x20    /* REAL: a := b == c                    */
#define T5C_NEREAL          0x25    /* REAL: a := b != c                    */
#define T5C_COPYREAL        0x2e    /* REAL: a := b                         */
#define T5C_ARGETREAL       0x39    /* REAL: a := b [ c ]                   */
#define T5C_ARPUTREAL       0x3e    /* REAL: a [ b ] := c                   */

#define T5C_O_NEGREAL       0x6f    /* <ifnolock> REAL: a := - b            */
#define T5C_O_MULREAL       0x72    /* <ifnolock> REAL: a := b * c          */
#define T5C_O_DIVREAL       0x74    /* <ifnolock> REAL: a := b / c          */
#define T5C_O_ADDREAL       0x76    /* <ifnolock> REAL: a := b + c          */
#define T5C_O_SUBREAL       0x79    /* <ifnolock> REAL: a := b - c          */
#define T5C_O_GTREAL        0x7f    /* <ifnolock> REAL: a := b > c          */
#define T5C_O_GEREAL        0x87    /* <ifnolock> REAL: a := b >= c         */
#define T5C_O_EQREAL        0x8b    /* <ifnolock> REAL: a := b == c         */
#define T5C_O_NEREAL        0x90    /* <ifnolock> REAL: a := b != c         */
#define T5C_O_COPYREAL      0x99    /* <ifnolock> REAL: a := b              */
#define T5C_O_AGETREAL      0xa3    /* <ifnolock> REAL: a := b [ c ]        */
#define T5C_O_APUTREAL      0xa8    /* <ifnolock> TIME: a [ b ] := c        */

/****************************************************************************/
/* LREAL */

#define T5C_NEGLREAL        0x141   /* LREAL: a := - b                      */
#define T5C_MULLREAL        0x143   /* LREAL: a := b * c                    */
#define T5C_DIVLREAL        0x145   /* LREAL: a := b / c                    */
#define T5C_ADDLREAL        0x147   /* LREAL: a := b + c                    */
#define T5C_SUBLREAL        0x149   /* LREAL: a := b - c                    */
#define T5C_GTLREAL         0x14d   /* LREAL: a := b > c                    */
#define T5C_GELREAL         0x151   /* LREAL: a := b >= c                   */
#define T5C_EQLREAL         0x153   /* LREAL: a := b == c                   */
#define T5C_NELREAL         0x155   /* LREAL: a := b != c                   */
#define T5C_COPYLREAL       0x157   /* LREAL: a := b                        */
#define T5C_ARGETLREAL      0x159   /* LREAL: a := b [ c ]                  */
#define T5C_ARPUTLREAL      0x15b   /* LREAL: a [ b ] := c                  */

#define T5C_O_NEGLREAL      0x140   /* <ifnolock> LREAL: a := - b           */
#define T5C_O_MULLREAL      0x142   /* <ifnolock> LREAL: a := b * c         */
#define T5C_O_DIVLREAL      0x144   /* <ifnolock> LREAL: a := b / c         */
#define T5C_O_ADDLREAL      0x146   /* <ifnolock> LREAL: a := b + c         */
#define T5C_O_SUBLREAL      0x148   /* <ifnolock> LREAL: a := b - c         */
#define T5C_O_GELREAL       0x14c   /* <ifnolock> LREAL: a := b > c         */
#define T5C_O_GTLREAL       0x150   /* <ifnolock> LREAL: a := b >= c        */
#define T5C_O_EQLREAL       0x152   /* <ifnolock> LREAL: a := b == c        */
#define T5C_O_NELREAL       0x154   /* <ifnolock> LREAL: a := b != c        */
#define T5C_O_COPYLREAL     0x156   /* <ifnolock> LREAL: a := b             */
#define T5C_O_ARGETLREAL    0x158   /* <ifnolock> LREAL: a := b [ c ]       */
#define T5C_O_ARPUTLREAL    0x15a   /* <ifnolock> LREAL: a [ b ] := c       */

/****************************************************************************/
/* TIME */

#define T5C_ADDTIME         0x0a    /* TIME: a := b + c                     */
#define T5C_SUBTIME         0x0e    /* TIME: a := b - c                     */
#define T5C_GTTIME          0x15    /* TIME: a := b > c                     */
#define T5C_GETIME          0x1d    /* TIME: a := b >= c                    */
#define T5C_EQTIME          0x21    /* TIME: a := b == c                    */
#define T5C_NETIME          0x26    /* TIME: a := b != c                    */
#define T5C_COPYTIME        0x2f    /* TIME: a := b                         */
#define T5C_ARGETTIME       0x3a    /* TIME: a := b [ c ]                   */
#define T5C_ARPUTTIME       0x3f    /* TIME: a [ b ] := c                   */
#define T5C_STARTTIME       0x113   /* tstart a                             */
#define T5C_STOPTIME        0x114   /* tstop a                              */
#define T5C_SETTIME         0x117   /* force TIME value                     */
#define T5C_LOCKTIME        0x119   /* lock bool                            */
#define T5C_UNLOCKTIME      0x11a   /* unlock bool                          */

#define T5C_O_ADDTIME       0x11b   /* add TIME - to output                 */
#define T5C_O_SUBTIME       0x11c   /* sub TIME - to output                 */
#define T5C_O_GTTIME        0x81    /* <ifnolock> TIME: a := b > c          */
#define T5C_O_GETIME        0x89    /* <ifnolock> TIME: a := b >= c         */
#define T5C_O_EQTIME        0x8e    /* <ifnolock> TIME: a := b == c         */
#define T5C_O_NETIME        0x93    /* <ifnolock> TIME: a := b != c         */
#define T5C_O_COPYTIME      0x9b    /* <ifnolock> TIME: a := b              */
#define T5C_O_AGETTIME      0xa5    /* <ifnolock> TIME: a := b [ c ]        */
#define T5C_O_APUTTIME      0x100   /* <ifnolock> TIME: a [ b ] := c        */
#define T5C_O_STARTTIME     0x115   /* tstart a                             */
#define T5C_O_STOPTIME      0x116   /* tstop a                              */
#define T5C_O_SETTIME       0x118   /* force TIME value                     */

/****************************************************************************/
/* STRING */

#define T5C_ADDSTRING       0x0b    /* STRING: a := b + c                   */
#define T5C_GTSTRING        0x16    /* STRING: a := b > c                   */
#define T5C_GESTRING        0x1e    /* STRING: a := b >= c                  */
#define T5C_EQSTRING        0x22    /* STRING: a := b == c                  */
#define T5C_NESTRING        0x27    /* STRING: a := b != c                  */
#define T5C_COPYSTRING      0x30    /* STRING: a := b                       */
#define T5C_ARGETSTRING     0x3b    /* STRING: a := b [ c ]                 */
#define T5C_ARPUTSTRING     0x40    /* STRING: a [ b ] := c                 */
#define T5C_SETSTRING       0x198   /* force string                         */
#define T5C_LOCKSTRING      0x19a   /* lock string variable                 */
#define T5C_UNLOCKSTRING    0x19b   /* unlock string variable               */

#define T5C_O_ADDSTRING     0x77    /* <ifnolock> STRING: a := b + c        */
#define T5C_O_GTSTRING      0x80    /* <ifnolock> STRING: a := b > c        */
#define T5C_O_GESTRING      0x88    /* <ifnolock> STRING: a := b >= c       */
#define T5C_O_EQSTRING      0x8c    /* <ifnolock> STRING: a := b == c       */
#define T5C_O_NESTRING      0x91    /* <ifnolock> STRING: a := b != c       */
#define T5C_O_COPYSTRING    0x9a    /* <ifnolock> STRING: a := b            */
#define T5C_O_AGETSTRING    0xa4    /* <ifnolock> STRING: a := b [ c ]      */
#define T5C_O_APUTSTRING    0xa9    /* <ifnolock> STRING: a [ b ] := c      */
#define T5C_O_SETSTRING     0x199   /* force output string                  */

/****************************************************************************/
/* bit access */

#define T5C_GETBIT8         0x1fd   /* extract bit - 8 bits                 */
#define T5C_GETBIT16        0x1fe   /* extract bit - 16 bits                */
#define T5C_GETBIT32        0x1ff   /* extract bit - 32 bits                */
#define T5C_GETBIT64        0x200   /* extract bit - 64 bits                */
#define T5C_SETBIT8         0x201   /* set bit - 8 bits                     */
#define T5C_SETBIT16        0x202   /* set bit - 16 bits                    */
#define T5C_SETBIT32        0x203   /* set bit - 32 bits                    */
#define T5C_SETBIT64        0x204   /* set bit - 64 bits                    */

#define T5C_O_GETBIT8       0x205   /* <ifnolock> extract bit - 8 bits      */
#define T5C_O_GETBIT16      0x206   /* <ifnolock> extract bit - 16 bits     */
#define T5C_O_GETBIT32      0x207   /* <ifnolock> extract bit - 32 bits     */
#define T5C_O_GETBIT64      0x208   /* <ifnolock> extract bit - 64 bits     */
#define T5C_O_SETBIT8       0x209   /* <ifnolock> set bit - 8 bits          */
#define T5C_O_SETBIT16      0x20a   /* <ifnolock> set bit - 16 bits         */
#define T5C_O_SETBIT32      0x20b   /* <ifnolock> set bit - 32 bits         */
#define T5C_O_SETBIT64      0x20c   /* <ifnolock> set bit - 64 bits         */

/****************************************************************************/
/* jumps */

#define T5C_JUMPIF          0x41    /* BRANCH: jump if a                    */
#define T5C_JUMPIFNOT       0x42    /* BRANCH: jump if not a                */
#define T5C_JUMP            0x43    /* BRANCH: jump - no condition          */
#define T5C_RET             0x44    /* BRANCH: return                       */
#define T5C_ENDSEQ          0xea    /* BRANCH: end of sequence              */
#define T5C_JA              0x210   /* BRANCH: abs jump - no condition      */
#define T5C_JAC             0x211   /* BRANCH: abs jump if a                */
#define T5C_JACN            0x212   /* BRANCH: abs jump if not a            */
#define T5C_JAEQ            0x0220  /* jump if EQ: abs-jumps, dint1, dint2  */
#define T5C_SWITCH          0x021f  /* switch table: i, #nb, abs-jumps      */
 
/****************************************************************************/
/* SFC */

#define T5C_TRSASSIGN       0x6b    /* SFC: transition = boolean            */
#define T5C_STEPTIME        0xd0    /* SFC: get step time: booldst, step    */
#define T5C_STEPFLAG        0xd1    /* SFC: get step act: timedst, step     */
#define T5C_STEP            0x19c   /* step,labelP1,labelP0,labelEND        */
#define T5C_TRANSEVAL       0x19d   /* label,trans,nbstepbefore,steps..     */
#define T5C_TRANSCROSSP0    0x19e   /* trans,nbstepbefore,steps..           */
#define T5C_TRANSCROSSP1    0x19f   /* trans,nbstepafter,steps..            */
#define T5C_TRANSSOURCE     0x1a0   /* trans,nbstepafter,steps..            */
#define T5C_TRANSEVAL1      0x1a1   /* label,trans,stepbefore               */
#define T5C_TRANSCROSS1     0x1a2   /* trans,stepbefore,stepafter           */
#define T5C_TRANSSOURCE1    0x1a3   /* trans,stepafter                      */
#define T5C_SELFPRGCONTROL  0x1a4   /* nbchildren, children...              */

/****************************************************************************/
/* simplified SFC control - with absolute jumps! */

#define T5C_JNFPSTAT        0x0221  /* abs-jmp if not prg stat: flag, dest  */
#define T5C_SETPSTAT        0x0222  /* set prg stat: status                 */
#define T5C_SFCSETSX        0x0223  /* set .X: step, value                  */
#define T5C_SFCSETST        0x0224  /* set .T: step, 0=reset | 1=update     */
#define T5C_SFCCHECKTBK     0x0225  /* check trans bkpt: trans              */
#define T5C_JNTRANS         0x0226  /* jump if not trans: trans, asb-dest   */
#define T5C_SFCTSR          0x0227  /* SFC test and reset: ASvar, asb-dest  */

/****************************************************************************/
/* POU control */

#define T5C_POUSTART        0xbb    /* MISC: start a program                */
#define T5C_POUSUSPEND      0xbc    /* MISC: suspend a program              */
#define T5C_POUSTOP         0xbd    /* MISC: stop a program                 */
#define T5C_POURESUME       0xbe    /* MISC: restart a program              */
#define T5C_POUSTATUS       0xc1    /* MISC: get program status             */
#define T5C_O_POUSTATUS     0xc8    /* <ifnolock> MISC: get program status  */
#define T5C_SCHEDPRG        0x1a5   /* schedule: period, offset             */
#define T5C_CALLPRG         0x1a7   /* prgno                                */

/****************************************************************************/
/* type conversion */

#define T5C_CAST            0xcd    /* cast: a <- b c=dsttype d=dsttype     */
#define T5C_O_CAST          0xcf    /* <ifnolock> cast: a <- b type(c<-d)   */

/****************************************************************************/
/* function calls */

#define T5C_STDFC           0x11d   /* std func: no + nbp + out + inputs    */
#define T5C_CUSFC           0x11e   /* custom func: iFC+nbp+inps+out+type   */
#define T5C_FBCALL          0x11f   /* FB: inst + nbp + inputs + outputs    */

/****************************************************************************/
/* misc */

#define T5C_NOP             0x20e   /* no operation                         */
#define T5C_CHECKBOUND      0x1a6   /* index, min, max                      */
#define T5C_SETWAITMARK     0x1fa   /* re-start posistion for WAIT          */
#define T5C_WAIT            0x1fb   /* WAIT test                            */
#define T5C_STEPMARK        0x1fc   /* stepping position marker             */

/****************************************************************************/
/* exten variables */

#define T5C_XVGET8          0x1a8   /* xv, index                            */
#define T5C_XVGET16         0x1a9   /* xv, index                            */
#define T5C_XVGET32         0x1aa   /* xv, index                            */
#define T5C_XVGET64         0x1ab   /* xv, index                            */
#define T5C_XVGETTIME       0x1ac   /* xv, index                            */
#define T5C_XVGETSTRING     0x1ad   /* xv, index                            */
#define T5C_XVSET8          0x1ae   /* xv, index                            */
#define T5C_XVSET16         0x1af   /* xv, index                            */
#define T5C_XVSET32         0x1b0   /* xv, index                            */
#define T5C_XVSET64         0x1b1   /* xv, index                            */
#define T5C_XVSETTIME       0x1b2   /* xv, index                            */
#define T5C_XVSETSTRING     0x1b3   /* xv, index                            */
#define T5C_XVF8            0x1b4   /* force 8 bit value                    */
#define T5C_XVF16           0x1b5   /* force 16 bit value                   */
#define T5C_XVF32           0x1b6   /* force 32 bit value                   */
#define T5C_XVF64           0x1b7   /* force 32 bit value                   */
#define T5C_XVFTIME         0x1b8   /* force TIME value                     */
#define T5C_XVFSTRING       0x1b9   /* force string                         */
#define T5C_XVLOCK          0x1c6   /* lock external variable               */
#define T5C_XVUNLOCK        0x1c7   /* unlock external variable             */
#define T5C_XAGET8          0x1c8   /* xv, index, array-index               */
#define T5C_XAGET16         0x1c9   /* xv, index, array-index               */
#define T5C_XAGET32         0x1ca   /* xv, index, array-index               */
#define T5C_XAGET64         0x1cb   /* xv, index, array-index               */
#define T5C_XAGETTIME       0x1cc   /* xv, index, array-index               */
#define T5C_XAGETSTRING     0x1cd   /* xv, index, array-index               */
#define T5C_XASET8          0x1ce   /* xv, index, array-index               */
#define T5C_XASET16         0x1cf   /* xv, index, array-index               */
#define T5C_XASET32         0x1d0   /* xv, index, array-index               */
#define T5C_XASET64         0x1d1   /* xv, index, array-index               */
#define T5C_XASETTIME       0x1d2   /* xv, index, array-index               */
#define T5C_XASETSTRING     0x1d3   /* xv, index, array-index               */

#define T5C_O_XVGET8        0x1ba   /* xv, index                            */
#define T5C_O_XVGET16       0x1bb   /* xv, index                            */
#define T5C_O_XVGET32       0x1bc   /* xv, index                            */
#define T5C_O_XVGET64       0x1bd   /* xv, index                            */
#define T5C_O_XVGETTIME     0x1be   /* xv, index                            */
#define T5C_O_XVGETSTRING   0x1bf   /* xv, index                            */
#define T5C_O_XVSET8        0x1c0   /* xv, index                            */
#define T5C_O_XVSET16       0x1c1   /* xv, index                            */
#define T5C_O_XVSET32       0x1c2   /* xv, index                            */
#define T5C_O_XVSET64       0x1c3   /* xv, index                            */
#define T5C_O_XVSETTIME     0x1c4   /* xv, index                            */
#define T5C_O_XVSETSTRING   0x1c5   /* xv, index                            */
#define T5C_O_XAGET8        0x1d4   /* xv, index, array-index               */
#define T5C_O_XAGET16       0x1d5   /* xv, index, array-index               */
#define T5C_O_XAGET32       0x1d6   /* xv, index, array-index               */
#define T5C_O_XAGET64       0x1d7   /* xv, index, array-index               */
#define T5C_O_XAGETTIME     0x1d8   /* xv, index, array-index               */
#define T5C_O_XAGETSTRING   0x1d9   /* xv, index, array-index               */
#define T5C_O_XASET8        0x1da   /* xv, index, array-index               */
#define T5C_O_XASET16       0x1db   /* xv, index, array-index               */
#define T5C_O_XASET32       0x1dc   /* xv, index, array-index               */
#define T5C_O_XASET64       0x1dd   /* xv, index, array-index               */
#define T5C_O_XASETTIME     0x1de   /* xv, index, array-index               */
#define T5C_O_XASETSTRING   0x1df   /* xv, index, array-index               */

/****************************************************************************/
/* CTSeg operations */

#define T5C_GETCTSEG        0x213   /* get: type - dst - reference          */
#define T5C_SETCTSEG        0x214   /* put: type - src - reference          */
#define T5C_SETCTSEGREF     0x215   /* setref for write: hi - low           */
#define T5C_CTSEG_NOP       0x216   /* CTSeg FB: no operation               */
#define T5C_CTSEG_FBCALL    0x217   /* CTSeg FB: ref + nbp + in.. + out..   */
#define T5C_CTSEG_FCARG     0x218   /* check CTseg arg func: iFC            */
#define T5C_CTSEG_FBARG     0x219   /* check CTSeg arg FB: ref              */
#define T5C_CTSEG_TSTART    0x21a   /* start CTSeg timer: ref               */
#define T5C_CTSEG_TSTOP     0x21b   /* stop CTSeg timer: ref                */
#define T5C_CTMEMCPY        0x22b   /* dst - srv - size                     */

#define T5C_O_GETCTSEG      0x236   /* unused */
#define T5C_O_SETCTSEG      0x237   /* unused */

/****************************************************************************/
/* pointers */

#define T5C_ADR             0x0228  /* dst, type, src, index                */
#define T5C_ADRCT           0x0229  /* dst, ref                             */

/****************************************************************************/
/* debugger commands */

#define T5C_PAUSE           0x101   /* pause to cycle by cycle mode         */
#define T5C_RESUME          0x102   /* resume to normal mode                */
#define T5C_CYCLESTEP       0x103   /* execute one cycle                    */
#define T5C_SETCYCLETIME    0x104   /* (long)tme - set cycle timing         */
#define T5C_STEPFWD         0x20d   /* execute next step in code            */
#define T5C_SETBKP          0x20f   /* set breakpoint - prog - pos - style  */
#define T5C_UNLOCKALL       0x022a  /* unlock all variables                 */
#define T5C_SFCBKS          0x21c   /* SFC: set step bkp - index - style    */
#define T5C_SFCBKT          0x21d   /* SFC: set trans bkp - index - style   */

#define T5C_EXECTL          0x21e   /* exec control: command                */
#define T5CCTL_CC           1       /* run CC code                          */
#define T5CCTL_PC           2       /* run p-code                           */

#define T5C_BSAMPLING       0x1e0   /* index, flags, periodH-L, on, off,    */
                                    /* ondelay, offdelay, nbvars, vars      */
#define T5C_BSMPCTL         0x1e1   /* index, bRun                          */
#define T5BSMP_ACTIVE       0x0001  /* main activation flag                 */
#define T5BSMP_AUTOSTART    0x0002  /* start immediately                    */
#define T5BSMP_ONESHOT      0x0004  /* starts only once                     */
#define T5BSMP_STOPONFULL   0x0008  /* stop on buffer full                  */
#define T5BSMP_ONFALL       0x0010  /* start on falling edge                */
#define T5BSMP_OFFFALL      0x0020  /* stop on falling edge                 */

/****************************************************************************/
/* for CS */

#define T5CCS_SETPSWLOAD    0x0001  /* oldH, oldL, newH, newL               */

/****************************************************************************/
/* diagnostics */

#define T5C_BM              0x0234  /* no arg                               */
#define T5C_EM              0x0235  /* diUs                                 */

/****************************************************************************/
/* VSI */

#define T5C_SETVSISTAMP     0x0238 /* type, index, dtHI, dtLO, tmHi, tmLo   */
#define T5C_RESETVSI        0x0239 /* type, index                           */

/****************************************************************************/

#define T5C_nextone         0x23a

/****************************************************************************/
/* compatibility */

#define TIC_BOO             0x01
#define TIC_ANA             0x02
#define TIC_REA             0x03
#define TIC_TMR             0x04
#define TIC_MSG             0x05
#define TIC_SINT            0x0a
#define TIC_WORD            0x0b
#define TIC_LINT            0x0c
#define TIC_LREAL           0x0d
#define TIC_USINT           0x0e
#define TIC_UINT            0x0f
#define TIC_UDINT           0x10
#define TIC_COMPLEX         0x7f
#define TIC_EXTERN          0x80

#endif /*_T5TIC_H_INCLUDED_*/

/* eof **********************************************************************/
