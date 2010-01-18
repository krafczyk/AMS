// $Id: qnetio.h,v 1.2 2010/01/18 11:35:22 shaino Exp $
//
// QNetIO : a class to manage directory access through TNetSystem
//
#ifndef QNetIO_H
#define QNetIO_H

#include <QtCore/qfsfileengine.h>
#include "TNetFile.h"

#include <map>
#include <string>

class NetFileEngineIterator : public QAbstractFileEngineIterator {

public:
  NetFileEngineIterator(QDir::Filters filters, 
			const QStringList &filterNames);
  ~NetFileEngineIterator();

  QString next();
  bool hasNext() const;
  void advance();

  QString   currentFileName() const { return currentEntry; }
  QFileInfo currentFileInfo() const {
    return QAbstractFileEngineIterator::currentFileInfo();
  }

private:
  void *dirPtr;
  bool  dirDone;
  QString currentEntry;
};


class NetFileEngine : public QFSFileEngine {

public:
  NetFileEngine(const QString &file) : QFSFileEngine(file) {}

  FileFlags fileFlags(FileFlags type) const;

  Iterator *beginEntryList(QDir::Filters filters, 
			   const QStringList &filterNames) {
    return new NetFileEngineIterator(filters, filterNames);
  }

  static FileStat_t getPathInfo(QString str);
  static void    *openDirectory(QString path);
  static void     freeDirectory(void *pdir);
  static QString    getDirEntry(void *pdir);

  static const QString &getUrl(void) { return netUrl; }
  static const QString &getDir(void) { return netDir; }
  static bool setUrl(const QString &url, const QString &dir, bool chk = true);

private:
  static QString netUrl;
  static QString netDir;

  static TNetSystem *netSystem;
  static bool netLock;

  static TNetSystem *NetSystem(void);
  static void clearNetSystem() { delete netSystem; netSystem = 0; }

  static  std::map<std::string,FileStat_t> statMap;
  typedef std::map<std::string,FileStat_t>::const_iterator statIT;

  static bool statLock;
  static const statIT &findStat(QString path);
  static void addStat(QString path, const FileStat_t &st);
};


class NetFileEngineHandler : public QAbstractFileEngineHandler {

public:
  static bool isEnabled;

  static void Enable (bool sw = true)  { isEnabled = sw; }
  static void Disable(bool sw = false) { isEnabled = sw; }

  QAbstractFileEngine *create(const QString &fileName) const;
};

#endif
