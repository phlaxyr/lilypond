/*
  direction.hh -- declare 

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef DIRECTION_HH
#define DIRECTION_HH

enum Direction 
{
  UP=1,
  DOWN=-1,
  LEFT=-1,
  RIGHT=1,
  CENTER=0,
  SMALLER=-1,
  BIGGER=1,
};

inline Direction
flip (Direction *i) {
  if (*i == (Direction)1)
    *i = (Direction)-1;
  else if (*i == (Direction)-1)
    *i = (Direction)1;
  return *i;
}

/**
   if d > 0: the max operator
   if d < 0: the min operator
 */
template<class T> minmax (Direction d, T, T);


#endif // DIRECTION_HH
