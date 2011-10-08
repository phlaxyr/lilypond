%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.14.0"

\header {
  lsrtags = "repeats"

%% Translation of GIT committish: 2d548a99cb9dba80f2ff035582009477cd37eceb
  texidoces = "
Las repeticiones de compases completos de más de dos repeticiones
pueden llevar un contador si se activa la propiedad adecuada, como se
ve en este ejemplo:

"
  doctitlees = "Contador de repeticiones de tipo porcentaje"


%% Translation of GIT committish: 0a868be38a775ecb1ef935b079000cebbc64de40
  texidocde = "
Ganztaktwiederholungen mit mehr als zwei Wiederholungen erhalten einen
Zähler, wenn man die entsprechende Eigenschaft einsetzt:

"
  doctitlede = "Prozent-Wiederholungen zählen"

%% Translation of GIT committish: a5bde6d51a5c88e952d95ae36c61a5efc22ba441
  texidocfr = "
Les répétitions de plus de 2 mesures sont surmontées d'un compteur,
si l'on active la propriété @code{countPercentRepeats} comme le montre
l'exemple suivant :

"
  doctitlefr = "Compteur de répétition en pourcent"


  texidoc = "
Measure repeats of more than two repeats can get a counter when the
convenient property is switched, as shown in this example:

"
  doctitle = "Percent repeat counter"
} % begin verbatim

\relative c'' {
  \set countPercentRepeats = ##t
  \repeat percent 4 { c1 }
}

