#include "qnetio.h"

#include <iostream>

NetFileEngineIterator::NetFileEngineIterator(QDir::Filters filters, 
					     const QStringList &filterNames)
    : QAbstractFileEngineIterator(filters, filterNames)
{
  dirPtr  = 0;
  dirDone = false;
}

NetFileEngineIterator::~NetFileEngineIterator()
{
}

QString NetFileEngineIterator::next()
{
  if (!hasNext()) return QString();

  advance();
  return currentFilePath();
}

void NetFileEngineIterator::advance()
{
  if (!dirPtr) return;


  const char *str = NetFileEngine::NetSystem()->GetDirEntry(dirPtr);

  if (str) currentEntry = str;
  else {
    NetFileEngine::NetSystem()->FreeDirectory(dirPtr);
    dirPtr  = 0;
    dirDone = true;
  }
}

bool NetFileEngineIterator::hasNext() const
{
  if (!dirDone && !dirPtr) {
    NetFileEngineIterator *that = const_cast<NetFileEngineIterator *>(this);
    that->dirPtr = NetFileEngine::NetSystem()
        ->OpenDirectory(QFile::encodeName(path()).data());

    if (!dirPtr) that->dirDone = true;
    else that->advance();
  }

  return !dirDone;
}

TNetSystem *NetFileEngine::netSystem = 0;
QString     NetFileEngine::netUrl;
QString     NetFileEngine::netDir;

std::map<std::string,FileStat_t> NetFileEngine::statMap;

TNetSystem *NetFileEngine::NetSystem(void)
{
  if (!netSystem) {
    std::cout<<"Init TNetSystem: "<<netUrl.toAscii().data()<<std::endl;
    netSystem = new TNetSystem(netUrl.toAscii().data());
  }
  return netSystem;
}

QAbstractFileEngine::FileFlags
NetFileEngine::fileFlags(QAbstractFileEngine::FileFlags type) const
{
  QAbstractFileEngine::FileFlags ret = 0;

  QString aname = fileName(AbsoluteName);
  if (aname[0] != '/') return ret;

  std::string path = aname.toAscii().data();
  for (int i = 1; 0 < i && i < aname.size(); i++) {
    i = aname.indexOf('/', i);
    std::string pchk = aname.left(i).toAscii().data();
    statIT it = statMap.find(pchk);
    if (it != statMap.end() && it->second.fMtime == 0) return ret;
  }

  FileStat_t st;
  statIT it = statMap.find(path);

  if (it == statMap.end()) {
    NetFileEngine *that = const_cast<NetFileEngine *>(this);
    NetSystem()->GetPathInfo(path.c_str(), st);
    that->statMap[path] = st;
  }
  else st = it->second;

  if (st.fMtime == 0) return ret;

  if (type & PermsMask) {
    if (st.fMode & kS_IRUSR) ret |= ReadOwnerPerm;
    if (st.fMode & kS_IWUSR) ret |= WriteOwnerPerm;
    if (st.fMode & kS_IXUSR) ret |= ExeOwnerPerm;
    if (st.fMode & kS_IRUSR) ret |= ReadUserPerm;
    if (st.fMode & kS_IWUSR) ret |= WriteUserPerm;
    if (st.fMode & kS_IXUSR) ret |= ExeUserPerm;
    if (st.fMode & kS_IRGRP) ret |= ReadGroupPerm;
    if (st.fMode & kS_IWGRP) ret |= WriteGroupPerm;
    if (st.fMode & kS_IXGRP) ret |= ExeGroupPerm;
    if (st.fMode & kS_IROTH) ret |= ReadOtherPerm;
    if (st.fMode & kS_IWOTH) ret |= WriteOtherPerm;
    if (st.fMode & kS_IXOTH) ret |= ExeOtherPerm;
  }

  if (type & TypesMask) {
    if ((type & LinkType) && st.fIsLink) ret |= LinkType;
    if      ((st.fMode & kS_IFMT) == kS_IFREG) ret |= FileType;
    else if ((st.fMode & kS_IFMT) == kS_IFDIR) ret |= DirectoryType;
  }

  if (type & FlagsMask) {
    ret |= LocalDiskFlag;
    ret |= ExistsFlag;
    if (fileName(BaseName)[0] == QLatin1Char  ('.')) ret |= HiddenFlag;
    if (fileName(PathName)    == QLatin1String("/")) ret |= RootFlag;
  }

  return ret;
}

bool NetFileEngineHandler::isEnabled = false;

QAbstractFileEngine *NetFileEngineHandler::
create(const QString &fileName) const 
{
  if (!isEnabled) return 0;
  if (!fileName.startsWith('/'))   return 0;
  if ( fileName.endsWith(".png"))  return 0;
  if ( fileName.endsWith(".pfa"))  return 0;
  if ( fileName.endsWith(".conf")) return 0;

  return new NetFileEngine(fileName);
}


