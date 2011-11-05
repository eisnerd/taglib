/* Copyright (C) 2003 Scott Wheeler <wheeler@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <stdio.h>

#include <fileref.h>
#include <tag.h>
#include <flac/flacfile.h>
#include <ogg/oggfile.h>
#include <ogg/xiphcomment.h>

using namespace std;

TagLib::String formatSeconds(int seconds)
{
  char secondsString[3];
  sprintf(secondsString, "%02i", seconds);
  return secondsString;
}

int main(int argc, char *argv[])
{
  for(int i = 1; i < argc; i++) {

    cout << "******************** \"" << argv[i] << "\" ********************" << endl;

    TagLib::FileRef f(argv[i]);

    if(!f.isNull() && f.tag()) {

      TagLib::Tag *tag = f.tag();

      cout << "-- TAG --" << endl;
      cout << "title   - \"" << tag->title()   << "\"" << endl;
      cout << "artist  - \"" << tag->artist()  << "\"" << endl;
      cout << "album   - \"" << tag->album()   << "\"" << endl;
      cout << "year    - \"" << tag->year()    << "\"" << endl;
      cout << "comment - \"" << tag->comment() << "\"" << endl;
      cout << "track   - \"" << tag->track()   << "\"" << endl;
      cout << "genre   - \"" << tag->genre()   << "\"" << endl;
      TagLib::Ogg::XiphComment *comment = NULL;
      TagLib::FLAC::File *flac = dynamic_cast<TagLib::FLAC::File *>(f.file());
      if (flac) {
        cout << "flac:" << endl;
        cout << "id3v1   - \"" << flac->ID3v1Tag()   << "\"" << endl;
        cout << "id3v2   - \"" << flac->ID3v2Tag()   << "\"" << endl;
        cout << "xiph    - \"" << flac->xiphComment()   << "\"" << endl;
        comment = flac->xiphComment();
      }
      if (!comment) {
        comment = dynamic_cast<TagLib::Ogg::XiphComment *>(tag);
      }
      if (comment) {
        TagLib::Ogg::FieldListMap fields = comment->fieldListMap();
        for(TagLib::Ogg::FieldListMap::ConstIterator it = fields.begin(), end = fields.end(); it != end; it++) {
          if (!it->second.isEmpty())
            cout << "xiph:" << it->first << " \"" << it->second[0].substr(0,3) << "\"" << endl;
        }
      }
      cout << "pictures- \"" << f.file()->pictures().size()   << "\"" << endl;
      TagLib::File::PictureList l = f.file()->pictures();
      for (TagLib::File::_PictureList::ConstIterator i = l.begin(), end = l.end(); i != end; i++) {
        cout << "\t" << (*i)->typeName() << ' ' << (*i)->mimeType() << ' ' << (*i)->base64data().size() << endl;
      }
      
      cout << "pictures- \"" << tag->pictures().size()   << "\"" << endl;
    }

    if(!f.isNull() && f.audioProperties()) {

      TagLib::AudioProperties *properties = f.audioProperties();

      int seconds = properties->length() % 60;
      int minutes = (properties->length() - seconds) / 60;

      cout << "-- AUDIO --" << endl;
      cout << "bitrate     - " << properties->bitrate() << endl;
      cout << "sample rate - " << properties->sampleRate() << endl;
      cout << "channels    - " << properties->channels() << endl;
      cout << "length      - " << minutes << ":" << formatSeconds(seconds) << endl;
    }
  }
  return 0;
}
