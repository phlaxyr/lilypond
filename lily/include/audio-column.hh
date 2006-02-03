/*
  audio-column.hh -- declare Audio_column

  (c) 1997--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef AUDIO_COLUMN_HH
#define AUDIO_COLUMN_HH

#include "lily-proto.hh"
#include "moment.hh"
#include "std-vector.hh"
#include "audio-element.hh"

/**
   generic audio grouped vertically.
*/

class Audio_column : public Audio_element
{
public:
  Audio_column (Moment at_mom);

  void add_audio_item (Audio_item *i);
  Moment at_mom () const;

  Link_array__Audio_item_ audio_items_;

private:
  Audio_column (Audio_column const &);

  Moment at_mom_;
};

#endif // AUDIO_COLUMN_HH
