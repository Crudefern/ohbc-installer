#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "video.h"
#include "libpatcher/libpatcher.h"
#include "haxx.h"
#include "wad_strerror.h" // split into a seperate file because it takes up a lot of space

#include "certs_bin.h"
#include "ohbc_tik_bin.h"
#include "ohbc_tmd_bin.h"
#include "ohbc_00000000_bin.h"
#include "ohbc_00000001_bin.h"

#define AHBPROT_DISABLED (*(vu32*)0xcd800064 == 0xFFFFFFFF)

int main()
{

	printf("certs: %#p %u\n", certs_bin, certs_bin_size);
	printf("ohbc_tik: %#p %u\n", ohbc_tik_bin, ohbc_tik_bin_size);
	printf("ohbc_tmd: %#p %u\n", ohbc_tmd_bin, ohbc_tmd_bin_size);
	printf("ohbc_00000000: %#p %u\n", ohbc_00000000_bin, ohbc_00000000_bin_size);
	printf("ohbc_00000001: %#p %u\n", ohbc_00000001_bin, ohbc_00000001_bin_size);

	/* code possibly */
	int cfd;
	int ohno = 0;

	Haxx_GetBusAccess();

	if (!AHBPROT_DISABLED)
	{
		printf("AHBPROT not disabled? what");
		while (1)
		{
			sleep(1);
		}
	}

	if (!(patch_ahbprot_reset() && patch_isfs_permissions() && patch_ios_verify()))
	{
		puts("failed to apply IOS patches!");
		while (1)
		{
			sleep(1);
		}
	}

	int ret = ES_AddTicket(ohbc_tik_bin, ohbc_tik_bin_size, certs_bin, certs_bin_size, NULL, 0);
	// check ret here
	if (ret < 0)
	{
		printf("ES_AddTicket failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	ret = ES_AddTitleStart(ohbc_tmd_bin, ohbc_tmd_bin_size, certs_bin, certs_bin_size, NULL, 0);
	// check ret here as well
	if (ret < 0)
	{
		printf("ES_AddTitleStart failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	cfd = ret = ES_AddContentStart(0x000100014C554C5ALL, 0);
	// check ret here
	if (ret < 0)
	{
		printf("ES_AddContentStart failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	ret = ES_AddContentData(cfd, ohbc_00000000_bin, ohbc_00000000_bin_size);
	if (ret < 0)
	{
		printf("ES_AddContentData failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	ret = ES_AddContentFinish(cfd);
	if (ret < 0)
	{
		printf("ES_AddContentFinish failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	cfd = ret = ES_AddContentStart(0x000100014C554C5ALL, 1);
	if (ret < 0)
	{
		printf("ES_AddContentStart 2 failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	ret = ES_AddContentData(cfd, ohbc_00000001_bin, ohbc_00000001_bin_size);
	if (ret < 0)
	{
		printf("ES_AddContentData 2 failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	ret = ES_AddContentFinish(cfd);
	if (ret < 0)
	{
		printf("ES_AddContentFinish 2 failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}
	ret = ES_AddTitleFinish();
	if (ret < 0)
	{
		printf("ES_AddTitleFinish failed (%i)\n%s\n", ret, wad_strerror(ret));
		ohno = 1;
	}

	if (ohno != 0)
	{
		printf("\nFailure!\n(Do you already have the hbc installed?)");
		while (1)
		{
			sleep(1);
		}
	}
	printf("Success!");
	sleep(2);

	// yippie
	return 0;
}