\header{
filename =	 "courante-cello.ly";
title =	 "Solo Cello Suites";
subtitle = "Suite II";
% piece = "Courante";
% opus =	 "BWV 1008";
opus =	 "";
composer =	 "Johann Sebastian Bach (1685-1750)";
enteredby =	 "JCN";
copyright =	 "public domain";
}

%{
 Tested Features:breaking algorithm, chords, multivoice, accents, 
 dotted slurs
%}

\version "1.0.16";

\include "courante-urtext.ly";

courante_cello_global = \notes{
	\time 3/4;
	\key f;
	\clef bass;
	\repeat 2 {
		\partial 16;
		s16
		s2.*15
		% hmm
		s2 s8
		\partial 16*11;
		s16
	} \repeat 2 {
		% urg
		s32 \partial 16; s32
		s2.*15
		s16*11
		\partial 16*11;
	}
}

courante_cello_scripts = \notes{
}

courante_cello_staff = \context Staff <
	\$courante
	\$courante_cello_global
	\$courante_cello_scripts
>

\score{
	\$courante_cello_staff
	\paper{
		\include "scs-paper.ly";
		gourlay_maxmeasures = 4.0;
	}
	\midi{ \tempo 4 = 55; }
	\header{ piece = "Courante"; }
}

