/***************************************************************************
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

#include <tbytevector.h>
#include <tdebug.h>
#include <base64.h>

#include <xiphcomment.h>
#include <flac/flacpicture.h>

using namespace TagLib;

class Ogg::XiphComment::XiphCommentPrivate
{
public:
  XiphCommentPrivate() {
    pictureListValid = false;
  }
  FieldListMap fieldListMap;
  String vendorID;
  String commentField;

  _PictureList pictureList;
  bool pictureListValid;
  
  void Invalidate()
  {
    pictureListValid = false;
    for(_PictureList::ConstIterator it = pictureList.begin(), end = pictureList.end(); it != end; it++) {
      if (*it)
        delete *it;
    }
    pictureList.clear();
  }
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Ogg::XiphComment::XiphComment() : TagLib::Tag()
{
  d = new XiphCommentPrivate();
}

Ogg::XiphComment::XiphComment(const ByteVector &data) : TagLib::Tag()
{
  d = new XiphCommentPrivate();
  parse(data);
}

Ogg::XiphComment::~XiphComment()
{
  d->Invalidate();
  delete d;
}

String Ogg::XiphComment::title() const
{
  if(d->fieldListMap["TITLE"].isEmpty())
    return String::null;
  return d->fieldListMap["TITLE"].front();
}

String Ogg::XiphComment::artist() const
{
  if(d->fieldListMap["ARTIST"].isEmpty())
    return String::null;
  return d->fieldListMap["ARTIST"].front();
}

String Ogg::XiphComment::album() const
{
  if(d->fieldListMap["ALBUM"].isEmpty())
    return String::null;
  return d->fieldListMap["ALBUM"].front();
}

String Ogg::XiphComment::comment() const
{
  if(!d->fieldListMap["DESCRIPTION"].isEmpty()) {
    d->commentField = "DESCRIPTION";
    return d->fieldListMap["DESCRIPTION"].front();
  }

  if(!d->fieldListMap["COMMENT"].isEmpty()) {
    d->commentField = "COMMENT";
    return d->fieldListMap["COMMENT"].front();
  }

  return String::null;
}

String Ogg::XiphComment::genre() const
{
  if(d->fieldListMap["GENRE"].isEmpty())
    return String::null;
  return d->fieldListMap["GENRE"].front();
}

TagLib::uint Ogg::XiphComment::year() const
{
  if(!d->fieldListMap["DATE"].isEmpty())
    return d->fieldListMap["DATE"].front().toInt();
  if(!d->fieldListMap["YEAR"].isEmpty())
    return d->fieldListMap["YEAR"].front().toInt();
  return 0;
}

TagLib::uint Ogg::XiphComment::track() const
{
  if(!d->fieldListMap["TRACKNUMBER"].isEmpty())
    return d->fieldListMap["TRACKNUMBER"].front().toInt();
  if(!d->fieldListMap["TRACKNUM"].isEmpty())
    return d->fieldListMap["TRACKNUM"].front().toInt();
  return 0;
}

void Ogg::XiphComment::setTitle(const String &s)
{
  addField("TITLE", s);
}

void Ogg::XiphComment::setArtist(const String &s)
{
  addField("ARTIST", s);
}

void Ogg::XiphComment::setAlbum(const String &s)
{
  addField("ALBUM", s);
}

void Ogg::XiphComment::setComment(const String &s)
{
  addField(d->commentField.isEmpty() ? "DESCRIPTION" : d->commentField, s);
}

void Ogg::XiphComment::setGenre(const String &s)
{
  addField("GENRE", s);
}

void Ogg::XiphComment::setYear(uint i)
{
  removeField("YEAR");
  if(i == 0)
    removeField("DATE");
  else
    addField("DATE", String::number(i));
}

void Ogg::XiphComment::setTrack(uint i)
{
  removeField("TRACKNUM");
  if(i == 0)
    removeField("TRACKNUMBER");
  else
    addField("TRACKNUMBER", String::number(i));
}

bool Ogg::XiphComment::isEmpty() const
{
  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it)
    if(!(*it).second.isEmpty())
      return false;

  return true;
}

TagLib::uint Ogg::XiphComment::fieldCount() const
{
  uint count = 0;

  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it)
    count += (*it).second.size();

  return count;
}

const Ogg::FieldListMap &Ogg::XiphComment::fieldListMap() const
{
  return d->fieldListMap;
}

String Ogg::XiphComment::vendorID() const
{
  return d->vendorID;
}

void Ogg::XiphComment::addField(const String &key, const String &value, bool replace)
{
  if(replace)
    removeField(key.upper());

  if(!key.isEmpty() && !value.isEmpty())
    d->fieldListMap[key.upper()].append(value);
}

void Ogg::XiphComment::removeField(const String &key, const String &value)
{
  if(!value.isNull()) {
    StringList::Iterator it = d->fieldListMap[key].begin();
    while(it != d->fieldListMap[key].end()) {
      if(value == *it)
        it = d->fieldListMap[key].erase(it);
      else
        it++;
    }
  }
  else
    d->fieldListMap.erase(key);
}

bool Ogg::XiphComment::contains(const String &key) const
{
  return d->fieldListMap.contains(key) && !d->fieldListMap[key].isEmpty();
}

Ogg::XiphComment::Picture::Picture(ByteVector *data)
  : _data(*data)
{ }
  
Ogg::XiphComment::Picture::~Picture()
{
  delete &_data;
}

ByteVector Ogg::XiphComment::Picture::data() const
{
  return _data;
}

String Ogg::XiphComment::Picture::mimeType() const
{
  ByteVector jpg(2,0xff); jpg[1] = 0xd8;
  if (_data.startsWith(jpg) && (jpg[1] = 0xd9) && _data.endsWith(jpg))
    return "image/jpeg";

  if (_data.startsWith("\211PNG\r\n\032\n"))
    return "image/png";

  ByteVector gif("GIF89a");
  if (_data.startsWith(gif) || ((gif[4] = '7') && _data.startsWith(gif)))
    return "image/gif";

  ByteVector bmp(2,0x42); bmp[1] = 0x4D;
  if (_data.startsWith(bmp))
    return "image/bmp";

  return "image/";
}

TagLib::Picture *Ogg::XiphComment::picture() const
{
	PictureList pictureList = pictures();
	if (pictureList.isEmpty())
		return NULL;
	else
    return pictureList.front();
}

Ogg::XiphComment::PictureList Ogg::XiphComment::pictures() const
{
  if (!d->pictureListValid)
  {
    const StringList *l;
    // See http://wiki.xiph.org/index.php/VorbisComment#Cover_art
    l = &d->fieldListMap["METADATA_BLOCK_PICTURE"];
    for(StringList::ConstIterator j = l->begin(), end = l->end(); j != end; j++) {
      ByteVector *decoded = base64decode(*j);
      if (decoded) {
        d->pictureList.sortedInsert(new FLAC::Picture(*decoded));
        delete decoded;
      }
    }
    // See http://wiki.xiph.org/VorbisComment#Unofficial_COVERART_field_.28deprecated.29
    l = &d->fieldListMap["COVERART"];
    for(StringList::ConstIterator j = l->begin(), end = l->end(); j != end; j++) {
      ByteVector *decoded = base64decode(*j);
      if (decoded) {
        d->pictureList.append(new Ogg::XiphComment::Picture(decoded));
      }
    }
    
    /*
    for(FieldListMap::ConstIterator i = d->fieldListMap.begin(), end = d->fieldListMap.end(); i != end; i++) {
      for(StringList::ConstIterator j = i->second.begin(), end = i->second.end(); j != end; j++) {
        Picture *picture = dynamic_cast<Picture *>(*j);
        if(picture) {
          d->pictureList.sortedInsert(picture);
        }
      }
    }
    */
	d->pictureListValid = true;
  }
  return d->pictureList;
}

ByteVector Ogg::XiphComment::render() const
{
  return render(true);
}

ByteVector Ogg::XiphComment::render(bool addFramingBit) const
{
  ByteVector data;

  // Add the vendor ID length and the vendor ID.  It's important to use the
  // length of the data(String::UTF8) rather than the length of the the string
  // since this is UTF8 text and there may be more characters in the data than
  // in the UTF16 string.

  ByteVector vendorData = d->vendorID.data(String::UTF8);

  data.append(ByteVector::fromUInt(vendorData.size(), false));
  data.append(vendorData);

  // Add the number of fields.

  data.append(ByteVector::fromUInt(fieldCount(), false));

  // Iterate over the the field lists.  Our iterator returns a
  // std::pair<String, StringList> where the first String is the field name and
  // the StringList is the values associated with that field.

  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it) {

    // And now iterate over the values of the current list.

    String fieldName = (*it).first;
    StringList values = (*it).second;

    StringList::ConstIterator valuesIt = values.begin();
    for(; valuesIt != values.end(); ++valuesIt) {
      ByteVector fieldData = fieldName.data(String::UTF8);
      fieldData.append('=');
      fieldData.append((*valuesIt).data(String::UTF8));

      data.append(ByteVector::fromUInt(fieldData.size(), false));
      data.append(fieldData);
    }
  }

  // Append the "framing bit".

  if(addFramingBit)
    data.append(char(1));

  return data;
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void Ogg::XiphComment::parse(const ByteVector &data)
{
  // The first thing in the comment data is the vendor ID length, followed by a
  // UTF8 string with the vendor ID.

  uint pos = 0;

  int vendorLength = data.mid(0, 4).toUInt(false);
  pos += 4;

  d->vendorID = String(data.mid(pos, vendorLength), String::UTF8);
  pos += vendorLength;

  // Next the number of fields in the comment vector.

  uint commentFields = data.mid(pos, 4).toUInt(false);
  pos += 4;

  if(commentFields > (data.size() - 8) / 4) {
    return;
  }

  for(uint i = 0; i < commentFields; i++) {

    // Each comment field is in the format "KEY=value" in a UTF8 string and has
    // 4 bytes before the text starts that gives the length.

    uint commentLength = data.mid(pos, 4).toUInt(false);
    pos += 4;

    String comment = String(data.mid(pos, commentLength), String::UTF8);
    pos += commentLength;
    if(pos > data.size()) {
      break;
    }

    int commentSeparatorPosition = comment.find("=");
    if(commentSeparatorPosition == -1) {
      break;
    }

    String key = comment.substr(0, commentSeparatorPosition);
    String value = comment.substr(commentSeparatorPosition + 1);

    addField(key, value, false);
  }
}
