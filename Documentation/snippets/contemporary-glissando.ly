%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.14.0"

\header {
  lsrtags = "expressive-marks"

%% Translation of GIT committish: 2d548a99cb9dba80f2ff035582009477cd37eceb
  texidoces = "
Se puede tipografiar un gissando contemporáneo sin nota final
utilizando una nota oculta y temporalización de cadenza.

"
  doctitlees = "Glissando contemporáneo"


%% Translation of GIT committish: 0a868be38a775ecb1ef935b079000cebbc64de40
texidocde = "
Ein modernes Glissando ohne eine Endnote kann gesetzt werden, indem
eine Kadenz eingesetzt wird und die Endnote unsichtbar gemacht wird.

"
  doctitlede = "Moderne Glissandi"

%% Translation of GIT committish: 217cd2b9de6e783f2a5c8a42be9c70a82195ad20
  texidocfr = "
De nos jours, il peut arriver que la note d'arrivée d'un glissando soit
absente de la partition.  Pour ce faire, il vous faudra utiliser une
cadence et « masquer » la note d'arrivée.

"
  doctitlefr = "Glissando contemporain"


  texidoc = "
A contemporary glissando without a final note can be typeset using a
hidden note and cadenza timing.

"
  doctitle = "Contemporary glissando"
} % begin verbatim

\relative c'' {
  \time 3/4
  \override Glissando #'style = #'zigzag
  c4 c
  \cadenzaOn
  c4\glissando
  \hideNotes
  c,,4
  \unHideNotes
  \cadenzaOff
  \bar "|"
}

