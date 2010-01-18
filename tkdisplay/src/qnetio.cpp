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

  QString str = NetFileEngine::getDirEntry(dirPtr);

  if (str != "") currentEntry = str;
  else {
    NetFileEngine::freeDirectory(dirPtr);
    dirPtr  = 0;
    dirDone = true;
  }
}

bool NetFileEngineIterator::hasNext() const
{
  if (!dirDone && !dirPtr) {
    NetFileEngineIterator *that = const_cast<NetFileEngineIterator *>(this);
    that->dirPtr = NetFileEngine::openDirectory(QFile::encodeName(path()));

    if (!dirPtr) that->dirDone = true;
    else that->advance();
  }

  return !dirDone;
}

QString NetFileEngine::netUrl;
QString NetFileEngine::netDir;


TNetSystem *NetFileEngine::netSystem = 0;
bool NetFileEngine::netLock = false;

TNetSystem *NetFileEngine::NetSystem(void)
{
  if (!netSystem) {
    std::cout << "Init TNetSystem: " << netUrl.toAscii().data() << std::endl;
    netSystem = new TNetSystem(netUrl.toAscii().data());
  }
  return netSystem;
}

FileStat_t NetFileEngine::getPathInfo(QString path)
{
  while (netLock) gSystem->Sleep(1);

  netLock = true;
  FileStat_t st;
  NetSystem()->GetPathInfo(path.toAscii().data(), st);
  netLock = false;

  return st;
}

void *NetFileEngine::openDirectory(QString path)
{
  while (netLock) gSystem->Sleep(1);

  netLock = true;
  void *pdir = NetSystem()->OpenDirectory(path.toAscii().data());
  netLock = false;

  return pdir;
}

void NetFileEngine::freeDirectory(void *pdir)
{
  while (netLock) gSystem->Sleep(1);

  netLock = true;
  NetSystem()->FreeDirectory(pdir);
  netLock = false;
}

QString NetFileEngine::getDirEntry(void *pdir)
{
  if (!pdir) return QString("");

  while (netLock) gSystem->Sleep(1);

  netLock = true;
  QString str = NetSystem()->GetDirEntry(pdir);
  netLock = false;

  return str;
}

bool NetFileEngine::setUrl(const QString &url, const QString &dir, bool chk)
{
  QString urlb = netUrl;
  QString dirb = netDir;

  netUrl = url;
  netDir = dir;

  if (chk) {
    clearNetSystem();

    void *pdir = openDirectory(netDir);
    if (!pdir) {
      clearNetSystem();
      netUrl = urlb;
      netDir = dirb;
      return false;
    }
    freeDirectory(pdir);
  }
  return true;
}

std::map<std::string, FileStat_t> NetFileEngine::statMap;

bool NetFileEngine::statLock = false;

const NetFileEngine::statIT &NetFileEngine::findStat(QString path)
{
  std::string str = path.toAscii().data();

  while (statLock) gSystem->Sleep(1);
  statLock  = true;
  statIT it = statMap.find(str);
  statLock  = false;

  return it;
}

void NetFileEngine::addStat(QString path, const FileStat_t &st)
{
  std::string str = path.toAscii().data();

  while (statLock) gSystem->Sleep(1);
  statLock = true;
  statMap[str] = st;
  statLock = false;
}

QAbstractFileEngine::FileFlags
NetFileEngine::fileFlags(QAbstractFileEngine::FileFlags type) const
{
  QAbstractFileEngine::FileFlags ret = 0;

  QString aname = fileName(AbsoluteName);
  if (aname[0] != '/') return ret;

  for (int i = 1; 0 < i && i < aname.size(); i++) {
    i = aname.indexOf('/', i);
    statIT it = findStat(aname.left(i));
    if (it != statMap.end() && it->second.fMtime == 0) return ret;
  }

  FileStat_t st;
  statIT it = findStat(aname);

  if (it == statMap.end()) {
    st = getPathInfo(aname);
    addStat(aname, st);
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


