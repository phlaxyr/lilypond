/*
  scoreline.cc -- implement Line_of_score

  source file of the GNU LilyPond music typesetter

  (c) 1996,  1997--1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "line-of-score.hh"
#include "paper-def.hh"
#include "paper-outputter.hh"
#include "paper-column.hh"
#include "paper-score.hh"
#include "warn.hh"

Line_of_score::Line_of_score()
{
  set_axes (Y_AXIS,Y_AXIS);
}

bool
Line_of_score::contains_b (Paper_column const* c) const
{
  return cols_.find_l ((Paper_column*)(c));
}

// const?
Line_of_score*
Line_of_score::set_breaking (Array<Column_x_positions> const &breaking, int j) const
{
  const Link_array<Paper_column> &curline (breaking[j].cols_);
  const Array<Real> &config (breaking[j].config_);
  Line_of_score *line_l=0;

  line_l = dynamic_cast <Line_of_score*> (clone());

  line_l->cols_ = curline;
  line_l->set_bounds(LEFT,curline[0]);
  line_l->set_bounds(RIGHT,curline.top());
	
  for (int i=0; i < curline.size(); i++)
    {
      curline[i]->translate_axis (config[i],X_AXIS);
      curline[i]->line_l_ = dynamic_cast<Line_of_score*>(line_l);
    }

  Breaking_information b;
  b.bounds_ = line_l->spanned_drul_;
  b.broken_spanner_l_ = line_l;
  b.line_l_ = line_l;
  
  broken_info_.push (b);

  return line_l;
}

void
Line_of_score::add_column (Paper_column*p)
{
  cols_.push (p);
}

void
Line_of_score::do_print() const
{
  Spanner::do_print();
  Axis_group_spanner::do_print ();
}

Link_array<Score_element>
Line_of_score::get_extra_dependencies () const
{
  Link_array<Score_element> r (Axis_group_spanner::get_extra_dependencies ());
  for (int i=0; i < cols_.size (); i++)
    r.push (cols_[i]);
  return r;
}

void
Line_of_score::do_substitute_element_pointer (Score_element*o, Score_element*n)
{
  if (Paper_column *p = dynamic_cast<Paper_column*>(o))
    cols_.substitute (p, dynamic_cast<Paper_column*>(n));
}

void
Line_of_score::output_all (bool last_line)
{
  Interval i(extent(Y_AXIS));
  if (i.empty_b())
    warning ("Huh? Empty Line_of_score?");
  else
    translate_axis (- i[MAX], Y_AXIS);
  
  pscore_l_->outputter_l_->start_line (i.length ());
  Super_element::output_all ();
  if (last_line)
    pscore_l_->outputter_l_->stop_last_line();
  else
    pscore_l_->outputter_l_->stop_line ();
}
