/*   
  simple-spacer.cc -- implement Simple_spacer
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>

  TODO:
  - add support for different stretch/shrink constants?
  
*/
#include <stdio.h>
#include <math.h>
#include <libc-extension.hh>	// isinf

#include "simple-spacer.hh"
#include "paper-column.hh"
#include "spring.hh"
#include "rod.hh"
#include "warn.hh"
#include "column-x-positions.hh"
#include "spaceable-grob.hh"
#include "dimensions.hh"


/*
   A simple spacing constraint solver. The approach:

   Stretch the line uniformly until none of the constraints (rods)
   block.  It then is very wide.

      Compress until the next constraint blocks,

      Mark the springs over the constrained part to be non-active.
      
   Repeat with the smaller set of non-active constraints, until all
   constraints blocked, or until the line is as short as desired.

   This is much simpler, and much much faster than full scale
   Constrained QP. On the other hand, a situation like this will not
   be typeset as dense as possible, because

   c4                   c4           c4                  c4
   veryveryverylongsyllable2         veryveryverylongsyllable2
   " "4                 veryveryverylongsyllable2        syllable4


   can be further compressed to


   c4    c4                        c4   c4
   veryveryverylongsyllable2       veryveryverylongsyllable2
   " "4  veryveryverylongsyllable2      syllable4


   Perhaps this is not a bad thing, because the 1st looks better anyway.  */


Simple_spacer::Simple_spacer ()
{
  /*
    Give an extra penalty for compression. Needed to avoid compressing
    tightly spaced lines.
  */
  compression_penalty_b_ = false;
  active_count_ = 0;
  force_ = 0.;
  indent_ =0.0;
  default_space_ = 20 PT;
}

void
Simple_spacer::add_rod (int l, int r, Real dist)
{
  if (isinf (dist) || isnan (dist))
    {
      programming_error ("Weird minimum distance. Ignoring");
      return;
    }

  Real c = range_stiffness (l,r);
  if (isinf (c))
    {
      /*
	If a spring is fixed, we have to do something here:
	we let the rod override the spring. 
       */
      Real total_dist = 0.;
      for (int i = l ; i < r; i++)
	total_dist += springs_[i].ideal_;

      if (total_dist < dist)
	for (int i = l ; i < r; i++)
	  springs_[i].ideal_ *= dist/total_dist;

      return;
    }
  
  Real d = range_ideal_len (l,r);
  Real block_stretch = dist - d;
  
  Real block_force = c * block_stretch;
  force_ = force_ >? block_force;

  for (int i=l; i < r; i++)
    springs_[i].block_force_ = block_force >?
      springs_[i].block_force_ ;
}

Real
Simple_spacer::range_ideal_len (int l, int r)   const
{
  Real d =0.;
  for (int i=l; i < r; i++)
    d += springs_[i].ideal_;
  return d;
}

Real
Simple_spacer::range_stiffness (int l, int r) const
{
  Real den =0.0;
  for (int i=l; i < r; i++)
    {
      if (springs_[i].is_active_)
	den += 1 / springs_[i].hooke_;
    }

  return 1 / den;
}

Real
Simple_spacer::is_activelocking_force () const
{
  Real bf = - infinity_f; 
  for (int i=0; i < springs_.size (); i++)
    if (springs_[i].is_active_)
      {
	bf = bf >? springs_[i].block_force_;
      }
  return bf;
}

Real
Simple_spacer::active_springs_stiffness () const
{
  return range_stiffness (0, springs_.size ());
}

void
Simple_spacer::set_active_states ()
{
  /* float comparison is safe, since force is only copied.  */
  for (int i=0 ; i <springs_.size (); i++)
    if (springs_[i].is_active_
	&& springs_[i].block_force_ >= force_)
      {
	springs_[i].is_active_ = false;
	active_count_ --; 
      }
}   

Real
Simple_spacer::configuration_length () const
{
  Real l =0.;
  for (int i=0; i < springs_.size (); i++)
    l += springs_[i].length (force_);

  return l;
}

bool
Simple_spacer::is_active () const
{
  return active_count_; 
}

void
Simple_spacer::my_solve_linelen ()
{
  while (is_active ())
    {
      force_ = is_activelocking_force ();
      Real conf = configuration_length ();

      if (conf < line_len_)
	{
	  force_ += (line_len_  - conf) * active_springs_stiffness ();
	  break;
	}
      else
	set_active_states ();
    }
}


void
Simple_spacer::my_solve_natural_len ()
{
  while (is_active ())
    {
      force_ = is_activelocking_force () >? 0.0;

      if (force_ < 1e-8) // ugh.,
	break;
      
      set_active_states ();
    }
}

void
Simple_spacer::add_columns (Link_array<Grob> const &icols)
{
  Link_array<Grob> cols (icols);
  
  for (int i =  cols.size (); i--;)
    if (ly_c_pair_p (cols[i]->get_property ("between-cols")))
      {
	loose_cols_.push (cols[i]);
	cols.del (i);
      }
  
  spaced_cols_ = cols;
  for (int i=0; i < cols.size () - 1; i++)
    {
      Spring_smob *spring = 0;

      for (SCM s = cols[i]->get_property ("ideal-distances");
	   !spring && ly_c_pair_p (s);
	   s = ly_cdr (s))
	{
	  Spring_smob *sp = unsmob_spring (ly_car (s));
	  
	  
	  if (sp->other_ == cols[i+1])
	    spring = sp;
	}

      Spring_description desc;
      if (spring)
	{
	  desc.ideal_ = spring->distance_;
	  desc.hooke_ = spring->strength_;
	}
      else
	{
	  programming_error (_f ("No spring between column %d and next one",
				Paper_column::get_rank (cols[i])
				));
	  desc.hooke_ = 1.0;
	  desc.ideal_ = default_space_;

	  continue;
	}

      if (!desc.is_sane ())
	{
	  programming_error ("Insane spring found. Setting to unit spring.");

	  desc.hooke_ = 1.0;
	  desc.ideal_ = 1.0;
	}

      if (isinf (desc.hooke_))
	{
	  desc.is_active_ = false;
	  springs_.push (desc);
	}
      else
	{
	  desc.block_force_ = - desc.hooke_ * desc.ideal_; // block at distance 0
	  springs_.push (desc);
      
	  active_count_ ++;
	}

      if (spring->expand_only_b_)
	{
	  compression_penalty_b_ = true;
	}
      
    }
  
  for (int i=0; i < cols.size () - 1; i++)
    {
      for (SCM s = Spaceable_grob::get_minimum_distances (cols[i]);
	   ly_c_pair_p (s); s = ly_cdr (s))
	{
	  Grob * other = unsmob_grob (ly_caar (s));
	  int oi = cols.find_index (other);
	  if (oi >= 0)
	    {
	      add_rod (i, oi, ly_scm2double (ly_cdar (s)));
	    }
	}
    }
}

/*
  
  TODO: should a add penalty for widely varying spring forces (caused
  by constraints, eg.


         =====  
         |   |
  o|o|  x ##x


  The ## forces the notes apart; we shouldn't allow the O's to touch
  this closely.
  
 */
void
Simple_spacer::solve (Column_x_positions *positions, bool ragged) 
{
  if (ragged)
    my_solve_natural_len ();
  else
    my_solve_linelen ();

  positions->force_ = force_;
  
  /*
    We used to have a penalty for compression, no matter what, but that
    fucked up wtk1-fugue2 (taking 3 full pages.)
  */
  positions->config_.push (indent_);
  for (int i=0; i <springs_.size (); i++)
    {
      Real  l = springs_[i].length ((ragged) ? 0.0 : force_);
      positions->config_.push (positions->config_.top () + l);
      /*
	we have l>= 0 here, up to rounding errors 
      */
    }

  /*
    For raggedright, we must have a measure of music density: this is
    to prevent lots of short lines (which all have force = 0).
    */
  if (ragged)
    {
      Real len = positions->config_.top ();
      if (line_len_ - len  >= 0)
	positions->force_ = ((line_len_ - len) *  active_springs_stiffness ());
      else
	{
	  positions->force_ = 0.0;
	  /*
	    Don't go past end-of-line in ragged right.
	   */
	  positions->satisfies_constraints_ = false;
	}
    }


  positions->cols_ = spaced_cols_;
  positions->loose_cols_ = loose_cols_;
  positions->satisfies_constraints_ =
    positions->satisfies_constraints_ && is_active ();

  /*
    Check if breaking constraints are met.
   */
  bool break_satisfy = true;
  int sz =  positions->cols_.size ();
  for (int i = sz; i--; )
    {
      SCM p = positions->cols_[i]->get_property ( "penalty");
      if (ly_c_number_p (p))
	{
	  if (ly_scm2double (p) < -9999)
	    break_satisfy = break_satisfy && (i == 0 || i == sz -1);
	  if (ly_scm2double (p) > 9999)
	    break_satisfy = break_satisfy && !(i == 0 || i == sz -1);
	}
      
    }

  positions->satisfies_constraints_ =
    positions->satisfies_constraints_ && break_satisfy;
}

/****************************************************************/

Spring_description::Spring_description ()
{
  ideal_ =0.0;
  hooke_ =0.0;
  is_active_ = true;
  block_force_ = 0.0;
}


bool
Spring_description::is_sane () const
{
  return (hooke_ > 0) &&  !isinf (ideal_) && !isnan (ideal_);
}

Real
Spring_description::length (Real f) const
{
  if (!is_active_)
    f = block_force_;
  return ideal_ + f / hooke_ ;
}
