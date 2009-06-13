// $Id: evthread.h,v 1.1 2009/06/13 21:40:46 shaino Exp $
//
// EvThread : a class to manage event scan in a thread process
//
#ifndef EvThread_H
#define EvThread_H

#include <QThread>
#include <QMutex>

class TTreeFormula;
class AMSChain;
class TkDisplay;

class EvThread : public QThread {
  Q_OBJECT

public:
  EvThread(TkDisplay *parent,
	   AMSChain *ch, TTreeFormula *fm, int ent, int istep);
 ~EvThread();

  int getEntry();

  void rmLock()   { rMutex.lock(); }
  void rmUnLock() { rMutex.unlock(); }

public Q_SLOTS:
  void cancelScan() { cancelFlag = true; }

signals:
  void updateEntry(int, int);

protected:
  void run();

private:
  AMSChain     *amsChain;
  TTreeFormula *tFormula;

  QMutex rMutex;

  int iEntry;
  int iStep;

  bool cancelFlag;
};

#endif
