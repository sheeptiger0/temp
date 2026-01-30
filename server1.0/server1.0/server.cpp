#include "server.h"
#include <QApplication>
#include <QScrollBar>
#include <QTimer>

Server::Server(QWidget* parent)
    : QWidget(parent)
    , m_tcpServer(nullptr)
    , m_serverPort(8888)
{
    loadSettings();
    setupUI();
    startServer();
}

Server::~Server()
{
    saveSettings();

    if (m_tcpServer) {
        m_tcpServer->close();
        m_tcpServer->deleteLater();
    }

    for (QTcpSocket* client : m_clients) {
        client->close();
        client->deleteLater();
    }
}

void Server::setupUI()
{
    setWindowTitle("Qt服务器 - 端口转发通信");
    resize(800, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 状态栏
    m_statusLabel = new QLabel("服务器未启动");
    mainLayout->addWidget(m_statusLabel);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* infoBtn = new QPushButton("网络信息");
    connect(infoBtn, &QPushButton::clicked, this, &Server::onShowNetworkInfo);
    buttonLayout->addWidget(infoBtn);

    QPushButton* clearBtn = new QPushButton("清空日志");
    connect(clearBtn, &QPushButton::clicked, this, &Server::onClearLog);
    buttonLayout->addWidget(clearBtn);

    mainLayout->addLayout(buttonLayout);

    // 消息发送区域
    QHBoxLayout* sendLayout = new QHBoxLayout();

    m_messageEdit = new QTextEdit();
    m_messageEdit->setMaximumHeight(80);
    m_messageEdit->setPlaceholderText("输入要广播的消息...");
    sendLayout->addWidget(m_messageEdit);

    QPushButton* sendBtn = new QPushButton("广播消息");
    connect(sendBtn, &QPushButton::clicked, this, &Server::onSendToAllClicked);
    sendLayout->addWidget(sendBtn);

    mainLayout->addLayout(sendLayout);

    // 日志区域
    m_logEdit = new QTextEdit();
    m_logEdit->setReadOnly(true);
    mainLayout->addWidget(m_logEdit);

    logMessage("服务器已启动", false);
}

void Server::startServer()
{
    m_tcpServer = new QTcpServer(this);

    // 尝试监听指定端口
    if (m_tcpServer->listen(QHostAddress::Any, m_serverPort)) {
        m_statusLabel->setText(QString("服务器运行中 - 端口: %1").arg(m_serverPort));
        logMessage(QString("✓ 服务器启动成功，监听端口 %1").arg(m_serverPort));

        connect(m_tcpServer, &QTcpServer::newConnection,
            this, &Server::onNewConnection);

        // 显示网络信息
        onShowNetworkInfo();
    }
    else {
        QString errorMsg = QString("服务器启动失败: %1").arg(m_tcpServer->errorString());
        m_statusLabel->setText(errorMsg);
        logMessage("✗ " + errorMsg, true);

        QMessageBox::critical(this, "错误",
            QString("无法启动服务器在端口 %1:\n%2\n\n请检查：\n1. 端口是否被占用\n2. 防火墙设置")
            .arg(m_serverPort).arg(m_tcpServer->errorString()));
    }
}

void Server::onNewConnection()
{
    QTcpSocket* clientSocket = m_tcpServer->nextPendingConnection();
    if (!clientSocket) return;

    // 添加到客户端列表
    m_clients.append(clientSocket);

    // 连接信号
    connect(clientSocket, &QTcpSocket::readyRead,
        this, &Server::onClientReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected,
        this, &Server::onClientDisconnected);

    QString clientInfo = getClientInfo(clientSocket);
    logMessage(QString("✓ 客户端连接: %1 - 当前客户端: %2")
        .arg(clientInfo).arg(m_clients.size()));

    // 发送欢迎消息
    QString welcomeMsg = QString("欢迎连接到服务器! 服务器时间: %1")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    clientSocket->write(welcomeMsg.toUtf8());

    // 广播新客户端加入消息
    broadcastMessage(QString("系统: 新客户端加入 - %1").arg(clientInfo), clientSocket);
}

void Server::onClientReadyRead()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data).trimmed();

    if (message.isEmpty()) return;

    QString clientInfo = getClientInfo(clientSocket);

    // 修复：使用不同的变量名，避免与函数名冲突
    QString receivedMsg = QString("[%1] %2").arg(clientInfo).arg(message);
    logMessage(receivedMsg);

    // 广播消息给所有客户端（除了发送者）
    QString broadcastMsg = QString("[%1] %2").arg(clientInfo).arg(message);
    broadcastMessage(broadcastMsg, clientSocket);
}

void Server::onClientDisconnected()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QString clientInfo = getClientInfo(clientSocket);
    m_clients.removeOne(clientSocket);

    logMessage(QString("✗ 客户端断开: %1 - 剩余客户端: %2")
        .arg(clientInfo).arg(m_clients.size()));

    // 广播客户端离开消息
    broadcastMessage(QString("系统: 客户端离开 - %1").arg(clientInfo));

    clientSocket->deleteLater();
}

void Server::onSendToAllClicked()
{
    QString message = m_messageEdit->toPlainText().trimmed();
    if (message.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入要发送的消息");
        return;
    }

    QString serverMessage = QString("[服务器] %1").arg(message);

    logMessage(serverMessage);
    broadcastMessage(serverMessage);

    m_messageEdit->clear();
}

void Server::onShowNetworkInfo()
{
    QString info = "=== 服务器网络信息 ===\n\n";

    info += QString("运行状态: %1\n").arg(m_tcpServer->isListening() ? "运行中" : "已停止");
    info += QString("监听端口: %1\n").arg(m_serverPort);
    info += QString("客户端数量: %1\n\n").arg(m_clients.size());

    // 显示所有网络接口
    info += "本地IP地址:\n";
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (const QHostAddress& address : addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            address != QHostAddress::LocalHost) {
            info += "  " + address.toString() + "\n";
        }
    }

    info += "\n端口转发设置指南:\n";
    info += "1. 登录路由器管理界面 (通常是 192.168.1.1 或 192.168.0.1)\n";
    info += "2. 找到'端口转发'或'虚拟服务器'设置\n";
    info += "3. 添加规则: 外部端口 " + QString::number(m_serverPort) +
        " → 内部IP:上述IP之一 → 内部端口:" + QString::number(m_serverPort) + "\n";
    info += "4. 客户端连接您的公网IP:" + QString::number(m_serverPort);

    logMessage(info);
}

void Server::onClearLog()
{
    m_logEdit->clear();
    logMessage("日志已清空");
}

void Server::broadcastMessage(const QString& message, QTcpSocket* excludeSocket)
{
    QByteArray data = message.toUtf8() + "\n";

    int sentCount = 0;
    for (QTcpSocket* client : m_clients) {
        if (client != excludeSocket &&
            client->state() == QAbstractSocket::ConnectedState) {
            qint64 bytesWritten = client->write(data);
            if (bytesWritten > 0) {
                sentCount++;
            }
        }
    }

    if (sentCount > 0) {
        logMessage(QString("消息已发送给 %1 个客户端").arg(sentCount));
    }
}

QString Server::getClientInfo(QTcpSocket* client) const
{
    if (!client) return "未知客户端";

    return QString("%1:%2").arg(client->peerAddress().toString())
        .arg(client->peerPort());
}

void Server::logMessage(const QString& message, bool isError)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);

    if (isError) {
        m_logEdit->append("<font color='red'>" + logEntry + "</font>");
    }
    else {
        m_logEdit->append(logEntry);
    }

    // 自动滚动到底部
    QScrollBar* scrollbar = m_logEdit->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void Server::saveSettings()
{
    QSettings settings("QtServer", "NetworkChat");
    settings.setValue("server_port", m_serverPort);
}

void Server::loadSettings()
{
    QSettings settings("QtServer", "NetworkChat");
    m_serverPort = settings.value("server_port", 8888).toUInt();
}