#include <tstring.h>
#include "asfpicture.h"

TagLib::String TagLib::ASF::Picture::typeName() const
{
  switch((uint)type())
  {
	case 0x00: return "Other";
	case 0x01: return "FileIcon";
	case 0x02: return "OtherFileIcon";
	case 0x03: return "FrontCover";
	case 0x04: return "BackCover";
	case 0x05: return "LeafletPage";
	case 0x06: return "Media";
	case 0x07: return "LeadArtist";
	case 0x08: return "Artist";
	case 0x09: return "Conductor";
	case 0x10: return "MovieScreenCapture";
	case 0x11: return "ColouredFish";
	case 0x12: return "Illustration";
	case 0x13: return "BandLogo";
	default: return String::null;
  }
}
