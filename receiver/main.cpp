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
            qCritical() << "Unable to start server:" << server_->errorString(); // if port is busy, we will be write the log letter
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
        // Central widget and layout
        // its like root container main in HTML
        auto *central = new QWidget(this);  // just a quadrate (an empty box) ... this - current MainWindow as parent (<div id="central"></div>)
        auto *layout = new QVBoxLayout(central); // help to put others widgets one over other (display: flex; flex-direction: column;)
        label_received_ = new QLabel("Received time from Sender: ---", this); // handed "this" just for manage with layout from center
        label_local_    = new QLabel("My local time: ---", this);
        layout->addWidget(label_received_); // like <span>Received time from Sender: ---</span>
        layout->addWidget(label_local_);
        central->setLayout(layout); // setting our "central" with layouts rules
        setCentralWidget(central); // put "central" in the center of our window

        // status bar
        statusBar()->showMessage("Not connected, messages: 0");

        // local timer
        auto *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::updateLocalTime); // where is it located on the ours board??
        timer->start(500);
        updateLocalTime();

        // network receiver

        // "new QTimer" give a life for object (keeping in dynamic memory) unlike just "QWidget central(this)""

    }

private slots:
    void updateLocalTime() {

    }    

private:
    QLabel *label_received_;
    QLabel *label_local_;
    // NetworkReceiver *network_;
    // int message_count_;
}




// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     QMainWindow window;
//     window.setWindowTitle("Receiver - IPC Client");
//     window.resize(400, 300);
//     window.show();
//     return app.exec();
// }