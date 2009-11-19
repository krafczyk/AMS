// $Id: evthread.cpp,v 1.2 2009/11/19 10:18:47 shaino Exp $
#include "evthread.h"
#include "tkdisplay.h"

#include "root.h"
#include "amschain.h"
#include "TTreeFormula.h"

EvThread::EvThread(TkDisplay *parent, 
		   AMSChain *ch, TTreeFormula *fm, int ent, int step) 
  : QThread(parent), amsChain(ch), tFormula(fm), iEntry(ent), iStep(step)
{
  cancelFlag = false;
}

EvThread::~EvThread()
{
  while (!isFinished()) msleep(10);
}

int EvThread::getEntry()
{
  while (!isFinished()) msleep(10);
  return iEntry;
}

void EvThread::run()
{
  msleep(100);
  if (!amsChain) return;

  int nent = amsChain->GetEntries();
  if (iStep != -1) iStep = 1;

  for (int i = iEntry+iStep; !cancelFlag && 0 <= i && i < nent; i += iStep) {
    if ((i-iEntry)/iStep > 100 && i%100 == 0) emit updateEntry(i, nent);
    amsChain->GetEntry(i);
    if (!tFormula || tFormula->EvalInstance()) {
      iEntry = i;
      break;
    }
  }
}
