#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "video.h"
#include "libpatcher.h"
#include "haxx.h"
#include "wad_strerror.h" // split into a separate file because it takes up a lot of space
#include "crc.h"

#include "certs_bin.h"
#include "ohbc_tik_bin.h"
#include "ohbc_tmd_bin.h"
#include "ohbc_00000000_bin.h"
#include "ohbc_00000001_bin.h"

#define AHBPROT_DISABLED (*(vu32 *)0xcd800064 == 0xFFFFFFFF)
#define TITLE_ID 0x000100014C554C5ALL

void hang() {
	while(1) {
		printf(".");
		sleep(1);
	}
}

int main() {
	uint32_t certs_crc = CRC32(&certs_bin, certs_bin_size);
	uint32_t ohbc_tik_crc = CRC32(&ohbc_tik_bin, ohbc_tik_bin_size);
	uint32_t ohbc_tmd_crc = CRC32(&ohbc_tmd_bin, ohbc_tmd_bin_size);
	uint32_t ohbc_00000000_crc = CRC32(&ohbc_00000000_bin, ohbc_00000000_bin_size);
	uint32_t ohbc_00000001_crc = CRC32(&ohbc_00000001_bin, ohbc_00000001_bin_size);

	printf("certs: 0x%08X, expected 0xBF34F23D\n", certs_crc);
	printf("ohbc_tik: 0x%08X, expected 0x6F97D819\n", ohbc_tik_crc);
	printf("ohbc_tmd: 0x%08X, expected 0x8FE6DFF8\n", ohbc_tmd_crc);
	printf("ohbc_00000000: 0x%08X, expected 0x0A77DDF9\n", ohbc_00000000_crc);
	printf("ohbc_00000001: 0x%08X, expected 0xC9C87DB1\n", ohbc_00000001_crc);

	if (certs_crc != 3207918141 || ohbc_tik_crc != 1872222233 || ohbc_tmd_crc != 2414272504 ||
		ohbc_00000000_crc != 175627769 || ohbc_00000001_crc != 3385359793) {
		printf("uh oh\nHanging...");
		hang();
	}

	/* code possibly */
	int16_t cfd;
	int16_t ret;
	bool ohno = false;

	Haxx_GetBusAccess();

	if (!AHBPROT_DISABLED) {
		printf("AHBPROT not disabled? what\nHanging...");
		hang();
	}

	if (!(patch_ahbprot_reset() && patch_isfs_permissions() && patch_ios_verify())) {
		printf("failed to apply IOS patches!\nHanging...");
		hang();
	}

	ret = ES_AddTicket(&ohbc_tik_bin, ohbc_tik_bin_size, &certs_bin, certs_bin_size, NULL, 0);
	if (ret < 0) {
		printf("ES_AddTicket failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}
	
	ret = ES_AddTitleStart(&ohbc_tmd_bin, ohbc_tmd_bin_size, &certs_bin, certs_bin_size, NULL, 0);
	if (ret < 0) {
		printf("ES_AddTitleStart failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	cfd = ret = ES_AddContentStart(TITLE_ID, 0);
	if (ret < 0) {
		printf("ES_AddContentStart failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	ret = ES_AddContentData(cfd, &ohbc_00000000_bin, ohbc_00000000_bin_size);
	if (ret < 0) {
		printf("ES_AddContentData failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	ret = ES_AddContentFinish(cfd);
	if (ret < 0) {
		printf("ES_AddContentFinish failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	cfd = ret = ES_AddContentStart(TITLE_ID, 1);
	if (ret < 0) {
		printf("ES_AddContentStart 2 failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	ret = ES_AddContentData(cfd, &ohbc_00000001_bin, ohbc_00000001_bin_size);
	if (ret < 0) {
		printf("ES_AddContentData 2 failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	ret = ES_AddContentFinish(cfd);
	if (ret < 0) {
		printf("ES_AddContentFinish 2 failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	ret = ES_AddTitleFinish();
	if (ret < 0) {
		printf("ES_AddTitleFinish failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = true;
	}

	if (ohno) {
		printf("\nFailure! (Do you already have the hbc installed?)\nHanging...");
		hang();
	}
	printf("Success!\n");
	sleep(2);
	printf("Launching hbc...");
	WII_LaunchTitle(TITLE_ID);
	// yippie
	return 0;
}