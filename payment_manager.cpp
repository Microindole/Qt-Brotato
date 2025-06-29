// #include "payment_manager.h"
// #include <QNetworkReply>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QUrlQuery>
// #include <QTimer>
// #include <QApplication>
// #include <QScreen>
// #include <QtWebEngineWidgets/QWebEngineView>
// #include <QUuid>

// // ！！！重要！！！
// // 这里的URL应该是您自己后端服务器的地址
// const QString YOUR_SERVER_BASE_URL = "http://127.0.0.1:3000"; // 示例：本地测试服务器

// PaymentManager::PaymentManager(QWidget *parent)
//     : QObject(parent)
//     , m_parentWidget(parent)
// {
//     m_networkManager = new QNetworkAccessManager(this);
//     connect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePaymentUrlReply);
// }

// // 1. 开始购买流程
// void PaymentManager::purchaseDlc(const QString& dlcId, float price)
// {
//     m_dlcId = dlcId;
//     m_outTradeNo = QUuid::createUuid().toString().remove('{').remove('}').remove('-'); // 生成一个唯一的订单号

//     // 准备发送给您自己服务器的数据
//     QJsonObject requestData;
//     requestData["dlcId"] = dlcId;
//     requestData["price"] = QString::number(price, 'f', 2);
//     requestData["subject"] = dlcId + " DLC";
//     requestData["outTradeNo"] = m_outTradeNo;

//     QUrl url(YOUR_SERVER_BASE_URL + "/api/create-payment");
//     QNetworkRequest request(url);
//     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

//     // 向您的服务器发送请求，以获取支付URL
//     m_networkManager->post(request, QJsonDocument(requestData).toJson());
// }

// // 2. 收到从您服务器返回的支付URL
// void PaymentManager::handlePaymentUrlReply(QNetworkReply *reply)
// {
//     disconnect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePaymentUrlReply);

//     if (reply->error() == QNetworkReply::NoError) {
//         QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
//         QString paymentUrl = json["paymentUrl"].toString();

//         if (!paymentUrl.isEmpty()) {
//             // 3. 创建并显示内嵌浏览器
//             m_webView = new QWebEngineView(m_parentWidget);
//             m_webView->setAttribute(Qt::WA_DeleteOnClose);

//             // 使WebView像一个模态对话框
//             QRect screenRect = qApp->primaryScreen()->geometry();
//             m_webView->resize(800, 600);
//             m_webView->move(screenRect.center() - m_webView->rect().center());
//             m_webView->setWindowTitle("支付宝安全支付");

//             m_webView->load(QUrl(paymentUrl));
//             m_webView->show();

//             // 4. 用户关闭支付窗口后，开始轮询支付结果
//             connect(m_webView, &QWebEngineView::destroyed, this, &PaymentManager::checkPurchaseStatus);
//         } else {
//             emit purchaseFailed("服务器未能创建支付链接。");
//         }
//     } else {
//         emit purchaseFailed("连接服务器失败: " + reply->errorString());
//     }
//     reply->deleteLater();
// }

// // 5. 开始检查支付状态
// void PaymentManager::checkPurchaseStatus()
// {
//     // 如果已有计时器，先停掉
//     if (m_pollingTimer && m_pollingTimer->isActive()) {
//         m_pollingTimer->stop();
//         m_pollingTimer->deleteLater();
//     }

//     // 创建一个新的计时器，每3秒查询一次，总共查询10次
//     m_pollingTimer = new QTimer(this);
//     int attempts = 0;
//     connect(m_pollingTimer, &QTimer::timeout, this, [=]() mutable {
//         if (++attempts > 10) {
//             m_pollingTimer->stop();
//             emit purchaseFailed("支付结果查询超时。");
//             return;
//         }

//         // 重新连接信号到状态查询的处理器
//         connect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePurchaseStatusReply);

//         QUrlQuery query;
//         query.addQueryItem("outTradeNo", m_outTradeNo);
//         QUrl url(YOUR_SERVER_BASE_URL + "/api/check-payment-status");
//         url.setQuery(query);

//         m_networkManager->get(QNetworkRequest(url));
//     });
//     m_pollingTimer->start(3000); // 3秒后开始第一次查询
// }

// // 6. 处理支付状态查询结果
// void PaymentManager::handlePurchaseStatusReply(QNetworkReply* reply)
// {
//     disconnect(m_networkManager, &QNetworkAccessManager::finished, this, &PaymentManager::handlePurchaseStatusReply);

//     if (reply->error() == QNetworkReply::NoError) {
//         QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
//         QString tradeStatus = json["tradeStatus"].toString();

//         if (tradeStatus == "TRADE_SUCCESS") {
//             m_pollingTimer->stop(); // 支付成功，停止轮询
//             emit purchaseSuccess(m_dlcId);
//         }
//         // 如果状态是 "WAIT_BUYER_PAY" 或其他，计时器会继续轮询
//     }
//     reply->deleteLater();
// }
