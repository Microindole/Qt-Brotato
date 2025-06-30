#ifndef PAYMENTMANAGER_H
#define PAYMENTMANAGER_H

#include <QObject>

// 前置声明
class QNetworkAccessManager;
class QNetworkReply;
class QWebEngineView;
class QTimer;
class QWidget;

class PaymentManager : public QObject
{
    Q_OBJECT
public:
    explicit PaymentManager(QWidget *parent = nullptr);
    ~PaymentManager();

    // 公共接口，用于从外部（例如菜单按钮）调用以开始购买流程
    void purchaseDlc(const QString& dlcId, float price, const QString& subject);

signals:
    // 信号，用于通知UI支付成功或失败
    void purchaseSuccess(const QString& dlcId);
    void purchaseFailed(const QString& reason);

private slots:
    // 私有槽，用于处理网络请求的返回结果
    void handlePaymentUrlReply(QNetworkReply *reply);
    void handlePurchaseStatusReply(QNetworkReply* reply);

private:
    // 私有函数，用于轮询检查支付状态
    void startPollingForStatus();

    QNetworkAccessManager* m_networkManager;
    QWebEngineView* m_webView;       // 用于显示支付二维码页面
    QWidget* m_parentWidget;         // WebView的父窗口，通常是主窗口
    QString m_outTradeNo;            // 我们自己生成的唯一订单号
    QString m_currentDlcId;          // 当前正在购买的DLC ID
    QTimer* m_pollingTimer = nullptr; // 用于轮询支付结果的计时器
};

#endif // PAYMENTMANAGER_H
