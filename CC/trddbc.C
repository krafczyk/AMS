//  $Id: trddbc.C,v 1.14 2001/03/29 12:17:05 kscholbe Exp $
#include <trddbc.h>
#include <amsdbc.h>
#include <math.h>
using namespace trdconst;
char * TRDDBc::_OctagonMedia[maxo]={"TRDCarbonFiber", "TRDCarbonFiber",
"TRDCarbonFiber","TRDCarbonFiber","TRDCarbonFiber","TRDHC","TRDHC",
"TRDHC","TRDRadiator"};
char * TRDDBc::_BulkheadsMedia="TRDCarbonFiber";
char * TRDDBc::_TubesMedia="TRDCapton";
char * TRDDBc::_ITubesMedia="TRDGas";
char * TRDDBc::_RadiatorMedia="VACUUM";  // Really hole in radiator
char * TRDDBc::_TubesBoxMedia="TRDFoam";

uinteger TRDDBc::_NoTRDOctagons[mtrdo]={8};
uinteger TRDDBc::_PrimaryOctagon[maxo]={0,1,2,3,4,3,4,1,1};
uinteger TRDDBc::_PrimaryOctagonNo=5;
uinteger TRDDBc::_TRDOctagonNo=1;
uinteger TRDDBc::_OctagonNo=maxo;
uinteger TRDDBc::_BulkheadsNo[mtrdo]={maxbulk};
uinteger TRDDBc::_LayersNo[mtrdo]={maxlay};
uinteger TRDDBc::_LaddersNo[mtrdo][maxlay]={14,14,14,14,16,16,16,16,16,16,16,16,18,18,18,18,18,18,18,18};

uinteger TRDDBc::_TubesNo[mtrdo][maxlay][maxlad];

uinteger   TRDDBc::_NumberTubes=0;
uinteger   TRDDBc::_NumberLadders=0;
uinteger   TRDDBc::_NumberBulkheads=0;


const number  TRDDBc::_TubeWallThickness=75e-4;
const number  TRDDBc::_TubeInnerDiameter=0.6;
const number  TRDDBc::_TubeBoxThickness=(0.62-TRDDBc::_TubeInnerDiameter-2*TRDDBc::_TubeWallThickness)/2.;
const number  TRDDBc::_LadderThickness=2.9;
const number  TRDDBc::_FirstLayerHeight = 1.7; // Distance of first layer-line from bottom (center of bottom main octagon skin)
const number TRDDBc::_WirePosition = 0.725; // Distance of wire below layer-line
const number TRDDBc::_ManifoldThickness = 0.70; // Z-height of manifold
const number TRDDBc::_ManifoldLength = 1.15; // Length of manifold along wire
const number TRDDBc::_ManifoldWidth = 10.1; // Width of manifold 
const number TRDDBc::_BulkheadGap = 0.78; // Gap between ladders at bulkhead

const integer TRDDBc::_LadderOrientation[mtrdo][maxlay]={0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0};   // 0 -x 1 -y    
number TRDDBc::_OctagonDimensions[maxo][10]; 
number TRDDBc::_BulkheadsDimensions[mtrdo][maxbulk][4];
number TRDDBc::_LaddersDimensions[mtrdo][maxlay][maxlad][3];
number TRDDBc::_TubesDimensions[mtrdo][maxlay][maxlad][3];    
number TRDDBc::_TubesBoxDimensions[mtrdo][maxlay][maxlad][10];    
number TRDDBc::_RadiatorDimensions[mtrdo][maxlay][maxlad][3];

  // Positions & Orientations
  
TRDDBc* TRDDBc::_HeadOctagonPos=0;
TRDDBc* TRDDBc::_HeadBulkheadPos=0;
TRDDBc* TRDDBc::_HeadLadderPos=0;
TRDDBc* TRDDBc::_HeadRadiatorPos=0;
TRDDBc* TRDDBc::_HeadTubeBoxPos=0;
TRDDBc* TRDDBc::_HeadTubePos=0;

void TRDDBc::init(){

   // Quantities
    int i,j,k,l;
    for(i=0;i<TRDOctagonNo();i++){
     for(j=0;j<LayersNo(i);j++){
      for(k=0;k<LaddersNo(i,j);k++){
       _TubesNo[i][j][k]=16;
      }
     }
    }


  // calculate tubesno #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++){
//           cout <<_NumberTubes<<" "<<i<<" "<<j<<" "<<k<<" "<<l<<endl;
           _NumberTubes++;
          }
         }
        }
       }

       cout <<"TRDDBcI-I-Total of " <<_NumberTubes<< "  tubes initialized."<<endl;
  // calculate ladder #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++)_NumberLadders++;
        }
       }
       cout <<"TRDDBcI-I-Total of " <<_NumberLadders<< "  ladders initialized."<<endl;
  // calculate bulkhead #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<BulkheadsNo(i);j++)_NumberBulkheads++;
       }
       cout <<"TRDDBcI-I-Total of " <<_NumberBulkheads<< "  bulkheads initialized."<<endl;


       _HeadLadderPos=new TRDDBc[_NumberLadders];
       _HeadBulkheadPos=new TRDDBc[_NumberBulkheads];
       _HeadRadiatorPos=new TRDDBc[_NumberLadders];
       _HeadTubeBoxPos=new TRDDBc[_NumberLadders];
       _HeadTubePos=new TRDDBc[_NumberTubes];
       _HeadOctagonPos= new TRDDBc[OctagonNo()]; 



   // HereStartsTheJob;

      number unitnrm[3][3]={1,0,0,0,1,0,0,0,1};

//    OctagonDimensions
// Drawings show mm so divide by 10.  Divide by 20 when the full width
// (or diameter as opposed to radius)
// is given in the drawing, since the
      // dimensions to be fed into Geant are centered on zero

   // Primary octagons

// Top octagon carbon fiber ring

      OctagonDimensions(0,4)=-6./20.;   // z coord

   // Bottom edge
      OctagonDimensions(0,5)=2052./20.;         // rmin
      OctagonDimensions(0,6)=2220./20.;  // rmax

   // Top edge
      OctagonDimensions(0,8)=2052./20;         // rmin
      OctagonDimensions(0,9)=2220./20.;  // rmax

// Main octagon

      number ang=22.5/180.*AMSDBc::pi;

     // z coord... drawing gives height dimension including
     //   top and bottom carbon fiber ring widths,
     // so subtract the widths of these
         OctagonDimensions(1,4)=-621./20.
                +6./20. + 6./20.;

      // Bottom edge
      OctagonDimensions(1,5)=0;                // rmin

      // On drawing, 1600 is intersection with middle of bottom skin
      OctagonDimensions(1,6)=1600./20.-2./10./cos(ang);   // rmax

      // Top edge
      OctagonDimensions(1,8)=0;    // rmin
      OctagonDimensions(1,9)=OctagonDimensions(1,6)+2*fabs(OctagonDimensions(1,4))*tan(ang);   // rmax
 
//  Bottom octagon carbon fiber ring

      OctagonDimensions(2,4)=-6./20.;    // z coord 

     // Bottom edge
      OctagonDimensions(2,5)=1470./20.;         // rmin
      OctagonDimensions(2,6)=1590./20.;  // rmax

     // Top edge
      OctagonDimensions(2,8)=1470./20.;         // rmin
      OctagonDimensions(2,9)=1590./20;  // rmax

// Top honeycomb cover

      OctagonDimensions(3,4)=-94./20.;   // z coord

   // Bottom edge
      OctagonDimensions(3,5)=0;         // rmin
      OctagonDimensions(3,6)=OctagonDimensions(0,6); // rmax
      // Same as top of carbon fiber ring       

   // Top edge
      OctagonDimensions(3,8)=0;         // rmin
      OctagonDimensions(3,9)=OctagonDimensions(0,9);  // rmax

  // Octagon to be filled with bottom honeycomb
      // This actually has notches in it.

      OctagonDimensions(4,4)=-41./20.;   // z coord

   // Bottom edge

      OctagonDimensions(4,5)=0;         // rmin
      OctagonDimensions(4,6)=1550./20.; // rmax

      // Top edge
      OctagonDimensions(4,8)=0;         // rmin
      OctagonDimensions(4,9)=1550./20.;  // rmax

  // Now the non-primary octagons

  //   Inner honeycomb part on top, inside octagon 3 (leaving
  //            1 mm skin of carbon fiber on top and bottom)

      OctagonDimensions(5,4)=OctagonDimensions(3,4)+2./20.;  //z coord

      // Bottom edge
      OctagonDimensions(5,5)=0;   // rmin
      OctagonDimensions(5,6)=OctagonDimensions(3,6);  // rmax

      // Top edge
      OctagonDimensions(5,8)=0;  // rmin
      OctagonDimensions(5,9)=OctagonDimensions(3,9); // rmax

  //   Inner honeycomb part on bottom, inside octagon 4 (leaving
  //            0.5 mm skin of carbon fiber on top and bottom)
      
      OctagonDimensions(6,4)=OctagonDimensions(4,4)+1./20.;  //z coord

      // Bottom edge
      OctagonDimensions(6,5)=0;   // rmin
      OctagonDimensions(6,6)=OctagonDimensions(4,6);  // rmax
                    
      // Top edge
      OctagonDimensions(6,8)=0;  // rmin
      OctagonDimensions(6,9)=OctagonDimensions(4,9); // rmax

  //  Honeycomb inside sides of TRD structure

      OctagonDimensions(7,4)=OctagonDimensions(1,4)+ 4./20.;  // z coord

      // Bottom edge
      OctagonDimensions(7,5)= OctagonDimensions(1,6)-30./10./cos(ang)
                                   +2./10./cos(ang);
      OctagonDimensions(7,6)=OctagonDimensions(1,6)-2./10./cos(ang);  // rmax

      // Top edge
      OctagonDimensions(7,8)=OctagonDimensions(1,9)-30./10./cos(ang)
                      +2./10./cos(ang);                             // rmin
      OctagonDimensions(7,9)=OctagonDimensions(1,9)-2./10./cos(ang); // rmax


  // Inner part... this is where ladders and tubes go

      OctagonDimensions(8,4)=OctagonDimensions(1,4)+8./20.;  // z coord

      // Bottom edge
      OctagonDimensions(8,5)=0;
      OctagonDimensions(8,6)=OctagonDimensions(7,5)-2./10./cos(ang);

      // Top edge
      OctagonDimensions(8,8)=0;
      OctagonDimensions(8,9)=OctagonDimensions(7,8)-2./10./cos(ang);

      
      for(i=0;i<OctagonNo();i++){
       OctagonDimensions(i,0)=0-360/16.;
       OctagonDimensions(i,1)=360;
       OctagonDimensions(i,2)=8;
       OctagonDimensions(i,3)=2;
       OctagonDimensions(i,7)=-OctagonDimensions(i,4);
      }    

//    Position the primary octagons

      uinteger status=1;
      uinteger gid=0;
      geant coo[3];
      for(i=0;i<PrimaryOctagonNo();i++){
       for(j=0;j<3;j++){
        coo[j]=0;
       }

// For all these, subtract half-width from position of top of volume

     // 835. is position of bottom of bottom carbon fiber ring

       switch(i){
       case 4:    // bottom honeycomb... starts 1.5 mm below carbon
	 //  fiber ring
          coo[2]= (835.-1.5)/10.+OctagonDimensions(i,4);
          break;

       case 3:   // top honeycomb...need to check this
          coo[2]= (1476+94.+6.)/10.+OctagonDimensions(i,4);
          break;

       case 2: // bottom carbon fiber cover
          coo[2]=(835.+6.)/10.+OctagonDimensions(i,4);
          break;

       case 1: // main octagon
          coo[2]=(835.+621.-6.)/10.+OctagonDimensions(i,4);
          break;

       case 0: // top carbon fiber octagon
          coo[2]=(835.+621.)/10.+OctagonDimensions(i,4);
          break;
       }
       SetOctagon(i,status,coo,unitnrm,gid); 
      }

      for(i=PrimaryOctagonNo();i<OctagonNo();i++){
       for(j=0;j<3;j++){
        coo[j]=0;
       }
       SetOctagon(i,status,coo,unitnrm,gid); 
      }


      number blknrm[2][3][3]={1,0,0,0,1,0,0,0,1,
                            0,1,0,1,0,0,0,0,1};


      // Bulkheads
      for(i=0;i<TRDOctagonNo();i++){

	// Note: this doesn't quite match the diagram
	number bulkhead_pos = 3.*ManifoldWidth()+BulkheadGap()/2.;
	
        // Central bulkheads  in x direction

	// Bulkhead 0

        // Bottom half-width	
	BulkheadsDimensions(i,0,0)= OctagonDimensions(8,6);

        // dy
	BulkheadsDimensions(i,0,2)= 3./20.;

	//dz
	// Not sure what this is for the moment
	// Look like it stops half a layer above the top layer line
        // Top layer line is 35 mm below top 

	BulkheadsDimensions(i,0,3)= OctagonDimensions(8,7)-35./20.+
                   LadderThickness()/4.;

        // Top half-width
	BulkheadsDimensions(i,0,1)= BulkheadsDimensions(i,0,0)+
                                     2.*tan(ang)*BulkheadsDimensions(i,0,3);


        // Position it
	coo[0] = 0.;
	coo[1] = -bulkhead_pos;
	coo[2] = -(fabs(OctagonDimensions(8,4))-BulkheadsDimensions(i,0,3));

	SetBulkhead(0,i,status,coo,blknrm[0],gid);

	// Bulkhead 1

	for (j=0;j<4;j++)
	  {
	    BulkheadsDimensions(i,1,j)= BulkheadsDimensions(i,0,j);
	  }

        // Position it
	coo[0] = 0.;
	coo[1] = bulkhead_pos;
	coo[2] = -(fabs(OctagonDimensions(8,4))-BulkheadsDimensions(i,1,3));

	SetBulkhead(1,i,status,coo,blknrm[0],gid);

	// Top bulkheads in y direction

	// Bulkhead 2

	// dz half-height.. also not sure what this is exactly
	BulkheadsDimensions(i,2,3)= 4.*LadderThickness()/2.;

        // Top half-width
	BulkheadsDimensions(i,2,1)= OctagonDimensions(8,9);

        // Bottom half-width
	BulkheadsDimensions(i,2,0)= BulkheadsDimensions(i,2,1)
	  -2.*BulkheadsDimensions(i,2,3)*tan(ang);


	BulkheadsDimensions(i,2,2)= 3./20.;

        // Position it wrt main octagon
	coo[0] = -bulkhead_pos;
	coo[1] = 0.;
	coo[2] = OctagonDimensions(8,7)-35./10.+LadderThickness()/2.
                        -BulkheadsDimensions(i,2,3);

	SetBulkhead(2,i,status,coo,blknrm[1],gid);

	// Bulkhead 3

	for (j=0;j<4;j++)
	  {
	    BulkheadsDimensions(i,3,j)= BulkheadsDimensions(i,2,j);
	  }

        // Position it
	coo[0] = bulkhead_pos;
	coo[1] = 0.;
	coo[2] = OctagonDimensions(8,7)-35./10.+LadderThickness()/2.
                         -BulkheadsDimensions(i,3,3);

	SetBulkhead(3,i,status,coo,blknrm[1],gid);

	// Bottom bulkheads in y direction


	// Bulkhead 4

	// dz half-height.. also not sure what this is exactly
	BulkheadsDimensions(i,4,3)= 3.5*LadderThickness()/2.
	     +FirstLayerHeight()/2.;

        // Bottom half-width
	BulkheadsDimensions(i,4,0)= OctagonDimensions(8,6);

        // Top half-width
	BulkheadsDimensions(i,4,1)= BulkheadsDimensions(i,4,0)
	 +2.*BulkheadsDimensions(i,4,3)*tan(ang);


        // dy
	BulkheadsDimensions(i,4,2)= 3./20.;


        // Position it
	coo[0] = -bulkhead_pos;
	coo[1] = 0.;
	coo[2] = OctagonDimensions(8,4)+BulkheadsDimensions(i,4,3);

	SetBulkhead(4,i,status,coo,blknrm[1],gid);

	// Bulkhead 5

	for (j=0;j<4;j++)
	  {
	    BulkheadsDimensions(i,5,j)= BulkheadsDimensions(i,4,j);
	  }

        // Position it
	coo[0] = bulkhead_pos;
	coo[1] = 0.;
	coo[2] = OctagonDimensions(8,4)+BulkheadsDimensions(i,5,3);

	SetBulkhead(5,i,status,coo,blknrm[1],gid);

      }


      number nrmxy[2][3][3]={0,0,1,1,0,0,0,1,0,
                            1,0,0,0,0,-1,0,1,0};


     // LadderSizes
      for(i=0;i<TRDOctagonNo();i++){

//       Get octagon parameters at lowest point

	number rmin=OctagonDimensions(NoTRDOctagons(i),6);
	number rmax=OctagonDimensions(NoTRDOctagons(i),9);
	number zoct=OctagonDimensions(NoTRDOctagons(i),7)-OctagonDimensions(NoTRDOctagons(i),4);
	number tang=(rmax-rmin)/zoct;
	
	for(j=0;j<LayersNo(i);j++){

	  for(k=0;k<LaddersNo(i,j);k++){

	    //	    coo[0]=TubeInnerDiameter()
            //          +2*TubeWallThickness()+2*TubeBoxThickness();

	    // coo[0]*=TubesNo(i,j,k);
	    coo[0] = ManifoldWidth();
	    coo[1] = LadderThickness();

	    // This is the inner tangent radius of the octagon
            //  at the center of the side inner skin

	    // Reset rmin here for check

	    number r = 1541./20.-2./20./cos(ang);

	    //	    number r=rmin+2./20./cos(ang);

	    // Height of the bottom of manifold for the layer
            // Measured above the top of the lower carbon skin

	    number lhgt = FirstLayerHeight()
	      -WirePosition()-ManifoldThickness()/2.+j*LadderThickness();

	    // Add half a layer thickness to certain
            // tubes corresponding to the staggered pattern

            if (j<=3 ||j>=12)
	      {
		if (k%2 == 0)
		  {
		    lhgt += LadderThickness()/2.;
		  }
	      }
	    else
	      {
		if (k%2 == 1)
		  {
		    lhgt += LadderThickness()/2.;
		  }
	      }

	    // leftright is -1 if the ladder pokes into left of octagon,
	    // +1 if it pokes into right.  

	    integer leftright;
	    if(k<(LaddersNo(i,j)+1)/2)leftright=-1;
	    else leftright=1;

	    // Add the bulkhead gap width for ladders outside the bulkhead

	    number gap = 0.;

	    // Number away from the center of the ladder

	    int num_from_center = k-LaddersNo(i,j)/2;
	    if (leftright>0) num_from_center++;

	    // Check if we've crossed the bulkhead
            //  Bulkhead gap only present for lower and upper 4 layers
            // "x wires"

	    if (fabs(num_from_center)>=4 && (j<=3 || j>=16) )
	      {
	        gap = BulkheadGap();
	      }

	    // Position of the corner of ladder, transverse to wire direction

	    number point=ManifoldWidth()*num_from_center
                            +leftright*gap;

            // Center of manifold distance from origin,
            // in direction transverse to wire direction
 
	    number man_center = fabs(point) - ManifoldWidth()/2.;

	    // Now find the radius of the center of the 
            // octagon side inner skin
	    // at the height of the lower manifold edge
	    // This octagon inner tangent circle radius to be
            // used for "90 deg" ladders.
 
	    number r90 = r+lhgt*tan(ang);

	    // Length of octagon side at this given height

	    number a=2*r90/(1.+sqrt(2.));

	    // Find if the center of the manifold is past the corner

	    int past_corner = 0;
	    if (fabs(man_center)>a/2)
	      {
		past_corner = 1;
	      }

	    //	    if (past_corner)
	    
	    int deg = 90;
	    if (fabs(num_from_center)>=5)
	      {
		deg = 45;
		//		cout <<"45 deg condition"<<endl;

		// Ladder is a "45 degree" one

                // Get bottom radius
                // and add half HC width and half inner skin width
                // to be at the center of the HC

		r += 2./20./cos(ang) + 26./20./cos(ang);

                // Inner tangent radius of center of HC octagon 
                // at the height of the manifold bottom

                number r45 =  r + lhgt*tan(ang);

		// Length of side of the octagon at this height

		a = 2.*r45/(1.+sqrt(2.));

		// This is the half length of the ladder: 
                //   add half the side length to the "rise"
		//  corresponding to "run" of octagon
                //   radius - distance from center of manifold center,
		// with slope of 1 since it's 45 degrees
		
		coo[2] = (r45-man_center)+a/2;

	      }
	    else
	      {
		//		cout<< "90 deg "<<r90<<" "<<past_corner<<endl;
		// If center sticks out past the corner
		//if (past_corner)
		  //		  {
		    //		    cout <<"Past corner!!"<<endl;
		  //coo[2] = (r90-man_center)+a/2;
		  //}
		//else
		// Use r90
		coo[2] = r90;
		}

	    // Length to compare with engineer's numbers, which give
            // length up to halfway along the manifold, in mm

	    number length_to_compare = 20.*(coo[2]+ManifoldLength()/2.);

	    // Add manifold length along wire

	    coo[2]+= ManifoldLength();

	    // Full ladder length

	    coo[2]*=2;
	
	    //	    cout <<j<<" "<<k<<" "<<j+1<<" "<<num_from_center<<" "<<deg<<" "<<past_corner<<" "<<length_to_compare<<endl;
	    for(int l=0;l<3;l++)LaddersDimensions(i,j,k,l)=coo[l]/2;          
	  }  
	}

      }
     number nrm[3][3];
    //Ladder Position & Orientation
      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){


	  // Check if past the bulkhead gap

	   number gap = 0.;

	    // Number away from the center of the ladder

	    integer leftright;
	    if(k<(LaddersNo(i,j)+1)/2)leftright=-1;
	    else leftright=1;

	    int num_from_center = k-LaddersNo(i,j)/2;
	    if (leftright>0) num_from_center++;

	    // Check if we've crossed the bulkhead
            //  Bulkhead gap only present for lower and upper 4 layers
            // "x wires"

	    if (fabs(num_from_center)>=4 && (j<=3 || j>=16) )
	      {
	        gap = BulkheadGap();
	      }

	  // x or y position coordinates wrt mother octagon

         coo[LadderOrientation(i,j)]=0;
         coo[1-LadderOrientation(i,j)]=-LaddersDimensions(i,j,k,0)*(LaddersNo(i,j)-1.-2*k)+leftright*gap;

         // z position of center of ladder wrt mother octagon

         coo[2]= OctagonDimensions(1,4)+FirstLayerHeight()+2./10.
            +LaddersDimensions(i,j,k,1)*2.*j;

         SetLadder(k,j,i,status,coo,nrmxy[LadderOrientation(i,j)],gid);
        }
       }
      }
      const number Safety=0;

      // Radiator and tube sizes

      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){
	  // width
          RadiatorDimensions(i,j,k,0)=LaddersDimensions(i,j,k,0);
          // height

          RadiatorDimensions(i,j,k,1)=LaddersDimensions(i,j,k,1)-(TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness()+2*Safety)/2.;

	  // It gets complicated when you're on an orientation change
          // boundary... need to specially calculate....
          // And the radiator may not be a uniform box.

	  // Bottom layer has no radiator below the tubes
          // This is what will be filled with vacuum

	  if (j==0)
	    {
	      // This distance is above the bottom floor
              //   of the bottom of the tube

	      if (k%2 == 0)
		{
		// Upper tube
	      // Start at height of 1st layer line... down to lower wire
              // position... add half a layer to arrive at upper
              ///  wire position... subtract 1/2 width of tube
		  RadiatorDimensions(i,j,k,1)
		    = FirstLayerHeight()-2./10.
                    -WirePosition()+LadderThickness()/2.
                    -(TubeInnerDiameter()+2*TubeWallThickness()
                        +2*TubeBoxThickness()+2*Safety)/2.;

		}
	      else  
		{
		  // Lower tube

		  RadiatorDimensions(i,j,k,1)
		    = FirstLayerHeight()-2./10.
                    -WirePosition()
                    -(TubeInnerDiameter()+2*TubeWallThickness()
                        +2*TubeBoxThickness()+2*Safety)/2.;
		}
	    }
	  RadiatorDimensions(i,j,k,1)/= 2.;


          // length
          RadiatorDimensions(i,j,k,2)=LaddersDimensions(i,j,k,2);

	  // width
          TubesDimensions(i,j,k,0)=TubeInnerDiameter()/2;          
          // height
          TubesDimensions(i,j,k,1)=(TubeInnerDiameter()+2*TubeWallThickness())/2;
          // length
          TubesDimensions(i,j,k,2)=LaddersDimensions(i,j,k,2);
        }
       }
      }
      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){

	  // x, y position of radiator wrt ladder

         coo[0]=coo[2]=0;

	 // z position of radiator wrt ladder

         // This will be modified for the bottom layer
	 int tube_is_upper = 0;
	 if (j<=3 ||j>=12)
	   {
	     if (k%2 == 0)  // Upper tube first
	       {
		 tube_is_upper = 1;
	       }
	   }
	  else
	   {

	     if (k%2 == 1)  // Lower tube first
	       {
		 tube_is_upper = 1;
	       }
	   }

	 if (tube_is_upper)
	   {
	     coo[1]=-WirePosition()+LadderThickness()/2.-(TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness()+2*Safety)/2.-RadiatorDimensions(i,j,k,1);		   }
	 else
	   {
	     coo[1]=-WirePosition()+(TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness()+2*Safety)/2.+RadiatorDimensions(i,j,k,1);

	     // Case of vacuum on bottom layer
	     if (j==0)
	       {
		 coo[1]=-WirePosition()-(TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness()+2*Safety)/2.-RadiatorDimensions(i,j,k,1);
	       }

	   }

         SetRadiator(k,j,i,status,coo,unitnrm,gid);

         for(int l=0;l<TubesNo(i,j,k);l++){

	   // x, y positions of tubes wrt ladder

           coo[0]=-LaddersDimensions(i,j,k,0)+(TubesDimensions(i,j,k,1)+TubeBoxThickness())*(1+2*l);

           coo[2]=0;

	   // z position of tubes 

	   if (tube_is_upper)
	     {
	       coo[1]= -WirePosition()+LadderThickness()/2.;
	     }
	   else
	     {
	       coo[1]= -WirePosition();
	     }
           SetTube(l,k,j,i,status,coo,unitnrm,gid);
         }
        }
       }
      }
}

void TRDDBc::read(){

init();

}

void TRDDBc::write(){
}

uinteger TRDDBc::getnumOctagon(uinteger oct){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
return oct;
}

uinteger TRDDBc::getnumBulkhead(uinteger bulkhead,uinteger oct){
       int num=0;
       int i,j;
       for ( i=0;i<oct;i++){
        for ( j=0;j<BulkheadsNo(i);j++)num++;
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<bulkhead;j++)num++;
       }


#ifdef __AMSDEBUG__
   assert(num<_NumberBulkheads);
#endif
   return num;

}


uinteger TRDDBc::getnumTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=0;k<ladder;k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=ladder;k<ladder+1;k++){
          for(l=0;l<tube;l++)num++;
         }
        }
       }
#ifdef __AMSDEBUG__
   assert(num<_NumberTubes);
#endif
     return num;
}

uinteger TRDDBc::getnumLadder(uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
         for ( k=0;k<LaddersNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=0;k<ladder;k++)num++;
        }
       }

#ifdef __AMSDEBUG__
   assert(num<_NumberBulkheads);
#endif
     return num;
}

void TRDDBc::SetOctagon(uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger &gid){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
   _HeadOctagonPos[oct]._status=status;     
   _HeadOctagonPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadOctagonPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadOctagonPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetOctagon(uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
   oct=getnumOctagon(oct);
   status=_HeadOctagonPos[oct]._status;     
   gid=_HeadOctagonPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadOctagonPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadOctagonPos[oct]._nrm[i][j];
    }
   }

}


void TRDDBc::SetBulkhead(uinteger bulkhead,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){

    oct=getnumBulkhead(bulkhead,oct);    
   _HeadBulkheadPos[oct]._status=status;     
   _HeadBulkheadPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadBulkheadPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadBulkheadPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetBulkhead(uinteger bulkhead, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){

   oct=getnumBulkhead(bulkhead,oct);    
   status=_HeadBulkheadPos[oct]._status;     
   gid=_HeadBulkheadPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadBulkheadPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadBulkheadPos[oct]._nrm[i][j];
    }
   }

}


void TRDDBc::SetLadder(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadLadderPos[oct]._status=status;     
   _HeadLadderPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadLadderPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadLadderPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetLadder(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadLadderPos[oct]._status;     
   gid=_HeadLadderPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadLadderPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadLadderPos[oct]._nrm[i][j];
    }
   }

}

void TRDDBc::SetRadiator(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadRadiatorPos[oct]._status=status;     
   _HeadRadiatorPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadRadiatorPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadRadiatorPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetRadiator(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadRadiatorPos[oct]._status;     
   gid=_HeadRadiatorPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadRadiatorPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadRadiatorPos[oct]._nrm[i][j];
    }
   }

}

void TRDDBc::SetTubeBox(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadTubeBoxPos[oct]._status=status;     
   _HeadTubeBoxPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadTubeBoxPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadTubeBoxPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetTubeBox(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadTubeBoxPos[oct]._status;     
   gid=_HeadTubeBoxPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadTubeBoxPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadTubeBoxPos[oct]._nrm[i][j];
    }
   }
}

void TRDDBc::SetTube(uinteger tube,uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder,tube);
#endif
    oct=getnumTube(tube,ladder,layer,oct);    
   _HeadTubePos[oct]._status=status;     
   _HeadTubePos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadTubePos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadTubePos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder,tube);
#endif
   oct=getnumTube(tube,ladder,layer,oct);    
   status=_HeadTubePos[oct]._status;     
   gid=_HeadTubePos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadTubePos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadTubePos[oct]._nrm[i][j];
    }
   }

}

uinteger TRDDBc::_check(uinteger oct,uinteger layer,uinteger ladder, uinteger tube){
 if(oct>=TRDOctagonNo()){
  cerr<<"TRDDBc::_check-S-TRDOctagonNoOutOfOrder "<<oct<<endl;
  return 0;
 }
 if(layer>=LayersNo(oct)){
  cerr<<"TRDDBc::_check-S-TRDLayersNoOutOfOrder "<<layer<<endl;
  return 0;
 }
 if(ladder>=LaddersNo(oct,layer)){
  cerr<<"TRDDBc::_check-S-TRDLaddersNoOutOfOrder "<<ladder<<endl;
  return 0;
 }
 if(tube>=TubesNo(oct,layer,ladder)){
  cerr<<"TRDDBc::_check-S-TRDTubesNoOutOfOrder "<<tube<<endl;
  return 0;
 }
 return 1;
}

number & TRDDBc::BulkheadsDimensions(uinteger toct, uinteger bulkhead ,uinteger index){
return _BulkheadsDimensions[toct][bulkhead][index];
}

number & TRDDBc::RadiatorDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_RadiatorDimensions)/sizeof(_RadiatorDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _RadiatorDimensions[toct][lay][lad][index];
}

number & TRDDBc::TubesBoxDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesBoxDimensions)/sizeof(_TubesBoxDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _TubesBoxDimensions[toct][lay][lad][index];
}

number  TRDDBc::ITubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesDimensions)/sizeof(_TubesDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
switch(index){
case 2:
return _TubesDimensions[toct][lay][lad][index];
case 1:
return _TubesDimensions[toct][lay][lad][0];
default:
return 0;
}
}

number & TRDDBc::TubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesDimensions)/sizeof(_TubesDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _TubesDimensions[toct][lay][lad][index];
}

number & TRDDBc::LaddersDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_LaddersDimensions)/sizeof(_LaddersDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _LaddersDimensions[toct][lay][lad][index];
}

number & TRDDBc::OctagonDimensions(uinteger toct, uinteger index){
#ifdef __AMSDEBUG__
assert(toct<OctagonNo());
assert(index<sizeof(_OctagonDimensions)/sizeof(_OctagonDimensions[0][0])/maxo);
#endif
return _OctagonDimensions[toct][index];
}


char* TRDDBc::CodeLad(uinteger gid){
 static char output[3]={'\0','\0','\0'};
 static char code[]="QWERTYUIOPASFGHJKLZXCVNM1234567890";
 integer size=strlen(code);
 if(gid<size*size){
  output[0]=code[gid%size]; 
  output[1]=code[gid/size]; 
 }
 else{
   cerr<<"TRDDBc::CodeLad-F-CouldNotCodeId "<<gid<<endl;
   exit(1);
 }
 return output;
}

integer TRDDBcI::_Count=0;

TRDDBcI::TRDDBcI(){
_Count++;
}

TRDDBcI::~TRDDBcI(){
  if(--_Count ==0){
    TRDDBc::write();   
  }
}
