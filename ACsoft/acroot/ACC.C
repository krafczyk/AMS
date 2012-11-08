#include "ACC.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_OUTPUT(ACC)
  DUMP(Clusters)
END_DEBUG_OUTPUT

void ACC::Clear(Option_t *) {

  fClusters.clear();
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const ACC& object) {

  stream << object.fClusters;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, ACC& object) {

  stream >> object.fClusters;
  return stream;
}

}
