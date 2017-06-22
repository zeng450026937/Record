#ifndef SERVICETHREAD_H
#define SERVICETHREAD_H

#include <QThread>

class WhiteList;
class MessageBase;
class AccountCenter;
class ConfServiceImpl;
class UserServiceImpl;
class PersonalMode;
class ConferenceMode;
class LoginControl;
class RecorderShared;
class DownloadDatabase;
class ClipFileDatabase;
class MarkDatabase;
class MarkControl;
class RecordControl;

class ServiceThreadPrivate;
class ServiceThread : public QThread {
  Q_OBJECT
 private:
  explicit ServiceThread(QObject *parent = 0);
  ServiceThread(ServiceThread &) {}
  ServiceThread &operator=(ServiceThread *) {}

 public:
  ~ServiceThread();

  static ServiceThread *GetInstance();
  static void Uninstance();

  ConferenceMode *GetConferenceMode();
  PersonalMode *GetPersonalMode();
  LoginControl *GetLoginControl();
  AccountCenter *GetAccountCenter();

  WhiteList *GetLoginWhiteList();
  MessageBase *GetMessager();
  ConfServiceImpl *GetConfService();
  UserServiceImpl *GetUserService();

  RecorderShared *GetRecordShared();

  DownloadDatabase *GetDownloadDB();
  ClipFileDatabase *GetClipDB();

  MarkDatabase *GetMarkDB();
  MarkControl *GetMarkControl();
  RecordControl *GetRecordControl();

  ServiceThreadPrivate *_private;
 signals:
  void service_ready();

 protected:
  void run() override;

 protected:
};

#endif  // SERVICETHREAD_H
