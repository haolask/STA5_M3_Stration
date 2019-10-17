/*****************************************************************************
T5vfsitm.h : virtual file system - list of possible memory/file items

DO NOT ALTER THIS !

(c) COPALP 2009
*****************************************************************************/

#ifndef _T5VFSITM_H_INCLUDED_
#define _T5VFSITM_H_INCLUDED_

#ifdef T5DEF_VFS

/****************************************************************************/
/* predefined objects in RAM memory */

#define T5VFS_RAM_VMDB          1   /* VM database */
#define T5VFS_RAM_CODE          2   /* original p-code */
#define T5VFS_RAM_CHANGE        3   /* changed p-code */

/****************************************************************************/
/* predefined objects flash or SRAM buffers */

#define T5VFS_FILE_VMDB         1   /* VM database */
#define T5VFS_FILE_CODE         2   /* original p-code */
#define T5VFS_FILE_CHANGE       3   /* changed p-code */

#define T5VFS_FILE_SOURCE       4   /* zipped source code (for upload) */

#define T5VFS_FILE_RCSCREENS    5   /* HMI resources: screens */
#define T5VFS_FILE_RCSTRINGS    6   /* HMI resources: strings */
#define T5VFS_FILE_RCBITMAPS    7   /* HMI resources: bitmaps */
#define T5VFS_FILE_RCFONTS      8   /* HMI resources: fonts */

#define T5VFS_FILE_REG          9   /* current embedded registry */
#define T5VFS_FILE_REGCHANGE    10  /* changed(next) embedded registry */

#define T5VFS_FILE_SYSINFO      11  /* system info text (read only) */

#define T5VFS_FILE_MAX (T5VFS_FILE_SYSINFO+1)

/****************************************************************************/

#endif /*T5DEF_VFS*/

#endif /*_T5VFSITM_H_INCLUDED_*/

/* eof **********************************************************************/
