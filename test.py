from scapy.all import *

# 目标IP和端口
source_ip = '192.168.240.136'
target_ip = '192.168.182.137'
target_port = 80

payload_a = b'|0a|.pl'

a = IP()
a.src = source_ip
a.dst = target_ip
a.flags = 0
a.frag = 0
a.proto = 6
a /= TCP(dport=target_port, sport=1234)  # 设置TCP头部
a /= payload_a  # 添加第一个分片的负载

a.show()
send(a)
