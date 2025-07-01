# 模块详解: DLC支付系统 (C++/Python 交互)

本文档详细解析 `Brotato-Qt` 项目中技术最复杂、最具综合性的模块——**DLC支付系统**。该系统通过C++客户端与Python后端的配合，完整地模拟了一套真实的商品购买与激活流程。

## 1. 核心职责与架构

该系统的核心职责是模拟用户购买游戏内DLC（地图拓展包）的完整流程。它采用了一个经典的**客户端/服务器 (C/S) 架构**：

- **C++ 客户端 (`PaymentManager`)**:
  - 内嵌在Qt游戏应用中。
  - 负责向服务器发起创建订单的请求。
  - 负责向服务器轮询查询支付结果。
  - 负责根据服务器返回的结果更新UI和游戏内状态。
- **Python 服务端 (`app.py`)**:
  - 一个基于**Flask**框架的轻量级Web服务器。
  - 提供API接口供客户端调用。
  - 负责与**支付宝沙箱API**进行交互，生成真实的支付链接和二维码。
  - 负责接收支付宝的**异步回调通知**，并更新订单状态。

这种C/S分离的架构是现代网络应用的通用模式，能将与第三方服务（如支付宝）的复杂交互逻辑封装在服务端，使客户端逻辑更纯粹。

## 2. C++ 客户端 (`PaymentManager`)

`PaymentManager`是C++侧的核心，负责所有与网络支付相关的任务。

### 文件组成

- `payment_manager.h`: 定义了 `PaymentManager` 类的接口，包括公共方法和信号。
- `payment_manager.cpp`: 实现了与后端服务器通信的逻辑。

### 关键实现分析

- **发起支付 (`purchaseDlc`)**:
  1. 当用户在`Menu`界面点击“购买DLC”按钮时，`Menu`会调用`PaymentManager::instance()->purchaseDlc(...)`。
  2. 此函数会生成一个唯一的订单号 (`QUuid`)，并打包好商品信息（ID, 价格, 名称）。
  3. 使用`QNetworkAccessManager`，将这些数据以JSON格式**POST**到Python服务器的 `/api/create-payment` 接口。
- **处理支付二维码 (`handlePaymentUrlReply`)**:
  1. 当收到服务器的响应后，`handlePaymentUrlReply`槽函数被触发。
  2. 它解析服务器返回的JSON，其中包含了**支付页面的URL**和**二维码图片的Base64编码**。
  3. 它将Base64字符串解码为`QPixmap`，并创建一个`QDialog`弹窗来向用户显示这个支付二维码。
- **轮询查询支付状态 (`startPollingForStatus`)**:
  1. 当二维码弹窗被关闭后，`startPollingForStatus`被调用。
  2. 它会启动一个`QTimer`，每隔3秒钟向Python服务器的 `/api/check-payment-status` 接口发起一次**GET**请求，查询订单的最新状态。
- **处理支付结果 (`handlePurchaseStatusReply`)**:
  1. 当轮询请求收到服务器的响应后，`handlePurchaseStatusReply`被触发。
  2. 它解析返回的JSON，检查`trade_status`字段。
  3. 如果状态是 `TRADE_SUCCESS`，则停止轮询计时器，并发出 `purchaseSuccess(dlcId)` 信号。
  4. `Menu`界面接收到这个信号后，就会更新`GameData`中的状态，并将UI从“购买”按钮切换为“激活”开关。

## 3. Python 服务端 (`app.py`)

`app.py` 是一个基于Flask框架的Web应用，它充当了游戏客户端和支付宝之间的桥梁。

### 关键实现分析

- **Flask框架**: 使用`@app.route(...)`装饰器来定义API接口的URL和请求方法（GET/POST）。
- **`/api/create-payment` 接口 (POST)**:
  1. 接收C++客户端发来的订单信息。
  2. 调用`alipay-sdk-python`库的`api_alipay_trade_page_pay`方法，向支付宝沙箱服务器请求创建一个网页支付订单。
  3. 支付宝返回一个包含所有支付参数的超长URL。
  4. 使用`qrcode`库将这个URL生成为一个二维码图片。
  5. 使用`base64`库将图片编码成字符串。
  6. 将原始URL和二维码的Base64字符串打包成JSON返回给C++客户端。
- **`/api/payment-notify` 接口 (POST)**:
  1. 这是提供给**支付宝服务器**回调的接口，URL必须是公网可访问的（因此需要内网穿透）。
  2. 当用户在沙箱环境中完成支付后，支付宝服务器会向这个接口发送一个POST请求，其中包含了支付结果和签名。
  3. 服务器代码首先使用`alipay.verify()`验证签名的合法性，防止伪造请求。
  4. 验证通过后，从请求中获取订单号和支付状态（`TRADE_SUCCESS`），并更新内部`payment_statuses`字典中的状态。
- **`/api/check-payment-status` 接口 (GET)**:
  1. 这是一个供C++客户端轮询调用的接口。
  2. 它接收一个订单号作为参数，然后在`payment_statuses`字典中查找该订单的当前状态，并将其返回。

## 4. 应用的Qt知识点

- **`Qt Network`模块**:
  - `QNetworkAccessManager`: 管理网络请求的事件循环。
  - `QNetworkRequest`: 封装一个HTTP请求，包括URL、Header（如`Content-Type: application/json`）等。
  - `QNetworkReply`: 封装HTTP响应，可以通过它读取返回数据和处理网络错误。
- **JSON处理**:
  - `QJsonDocument`: 用于解析和生成JSON数据。
  - `QJsonObject`: 代表一个JSON对象，用于构建要发送的POST数据。
- **多线程与异步**: 整个网络通信过程是**异步非阻塞**的。当`m_networkManager->post(...)`被调用后，程序不会卡住等待响应，而是继续执行。当网络响应到达时，Qt的事件循环会自动触发对应的`finished`信号和连接的槽函数。这是Qt网络编程的核心优势。
- **`QUuid`**: 用于生成全局唯一的字符串，作为订单号，确保每次交易都独一无二。
- **`QDialog`**: 用于创建模态或非模态的弹窗，本项目用它来显示支付二维码。