/*
  p-score.cc -- implement Paper_score

  source file of the GNU LilyPond music typesetter

  (c) 1996--2000 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "main.hh"
#include "debug.hh"
#include "lookup.hh"
#include "spanner.hh"
#include "paper-def.hh"
#include "line-of-score.hh"
#include "paper-column.hh"
#include "paper-score.hh"
#include "paper-column.hh"
#include "scope.hh"
#include "gourlay-breaking.hh"
#include "paper-stream.hh"
#include "paper-outputter.hh"
#include "file-results.hh"
#include "misc.hh"
#include "all-font-metrics.hh"

Paper_score::Paper_score ()
{
  paper_l_ =0;
  outputter_l_ =0;
  line_l_ = new Line_of_score;
  element_smob_list_ = SCM_EOL;
  typeset_line (line_l_);
}

void
Paper_score::typeset_line (Line_of_score *l)
{
  line_l_->pscore_l_ = this;
  element_smob_list_ = gh_cons (l->self_scm_, element_smob_list_);
}

Paper_score::Paper_score (Paper_score const &s)
  : Music_output (s)
{
  assert (false);
}

Array<Column_x_positions>
Paper_score::calc_breaking ()
{
  Break_algorithm *algorithm_p=0;
  Array<Column_x_positions> sol;

  algorithm_p = new Gourlay_breaking ;
  algorithm_p->set_pscore (this);
  sol = algorithm_p->solve ();
  delete algorithm_p;

  return sol;
}



/*
  urg. clean me
 */
void
Paper_score::process ()
{
  progress_indication (_ ("Preprocessing elements...") + " ");

  /*
    Be sure to set breakability on first & last column.
   */
  Link_array<Paper_column> pc (line_l_->column_l_arr ());
  
  pc[0]->set_elt_property ("breakable", SCM_BOOL_T);
  pc.top ()->set_elt_property ("breakable", SCM_BOOL_T);

  line_l_->pre_processing ();
 
  Array<Column_x_positions> breaking = calc_breaking ();
  line_l_->break_into_pieces (breaking);

  
  outputter_l_ = new Paper_outputter (paper_l_->paper_stream_p ());
;
  outputter_l_->output_header ();
  outputter_l_->output_version();
  
  if (header_global_p)
    outputter_l_->output_scope (header_global_p, "mudela");
  if (header_l_)
    outputter_l_->output_scope (header_l_, "mudela");

  outputter_l_->output_comment (_ ("Outputting Score, defined at: "));
  outputter_l_->output_comment (origin_str_);

  if (paper_l_->scope_p_)
    outputter_l_->output_scope (paper_l_->scope_p_, "mudelapaper");

  SCM scm;
  if(experimental_features_global_b)
    {
      SCM scm = gh_list (ly_symbol2scm ("experimental-on"), SCM_UNDEFINED);
      outputter_l_->output_scheme (scm);
    }
  scm = gh_list (ly_symbol2scm ("header-end"), SCM_UNDEFINED);
  outputter_l_->output_scheme (scm);
  
  line_l_->output_lines ();


  scm = gh_list (ly_symbol2scm ("end-output"), SCM_UNDEFINED);
  outputter_l_->output_scheme (scm);


  // huh?
  delete outputter_l_;
  outputter_l_ = 0;
}
