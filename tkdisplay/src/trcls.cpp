// $Id: trcls.cpp,v 1.1 2010/01/18 11:17:00 shaino Exp $

#include "trcls.h"
#include "root.h"

void TrCls::newEvent(AMSEventR *evt)
{
  rEvent = evt;
  if (!rEvent) return;

  clsTkId[0].clear();
  clsTkId[1].clear();
  for (int i = 0; i < MAXLAY; i++) tkLayer[i].clear();

  for (int i = 0; i < nCluster(); i++) {
    if (!pCluster(i)) continue;

    int ilyr = getLayer(i)-1;
    int tkid = getTkId(i);
    clsTkId[getSide(i)][tkid].push_back(i);

    if (std::find(tkLayer[ilyr].begin(), tkLayer[ilyr].end(), tkid)
	== tkLayer[ilyr].end()) tkLayer[ilyr].push_back(tkid);
  }

  for (int i = 0; i < MAXLAY; i++)
    std::sort(tkLayer[i].begin(), tkLayer[i].end());
}

int TrCls::nTkLayer(int layer) const
{
  return (1 <= layer && layer <= MAXLAY) ? tkLayer[layer-1].size() : 0;
}

int TrCls::iTkLayer(int layer, int i) const
{
  return (0 <= i && i < nTkLayer(layer)) ? tkLayer[layer-1].at(i) : 0;
}

int TrCls::nClsTkId(int tkid, int side)// const
{
  if (side != 0 && side != 1) return 0;
  ITclsMap it = clsTkId[side].find(tkid);
  return (it != clsTkId[side].end()) ? it->second.size() : 0;
}

int TrCls::iClsTkId(int tkid, int side, int i)//const
{
  if (side != 0 && side != 1) return 0;
  ITclsMap it = clsTkId[side].find(tkid);
  return (it != clsTkId[side].end() && 
	  0 <= i && i < it->second.size()) ? it->second.at(i) : 0;
}


int TrCls::nCluster() const
{
  if (!rEvent) return 0;
  return (cMode == RAW) ? rEvent->nTrRawCluster() 
                        : rEvent->nTrCluster();
}

bool TrCls::pCluster(int icls) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return false;

  return (cMode == RAW) ? ((TrElem*)rEvent->pTrRawCluster(icls))
                        : ((TrElem*)rEvent->pTrCluster   (icls));
}

int TrCls::getTkId(int icls) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;

  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetTkId()
                        : rEvent->pTrCluster   (icls)->GetTkId();
}

int TrCls::getLayer(int icls) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;

  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetLayer()
                        : rEvent->pTrCluster   (icls)->GetLayer();
}

int TrCls::getNelem(int icls) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;
  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetNelem()
                        : rEvent->pTrCluster   (icls)->GetNelem();
}

int TrCls::getSide(int icls) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;

  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetSide()
                        : rEvent->pTrCluster   (icls)->GetSide();
}

int TrCls::getAddress(int icls, int i) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;

  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetAddress(i)
                        : rEvent->pTrCluster   (icls)->GetAddress(i);
}

int TrCls::getStatus(int icls, int i) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;

  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetStatus(i)
                        : rEvent->pTrCluster   (icls)->GetStatus(i);
}

float TrCls::getSignal(int icls, int i) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;

  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetSignal(i)
                        : rEvent->pTrCluster   (icls)->GetSignal(i);
}

float TrCls::getSigma(int icls, int i) const
{
  if (!rEvent || icls < 0 || nCluster() <= icls) return 0;

  return (cMode == RAW) ? rEvent->pTrRawCluster(icls)->GetSigma(i)
                        : rEvent->pTrCluster   (icls)->GetSigma(i);
}
