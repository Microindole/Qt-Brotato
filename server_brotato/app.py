import os
from flask import Flask, request, jsonify
from flask_cors import CORS
from alipay import AliPay
import qrcode  # 导入二维码库
import base64
from io import BytesIO

# --- 初始化 Flask 应用 ---
app = Flask(__name__)
CORS(app)

payment_statuses = {}

PUBLIC_DOMAIN = "http://e94e666c.natappfree.cc"

# --- 支付宝配置 (与之前保持一致) ---
ALIPAY_CONFIG = {
    'APP_ID': '2021000148677978',
    'APP_PRIVATE_KEY_PATH': 'keys/app_private_key.pem',
    'ALIPAY_PUBLIC_KEY_PATH': 'keys/alipay_public_key.pem',
    'SIGN_TYPE': 'RSA2',
    'DEBUG': False,
    'GATEWAY': 'https://openapi-sandbox.dl.alipaydev.com/gateway.do'
}


# --- 初始化支付宝客户端 ---
def get_alipay_client():
    try:
        with open(ALIPAY_CONFIG['APP_PRIVATE_KEY_PATH']) as f:
            app_private_key_string = f.read()
        with open(ALIPAY_CONFIG['ALIPAY_PUBLIC_KEY_PATH']) as f:
            alipay_public_key_string = f.read()
    except FileNotFoundError as e:
        print(f"错误: 找不到密钥文件。 {e}")
        exit()

    alipay = AliPay(
        appid=ALIPAY_CONFIG['APP_ID'],
        app_notify_url=None,
        app_private_key_string=app_private_key_string,
        alipay_public_key_string=alipay_public_key_string,
        sign_type=ALIPAY_CONFIG['SIGN_TYPE'],
        debug=ALIPAY_CONFIG['DEBUG']
    )
    return alipay

# --- API 路由 ---


@app.route('/', methods=['GET'])
def health_check():
    """一个简单的健康检查/诊断接口"""
    print("收到了来自公网的健康检查请求！穿透成功！")
    return "Brotato-Server is running!"


@app.route('/api/create-payment', methods=['POST'])
def create_payment():
    data = request.get_json()
    out_trade_no = data.get('outTradeNo')
    # 在创建订单时，就初始化它的状态
    payment_statuses[out_trade_no] = "WAIT_BUYER_PAY"  # 初始状态：等待支付
    print(f"创建订单 {out_trade_no}，初始状态: {payment_statuses[out_trade_no]}")

    alipay = get_alipay_client()
    notify_url = PUBLIC_DOMAIN + '/api/payment-notify'
    order_string = alipay.api_alipay_trade_page_pay(
        out_trade_no=out_trade_no,
        total_amount=float(data.get('price')),
        subject=data.get('subject'),
        notify_url=notify_url
    )
    payment_url = ALIPAY_CONFIG['GATEWAY'] + '?' + order_string

    # 用支付链接生成二维码
    qr = qrcode.QRCode(version=1, box_size=10, border=5)
    qr.add_data(payment_url)
    qr.make(fit=True)
    img = qr.make_image(fill_color="black", back_color="white")

    # 将二维码图片转换为 Base64 字符串
    buffered = BytesIO()
    img.save(buffered, format="PNG")
    img_str = base64.b64encode(buffered.getvalue()).decode("utf-8")

    print("已成功生成二维码")
    # 将二维码数据和原始链接一起返回给客户端
    return jsonify({
        "paymentUrl": payment_url,  # 用于“在浏览器中打开”的备用方案
        "qrCodeBase64": img_str    # 二维码图片数据
    })


@app.route('/api/payment-notify', methods=['POST'])
def payment_notify():
    """接收支付宝的异步通知"""
    # 获取所有POST过来的数据
    data = request.form.to_dict()
    print(f"收到支付宝异步通知: {data}")

    # 2. 验证签名
    signature = data.pop("sign", None)
    alipay = get_alipay_client()
    # verify() 会自动验证签名是否正确
    success = alipay.verify(data, signature)

    if success and data.get("trade_status") in ("TRADE_SUCCESS", "TRADE_FINISHED"):
        out_trade_no = data.get('out_trade_no')
        print(f"订单 {out_trade_no} 支付成功！正在更新状态...")
        # 更新模拟数据库中的状态
        payment_statuses[out_trade_no] = "TRADE_SUCCESS"
        print(f"订单 {out_trade_no} 状态已更新为: {payment_statuses[out_trade_no]}")
        # 在这里执行解锁DLC等业务逻辑...
        return "success", 200
    else:
        # 签名验证失败
        print("签名验证失败。")
        return "failure", 400


@app.route('/api/check-payment-status', methods=['GET'])
def check_payment_status():
    out_trade_no = request.args.get('outTradeNo')
    if not out_trade_no:
        return jsonify({"error": "缺少订单号"}), 400
    print(f"查询订单 {out_trade_no} 的状态...")
    status = payment_statuses.get(out_trade_no, "NOT_FOUND")
    print(f"客户端查询订单 {out_trade_no} 状态，当前状态为: {status}")

    return jsonify({"trade_status": status})


# --- 启动服务器 ---
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
