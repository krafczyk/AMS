// $Id: qnetio.h,v 1.1 2009/12/21 17:41:49 shaino Exp $
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

  static TNetSystem *NetSystem(void);
  static void clearNetSystem() { delete netSystem; netSystem = 0; }

  static const QString &getUrl(void) { return netUrl; }
  static const QString &getDir(void) { return netDir; }
  static void setUrl(const QString &url) { netUrl = url; }
  static void setDir(const QString &dir) { netDir = dir; }

private:
  static QString netUrl;
  static QString netDir;
  static TNetSystem *netSystem;

  static  std::map<std::string,FileStat_t> statMap;
  typedef std::map<std::string,FileStat_t>::const_iterator statIT;
};


class NetFileEngineHandler : public QAbstractFileEngineHandler {

public:
  static bool isEnabled;

  static void Enable (bool sw = true)  { isEnabled = sw; }
  static void Disable(bool sw = false) { isEnabled = sw; }

  QAbstractFileEngine *create(const QString &fileName) const;
};

#endif
