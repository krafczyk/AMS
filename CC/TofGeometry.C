#include "TofGeometry.h"

#include "TLine.h"

ClassImp(TofGeometry);

static float tof_xmin[4][10]={
  -63.25,-65.25,-65.25,-65.25,-65.25,-65.25,-65.25,-63.25,0,0,
  -59.75,-34.75,-23.25,-11.75,-0.25,11.25,22.75,34.25,0,0,
  -64.25,-46.25,-34.75,-23.25,-11.75,-0.25,11.25,22.75,34.25,45.75,
  -65.00,-67.00,-67.00,-67.00,-67.00,-67.00,-67.00,-65.00,0,0
};
static float tof_xmax[4][10]={
  63.25,65.25,65.25,65.25,65.25,65.25,65.25,63.25,0,0,
  -34.25,-22.75,-11.25,0.25,11.75,23.25,34.75,59.75,0,0,
  -45.75,-34.25,-22.75,-11.25,0.25,11.75,23.25,34.75,46.25,64.25,
  65.00,67.00,67.00,67.00,67.00,67.00,67.00,65.00,0,0
};
static float tof_ymin[4][10]={
  -56.75,-34.75,-23.25,-11.75,-0.25,11.25,22.75,34.25,0,0,
  -58.60,-63.50,-66.10,-66.10,-66.10,-66.10,-63.50,-58.60,0,0,
  -55.00,-58.60,-63.50,-66.10,-66.10,-66.10,-66.10,-63.50,-58.60,-55.00,
  -60.25,-34.75,-23.25,-11.75,-0.25,11.25,22.75,34.25,0,0
};
static float tof_ymax[4][10]={
  -34.25,-22.75,-11.25,0.25,11.75,23.25,34.75,56.75,0,0,
  58.60,63.50,66.10,66.10,66.10,66.10,63.50,58.60,0,0,
  55.00,58.60,63.50,66.10,66.10,66.10,66.10,63.50,58.60,55.00,
  -34.25,-22.75,-11.25,0.25,11.75,23.25,34.75,60.25,0,0
};
static float tof_z[4][10]={
  64.425,65.975,64.425,65.975,64.425,65.975,64.425,65.975,0,0,
  61.325,62.875,61.325,62.875,61.325,62.875,61.325,62.875,0,0,
  -62.875,-61.325,-62.875,-61.325,-62.875,-61.325,-62.875,-61.325,-62.875,-61.325,
  -64.425,-65.975,-64.425,-65.975,-64.425,-65.975,-64.425,-65.975,0,0
};
static float tof_trap_xycut[4]={38.25,34.3,35.0,38.1};
static float tof_trap_width[4]={17.5,12.,12.,17.5};
static int   nbars[4] = {8,8,10,8};
static int   transverse[4] = {1,0,0,1}; // layer transverse coordinate 0 = x, 1 = y


bool TofGeometry::IsTransverseX(int il) {
  return ((il>=0)&&(il<4)) ? (transverse[il]==0) : false;
}


float TofGeometry::GetZLayer(int il) {
  return ((il<4)&&(il>=0)) ? (tof_z[il][0] + tof_z[il][1])/2 : -1000;
}


float TofGeometry::GetLayerThickness(int il) {
  return fabs(tof_z[il][1] - tof_z[il][0])+1;
}


float TofGeometry::GetZPaddle(int il, int ib) {
  if ( (il<0)||(il>3) ) return -100;
  if ( (ib<0)||(ib>=GetNBars(il)) ) return -1000;
  return tof_z[il][ib];
}


int TofGeometry::GetNBars(int il) { 
  return ((il<4)&&(il>=0)) ? nbars[il] : 0;
}


/*
int TofGeometry::GetTofPaddleIndex(AMSPoint point) {
  AMSDir dir(0,0,1);
  for (int ilayer=0; ilayer<4; ilayer++) {
    for (int ibar=0; ibar<GetNBars(ilayer); ibar++) {
      if (PathLengthInAPaddle(ilayer,ibar,point,dir)>0) return ilayer*10+ibar;
    }
  }  
  return -1;
}
*/


int TofGeometry::GetTofPaddleIndex(AMSPoint point, float delta) {
  int index = -1;
  int nfound = 0;
  for (int ilayer=0; ilayer<4; ilayer++) {
    for (int ibar=0; ibar<GetNBars(ilayer); ibar++) {
      // z check
      if (fabs(point.z()-GetZPaddle(ilayer,ibar))>0.5) continue;
      // xy check
      if (HitCounter(point.x(),point.y(),ilayer,ibar,delta)) {
        index = ilayer*10 + ibar;
        nfound++;
      }
    }
  }
  if (nfound>1) return -nfound; 
  return index;
}


/*
int TofGeometry::GetTofPaddleIndexYZ(float y, float z) {
  // evaluate layer (even/odd)
  int iLayer = -1;
  for (int ilayer=0; ilayer<4; ilayer++) {
    // position of the layer is approximately (z_pad1 + z_pad2)/2
    if (fabs(z-(tof_z[ilayer][0]+tof_z[ilayer][1])/2)<2) {
      iLayer = ilayer;
      break;
    }
  }
  if (iLayer<0) return -1;
  // the layer should have Y transverse coordinate 
  if (IsTransverseX(iLayer)) return -3;
  // evaluate paddle  
  int iBar = -1;
  for (int ibar=0; ibar<GetNBars(iLayer); ibar++) {
    if ( (y>=tof_ymin[iLayer][ibar])&&(y<=tof_ymax[iLayer][ibar]) ) { 
      iBar = ibar; 
      break; 
    }
  }
  if (iBar<0) return -2;
  return iLayer*10 + iBar;
}
*/

bool TofGeometry::HitCounter(float x, float y, int il, int ic, float delta) {
  int ic_max;
  float x0; float x1; float x2; float x3;
  float y0; float y1; float y2; float y3;
  float a; float b;
  float y_calc;
  float x_calc;
  if(il<0 || il>3) return false;
  if (il==2) {
    ic_max=10;
  }
  else {
    ic_max=8;
  }
  if(ic<0 || ic>ic_max) return false;
  if (il==0 || il==3) {
    if (ic==0) {
      x0=tof_xmin[il][ic];    y0=tof_ymax[il][ic]-tof_trap_width[il];
      x1=-tof_trap_xycut[il]; y1=tof_ymin[il][ic];
      x2=tof_trap_xycut[il];  y2=tof_ymin[il][ic];
      x3=tof_xmax[il][ic];    y3=tof_ymax[il][ic]-tof_trap_width[il];
    }
    else if (ic==ic_max-1) {
      x0=tof_xmin[il][ic];    y0=tof_ymin[il][ic]+tof_trap_width[il];
      x1=-tof_trap_xycut[il]; y1=tof_ymax[il][ic];
      x2=tof_trap_xycut[il];  y2=tof_ymax[il][ic];
      x3=tof_xmax[il][ic];    y3=tof_ymin[il][ic]+tof_trap_width[il];
    }
    if (x<0) {
      a=(y1-y0)/(x1-x0);
      b=-a*x0+y0;
    }
    else {
      a=(y3-y2)/(x3-x2);
      b=-a*x2+y2;
    }
    y_calc=a*x+b;
  }
  if (il==1 || il==2) {
    if (ic==0) {
      y0=tof_ymin[il][ic];    x0=tof_xmax[il][ic]-tof_trap_width[il];
      y1=-tof_trap_xycut[il]; x1=tof_xmin[il][ic];
      y2=tof_trap_xycut[il];  x2=tof_xmin[il][ic];
      y3=tof_ymax[il][ic];    x3=tof_xmax[il][ic]-tof_trap_width[il];
    }
    else if (ic==ic_max-1) {
      y0=tof_ymin[il][ic];    x0=tof_xmin[il][ic]+tof_trap_width[il];
      y1=-tof_trap_xycut[il]; x1=tof_xmax[il][ic];
      y2=tof_trap_xycut[il];  x2=tof_xmax[il][ic];
      y3=tof_ymax[il][ic];    x3=tof_xmin[il][ic]+tof_trap_width[il];
    }
    if (y<0) {
      a=(x1-x0)/(y1-y0);
      b=-a*y0+x0;
    }
    else {
      a=(x3-x2)/(y3-y2);
      b=-a*y2+x2;
    }
    x_calc=a*y+b;
  }
  if (il==2) {
    ic_max=10;
  }
  else{
    ic_max=8;
  }
  if (ic==0 || ic==ic_max-1) {
    if (il==0 || il==3) {
      if (-tof_trap_xycut[il] <= x && x < tof_trap_xycut[il]) {
	if(tof_ymin[il][ic]+delta <= y && y < tof_ymax[il][ic]-delta) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else if ((-tof_trap_xycut[il] > x && x >= tof_xmin[il][ic]+delta) ||
	       (tof_trap_xycut[il] <= x && x < tof_xmax[il][ic]-delta)) {
	if( ic==0 && y_calc+delta <= y && y < tof_ymax[il][ic]-delta) {
	  return true;
	}
	else if(ic==ic_max-1 && tof_ymin[il][ic]+delta<= y && y < y_calc-delta) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	return false;
      }
    }
    else if (il==1 || il==2) {
      if (-tof_trap_xycut[il] <= y && y < tof_trap_xycut[il]) {
	if(tof_xmin[il][ic]+delta <= x && x < tof_xmax[il][ic]-delta) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else if ((-tof_trap_xycut[il] > y && y >= tof_ymin[il][ic]+delta) ||
	       (tof_trap_xycut[il] <= y && y < tof_ymax[il][ic]-delta)) {
	if(ic==0 && x_calc+delta <= x && x < tof_xmax[il][ic]-delta) {
	  return true;
	}
	else if(ic==ic_max-1 && tof_xmin[il][ic]+delta <= x && x < x_calc-delta) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	return false;
      }
    }
  }
  else {
    if ((tof_xmin[il][ic]+delta <= x) && (x < tof_xmax[il][ic]-delta) &&
	(tof_ymin[il][ic]+delta <= y) && (y < tof_ymax[il][ic]-delta)) {
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}


float TofGeometry::LengthInsideCounter(int il, int ib, float x, float y, AMSDir dir) {
  float tanxz = dir.x()/dir.z();
  float tanyz = dir.y()/dir.z();
  // point in the middle of the counter
  AMSPoint p_mid = AMSPoint(x,y,tof_z[il][ib]);
  // point in the input surface of the counter
  AMSPoint p_in;
  p_in = AMSPoint(x-tanxz*0.5,y-tanyz*0.5,tof_z[il][ib]-0.5); //*dir.z());
  // point in the exit surface of the counter
  AMSPoint p_out;
  p_out = AMSPoint(x+tanxz*0.5,y+tanyz*0.5,tof_z[il][ib]+0.5); //*dir.z());
  // p_in within counter
  bool is_p_in=HitCounter(p_in.x(),p_in.y(),il,ib,0);
  // p_out within counter
  bool is_p_out=HitCounter(p_out.x(),p_out.y(),il,ib,0);
 
  // printf("TofGeometry-V %10.5f %10.5f %10.5f %10.5f %10.5f %10.5f  dist=%10.5f\n",p_in.x(),p_in.y(),p_in.z(),p_out.x(),p_out.y(),p_out.z(),(p_out-p_in).norm()); 
  
  if(!is_p_in) {
    // normal to the lateral surface
    AMSDir n = AMSDir(1.-transverse[il],transverse[il],0);
    // mid point of the lateral surface at min
    AMSPoint p_0 = (transverse[il]) ? AMSPoint(0,tof_ymin[il][ib],tof_z[il][ib]) : AMSPoint(tof_xmin[il][ib],0,tof_z[il][ib]);
    // intercept on the lateral surface at min
    AMSPoint diff=p_0-p_mid;
    // d = {(\mathbf{p_0}-\mathbf{l_0})\cdot\mathbf{n} \over \mathbf{l}\cdot\mathbf{n}} 
    float d = diff.prod(n)/dir.prod(n);
    p_in = dir*d+p_mid;
    // printf("new p_in %f %f %f \n",p_in.x(),p_in.y(),p_in.z());
    if(fabs(p_in.z()-tof_z[il][ib])>0.5) {
      // mid point of the lateral surface at max
      AMSPoint p_0= (transverse[il]) ? AMSPoint(0,tof_ymax[il][ib],tof_z[il][ib]) : AMSPoint(tof_xmax[il][ib],0,tof_z[il][ib]);
      // intercept on the lateral surface at max
      AMSPoint diff=p_0-p_mid;
      float d=diff.prod(n)/dir.prod(n);
      // d = {(\mathbf{p_0}-\mathbf{l_0})\cdot\mathbf{n} \over \mathbf{l}\cdot\mathbf{n}} 
      p_in=dir*d+p_mid;
      // printf("new p_in %f %f %f \n",p_in.x(),p_in.y(),p_in.z());
      if(fabs(p_in.z()-tof_z[il][ib])>0.5) return 0;
    }
  }
  if(!is_p_out) {
    // normal to the lateral surface
    AMSDir n=AMSDir(1.-transverse[il],transverse[il],0);
    // mid point of the lateral surface at min
    AMSPoint p_0= (transverse[il]) ? AMSPoint(0,tof_ymin[il][ib],tof_z[il][ib]) : AMSPoint(tof_xmin[il][ib],0,tof_z[il][ib]);
    // intercept on the lateral surface at min
    AMSPoint diff=p_0-p_mid;
    // d = {(\mathbf{p_0}-\mathbf{l_0})\cdot\mathbf{n} \over \mathbf{l}\cdot\mathbf{n}} 
    float d=diff.prod(n)/dir.prod(n);
    p_out=dir*d+p_mid;
    // printf("new p_out %f %f %f \n",p_out.x(),p_out.y(),p_out.z());
    if(fabs(p_out.z()-tof_z[il][ib])>0.5) {
      // mid point of the lateral surface at max
      AMSPoint p_0= (transverse[il]) ? AMSPoint(0,tof_ymax[il][ib],tof_z[il][ib]) : AMSPoint(tof_xmax[il][ib],0,tof_z[il][ib]);
      // intercept on the lateral surface at max
      AMSPoint diff=p_0-p_mid;
      float d=diff.prod(n)/dir.prod(n);
      // d = {(\mathbf{p_0}-\mathbf{l_0})\cdot\mathbf{n} \over \mathbf{l}\cdot\mathbf{n}} 
      p_out=dir*d+p_mid;
      // printf("new p_out %f %f %f \n",p_in.x(),p_out.y(),p_out.z());
      if(fabs(p_out.z()-tof_z[il][ib])>0.5) return 0;
    }
  }
  // printf("il,ib,tof_xmin[il][ib],tof_xmax[il][ib],tof_ymin[il][ib],tof_ymax[il][ib],tof_z[il][ib],dir.z %d %d %f %f %f %f %f %f\n",il,ib,tof_xmin[il][ib],tof_xmax[il][ib],tof_ymin[il][ib],tof_ymax[il][ib],tof_z[il][ib],dir.z());
  AMSPoint length=p_out - p_in;
  // printf("p_in,p_out,length.norm %f %f %f %f %f %f %f \n",p_in.x(),p_in.y(),p_in.z(),p_out.x(),p_out.y(),p_out.z(),length.norm());
  return length.norm();
}


void TofGeometry::DrawTof(int il) {
  TLine *line[10][6];
  int ic_max;
  float x0; float x1; float x2; float x3;
  float y0; float y1; float y2; float y3;
  if (il==2) {ic_max=10;}else{ic_max=8;}
  for (int ic=0; ic<ic_max; ic++) {
    if (ic==0 || ic==ic_max-1) {
      if (ic==0 && (il==0 || il==3)) {
	line[ic][0]= new TLine(tof_xmin[il][ic],tof_ymax[il][ic],tof_xmax[il][ic],tof_ymax[il][ic]);
	line[ic][0]->Draw("SAME");
	line[ic][0]->SetLineColor(3);
	line[ic][0]->SetLineWidth(1);
	line[ic][1]= new TLine(tof_xmin[il][ic],tof_ymax[il][ic]-tof_trap_width[il],tof_xmin[il][ic],tof_ymax[il][ic]);
	line[ic][1]->Draw("SAME");
	line[ic][1]->SetLineColor(3);
	line[ic][1]->SetLineWidth(1);
	line[ic][2]= new TLine(tof_xmax[il][ic],tof_ymax[il][ic]-tof_trap_width[il],tof_xmax[il][ic],tof_ymax[il][ic]);
	line[ic][2]->Draw("SAME");
	line[ic][2]->SetLineColor(3);
	line[ic][2]->SetLineWidth(1);
	x0=tof_xmin[il][ic];    y0=tof_ymax[il][ic]-tof_trap_width[il];
	x1=-tof_trap_xycut[il]; y1=tof_ymin[il][ic];
	x2=tof_trap_xycut[il];  y2=tof_ymin[il][ic];
	x3=tof_xmax[il][ic];    y3=tof_ymax[il][ic]-tof_trap_width[il];
      }else if (ic==ic_max-1 && (il==0 || il==3)) {
	line[ic][0]= new TLine(tof_xmin[il][ic],tof_ymin[il][ic],tof_xmax[il][ic],tof_ymin[il][ic]);
	line[ic][0]->Draw("SAME");
	line[ic][0]->SetLineColor(3);
	line[ic][0]->SetLineWidth(1);
	line[ic][1]= new TLine(tof_xmin[il][ic],tof_ymin[il][ic],tof_xmin[il][ic],tof_ymin[il][ic]+tof_trap_width[il]);
	line[ic][1]->Draw("SAME");
	line[ic][1]->SetLineColor(3);
	line[ic][1]->SetLineWidth(1);
	line[ic][2]= new TLine(tof_xmax[il][ic],tof_ymin[il][ic],tof_xmax[il][ic],tof_ymin[il][ic]+tof_trap_width[il]);
	line[ic][2]->Draw("SAME");
	line[ic][2]->SetLineColor(3);
	line[ic][2]->SetLineWidth(1);
	x0=tof_xmin[il][ic];    y0=tof_ymin[il][ic]+tof_trap_width[il];
	x1=-tof_trap_xycut[il]; y1=tof_ymax[il][ic];
	x2=tof_trap_xycut[il];  y2=tof_ymax[il][ic];
	x3=tof_xmax[il][ic];    y3=tof_ymin[il][ic]+tof_trap_width[il];
      }else if (ic==0 && (il==1 || il==2)) {
	line[ic][0]= new TLine(tof_xmax[il][ic],tof_ymin[il][ic],tof_xmax[il][ic],tof_ymax[il][ic]);
	line[ic][0]->Draw("SAME");
	line[ic][0]->SetLineColor(3);
	line[ic][0]->SetLineWidth(1);
	line[ic][1]= new TLine(tof_xmax[il][ic]-tof_trap_width[il],tof_ymin[il][ic],tof_xmax[il][ic],tof_ymin[il][ic]);
	line[ic][1]->Draw("SAME");
	line[ic][1]->SetLineColor(3);
	line[ic][1]->SetLineWidth(1);
	line[ic][2]= new TLine(tof_xmax[il][ic]-tof_trap_width[il],tof_ymax[il][ic],tof_xmax[il][ic],tof_ymax[il][ic]);
	line[ic][2]->Draw("SAME");
	line[ic][2]->SetLineColor(3);
	line[ic][2]->SetLineWidth(1);
	y0=tof_ymin[il][ic];    x0=tof_xmax[il][ic]-tof_trap_width[il];
	y1=-tof_trap_xycut[il]; x1=tof_xmin[il][ic];
	y2=tof_trap_xycut[il];  x2=tof_xmin[il][ic];
	y3=tof_ymax[il][ic];    x3=tof_xmax[il][ic]-tof_trap_width[il];
      }else if (ic==ic_max-1 && (il==1 || il==2)) {
	line[ic][0]= new TLine(tof_xmin[il][ic],tof_ymin[il][ic],tof_xmin[il][ic],tof_ymax[il][ic]);
	line[ic][0]->Draw("SAME");
	line[ic][0]->SetLineColor(3);
	line[ic][0]->SetLineWidth(1);
	line[ic][1]= new TLine(tof_xmin[il][ic],tof_ymin[il][ic],tof_xmin[il][ic]+tof_trap_width[il],tof_ymin[il][ic]);
	line[ic][1]->Draw("SAME");
	line[ic][1]->SetLineColor(3);
	line[ic][1]->SetLineWidth(1);
	line[ic][2]= new TLine(tof_xmin[il][ic],tof_ymax[il][ic],tof_xmin[il][ic]+tof_trap_width[il],tof_ymax[il][ic]);
	line[ic][2]->Draw("SAME");
	line[ic][2]->SetLineColor(3);
	line[ic][2]->SetLineWidth(1);
	y0=tof_ymin[il][ic];    x0=tof_xmin[il][ic]+tof_trap_width[il];
	y1=-tof_trap_xycut[il]; x1=tof_xmax[il][ic];
	y2=tof_trap_xycut[il];  x2=tof_xmax[il][ic];
	y3=tof_ymax[il][ic];    x3=tof_xmin[il][ic]+tof_trap_width[il];
      }
      line[ic][3]= new TLine(x0,y0,x1,y1);
      line[ic][3]->Draw("SAME");
      line[ic][3]->SetLineColor(3);
      line[ic][3]->SetLineWidth(1);
      line[ic][4]= new TLine(x1,y1,x2,y2);
      line[ic][4]->Draw("SAME");
      line[ic][4]->SetLineColor(3);
      line[ic][4]->SetLineWidth(1);
      line[ic][5]= new TLine(x2,y2,x3,y3);
      line[ic][5]->Draw("SAME");
      line[ic][5]->SetLineColor(3);
      line[ic][5]->SetLineWidth(1);
    } else {
      line[ic][0]= new TLine(tof_xmin[il][ic],tof_ymin[il][ic],tof_xmax[il][ic],tof_ymin[il][ic]);
      line[ic][0]->Draw("SAME");
      line[ic][0]->Draw("SAME");
      line[ic][0]->SetLineColor(3);
      line[ic][0]->SetLineWidth(1);
      line[ic][1]= new TLine(tof_xmax[il][ic],tof_ymin[il][ic],tof_xmax[il][ic],tof_ymax[il][ic]);
      line[ic][1]->Draw("SAME");
      line[ic][1]->SetLineColor(3);
      line[ic][1]->SetLineWidth(1);
      line[ic][2]= new TLine(tof_xmax[il][ic],tof_ymax[il][ic],tof_xmin[il][ic],tof_ymax[il][ic]);
      line[ic][2]->Draw("SAME");
      line[ic][2]->SetLineColor(3);
      line[ic][2]->SetLineWidth(1);
      line[ic][3]= new TLine(tof_xmin[il][ic],tof_ymax[il][ic],tof_xmin[il][ic],tof_ymin[il][ic]);
      line[ic][3]->Draw("SAME");
      line[ic][3]->SetLineColor(3);
      line[ic][3]->SetLineWidth(1);
    }
  }
}


AMSPoint TofGeometry::GetPaddleCorner(int il, int ib, int ix, int iy, int iz) {
  if ( (il<0)||(il>3)||(ib<0)||(ib>9) ) return AMSPoint(-100,-100,-100);
  float x = (ix==0) ? tof_xmin[il][ib] : tof_xmax[il][ib];
  float y = (iy==0) ? tof_ymin[il][ib] : tof_ymax[il][ib];
  float z = tof_z[il][ib] - 0.5 + iz;
  return AMSPoint(x,y,z);
}


AMSPoint TofGeometry::GetPaddleSurfaceCorner(int il, int ib, int isurface, int icorner) {
  if ( (il<0)||(il>3)||(ib<0)||(ib>9) ) return AMSPoint(-100,-100,-100);
  if ( (isurface<0)||(isurface>5)||(icorner<0)||(icorner>3) ) return AMSPoint(-100,-100,-100);
  int a = 0;
  int b = 0;
  switch (icorner) {
    case 0:
      a = 0; b = 0;
      break;
    case 1:
      a = 0; b = 1;
      break;
    case 2:
      a = 1; b = 1;
      break;
    case 3:
      a = 1; b = 0;
      break;
  }
  switch (isurface) {
    case 0:
      return GetPaddleCorner(il,ib,0,a,b);
      break;
    case 1:
      return GetPaddleCorner(il,ib,1,a,b);
      break;
    case 2:
      return GetPaddleCorner(il,ib,a,0,b);
      break;
    case 3:
      return GetPaddleCorner(il,ib,a,1,b);
      break;
    case 4:
      return GetPaddleCorner(il,ib,a,b,0);
      break;
    case 5:
      return GetPaddleCorner(il,ib,a,b,1);
      break;
  }
  return AMSPoint(-100,-100,-100);
}


bool TofGeometry::IsInsidePlane(AMSPoint x[3], AMSPoint point) {
  float x0 = x[0].x(); float y0 = x[0].y(); float z0 = x[0].z();
  float x1 = x[1].x(); float y1 = x[1].y(); float z1 = x[1].z();
  float x2 = x[2].x(); float y2 = x[2].y(); float z2 = x[2].z();
  float a = (y2 - y0)*(z1 - z0) - (z2 - z0)*(y1 - y0);
  float b = (z2 - z0)*(x1 - x0) - (x2 - x0)*(z1 - z0);
  float c = (x2 - x0)*(y1 - y0) - (y2 - y0)*(x1 - x0);
  float d = - a*x0 - b*y0 - c*z0;
  float result = a*point.x() + b*point.y() + c*point.z() + d;
  return (fabs(result)<0.01); // .1 mm
}


bool TofGeometry::IsInsideRectangle(AMSPoint x[4], AMSPoint point) {
  /* from http://paulbourke.net/geometry/insidepoly/
   * To determine whether a point is on the interior of a convex polygon in 3D one might 
   * be tempted to first determine whether the point is on the plane, then determine it's
   * interior status. Both of these can be accomplished at once by computing the sum of 
   * the angles between the test point (q below) and every pair of edge points p[i]->p[i+1]. 
   * This sum will only be 2pi if both the point is on the plane of the polygon AND on the 
   * interior. The angle sum will tend to 0 the further away from the polygon point q becomes.
   */
  // prescription: the points passed should be spatially ordered
  float angle = 0.;
  for (int i=0; i<4; i++) {
    AMSPoint point0 = x[i];
    AMSPoint point1 = (i<3) ? x[i+1] : x[0];
    AMSPoint diff0  = point0 - point;
    AMSPoint diff1  = point1 - point;
    if ( (diff0.norm()<0.01) || (diff1.norm()<0.01) ) return true; // the point requested is on a vertex at .1 mm level 
    float costheta = (diff0.x()*diff1.x() + diff0.y()*diff1.y() + diff0.z()*diff1.z()) / (diff0.norm()*diff1.norm());
    angle += acos(costheta);
  }
  return (fabs(angle-2*3.1415926536)<0.001); // 1 mrad tolerance
}


bool TofGeometry::IsInsideSurface(int il, int ib, int isurface, AMSPoint point) {
  if ( (il<0)||(il>3)||(ib<0)||(ib>9) ) return false;
  if ( (isurface<0)||(isurface>5) ) return false;
  AMSPoint x[4] = {
    TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,0),
    TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,1),
    TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,2),
    TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,3)
  };
  // if (!IsInsidePlane(x,point)) return false; // no longer needed
  if (!IsInsideRectangle(x,point)) return false;
  return true;
}


AMSPoint TofGeometry::GetIntersection(AMSPoint x[3], AMSPoint point, AMSDir dir) {
  // plane equation, ax + by + cz + d = 0
  float x0 = x[0].x(); float y0 = x[0].y(); float z0 = x[0].z();
  float x1 = x[1].x(); float y1 = x[1].y(); float z1 = x[1].z();
  float x2 = x[2].x(); float y2 = x[2].y(); float z2 = x[2].z();
  float a = (y2 - y0)*(z1 - z0) - (z2 - z0)*(y1 - y0);
  float b = (z2 - z0)*(x1 - x0) - (x2 - x0)*(z1 - z0);
  float c = (x2 - x0)*(y1 - y0) - (y2 - y0)*(x1 - x0);
  float d = - a*x0 - b*y0 - c*z0;
  // parametric line (x,y,z) = (A,B,C)*t + (X,Y,Z) 
  float A = dir.x();   float B = dir.y();   float C = dir.z();
  float X = point.x(); float Y = point.y(); float Z = point.z();
  // intersection  
  if (fabs(a*A + b*B + c*C)<1e-6) return AMSPoint(-300,-300,-300);
  float t = - (a*X + b*Y + c*Z + d) / (a*A + b*B + c*C);
  return AMSPoint(A*t + X, B*t + Y, C*t + Z);
}


float TofGeometry::PathLengthInAPaddle(int il, int ib, AMSPoint point, AMSDir dir) {
  if ( (il<0)||(il>3)||(ib<0)||(ib>9) ) return false;
  // check intersection with every surface 
  AMSPoint points[6];
  int      npoints = 0;
  for (int isurface=0; isurface<6; isurface++) {
    AMSPoint x[4] = {
      TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,0),
      TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,1),
      TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,2),
      TofGeometry::GetPaddleSurfaceCorner(il,ib,isurface,3)
    };
    AMSPoint intersection = GetIntersection(x,point,dir);
    if (!IsInsideRectangle(x,intersection)) continue;
    points[npoints].setp(intersection.x(),intersection.y(),intersection.z());
    npoints++;
  }
  // in the case of 2 intersection the particle is entering and exiting 
  if (npoints!=2) return 0.;
  return (points[1] - points[0]).norm();
}

