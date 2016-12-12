#ifndef _SEEPROM_H
#define _SEEPROM_H

#include "short_types.h"

/*
	seeprom.h
	Written by rw-r-r_0644
	Based on http://wiiubrew.org/wiki/Hardware/OTP
	
	Seeprom Structure files
*/


typedef struct _bc_struct_t {
	u8 crc32				[0x04];
	u8 size					[0x02];
	u8 library_version		[0x02];
	u8 author				[0x02];
	u8 boardtype			[0x02];
	u8 boardrevision		[0x02];
	u8 bootsource			[0x02];
	u8 ddr3size				[0x02];
	u8 ddr3speed			[0x02];
	u8 ppcclockmultiplier	[0x02];
	u8 iopclockmultiplier	[0x02];
	u8 video1080p			[0x02];
	u8 ddr3vendor			[0x02];
	u8 movpassivereset		[0x02];
	u8 syspllspeed			[0x02];
	u8 satadevice			[0x02];
	u8 consoletype			[0x02];
	u8 devicepresence		[0x04];
} bc_struct_t;

typedef struct _sys_prod_t {
	u8 version				[0x04];
	u8 eeprom_version		[0x04];
	u8 product_area			[0x04];
	u8 game_region			[0x04];
	u8 ntsc_pal				[0x04];
	u8 country_code_5ghz	[0x02];
	u8 country_code_revision_5ghz[0x02];
	u8 code_id				[0x08];
	u8 serial_id			[0x10];
	u8 model_number			[0x10];
} sys_prod_t;

typedef struct _boot_param1_t {
	u8 control_flags		[0x02];
	u8 nand_cotrol_flags	[0x02];
	u8 nand_config_overwrite[0x04];
	u8 nand_bank_overwrite	[0x04];
	u8 crc32				[0x04];
} boot_param1_t;

typedef struct _boot_param2_t {
	u8 boot1_version		[0x02];
	u8 boot1_nand_sector	[0x02];
	u8 empty				[0x08];
	u8 crc32				[0x04];
} boot_param2_t;

typedef struct _boot_param3_t {
	u8 boot1_copy_version	[0x02];
	u8 boot1_copy_nand_sector[0x02];
	u8 empty				[0x08];
	u8 crc32				[0x04];
} boot_param3_t;

typedef struct _seeprom_struct_t {
//	Empty.
	u8 empty1				[0x12];
/* 
	SEEPROM PRNG seed.
	Incremented every time IOS-CRYPTO starts. 
	Used with the OTP's RNG key and the OTP's RNG seed to setup IOSU's PRNG.
*/
	u8 seeprom_prng_seed	[0x08];
//	Empty.
	u8 empty2				[0x06];
//	PPC PVR (0x70010201).
	u8 ppc_pvr				[0x04];
//	ASCII tag.
	char ascii_tag1			[0x06];
//	Unknown.
	u8 unknown1				[0x06];
//	OTP tag.
	char otp_tag			[0x08];
//	BC ("BoardConfig") structure
	bc_struct_t bc_struct;
//	Reserved for BC ("BoardConfig") library.
	u8 reserved_for_bc_library[0x20];
//	Wii U drive key.
	u8 wii_u_drive_key		[0x10];
//	Wii U factory key (cleared by IOS-MCP).
	u8 wii_u_factory_key	[0x10];
//	Wii U devkit key (?).
	u8 wii_u_devkit_key		[0x10];
/*
	Wii U USB key seed.
	This seed is encrypted with a key from OTP then used to set the /dev/crypto USB key.
	The first 4 bytes of this key must match the last 4 bytes of a 0x10 seed stored in the OTP.
*/
	u8 wii_u_usb_key_seed	[0x10];
/*
	Drive key's status flag.
	If the flag is 0xFFFF, the drive key is set and encrypted with the Wii U SEEPROM key.
	If the flag is 0x0000, the drive key is set and in plain form.
*/
	u8 drive_key_status_flag[0x02];
//	USB key seed's status flag.
	u8 usb_key_seed_status_flag[0x02];
/*
	Devkit key's status flag.
	If the flag is 0xFFFF, the devkit key is set and encrypted with a key from OTP.
	If the flag is 0x0000, the devkit key is not set.
*/
	u8 devkit_key_status_flag[0x02];
//	Unknown.
	u8 unknown2[0x6a + 0x04 + 0x04 + 0x08];
//	slc: sys_prod structure
	sys_prod_t sys_prod;
//	Unknown.
	u8 unknown3[0x04 + 0x0c + 0x10 + 0x08];
//	ASCII tag.
	char ascii_tag2[0x08];
	//Unknown
	u8 unknown[0x10]; 
	//boot parameters (encrypted with seeprom key)
	u8 boot_param1_enc[0x10]; //Control flags
	u8 boot_param2_enc[0x10]; //Boot parameters about boot1
	u8 boot_param3_enc[0x10]; //Boot parameters about boot1 copy
	u8 empty3[0x10];
} seeprom_t;


#endif