// #ifndef PAYMENTMANAGER_H
// #define PAYMENTMANAGER_H

// #include <QObject>
// #include <QNetworkAccessManager>
// #include <QTimer>

// class QWebEngineView;

// class PaymentManager : public QObject
// {
//     Q_OBJECT
// public:
//     explicit PaymentManager(QWidget *parent = nullptr);

//     void purchaseDlc(const QString& dlcId, float price);

// signals:
//     void purchaseSuccess(const QString& dlcId);
//     void purchaseFailed(const QString& reason);

// private slots:
//     void handlePaymentUrlReply(QNetworkReply *reply);
//     void handlePurchaseStatusReply(QNetworkReply* reply);

// private:
//     void checkPurchaseStatus();

//     QNetworkAccessManager* m_networkManager;
//     QWebEngineView* m_webView;
//     QWidget* m_parentWidget; // 用于显示WebView
//     QString m_outTradeNo; // 我们自己生成的订单号
//     QString m_dlcId;      // 当前购买的DLC ID
//     QTimer* m_pollingTimer; // 用于轮询支付结果的计时器
// };

// #endif // PAYMENTMANAGER_H
