/*   
  ly-symbols.hh -- declare SCM symbols.
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef LY_SYMBOLS_HH
#define LY_SYMBOLS_HH

#ifndef DECLARE_LY_SYMBOL
#define DECLARE_LY_SYMBOL(a) extern SCM a ## _scm_sym
#endif

DECLARE_LY_SYMBOL(at_line_start);
DECLARE_LY_SYMBOL(beam);
DECLARE_LY_SYMBOL(beam_thickness);
DECLARE_LY_SYMBOL(beam_dir);
DECLARE_LY_SYMBOL(beam_gap);
DECLARE_LY_SYMBOL(bracket);
DECLARE_LY_SYMBOL(break_helper_only);
DECLARE_LY_SYMBOL(break_priority);
DECLARE_LY_SYMBOL(breakable);
DECLARE_LY_SYMBOL(char);
DECLARE_LY_SYMBOL(change);
DECLARE_LY_SYMBOL(damping);
DECLARE_LY_SYMBOL(dashed);
DECLARE_LY_SYMBOL(extremal);
DECLARE_LY_SYMBOL(extra_space);
DECLARE_LY_SYMBOL(dir_forced);
DECLARE_LY_SYMBOL(extender_height);
DECLARE_LY_SYMBOL(filledbox);
DECLARE_LY_SYMBOL(fontsize);
DECLARE_LY_SYMBOL(header);
DECLARE_LY_SYMBOL(horizontal_shift);
DECLARE_LY_SYMBOL(interbeam);
DECLARE_LY_SYMBOL(interbeam4);
DECLARE_LY_SYMBOL(interline);
DECLARE_LY_SYMBOL(linewidth);
DECLARE_LY_SYMBOL(molecule);
DECLARE_LY_SYMBOL(notewidth);
DECLARE_LY_SYMBOL(non_default);
DECLARE_LY_SYMBOL(octave_dir);
DECLARE_LY_SYMBOL(output);
DECLARE_LY_SYMBOL(padding);
DECLARE_LY_SYMBOL(pianobrace);
DECLARE_LY_SYMBOL(placebox);
DECLARE_LY_SYMBOL(rulesym);
DECLARE_LY_SYMBOL(rulethickness);
DECLARE_LY_SYMBOL(script_priority);
DECLARE_LY_SYMBOL(staffheight);
DECLARE_LY_SYMBOL(style);
DECLARE_LY_SYMBOL(text);
DECLARE_LY_SYMBOL(transparent);
DECLARE_LY_SYMBOL(tuplet);
DECLARE_LY_SYMBOL(tuplet_thick);
DECLARE_LY_SYMBOL(visibility_lambda);
DECLARE_LY_SYMBOL(volta);
DECLARE_LY_SYMBOL(volta_thick);


#endif /* LY_SYMBOLS_HH */

