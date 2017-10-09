#include "account_center.h"

#include "common/config.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QUrl>

AccountCenter::AccountCenter(QObject *parent)
    : QObject(parent), m_pConfig(Config::GetInstance()) {

  QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)),
                   SLOT(requestFinished(QNetworkReply *)),
                   Qt::QueuedConnection);
  QObject::connect(this, SIGNAL(userLogout()), this, SLOT(on_user_logout()),
                   Qt::QueuedConnection);
}

AccountCenter::~AccountCenter() {}

void AccountCenter::UserLogin(QString account, QString password) {
  Config::USER &user = m_pConfig->GetUser();
  user.user_id = account;
  user.user_group =
      "ND"; // 对这个字段暂完明确的目的，接口文档中体现的是该值，暂时写死。

  QJsonObject content_type;

  content_type.insert("login_name", QJsonValue(account));
  content_type.insert("password", QJsonValue(this->encryptMD5_Salt(password)));
  content_type.insert("org_name", ORG_NAME);
  if (!_session.session_id.isEmpty())
    content_type.insert("session_id", QJsonValue(_session.session_id));
  QJsonDocument Jdocument(content_type);

  this->doRequest(AccountCenter::POST, AccountCenter::Login,
                  Jdocument.toJson());
}

void AccountCenter::UserLogout() { emit userLogout(); }

void AccountCenter::on_user_logout() {
  this->doRequest(AccountCenter::DELETE, AccountCenter::Logout, QByteArray());
}
void AccountCenter::requestFinished(QNetworkReply *reply) {
  QJsonParseError error;
  QJsonDocument jsonDocument =
      QJsonDocument::fromJson(reply->readAll(), &error);

  if (error.error == QJsonParseError::NoError) {
    if (jsonDocument.isObject()) {

      bool isError(false);
      if (reply->error() == QNetworkReply::NoError) {

        isError = false;
      } else {
        qDebug() << "htpp request error:" << reply->errorString();
        isError = true;
      }

      this->parseResult(isError, currentRequest.value(reply, -1), jsonDocument);
    }
  } else {
    qDebug() << "json command parse failed" << error.errorString();
  }

  currentRequest.remove(reply);
  reply->deleteLater();
}

QUrl AccountCenter::commandToUrl(int command) {
  QUrl uri;
  QString url;
  Config::USER &user = m_pConfig->GetUser();

  switch (command) {
  case Session:
    url = QString("/v0.93/session");
    break;
  case IdentifyCode:
    url = QString("/v0.93/session/%1/identify_code").arg(_session.session_id);
    break;
  case ValidIdentifyCode:
    url = QString("/v0.93/session/%1/identify_code/action/valid")
              .arg(_session.session_id);
    break;

  case Smses:
    url = QString("/v0.93/smses");
    break;
  case Login:
    url = QString("/v0.93/tokens");
    break;
  case Logout:
    url = QString("/v0.93/tokens/%1").arg(user.access_token);
    break;
  case UserInfo:
    url = QString("/v0.93/users/%1?realm=uc.sdp.nd").arg(user.user_id);
    this->encryptAuthorization(AccountCenter::GET, url);
    break;
  }

  uri.setUrl(PROTOCOL + HOST + url);

  return uri;
}

void AccountCenter::doRequest(int method, int command, const QByteArray &data) {
  QNetworkRequest request(this->commandToUrl(command));
  request.setRawHeader("Accept", "application/json");
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    QVariant("application/json"));
  request.setHeader(QNetworkRequest::ContentLengthHeader,
                    QVariant(data.size()));
  request.setRawHeader("Authorization", _authorization.getAuthorization());

  QNetworkReply *reply;
  switch (method) {
  case POST:
    reply = manager.post(request, data);
    break;
  case GET:
    reply = manager.get(request);
    break;
  case DELETE:
    reply = manager.deleteResource(request);
    break;
  }

  currentRequest.insert(reply, command);
}

void AccountCenter::parseResult(bool error, int command,
                                const QJsonDocument &jsDoc) {
  QString reason;     // error message
  QJsonObject jsRoot; //

  switch (command) {
  case Session:
    // qDebug()<<jsonDocument;
    jsRoot = jsDoc.object();

    if (!error) {
      _session.op_count = jsRoot.value("op_count").toInt();
      _session.session_id = jsRoot.value("session_id").toString();
      _session.session_key = jsRoot.value("session_key").toString();
    } else {
      reason = jsRoot.value("message").toString();
    }
    break;
  case IdentifyCode:
    if (!error) {
      QByteArray image = jsDoc.toBinaryData();
    }
    break;
  case ValidIdentifyCode:

    break;
  case Smses:

    break;
  case Login:

    jsRoot = jsDoc.object();
    if (!error) {
      Config::USER &user = m_pConfig->GetUser();
      user.access_token = jsRoot.value("access_token").toString();
      user.mac_key = jsRoot.value("mac_key").toString();

      this->doRequest(GET, UserInfo, QByteArray());
    } else {
      reason = jsRoot.value("message").toString();
      qDebug() << "Login Failed!" << reason;
      emit loginResult(!error, reason);
    }

    break;
  case Logout: {
    jsRoot = jsDoc.object();
    if (!error) {
      // nothing
    } else {
      reason = jsRoot.value("message").toString();
      qDebug() << "Logout Failed!" << reason;
    }

    Config::USER &user = m_pConfig->GetUser();
    user.user_id.clear();
  } break;
  case UserInfo:

    jsRoot = jsDoc.object();
    if (error) {
      reason = jsRoot.value("message").toString();
      qDebug() << "Get UserInfo Failed!" << reason;
    } else {
      Config::USER &user = m_pConfig->GetUser();
      QJsonObject jsRealmExinfo = jsRoot["org_exinfo"].toObject();
      user.user_name = jsRealmExinfo["username"].toString();
    }

    emit loginResult(!error, reason);
    break;
  }
}

void AccountCenter::encryptAuthorization(int method, QString uri) {
  QString httpMethod;
  switch (method) {
  case POST:
    httpMethod = "POST";
    break;
  case GET:
    httpMethod = "GET";
    break;
  case DELETE:
    httpMethod = "DELETE";
    break;
  }

  Config::USER &user = m_pConfig->GetUser();
  _authorization.mac_id = user.access_token;
  _authorization.nonce = this->makeNonce();

  QString mac;
  mac += _authorization.nonce;
  mac += "\n";
  mac += httpMethod;
  mac += "\n";
  mac += uri;
  mac += "\n";
  mac += HOST;
  mac += "\n";
  _authorization.mac = this->hmacSha1(user.mac_key.toUtf8(), mac.toUtf8());

  qDebug() << _authorization.mac;
  qDebug() << _authorization.nonce;
  qDebug() << _authorization.mac_id;
  qDebug() << _authorization.getAuthorization();
}

QString AccountCenter::makeNonce() {
  QString nonce;
  nonce += QString::number(QDateTime::currentMSecsSinceEpoch());
  nonce += ":";
  nonce += this->randString();

  return nonce;
}
QString AccountCenter::randString() {
  QString randString;

  int max = 8;
  QString tmp = QString("0123456789ABCDEFGHIJKLMNOPQRSTUVWZYZ");
  QTime t;
  t = QTime::currentTime();
  qsrand(t.msec() + t.second() * 1000);
  for (int i = 0; i < max; i++) {
    int ir = qrand() % tmp.length();
    randString[i] = tmp.at(ir);
  }

  return randString;
}

QString AccountCenter::encryptMD5_Salt(QString content) {
  QString appkey = "fdjf,jkgfkl";

  QByteArray a = appkey.toUtf8();
  QByteArray datSource = content.toUtf8();
  QByteArray b;
  b.resize(a.length() + 4 + datSource.length());

  int i;
  for (i = 0; i < datSource.length(); i++) {
    b.data()[i] = datSource.constData()[i];
  }

  b.data()[i++] = (char)163;
  b.data()[i++] = (char)172;
  b.data()[i++] = (char)161;
  b.data()[i++] = (char)163;

  for (int k = 0; k < a.length(); k++) {
    b.data()[i] = a.constData()[k];
    i++;
  }

  QByteArray md5;
  md5 = QCryptographicHash::hash(b, QCryptographicHash::Md5);

  return QString(md5.toHex());
}

QString AccountCenter::hmacSha1(QByteArray key, QByteArray baseString) {
  int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
  if (key.length() > blockSize) { // if key is longer than block size (64),
                                  // reduce key length with SHA-1 compression
    key = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
  }

  QByteArray innerPadding(blockSize,
                          char(0x36)); // initialize inner padding with char "6"
  QByteArray outerPadding(
      blockSize, char(0x5c)); // initialize outer padding with char "quot;
  // ascii characters 0x36 ("6") and 0x5c ("quot;) are selected because they
  // have large
  // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

  for (int i = 0; i < key.length(); i++) {
    innerPadding[i] =
        innerPadding[i] ^ key.at(i); // XOR operation between every byte in key
                                     // and innerpadding, of key length
    outerPadding[i] =
        outerPadding[i] ^ key.at(i); // XOR operation between every byte in key
                                     // and outerpadding, of key length
  }

  // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString )
  // ).toBase64
  QByteArray total = outerPadding;
  QByteArray part = innerPadding;
  part.append(baseString);
  total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha256));
  QByteArray hashed =
      QCryptographicHash::hash(total, QCryptographicHash::Sha256);
  return hashed.toBase64();
}
