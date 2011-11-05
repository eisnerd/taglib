/***************************************************************************
    copyright            : (C) 2003 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it  under the terms of the GNU Lesser General Public License version  *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fileref.h>
#include <tfile.h>
#include <asffile.h>
#include <vorbisfile.h>
#include <mpegfile.h>
#include <flacfile.h>
#include <oggflacfile.h>
#include <mpcfile.h>
#include <wavpackfile.h>
#include <speexfile.h>
#include <trueaudiofile.h>
#include <mp4file.h>
#include <tag.h>
#include <string.h>
#include <id3v2framefactory.h>

#include "tag_c.h"

using namespace TagLib;

static List<char *> strings;
static bool unicodeStrings = true;
static bool stringManagementEnabled = true;

TAGLIB_C_DECL
void taglib_set_strings_unicode(BOOL unicode)
{
  unicodeStrings = bool(unicode);
}

TAGLIB_C_DECL
void taglib_set_string_management_enabled(BOOL management)
{
  stringManagementEnabled = bool(management);
}

TAGLIB_C_DECL
void taglib_free(void* pointer)
{
  free(pointer);
}

////////////////////////////////////////////////////////////////////////////////
// TagLib::File wrapper
////////////////////////////////////////////////////////////////////////////////

TAGLIB_C_DECL
TagLib_File *taglib_file_new(const char *filename)
{
  return reinterpret_cast<TagLib_File *>(FileRef::create(filename));
}

TAGLIB_C_DECL
TagLib_File *taglib_file_new_type(const char *filename, TagLib_File_Type type)
{
  switch(type) {
  case TagLib_File_MPEG:
    return reinterpret_cast<TagLib_File *>(new MPEG::File(filename));
  case TagLib_File_OggVorbis:
    return reinterpret_cast<TagLib_File *>(new Ogg::Vorbis::File(filename));
  case TagLib_File_FLAC:
    return reinterpret_cast<TagLib_File *>(new FLAC::File(filename));
  case TagLib_File_MPC:
    return reinterpret_cast<TagLib_File *>(new MPC::File(filename));
  case TagLib_File_OggFlac:
    return reinterpret_cast<TagLib_File *>(new Ogg::FLAC::File(filename));
  case TagLib_File_WavPack:
    return reinterpret_cast<TagLib_File *>(new WavPack::File(filename));
  case TagLib_File_Speex:
    return reinterpret_cast<TagLib_File *>(new Ogg::Speex::File(filename));
  case TagLib_File_TrueAudio:
    return reinterpret_cast<TagLib_File *>(new TrueAudio::File(filename));
  case TagLib_File_MP4:
    return reinterpret_cast<TagLib_File *>(new MP4::File(filename));
  case TagLib_File_ASF:
    return reinterpret_cast<TagLib_File *>(new ASF::File(filename));
  default:
    return 0;
  }

  return 0;
}

TAGLIB_C_DECL
void taglib_file_free(TagLib_File *file)
{
  delete reinterpret_cast<File *>(file);
}

TAGLIB_C_DECL
BOOL taglib_file_is_valid(const TagLib_File *file)
{
	return reinterpret_cast<const File *>(file)->isValid();
}

TAGLIB_C_DECL
TagLib_Tag *taglib_file_tag(const TagLib_File *file)
{
  const File *f = reinterpret_cast<const File *>(file);
  return reinterpret_cast<TagLib_Tag *>(f->tag());
}

TAGLIB_C_DECL
const TagLib_AudioProperties *taglib_file_audioproperties(const TagLib_File *file)
{
  const File *f = reinterpret_cast<const File *>(file);
  return reinterpret_cast<const TagLib_AudioProperties *>(f->audioProperties());
}

TAGLIB_C_DECL
const TagLib_Picture *taglib_file_picture(const TagLib_File *file)
{
  const File *f = reinterpret_cast<const File *>(file);
  return reinterpret_cast<const TagLib_Picture *>(f->picture());
}

TAGLIB_C_DECL
size_t taglib_file_picture_count(const TagLib_File *file)
{
  const File *f = reinterpret_cast<const File *>(file);
  return f->pictures().size();
}

TAGLIB_C_DECL
TagLib_Pictures *taglib_file_pictures(const TagLib_File *file)
{
  const File *f = reinterpret_cast<const File *>(file);
  TagLib_Pictures *it = new TagLib_Pictures;
    it->real = reinterpret_cast<const TagLib_Pictures *>(&f->pictures());
    it->i = 0;
  return it;
}

TAGLIB_C_DECL
const TagLib_Picture *taglib_pictures_next(TagLib_Pictures *pictures)
{
  TagLib::File::PictureList l = *reinterpret_cast<const TagLib::File::_PictureList *>(pictures->real);
  if (pictures->i < l.size())
    return reinterpret_cast<const TagLib_Picture *>(l[pictures->i++]);
  else
    return NULL;
}

TAGLIB_C_DECL
char *taglib_picture_data(const TagLib_Picture *picture)
{
  const TagLib::Picture *p = reinterpret_cast<const TagLib::Picture *>(picture);
  return p->data().data();
}

TAGLIB_C_DECL
char *taglib_picture_base64data(const TagLib_Picture *picture)
{
  const TagLib::Picture *p = reinterpret_cast<const TagLib::Picture *>(picture);
  return p->base64data().data();
}

TAGLIB_C_DECL
char *taglib_picture_mimetype(const TagLib_Picture *picture)
{
  const TagLib::Picture *p = reinterpret_cast<const TagLib::Picture *>(picture);
  char *s = ::strdup(p->mimeType().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
char *taglib_picture_description(const TagLib_Picture *picture)
{
  const TagLib::Picture *p = reinterpret_cast<const TagLib::Picture *>(picture);
  char *s = ::strdup(p->description().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
char *taglib_picture_typename(const TagLib_Picture *picture)
{
  const TagLib::Picture *p = reinterpret_cast<const TagLib::Picture *>(picture);
  char *s = ::strdup(p->typeName().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
unsigned int taglib_picture_typecode(const TagLib_Picture *picture)
{
  const TagLib::Picture *p = reinterpret_cast<const TagLib::Picture *>(picture);
  return p->typeCode();
}

TAGLIB_C_DECL
BOOL taglib_file_save(TagLib_File *file)
{
  return reinterpret_cast<File *>(file)->save();
}

////////////////////////////////////////////////////////////////////////////////
// TagLib::Tag wrapper
////////////////////////////////////////////////////////////////////////////////

TAGLIB_C_DECL
char *taglib_tag_title(const TagLib_Tag *tag)
{
  const Tag *t = reinterpret_cast<const Tag *>(tag);
  char *s = ::strdup(t->title().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
char *taglib_tag_artist(const TagLib_Tag *tag)
{
  const Tag *t = reinterpret_cast<const Tag *>(tag);
  char *s = ::strdup(t->artist().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
char *taglib_tag_album(const TagLib_Tag *tag)
{
  const Tag *t = reinterpret_cast<const Tag *>(tag);
  char *s = ::strdup(t->album().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
char *taglib_tag_comment(const TagLib_Tag *tag)
{
  const Tag *t = reinterpret_cast<const Tag *>(tag);
  char *s = ::strdup(t->comment().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
char *taglib_tag_genre(const TagLib_Tag *tag)
{
  const Tag *t = reinterpret_cast<const Tag *>(tag);
  char *s = ::strdup(t->genre().toCString(unicodeStrings));
  if(stringManagementEnabled)
    strings.append(s);
  return s;
}

TAGLIB_C_DECL
unsigned int taglib_tag_year(const TagLib_Tag *tag)
{
  const Tag *t = reinterpret_cast<const Tag *>(tag);
  return t->year();
}

TAGLIB_C_DECL
unsigned int taglib_tag_track(const TagLib_Tag *tag)
{
  const Tag *t = reinterpret_cast<const Tag *>(tag);
  return t->track();
}

TAGLIB_C_DECL
void taglib_tag_set_title(TagLib_Tag *tag, const char *title)
{
  Tag *t = reinterpret_cast<Tag *>(tag);
  t->setTitle(String(title, unicodeStrings ? String::UTF8 : String::Latin1));
}

TAGLIB_C_DECL
void taglib_tag_set_artist(TagLib_Tag *tag, const char *artist)
{
  Tag *t = reinterpret_cast<Tag *>(tag);
  t->setArtist(String(artist, unicodeStrings ? String::UTF8 : String::Latin1));
}

TAGLIB_C_DECL
void taglib_tag_set_album(TagLib_Tag *tag, const char *album)
{
  Tag *t = reinterpret_cast<Tag *>(tag);
  t->setAlbum(String(album, unicodeStrings ? String::UTF8 : String::Latin1));
}

TAGLIB_C_DECL
void taglib_tag_set_comment(TagLib_Tag *tag, const char *comment)
{
  Tag *t = reinterpret_cast<Tag *>(tag);
  t->setComment(String(comment, unicodeStrings ? String::UTF8 : String::Latin1));
}

TAGLIB_C_DECL
void taglib_tag_set_genre(TagLib_Tag *tag, const char *genre)
{
  Tag *t = reinterpret_cast<Tag *>(tag);
  t->setGenre(String(genre, unicodeStrings ? String::UTF8 : String::Latin1));
}

TAGLIB_C_DECL
void taglib_tag_set_year(TagLib_Tag *tag, unsigned int year)
{
  Tag *t = reinterpret_cast<Tag *>(tag);
  t->setYear(year);
}

TAGLIB_C_DECL
void taglib_tag_set_track(TagLib_Tag *tag, unsigned int track)
{
  Tag *t = reinterpret_cast<Tag *>(tag);
  t->setTrack(track);
}

TAGLIB_C_DECL
void taglib_tag_free_strings()
{
  if(!stringManagementEnabled)
    return;

  for(List<char *>::Iterator it = strings.begin(); it != strings.end(); ++it)
    free(*it);
  strings.clear();
}

////////////////////////////////////////////////////////////////////////////////
// TagLib::AudioProperties wrapper
////////////////////////////////////////////////////////////////////////////////

TAGLIB_C_DECL
int taglib_audioproperties_length(const TagLib_AudioProperties *audioProperties)
{
  const AudioProperties *p = reinterpret_cast<const AudioProperties *>(audioProperties);
  return p->length();
}

TAGLIB_C_DECL
int taglib_audioproperties_bitrate(const TagLib_AudioProperties *audioProperties)
{
  const AudioProperties *p = reinterpret_cast<const AudioProperties *>(audioProperties);
  return p->bitrate();
}

TAGLIB_C_DECL
int taglib_audioproperties_samplerate(const TagLib_AudioProperties *audioProperties)
{
  const AudioProperties *p = reinterpret_cast<const AudioProperties *>(audioProperties);
  return p->sampleRate();
}

TAGLIB_C_DECL
int taglib_audioproperties_channels(const TagLib_AudioProperties *audioProperties)
{
  const AudioProperties *p = reinterpret_cast<const AudioProperties *>(audioProperties);
  return p->channels();
}

TAGLIB_C_DECL
void taglib_id3v2_set_default_text_encoding(TagLib_ID3v2_Encoding encoding)
{
  String::Type type = String::Latin1;

  switch(encoding)
  {
  case TagLib_ID3v2_Latin1:
    type = String::Latin1;
    break;
  case TagLib_ID3v2_UTF16:
    type = String::UTF16;
    break;
  case TagLib_ID3v2_UTF16BE:
    type = String::UTF16BE;
    break;
  case TagLib_ID3v2_UTF8:
    type = String::UTF8;
    break;
  }

  ID3v2::FrameFactory::instance()->setDefaultTextEncoding(type);
}
