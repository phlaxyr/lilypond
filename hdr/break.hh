/*
  break.hh -- part of LilyPond

  (c) 1996 Han-Wen Nienhuys
*/

#ifndef BREAK_HH
#define BREAK_HH
#include "vray.hh"
#include "proto.hh"
typedef svec<PCol*>  Line_of_cols;

struct Col_configuration {
    Line_of_cols cols;
    svec<Real> config;
    Real energy;

    /****************/
    void OK()const;
    void setsol(svec<Real>);
    Col_configuration() ;
    void add( PCol*c);
    void print() const;
};

struct Break_algorithm {
    PScore &pscore_;
    Real linelength;

    /****************/

    Break_algorithm(PScore&);
    /// check if the spacing/breaking problem is well-stated
    void problem_OK()const;
    /// search all pcols which are breakable.
    svec<PCol *> find_breaks() const;

     /// helper: solve for the columns in #curline#.
    svec<Real> solve_line(Line_of_cols) const;

    
    /// does curline fit on the paper?    
    bool feasible(Line_of_cols)const;
    
    virtual svec<Col_configuration> solve()=0;
};

/// wordwrap type algorithm: move to next line if current is optimal.
struct Word_wrap : Break_algorithm {
    virtual svec<Col_configuration> solve();
    Word_wrap(PScore&);
};
#endif // BREAK_HH

