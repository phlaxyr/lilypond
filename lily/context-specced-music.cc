/*   
  context-specced-music.cc --  implement Context_specced_music
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */
#include "context-specced-music.hh"

Context_specced_music::Context_specced_music(Music *m)
  : Music_wrapper  (m)
{
}
