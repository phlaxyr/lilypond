/*   
  system-start-delimiter-engraver.cc -- implement System_start_delimiter_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2000 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */


#include "system-start-delimiter.hh"
#include "engraver.hh"
#include "staff-symbol.hh"
#include "group-interface.hh"
#include "paper-column.hh"
#include "paper-def.hh"

class System_start_delimiter_engraver : public Engraver
{
public:
  VIRTUAL_COPY_CONS(Translator);
  System_start_delimiter_engraver();

protected:
  Spanner * delim_;
  virtual void acknowledge_element (Score_element_info);
  virtual void do_creation_processing ();
  virtual void do_removal_processing ();
};

ADD_THIS_TRANSLATOR(System_start_delimiter_engraver);

void
System_start_delimiter_engraver::acknowledge_element (Score_element_info inf)
{
  if (dynamic_cast<Staff_symbol*> (inf.elem_l_))
    {
      /*
	don't add as Axis_group_interface (delim_).add_element (),
	because that would set the parent as well */
	  
      Pointer_group_interface (delim_).add_element (inf.elem_l_);
    }
  else if (System_start_delimiter * b = dynamic_cast<System_start_delimiter *> (inf.elem_l_))
    {
      SCM gl = b->get_elt_property ("glyph");
      SCM my_gl = delim_->get_elt_property ("glyph");

      /*
	UGH UGH
       */
      if (gh_symbol_p (gl) && gl  == ly_symbol2scm ("brace")
	  && gh_symbol_p (my_gl) && my_gl == ly_symbol2scm ("bracket"))
	b->translate_axis ( -paper_l ()->get_var ("interline"), X_AXIS); // ugh
    }

}

System_start_delimiter_engraver::System_start_delimiter_engraver()
{
  delim_ = 0;
}

void
System_start_delimiter_engraver::do_creation_processing()
{
  delim_ = new System_start_delimiter;
  delim_->set_bound (LEFT, get_staff_info ().command_pcol_l ());

  /*
    You can't override the glyph using \property, you must do it in
    the predefined context (ie. \translator { ... } )
   */
  SCM s = get_property ("systemStartDelimiterGlyph");
  if (gh_symbol_p (s))
    {
      delim_->set_elt_property ("glyph", s);
    }

  announce_element (Score_element_info (delim_,0));
}

void
System_start_delimiter_engraver::do_removal_processing ()
{
  // ugh, should have naming without bracket
  SCM collapse = get_property ("bracketCollapseHeight");
  if (gh_number_p (collapse))
    delim_->set_elt_property ("collapse-height", collapse);
  else
    delim_->set_elt_property ("collapse-height", gh_double2scm (1));
      
  delim_->set_bound (RIGHT, get_staff_info ().command_pcol_l ());
  typeset_element (delim_);
}

