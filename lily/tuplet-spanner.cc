/*
  plet-spanner.cc -- implement Tuplet_spanner

  source file of the GNU LilyPond music typesetter

  (c)  1997--2000 Jan Nieuwenhuizen <janneke@gnu.org>
*/


#include "beam.hh"
#include "box.hh"
#include "debug.hh"
#include "lookup.hh"
#include "molecule.hh"
#include "paper-column.hh"
#include "paper-def.hh"
#include "tuplet-spanner.hh"
#include "stem.hh"
#include "note-column.hh"
#include "dimensions.hh"
#include "group-interface.hh"
#include "directional-element-interface.hh"


Tuplet_spanner::Tuplet_spanner ()
{
  set_elt_pointer ("beams", SCM_EOL);
  set_elt_pointer ("columns", SCM_EOL);

  // ugh.
  set_elt_property ("delta-y", gh_int2scm (0));
}

/*
  TODO. 
 */
Molecule 
Tuplet_spanner::do_brew_molecule () const
{
  Molecule  mol;

  // Default behaviour: number always, bracket when no beam!
  bool par_beam = to_boolean (get_elt_property ("parallel-beam"));
  bool bracket_visibility = !par_beam;
  bool number_visibility = true;

  SCM bracket = get_elt_property ("tuplet-bracket-visibility");
  if (gh_boolean_p (bracket))
    {
      bracket_visibility = gh_scm2bool (bracket);
    }
  else if (bracket == ly_symbol2scm ("if-no-beam"))
    bracket_visibility = !par_beam;

  SCM numb = get_elt_property ("tuplet-number-visibility");  
  if (gh_boolean_p (numb))
    {
      number_visibility = gh_scm2bool (numb);
    }
  else if (bracket == ly_symbol2scm ("if-no-beam"))
    number_visibility = !par_beam;
  
  if (gh_pair_p (get_elt_pointer ("columns")))
    {
      Link_array<Note_column> column_arr=
	Pointer_group_interface__extract_elements (this, (Note_column*)0, "columns");
	
      Real ncw = column_arr.top ()->extent(X_AXIS).length ();
      Real w = spanner_length () + ncw;


      Real staff_space = paper_l ()->get_var ("interline");
      Direction dir = directional_element (this).get ();
      Real dy = gh_scm2double (get_elt_property ("delta-y"));
      SCM number = get_elt_property ("text");
      if (gh_string_p (number) && number_visibility)
	{
	  Molecule
	    num (lookup_l ()->text ("italic",
				    ly_scm2string (number), paper_l ()));
	  num.align_to (X_AXIS, CENTER);
	  num.translate_axis (w/2, X_AXIS);
	  num.align_to (Y_AXIS, CENTER);
	  num.translate_axis (dir * staff_space, Y_AXIS);
	
	  num.translate_axis (dy/2, Y_AXIS);

	  mol.add_molecule (num);
	}
      
      Real thick = paper_l ()->get_var ("tuplet_thick");
      if (bracket_visibility)      
	{
	  Real gap = paper_l () -> get_var ("tuplet_spanner_gap");
	  Real height = staff_space;
	  SCM at =gh_list(ly_symbol2scm ("tuplet"),
				       gh_double2scm (height),
				       gh_double2scm (gap),
				       gh_double2scm (w),
				       gh_double2scm (dy),
				       gh_double2scm (thick),
				       gh_int2scm (dir),
				       SCM_UNDEFINED);

	  Box b;
	  mol.add_molecule (Molecule (b, at));
	}

      //mol.translate_axis (dir * staff_space, Y_AXIS);
    }
  return mol;
}
  
void
Tuplet_spanner::do_add_processing ()
{
  if (gh_pair_p (get_elt_pointer ("columns")))
    {
      Link_array<Note_column> column_arr=
	Pointer_group_interface__extract_elements (this, (Note_column*)0, "columns");
      
      set_bound (LEFT, column_arr[0]);
      set_bound (RIGHT, column_arr.top ());  
    }
}



/*
  use first -> last note for slope, and then correct for disturbing
  notes in between.  */
void
Tuplet_spanner::calc_position_and_height (Real *offset, Real * dy) const
{
  Link_array<Note_column> column_arr=
    Pointer_group_interface__extract_elements (this, (Note_column*)0, "columns");


  Score_element * common = common_refpoint (get_elt_pointer ("columns"), Y_AXIS);
  
  Direction d = directional_element (this).get ();

  /*
    Use outer non-rest columns to determine slope
   */
  int l = 0;
  while (l <column_arr.size() && column_arr[l]->rest_b())
    l ++;

  int r = column_arr.size ()- 1;
  while (r >= l && column_arr[r]->rest_b())
    r--;
  
  if (l < r)
    {
      *dy = column_arr[r]->extent (Y_AXIS) [d] + column_arr[r]->relative_coordinate (common, Y_AXIS)
	- column_arr[l]->extent (Y_AXIS) [d] - column_arr[l]->relative_coordinate (common, Y_AXIS);
    }
  else
    * dy = 0;


  *offset = - d * infinity_f;

  if (!column_arr.size ())
    return;
  
  Real x0 = column_arr[0]->relative_coordinate (0, X_AXIS);
  Real x1 = column_arr.top ()->relative_coordinate (0, X_AXIS);
  
  Real factor = column_arr.size () > 1 ? 1/(x1 - x0) : 1.0;
  
  for (int i = 0; i < column_arr.size ();  i++)
    {
      Real notey = column_arr[i]->extent (Y_AXIS)[d] +
	column_arr[i]->relative_coordinate (common, Y_AXIS)
	;
      Real x = column_arr[i]->relative_coordinate (0, X_AXIS) - x0;
      Real tuplety =  *dy * x * factor;

      if (notey * d > (*offset + tuplety) * d)
	*offset = notey - tuplety; 
    }
}

/*
  use first -> last note for slope,
*/
void
Tuplet_spanner::calc_dy (Real * dy) const
{
  Link_array<Note_column> column_arr=
    Pointer_group_interface__extract_elements (this, (Note_column*)0, "columns");

 
  Direction d = directional_element (this).get ();
  *dy = column_arr.top ()->extent (Y_AXIS) [d]
    - column_arr[0]->extent (Y_AXIS) [d];
}

void
Tuplet_spanner::after_line_breaking ()
{
  Link_array<Note_column> column_arr=
    Pointer_group_interface__extract_elements (this, (Note_column*)0, "columns");

  if (!column_arr.size ())
    {
      set_elt_property ("transparent", SCM_BOOL_T);
      set_extent_callback (0, X_AXIS);
      set_extent_callback (0, Y_AXIS);
    }

  Direction d = directional_element (this).get ();
  if (!d)
    {
      d = get_default_dir ();
      directional_element (this).set (d);

    }
  Real dy, offset;

  calc_position_and_height  (&offset,&dy);
  
  set_elt_property ("delta-y", gh_double2scm (dy));

  translate_axis (offset, Y_AXIS);
  
  if (scm_ilength (get_elt_pointer ("beams")) == 1)
    {
      SCM bs = get_elt_pointer ("beams");
      Score_element *b = unsmob_element (gh_car (bs));
      Beam * beam_l = dynamic_cast<Beam*> (b);
      if (!broken_b () 
	  && get_bound (LEFT)->column_l () == beam_l->get_bound (LEFT)->column_l ()
	  && get_bound (RIGHT)->column_l () == beam_l->get_bound (RIGHT)->column_l ())
	set_elt_property ("parallel-beam", SCM_BOOL_T);
    }
}


Direction
Tuplet_spanner::get_default_dir () const
{
  Direction d = UP;
  SCM dir_sym =get_elt_property ("dir-forced");
  if (isdir_b (dir_sym))
    {
      d= to_dir (dir_sym);
      if (d != CENTER)
	return d;
    }

  d = UP ;
  for (SCM s = get_elt_pointer ("columns"); gh_pair_p (s); s = gh_cdr (s))
    {
      Score_element * sc = unsmob_element (gh_car (s));
      Note_column * nc = dynamic_cast<Note_column*> (sc);
      if (nc->dir () < 0) 
	{
	  d = DOWN;
	  break;
	}
    }
  
  return d;
}

void
Tuplet_spanner::add_beam (Beam *b)
{
  add_dependency (b);
  Pointer_group_interface gi (this, "beams");
  gi.add_element (b);
}

void
Tuplet_spanner::add_column (Note_column*n)
{
  Pointer_group_interface gi (this, "columns");
  gi.add_element (n);

  add_dependency (n);
}


