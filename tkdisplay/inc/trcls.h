// $Id: trcls.h,v 1.1 2010/01/18 11:17:00 shaino Exp $
//
// TrCls : a class to manage Tracker cluster data by SH
//
#ifndef TRCLS_H
#define TRCLS_H

#include <vector>
#include <map>

class AMSEventR;

class TrCls {

public:
  enum EMode{ RAW = 1, CLS = 2 };

  TrCls(EMode mode) : rEvent(0), cMode(mode) {}
 ~TrCls() {}

  EMode getMode() const { return cMode; }

  void newEvent(AMSEventR *evt);
  AMSEventR *getEvent() const { return rEvent; }

  int   nTkLayer(int layer)        const;
  int   iTkLayer(int layer, int i) const;
  int   nClsTkId(int tkid, int side);//        const;
  int   iClsTkId(int tkid, int side, int i);// const;

  int   nCluster  () const;
  bool  pCluster  (int icls) const;
  int   getTkId   (int icls) const;
  int   getLayer  (int icls) const;
  int   getNelem  (int icls) const;
  int   getSide   (int icls) const;
  int   getAddress(int icls, int i) const;
  int   getStatus (int icls, int i) const;
  float getSignal (int icls, int i) const;
  float getSigma  (int icls, int i) const;

private:
  AMSEventR *rEvent;
  EMode      cMode;

  enum { MAXLAY = 8 };
  std::vector<int> tkLayer[MAXLAY];
  std::map< int, std::vector<int> > clsTkId[2];

  typedef std::vector<int>::iterator ITtkVec;
  typedef std::map< int, std::vector<int> >::iterator ITclsMap;
};

#endif
