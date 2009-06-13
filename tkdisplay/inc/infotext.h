// $Id: infotext.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// InfoText : a class to manage text information of Tracker objects by SH
//
#ifndef INFOTEXT_H
#define INFOTEXT_H

#include "qstring.h"

class AMSEventR;

class InfoText {

public:
  static QString &EventInfo  (AMSEventR *event);
  static QString &LadderInfo (AMSEventR *event, int tkid);
  static QString &TrackInfo  (AMSEventR *event, int itrk);
  static QString &HitInfo    (AMSEventR *event, int ihit);
  static QString &ClusterInfo(AMSEventR *event, int icls);
};

#endif
