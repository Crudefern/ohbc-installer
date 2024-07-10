#include <errno.h>

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