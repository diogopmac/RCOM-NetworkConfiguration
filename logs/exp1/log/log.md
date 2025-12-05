## netedu@tux113:~$ sudo route -n
Kernel IP routing table
| Destination     | Gateway         | Genmask         | Flags | Metric | Ref  | Use | Iface |
| ----- | ----- | ------ | ------ | -------- | ---------- | ---------- | ------- |
| 0.0.0.0      |   10.227.20.254  | 0.0.0.0      |   UG   | 100   | 0       | 0 | if_mng |
| 10.227.20.0  |   0.0.0.0       |  255.255.255.0  | U   |  100  |  0     |   0 | if_mng |
| 172.16.110.0  |  0.0.0.0     |    255.255.255.0  | U  |   0  |    0   |     0 | if_e1 |

## netedu@tux113:~$ sudo arp -a
dns.netlab.fe.up.pt (10.227.20.2) at bc:24:11:f5:a2:54 [ether] on if_mng
posto11.netlab.fe.up.pt (10.227.20.115) at 20:7c:14:a1:64:e8 [ether] on if_mng
_gateway (10.227.20.254) at e4:8d:8c:20:25:c8 [ether] on if_mng
fog.netlab.fe.up.pt (10.227.20.223) at bc:24:11:f8:0d:71 [ether] on if_mng
ns1.netlab.fe.up.pt (10.227.20.3) at bc:24:11:e7:5e:5b [ether] on if_mng
? (172.16.110.254) at ec:75:0c:c2:3c:96 [ether] on if_e1