# BPlan
尝试实现 bittorrent 协议。制作一个简单 GUI bt 下载器。

## TODO List（先不管跨平台问题）
- [x] metainfo 文件解析
- [ ] 基本下载
- [ ] Direct2D 做 GUI
- [ ] DHT 支持
- [ ] μTP 支持
- [ ] 两个模块：server 和 client。server 负责 bittorrent 协议实现，client 负责查询 server 状态及设置server参数。用 asio 负责 tcp 通信
- [ ] 自定义追番表


## Dependencies
- [asio](https://think-async.com/Asio)
- [nowide](https://github.com/nephatrine/nowide-standalone)


## References
- [The BitTorrent Protocol Specification](http://www.bittorrent.org/beps/bep_0003.html)
- [BitTorrentSpecification](https://wiki.theory.org/index.php/BitTorrentSpecification)
- [BitTorrentSpecification 中文](https://zh.wikibooks.org/wiki/BitTorrent%E5%8D%8F%E8%AE%AE%E8%A7%84%E8%8C%83)
- [UTF8 everywhere](http://utf8everywhere.org/zh-cn)
