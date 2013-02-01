#ifndef TRDRawHit_h
#define TRDRawHit_h

#include <TVector3.h>
#include "AMSGeometry.h"
#include "Tools.h"
#include "TRDRawHit-Streamer.h"

namespace ACsoft {

namespace AC {

/** Defines the mode of measurement (xz or yz)
  */
enum MeasurementMode {

  XZMeasurement = 0,
  YZMeasurement = 1
};

/** Transition Radiation %Detector raw hit data
  */
class TRDRawHit {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_TRDRawHit_Variables

  /** Helper method dumping a TRDRawHit object to the console
    */
  void Dump() const;

  /** Layer associated with this hit
    */
  Short_t Layer() const { return GeometryForHardwareAddress()[0]; } // Layer 0..19 in +z  order

  /** Ladder associated with this hit
    */
  Short_t Ladder() const { return GeometryForHardwareAddress()[1]; } // Ladder 0..13/0..15/0..17 in +x/+y order
  
  /** Tube associated with this hit
    */
  Short_t Tube() const {

    if( !GeometryForHardwareAddress()[2] )
      return ((fHWAddress % 100) % 16);     // Tube 0..15 in +x/+y order
    return 15 - ((fHWAddress % 100) % 16);  // UFE on other side
  }

  /** Global straw number for this hit
    */
  Short_t Straw() const {

    return Module() * AMSGeometry::TRDStrawsPerModule + Tube();
  }

  /** Deposited Energy [ADC counts].
    */
  Float_t DepositedEnergy() const { return ((Float_t) fADC) / 8.0; }


  /** Module number (0..327)
    */
  Short_t Module() const {

    return AMSGeometry::Self()->TRDLadderLayerToModuleLookupTable[Ladder()][Layer()];
  }

  /** Manifold Gas-Circuit (1..41)
    */
  Short_t GasCircuit() const {

    // FIXME: Introduce TrdStraw2GasCircuit and use it here.
    static const Int_t gGasCircuitInLayer[5] = { 7,8,8,9,9 };

    Int_t gc = 0; 
    for( Int_t i=0; i<Layer()/4; i++) gc += gGasCircuitInLayer[i];
    gc += Ladder()/2;

    return gc;
  }

  /** Manifold Gas-Group  1-5:W3   6-10:W7
    */
  Short_t GasGroup() const {

    // FIXME: Introduce TrdStraw2GasGroup and use it here.
    static const Int_t gGasGroupInGasCircuit[41] = { 
                           10,  9,  6,  3,  5,  2,  4,       // TRD-BOT
                          4,  8,  3,  6,  2,  7,  1,  9, 
                          1,  7,  2, 10,  3,  9,  5,  8,
                        7,  5,  6,  4,  8,  1,  6,  3, 10,
                        8,  8, 10,  7,  9,  4,  1,  5,  2 }; // TRD-TOP

    return gGasGroupInGasCircuit[GasCircuit()]; 
  }

private:
  inline Int_t* GeometryForHardwareAddress() const {

    return AMSGeometry::Self()->GetGeo[(fHWAddress/10000)][(fHWAddress/1000)%10][(fHWAddress/100)%10][(fHWAddress%100)/16];
  }

  Int_t getUFEch() const { // UFE-Channel 0..63
    int   SM_Lay[5] = {0,0,0,1,1}; // lowest Ladder in Layer/4 begins with Pos(0)/Neg(1) ReadoutSide
    Int_t ch;

    ch = (Layer() % 4) * 16; 
    if((Ladder()/2+SM_Lay[Layer()/4])%2) ch += (15-Tube());
    else                        ch +=     Tube();

    return ch;
  }

private:
  REGISTER_CLASS_WITH_TABLE(TRDRawHit)
};

/** Comparision operator, which can be used to sort a vector of TRDRawHits .
  * It will sort by layer number, then ladder number, then straw number, and finally descending amplitude.
  */
bool operator<(const TRDRawHit&, const TRDRawHit&);

}

}

#endif
