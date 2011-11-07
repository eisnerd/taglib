/***************************************************************************
    copyright            : (C) 2011 by David Eisner, and
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#ifndef TAGLIB_PICTURE_H
#define TAGLIB_PICTURE_H

#include "taglib_export.h"
#include "tstring.h"

namespace TagLib {

  //! A simple, generic interface to embedded picture meta data

  /*!
   * This is an attempt to abstract away the difference in the embedded picture
   * meta data formats of various audio codecs and tagging schemes.  As such it
   * is generally a subset of what is available in the specific formats but should
   * be suitable for most applications.
   */

  class TAGLIB_EXPORT Picture
  {
  public:

    /*!
     * Detroys this Tag instance.
     */
    virtual ~Picture();

    /*!
     * Returns the picture content
     */
    virtual ByteVector data() const = 0;

    /*!
     * Returns the picture content encoded as a base64 string
     */
    virtual ByteVector &base64data() const;

    /*!
     * Returns the MIME type of the picture content.
     */
    virtual String mimeType() const = 0;

    /*!
     * Returns the description; if no description is present or descriptions are
     * not supported by the tag, String::null will be returned.
     */
    virtual String description() const;

    /*!
     * Returns the name of the picture type; if picture types are
     * not supported by the tag, "Other" will be returned.
     */
    virtual String typeName() const;

    /*!
     * Returns the value of the picture type; if picture types are
     * not supported by the tag, 0x00 will be returned.
     */
    virtual uint typeCode() const;
	
    /*!
     * Default sort order.
     * 
     * \see typeCodeOrder()
     */
    virtual bool operator < (Picture &other)
    {
        return typeCodeOrder() < other.typeCodeOrder();
    }

    virtual bool operator <= (Picture &other)
    {
        return typeCodeOrder() <= other.typeCodeOrder();
    }

    virtual bool operator >= (Picture &other)
    {
        return !(*this < other);
    }

    virtual bool operator > (Picture &other)
    {
        return !(*this <= other);
    }

  protected:
    /*!
     * Construct a Picture.  This is protected since tags should only be instantiated
     * through subclasses.
     */
    Picture();

    /*!
     * Default sort order representative.
     */
	virtual uint typeCodeOrder() const;

  private:
    Picture(const Picture &);
    Picture &operator=(const Picture &);
    mutable ByteVector *d;
  };
}

#endif
