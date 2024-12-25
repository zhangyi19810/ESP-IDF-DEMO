import socket
import struct
import cv2
import numpy as np
from datetime import datetime
import os

# UDP配置
UDP_PORT = 3333
PACKET_SIZE = 96*3*8
MAX_PACKETS = 13

# 包头格式
PACKET_HEADER_FORMAT = '<BBB'  # 小端序，3个uint8: seq_num, total_packets, payload_length

class PacketHeader:
    def __init__(self, data):
        # 解析包头
        (
            self.seq_num,
            self.total_packets,
            self.payload_length
        ) = struct.unpack(PACKET_HEADER_FORMAT, data)

def main():
    # 创建保存图片的目录
    if not os.path.exists('captured_images'):
        os.makedirs('captured_images')

    # 创建UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('0.0.0.0', UDP_PORT))
    print(f"UDP服务器启动在端口 {UDP_PORT}")

    # 帧缓存
    frame_buffer = bytearray(PACKET_SIZE * (MAX_PACKETS - 1))  # 减1是因为最后一个包是结果包
    received_packets = set()
    header_size = struct.calcsize(PACKET_HEADER_FORMAT)
    frame_count = 0

    while True:
        try:
            # 接收数据包
            data, addr = sock.recvfrom(PACKET_SIZE + header_size)
            
            # 如果是结果包（固定4字节）
            if len(data) == 4:
                seq_num, total_packets, _, result = struct.unpack('BBBB', data)
                if seq_num == MAX_PACKETS and total_packets == MAX_PACKETS:
                    print(f"收到结果包，结果值: {result}")
                    
                    # 如果收到了所有数据包，处理图像
                    if len(received_packets) == MAX_PACKETS - 1:
                        try:
                            # 将JPEG数据转换为图像
                            img_array = np.frombuffer(frame_buffer, dtype=np.uint8)
                            img = cv2.imdecode(img_array, cv2.IMREAD_COLOR)
                            
                            if img is not None:
                                # 保存图像
                                timestamp = datetime.now().strftime('%Y%m%d_%H%M%S_%f')
                                filename = f'captured_images/frame_{frame_count}_{timestamp}.jpg'
                                cv2.imwrite(filename, img)
                                print(f"保存图片: {filename}")
                                
                                # 显示图像
                                cv2.imshow('Received Frame', img)
                                if cv2.waitKey(1) & 0xFF == ord('q'):
                                    break
                                
                                frame_count += 1
                            else:
                                print("图像解码失败")
                        except Exception as e:
                            print(f"处理图像时出错: {e}")
                    
                    # 重置接收状态
                    received_packets.clear()
                    continue

            # 解析包头
            header = PacketHeader(data[:header_size])
            
            # 验证包序号
            if header.seq_num < 1 or header.seq_num > MAX_PACKETS - 1:
                print(f"无效的包序号: {header.seq_num}")
                continue

            # 保存数据包
            start_pos = (header.seq_num - 1) * PACKET_SIZE
            frame_buffer[start_pos:start_pos + PACKET_SIZE] = data[header_size:]
            received_packets.add(header.seq_num)
            
            print(f"收到包 {header.seq_num}/{header.total_packets}")

        except Exception as e:
            print(f"接收数据时出错: {e}")

    cv2.destroyAllWindows()
    sock.close()

if __name__ == "__main__":
    main() 