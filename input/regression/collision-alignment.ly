\header {

    texidoc = "Notes in different staves should be aligned to the
 left-most note, in case of collisions."

}

\layout  { raggedright = ##t }

\version "2.4.0"

\relative <<
    \new Staff {
	<<
	    { f g } \\
	    { g f }
	>> }
    \new Staff {
	c4 c 
    }
>>
