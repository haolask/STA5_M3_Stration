
/**
 * STA1295_EVB memory maps definitions
 *
 * This file has been automatically generated.
 * Please, do NOT edit.
 */

#ifndef STA1295_EVB_MEM_MAP_H
#define STA1295_EVB_MEM_MAP_H

/* ESRAM_M3 memory mapping */
#define ESRAM_M3_BASE 0x10000000
#define ESRAM_M3_SIZE 0x40000
#define ESRAM_M3_XL_SOFTWARE_BASE (ESRAM_M3_BASE + 0x0)	/* 0x10000000 */
#define ESRAM_M3_XL_SOFTWARE_SIZE 0x3f800
#define ESRAM_M3_SHARED_DATA_BASE (ESRAM_M3_BASE + 0x3f800)	/* 0x1003f800 */
#define ESRAM_M3_SHARED_DATA_SIZE 0x600
#define ESRAM_M3_RUNTIME_TOC_BASE (ESRAM_M3_BASE + 0x3fe00)	/* 0x1003fe00 */
#define ESRAM_M3_RUNTIME_TOC_SIZE 0x200

/* BACKUP_RAM memory mapping */
#define BACKUP_RAM_BASE 0x20000000
#define BACKUP_RAM_SIZE 0x200
#define BACKUP_RAM_ROM_BOOT_API_BASE (BACKUP_RAM_BASE + 0x8)	/* 0x20000008 */
#define BACKUP_RAM_ROM_BOOT_API_SIZE 0x14
#define BACKUP_RAM_TUNER_CONTEXT_BASE (BACKUP_RAM_BASE + 0x1c)	/* 0x2000001c */
#define BACKUP_RAM_TUNER_CONTEXT_SIZE 0x20
#define BACKUP_RAM_AUDIO_CONTEXT_BASE (BACKUP_RAM_BASE + 0x3c)	/* 0x2000003c */
#define BACKUP_RAM_AUDIO_CONTEXT_SIZE 0x20
#define BACKUP_RAM_DDR_CHECKSUM_BASE (BACKUP_RAM_BASE + 0x1ec)	/* 0x200001ec */
#define BACKUP_RAM_DDR_CHECKSUM_SIZE 0x4
#define BACKUP_RAM_DDR_ZQ_CAL_BASE (BACKUP_RAM_BASE + 0x1f0)	/* 0x200001f0 */
#define BACKUP_RAM_DDR_ZQ_CAL_SIZE 0x4
#define BACKUP_RAM_STR_TAG_BASE (BACKUP_RAM_BASE + 0x1f8)	/* 0x200001f8 */
#define BACKUP_RAM_STR_TAG_SIZE 0x4
#define BACKUP_RAM_STR_RESUME_ADDR_BASE (BACKUP_RAM_BASE + 0x1fc)	/* 0x200001fc */
#define BACKUP_RAM_STR_RESUME_ADDR_SIZE 0x4

/* ESRAM_A7 memory mapping */
#define ESRAM_A7_BASE 0x70000000
#define ESRAM_A7_SIZE 0x80000
#define ESRAM_A7_ATF_TRUSTED_ZONE_BASE (ESRAM_A7_BASE + 0x0)	/* 0x70000000 */
#define ESRAM_A7_ATF_TRUSTED_ZONE_SIZE 0x80000
#define ESRAM_A7_ATF_BL1_BL2_BASE (ESRAM_A7_ATF_TRUSTED_ZONE_BASE + 0x0)	/* 0x70000000 */
#define ESRAM_A7_ATF_BL1_BL2_SIZE 0x1ec00
#define ESRAM_A7_C3_PROGRAMS_BASE (ESRAM_A7_ATF_TRUSTED_ZONE_BASE + 0x1ec00)	/* 0x7001ec00 */
#define ESRAM_A7_C3_PROGRAMS_SIZE 0x1000
#define ESRAM_A7_ATF_MAILBOXES_BASE (ESRAM_A7_ATF_TRUSTED_ZONE_BASE + 0x1fdb0)	/* 0x7001fdb0 */
#define ESRAM_A7_ATF_MAILBOXES_SIZE 0x10
#define ESRAM_A7_COT_CCC_DATA_BASE (ESRAM_A7_ATF_TRUSTED_ZONE_BASE + 0x1fdc0)	/* 0x7001fdc0 */
#define ESRAM_A7_COT_CCC_DATA_SIZE 0x40
#define ESRAM_A7_ATF_IPC_BASE (ESRAM_A7_ATF_TRUSTED_ZONE_BASE + 0x1fe00)	/* 0x7001fe00 */
#define ESRAM_A7_ATF_IPC_SIZE 0x200
#define ESRAM_A7_ATF_BL32_BASE (ESRAM_A7_ATF_TRUSTED_ZONE_BASE + 0x20000)	/* 0x70020000 */
#define ESRAM_A7_ATF_BL32_SIZE 0x40000
#define ESRAM_A7_M3_CODE_BASE (ESRAM_A7_ATF_TRUSTED_ZONE_BASE + 0x60000)	/* 0x70060000 */
#define ESRAM_A7_M3_CODE_SIZE 0x20000

/* DDRAM memory mapping */
#define DDRAM_BASE 0xa0000000
#define DDRAM_SIZE 0x20000000
#define DDRAM_ATF_UNTRUSTED_ZONE_BASE (DDRAM_BASE + 0x0)	/* 0xa0000000 */
#define DDRAM_ATF_UNTRUSTED_ZONE_SIZE 0x1c000000
#define DDRAM_APP_OS_SYSTEM_MEM_BASE (DDRAM_ATF_UNTRUSTED_ZONE_BASE + 0x0)	/* 0xa0000000 */
#define DDRAM_APP_OS_SYSTEM_MEM_SIZE 0x13300000
#define DDRAM_RAMDUMP_MAGIC_BASE (DDRAM_APP_OS_SYSTEM_MEM_BASE + 0x131ff000)	/* 0xb31ff000 */
#define DDRAM_RAMDUMP_MAGIC_SIZE 0x1000
#define DDRAM_RAMOOPS_RAMOOPS_MEM_BASE (DDRAM_APP_OS_SYSTEM_MEM_BASE + 0x13200000)	/* 0xb3200000 */
#define DDRAM_RAMOOPS_RAMOOPS_MEM_SIZE 0x100000
#define DDRAM_SHARED_DATA_BASE (DDRAM_ATF_UNTRUSTED_ZONE_BASE + 0x13300000)	/* 0xb3300000 */
#define DDRAM_SHARED_DATA_SIZE 0x1000
#define DDRAM_APP_OS_MAILBOXES_BASE (DDRAM_ATF_UNTRUSTED_ZONE_BASE + 0x13301000)	/* 0xb3301000 */
#define DDRAM_APP_OS_MAILBOXES_SIZE 0x1000
#define DDRAM_TEE_SHMEM_BASE (DDRAM_ATF_UNTRUSTED_ZONE_BASE + 0x13400000)	/* 0xb3400000 */
#define DDRAM_TEE_SHMEM_SIZE 0x200000
#define DDRAM_M3_OS_HEAP_BASE (DDRAM_ATF_UNTRUSTED_ZONE_BASE + 0x13600000)	/* 0xb3600000 */
#define DDRAM_M3_OS_HEAP_SIZE 0xa00000
#define DDRAM_APP_OS_GCNANO_POOL_BASE (DDRAM_ATF_UNTRUSTED_ZONE_BASE + 0x14000000)	/* 0xb4000000 */
#define DDRAM_APP_OS_GCNANO_POOL_SIZE 0x8000000
#define DDRAM_ATF_TRUSTED_ZONE_BASE (DDRAM_BASE + 0x1c000000)	/* 0xbc000000 */
#define DDRAM_ATF_TRUSTED_ZONE_SIZE 0x4000000
#define DDRAM_ATF_BLOCKDEV_DATA_BASE (DDRAM_ATF_TRUSTED_ZONE_BASE + 0x0)	/* 0xbc000000 */
#define DDRAM_ATF_BLOCKDEV_DATA_SIZE 0x80000
#define DDRAM_ATF_BL32_SAVED_BASE (DDRAM_ATF_TRUSTED_ZONE_BASE + 0x80000)	/* 0xbc080000 */
#define DDRAM_ATF_BL32_SAVED_SIZE 0x80000
#define DDRAM_ATF_M3OS_SAVED_BASE (DDRAM_ATF_TRUSTED_ZONE_BASE + 0x100000)	/* 0xbc100000 */
#define DDRAM_ATF_M3OS_SAVED_SIZE 0x40000
#define DDRAM_DDR_TRAINING_BASE (DDRAM_ATF_TRUSTED_ZONE_BASE + 0x140000)	/* 0xbc140000 */
#define DDRAM_DDR_TRAINING_SIZE 0x400
#define DDRAM_ATF_TEE_BASE (DDRAM_ATF_TRUSTED_ZONE_BASE + 0x3000000)	/* 0xbf000000 */
#define DDRAM_ATF_TEE_SIZE 0x1000000

#endif