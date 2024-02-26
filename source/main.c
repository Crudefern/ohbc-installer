#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "video.h"

#include "certs_bin.h"
#include "ohbc_tik_bin.h"
#include "ohbc_tmd_bin.h"
#include "ohbc_00000000_bin.h"
#include "ohbc_00000001_bin.h"

const char *wad_strerror(int ret)
{
	switch (ret)
	{
	case -EPERM:
		return "No perms lol";
	case -EIO:
		return "Short read/write (probably read.)";
	case -ENOMEM:
		return "Out of memory (!?)";

	case -106:
		return "(FS) No such file or directory.";
	case -1005:
		return "Invalid public key type in certificate.";
	case -1009:
		return "(ES) Short read.";
	case -1010:
		return "(ES) Short write. Consider freeing up space in Data Management.";
	case -1012:
		return "Invalid signature type.";
	case -1016:
		return "Maximum amount of handles exceeded (3). Who is reopening /dev/es anyways?";
	case -1017:
		return "(ES) Invalid arguments. ES might not like what you're trying to do.";
	case -1020:
		return "This ticket is for a specific Wii, and it's not this one.";
	case -1022:
		return "Hash mismatch.\nIf uninstalling first does not fix this, this WAD is corrupt.";
	case -1024:
		return "ES is out of memory (!?)";
	case -1026:
		return "Incorrect access rights (according to the TMD.)";
	case -1027:
		return "Issuer not found in the certificate chain.";
	case -1028:
		return "Ticket not installed.";
	case -1029:
		return "Invalid ticket. This ticket probably uses the vWii common key.\nUninstalling the title first might fix this.";
	case -1031:
		return "Installed boot2 version is too old (or you are trying to downgrade it.)";
	case -1032:
		return "Fatal error in early ES initialization (!?)";
	case -1033:
		return "A ticket limit was exceeded. Play time is over, sorry.";
	case -1035:
		return "A newer version of this title is already present.\nConsider uninstalling it first.";
	case -1036:
		return "Required IOS for this title is not present.";
	case -1037:
		return "Installed number of contents doesn't match TMD.";
		//	case -1039: return "(DI) \"TMD not supplied for disc/nand game\"";

	case -2011:
		return "(IOSC) Signature check failed. Lol!! No trucha bug!!";
	case -2016:
		return "(IOSC) Unaligned data.";

	default:
		return "Unknown error, maybe this is an errno ?";
	}
}

int main()
{

	printf("certs: %#p %u\n", certs_bin, certs_bin_size);
	printf("ohbc_tik: %#p %u\n", ohbc_tik_bin, ohbc_tik_bin_size);
	printf("ohbc_tmd: %#p %u\n", ohbc_tmd_bin, ohbc_tmd_bin_size);
	printf("ohbc_00000000: %#p %u\n", ohbc_00000000_bin, ohbc_00000000_bin_size);
	printf("ohbc_00000001: %#p %u\n", ohbc_00000001_bin, ohbc_00000001_bin_size);

	/* code possibly */
	int cfd;

	int ret = ES_AddTicket(ohbc_tik_bin, ohbc_tik_bin_size, certs_bin, certs_bin_size, NULL, 0);
	// check ret here
	if (ret < 0)
		printf("ES_AddTicket failed (%i)\n%s\n", ret, wad_strerror(ret));

	ret = ES_AddTitleStart(ohbc_tmd_bin, ohbc_tmd_bin_size, certs_bin, certs_bin_size, NULL, 0);
	// check ret here as well
	if (ret < 0)
		printf("ES_AddTitleStart failed (%i)\n%s\n", ret, wad_strerror(ret));

	cfd = ret = ES_AddContentStart(0x000100014C554C5ALL, 0);
	// check ret here
	if (ret < 0)
		printf("ES_AddContentStart failed (%i)\n%s\n", ret, wad_strerror(ret));

	ret = ES_AddContentData(cfd, ohbc_00000000_bin, ohbc_00000000_bin_size);
	if (ret < 0)
		printf("ES_AddContentData failed (%i)\n%s\n", ret, wad_strerror(ret));

	ret = ES_AddContentFinish(cfd);
	if (ret < 0)
		printf("ES_AddContentFinish failed (%i)\n%s\n", ret, wad_strerror(ret));

	cfd = ret = ES_AddContentStart(0x000100014C554C5ALL, 1);
	if (ret < 0)
		printf("ES_AddContentStart 2 failed (%i)\n%s\n", ret, wad_strerror(ret));

	ret = ES_AddContentData(cfd, ohbc_00000001_bin, ohbc_00000001_bin_size);
	if (ret < 0)
		printf("ES_AddContentData 2 failed (%i)\n%s\n", ret, wad_strerror(ret));

	ret = ES_AddContentFinish(cfd);
	if (ret < 0)
		printf("ES_AddContentFinish 2 failed (%i)\n%s\n", ret, wad_strerror(ret));

	ret = ES_AddTitleFinish();
	if (ret < 0)
		printf("ES_AddTitleFinish failed (%i)\n%s\n", ret, wad_strerror(ret));

	printf("Success!");
	sleep(2);

	// yippie
	return 0;
}