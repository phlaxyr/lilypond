
\header
{

    texidoc = "Quotations take into account the transposition of both
source and target.  In this example, all instruments play sounding
central C, the target is a instrument in F.  The target part may be
@code{\\transpose}d.  In this case, all the pitches (including the
quoted ones) will transposed as well.  "

}

\version "2.4.0"

\layout { raggedright = ##t }


\addquote clarinet  {
    \transposition bes
    d'16 d'16 d'8 
    d'16 d'16 d'8 
    d'16 d'16 d'8 
    d'16 d'16 d'8 
}

\addquote sax  {
    \transposition es'
    a8 a a a a a  a a 
}

quoteTest = {
    \transposition f  % french horn
    
    g'4
    << \quote clarinet 4 s4^"clar" >> 
    << \quote sax 4 s4^"sax" >> 
}


<< \quoteTest
   \new Staff
   << \transpose c' d' \quoteTest
     s4_"up 1 tone"
  >>
>>
