/*
  paper-outputter.cc -- implement Paper_outputter

  source file of the GNU LilyPond music typesetter

  (c)  1997--2000 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include <time.h>
#include <fstream.h>
#include <math.h>
#include <iostream.h>

#include "dimensions.hh"
#include "dictionary-iter.hh"
#include "virtual-methods.hh"
#include "paper-outputter.hh"
#include "paper-stream.hh"
#include "molecule.hh"
#include "array.hh"
#include "string-convert.hh"
#include "debug.hh"
#include "lookup.hh"
#include "main.hh"
#include "scope.hh"
#include "identifier.hh"
#include "lily-version.hh"


/*
  Ugh, this is messy.
 */

Paper_outputter::Paper_outputter (Paper_stream  * ps )
{
#if 0 
  molecules_ = gh_cons (SCM_EOL, SCM_EOL);
  last_cons_ = molecules_;
#endif

 /*
   lilypond -f scm x.ly
   guile -s x.scm
  */
  verbatim_scheme_b_ =  output_global_ch == String ("scm");

  if (verbatim_scheme_b_)
    {
	*ps << ""
	  ";;; Usage: guile -s x.scm > x.tex\n"
	  "(primitive-load-path 'lily.scm)\n"
	  "(scm-tex-output)\n"
	  ";(scm-ps-output)\n"
	  "(map (lambda (x) (display (eval x))) '(\n"
	;
    }

  stream_p_ = ps;
}

Paper_outputter::~Paper_outputter ()
{
  if (verbatim_scheme_b_)
    {
      *stream_p_ << "))";
    }
  delete stream_p_;
}


void
Paper_outputter::output_header ()
{
  if (safe_global_b)
    {
      gh_define ("security-paranoia", SCM_BOOL_T);      
    }

  SCM exp = gh_list (ly_symbol2scm ((String (output_global_ch) + "-scm").ch_C()),
		     ly_quote_scm (ly_symbol2scm ("all-definitions")),
		     SCM_UNDEFINED);
  exp = scm_eval (exp);
  scm_eval (exp);
  
  String creator;
  if (no_timestamps_global_b)
    creator = gnu_lilypond_str ();
  else
    creator = gnu_lilypond_version_str ();
  
  String generate;
  if (no_timestamps_global_b)
    generate = ".";
  else
    {
      generate = _ (", at ");
      time_t t (time (0));
      generate += ctime (&t);
      generate = generate.left_str (generate.length_i () - 1);
    }

  SCM args_scm = 
    gh_list (ly_str02scm (creator.ch_l ()),
	     ly_str02scm (generate.ch_l ()), SCM_UNDEFINED);


  SCM scm = gh_cons (ly_symbol2scm ("header"), args_scm);
  output_scheme (scm);
}



void
Paper_outputter::output_comment (String str)
{
  output_scheme (gh_list (ly_symbol2scm ("comment"),
			  ly_str02scm ((char*)str.ch_C()),
			  SCM_UNDEFINED)
		 );
}


void
Paper_outputter::output_scheme (SCM scm)
{
#if 0
  SCM c = gh_cons (scm,gh_cdr (last_cons_));
  gh_set_cdr_x(last_cons_, c);
  last_cons_ = c;
#endif

  dump_scheme (scm);
}


#if 0
void
Paper_outputter::dump ()
{

  for (SCM s = gh_cdr (molecules_); gh_pair_p (s); s = gh_cdr (s))
    {
      dump_scheme (gh_car (s));
    }
}
#endif

void
Paper_outputter::dump_scheme (SCM s)
{
  if  (verbatim_scheme_b_)
    {
      SCM result =  scm_eval (scm_listify (ly_symbol2scm ("scm->string"),
					   ly_quote_scm (gh_car (s)), SCM_UNDEFINED));
	  
      *stream_p_ << ly_scm2string (result);
    }
  else
    {
      SCM result = scm_eval (s);
      char *c=gh_scm2newstr (result, NULL);
  
      *stream_p_ << c;
      free (c);
    }
}
void
Paper_outputter::output_scope (Scope *scope, String prefix)
{
  for (Scope_iter i (*scope); i.ok (); i++)
    {
      if (dynamic_cast<String_identifier*> (i.val ()))
	{
	  String val = *i.val()->access_content_String (false);

	  output_String_def (prefix + i.key (), val);
	}
      else if(dynamic_cast<Real_identifier*> (i.val ()))
	{
	  Real val  = *i.val ()->access_content_Real (false);

	  output_Real_def (prefix + i.key (), val);	  
	}
      else if (dynamic_cast<int_identifier*> (i.val ()))
	{
	  int val  = *i.val ()->access_content_int (false);	  
	  
	  output_int_def (prefix + i.key (), val);	  
	}
    }
}

void
Paper_outputter::output_version ()
{
  String id_str = "Lily was here";
  if (no_timestamps_global_b)
    id_str += ".";
  else
    id_str += String (", ") + version_str ();

  output_String_def ( "mudelatagline", id_str);
  output_String_def ( "LilyPondVersion", version_str ());
}



void
Paper_outputter::output_font_def (int i, String str)
{
  SCM scm = gh_list (ly_symbol2scm ("font-def"),
		     gh_int2scm (i),
		     ly_str02scm (str.ch_l ()),
		     SCM_UNDEFINED);

  output_scheme (scm);
}

void
Paper_outputter::output_Real_def (String k, Real v)
{
  
  SCM scm = gh_list (ly_symbol2scm ("lily-def"),
		     ly_str02scm (k.ch_l ()),
		     ly_str02scm (to_str(v).ch_l ()),
		     SCM_UNDEFINED);
  output_scheme (scm);
}

void
Paper_outputter::output_String_def (String k, String v)
{
  
  SCM scm = gh_list (ly_symbol2scm ("lily-def"),
		     ly_str02scm (k.ch_l ()),
		     ly_str02scm (v.ch_l ()),
		     SCM_UNDEFINED);
  output_scheme (scm);
}

void
Paper_outputter::output_int_def (String k, int v)
{
  SCM scm = gh_list (ly_symbol2scm ("lily-def"),
		     ly_str02scm (k.ch_l ()),
		     ly_str02scm (to_str (v).ch_l ()),
		     SCM_UNDEFINED);
  output_scheme (scm);
}





