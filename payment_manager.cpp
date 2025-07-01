#include "payment_manager.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QDialog>      // 用于创建对话框
#include <QLabel>       // 用于显示二维码
#include <QVBoxLayout>  // 用于布局
#include <QPushButton>  // 用于创建备用按钮
#include <QDesktopServices> // 用于在浏览器打开链接


const QString YOUR_SERVER_BASE_URL = "http://127.0.0.1:5000";

PaymentManager::PaymentManager(QWidget *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this))
    , m_webView(nullptr)
    , m_parentWidget(parent)
    , m_outTradeNo("")
    , m_currentDlcId("")
    , m_pollingTimer(nullptr)
{

}

PaymentManager::~PaymentManager()
{
    if (m_pollingTimer) {
        m_pollingTimer->stop();
    }
}

// 1. 外部调用此函数开始购买流程
void PaymentManager::purchaseDlc(const QString& dlcId, float price, const QString& subject)
{
    m_currentDlcId = dlcId;
    m_outTradeNo = QUuid::createUuid().toString(QUuid::WithoutBraces); // 生成一个唯一的订单号

    QJsonObject requestData;
    requestData["dlcId"] = dlcId;
    requestData["price"] = QString::number(price, 'f', 2);
    requestData["subject"] = subject;
    requestData["outTradeNo"] = m_outTradeNo;

    QUrl url(YOUR_SERVER_BASE_URL + "/api/create-payment");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    connect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePaymentUrlReply);
    m_networkManager->post(request, QJsonDocument(requestData).toJson());
}

// 2. 收到从您服务器返回的支付URL
void PaymentManager::handlePaymentUrlReply(QNetworkReply *reply)
{
    disconnect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePaymentUrlReply);

    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QString qrCodeBase64 = json["qrCodeBase64"].toString();
        QString paymentUrl = json["paymentUrl"].toString(); // 备用链接

        if (!qrCodeBase64.isEmpty()) {
            // 解码 Base64 数据为图片
            QByteArray qrCodeData = QByteArray::fromBase64(qrCodeBase64.toUtf8());
            QPixmap qrPixmap;
            qrPixmap.loadFromData(qrCodeData, "PNG");
            // 创建一个简单的对话框来显示二维码
            QDialog* paymentDialog = new QDialog(m_parentWidget);
            paymentDialog->setWindowTitle("请使用支付宝扫码支付");
            paymentDialog->setFixedSize(350, 420); // 固定大小

            QVBoxLayout* layout = new QVBoxLayout(paymentDialog);
            QLabel* qrLabel = new QLabel(paymentDialog);
            qrLabel->setPixmap(qrPixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            qrLabel->setAlignment(Qt::AlignCenter);
            QLabel* infoLabel = new QLabel("<b>支付成功后，请手动关闭此窗口</b>", paymentDialog);
            infoLabel->setAlignment(Qt::AlignCenter);
            QPushButton* browserButton = new QPushButton("在浏览器中打开", paymentDialog);

            layout->addWidget(qrLabel);
            layout->addWidget(infoLabel);
            layout->addWidget(browserButton);
            paymentDialog->setLayout(layout);

            // 连接备用按钮的点击事件
            connect(browserButton, &QPushButton::clicked, [paymentUrl](){
                QDesktopServices::openUrl(QUrl(paymentUrl));
            });

            // 当对话框关闭时，开始轮询支付状态
            connect(paymentDialog, &QObject::destroyed, this, &PaymentManager::startPollingForStatus);
            paymentDialog->setAttribute(Qt::WA_DeleteOnClose);
            paymentDialog->show();

        } else {
            emit purchaseFailed("服务器未能生成二维码。");
        }
    } else {
        emit purchaseFailed("连接服务器失败: " + reply->errorString());
    }
    reply->deleteLater();
}

// 5. 开始轮询检查支付状态
void PaymentManager::startPollingForStatus()
{
    if (m_pollingTimer) {
        m_pollingTimer->stop();
        delete m_pollingTimer;
    }

    m_pollingTimer = new QTimer(this);
    int attempts = 0;
    connect(m_pollingTimer, &QTimer::timeout, this, [=]() mutable {
        if (++attempts > 20) {
            m_pollingTimer->stop();
            emit purchaseFailed("支付结果查询超时。");
            return;
        }

        connect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePurchaseStatusReply);

        QUrlQuery query;
        query.addQueryItem("outTradeNo", m_outTradeNo);
        QUrl url(YOUR_SERVER_BASE_URL + "/api/check-payment-status");
        url.setQuery(query);

        m_networkManager->get(QNetworkRequest(url));
    });
    m_pollingTimer->start(3000); // 每3秒查询一次
}

// 6. 处理支付状态查询结果
void PaymentManager::handlePurchaseStatusReply(QNetworkReply* reply)
{
    disconnect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePurchaseStatusReply);

    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QString tradeStatus = json["trade_status"].toString(); // 支付宝返回的是 trade_status

        if (tradeStatus == "TRADE_SUCCESS") {
            m_pollingTimer->stop();
            emit purchaseSuccess(m_currentDlcId);
        }
    }
    reply->deleteLater();
}
