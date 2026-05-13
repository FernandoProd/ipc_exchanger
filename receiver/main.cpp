#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include <QByteArray>
#include "protocol.hpp"



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
        network_ = new NetworkReceiver(this);
        // tying between signal network_ and updating GUI
        connect(network_, &NetworkReceiver::connected, this, [this]() {
            statusBar()->showMessage(QString("Connected, messages: %1").arg(message_count_));
        });
        connect(network_, &NetworkReceiver::disconnected, this, [this]() {
            label_received_->setText("Received time from Sender: ---");
            statusBar()->showMessage(QString("Not connected, messages: %1").arg(message_count_));
        });
        connect(network_, &NetworkReceiver::messageReceived, this, [this](QDateTime timestamp) {
            label_received_->setText("Received time from Sender: " +
                                     timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz"));
            ++message_count_;
            statusBar()->showMessage(QString("Connected, messages: %1").arg(message_count_));
        });
        connect(network_, &NetworkReceiver::errorOccurred, this, [this](QString desc) {
            statusBar()->showMessage("Error: " + desc);
        });

        
        // connect(network_, &NetworkReceiver::connected, this, [this]() { }
        // network_ - the object that sending a signal
        // &NetworkReceiver::connected - which signal, "connected" is a signal 
        // this - the object (mainwindow) its like self in python classes
        // [this]() {} - labnda function (processor)

        // connect means: when event A is happened do event B


        // "new QTimer" give a life for object (keeping in dynamic memory) unlike just "QWidget central(this)""

    }

private slots:
    void updateLocalTime() {
        label_local_->setText("Моё локальное время: " +
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    }    

private:
    QLabel *label_received_;
    QLabel *label_local_;
    NetworkReceiver *network_;
    int message_count_;
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.setWindowTitle("Receiver");
    w.resize(500, 200);
    w.show();
    return app.exec();
}

#include "main.moc"

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     QMainWindow window;
//     window.setWindowTitle("Receiver - IPC Client");
//     window.resize(400, 300);
//     window.show();
//     return app.exec();
// }