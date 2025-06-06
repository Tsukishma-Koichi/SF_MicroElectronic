/***************************************************************************//**
* \file cy_srom.h
* \version 1.0
*
* \brief
* Provides declaration of API controlling the SROM APIs
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/* This driver is created by referring SROM BROS (002-03298) rev 0W. */
/* Please note SROM API is subject to update in the future. */

#if !defined(CY_SROM_H)
#define CY_SROM_H

#include "syslib/cy_syslib.h"
#include "ipc/cy_ipc_drv.h"
#include "cy_device_headers.h"
#include "sysint/cy_sysint.h"

#pragma pack(1)

typedef void (*cy_srom_handler)(void);

#define CY_SROM_DR_IPC_NOTIFY_STRUCT (0x1ul)
#define CY_SROM_DR_IPC_INTR_STRUCT   (0x2ul)
#define CY_SROM_DR_IPC_INTR_NO       (cpuss_interrupts_ipc_2_IRQn)

typedef enum
{
    CY_SROM_OP_SILICON_ID               = (0x00ul),
    CY_SROM_OP_READ_FUSE_BYTE           = (0x03ul),
    CY_SROM_OP_READ_FUSE_BYTE_MARGIN    = (0x2Bul),
    CY_SROM_OP_READ_UNIQUE_ID           = (0x1Ful),
    CY_SROM_OP_SWITCH_REGULATOR         = (0x11ul),
    CY_SROM_OP_LoadRegulatorTrim        = (0x16ul),
    CY_SROM_OP_CONFIGURE_REGULATOR      = (0x15ul),
    CY_SROM_OP_BLOW_FUSE_BIT            = (0x01ul),
    CY_SROM_OP_FLASH_PROGRAM_ROW        = (0x06ul),
    CY_SROM_OP_FLASH_PROGRAM_ROW2       = (0x09ul),
    CY_SROM_OP_FLASH_CHECKSUM           = (0x0Bul),
    CY_SROM_OP_FLASH_CHECKSUM2          = (0x19ul),
    CY_SROM_OP_FLASH_HASH               = (0x0Dul),
    CY_SROM_OP_FLASH_ERASE_ALL          = (0x0Aul),
    CY_SROM_OP_FLASH_ERASE_ALL2         = (0x18ul),
    CY_SROM_OP_FLASH_ERASE_SECTOR       = (0x14ul),
    CY_SROM_OP_FLASH_ERASE_SECTOR2      = (0x1Cul),
    CY_SROM_OP_FLASH_FM_STATUS          = (0x07ul),
    CY_SROM_OP_FLASH_FM_STATUS2         = (0x0Cul),
    CY_SROM_OP_FLASH_FM_INTR            = (0x08ul),
    CY_SROM_OP_FLASH_FM_INTR2           = (0x17ul),
    CY_SROM_OP_FLASH_ERASE_SUSPEND      = (0x22ul),
    CY_SROM_OP_FLASH_ERASE_SUSPEND2     = (0x25ul),
    CY_SROM_OP_FLASH_ERASE_RESUME       = (0x23ul),
    CY_SROM_OP_FLASH_ERASE_RESUME2      = (0x26ul),
    CY_SROM_OP_FLASH_BLANK_CHECK        = (0x2Aul),
    CY_SROM_OP_FLASH_PROGRAM_WF         = (0x30ul),
    CY_SROM_OP_FLASH_PROGRAM_WF2        = (0x31ul),
} cy_en_srom_api_opcode_t;

typedef enum
{
    CY_SROM_STATUS_SUCCESS                             = (0x00000000ul), // SROM PASS. Note this is driver specific value.
    CY_SROM_STATUS_INVALID                             = (0x0FFFFFFFul), // Status Value invalid. Note this is driver specific value.
    CY_SROM_STATUS_INVALID_PROTECTION                  = (int32_t)(0xF0000001ul), // Invalid Protection state. This API is not available in current protection state.
    CY_SROM_STATUS_INVALID_FUSE_ADDR                   = (int32_t)(0xF0000002ul), // Invalid efuse address.This status is returned when BlowFuseBit is called on out-of-bound eFuse.
    CY_SROM_STATUS_INVALID_FM_PL                       = (int32_t)(0xF0000003ul), // Invalid FLASH page latch address
    CY_SROM_STATUS_INVALID_FLASH_ADDR                  = (int32_t)(0xF0000004ul), // Wrong or out of bound FLASH address
    CY_SROM_STATUS_NVM_PROTECTED                       = (int32_t)(0xF0000005ul), // Non-Volatile Memory(FLASH or eFuse) bytes are read/write protected via protection units.Previously called as STATUS_ROW_PROTECTED.
    CY_SROM_STATUS_INVALID_IPC_STRUCT                  = (int32_t)(0xF0000006ul), // Client did not use its reserved IPC structure for invoking system call.
    CY_SROM_STATUS_ADDR_PROTECTED                      = (int32_t)(0xF0000008ul), // Returned by all APIs when client doesn't have access to region it is using for passing arguments.
    CY_SROM_STAUTS_CHECKSUM_NON_ZERO                   = (int32_t)(0xF000000Aul), // Checksum of FLASH resulted in non-zero
    CY_SROM_STATUS_INVALID_OPCODE                      = (int32_t)(0xF000000Bul), // The opcode is not a valid API opcode.
    CY_SROM_STATUS_INVALID_ADDR_RANGE                  = (int32_t)(0xF000000Eul), // Returned when ConfigureRegionBulk is called with start address greater than end address or EraseAll called in dual bank mode for SONOS.
    CY_SROM_STATUS_INVALID_ARGUMENTS                   = (int32_t)(0xF000000Ful), // Returned when invalid arguments are passed to the API. For e.g calling Silicon ID API with ID type of 0x5.
    CY_SROM_STATUS_BOOT_FLASH_AUTH_FAIL                = (int32_t)(0xF0000010ul), // HASH stored in efuse does not match to computed HASH
    CY_SROM_STATUS_TEST_KEY_DFT_EN                     = (int32_t)(0xF0000011ul), // Indicates that TEST_KEY_DFT_EN was set during boot up
    CY_SROM_STATUS_TEST_KEY_SAFE_MODE                  = (int32_t)(0xF0000012ul), // Indicates that TST_KEY_SAFE_MODE was set during boot up
    CY_SROM_STATUS_INVALID_ARGUMENT_LOCATION           = (int32_t)(0xF0000013ul), // User has provided arguments in region other than available user SRAM or not 32-bit
    CY_SROM_STATUS_NON_NMI_ENTRY                       = (int32_t)(0xF0000014ul), // NMI Handler function was executed under non-NMI mode.
    CY_SROM_STATUS_INVALID_TRIM_LEN                    = (int32_t)(0xF0000015ul), // General trims length was found to be less than 2
    CY_SROM_STATUS_INVALID_HASH_ZEROS                  = (int32_t)(0xF0000017ul), // Number of zeros in the HASH computed by ROM boot & number of zeros stored in eFuse don't match.
    CY_SROM_STATUS_INVALID_TOC1                        = (int32_t)(0xF0000018ul), // Table of contents 1's CRC check failed or /and magic number is not present
    CY_SROM_STATUS_CM4_NOT_IN_DEEPSLEEP                = (int32_t)(0xF0000019ul), // CM4 only reset requested by SoftReset when CM4 is not in deepsleep.
    CY_SROM_STATUS_ALT_FLASH_BOOT_AUTH_FAIL            = (int32_t)(0xF000001Aul), // Returned during secure boot if SFLASH bank 1 authentication check fails.
    CY_SROM_STATUS_INVALID_TOC2                        = (int32_t)(0xF0000020ul), // Table of contents 2's CRC check failed or /and magic number is not present
    CY_SROM_STATUS_SR_ILLEGAL_OP                       = (int32_t)(0xF0000021ul), // Bulk operation requested on SFLASH region.
    CY_SROM_STATUS_OPERATION_NOT_ALLOWED               = (int32_t)(0xF0000022ul), // Returned when flash embedded operations are invoked during margin mode operation.
    CY_SROM_STATUS_MM_ERROR                            = (int32_t)(0xF0000030ul), // Returned by FlashMarginScreenArray SROM API when there is a margin mode failure
    CY_SROM_STATUS_INVALID_CY_ASSET_HASH               = (int32_t)(0xF0000053ul), // Calculated CY_ASSET_HASH doesn't match to the one stored in efuse unused 0xF0000054
    CY_SROM_STATUS_ENABLE_FB_CHECK_BIT_NOT_BLOWN       = (int32_t)(0xF0000055ul), // Returned by TransitionToSecure if he ENABLE_FB_CHECK_IN_NORMAL_is not blown
    CY_SROM_STATUS_CY_ASSET_HASH_MAX_OBJECTS           = (int32_t)(0xF0000056ul), // If the number of the CY_ASSET_HASH exceeds maximum value
    CY_SROM_STATUS_INVALID_FB_CHECK_EFUSE              = (int32_t)(0xF00000ACul), // The ENABLE_FB_CHECK_IN_NORMAL fuse is corrupted
    CY_SROM_STATUS_CALIBRATE_FAILED                    = (int32_t)(0xF0000080ul), // FLASH trim hunt failed for SONOS. For eCT magic number was not found in TOC1
    CY_SROM_STATUS_TURBON_FAILED                       = (int32_t)(0xF0000081ul), // turbo is low during  FmCalibrate
    CY_SROM_STATUS_ISOLATED_FAILED                     = (int32_t)(0xF0000082ul), // Isolate is high during FmCalibrate
    CY_SROM_STATUS_ERROR_CORRECTION_FAILED             = (int32_t)(0xF0000086ul), // FLASH APIs were not able to retrieve majority data when fetching HV parameters
    CY_SROM_STATUS_SECTOR_SUSPEND                      = (int32_t)(0xF0000091ul), // Returned when Program operation is called on sector which is suspended from erase.
    CY_SROM_STATUS_NO_ERASE_SUSPEND                    = (int32_t)(0xF0000092ul), // Returned when EraseResume is called when no sector is suspended from erase.
    CY_SROM_STATUS_API_NOT_APPROVED                    = (int32_t)(0xF0000095ul), // Returned by API when the requested system call is not approved by TEE
    CY_SROM_STATUS_POR_NATIVE                          = (int32_t)(0xF00000A0ul), // Returned when FUR download fails with POR_NATIVE=1.
    CY_SROM_STATUS_FUR_ECC_ERROR                       = (int32_t)(0xF00000A1ul), // Returned when FUR download fails due to ECC error.
    CY_SROM_STATUS_IRAM_ECC_ERROR                      = (int32_t)(0xF00000A2ul), // Returned when IRAM download fails due to ECC error.
    CY_SROM_STATUS_FM_ECC_ERROR                        = (int32_t)(0xF00000A3ul), // Returned when FM fails due to ECC error.
    CY_SROM_STATUS_FLASH_NOT_ERASED                    = (int32_t)(0xF00000A4ul), // Returned when ProgramRow is invoked on unerased cells or blank check fails.
    CY_SROM_STATUS_NO_ERASE_ONGOING                    = (int32_t)(0xF00000A5ul), // Returned by EraseSuspend when called with no ongoing erase operation
    CY_SROM_STATUS_ACTIVE_ERASE                        = (int32_t)(0xF00000A6ul), // Returned by ProgramRow when active erase operation is going on.
    CY_SROM_STATUS_EMB_FAIL                            = (int32_t)(0xF00000A7ul), // Returned by FLASH SROM APIs when the embedded operation fails.
    CY_SROM_STAUTS_INVALID_DATA_WIDTH                  = (int32_t)(0xF00000A8ul), // Returned by ProgramRow API if invalid program width option is provided
    CY_SROM_STATUS_EMB_ACTIVE                          = (int32_t)(0xF00000A9ul), // Returned by WriteRow when invoked during an active embedded operation.
    CY_SROM_STATUS_FLASH_SAFTEY_ENABLED                = (int32_t)(0xF00000AAul), // Returned by FLASH program/erase APIs when writes are disabled in safety register.
    CY_SROM_STATUS_INVALID_BANK_MODE                   = (int32_t)(0xF00000ABul), // Returned by WriteRow when invoked in dual bank mode.
    CY_SROM_STATUS_INVALID_NUM_ENTRIES                 = (int32_t)(0xF00000B0ul), // Returned by WriteTOC2 when invalid number of entries are passed
    CY_SROM_STATUS_INVALID_ACCESS_RESTRICTION          = (int32_t)(0xF00000B1ul), // Returned when WriteNormalAccessRestrict is called to restrict less
    CY_SROM_STATUS_INVALID_SFLASH_ADDR                 = (int32_t)(0xF00000B2ul), // Returned when WriteRow is called on invalid SFLASH rows in NORMAL state or TOC/patch entries are not in SFLASH
    CY_SROM_STATUS_INVALID_UID                         = (int32_t)(0xF00000B3ul), // Returned by when invalid Unique ID is passed during RMA
    CY_SROM_STATUS_INVALID_SIGN                        = (int32_t)(0xF00000B4ul), // Returned when invalid signature is found during RMA
    CY_SROM_STATUS_INVALID_FACTORY_HASH                = (int32_t)(0xF00000B5ul), // Returned when FACTORY_HASH1(in NORMAL1) or FACTORY_HASH2(in NORMAL2) does not match
    CY_SROM_STATUS_INVALID_TOC1_HASH_OBJECT            = (int32_t)(0xF00000B8ul), // Returned when more than 30(for PSOC6, 10 for Traveo II) HASH objects are indicated in TOC1
    CY_SROM_STATUS_INVALID_TOC2_HASH_OBJECT            = (int32_t)(0xF00000B9ul), // Returned when more than 30(for PSOC6, 10 for Traveo II) HASH  objects are indicated in TOC2
    CY_SROM_STATUS_INVALID_PUBLIC_KEY                  = (int32_t)(0xF00000BAul), // Returned by TransitionRMA & OpenRMA when public key structure is invalid
    CY_SROM_STATUS_SFLASH_BACKUP_ERASED                = (int32_t)(0xF00000BBul), // Returned by Sflash programming APIs when backup sector is in erased state.
    CY_SROM_STATUS_INVALID_PU1_FLASH_WRITE_NR          = (int32_t)(0xF00000BCul), // Returned during boot when SWPU in SFLASH is more than expected
    CY_SROM_STATUS_INVALID_PU1_FUSE_WRITE_NR           = (int32_t)(0xF00000BDul), // Returned during boot when SWPU in SFLASH is more than expected
    CY_SROM_STATUS_INVALID_PU1_FUSE_READ_NR            = (int32_t)(0xF00000BEul), // Returned during boot when SWPU in SFLASH is more than expected
    CY_SROM_STATUS_INVALID_PU2_FLASH_WRITE_NR          = (int32_t)(0xF00000BFul), // Returned during boot when SWPU in SFLASH is more than expected
    CY_SROM_STATUS_INVALID_PU2_FUSE_WRITE_NR           = (int32_t)(0xF00000C0ul), // Returned during boot when SWPU in SFLASH is more than expected
    CY_SROM_STATUS_INVALID_PU2_FUSE_READ_NR            = (int32_t)(0xF00000C1ul), // Returned during boot when SWPU in SFLASH is more than expected
    CY_SROM_STATUS_INVALID_PU_ID                       = (int32_t)(0xF00000C2ul), // Returned by Read or WriteSWPU API when invalid ID is passed.
    CY_SROM_STATUS_PU_PROTECTED                        = (int32_t)(0xF00000C3ul), // Returned by WriteSWPU API when client doesn't have access to update SWPU
    CY_SROM_STATUS_PU_MISMATCH                         = (int32_t)(0xF00000C4ul), // Returned by WriteSWPU API when client doesn't providing matching SL_ADDR & SL_SIZE.
    CY_SROM_STATUS_PU_READ_ECC_ERROR                   = (int32_t)(0xF00000C5ul), // Returned by ReadSWPU API if ECC error occurred during SRAM read operation.
    CY_SROM_STATUS_PU_REJECTED                         = (int32_t)(0xF00000C6ul), // Returned by Read & WriteSWPU API if the IDs PU was rejected during boot due to overlap or out-of-order region.
    CY_SROM_STATUS_PENDING_ECC_ERROR                   = (int32_t)(0xF00000C7ul), // Returned by Read & Write SWPU APIs if there was a pending ECC error before performing SWPU operations
    CY_SROM_STATUS_INVALID_LIFECYCLE_FUSE              = (int32_t)(0xF00000C8ul), // Returned during boot if valid lifecycle fuse combinations are not read from eFuse.
    CY_SROM_STATUS_FUSE_PGM_FAILED                     = (int32_t)(0xF00000CBul), // Returned by BlowFuseBit API when read value from programmed fuse is 0
    CY_SROM_STATUS_ARG_PROTECTED                       = (int32_t)(0xF00000CFul), // User has provided arguments in protected region.
    CY_SROM_STATUS_EFUSE_BOOTROW_NON_ZERO              = (int32_t)(0xF00000D1ul), // Returned by TransitionToSort() in case if the bootrow is not zero in VIRGIN
    CY_SROM_STATUS_SRAM_REPAIR_FAILED                  = (int32_t)(0xF00000D2ul), // Returned by Boot if SRAM BIHR repair operation fails
    CY_SROM_STATUS_SRAM_REPAIR_FUSE_FAILED             = (int32_t)(0xF00000D3ul), // Returned by Boot if SRAM repair fuse redundancy check fails
    CY_SROM_STATUS_SFLASH_CORRUPTED                    = (int32_t)(0xF00000D5ul), // Returned when SFLASH markers are corrupted during boot.
    CY_SROM_STATUS_MARKER_OVERFLOW                     = (int32_t)(0xF00000D6ul), // Returned by WriteRow when marker overflows by 2^32 times.
    CY_SROM_STATUS_CM7_X_NOT_IN_DEEPSLEEP              = (int32_t)(0xF00000D7ul), // Returned by SoftReset API when CM7 reset is requested with CM7 not being in deepsleep mode.
    CY_SROM_STATUS_INVALID_LIFECYCLE                   = (int32_t)(0xF00000D8ul), // Operation is not supported in this LifeCycle  (TVII products only)
    CY_SROM_STATUS_INVALID_HASH_OBJECT_ADDRESS         = (int32_t)(0xF00000DAul), // Returned when TOC object StartAddr and EndAddr(TOC Object StartAddr+ TOC Object Size) are out of bound of SFLASH/OTP.
    CY_SROM_STATUS_REGULATOR_MODE_MANUAL               = (int32_t)(0xF00000E0ul), // Regulator is configured for manual mode
    CY_SROM_STATUS_REGULATOR_SEQ_BUSY                  = (int32_t)(0xF00000E1ul), // Regulator is currently in transition.
    CY_SROM_STATUS_REGULATOR_ALREADY_ENABLED           = (int32_t)(0xF00000E2ul), // Regulator is already enabled
    CY_SROM_STATUS_REGULATOR_NOT_CONFIGURED            = (int32_t)(0xF00000E3ul), // Regulator is not configured with ConfigureRegulator()
    CY_SROM_STATUS_REGULATOR_WRONG_CONFIGURATION       = (int32_t)(0xF00000E4ul), // Returned by SwitchOverRegulators() in case when the syscall is called with different OpMode parameter then ConfigureRegulator().
    CY_SROM_STATUS_WRITE_DEVICE_KEYS_TRIVIAL_KEY       = (int32_t)(0xF00000E5ul), // -TBD
    CY_SROM_STATUS_WRITE_DEVICE_KEYS_NON_ZERO_KEYS     = (int32_t)(0xF00000E6ul), // - TBD
    CY_SROM_STATUS_WRITE_DEVICE_KEYS_WRONG_LIFE_CYCLE  = (int32_t)(0xF00000E7ul), // - TBD
    CY_SROM_STATUS_WRITE_DEVICE_KEYS_MISMATCHED_KEYS   = (int32_t)(0xF00000E8ul), // - TBD
    CY_SROM_STATUS_WRITE_DEVICE_KEYS_WRONG_EFUSE_ZEROS = (int32_t)(0xF00000E9ul), // - TBD
    CY_SROM_STATUS_WRITE_DEVICE_KEYS_CORRUPTED_P_BITS  = (int32_t)(0xF00000EAul), // - TBD
    CY_SROM_STATUS_ERROR_SYSCALL0                      = (int32_t)(0xF00000EBul), // - TBD
    CY_SROM_STATUS_ERROR_SYSCALL1                      = (int32_t)(0xF00000ECul), // - TBD
    CY_SROM_STATUS_ERROR_SYSCALL2                      = (int32_t)(0xF00000EDul), // - TBD
    CY_SROM_STATUS_ERROR_SYSCALL3                      = (int32_t)(0xF00000EEul), // - TBD
    CY_SROM_STATUS_ERROR_SYSCALL4                      = (int32_t)(0xF00000EFul), // - TBD
    CY_SROM_STATUS_HARDFAULT_BOOT                      = (int32_t)(0xF00000F0ul), // Returned during boot if hard fault occurs during bootup
    CY_SROM_STATUS_HARDFAULT_SYSCALL                   = (int32_t)(0xF00000F1ul), // Returned by system call if hard fault occurs in context of system calls
    CY_SROM_STATUS_INVALID_PROG_PPU_ACCESS             = (int32_t)(0xF4000000ul), // Returned when system call(PC1) doesn't have access to the programmable PPU it uses. The programmable PPU ID is returned in LSB.
    CY_SROM_STATUS_INVALID_FIXED_PPU_ACCESS            = (int32_t)(0xF5000000ul), // Returned when system call(PC1) doesn't have access to the fixed PPU it uses. The fixed PPU ID is returned in LSB.
    CY_SROM_STATUS_BOOTROW_MMIO_MISMATCH               = (int32_t)(0xF6000019ul), // Returned when bootrow fuse and MMIO doesn't match
    CY_SROM_STATUS_BOOTROW_KEY_MISMATCH                = (int32_t)(0xF6000029ul), // Returned when key in the bootrow is not 0x29
    CY_SROM_STATUS_BOOTROW_TRIM_MISMATCH               = (int32_t)(0xF6000039ul), // Returned when trim and trim inverse in bootrow are not equal
    CY_SROM_STATUS_BOOTROW_FUSE_MISMATCH               = (int32_t)(0xF6000049ul), // Returned when lifecycle fuses fail its redundancy check.
    CY_SROM_STATUS_BOOTROW_INVALID_COMB                = (int32_t)(0xF6000059ul), // Returned when invalid lifecycle fuse combinations are blown
    CY_SROM_STATUS_HASH_FAILED                         = (int32_t)(0xF1000000ul), // Hash on SFLASH trims failed. The computed  hash is ORed with this status.
    CY_SROM_STATUS_FUSE_ZERO_FAILED                    = (int32_t)(0xF2000000ul), // Number of zeroes stored in efuse does not match to computed number
    CY_SROM_STATUS_BOOT_FAULT_MASK                     = (int32_t)(0xF3000000ul), // Returned during boot in IPC_STRUCT0.DATA1 if fault structure 0 valid bit is set. The LSBs will hold the fault ID information.
    CY_SROM_CY_FB_ERROR_INVALID_APP_SIGN               = (int32_t)(0xF1000100ul), // App signature validation failed.
    CY_SROM_CY_FB_ERROR_INVALID_TOC                    = (int32_t)(0xF1000101ul), // Empty or Invalid TOC
    CY_SROM_CY_FB_ERROR_INVALID_KEY                    = (int32_t)(0xF1000102ul), // Invalid Public Key
    CY_SROM_CY_FB_ERROR_UNREACHABLE                    = (int32_t)(0xF1000103ul), // Unreachable Code
    CY_SROM_CY_FB_ERROR_TOC_DATA_CLOCK                 = (int32_t)(0xF1000104ul), // TOC contains invalid CM0+ clock attribute.
    CY_SROM_CY_FB_ERROR_TOC_DATA_DELAY                 = (int32_t)(0xF1000105ul), // TOC contains invalid listen window delay
    CY_SROM_CY_FB_ERROR_FLL_CONFIG                     = (int32_t)(0xF1000106ul), // FLL configuration failed
    CY_SROM_CY_FB_ERROR_INVALID_APP_DATA               = (int32_t)(0xF1000107ul), // Invalid application structure
} cy_en_srom_api_status_t;

typedef enum
{ 
    CY_SROM_DR_SUCCEEDED         = 0,
    CY_SROM_DR_IPC_BUSY          = 1,
    CY_SROM_DR_TIMEOUT           = 2,
    CY_SROM_DR_API_ERROR         = 3,
    CY_SROM_DR_API_UNKNOWN       = 4,
    CY_SROM_DR_INVALID_INPUT     = 5,
} cy_en_srom_driver_status_t;

typedef enum
{ 
    CY_SROM_RESPONSE_SUCCESS = 0,
    CY_SROM_RESPONSE_FAIL    = 1,
    CY_SROM_NOT_RESPONSE     = 2,
} cy_en_srom_response_type_t;


/**********************************/
/*** Silicon ID Data Structures ***/
/**********************************/

/* Request Message */
typedef struct
{
    uint32_t        : 8;
    uint32_t IdType : 8;
    uint32_t        : 8;
    uint32_t Opcode : 8;
} cy_stc_silicon_id_arg0_t;

typedef struct
{
    cy_stc_silicon_id_arg0_t arg0;
    uint32_t                 resv[3];
} cy_stc_silicon_id_args_t;

/* Response Message */
typedef struct
{
    uint32_t FamilyIdLow     : 8;
    uint32_t FamilyIdHigh    : 8;
    uint32_t MinorRevisionID : 4;
    uint32_t MajorRevisionId : 4;
    uint32_t                 : 4;
    uint32_t StatusCode      : 4;
} cy_stc_silicon_id_resp_type0_t;

typedef struct
{
    uint32_t SiliconIdLow    : 8;
    uint32_t SiliconIdHigh   : 8;
    uint32_t ProtectionState : 4;
    uint32_t LifeCycleState  : 4;
    uint32_t                 : 4;
    uint32_t StatusCode      : 4;
} cy_stc_silicon_id_resp_type1_t;

typedef enum
{
    CY_LIFE_CYCLE_STATE_VIRGIN             = 0,
    CY_LIFE_CYCLE_STATE_NORMAL             = 1,
    CY_LIFE_CYCLE_STATE_SEC_W_DBG          = 2,
    CY_LIFE_CYCLE_STATE_SECURE             = 3,
    CY_LIFE_CYCLE_STATE_RMA                = 4,
    CY_LIFE_CYCLE_STATE_SORT               = 5,
    CY_LIFE_CYCLE_STATE_PROVISIONED        = 6,
    CY_LIFE_CYCLE_STATE_NORMAL_PROVISIONED = 7,
    CY_LIFE_CYCLE_STATE_NORMAL_RMA         = 8,
    CY_LIFE_CYCLE_STATE_CORRUPTED          = 9,
} cy_en_life_cycle_state_t;

typedef enum
{
    CY_PROTECTION_STATE_UNKNOWN = 0,
    CY_PROTECTION_STATE_VIRGIN  = 1,
    CY_PROTECTION_STATE_NORMAL  = 2,
    CY_PROTECTION_STATE_SECURE  = 3,
    CY_PROTECTION_STATE_DEAD    = 4,
} cy_en_protection_state_t;

typedef struct
{
    uint32_t SROM_FW_MinorVersion  : 8;
    uint32_t SROM_FW_MajorVersion  : 8;
    uint32_t FlashBootMinorVersion : 8;
    uint32_t FlashBootMajorVersion : 4;
    uint32_t StatusCode            : 4;
} cy_stc_silicon_id_resp_type2_t;

typedef union
{
    uint32_t                       u32;
    cy_stc_silicon_id_resp_type0_t IdType0;
    cy_stc_silicon_id_resp_type1_t IdType1;
    cy_stc_silicon_id_resp_type2_t IdType2;
} cy_un_slicon_id_resp0_t;

typedef struct
{
    cy_un_slicon_id_resp0_t resp0;
    uint32_t                resv[3];
} cy_stc_slicon_id_resps_t;

/**************************************/
/*** Read Unique ID Data Structures ***/
/**************************************/
/* Request Message */
typedef struct
{
    uint32_t        : 24;
    uint32_t Opcode : 8;
} cy_stc_rd_unique_id_arg0_t;

typedef struct
{
    cy_stc_rd_unique_id_arg0_t arg0;
    uint32_t                   resv[3];
} cy_stc_rd_unique_id_args_t;

/* Response Message */
typedef struct
{
    uint32_t ErrorCode  : 24;
    uint32_t            : 4;
    uint32_t StatusCode : 4;
} cy_stc_rd_unique_id_resp0_t;

typedef struct
{
    uint32_t DIE_ID0  : 32;
} cy_stc_rd_unique_id_resp1_t;

typedef struct
{
    uint32_t DIE_ID1  : 32;
} cy_stc_rd_unique_id_resp2_t;

typedef struct
{
    cy_stc_rd_unique_id_resp0_t resp0;
    cy_stc_rd_unique_id_resp1_t resp1;
    cy_stc_rd_unique_id_resp2_t resp2;
    uint32_t                    resp3;
} cy_stc_rd_unique_id_resps_t;

/*********************************/
/*** Read Fuse Byte Structures ***/
/*********************************/
#define CY_SROM_EFUSE_RESERVED_NUM (104)
#define CY_SROM_EFUSE_CUSTOMER_DATA_NUM (24)
typedef struct
{
    uint8_t Reserved[CY_SROM_EFUSE_RESERVED_NUM];
    uint8_t CustomerData[CY_SROM_EFUSE_CUSTOMER_DATA_NUM];
} cy_stc_efuse_byte_map_t;

/* Request Message */
typedef struct
{
    uint32_t           : 8;
    uint32_t eFuseAddr : 16;
    uint32_t Opcode    : 8;
} cy_stc_rd_efuse_byte_arg0_t;

typedef struct
{
    cy_stc_rd_efuse_byte_arg0_t arg0;
    uint32_t                    resv[3];
} cy_stc_rd_efuse_byte_args_t;

/* Response Message */
typedef struct
{
    uint32_t ReadByte   : 8;
    uint32_t            : 20;
    uint32_t StatusCode : 4;
} cy_stc_rd_efuse_byte_resp0_t;

typedef struct
{
    cy_stc_rd_efuse_byte_resp0_t resp0;
    uint32_t                     resv[3];
} cy_stc_rd_efuse_byte_resps_t;


/*********************************/
/*** Read Fuse Byte Margin Structures ***/
/*********************************/

/** Value for the marginal control while reading eFuse */
typedef enum
{
    CY_SROM_READ_MARGIN_RES_LOW         = 0,
    CY_SROM_READ_MARGIN_RES_NOMINAL     = 1,
    CY_SROM_READ_MARGIN_RES_HIGH        = 2,
} cy_en_srom_read_margin_control_t;

/* Request Message */
typedef struct
{
    uint32_t           : 8;
    uint32_t eFuseAddr : 12;
    uint32_t marginCtl : 4;
    uint32_t Opcode    : 8;
} cy_stc_rd_efuse_byte_margin_arg0_t;

typedef struct
{
    cy_stc_rd_efuse_byte_margin_arg0_t  arg0;
    uint32_t                            resv[3];
} cy_stc_rd_efuse_byte_margin_args_t;

/* Response Message */
typedef struct
{
    uint32_t ReadByte   : 8;
    uint32_t            : 20;
    uint32_t StatusCode : 4;
} cy_stc_rd_efuse_byte_margin_resp0_t;

typedef struct
{
    cy_stc_rd_efuse_byte_margin_resp0_t resp0;
    uint32_t                            resv[3];
} cy_stc_rd_efuse_byte_margin_resps_t;


/*********************************/
/*** Blow Fuse Bit Structures  ***/
/*********************************/

/* Request Message */
typedef struct
{
    uint32_t           : 8;
    uint32_t bitAddr   : 3;
    uint32_t           : 1;
    uint32_t macroAddr : 4;
    uint32_t byteAddr  : 8;
    uint32_t Opcode    : 8;
} cy_stc_wr_efuse_blow_bit_arg0_t;

typedef struct
{
    cy_stc_wr_efuse_blow_bit_arg0_t     arg0;
    uint32_t                            resv[3];
} cy_stc_wr_efuse_blow_bit_args_t;

/* Response Message */
typedef struct
{
    uint32_t ErrorCode  : 24;
    uint32_t            : 4;
    uint32_t StatusCode : 4;
} cy_stc_wr_efuse_blow_bit_resp0_t;

typedef struct
{
    cy_stc_wr_efuse_blow_bit_resp0_t    resp0;
    uint32_t                            resv[3];
} cy_stc_wr_efuse_blow_bit_resps_t;


/*********************************************************/
/*** Configure & Switch Over Regulator Data Structures ***/
/*********************************************************/
/** Mode selection */ 
typedef enum
{
    CY_SROM_LoadRegulatorTrim_LDO   = (0x00ul), /* Apply trim for internal regulator (LDO) */
    CY_SROM_LoadRegulatorTrim_REGHC = (0x01ul)  /* Apply trim for PMIC/REGHC */
}cy_en_srom_LoadRegulatorTrim_TrimType_t;

/** Use Case selection */
typedef enum
{
    CY_SROM_LoadRegulatorTrim_ForceTrim     = (0x00ul),
    CY_SROM_LoadRegulatorTrim_DPSLP_Entry   = (0x01ul),
    CY_SROM_LoadRegulatorTrim_DPSLP_Exit    = (0x02ul),
    CY_SROM_LoadRegulatorTrim_Reset_Recovery= (0x03ul),
}cy_en_srom_LoadRegulatorTrim_UseCase_t;


/** Operation mode of REGHC IP (ignored in case of PMIC IP) */
typedef enum
{
    CY_SROM_REGULATOR_MODE_TRANSISTOR = 0,
    CY_SROM_REGULATOR_MODE_PMIC       = 1,
} cy_en_srom_regulator_mode_t;

/** Target of switching regulators  */
typedef enum
{
    CY_SROM_REGULATOR_SWITCH_TARGET_EXT = 0,
    CY_SROM_REGULATOR_SWITCH_TARGET_INT = 1,
} cy_en_srom_regulator_switch_target_t;

/** Polarity of the PMICs "power good" / "abnormal" status output */
typedef enum
{
    CY_SROM_REGULATOR_STATUS_ABNORMAL_LOW  = 0,
    CY_SROM_REGULATOR_STATUS_ABNORMAL_HIGH = 1,
} cy_en_srom_regulator_status_polarity_t;

/** Polarity of the signal to enable the PMIC */
typedef enum
{
    CY_SROM_REGULATOR_ENABLE_LOW  = 0,
    CY_SROM_REGULATOR_ENABLE_HIGH = 1,
} cy_en_srom_regulator_enable_polarity_t;


/* Request Message */
typedef struct
{
    uint32_t              : 1;
    uint32_t Mode         : 1;
    uint32_t              : 6;
    uint32_t SwitchTarget : 8;
    uint32_t Blocking     : 8;
    uint32_t Opcode       : 8;
} cy_stc_switch_regulator_arg0_t;

typedef struct
{
    cy_stc_switch_regulator_arg0_t arg0;
    uint32_t                       resv[3];
} cy_stc_switch_regulator_args_t;

/* Request Message 0 for configure regulator */
typedef struct
{
    uint32_t                         : 1;
    uint32_t Mode                    : 1;
    uint32_t EnablePolarity          : 1;
    uint32_t StatusAbnormalPolarity  : 1;
    uint32_t DeepSleep               : 1;
    uint32_t KeepIntRegEnabled       : 1;
    uint32_t                         : 2;
    uint32_t VoltageAdjust           : 4;
    uint32_t                         : 12;
    uint32_t Opcode                  : 8;
} cy_stc_configure_regulator_arg0_t;

typedef struct
{
    cy_stc_configure_regulator_arg0_t arg0;
    uint32_t                          resv[3];
} cy_stc_configure_regulator_args_t;

typedef struct
{
    uint32_t                         : 1;
    uint32_t Mode                    : 1;
    uint32_t EnablePolarity          : 1;
    uint32_t StatusAbnormalPolarity  : 1;
    uint32_t DeepSleep               : 1;
    uint32_t UseLinReg               : 1;
    uint32_t UseRadj                 : 1;
    uint32_t VadjOption              : 1;
    uint32_t VoltageAdjust           : 5;
    uint32_t RAdjust                 : 3;
    uint32_t                         : 8;
    uint32_t Opcode                  : 8;
} cy_stc_configure_regulator2_arg0_t;

typedef struct
{
    cy_stc_configure_regulator2_arg0_t arg0;
    uint32_t                           resv[3];
} cy_stc_configure_regulator2_args_t;

typedef struct
{
    uint32_t                                        arg         : 1;
    cy_en_srom_LoadRegulatorTrim_TrimType_t         trim        : 1;
    cy_en_srom_LoadRegulatorTrim_UseCase_t          usecase     : 2;
    uint32_t                                        rev         : 20;
    uint32_t                                        opcode      : 8;
} cy_stc_LoadRegulatorTrim_arg0_t;

typedef struct 
{
    cy_stc_LoadRegulatorTrim_arg0_t arg0;
    uint32_t                        resv[3];
} cy_stc_LoadRegulatorTrim_args_t;

/* Request Message 1 for configure regulator 2 (new api) */
typedef struct
{
    uint32_t WaitCountIn1us : 10; // Wait count in 1us steps after PMIC status ok.
    uint32_t                : 22;
} cy_stc_configure_regulator2_arg0_2_t;

typedef struct
{
    cy_stc_configure_regulator2_arg0_2_t arg0;
    uint32_t                             resv[3];
} cy_stc_configure_regulator2_args_2_t;

/************************************/
/*** Flash Program Row Structures ***/
/************************************/

typedef enum
{
    CY_SROM_PROGRAMROW_DATA_SIZE_8BIT    = (0x00ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_16BIT   = (0x01ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_32BIT   = (0x02ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_64BIT   = (0x03ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_128BIT  = (0x04ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_256BIT  = (0x05ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_512BIT  = (0x06ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_1024BIT = (0x07ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_2048BIT = (0x08ul),
    CY_SROM_PROGRAMROW_DATA_SIZE_4096BIT = (0x09ul)
} cy_en_programrow_datasize_t;

typedef enum
{
    CY_SROM_PROGRAMROW_NON_BLOCKING = (0x00ul), // Non-blocking
    CY_SROM_PROGRAMROW_BLOCKING     = (0x01ul)  // API blocks CM0+
} cy_en_programrow_blocking_t;

typedef enum
{
    CY_SROM_PROGRAMROW_BLANK_CHECK      =  (0x00ul), // Perform blank check
    CY_SROM_PROGRAMROW_SKIP_BLANK_CHECK =  (0x01ul)  // Skips the blank check step
} cy_en_programrow_skipblankcheck_t;

typedef enum
{
    CY_SROM_PROGRAMROW_DATA_LOCATION_SRAM = (0x01ul) // SRAM
} cy_en_programrow_location_t;

typedef enum
{
    CY_SROM_PROGRAMROW_NOT_SET_INTR_MASK = (0x00ul), // Set FM interrupt mask
    CY_SROM_PROGRAMROW_SET_INTR_MASK     = (0x01ul)  // Don't set FM interrupt mask
} cy_en_programrow_intrmask_t;

/* Request Message */
typedef struct
{
    uint32_t rev0           : 8; // reserved
    uint32_t blocking       : 8; // Blocking. see \ref cy_en_programrow_blocking_t
    uint32_t skipBlankCheck : 8; // Skip blank check. see \ref cy_en_programrow_skipblankcheck_t
    uint32_t opcode         : 8; // Opecode = 0x06
} cy_stc_program_row_arg0_t;

typedef struct
{
    uint32_t dataSize      : 8; // Data size. see \ref cy_en_programrow_datasize_t
    uint32_t dataLoc       : 8; // Data location. see \ref cy_en_programrow_location_t
    uint32_t rev0          : 8; // reserved
    uint32_t interruptMask : 8; // Interrupt mask (Applicable only for eCT non-blocking). see \ref cy_en_programrow_intrmask_t
} cy_stc_program_row_arg1_t;

typedef struct
{
    uint32_t dstAddr : 32; // Flash address to be programmed
} cy_stc_program_row_arg2_t;

typedef struct
{
    uint32_t srcAddr : 32; // 32-bit aligned address of SRAM where data to be programmed will be stored
} cy_stc_program_row_arg3_t;

typedef struct
{
    cy_stc_program_row_arg0_t arg0;
    cy_stc_program_row_arg1_t arg1;
    cy_stc_program_row_arg2_t arg2;
    cy_stc_program_row_arg3_t arg3;
} cy_stc_program_row_args_t;

/* Response Message */
typedef struct
{
    uint32_t Indication : 28; // indication
    uint32_t statusCode : 4;  // status code
} cy_stc_program_row_resp0_t;

typedef struct
{
    cy_stc_program_row_resp0_t resp0;
    uint32_t                   resv[3];
} cy_stc_program_row_resps_t;

/*************************************/
/*** Flash Erase Sector Structures ***/
/*************************************/
typedef enum
{
    CY_SROM_ERASE_NON_BLOCKING      = (0x00ul), // Non-blocking
    CY_SROM_ERASE_BLOCKING          = (0x01ul)  // API blocks CM0+
} cy_en_erase_blocking_t;

typedef enum
{
    CY_SROM_ERASE_NOT_SET_INTR_MASK = (0x00ul), // Set FM interrupt mask
    CY_SROM_ERASE_SET_INTR_MASK     = (0x01ul)  // Don't set FM interrupt mask
} cy_en_erase_intrmask_t;

/* Request Message */
typedef struct
{
    uint32_t rev0          : 8;
    uint32_t blocking      : 8; // see \ref cy_en_erase_blocking_t
    uint32_t interruptMask : 8; // see \ref cy_en_erase_intrmask_t
    uint32_t opcode        : 8; // Opecode = 0x14
} cy_stc_erase_sector_arg0_t;

typedef struct
{
    uint32_t address : 32;
} cy_stc_erase_sector_arg1_t;

typedef struct
{
    cy_stc_erase_sector_arg0_t arg0;
    cy_stc_erase_sector_arg1_t arg1;
    uint32_t resv[2];
} cy_stc_erase_sector_args_t;

/* Response Message */
typedef struct
{
    uint32_t indication : 28; // indication
    uint32_t statusCode : 4;  // status code
} cy_stc_erase_sector_resp0_t;

typedef struct
{
    cy_stc_erase_sector_resp0_t resp0;
    uint32_t                    resv[3];
} cy_stc_erase_sector_resps_t;

/**********************************/
/*** Flash Erase All Structures ***/
/**********************************/
/* Request Message */
typedef struct
{
    uint32_t rev0   : 24;
    uint32_t opcode : 8;
} cy_stc_erase_all_arg0_t;

typedef struct
{
    cy_stc_erase_all_arg0_t arg0;
    uint32_t                resv[3];
} cy_stc_erase_all_args_t;

/* Response Message */
typedef struct
{
    uint32_t indication : 28; // indication
    uint32_t statusCode : 4;  // status code
} cy_stc_erase_all_resp0_t;

typedef struct
{
    cy_stc_erase_sector_resp0_t resp0;
    uint32_t                    resv[3];
} cy_stc_erase_all_resps_t;

/****************************************/
/*** Flash Check FM Status Structures ***/
/****************************************/
/* Request Message */
typedef struct
{
    uint32_t rev0   : 24;
    uint32_t opcode : 8; // Opecode = 0x07
} cy_stc_check_fm_status_arg0_t;

typedef struct
{
    cy_stc_check_fm_status_arg0_t arg0;
    uint32_t                      resv[3];
} cy_stc_check_fm_status_args_t;

/* Response Message */
typedef struct
{
    uint16_t pgm_code         : 1; // Indicates if active PGM operation to the Code flash is taking place
    uint16_t pgm_work         : 1; // Indicates if active PGM operation to the Work flash is taking place
    uint16_t erase_code       : 1; // Indicates if active Erase operation to the Code flash is taking place
    uint16_t erase_work       : 1; // Indicates if active Erase operation to the Work flash is taking place
    uint16_t ers_suspend      : 1; // Indicates if Erase operation (Code/Work) is currently being suspended
    uint16_t blank_check_work : 1; // Indicates if Blank Check mode is currently running on the work flash
    uint16_t blank_check_pass : 1; // Indicates  the Blank check command result is Blank
    uint16_t hang             : 1; // After embedded operation (pgm/erase) this flag will tell if it was successful or failed
    uint16_t busy             : 1; // Whenever the device is in embedded mode the RDY goes
    uint16_t                  : 7; // reserved
} cy_stc_fm_status_t;

typedef union 
{
    uint16_t           u16;
    cy_stc_fm_status_t bf; // see \n cy_stc_fm_status_t
} cy_un_fm_status_t ;

typedef struct
{
    uint32_t status     : 9; // FM status
    uint32_t            :19; // reserved
    uint32_t statusCode : 4;  // status code
} cy_stc_check_fm_status_resp0_t;

typedef struct
{
    cy_stc_check_fm_status_resp0_t resp0;
    uint32_t                       resv[3];
} cy_stc_check_fm_status_resps_t;


/***********************************************/
/*** Flash Configure FM Interrupt Structures ***/
/***********************************************/
typedef enum
{
    CY_SROM_FM_INTR_OPT_CLEAR_INTR_MASK = (0x00ul),
    CY_SROM_FM_INTR_OPT_SET_INTR_MASK   = (0x01ul),
    CY_SROM_FM_INTR_OPT_CLEAR_INTR      = (0x02ul)
} cy_en_conf_fm_intr_option_t;

/* Request Message */
typedef struct
{
    uint32_t        : 8;
    uint32_t option : 8; // options, see \ref cy_en_conf_fm_intr_option_t
    uint32_t        : 8;
    uint32_t opcode : 8; // Opecode = 0x08
} cy_stc_conf_fm_intr_arg0_t;

typedef struct
{
    cy_stc_conf_fm_intr_arg0_t arg0;
    uint32_t                   resv[3];
} cy_stc_conf_fm_intr_args_t;

/* Response Message */
typedef struct
{
    uint32_t status     : 28; // status
    uint32_t statusCode : 4;  // status code
} cy_stc_conf_fm_intr_resp0_t;

typedef struct
{
    cy_stc_conf_fm_intr_resp0_t resp0;
    uint32_t                    resv[3];
} cy_stc_conf_fm_intr_resps_t;

/*********************************/
/*** Flash Checksum Structures ***/
/*********************************/
typedef enum
{
    CY_SROM_CHECKSUM_BANK0     = (0x00ul),
    CY_SROM_CHECKSUM_BANK1     = (0x01ul)
} cy_en_checksum_bank_t;

typedef enum
{
    CY_SROM_CHECKSUM_PAGE     = (0x00ul),
    CY_SROM_CHECKSUM_WHOLE    = (0x01ul)
} cy_en_checksum_scope_t;

typedef enum
{
    CY_SROM_CHECKSUM_MAIN        = (0x00ul),
    CY_SROM_CHECKSUM_WOEK        = (0x01ul),
    CY_SROM_CHECKSUM_SUPERVISORY = (0x02ul)
} cy_en_checksum_region_t;

/* Request Message */
typedef struct
{
    uint32_t        : 7;
    uint32_t bank   : 1;  // see \ref cy_en_checksum_bank_t
    uint32_t row_id : 13; // Row id
    uint32_t whole  : 1;  // see \ref cy_en_checksum_scope_t
    uint32_t region : 2;  // see \ref cy_en_checksum_region_t
    uint32_t opcode : 8;  // Opecode = 0x0B
} stc_checksum_less_than_4M_arg0_t;

typedef struct
{
    uint32_t        : 1;
    uint32_t bank   : 1;  // see \ref cy_en_checksum_bank_t
    uint32_t        : 2;
    uint32_t row_id : 17; // Row id
    uint32_t whole  : 1;  // see \ref cy_en_checksum_scope_t
    uint32_t region : 2;  // see \ref cy_en_checksum_region_t
    uint32_t opcode : 8;  // Opecode = 0x0B
} stc_checksum_arg0_t;

typedef struct
{
    stc_checksum_less_than_4M_arg0_t arg0;
    uint32_t                         resv[3];
} stc_checksum_less_than_4M_args_t;

typedef struct
{
    stc_checksum_arg0_t arg0;
    uint32_t            resv[3];
} stc_checksum_args_t;

/* Response Message */
typedef struct
{
    uint32_t            : 28; // Nothing
    uint32_t statusCode : 4;  // status code
} stc_checksum_resp0_t;

typedef struct
{
    stc_checksum_resp0_t resp0;
    uint32_t             resv[3];
} cy_stc_checksum_resps_t;

/*******************************************/
/*** Flash Compute Basic Hash Structures ***/
/*******************************************/
typedef enum
{
    CY_SROM_COMPUTE_HASH_BASIC  = (0x00ul),
    CY_SROM_COMPUTE_HASH_CRC8   = (0x01ul)
} cy_en_computehash_type_t;

/* Request Message */
typedef struct
{
    uint32_t        : 8;
    uint32_t type   : 8; // see \ref cy_en_computehash_type_t
    uint32_t        : 8;
    uint32_t opcode : 8; // Opecde = 0x0D
} cy_stc_compute_hash_arg0_t;

typedef struct
{
    uint32_t start_addr : 32;
} cy_stc_compute_hash_arg1_t;

typedef struct
{
    uint32_t number_byte : 24; // Number of bytes (0: 1 byte,1: 2 bytes etc.)
    uint32_t             : 8;
} cy_stc_compute_hash_arg2_t;

typedef struct
{
    cy_stc_compute_hash_arg0_t arg0;
    cy_stc_compute_hash_arg1_t arg1;
    cy_stc_compute_hash_arg2_t arg2;
    uint32_t                   resv;
} cy_stc_compute_hash_args_t;

/* Response Message */
typedef struct
{
    uint32_t hash_crc   : 28; // Hash/CRC of the data
    uint32_t statusCode : 4;  // status code
} cy_stc_compute_hash_resp0_t;

typedef struct
{
    cy_stc_compute_hash_resp0_t resp0;
    uint32_t                    resv[3];
} cy_stc_compute_hash_resps_t;

/**************************************/
/*** Flash Erase Suspend Structures ***/
/**************************************/
/* Request Message */
typedef struct
{
    uint32_t rev0   : 24;
    uint32_t opcode : 8; // Opcode = 0x22
} cy_stc_erase_suspend_arg0_t;

typedef struct
{
    cy_stc_erase_suspend_arg0_t arg0;
    uint32_t                    resv[3];
} cy_stc_erase_suspend_args_t;

/* Response Message */
typedef struct
{
    uint32_t errorCode  : 28; // error code
    uint32_t statusCode : 4;  // status code
} cy_stc_erase_suspend_resp0_t;

typedef struct
{
    cy_stc_erase_suspend_resp0_t resp0;
    uint32_t                     resv[3];
} cy_stc_erase_suspend_resps_t;

/*************************************/
/*** Flash Erase Resume Structures ***/
/*************************************/
/* Request Message */
typedef struct
{
    uint32_t rev0     : 8;
    uint32_t blocking : 8; // see \ref cy_en_erase_blocking_t
    uint32_t intrmask : 1; // see \ref cy_en_erase_intrmask_t
    uint32_t          : 7;
    uint32_t opcode   : 8; // Opecode = 0x23
} cy_stc_erase_resume_arg0_t;

typedef struct
{
    cy_stc_erase_resume_arg0_t arg0;
    uint32_t                   resv[3];
} cy_stc_erase_resume_args_t;

/* Response Message */
typedef struct
{
    uint32_t errorCode  : 28; // error code
    uint32_t statusCode : 4;  // status code
} cy_stc_erase_resume_resp0_t;

typedef struct
{
    cy_stc_erase_resume_resp0_t resp0;
    uint32_t                    resv[3];
} cy_stc_erase_resume_resps_t;

/************************************/
/*** Flash Blank Check Structures ***/
/************************************/
/* Request Message */
typedef struct
{
    uint32_t        : 24;
    uint32_t opcode : 8; // Opecode = 0x2A
} cy_stc_blank_check_arg0_t;

typedef struct
{
    uint32_t addrToBeChecked : 32;
} cy_stc_blank_check_arg1_t;

typedef struct
{
    uint32_t numOfWordsToBeChecked : 16; // 0: 1 word, 1: 2 words etc.
    uint32_t                       : 16;
} cy_stc_blank_check_arg2_t;

typedef struct
{
    cy_stc_blank_check_arg0_t arg0;
    cy_stc_blank_check_arg1_t arg1;
    cy_stc_blank_check_arg2_t arg2;
    uint32_t                  resv;
} cy_stc_blank_check_args_t;

/* Response Message */
typedef struct
{
    uint32_t errorCode    : 8;  // error code
    uint32_t failedWordId : 16; // first failed word number
    uint32_t              : 4;
    uint32_t statusCode   : 4;  // status code
} cy_stc_blank_check_resp0_t;

typedef struct
{
    cy_stc_blank_check_resp0_t resp0;
    uint32_t                   resv[3];
} cy_stc_blank_check_resps_t;

/*************************/
/*** Generic Structure ***/
/*************************/
/* SROM API argument union 1 */
typedef union
{
    uint32_t                                arg[4];
    cy_stc_silicon_id_args_t                SiId;
    cy_stc_rd_unique_id_args_t              RdUnId;
    cy_stc_rd_efuse_byte_args_t             RdFuse;
    cy_stc_rd_efuse_byte_margin_args_t      RdFuseMargin;
    cy_stc_switch_regulator_args_t          SwitchRegulator;
    cy_stc_configure_regulator_args_t       ConfigureRegulator;
    cy_stc_configure_regulator2_args_t      ConfigureRegulator2;
    cy_stc_LoadRegulatorTrim_args_t         LoadRegulatorTrim;
    cy_stc_wr_efuse_blow_bit_args_t         BlowFuseBit;
    cy_stc_program_row_args_t               ProgramRow;
    cy_stc_erase_sector_args_t              EraseSector;
    cy_stc_erase_all_args_t                 EraseAll;
    cy_stc_check_fm_status_args_t           CheckFmStatus;
    cy_stc_conf_fm_intr_args_t              ConfigFmIntr;
    stc_checksum_less_than_4M_args_t        CheckSumLessThan4M;
    stc_checksum_args_t                     CheckSum;
    cy_stc_compute_hash_args_t              ComputeHash;
    cy_stc_erase_suspend_args_t             EraseSuspend;
    cy_stc_erase_resume_args_t              EraseResume;
    cy_stc_blank_check_args_t               BlankCheck;
} un_srom_api_args_t;

/* SROM API argument union 2 */
typedef union
{
    uint32_t                             arg[4];
    cy_stc_configure_regulator2_args_2_t ConfigureRegulator2;
} un_srom_api_args_2_t;


/* SROM API response union */
typedef union
{
    uint32_t                                resp[4];
    cy_stc_slicon_id_resps_t                SiId;
    cy_stc_rd_unique_id_resps_t             RdUnId;
    cy_stc_rd_efuse_byte_resps_t            RdFuse;
    cy_stc_rd_efuse_byte_margin_resps_t     RdFuseMargin;
    cy_stc_wr_efuse_blow_bit_resps_t        BlowFuseBit;
    cy_stc_program_row_resps_t              ProgramRow;
    cy_stc_erase_sector_resps_t             EraseSector;
    cy_stc_erase_all_resps_t                EraseAll;
    cy_stc_check_fm_status_resps_t          CheckFmStatus;
    cy_stc_conf_fm_intr_resps_t             ConfigFmIntr;
    cy_stc_checksum_resps_t                 CheckSum;
    cy_stc_compute_hash_resps_t             ComputeHash;
    cy_stc_erase_suspend_resps_t            EraseSuspend;
    cy_stc_erase_resume_resps_t             EraseResume;
    cy_stc_blank_check_resps_t              BlankCheck;
} un_srom_api_resps_t;

typedef union
{
    uint32_t            u32[4];
    un_srom_api_args_t  args;
    un_srom_api_resps_t resps;
} un_srom_api_scrach_sram_t;

#pragma pack()

/****************************/
/*** Function Declaration ***/
/****************************/
extern cy_en_srom_driver_status_t Cy_Srom_CallApi(const un_srom_api_args_t* params, un_srom_api_resps_t* resp);
extern cy_en_srom_driver_status_t Cy_Srom_CallApi_NonBlock(const un_srom_api_args_t* params);
extern cy_en_srom_driver_status_t Cy_Srom_CallApi_2(const un_srom_api_args_t* params, const un_srom_api_args_2_t* params2, un_srom_api_resps_t* resp);
extern cy_en_srom_driver_status_t Cy_Srom_CallApi_NonBlock_2(const un_srom_api_args_t* params, const un_srom_api_args_2_t* params2);
extern cy_en_srom_response_type_t Cy_Srom_GetResponseType(void);
extern cy_en_srom_api_status_t Cy_Srom_GetApiResponse(un_srom_api_resps_t* resp);
extern cy_en_srom_api_status_t Cy_Srom_ConvertRespToStatus(uint32_t resp0);
extern void Cy_Srom_SetResponseHandler(cy_srom_handler handler, IRQn_Type intIdx);

#endif

