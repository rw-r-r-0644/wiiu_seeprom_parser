#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_hex.h"
#include "seeprom.h"
#include "aes.h"

/*
	Kinda hacky Wii U seeprom parser
	Written by rw-r-r_0644
*/


//! Globals
seeprom_t     seeprom;
boot_param1_t boot_param1;
boot_param2_t boot_param2;
boot_param3_t boot_param3;
u8 otp[0x400];

int use_otp = 0;

int main (int argc, char *argv[])
{
	printf("\n=======================================\n");
	printf("   Wii U SEEPROM parser v1   \n");
	printf("=======================================\n\n");

	printf("Based on http://wiiubrew.org/wiki/Hardware/OTP\n\n");
	if (argc < 2) {
		printf("USAGE: seeprom_parse seeprom.bin [otp.bin]\n\n");
		return -1;
	}
	
	//! Run program
	//Initialize variables
	memset((void *)&seeprom,      0, sizeof(seeprom_t));
	memset((void *)&boot_param1,  0, sizeof(boot_param1_t));
	memset((void *)&boot_param2,  0, sizeof(boot_param2_t));
	memset((void *)&boot_param3,  0, sizeof(boot_param3_t));
		
	if (argc < 3)
		printf("WARNING: no otp.bin specified, some values won't be decrypted\n\n");
	else {
		//Load otp file
		printf("Reading otp...\n");
		FILE * otp_file;
		otp_file = fopen(argv[2], "rb");
		if (otp_file == NULL) {
			printf("ERROR: failed to open %s!\n", argv[2]);
			return -1;
		}
		if (fread(otp, 1, 0x400, otp_file) != 0x400) {
			printf("ERROR: Failed to read %s!\n", argv[2]);
			return -1;
		}
		fclose(otp_file);
		printf("Successfully read otp.\n");
		use_otp = 1;
	}
	
	//Load seeprom file
    printf("Reading seeprom...\n");
	FILE * seeprom_file;
	seeprom_file = fopen(argv[1], "rb");
    if (seeprom_file == NULL) {
        printf("ERROR: failed to open %s!\n", argv[1]);
		return -1;
    }
	if (fread (&seeprom, 1, sizeof(seeprom_t), seeprom_file) != 512) {
		printf("ERROR: Failed to read %s!\n", argv[1]);
		return -1;
	}
	fclose(seeprom_file);
	printf("Successfully read seeprom.\n\n\n\n\n\n\n");
	
	// Print SEEPROM values
	printf("================SEEPROM VALUES================\n\n\n");
	
	print_hex("SEEPROM PRNG seed:...............", seeprom.seeprom_prng_seed, 0x08);
	print_hex("PPC PVR (should be 0x70010201):..", seeprom.ppc_pvr, 0x04);
	printf   ("ASCII Tag 1:.....................\"%.*s\"\n", 0x6, seeprom.ascii_tag1);
	printf   ("OTP tag:.........................\"%.*s\"\n", 0x8, seeprom.otp_tag);
	
	//Print BC struct
	printf   ("\n-----------------Board Config----------------\n");
	print_hex("CRC32:...........................", seeprom.bc_struct.crc32, 0x4);
	print_hex("size:............................", seeprom.bc_struct.size, 0x2);
	print_hex("library version:.................", seeprom.bc_struct.library_version, 0x2);
	print_hex("author:..........................", seeprom.bc_struct.author, 0x2);
	printf   ("boardType:.......................\"%.*s\"\n", 0x2, seeprom.bc_struct.boardtype);
	print_hex("boardRevision:...................", seeprom.bc_struct.boardrevision, 0x2);
	printf   ("bootSource:......................\"%.*s\"\n", 0x2, seeprom.bc_struct.bootsource);
	print_hex("ddr3Size:........................", seeprom.bc_struct.ddr3size, 0x2);
	print_hex("ddr3Speed:.......................", seeprom.bc_struct.ddr3speed, 0x2);
	print_hex("ppcClockMultiplier:..............", seeprom.bc_struct.ppcclockmultiplier, 0x2);
	print_hex("iopClockMultiplier:..............", seeprom.bc_struct.iopclockmultiplier, 0x2);
	print_hex("video1080p:......................", seeprom.bc_struct.video1080p, 0x2);
	print_hex("ddr3Vendor:......................", seeprom.bc_struct.ddr3vendor, 0x2);
	print_hex("movPassiveReset:.................", seeprom.bc_struct.movpassivereset, 0x2);
	print_hex("sysPllSpeed:.....................", seeprom.bc_struct.syspllspeed, 0x2);
	print_hex("sataDevice:......................", seeprom.bc_struct.satadevice, 0x2);
	print_hex("consoleType:.....................", seeprom.bc_struct.consoletype, 0x2);
	print_hex("devicePresence:..................", seeprom.bc_struct.devicepresence, 0x4);
	printf   ("---------------------------------------------\n\n");
	
	//Keys
	print_hex("Wii U drive key:.................", seeprom.wii_u_drive_key, 0x10);
	print_hex("Wii U factory key:...............", seeprom.wii_u_factory_key, 0x10);
	print_hex("Wii U devkit key (?):............", seeprom.wii_u_devkit_key, 0x10);
	print_hex("Wii U USB key seed:..............", seeprom.wii_u_usb_key_seed, 0x10);
	
	//Keys states
	printf   ("Drive key's status flag:.........");
	u16 dksf = seeprom.drive_key_status_flag[0] << 8 | seeprom.drive_key_status_flag[1]; //drive key status flag
	if (dksf == 0xFFFF)       printf("SET, encrypted with Wii U SEEPROM key (0xFFFF)\n");
	else if (dksf == 0x0000)  printf("SET, not encrypted (0x0000)\n");
	
	printf   ("USB key seed's status flag:......");
	u16 ukssf = seeprom.usb_key_seed_status_flag[0] << 8 | seeprom.usb_key_seed_status_flag[1]; //drive key status flag
	if (ukssf == 0xFFFF)      printf("SET, encrypted with Wii U SEEPROM key (0xFFFF)\n");
	else if (ukssf == 0x0000) printf("SET, not encrypted (0x0000)\n");
	
	printf   ("Devkit key's status flag:........");
	u16 dkksf = seeprom.devkit_key_status_flag[0] << 8 | seeprom.devkit_key_status_flag[1]; //drive key status flag
	if (dkksf == 0xFFFF)      printf("SET, encrypted with a key from OTP (0xFFFF) [whoa, you're using a devkit??]\n");
	else if (dkksf == 0x0000) printf("NOT SET (0x0000)\n");
	
	
	// SLC: sys_prod
	printf   ("\n-----------------SLC:sys_prod----------------\n");
	print_hex("version:.........................", seeprom.sys_prod.version, 0x4);
	print_hex("eeprom_version:..................", seeprom.sys_prod.eeprom_version, 0x4);
	print_hex("product_area:....................", seeprom.sys_prod.product_area, 0x4);
	print_hex("game_region:.....................", seeprom.sys_prod.game_region, 0x4);
	printf   ("ntsc_pal:........................\"%.*s\"\n", 0x4, seeprom.sys_prod.ntsc_pal);
	printf   ("country_code_5ghz:...............\"%.*s\"\n", 0x2, seeprom.sys_prod.country_code_5ghz);
	print_hex("country_code_revision_5ghz:......", seeprom.sys_prod.country_code_revision_5ghz, 0x2);
	printf   ("code_id:.........................\"%.*s\"\n", 0x8, seeprom.sys_prod.code_id);
	printf   ("serial_id:.......................\"%.*s\"\n", 0x10, seeprom.sys_prod.serial_id);
	printf   ("model_number:....................\"%.*s\"\n", 0x10, seeprom.sys_prod.model_number);
	printf   ("---------------------------------------------\n\n");
	
	// Other values
	printf   ("ASCII Tag 2:.....................\"%.*s\"\n", 0x8, seeprom.ascii_tag2);
	
	
	// Decrypt boot parameters
	if (use_otp == 1) {
		//Take Wii U seeprom key from OTP
		u8 wiiu_seeprom_key[0x10];
		memcpy(wiiu_seeprom_key, &otp[0x0A0], 0x10);
		
		print_hex("\n\nDecrypting boot prameters with key ", wiiu_seeprom_key, 0x10);
		
		//IV for all the parameters
		u8 boot_param_IV[0x10];
		memset(boot_param_IV, 0x0, 0x10); // IV = 0 (?)
		
		//Decrypt boot flags
		AES128_CBC_decrypt_buffer((u8*)&boot_param1, seeprom.boot_param1_enc, 0x10, wiiu_seeprom_key, boot_param_IV);
		AES128_CBC_decrypt_buffer((u8*)&boot_param2, seeprom.boot_param2_enc, 0x10, wiiu_seeprom_key, boot_param_IV);
		AES128_CBC_decrypt_buffer((u8*)&boot_param3, seeprom.boot_param3_enc, 0x10, wiiu_seeprom_key, boot_param_IV);
		
		printf   ("Boot parameres:\n");		
		
		u16 control_flag = boot_param1.control_flags[0] << 8 | boot_param1.control_flags[1];
		// TODO: Bits counted from more important bits or less important bit??
		u16 cpu_speed =					(control_flag & 0xFFC0) >> 6; // 1111111111000000
		u16 io_delay_before_sd_check =	(control_flag & 0x003E) >> 1; // 0000000000111110
		u16 clock_increased =			(control_flag & 0x0001); // 0000000000000001
		
		u16 nand_cotrol_flags = boot_param1.nand_cotrol_flags[0] << 8 | boot_param1.nand_cotrol_flags[1];
		// TODO: Bits counted from more important bits or less important bit??
		u16 overwrite_NAND_BANK   =	(nand_cotrol_flags & 0x0004) >> 2; // 0000000000000100
		u16 overwrite_NAND_CONFIG =	(nand_cotrol_flags & 0x0002) >> 1; // 0000000000000010
		
		printf   ("    Control flags (clock config + SMC delay):\n");
		printf   ("        CPU speed in MHz:......................0x%04X\n", cpu_speed);
		printf   ("        IO delay before checking SD boot:......0x%04X\n", io_delay_before_sd_check);
		printf   ("        Clock increased:.......................0x%04X\n", clock_increased);
		printf   ("    Control flags (NAND config):\n");
		printf   ("        Overwrite NAND_BANK?:..................0x%04X\n", overwrite_NAND_BANK);
		printf   ("        Overwrite NAND_CONFIG?:................0x%04X\n", overwrite_NAND_CONFIG);
		print_hex("    Overwrite NAND_CONFIG with value:..........", boot_param1.nand_config_overwrite, 0x4);
		print_hex("    Overwrite NAND_BANK with value:............", boot_param1.nand_bank_overwrite, 0x4);
		print_hex("    CRC32:.....................................", boot_param1.crc32, 0x4);

		printf   ("Boot parameres for boot0 about boot1:\n");
		print_hex("    boot1 version:.............................", boot_param2.boot1_version, 0x2);
		print_hex("    boot1 NAND sector:.........................", boot_param2.boot1_nand_sector, 0x2);
		print_hex("    CRC32:.....................................", boot_param2.crc32, 0x4);
		
		printf   ("Boot parameres for boot0 about boot1 copy:\n");
		print_hex("    boot1 copy version:........................", boot_param3.boot1_copy_version, 0x2);
		print_hex("    boot1 copy NAND sector:....................", boot_param3.boot1_copy_nand_sector, 0x2);
		print_hex("    CRC32:.....................................", boot_param3.crc32, 0x4);
	}
	
	printf("\n\n=============================================\n\n");
	
    return 0;
}
