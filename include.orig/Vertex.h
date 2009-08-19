#ifndef _Vertex_h
#define _Vertex_h

/////////////////////////////////////////////
/////////   PGTRACK VERSION     /////////////
/////////////////////////////////////////////

#include <vector>
#include "TObject.h"
#include "TkDBc.h"
#include "point.h"
#include "TrTrack.h"
#include "VCon.h"


//! Class that implements a vertex made of two tracks
class VertexR: public TrElem{

protected:

// VertexR Protected Data

  number _Momentum;     ///< Total Momentum attached to the vertex (GeV)
  number _ErrMomentum;  ///< Error in 1 / Momentum Sum (1/GeV)
  number _Theta;        ///< Theta (rad)
  number _Phi;          ///< Phi  (rad)
  number _Mass;         ///< Rec mass at vertex (GeV)
  integer _Charge;      ///< Charge at vertex
  number _Chi2;         ///< Chi2
  integer _Ndof;        ///< Number of degrees of freedom
  AMSPoint _Vertex;     ///< reconstructed vertex (cm)
  int Status;          ///< Status word

  /// Vector holding the index to tracks connected to the vertex
  vector <int>          _fTrTrack;
  /// Vector holding the pointer to tracks connected to the vertex
  vector<TrTrackR *> _Ptrack; //!
  /// 
  int _filled;
  
  /// Virtual container
  static VCon* vcon;

  /// Build index vector (_fTrTrack) from hits vector (_PTrack)
  void BuildTracksIndex();
  void _PrepareOutput(int full=0);
public:

  // VertexR Public Creators, Destructor, Operators
  ///dummy constructor
  VertexR(){Clear();}
  /// Explicit constructor it builds up a vertex
  VertexR(int ntracks, TrTrackR *ptrack[]);
  ~VertexR(){Clear(); }
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
  TrTrackR * pTrTrack(unsigned int i) ;

  // VertexR Get data
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


  uinteger checkstatus(integer checker) const{return Status & checker;}
  uinteger getstatus() const{return Status;}
  void     setstatus(uinteger status){Status=Status | status;}
  void     clearstatus(uinteger status){Status=Status & ~status;} 


//################ PRINTOUT  ########################################

  /// Print Vertex basic information
  std::ostream& putout(std::ostream &ostr = std::cout);
  /// ostream operator
  friend std::ostream &operator << 
    (std::ostream &ostr,  VertexR &cls) { 
    return cls.putout(ostr); 
  }

  /// Print cluster strip variables (printopt>0 --> verbose)
  void Print(int printopt =0);
  /// Return a string with some info (used for event display)
  char* Info(int iRef);


  void set_vertex();

 
  // Friends
  friend class AMSTrTrack;

  ClassDef(VertexR,1);

};
// End VertexR CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#endif
