#!/bin/sh

(echo '#include <tstring.h>
#include "attachedpictureframe.h"

TagLib::String TagLib::ID3v2::AttachedPictureFrame::typeName() const
{
  switch((uint)type())
  {'; sed -n 's/[[:space:]]*\([A-Z][A-Za-z]*\)[[:space:]]*=[[:space:]]*\(0x[0-9]*\),[[:space:]]*$/\tcase \2: return "\1";/p' attachedpictureframe.h; echo '\tdefault: return String::null;
  }
}') > attachedpictureframetypename.cpp

