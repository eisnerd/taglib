#include <limits.h>
#include "base64.h"

// From github.com/msanders/autopy and modified for a taglib toolkit-style interface

// Copyright 2010 Michael Sanders.
// AutoPy (the software) is licensed under the terms of the MIT license.

/* Encoding table as described in RFC1113. */
const static char b64_encode_table[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz0123456789+/";

/* Decoding table. */
const static short b64_decode_table[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* 00-0F */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* 10-1F */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,	/* 20-2F */
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,	/* 30-3F */
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* 40-4F */
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,	/* 50-5F */
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,	/* 60-6F */
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,	/* 70-7F */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* 80-8F */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* 90-9F */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* A0-AF */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* B0-BF */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* C0-CF */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* D0-DF */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* E0-EF */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1	/* F0-FF */
};

using namespace TagLib;

ByteVector *base64decode(const String &src)
{
  if (src.isEmpty())
    return NULL;

	short digit, lastdigit;
	size_t i, j, buflen = src.length();
	const TagLib::uint maxlen = ((buflen + 3) / 4) * 3;

	digit = lastdigit = j = 0;
	ByteVector &decoded = *new ByteVector(maxlen + 1);
	for (i = 0; i < buflen; ++i) {
		if ((digit = b64_decode_table[src[i]]) != -1) {
			/* Decode block */
			switch (i % 4) {
				case 1:
					decoded[j++] = ((lastdigit << 2) | ((digit & 0x30) >> 4));
					break;
				case 2:
					decoded[j++] = (((lastdigit & 0xF) << 4) | ((digit & 0x3C) >> 2));
					break;
				case 3:
					decoded[j++] = (((lastdigit & 0x03) << 6) | digit);
					break;
			}
			lastdigit = digit;
		}
	}

  decoded.resize(j); /* Assumes the resize() implementation returns *this, rather than allocating a new ByteVector */
	return &decoded; /* Must be free()'d by caller */
}

ByteVector *base64encode(const ByteVector &in, bool insertLFs)
{
/*
   Derived from the WebKit WebCore implementation:

   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2006 Alexey Proskuryakov <ap@webkit.org>
   Copyright (C) 2007, 2008 Apple Inc. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL)
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

   This code is based on the java implementation in HTTPClient
   package by Ronald Tschalär Copyright (C) 1996-1999.
*/

	if (in.isEmpty())
		return NULL;

	// If the input string is pathologically large, just return nothing.
	// Note: Keep this in sync with the "out_len" computation below.
	// Rather than being perfectly precise, this is a bit conservative.
	const unsigned maxInputBufferSize = UINT_MAX / 77 * 76 / 4 * 3 - 2;
	if (in.size() > maxInputBufferSize)
		return NULL;

	unsigned sidx = 0;
	unsigned didx = 0;
	const char* data = in.data();
	const unsigned len = in.size();

	unsigned out_len = ((len + 2) / 3) * 4;

	// Deal with the 76 character per line limit specified in RFC 2045.
	insertLFs = (insertLFs && out_len > 76);
	if (insertLFs)
		out_len += ((out_len - 1) / 76);

	int count = 0;
	ByteVector &out = *new ByteVector(out_len + 1);

	// 3-byte to 4-byte conversion + 0-63 to ascii printable conversion
	if (len > 1) {
		while (sidx < len - 2) {
			if (insertLFs) {
				if (count && (count % 76) == 0)
					out[didx++] = '\n';
				count += 4;
			}
			out[didx++] = b64_encode_table[(data[sidx] >> 2) & 077];
			out[didx++] = b64_encode_table[((data[sidx + 1] >> 4) & 017) | ((data[sidx] << 4) & 077)];
			out[didx++] = b64_encode_table[((data[sidx + 2] >> 6) & 003) | ((data[sidx + 1] << 2) & 077)];
			out[didx++] = b64_encode_table[data[sidx + 2] & 077];
			sidx += 3;
		}
	}

	if (sidx < len) {
		if (insertLFs && (count > 0) && (count % 76) == 0)
			out[didx++] = '\n';

		out[didx++] = b64_encode_table[(data[sidx] >> 2) & 077];
		if (sidx < len - 1) {
			out[didx++] = b64_encode_table[((data[sidx + 1] >> 4) & 017) | ((data[sidx] << 4) & 077)];
			out[didx++] = b64_encode_table[(data[sidx + 1] << 2) & 077];
		} else
			out[didx++] = b64_encode_table[(data[sidx] << 4) & 077];
	}

	// Add padding
	while (didx < out.size()) {
		out[didx] = '=';
		didx++;
	}

	out[out_len] = '\0';
	return &out; /* Must be free()'d by caller */
}
