#ifndef ACCOUNTCENTER_H
#define ACCOUNTCENTER_H

#include <QObject>
#include <QNetworkAccessManager>

class AccountCenter : public QObject
{
public:
//	struct USER_INFO {
//		QString user_id;
//		QString user_name;
// 		QString nick_name;
// 		QString nick_name_full;
// 		QString nick_name_short;
// 		QString realm_exinfo;
//		int region;
//	};

	struct USER 
	{
		QString user_id;
		QString user_name;
		QString access_token;
		QString user_group;
		//         QString expires_at;
		//         QString refresh_token;
		QString mac_key;
		//         QString mac_algorithm;
		//         QString server_time;
		//         QString warning_code;
	};

    Q_OBJECT
public:
    explicit AccountCenter(QObject *parent = 0);
    ~AccountCenter();

	USER &GetUser();
	//USER_INFO &GetUserInfo();

	const QString &GetDeviceUuid();

signals:
    void loginResult(bool ok,const QString &reason);
    void logoutResult(QString account, bool ok, QString reason);

    void userLogout();

public slots:
    void UserLogin(QString account, QString password);
    void UserLogout();

private slots:
    void on_user_login(QString account, QString password);
    void on_user_logout();
    void requestFinished(QNetworkReply* reply);

private:
    enum METHOD{
        POST = 0,
        GET,
        DELETE
    };
    enum COMMADN{
        Session = 0,
        IdentifyCode,
        ValidIdentifyCode,
        Smses,
        Login,
        Logout,
        Authorization,
        UserInfo
    };

    QUrl commandToUrl(int command);

    void doRequest(int method ,int command, const QByteArray &data);

    void parseResult(bool error, int command, const QJsonDocument& jsonDocument);

    void encryptAuthorization(int method, QString uri);
    QString makeNonce();
    QString randString();

    QString encryptMD5_Salt(QString content);
    QString hmacSha1(QByteArray key, QByteArray baseString);

private:
    QNetworkAccessManager manager;
    QMap<QNetworkReply *, int> currentRequest;

	const QString PROTOCOL = "https://";
    const QString HOST = "aqapi.101.com";
    const QString ORG_NAME = "ND";

	const QString _device_uuid;

    struct SESSION{
        QString session_id;
        QString session_key;
        int     op_count;
    }_session;

	struct USER _user;

    struct AUTHORIZATION{
        QString mac_id;
        QString nonce;
        QString mac;
        QByteArray getAuthorization(){
            QString final;
            final += "MAC id=";
            final.append("\"");
            final += mac_id;
            final.append("\"");
            final += ",";
            final += "nonce=";
            final.append("\"");
            final += nonce;
            final.append("\"");
            final += ",";
            final += "mac=";
            final.append("\"");
            final += mac;
            final.append("\"");

            return final.toUtf8();
        }
    }_authorization;

 //   struct USER_INFO _user_info;
};

#endif // ACCOUNTCENTER_H
