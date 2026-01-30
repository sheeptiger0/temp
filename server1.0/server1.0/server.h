#ifndef SERVER_H
#define SERVER_H
#pragma execution_character_set("utf-8")
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QList>
#include <QDateTime>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget* parent = nullptr);
    ~Server();

private slots:
    void onNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();
    void onSendToAllClicked();
    void onShowNetworkInfo();
    void onClearLog();

private:
    void setupUI();
    void startServer();
    void broadcastMessage(const QString& message, QTcpSocket* excludeSocket = nullptr);
    QString getClientInfo(QTcpSocket* client) const;
    void logMessage(const QString& message, bool isError = false);
    void saveSettings();
    void loadSettings();

    QTcpServer* m_tcpServer;
    QList<QTcpSocket*> m_clients;

    QTextEdit* m_logEdit;
    QTextEdit* m_messageEdit;
    QLabel* m_statusLabel;
    quint16 m_serverPort;
};

#endif // SERVER_H