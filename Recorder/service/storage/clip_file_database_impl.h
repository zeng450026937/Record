#ifndef CLIPFILEDATABASEIMPL_H
#define CLIPFILEDATABASEIMPL_H

#include "include/clip_file_database.h"

class SharedData;

class ClipFileDatabaseImpl : public ClipFileDatabase {
 public:
  int AddFile(QString uuid, int start_time, QString path) override;

  int RemoveConferenceFile(QString uuid) override;

  QVariantList GetFileList(QString uuid) override;
  QVariantList GetConferenceFile(QString uuid) override;
  QVariantList GetClipFile(QString uuid) override;

 protected:
  ClipFileDatabaseImpl(SharedData* shared) : _shared(NULL) { _shared = shared; }
  ~ClipFileDatabaseImpl() override {}

 private:
  SharedData* _shared;
};

#endif  // CLIPFILEDATABASEIMPL_H
