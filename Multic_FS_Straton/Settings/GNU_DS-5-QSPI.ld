/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name     : GNU_DS-5-QSPI.ld
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-RZv13.01-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Linker file for projects that require to run from 
*               : SPI multi I/O device ROM 
******************************************************************************/
/******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 18.06.2013 1.00
*               : 21.03.2014 2.00    Fixed RAM0L origin address. 
******************************************************************************/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)

/* Base Address RAM Memory Table 10-Mbyte on-chip RAM */  
/* Total RAM available 10112K */
MEMORY 
{
  /* Internal RAM address range H'2000_0000 to H'2001_FFFF is configured as data retention RAM            */
  /* Write access to this address range has to be enabled by writing to registers SYSCR1 and SYSCR2       */
  /* In QSPI Boot Mode 1, the on-chip boot program uses area at H'2002_0000 to H'2002_3FFF as work memory */
  /* Therefore RAM0L does not include area H'2002_0000 to H'2002_3FFF                                     */
  RAM0L (rwx)   : ORIGIN = 0x20020000, LENGTH = 0x000E0000  /* Page 0 Lower bank ( 896KB) H'20020000 to H'200FFFFF */
  RAM1L (rwx)   : ORIGIN = 0x20100000, LENGTH = 0x00100000  /* Page 1 Upper bank (1024KB) H'20100000 to H'201FFFFF */
  RAM2L (rwx)   : ORIGIN = 0x20200000, LENGTH = 0x00100000  /* Page 2 Upper bank (1024KB) H'20200000 to H'202FFFFF */
  RAM3L (rwx)   : ORIGIN = 0x20300000, LENGTH = 0x00100000  /* Page 3 Upper bank (1024KB) H'20300000 to H'203FFFFF */
  RAM4L (rwx)   : ORIGIN = 0x20400000, LENGTH = 0x00100000  /* Page 4 Upper bank (1024KB) H'20400000 to H'204FFFFF */
  RAM0U (rwx)   : ORIGIN = 0x20500000, LENGTH = 0x00100000  /* Page 0 Upper bank (1024KB) H'20500000 to H'205FFFFF */
  RAM1U (rwx)   : ORIGIN = 0x20600000, LENGTH = 0x00100000  /* Page 1 Upper bank (1024KB) H'20600000 to H'206FFFFF */
  RAM2U (rwx)   : ORIGIN = 0x20700000, LENGTH = 0x00100000  /* Page 2 Upper bank (1024KB) H'20700000 to H'207FFFFF */
  RAM3U (rwx)   : ORIGIN = 0x20800000, LENGTH = 0x00100000  /* Page 3 Upper bank (1024KB) H'20800000 to H'208FFFFF */
  RAM4U (rwx)   : ORIGIN = 0x20900000, LENGTH = 0x00100000  /* Page 4 Upper bank (1024KB) H'20900000 to H'209FFFFF */
  SDRAM (rwx)   : ORIGIN = 0x4c000000, LENGTH = 0x02000000  /* SDRAM 32MB attached to CS2 H'08000000 to H'09FFFFFF */

  ROM   (rw)    : ORIGIN = 0x18000000, LENGTH = 0x02000000  /* SPI ROM attached to H'18000000 to H'19FFFFFF */
  HRAM  (rw)    : ORIGIN = 0x20100000, LENGTH = 0x00600000  /* Page[4L01234H]: H'20400000 to H'209FFFFF */
  VRAM0 (rw)    : ORIGIN = 0x60000100, LENGTH = 0x00007000  /* Page[12L]>VRAM0: H'60000100 to H'60007100 */
  VRAM  (rw)    : ORIGIN = 0x60100000, LENGTH = 0x00900000  /* Page[12L]>VRAM: H'60300000 to H'604FFFFF */

  INCLUDE MONITOR_MEMORY.ld
}

PROGRAM_STACK_SIZE    = 0x8000;   /* Application stack                  */
IRQ_STACK_SIZE        = 0x2000;     /* IRQ mode stack                     */
FIQ_STACK_SIZE        = 0x2000;     /* FRQ mode stack                     */
SVC_STACK_SIZE        = 0x2000;     /* SVC mode stack                     */
ABT_STACK_SIZE        = 0x2000;     /* ABT mode stack                     */
TTB_SIZE              = 0x8000;     /* Level-1 Translation Table for MMU  */
   
QSPI_BASE             = 0x18080000; /* VECTOR_TABLE located here */
MIRRORED_VECTOR_TABLE = 0x60040000; /* VECTOR_MIRROR_TABLE located here */   

__MonitorFunctions_start = ORIGIN(MON_FUNC);
__MonitorCode_start = ORIGIN(MON_CODE);
__MonitorCode_end = ORIGIN(MON_CODE) + LENGTH(MON_CODE);

__StratonFunctions_start = ORIGIN(STR_FUNC);
__StratonCode_start = ORIGIN(STR_CODE);
__StratonCode_end = ORIGIN(STR_CODE) + LENGTH(STR_CODE);
__StratonZED_start = ORIGIN(STR_ZED);

__StratonEmWi_ZED_Wiring_start = ORIGIN(STR_EMW_ZED_WIR);
__StratonEmWi_ZED_Wiring_size = LENGTH(STR_EMW_ZED_WIR);
__StratonEmWi_ZED_Wiring_end = __StratonEmWi_ZED_Wiring_start + __StratonEmWi_ZED_Wiring_size;


__EmWizardFunctions_start = ORIGIN(EMW_FUNC);
__EmWizardCode_start = ORIGIN(EMW_CODE);
__EmWizardCode_end = ORIGIN(EMW_CODE) + LENGTH(EMW_CODE);

__MON_ZED_start = ORIGIN(MON_ZED);
__MON_ZED_size = LENGTH(MON_ZED);
__MON_ZED_end = __MON_ZED_start + __MON_ZED_size;
   
__MON_NVDM_start = ORIGIN(MON_NVDM);
__MON_NVDM_size = LENGTH(MON_NVDM);
__MON_NVDM_end = __MON_NVDM_start + __MON_NVDM_size;

__VMDB_ram_start = ORIGIN(STR_VMDB);
__VMDB_ram_size = LENGTH(STR_VMDB);

__code_app_start = ORIGIN(STR_T5AP);
__code_app_size = LENGTH(STR_T5AP);

__ActiGRAF_Banners_start = ORIGIN(AGF_IDEN);
__ActiGRAF_Banners_size = LENGTH(AGF_IDEN);

__AppParamConfig_start = ORIGIN(STR_PARA);
__AppParamConfig_size = LENGTH(STR_PARA);

SECTIONS
{
  __StratonFunctions : {} > STR_FUNC
  __StratonSWIdent   : {} > STR_IDEN
  
  .text :
  {
    *(.text)                        /* .text sections (code) */
    *(.text.startup)
  } > STR_CODE
  
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > MON_CODE
   
  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > MON_CODE

  .rodata :
  {
    *(.rodata)                      /* .rodata sections (constants, strings, etc.) */
    *(.rodata.*)                    /* .rodata* sections (constants, strings, etc.) */
                                    /*   *(.glue_7)   glue arm to thumb code */
                                    /*   *(.glue_7t)  glue thumb to arm code */
    *(.got.plt) *(.got)        
    . = ALIGN(0x8);
    address_start_data_ROM = .;
  } > STR_CODE

  MON_ZED_VM_SECTION    (NOLOAD) : {} > MON_ZED_VM
  MON_ZED_MULTICS_SECTION (NOLOAD) : {} > MON_ZED_MULTICS
  MON_ZED_POWERXX_SECTION (NOLOAD) : {} > MON_ZED_POWERXX
  MON_ZED_SWP_SECTION   (NOLOAD) : {} > MON_ZED_SWP
  MON_ZED_J1939_SECTION (NOLOAD) : {} > MON_ZED_J1939

  .data : AT(address_start_data_ROM)
  {
    address_start_data_RAM = .;
    *(.data)                        /* .data sections */
    *(.data.*)                      /* .data* sections */
    . = ALIGN(0x8);
    address_end_data_RAM = .;       /* define a global symbol at data end */
    } > STR_RAM 

  address_end_data_ROM = LOADADDR(.data) + SIZEOF (.data); 
  PROVIDE(end = address_end_data_ROM);
    
  .bss :
  {                                 /* used by the startup in order to initialize the .bss secion */
    _bss = .;                       /* define a global symbol at bss start */
    PROVIDE(__bss_start__ = .);         
    *(.bss)
    *(.bss.**)
    *(COMMON)
    PROVIDE(__bss_end__ = .);
    _ebss = .;                      /* define a global symbol at bss end */
    _end = .;
  } > STR_RAM 
    

  .ARM.attributes 0 : { *(.ARM.attributes) }

  /* NOLOAD directs linker NOT to fill xxxx_SECTION with 0.          */
  /* Usage of NOLOAD increases speed of linker and download to target */
  BSS_DMAC_SAMPLE_INTERNAL_RAM 0x60000200 (NOLOAD) : {} > VRAM0
  VRAM_SECTION 0x60100000 (NOLOAD) : {} > VRAM
  HEAP_SECTION 0x4c400000 (NOLOAD) : {} > SDRAM  
}