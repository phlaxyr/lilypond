%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.14.0"

\header {
  lsrtags = "repeats, tweaks-and-overrides"

%% Translation of GIT committish: 2d548a99cb9dba80f2ff035582009477cd37eceb
  texidoces = "
Se pueden mostrar los contadores de las repeticiones del tipo
porcentaje a intervalos regulares mediante el establecimiento de
la propiedad de contexto @code{repeatCountVisibility}.

"
  doctitlees = "Visibilidad del contador de repeticiones de tipo porcentaje"


%% Translation of GIT committish: 0a868be38a775ecb1ef935b079000cebbc64de40
  texidocde = "
Prozentwiederholungszähler können in regelmäßigen Intervallen angezeigt
werden, indem man die Eigenschaft @code{repeatCountVisibility} beeinflusst.
"
  doctitlede = "Sichtbarkeit von Prozent-Wiederholungen"

%% Translation of GIT committish: a5bde6d51a5c88e952d95ae36c61a5efc22ba441
  texidocfr = "
Le numéro de mesure répétée sera imprimé à intervalle régulier si vous
déterminez la propriété de contexte @code{repeatCountVisibility}.

"
  doctitlefr = "Affichage du numéro de répétition en pourcent"


  texidoc = "
Percent repeat counters can be shown at regular intervals by setting
the context property @code{repeatCountVisibility}.

"
  doctitle = "Percent repeat count visibility"
} % begin verbatim

\relative c'' {
  \set countPercentRepeats = ##t
  \set repeatCountVisibility = #(every-nth-repeat-count-visible 5)
  \repeat percent 10 { c1 } \break
  \set repeatCountVisibility = #(every-nth-repeat-count-visible 2)
  \repeat percent 6 { c1 d1 }
}
