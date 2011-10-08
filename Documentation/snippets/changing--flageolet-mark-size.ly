%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.14.0"

\header {
  lsrtags = "expressive-marks, unfretted-strings"

%% Translation of GIT committish: 2d548a99cb9dba80f2ff035582009477cd37eceb
  texidoces = "
Para hacer más pequeño el círculo de @code{\\flageolet} (armónico)
utilice la siguiente función de Scheme.

"
  doctitlees = "Cambiar el tamaño de la marca de \\flageolet"


%% Translation of GIT committish: 0a868be38a775ecb1ef935b079000cebbc64de40
  texidocde = "
Um den @code{\\flageolet}-Kreis kleiner zu machen, kann diese
Scheme-Funktion eingesetzt werden.

"
  doctitlede = "\\flageolet-Zeichen verkleinern"

%% Translation of GIT committish: 5b980ee07bba23d3633de1d371fb2926f164e295

  texidocfr = "
Il est possible de rapetisser le cercle d'un @code{\\flageolet} grâce
à une fonction Scheme.

"
  doctitlefr = "Modifier la taille d'un \\flageolet"


  texidoc = "
To make the @code{\\flageolet} circle smaller use the following Scheme
function.

"
  doctitle = "Changing \\flageolet mark size"
} % begin verbatim

smallFlageolet =
#(let ((m (make-articulation "flageolet")))
   (set! (ly:music-property m 'tweaks)
         (acons 'font-size -3
                (ly:music-property m 'tweaks)))
   m)

\layout { ragged-right = ##f }

\relative c'' {
  d4^\flageolet_\markup { default size } d_\flageolet
  c4^\smallFlageolet_\markup { smaller } c_\smallFlageolet
}

