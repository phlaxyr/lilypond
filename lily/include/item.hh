/*
  item.hh -- declare Item

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/
#ifndef ITEM_HH
#define ITEM_HH


#include "boxes.hh"
#include "string.hh"
#include "score-elem.hh"

/**
  A horizontally fixed size element of the score.

  Item is the datastructure for printables whose width is known
  before the spacing is calculated

  NB. This doesn't mean an Item has to initialize the output field before
  spacing calculation. 
  
*/
class Item : public virtual Score_elem {
public:
    /// indirection to the column it is in
    PCol * pcol_l_;
    
    Item * broken_to_a_[2];

    /// should be put in a breakable col.
    bool breakable_b_;
    int break_status_i_;
    /// nobreak = 0, pre = -1, post = 1
    int break_status_i()const;
    Item * find_prebroken_piece(PCol*)const;
    Item * find_prebroken_piece(Line_of_score*)const;    

    virtual Item *item() { return this; }
    Item();
    Real hpos_f() const;
    DECLARE_MY_RUNTIME_TYPEINFO;
    virtual Line_of_score * line_l() const;
    
protected:
    virtual void  do_breakable_col_processing();
    virtual void handle_prebroken_dependencies();
    virtual void do_print()const;

    void copy_breakable_items();
};



#endif
