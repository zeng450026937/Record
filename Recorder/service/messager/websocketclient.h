#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#define ASIO_STANDALONE

#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_FUNCTIONAL_
#define _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_MEMORY_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_

#include <QThread>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <string>

// This header pulls in the WebSocket++ abstracted thread support that will
// select between boost::thread and std::thread based on how the build system
// is configured.
#include <websocketpp/common/thread.hpp>


using namespace std;


// Bind the handlers we are using
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

/**
* The client connects to a WebSocket server.
*/
class WebSocketClient : public QThread
{
    Q_OBJECT
public:
    enum ConnectionStatus
    {
        CS_OPENED,
        CS_CLOSED,
        CS_FAILED
    };

public:
    typedef websocketpp::client<websocketpp::config::asio_client> client;
    typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;

    explicit WebSocketClient(QObject* parent=0);
    ~WebSocketClient();

protected:
    // The open handler will signal that we are ready to start sending message
    void on_open(websocketpp::connection_hdl);

    // The close handler will signal that we should stop sending message
    void on_close(websocketpp::connection_hdl);

    // The fail handler will signal that we should stop sending message
    void on_fail(websocketpp::connection_hdl);

    // The message handler will signal that we have received a new message
    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg);

signals:
    void connection_status(int iStatus);

    void text_message(QString text);
    void binary_message(QByteArray content);

public slots:
    // This method will complete the connection
    void stop_connect();

    void connect_to(std::string header,std::string uri);

    bool sendText(std::string text);

    // This method will send binary message to server
    bool sendBinary(void* binary, long size);

protected:
    void run();


private:
    std::string m_uri;
    std::string m_header;
    client m_client;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::mutex m_lock;
};

#endif // WEBSOCKETCLIENT_H
