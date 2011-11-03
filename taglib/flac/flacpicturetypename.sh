#!/bin/sh

(echo '#include <tstring.h>
#include "flacpicture.h"

TagLib::String TagLib::FLAC::Picture::typeName() const
{
  switch((uint)type())
  {'; sed -n 's/[[:space:]]*\([A-Z][A-Za-z]*\)[[:space:]]*=[[:space:]]*\(0x[0-9]*\),[[:space:]]*$/\tcase \2: return "\1";/p' flacpicture.h; echo '\tdefault: return String::null;
  }
}') > flacpicturetypename.cpp

