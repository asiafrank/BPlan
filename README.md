# BPlan
尝试实现 bittorrent 协议。制作一个shell版 bt 下载器。

## TODO List（先不管跨平台问题）
- 两个模块：server 和 client。server 负责 bittorrent 协议实现，client 负责查询 server 状态及设置server参数。用 asio 负责 tcp 通信
- 自定义追番表
- future: Direct3D 图形化 client。


## Dependencies
- [asio](https://think-async.com/Asio)

## References
- [The BitTorrent Protocol Specification](http://www.bittorrent.org/beps/bep_0003.html)
