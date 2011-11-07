#pragma once
#ifndef BASE64_H
#define BASE64_H

// From github.com/msanders/autopy and modified for a taglib toolkit-style interface

// Copyright 2010 Michael Sanders.
// AutoPy (the software) is licensed under the terms of the MIT license.

#include <tbytevector.h>
#include <tstring.h>

/* Decode a base64 encoded string discarding line breaks and noise.
 *
 * Returns a new string to be free()'d by caller, or NULL on error.
 * Returned string is guaranteed to be NUL-terminated.
 *
 * If |retlen| is not NULL, it is set to the length of the returned string
 * (minus the NUL-terminator) on successful return. */
TagLib::ByteVector *base64decode(const TagLib::String &buf);

/* Encode a base64 encoded string without line breaks or noise.
 *
 * Returns a new string to be free()'d by caller, or NULL on error.
 * Returned string is guaranteed to be NUL-terminated with the correct padding.
 *
 * If |retlen| is not NULL, it is set to the length of the returned string
 * (minus the NUL-terminator) on successful return. */
TagLib::ByteVector *base64encode(const TagLib::ByteVector &, bool = false);

#endif /* BASE64_H */
