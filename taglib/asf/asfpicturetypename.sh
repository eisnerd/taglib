#!/bin/sh

(echo '#include <tstring.h>
#include "asfpicture.h"

TagLib::String TagLib::ASF::Picture::typeName() const
{
  switch((uint)type())
  {'; sed -n 's/[[:space:]]*\([A-Z][A-Za-z]*\)[[:space:]]*=[[:space:]]*\(0x[0-9]*\),[[:space:]]*$/\tcase \2: return "\1";/p' asfpicture.h; echo '\tdefault: return String::null;
  }
}') > asfpicturetypename.cpp

