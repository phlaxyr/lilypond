/*
  staff-margin-engraver.cc -- implement Staff_margin_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1998--1999 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "staff-margin-engraver.hh"
#include "bar.hh"
#include "time-description.hh"
#include "g-text-item.hh"
#include "g-staff-side.hh"

ADD_THIS_TRANSLATOR (Staff_margin_engraver);

Staff_margin_engraver::Staff_margin_engraver ()
{
  axis_ = X_AXIS;
  type_ = "margin";
  visibility_lambda_
    = gh_eval_str ("postbreak_only_visibility");
}


/*
    TODO

    should be able to set whole paragraph (multiple lines, center) to
    left (right?) of staff, e.g.:
                    ______
                   |_______
      2 Clarinetti |________
         (Bb)      |___________
	           |______________
*/
void
Staff_margin_engraver::acknowledge_element (Score_element_info inf)
{
  Item *i = cast_to_interesting_item (inf.elem_l_);
  if (!i || inf.origin_grav_l_arr_.size() != 1)
    return;


  String long_str = get_property ("instrument", 0);
  String str = get_property ("instr", 0);
  if (now_mom () > Moment (0) && str.length_i ())
    long_str = str;

  if (long_str.empty_b ())
    return;

  create_items (0);
  text_p_->text_str_ = long_str;
  staff_side_p_->dir_ = LEFT;
  Bar_script_engraver::do_acknowledge_element (i);

  /*
    UGH. ignores font size settings.
   */
  Interval iv(text_p_->extent (Y_AXIS));
  text_p_->translate_axis (- iv.center (),  Y_AXIS);
}


