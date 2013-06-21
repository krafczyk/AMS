#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceDAQ() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  if (fAMSEvent->nDaqEvent() != 1) {
    WARN_OUT << "Problem building AC::DAQ: AMS ROOT File is broken! "
             << "nDaqEvent() returned: " << fAMSEvent->nDaqEvent() << " expected: 1." << std::endl;
    return false;
  }

  DaqEventR* pDAQ = fAMSEvent->pDaqEvent(0);
  if (!pDAQ) {
    WARN_OUT << "Problem building AC::DAQ: AMS ROOT File is broken! "
             << "pDaqEvent(0) is null." << std::endl;
    return false;
  }

  fEvent->fDAQ.fL3Errors = pDAQ->L3Error;

  fEvent->fDAQ.fJINJStatus.clear();
  for (int i = 0; i < 4; ++i)
    fEvent->fDAQ.fJINJStatus.append(pDAQ->JINJStatus[i]);

  fEvent->fDAQ.fSlaveStatus.clear();
  for (int i = 0; i < 24; ++i)
    fEvent->fDAQ.fSlaveStatus.append(pDAQ->JError[i]);

  return true;
}
