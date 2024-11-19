import socket
import struct
import os
from datetime import datetime

# UDP服务器配置
UDP_IP = "0.0.0.0"  # 监听所有接口
UDP_PORT = 3333

# 创建保存图片的目录
if not os.path.exists('received_images'):
    os.makedirs('received_images')

def main():
    # 创建UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))
    print(f"UDP server listening on port {UDP_PORT}")

    while True:
        try:
            print("\n等待接收新图像...")
            # 接收图像大小
            data, addr = sock.recvfrom(8)
            image_size = struct.unpack('Q', data)[0]
            print(f"检测到新图像，大小: {image_size} bytes，来自: {addr}")

            # 接收图像数据
            received_data = bytearray()
            start_time = datetime.now()
            
            while len(received_data) < image_size:
                packet, addr = sock.recvfrom(1500)
                received_data.extend(packet)
                if len(received_data) % (50*1400) == 0:  # 每50个包打印一次进度
                    print(f"接收进度: {len(received_data)}/{image_size} bytes "
                          f"({len(received_data)*100/image_size:.1f}%)")

            # 计算接收用时
            time_used = (datetime.now() - start_time).total_seconds()
            print(f"图像接收完成！用时: {time_used:.2f}秒")

            # 保存图像
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"received_images/image_{timestamp}.jpg"
            with open(filename, 'wb') as f:
                f.write(received_data)
            print(f"图像已保存为: {filename}")

        except Exception as e:
            print(f"错误: {e}")

if __name__ == "__main__":
    main() 