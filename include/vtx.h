// Author J. Alcaraz June-2003
//
//
#ifndef __AMSVtx__
#define __AMSVtx__

#ifndef _PGTRACK_
#include "upool.h"
#include "apool.h"
#include "gsen.h"
#include "trid.h"
#include "link.h"
#include "commons.h"
#include "cont.h"
#include "event.h"
#include <vector>
#include "trrec.h"
#include "trdrec.h"
#include "point.h"

// Global namespace Vtxconst
namespace Vtxconst{
	  const integer maxtr=7;
}

// Begin AMSVtx CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class AMSVtx: public AMSlink{

protected:

// AMSVtx Protected Data
  number _Momentum;
  number _ErrMomentum;
  number _Theta;
  number _Phi;
  number _Mass;
  integer _Charge;
  number _Chi2;
  integer _Ndof;
  integer _Ntracks;
  AMSTrTrack * _Ptrack[Vtxconst::maxtr];
  AMSTRDTrack * _Ptrd;
  AMSPoint _Vertex;

// AMSVtx Protected Methods
  void _printEl(ostream & stream){ 
                     stream <<
                     " Vtx >>> Run " << AMSEvent::gethead()->getrun() <<
                     " Event " << AMSEvent::gethead()->getEvent() << endl; 
                     
                     stream << 
                     " Vtx >>> #tracks " << _Ntracks << 
                     " Momentum " << _Momentum << 
                     " ErrMomentum " << _ErrMomentum << 
                     " Charge " << _Charge << 
                     " Theta " << _Theta <<
                     " Phi " << _Phi <<
                     " Chi2 " << _Chi2 <<
                     " Ndof " << _Ndof << endl;

                     stream << 
                     " Vtx >>> Vextex Point: " << _Vertex[0] <<
                     "        " << _Vertex[1] <<
                     "        " << _Vertex[2] << endl; 

  }

  void _copyEl();
  void _writeEl();
  static integer Out(integer);

public:

// AMSVtx Public Creators, Destructor, Operators
  AMSVtx(): AMSlink(){};
  AMSVtx(int ntracks, AMSTrTrack *ptrack[]): AMSlink(), _Ntracks(ntracks) {
    for (int i=0; i<ntracks; i++) { _Ptrack[i]=ptrack[i]; }
  }
  ~AMSVtx(){};

// AMSVtx Public Methods
  AMSVtx * next(){return (AMSVtx*)_next;}
  static integer build(integer refit=0);
  static integer build_old(integer refit=0);
  void print(){_printEl(cout);}

// AMSVtx Set data
  integer set_all();
  void set_vertex();

// AMSVtx Get data
  number getmom() const {return _Momentum;}
  number geterrmom() const {return _ErrMomentum;}
  number gettheta() const {return _Theta;}
  number getphi() const {return _Phi;}
  number getmass() const {return _Mass;}
  integer getcharge() const {return _Charge;}
  number getchi2() const {return _Chi2;}
  integer getndof() const {return _Ndof;}
  integer getntracks() const {return _Ntracks;}
  AMSTrTrack * gettrack(integer i){return i>=0 && i<_Ntracks? _Ptrack[i]:0;}
  AMSTRDTrack * gettrd(){return _Ptrd;}
  AMSPoint getvert()const {return _Vertex;}

// Friends
  friend class AMSTrTrack;
  friend class AMSTRDTrack;

};
// End AMSVtx CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#else



/////////////////////////////////////////////
/////////   PGTRACK VERSION     /////////////
/////////////////////////////////////////////

#include <vector>
#include "TObject.h"
#include "TkDBc.h"
#include "link.h"
#include "point.h"
#include "TrTrack.h"
#include "VCon.h"

// Begin AMSVtx CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//! Class that implements a vertex made of two tracks
class AMSVtx: public AMSlink{

protected:

// AMSVtx Protected Data

  number _Momentum;     ///< Total Momentum attached to the vertex (GeV)
  number _ErrMomentum;  ///< Error in 1 / Momentum Sum (1/GeV)
  number _Theta;        ///< Theta (rad)
  number _Phi;          ///< Phi  (rad)
  number _Mass;         ///< Rec mass at vertex (GeV)
  integer _Charge;      ///< Charge at vertex
  number _Chi2;         ///< Chi2
  integer _Ndof;        ///< Number of degrees of freedom
  AMSPoint _Vertex;     ///< reconstructed vertex (cm)

  /// Vector holding the index to tracks connected to the vertex
  vector <int>          _fTrTrack;
  /// Vector holding the pointer to tracks connected to the vertex
  vector<AMSTrTrack *> _Ptrack; //!
  /// 
  int _filled;
  // AMSVtx Protected Methods
  void _printEl(ostream & stream); 
 

  void _copyEl(){}
  void _writeEl(){}

  /// Build index vector (_fTrTrack) from hits vector (_PTrack)
  void BuildTracksIndex();

public:

  // AMSVtx Public Creators, Destructor, Operators
  ///dummy constructor
  AMSVtx(): AMSlink(){Clear();}
  /// Explicit constructor it builds up a vertex
  AMSVtx(int ntracks, AMSTrTrack *ptrack[]);
  ~AMSVtx(){Clear(); }
  /// Clean up the class to "zero" default  
  void Clear();
  
  
  /// access function to TrTrackR objects used
  /// \return number of TrTrackR used
  int NTrTrack()const {return _Ptrack.size();}
   /// access function to TrTrackR objects used
  /// \param i index of fTrTrackR vector
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack(unsigned int i)const {return i<_fTrTrack.size()? _fTrTrack[i]:-1;}
  /// access function to TrTrackR objects   
  /// \param i index of fTrTrackR vector
  /// \return pointer to TrTrackR object  or 0
  AMSTrTrack * pTrTrack(unsigned int i) ;

  // AMSVtx Get data
  /// Return the Total Momentum (GeV)
  number        getmom()     const  {return _Momentum;}
  /// Return the Error in 1 / Momentum Sum (1/GeV)
  number        geterrmom()  const  {return _ErrMomentum;}
  /// Return theta (rad)
  number        gettheta()   const  {return _Theta;}
  /// Return phi (rad)
  number        getphi()     const  {return _Phi;}
  /// Return Rec mass at vertex (GeV)
  number        getmass()    const  {return _Mass;}
  /// Return Total Charge at vertex
  integer       getcharge()  const  {return _Charge;}
  /// Chi2
  number        getchi2()    const  {return _Chi2;}
  /// Number of degrees of freedom
  integer       getndof()    const  {return _Ndof;}
  /// Returns the Vertex Coo (cm) as AMSPoint
  AMSPoint      getvert()    const  {return _Vertex;}
  /// Return true if the vertex has been properly built 
  bool           IsFilled()   const  {return _filled!=0;}


  //OTHER Methos
  // AMSVtx Public Methods
  AMSVtx * next(){return (AMSVtx*)_next;}
  
  /// Print out to stdoutput some class info
  void print(){_printEl(cout);}

  void set_vertex();

   /// Virtual container
  static VCon* vcon;

// Friends
  friend class AMSTrTrack;

  ClassDef(AMSVtx,1);

};
// End AMSVtx CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef AMSVtx VertexR;

#endif


#endif
