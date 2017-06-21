#ifndef CLIP_FILE_DATABASE_H
#define CLIP_FILE_DATABASE_H

#include <QVariant>

class DataBase;

class ClipFileDatabase {
 public:
  static ClipFileDatabase* GetInterface(DataBase* db);
  virtual int Release() = 0;

  virtual int AddFile(QString uuid, int start_time, QString path) = 0;

  virtual int RemoveConferenceFile(QString uuid) = 0;

  virtual QVariantList GetFileList(QString uuid) = 0;
  virtual QVariantList GetConferenceFile(QString uuid) = 0;
  virtual QVariantList GetClipFile(QString uuid) = 0;

 protected:
  ClipFileDatabase() {}
  virtual ~ClipFileDatabase() {}
};

#endif  // CLIP_FILE_DATABASE_H
