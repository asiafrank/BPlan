#pragma once

/*
一般正常的 torrent 文件的 meta info，其根数据都是一个字典。
Single File Mode
下面以 “Shingeki no Kyojin Season 3 08 [BF69427F].mp4” 的 torrent 内容做例子，
其内容如下（其文件在 test_file/1b4c128f826511623d2bf73f71f7208eaa2dd135.torrent）：
{
    "announce": "http://tr.bangumi.moe:6969/announce",
    "announce-list": [["http://tr.bangumi.moe:6969/announce"], ["http://t.nyaatracker.com/announce"], ["http://open.acgtracker.com:1096/announce"], ["http://open.nyaatorrents.info:6544/announce"], ["http://t2.popgo.org:7456/annonce"], ["http://share.camoe.cn:8080/announce"], ["http://opentracker.acgnx.se/announce"], ["http://tracker.acgnx.se/announce"], ["http://nyaa.tracker.wf:7777/announce"], ["http://tracker.anirena.com:80/announce"], ["udp://tracker.opentrackr.org:1337/announce"], ["udp://tracker.coppersurfer.tk:6969/announce"], ["udp://tracker.doko.moe:6969"], ["udp://tracker.zer0day.to:1337/announce"], ["udp://tracker.openbittorrent.com:80/announce"], ["udp://tracker.publicbt.com:80/announce"], ["udp://tracker.prq.to:80/announce"], ["udp://104.238.198.186:8000/announce"], ["http://104.238.198.186:8000/announce"], ["http://94.228.192.98/announce"], ["http://share.dmhy.org/annonuce"], ["http://tracker.btcake.com/announce"], ["http://tracker.ktxp.com:6868/announce"], ["http://tracker.ktxp.com:7070/announce"], ["http://bt.sc-ol.com:2710/announce"], ["http://btfile.sdo.com:6961/announce"], ["https://t-115.rhcloud.com/only_for_ylbud"], ["http://exodus.desync.com:6969/announce"], ["udp://coppersurfer.tk:6969/announce"], ["http://tracker3.torrentino.com/announce"], ["http://tracker2.torrentino.com/announce"], ["udp://open.demonii.com:1337/announce"], ["udp://tracker.ex.ua:80/announce"], ["http://pubt.net:2710/announce"], ["http://tracker.tfile.me/announce"], ["http://bigfoot1942.sektori.org:6969/announce"], ["udp://bt.sc-ol.com:2710/announce"]],
    "comment": "By `c-a Raws`",
    "creation date": 1536516922,
    "info": {
        "length": 967112391,
        "name": "Shingeki no Kyojin Season 3 08 [BF69427F].mp4",
        "piece length": 262144,
        "pieces": "<ignore sha1 code>"
    }
}

Multi File Mode
test_file/sample.torrent 为例：
{
    "announce": "udp://tracker.opentracker.com:80/announce",
    "announce-list": [["udp://tracker.opentracker.com:80/announce"], ["tracker.publicbt.com:80/announce"]],
    "comment": "sample comment",
    "created by": "libtorrent",
    "creation date": 1418787579,
    "info": {
        "files": [{
                "length": 25,
                "path": ["text_file2.txt"]
            }, {
                "attr": "p",
                "length": 16359,
                "path": [".____padding_file", "0"]
            }, {
            "length": 20,
                "path": ["text_file.txt"],
                "sha1": "abababababababababab"
            }
        ],
        "name": "sample",
        "piece length": 16384,
        "pieces": "<ignore sha1 code>"
    }
}

详情见：https://wiki.theory.org/index.php/BitTorrentSpecification#Metainfo_File_Structure
*/

#include <string>
#include <vector>

// 前置声明
class TorrentInfo;

/*
解析 .torrent 文件，返回 TorrentInfo 对象

filename: 文件名，绝对路径或者相对路径，如 "../test_file/sample.torrent"

return shared_ptr<TorrentInfo>, 如果 if 判断为 false 则表示解析失败
Example:
shared_ptr<TorrentInfo> pInfo = decode_to_torrent_info("sample.torrent");
if (pInfo)
    cout << "decode success" << endl;
else
    cout << "decode failed" << endl;
*/
std::shared_ptr<TorrentInfo> makeTorrentInfo(const std::string& filename);

enum FileMode
{
    None = 0, // 还没解析的时候，判断为 None
    Single,   // 单文件模式
    Multi     // 多文件模式
};

/*
FileMode = Multi 时, TorrentInfo 中的文件列表对象
*/
class TFile
{
    friend std::shared_ptr<TorrentInfo> makeTorrentInfo(const std::string& filename);
public: 
    const std::uint64_t& getLength() { return length; };
    const std::vector<std::string>& getPath() { return path; };
    const std::string& getMD5sum() { return md5sum; };
private:
    std::uint64_t length;
    std::vector<std::string> path; // ["dir1", "dir2", "file.ext"] 代表 "dir1/dir2/file.ext", 
                                   // B编码为 "l4:dir14:dir28:file.exte"
    std::string md5sum;   // optional, 32-character hexadecimal string
};

class TorrentInfo
{
    friend std::shared_ptr<TorrentInfo> makeTorrentInfo(const std::string& filename);
public:
    /*
    throw exception when decode failed
    */
    TorrentInfo() {};
    ~TorrentInfo() {};
public:
    const FileMode getMode() const { return mode; };
    const std::string& getAnnounce() const { return announce; };
    const std::vector<std::vector<std::string>>& getAnnounceList() const { return announceList; };
    const std::uint64_t& getCreationDate() const { return creationDate; };
    const std::string& getComment() const { return comment; };
    const std::string& getCreatedBy() const { return createdBy; };
    const std::string& getEncoding() const { return encoding; };

    const std::uint64_t& getPieceLength() const { return pieceLength; };
    const std::string& getPieces() const { return pieces; };
    const std::size_t& getPrivate() const { return xprivate; };

    const std::string& getName() const { return name; };
    const std::uint64_t& getLength() const { return length; };
    const std::string& getMD5sum() const { return md5sum; };
    const std::vector<TFile>& getFiles() const { return files; };

    /* 将 20-byte SHA1 串转换成可读的形式 */
    const std::vector<std::string>& getHumanReadablePiecesSHA1();
    /*
    将各属性的值收集成 str，方便打印。格式如下：
    announce: ""
    announce-list: [
      [""],
      [""]
    ]
    comment: ""
    created by: ""
    creation date: 10000
    info:
        name: ""
        piece length: 10000
        pieces: [
          "",
          ""
        ]
        files: [
          {
            "length": 1000,
            "path": [""]
          }
        ]
    */
    const std::string& str();
private:
    FileMode mode;
    std::string announce;
    std::vector<std::vector<std::string>> announceList; // optional
    std::uint64_t creationDate;            // optional, seconds since 1-Jan-1970 00:00:00 UTC
    std::string comment;                   // optional
    std::string createdBy;                 // optional
    std::string encoding;                  // optional, TODO: 学习下这个什么用
private:
    // info 中的内容
    std::uint64_t pieceLength; // number of bytes in each piece
    std::string pieces;        // string consisting of the concatenation of all 20-byte SHA1 hash values, 
                               // one per piece (byte string, i.e. not urlencoded)
    std::vector<std::string> pieces_h; // human readable pieces code
    std::size_t xprivate;      // "private": 0,1
    
    std::string name;          // 如果是 Single，则表示文件名；如果是 Multi，则表示文件夹名
    
    std::uint64_t length;      // Single, length of the file in bytes
    std::string md5sum;        // Single, optional, 32-character hexadecimal string

    std::vector<TFile> files;  // Multi, 文件列表
private:
    std::string collectedStr;
};


