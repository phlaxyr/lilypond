/*   
  repeated-music.hh -- declare Repeated_music
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998 Jan Nieuwenhuizen <janneke@gnu.org>
  
 */

#ifndef REPEATED_MUSIC_HH
#define REPEATED_MUSIC_HH

#include "music-list.hh"

/**
   Repeats and voltas
 */
class Repeated_music : public Music
{
public:
  int repeats_i_;
  Music_wrapper* repeat_p_;
  Sequential_music* alternative_p_;

  Repeated_music (Music_wrapper *, int n, Sequential_music*);
  Repeated_music (Repeated_music const& s);
  virtual ~Repeated_music ();
  
  virtual void do_print () const;
  virtual void transpose (Musical_pitch p);
  virtual Moment duration () const;

  VIRTUAL_COPY_CONS(Music);
};

#endif /* REPEATED_MUSIC_HH */

