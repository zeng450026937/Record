#include "websocketclient.h"

WebSocketClient::WebSocketClient(QObject* parent) : QThread(parent)
{
    // set up access channels to only log interesting things
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    //m_client.set_access_channels(websocketpp::log::alevel::connect);
    //m_client.set_access_channels(websocketpp::log::alevel::disconnect);
    //m_client.set_access_channels(websocketpp::log::alevel::app);
    //m_client.set_access_channels(websocketpp::log::alevel::all);
    //m_client.set_error_channels(websocketpp::log::elevel::all);

    // Initialize the Asio transport policy
    m_client.init_asio();

    m_client.set_message_handler(bind(&WebSocketClient::on_message, this, ::_1, ::_2));
    m_client.set_open_handler(bind(&WebSocketClient::on_open, this, _1));
    m_client.set_close_handler(bind(&WebSocketClient::on_close, this, _1));
    m_client.set_fail_handler(bind(&WebSocketClient::on_fail, this, _1));
}

WebSocketClient::~WebSocketClient()
{
    this->stop_connect();
}

void WebSocketClient::connect_to(std::string header, std::string uri)
{
    m_uri = uri;
    m_header = header;

    this->start();
}

// This method will block until the connection is complete
void WebSocketClient::run()
{
    // Create a new connection to the given URI
    websocketpp::lib::error_code ec;

    client::connection_ptr con;

    con = m_client.get_connection(m_uri, ec);

    if (ec) {
        m_client.get_alog().write(websocketpp::log::alevel::app,
            "Get Connection Error: " + ec.message());
        return;
    }

    // Grab a handle for this connection so we can talk to it in a thread
    // safe manor after the event loop starts.
    m_hdl = con->get_handle();

    // add some header for server to validate the client.
    //con->append_header("id", "pc");
    con->append_header("json_header", m_header);

    // Queue the connection. No DNS queries or network connections will be
    // made until the io_service event loop is run.
    m_client.connect(con);

    // Create a thread to run the ASIO io_service event loop
    websocketpp::lib::thread asio_thread(&client::run, &m_client);

    asio_thread.join();
}


void WebSocketClient::stop_connect() {
    m_uri = "";
    m_header = "";

    websocketpp::lib::error_code ec;
    m_client.close(m_hdl,websocketpp::close::status::normal,QString("Terminating connection").toStdString(),ec);

    if(m_client.is_listening()){
        m_client.stop_listening();
    }
    if(!m_client.stopped()){
        m_client.stop();
    }

    while(this->isRunning()){
        this->quit();
        this->wait(1);
    }

    m_hdl.reset();
    m_client.reset();
}

bool WebSocketClient::sendText(std::string data) {
    websocketpp::lib::error_code ec;
    scoped_lock guard(m_lock);

    try {
        m_client.send(m_hdl, data, websocketpp::frame::opcode::text, ec);
    }
    catch (const websocketpp::lib::error_code& e) {
        std::cout << "Send failed because: " << e
            << "(" << e.message() << ")" << std::endl;
    }
    if (ec) { // we got an error
        // Error sending on websocket. Log reason using ec.message().
        return false;
    }
    return true;
}

bool WebSocketClient::sendBinary(void* data, long size) {
    websocketpp::lib::error_code ec;
    scoped_lock guard(m_lock);

    try {
        m_client.send(m_hdl, data, size, websocketpp::frame::opcode::binary, ec);
    }
    catch (const websocketpp::lib::error_code& e) {
        std::cout << "Send failed because: " << e
            << "(" << e.message() << ")" << std::endl;
    }
    if (ec) { // we got an error
        // Error sending on websocket. Log reason using ec.message().
        return false;
    }
    return true;
}

// The open handler will signal that we are ready to start sending telemetry
void WebSocketClient::on_open(websocketpp::connection_hdl) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
        "Connection opened, starting telemetry!");
    //scoped_lock guard(m_lock);

    emit connect_open();
}

// The close handler will signal that we should stop sending telemetry
void WebSocketClient::on_close(websocketpp::connection_hdl) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
        "Connection closed, stopping telemetry!");
    //scoped_lock guard(m_lock);

    emit connect_close();

}

// The fail handler will signal that we should stop sending telemetry
void WebSocketClient::on_fail(websocketpp::connection_hdl) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
        "Connection failed, stopping telemetry!");
    //scoped_lock guard(m_lock);

    emit connect_fail();

}


// Define a callback to handle incoming messages
void WebSocketClient::on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {

    //scoped_lock guard(m_lock);

    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
        emit text_message(QString::fromStdString(msg->get_payload()));
    }
    else if (msg->get_opcode() == websocketpp::frame::opcode::binary) {
        emit binary_message(msg->get_payload().size(),QByteArray::fromStdString(msg->get_payload()));
    }
}
