/*
  pitched-trill-engraver.cc -- implement Pitched_trill_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"

#include "dots.hh"
#include "pointer-group-interface.hh"
#include "axis-group-interface.hh"
#include "context.hh"
#include "note-head.hh"
#include "item.hh"
#include "side-position-interface.hh"
#include "pitch.hh"
#include "warn.hh"

class Pitched_trill_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Pitched_trill_engraver);

protected:
  DECLARE_ACKNOWLEDGER (note_head);
  DECLARE_ACKNOWLEDGER (dots);
  DECLARE_ACKNOWLEDGER (text_spanner);
  void process_music ();
  void stop_translation_timestep ();

private:
  Item *trill_head_;
  Item *trill_group_;
  Item *trill_accidental_;

  Link_array__Grob_ heads_;

  void make_trill (Music *);
};

Pitched_trill_engraver::Pitched_trill_engraver ()
{
  trill_head_ = 0;
  trill_group_ = 0;
  trill_accidental_ = 0;
}

void
Pitched_trill_engraver::acknowledge_dots (Grob_info info)
{
  heads_.push_back (info.grob ());
}
void
Pitched_trill_engraver::acknowledge_note_head (Grob_info info)
{
  heads_.push_back (info.grob ());
}

void
Pitched_trill_engraver::acknowledge_text_spanner (Grob_info info)
{
  Music *mus = info.music_cause ();
  if (mus
      && mus->is_mus_type ("trill-span-event")
      && to_dir (mus->get_property ("span-direction")) == START
      && unsmob_pitch (mus->get_property ("pitch")))
    make_trill (mus);
}

void
Pitched_trill_engraver::make_trill (Music *mus)
{
  SCM scm_pitch = mus->get_property ("pitch");
  Pitch *p = unsmob_pitch (scm_pitch);

  SCM keysig = get_property ("localKeySignature");

  SCM key = scm_cons (scm_from_int (p->get_octave ()),
		      scm_from_int (p->get_notename ()));

  SCM handle = scm_assoc (key, keysig);
  bool print_acc
    = (handle == SCM_BOOL_F)
    || p->get_alteration () == 0;

  if (trill_head_)
    {
      programming_error ("already have a trill head.");
      trill_head_ = 0;
    }

  trill_head_ = make_item ("TrillPitchHead", mus->self_scm ());
  SCM c0scm = get_property ("middleCPosition");

  int c0 = scm_is_number (c0scm) ? scm_to_int (c0scm) : 0;

  trill_head_->set_property ("staff-position",
			     scm_from_int (unsmob_pitch (scm_pitch)->steps ()
					   + c0));

  trill_group_ = make_item ("TrillPitchGroup", mus->self_scm ());

  Axis_group_interface::add_element (trill_group_, trill_head_);

  if (print_acc)
    {
      trill_accidental_ = make_item ("TrillPitchAccidental", mus->self_scm ());

      // fixme: naming -> alterations
      trill_accidental_->set_property ("accidentals", scm_list_1 (scm_from_int (p->get_alteration ())));
      Side_position_interface::add_support (trill_accidental_, trill_head_);
      trill_head_->set_object ("accidental-grob", trill_accidental_->self_scm ());
      trill_group_->set_parent (trill_head_, Y_AXIS);
      Axis_group_interface::add_element (trill_group_, trill_accidental_);
      trill_accidental_->set_parent (trill_head_, Y_AXIS);
    }
}

void
Pitched_trill_engraver::stop_translation_timestep ()
{
  if (trill_group_)
    for (vsize i = 0; i < heads_.size (); i++)
      Side_position_interface::add_support (trill_group_, heads_[i]);

  heads_.clear ();
  trill_head_ = 0;
  trill_group_ = 0;
  trill_accidental_ = 0;
}

void
Pitched_trill_engraver::process_music ()
{
}


#include "translator.icc"

ADD_ACKNOWLEDGER (Pitched_trill_engraver, note_head);
ADD_ACKNOWLEDGER (Pitched_trill_engraver, dots);
ADD_ACKNOWLEDGER (Pitched_trill_engraver, text_spanner);

ADD_TRANSLATOR (Pitched_trill_engraver,
		/* doc */ "Print the bracketed notehead after a notehead with trill.",
		/* create */
		"TrillPitchHead "
		"TrillPitchAccidental "
		"TrillPitchGroup",
		/* accept */ "",
		/* read */ "",
		/* write */ "");
