%{MudelaHeader

 filename:multi.ly
 title: 
 description:  stupid testfile for pl68 features.
 composers:
 entered-by:HWN
 copyright: public domain

 Tested Features: \multi
EndMudelaHeader
%}


\version "0.1.1";

\score{
	\melodic 
		{ \octave c'; c4 c4 
			< \multi 1;  { c2 c2 } { c'2 c'2 } > 
			< \multi 2;  { \stem \down; c2 c2 } { \stem \up; c'2 c'2 } > 
			< \multi 3;  { \clef "bass"; c2 c2 } { \meter 2/4;\bar "||"; \key fis cis gis; c'2 c'2 } > 
	 			c2 c1 
			c1 c1
			< \multi 1; < \multi 3; 
				{ \meter 2/4; \clef "violin"; c2 c2 }
				{ \meter 2/4; \clef "bass"; c2 c2 }
			>
			< \multi 3; 
				{ \meter 2/4; \clef "violin"; c2 c2 }
				{ \meter 2/4; \clef "bass"; c2 c2 }
			>
			>
		}
}
