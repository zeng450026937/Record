#include "account_center.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QUrl>
#include <QCryptographicHash>

#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>

AccountCenter::AccountCenter(QObject *parent)
    :QObject(parent)
{

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),
                     SLOT(requestFinished(QNetworkReply*)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(userLogin(QString,QString)),
                     this,SLOT(on_user_login(QString,QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(userLogout()),this,
                     SLOT(on_user_logout()),Qt::QueuedConnection);
}

AccountCenter::~AccountCenter()
{

}

void AccountCenter::UserLogin(QString account, QString password)
{
    emit userLogin(account, password);
}

void AccountCenter::UserLogout()
{
    emit userLogout();
}
void AccountCenter::on_user_login(QString account, QString password)
{
    _user.user_id = account;

    QJsonObject  content_type;

    content_type.insert("login_name",QJsonValue(account));
    content_type.insert("password",QJsonValue(this->encryptMD5_Salt(password)));
    content_type.insert("org_name",QJsonValue(ORG_NAME));
    if(!_session.session_id.isEmpty())
        content_type.insert("session_id",QJsonValue(_session.session_id));
    QJsonDocument Jdocument(content_type);

    this->doRequest(AccountCenter::POST ,AccountCenter::Login, Jdocument.toJson());
}
void AccountCenter::on_user_logout()
{
    this->doRequest(AccountCenter::DELETE, AccountCenter::Logout, QByteArray());
}
void AccountCenter::requestFinished(QNetworkReply* reply)
{
    QJsonParseError error;
    QJsonDocument jsonDocument  = QJsonDocument::fromJson(reply->readAll(), &error);

    if(error.error == QJsonParseError::NoError){
        if(jsonDocument .isObject()){

            bool isError(false);
            if(reply->error() == QNetworkReply::NoError){
                //qDebug()<<reply->url();
                //qDebug()<<reply->rawHeaderPairs();
                isError = false;
            }
            else{
                qDebug()<<reply->errorString();
                isError = true;
            }

            this->parseResult(isError, currentRequest.value(reply,-1), jsonDocument);
        }
    }
    else{
        qDebug()<<"json command parse failed"<<error.errorString();
    }

    currentRequest.remove(reply);
    reply->deleteLater();
}


QUrl AccountCenter::commandToUrl(int command)
{
    QUrl uri;
    QString url;

    switch(command){
    case Session:
        url = QString("/v0.93/session");
        break;
    case IdentifyCode:
        url = QString("/v0.93/session/%1/identify_code").arg(_session.session_id);
        break;
    case ValidIdentifyCode:
        url = QString("/v0.93/session/%1/identify_code/action/valid").arg(_session.session_id);
        break;

    case Smses:
        url = QString("/v0.93/smses");
        break;
    case Login:
        url = QString("/v0.93/tokens");
        break;
    case Logout:
        url = QString("/v0.93/tokens/%1").arg(_user.access_token);
        break;
    case UserInfo:
        url = QString("/v0.93/users/actions/query?realm=xxx");
        this->encryptAuthorization(AccountCenter::POST, url);
        break;
    }

    uri.setUrl(HOST+url);

    return uri;
}

void AccountCenter::doRequest(int method, int command, const QByteArray &data)
{
    QNetworkRequest request(this->commandToUrl(command));
    request.setRawHeader("Accept", "application/json");
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader,QVariant(data.size()));
    request.setRawHeader("Authorization", _authorization.getAuthorization());

    QNetworkReply* reply;

    switch(method){
    case POST:
        reply = manager.post(request,data);
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

void AccountCenter::parseResult(bool error, int command, const QJsonDocument& jsonDocument)
{
    QString reason;//error message
    QVariantMap content;//

    switch(command){
    case Session:
        //qDebug()<<jsonDocument;
        content = jsonDocument.toVariant().toMap();

        if(!error){
            _session.op_count = content.value("op_count").toInt();
            _session.session_id = content.value("session_id").toString();
            _session.session_key = content.value("session_key").toString();
        }
        else{
            reason = content.value("message").toString();
        }
        break;
    case IdentifyCode:
        if(!error){
            QByteArray image =  jsonDocument.toBinaryData();
        }
        break;
    case ValidIdentifyCode:
        //qDebug()<<jsonDocument;
        break;
    case Smses:
        //qDebug()<<jsonDocument;
        break;
    case Login:
        //qDebug()<<jsonDocument;
        content = jsonDocument.toVariant().toMap();

        if(!error){
            _user.access_token = content.value("access_token").toString();
            _user.expires_at = content.value("expires_at").toString();
            _user.mac_algorithm = content.value("mac_algorithm").toString();
            _user.mac_key = content.value("mac_key").toString();
            _user.refresh_token = content.value("refresh_token").toString();
            _user.server_time = content.value("server_time").toString();
            _user.user_id = content.value("user_id").toString();
            _user.warning_code = content.value("warning_code").toString();
        }
        else{
            reason = content.value("message").toString();
        }

        emit loginResult(_user.user_id, !error, reason);

        break;
    case Logout:
        //qDebug()<<jsonDocument;
        content = jsonDocument.toVariant().toMap();

        if(!error){
            //nothing
        }
        else{
            reason = content.value("message").toString();
        }

        emit logoutResult(_user.user_id, !error, reason);
        _user.user_id.clear();
        break;
    case UserInfo:
        //qDebug()<<jsonDocument;
        content = jsonDocument.toVariant().toMap();

        if(!error){
            _user_info.nick_name = content.value("nick_name").toString();
            _user_info.nick_name_full = content.value("nick_name_full").toString();
            _user_info.nick_name_short = content.value("nick_name_short").toString();
            _user_info.realm_exinfo = content.value("realm_exinfo").toString();
            _user_info.region = content.value("region").toInt();
            _user_info.user_id = content.value("user_id").toString();
            _user_info.user_name = content.value("user_name").toString();
        }
        else{
            reason = content.value("message").toString();
        }
        break;
    }
}

void AccountCenter::encryptAuthorization(int method, QString uri)
{
    QString httpMethod;
    switch(method){
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

    _authorization.mac_id = _user.access_token;
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
    _authorization.mac = this->hmacSha1(_user.mac_key.toUtf8(), mac.toUtf8());

    qDebug()<<_authorization.mac;
    qDebug()<<_authorization.nonce;
    qDebug()<<_authorization.mac_id;
    qDebug()<<_authorization.getAuthorization();
}

QString AccountCenter::makeNonce()
{
    QString nonce;
    nonce += QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    nonce += ":";
    nonce += this->randString();

    return nonce;
}
QString AccountCenter::randString()
{
    QString randString;

    int max = 8;
    QString tmp = QString("0123456789ABCDEFGHIJKLMNOPQRSTUVWZYZ");
    QTime t;
    t= QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);
    for(int i=0;i<max;i++) {
        int ir = qrand()%tmp.length();
        randString[i] = tmp.at(ir);
    }

    return randString;
}

QString AccountCenter::encryptMD5_Salt(QString content)
{
    QString appkey = "fdjf,jkgfkl";

    QByteArray a = appkey.toUtf8();
    QByteArray datSource = content.toUtf8();
    QByteArray b;
    b.resize(a.length() + 4 + datSource.length());

    int i;
    for (i = 0; i < datSource.length(); i++)
    {
        b.data()[i] = datSource.constData()[i];
    }

    b.data()[i++] = (char)163;
    b.data()[i++] = (char)172;
    b.data()[i++] = (char)161;
    b.data()[i++] = (char)163;

    for (int k = 0; k < a.length(); k++)
    {
        b.data()[i] = a.constData()[k];
        i++;
    }

    QByteArray md5;
    md5 = QCryptographicHash::hash(b, QCryptographicHash::Md5);

    md5 = md5.toHex();

    return QString(md5);
}

QString AccountCenter::hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "quot;
    // ascii characters 0x36 ("6") and 0x5c ("quot;) are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed.toBase64();
}

