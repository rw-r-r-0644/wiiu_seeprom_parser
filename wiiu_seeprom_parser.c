#include <stdio.h>
#include "seeprom.h"

/*
	Kinda hacky Wii U seeprom parser
	Written by rw-r-r_0644
*/


seeprom_t *seeprom;

//#define DONT_PRINT_0X
//#define REMOVE_0

void print_hex(const char *desc, u8 *s, int size) { //Used for printing keys/other values
	printf(desc);
#ifndef DONT_PRINT_0X
	printf("0x");
#endif
#ifdef REMOVE_0
	int startp = 0;
	for (int i = 0; i < size; i++) {
		if (startp == 0 && s[i] == 0) //If the value is 0 and we aren't in the middle of the number don't print it
			continue;
		if (startp == 0 && s[i] != 0) { //The value is not 0: print the next value when 0 too
			startp = 1;
			printf("%X", s[i]);
			continue;
		}
		printf("%02X", s[i]);
	}
	if (startp == 0) //It was all 0 so nothing got printed, print 0
		printf("0");
#else
	for (int i = 0; i < size; i++)
		printf("%02X", s[i]);
#endif
	printf("\n");
}

void print_bc_struct(bc_struct_t bc_struct) {
	printf   ("\n-----------------Board Config----------------\n");
	print_hex("CRC32:...........................", bc_struct.crc32, 0x4);
	print_hex("size:............................", bc_struct.size, 0x2);
	print_hex("library version:.................", bc_struct.library_version, 0x2);
	print_hex("author:..........................", bc_struct.author, 0x2);
	print_hex("boardType:.......................", bc_struct.boardtype, 0x2);
	print_hex("boardRevision:...................", bc_struct.boardrevision, 0x2);
	print_hex("bootSource:......................", bc_struct.bootsource, 0x2);
	print_hex("ddr3Size:........................", bc_struct.ddr3size, 0x2);
	print_hex("ddr3Speed:.......................", bc_struct.ddr3speed, 0x2);
	print_hex("ppcClockMultiplier:..............", bc_struct.ppcclockmultiplier, 0x2);
	print_hex("iopClockMultiplier:..............", bc_struct.iopclockmultiplier, 0x2);
	print_hex("video1080p:......................", bc_struct.video1080p, 0x2);
	print_hex("ddr3Vendor:......................", bc_struct.ddr3vendor, 0x2);
	print_hex("movPassiveReset:.................", bc_struct.movpassivereset, 0x2);
	print_hex("sysPllSpeed:.....................", bc_struct.syspllspeed, 0x2);
	print_hex("sataDevice:......................", bc_struct.satadevice, 0x2);
	print_hex("consoleType:.....................", bc_struct.consoletype, 0x2);
	print_hex("devicePresence:..................", bc_struct.devicepresence, 0x4);
	
	printf   ("---------------------------------------------\n\n");
}

void print_sys_prod_struct(sys_prod_t sys_prod) {
	printf   ("\n-----------------SLC:sys_prod----------------\n");
	print_hex("version:.........................", sys_prod.version, 0x4);
	print_hex("eeprom_version:..................", sys_prod.eeprom_version, 0x4);
	print_hex("product_area:....................", sys_prod.product_area, 0x4);
	print_hex("game_region:.....................", sys_prod.game_region, 0x4);
	print_hex("ntsc_pal:........................", sys_prod.ntsc_pal, 0x4);
	print_hex("country_code_5ghz:...............", sys_prod.country_code_5ghz, 0x2);
	print_hex("country_code_revision_5ghz:......", sys_prod.country_code_revision_5ghz, 0x2);
	print_hex("code_id:.........................", sys_prod.code_id, 0x8);
	print_hex("serial_id:.......................", sys_prod.serial_id, 0x10);
	print_hex("model_number:....................", sys_prod.model_number, 0x10);
	printf   ("---------------------------------------------\n\n");
}

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
	if (argc < 3) {
		//printf("WARNING: no otp.bin specified, some values won't be decrypted\n\n");
	}
	
    printf("Parsing seeprom...\n");
	
	/* LOAD SEEPROM FILE */
	FILE * seeprom_file;
	seeprom_file = fopen(argv[1], "r");
    if (seeprom_file == NULL) {
        printf("ERROR: failed to open %s!\n", argv[1]);
		return -1;
    }
		
	fseek(seeprom_file, 0, SEEK_END);
	long seeprom_size = ftell(seeprom_file);
	fseek(seeprom_file, 0, SEEK_SET);
	
	if (seeprom_size != sizeof(seeprom_t)) {
		printf("ERROR: seeprom size is incorrect\n");
		return -1;
	}
	
	seeprom = (seeprom_t *) malloc( sizeof(seeprom_t) );
	if (fread (seeprom, 1, sizeof(seeprom_t), seeprom_file) != 512) {
		printf("ERROR: Failed to read %s!\n", argv[1]);
		return -1;
	}
	printf("Successfully parsed seeprom.\n\n\n\n\n\n\n");
	printf("================EEPROM VALUES================\n\n\n");
	/* PRINT SEEPROM VALUES */
	print_hex("SEEPROM PRNG seed:...............", seeprom->seeprom_prng_seed, 0x08);
	print_hex("PPC PVR (should be 0x70010201):..", seeprom->ppc_pvr, 0x04);
	printf   ("ASCII Tag 1:.....................\"%.*s\"\n", 0x6, seeprom->ascii_tag1);
	printf   ("OTP tag:.........................\"%.*s\"\n", 0x8, seeprom->otp_tag);
	print_bc_struct(seeprom->bc_struct);
	print_hex("Wii U drive key:.................", seeprom->wii_u_drive_key, 0x10);
	print_hex("Wii U factory key:...............", seeprom->wii_u_factory_key, 0x10);
	print_hex("Wii U devkit key (?):............", seeprom->wii_u_devkit_key, 0x10);
	print_hex("Wii U USB key seed:..............", seeprom->wii_u_usb_key_seed, 0x10);
	print_hex("Drive key's status flag:.........", seeprom->drive_key_status_flag, 0x02);
	print_hex("USB key seed's status flag:......", seeprom->usb_key_seed_status_flag, 0x02);
	print_hex("Devkit key's status flag:........", seeprom->devkit_key_status_flag, 0x02);
	print_sys_prod_struct(seeprom->sys_prod);
	printf   ("ASCII Tag 2:.....................\"%.*s\"\n", 0x8, seeprom->ascii_tag2);
	
	//decrypt_boot_param
	//print_boot_param1
	//decrypt_boot_param
	//print_boot_param2
	//decrypt_boot_param
	//print_boot_param3
	
	printf("\n\n=============================================\n\n");
	fclose(seeprom_file);
    return 0;
}