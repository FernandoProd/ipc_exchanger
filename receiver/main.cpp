#include <QApplication>
#include <QMainWindow>



class NetworkReceiver : public QObject {
    Q_OBJECT   // We inherited of basic class Q_Object
public:
    explicit NetworkReceiver(QObject *parent = nullptr)
        : QObject(parent)
        , server_(new QTcpServer(this))
        , client_socket_(nullptr)
    {
        if (!server_->listen(QHostAddress::LocalHost, 12345)) {  // Starting TCP server for listening input connections
            qCritical() << "Unable to start server:" << server_->errorString(); // if port id busy, we will be write the log letter
        } else {
            qDebug() << "Server listening on" << server_->serverAddress() << server_->serverPort(); 
        }
        // server_ - source of signal 
        // &QTcpServer::newConnection - signal that generated when we have a new input connection
        // this - object receiver (NetworkReceiver) 
        // &NetworkReceiver::onNewConnection - calling method if signal is happened
        connect(server_, &QTcpServer::newConnection, this, &NetworkReceiver::onNewConnection); 
    }

signals: // what are the slots and the signals??!
    void connected();
    void disconnected();
    void messageReceived(QDateTime timestamp);
    void errorOccurred(QString description);

private slots:
    void onNewConnection() { /* TODO */ }
    void onReadyRead() { /* TODO */ }
    void onDisconnected() { /* TODO */ }

private:
    QTcpServer *server_;
    QTcpSocket *client_socket_;
    QByteArray buffer_;
};


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , message_count_(0)
    {

    }

}




// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     QMainWindow window;
//     window.setWindowTitle("Receiver - IPC Client");
//     window.resize(400, 300);
//     window.show();
//     return app.exec();
// }