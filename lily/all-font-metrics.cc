/*   
  all-font-metrics.cc --  implement All_font_metrics
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999--2000 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "main.hh"
#include "all-font-metrics.hh"
#include "debug.hh"
#include "warn.hh"
#include "afm.hh"
#include "tfm.hh"
#include "lily-guile.hh"
#include "tfm-reader.hh"

const char * default_font_sz_ = "cmr10";

All_font_metrics::All_font_metrics (String path)
{
  search_path_.parse_path (path);
}

Adobe_font_metric *
All_font_metrics::find_afm (String name)
{
  SCM sname = ly_symbol2scm (name.ch_C ());
  if (!afm_p_dict_.elem_b (sname))
    {
      String path = name  + ".afm";
      path = search_path_.find (path);
      if (path.empty_b ())
	return 0;
      progress_indication ("[" + path);
      Adobe_font_metric * afm_p = read_afm_file (path);

      afm_p->name_ = ly_symbol2scm (name.ch_C ());
      progress_indication ("]");

      afm_p_dict_.set (sname,afm_p->self_scm_);
    }
  
  return dynamic_cast<Adobe_font_metric*> (unsmob_metrics (afm_p_dict_.get (sname)));
}

Scaled_font_metric * 
All_font_metrics::find_scaled (String nm, int m)
{
  Scaled_font_metric * s=0;
  String index =  nm + "@" + to_str (m);
  SCM sname = ly_symbol2scm (index.ch_C ());

  Font_metric *fm =0;
  if (!scaled_p_dict_.elem_b (sname))
    {
      Font_metric *f = find_font (nm);
      s = new Scaled_font_metric (f, m);
      scaled_p_dict_.set (sname, s->self_scm_);
      fm =  s;
    }
  else
    fm = unsmob_metrics (scaled_p_dict_.get (sname));

  return dynamic_cast<Scaled_font_metric*> (fm);
}

Tex_font_metric *
All_font_metrics::find_tfm (String name)
{
  SCM sname = ly_symbol2scm (name.ch_C ());  
  if (!tfm_p_dict_.elem_b (sname))
    {
      String path = name  + ".tfm";
      path = search_path_.find (path);
      if (path.empty_b ())
	return 0;
      progress_indication ("[" + path);
      Tex_font_metric	* tfm_p = Tex_font_metric_reader::read_file (path);
      tfm_p->name_ = ly_symbol2scm (name.ch_C( ));
      progress_indication ("]");

      tfm_p_dict_.set (sname, tfm_p->self_scm_);
    }
    
  return
    dynamic_cast<Tex_font_metric*> (unsmob_metrics (tfm_p_dict_.get(sname)));
}


Font_metric *
All_font_metrics::find_font (String name)
{
  Font_metric * f=0;
  f = find_tfm (name);
  if (f)
    return f;

  f= find_afm (name);
  if (f)
    return f;

  warning (_f ("can't find font: `%s'", name.ch_C ()));
  warning (_ ("Loading default font"));
  
  String def_name = default_font_sz_;
  SCM l = scm_assoc (ly_str02scm ("default"),
		     scm_eval (ly_symbol2scm ("cmr-alist")));
  
  if (l != SCM_BOOL_F)
    def_name = ly_scm2string (gh_cdr (l));

  f =  find_tfm (def_name);
  if (f)
    return f;

  f= find_afm (def_name);
  if (f)
    return f;

  non_fatal_error (_f ("can't find default font: `%s'", def_name.ch_C ()));
  non_fatal_error (_f ("(search path: `%s')", search_path_.str ()));
  error (_ ("Giving up"));

  return 0;
}

SCM
All_font_metrics::font_descriptions () const
{
  SCM l[] = {0,0,0};

  l[0] = afm_p_dict_.to_alist ();
  l[1] = tfm_p_dict_.to_alist ();
  l[2] = scaled_p_dict_.to_alist ();  

  SCM list = SCM_EOL;
  for (int i=0; i < 3; i++)
    {
      for (SCM s = l[i];  gh_pair_p (s); s = gh_cdr (s))
	{
	  Font_metric * fm = unsmob_metrics (gh_cdar (s));

	  list = gh_cons (fm->description (), list);
	}
    }
  return list;
}



Font_metric*
find_font (String name)
{
  return   all_fonts_global_p->find_font (name);
}
