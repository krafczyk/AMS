#ifndef _Vertex_h
#define _Vertex_h

/////////////////////////////////////////////
/////////   PGTRACK VERSION     /////////////
/////////////////////////////////////////////

#include <vector>
#include "TObject.h"
#include "point.h"
#include "TrTrack.h"


//! Class that implements a vertex made of two tracks
class VertexR: public TrElem{
public:
  int Status;          ///< Status word
  number Mass;         ///< Rec mass at vertex (GeV)
  number Momentum;     ///< Total Momentum attached to the vertex (GeV)
  number ErrMomentum;  ///< Error in 1 / Momentum Sum (1/GeV)
  number Theta;        ///< Theta (rad)
  number Phi;          ///< Phi  (rad)
  int16   fCharge;       ///< Charge at vertex
  uint16  fNdof;         ///< Number of degrees of freedom
  number Chi2;         ///< Chi2
  float  Vertex [3];   ///< Vertex position (cm)

  /// Vector holding the index to tracks connected to the vertex
  vector <int16>          fTrTrack;

  /// Build index vector (_fTrTrack) from hits vector (pTrTrack)
  void BuildTracksIndex();

  /// Build track pointer vector (_pTrTrack) from index vector
  void BuildTracksPointerVector();

protected:
  /// Vector holding the pointer to tracks connected to the vertex
  vector<TrTrackR *> _pTrTrack; //!
  /// 
  int8 _filled;
  std::string _PrepareOutput(int full=0);

public:

  // VertexR Public Creators, Destructor, Operators
  ///dummy constructor
  VertexR(){Clear();}
  /// Explicit constructor it builds up a vertex
  VertexR(int ntracks, TrTrackR *ptrack[],double beta=1);
  /// Explicit constructor with two tracks (assuming photon vertex)
  VertexR(TrTrackR *track1, TrTrackR *track2,double beta=1);

  ~VertexR(){Clear(); }
  /// Clean up the class to "zero" default  
  void Clear();
  
  
  /// access function to TrTrackR objects used
  /// \return number of TrTrackR used
  int NTrTrack() const { return fTrTrack.size(); }
   /// access function to TrTrackR objects used
  /// \param i index of fTrTrackR vector
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack(unsigned int i)const {return i<fTrTrack.size()? fTrTrack[i]:-1;}
  /// access function to TrTrackR objects   
  /// \param i index of fTrTrackR vector
  /// \return pointer to TrTrackR object  or 0
  TrTrackR * pTrTrack(unsigned int i) ;

  // VertexR Get data
  /// Return the Total Momentum (GeV)
  number        getmom()     const  {return Momentum;}
  /// Return the Error in 1 / Momentum Sum (1/GeV)
  number        geterrmom()  const  {return ErrMomentum;}
  /// Return theta (rad)
  number        gettheta()   const  {return Theta;}
  /// Return phi (rad)
  number        getphi()     const  {return Phi;}
  /// Return Rec mass at vertex (GeV)
  number        getmass()    const  {return Mass;}
  /// Return Total Charge at vertex
  integer       getcharge()  const  {return fCharge;}
  /// Chi2
  number        getchi2()    const  {return Chi2;}
  /// Number of degrees of freedom
  integer       getndof()    const  {return fNdof;}
  /// Return true if the vertex has been properly built 
  bool           IsFilled()   const  {return _filled!=0;}
  /// Return true if the vertex is build with an assumption of photon
  bool           IsPhotonVertex() const  {return _filled==2;}
  /// Returns an AMSPoint with the vertex coo
  AMSPoint      getvert()  const {return AMSPoint(Vertex[0],Vertex[1],Vertex[2]);}

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
  const char* Info(int iRef);

  /// Recover track index
  void Recover();

  void set_vertex();

  /// Fit vertex with two tracks
  double FitV(TrTrackR *trk1, TrTrackR *trk2,double beta);

  static double ZrefV;
 
  // Friends
  friend class AMSTrTrack;

  ClassDef(VertexR,4);

};
// End VertexR CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#endif
