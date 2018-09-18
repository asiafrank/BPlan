#include "stdafx.h"
#include <fstream>

#include "TorrentInfo.h"
#include "bdecode.h"
#include "hex.h"
#include "hash.h"

namespace bplan
{
    const std::vector<std::string>& TorrentInfo::getPieces()
    {
        if (!pieces.empty())
            return pieces;

        ostringstream buf;
        size_t count = 1;
        string::const_iterator it = pieces_raw.cbegin();
        string::const_iterator end = pieces_raw.cend();

        for (; it != end; ++it, ++count)
        {
            char c = *it;
            buf << c;
            if (count == 20)
            {
                count = 0;
                pieces.push_back(buf.str());
                buf.clear();
                buf.str("");
            }
        }
        return pieces;
    }

    const std::vector<std::string>& TorrentInfo::getPiecesHex()
    {
        if (!pieces_hex.empty())
            return pieces_hex;

        ostringstream buf;
        size_t count = 1;
        string::const_iterator it = pieces_raw.cbegin();
        string::const_iterator end = pieces_raw.cend();

        for (; it != end; ++it, ++count)
        {
            char c = *it;
            buf << c;
            if (count == 20)
            {
                count = 0;
                string hexStr = to_hex(buf.str());
                pieces_hex.push_back(hexStr);
                buf.clear();
                buf.str("");
            }
        }
        return pieces_hex;
    }

    const std::string& TorrentInfo::str()
    {
        if (!formatedStr.empty()) return formatedStr;

        ostringstream oss;
        string indent = "  ";      // 2 whitespace
        string indent2 = "    ";   // 4 whitespace
        string indent3 = "      "; // 6 whitespace
        oss << "announce: \"" << announce << "\"" << endl;
        oss << "announce-list:[";
        for (auto& list : announceList)
        {
            oss << endl << indent << "[";
            for (auto& i : list)
            {
                oss << "\"" << i << "\",";
            }
            // replace last ',' to '],'
            oss.seekp(-1, oss.cur);
            oss << "],";
        }
        // replace last ',' to endl
        oss.seekp(-1, oss.cur);
        oss << endl << "]" << endl;

        oss << "comment:\"" << comment << "\"" << endl;
        oss << "created by: \"" << createdBy << "\"" << endl;
        oss << "creation date: " << creationDate << endl;
        oss << "info:" << endl;
        oss << indent << "info hash: " << info_sha1_hex << endl;
        oss << indent << "name: " << "\"" << name << "\"" << endl;
        oss << indent << "md5sum: " << "\"" << md5sum << "\"" << endl;
        oss << indent << "piece length: " << pieceLength << endl;
        oss << indent << "pieces:[";

        auto& piecesHexlist = getPiecesHex();
        for (auto& s : piecesHexlist)
        {
            oss << endl;
            oss << indent << indent << "\"" << s << "\",";
        }
        // replace last ',' to endl
        oss.seekp(-1, oss.cur);
        oss << endl << indent << "]" << endl;

        oss << indent << "files:[";
        if (!files.empty()) {
            for (auto& f : files)
            {
                oss << endl;
                oss << indent2 << "{" << endl;
                oss << indent3 << "length:" << f.getLength() << endl;
                oss << indent3 << "md5sum:" << f.getMD5sum() << endl;

                oss << indent3 << "path:[";
                auto& path = f.getPath();
                for (auto& p : path)
                {
                    oss << "\"" << p << "\",";
                }
                // replace last ',' to ']'
                oss.seekp(-1, oss.cur);
                oss << "]";
                oss << indent2 << "},";
            }
            // replace last ',' to endl
            oss.seekp(-1, oss.cur);
        }
        oss << endl << indent << "]" << endl;

        formatedStr = oss.str();
        return formatedStr;
    }

    /* throw exception when not found or not valid */
    template <class T>
    shared_ptr<T> getNode(const string& key, const PDNode& pdict, NodeType type)
    {
        PNode pnode = pdict->findNode(key);
        if (!pnode) {
            thrExpt(key, " node not found");
        }
        NodeType t = pnode->getType();
        if (t != type) {
            thrExpt(t, " not valid, it must be ", type);
        }
        return static_pointer_cast<T>(pnode);
    }

    /* return empty shared_prt when not found or not valid */
    template <class T>
    shared_ptr<T> getOptionalNode(const string& key, const PDNode& pdict, NodeType type)
    {
        shared_ptr<T> p; // empty point
        PNode pnode = pdict->findNode(key);
        if (!pnode) {
            return p;
        }
        NodeType t = pnode->getType();
        if (t != type) {
            return p;
        }
        return static_pointer_cast<T>(pnode);
    }

    std::shared_ptr<TorrentInfo> makeTorrentInfo(const std::string& filename)
    {
        shared_ptr<TorrentInfo> pInfo;
        ifstream file{ filename, ios::binary };
        if (!file) // file open failed
            return pInfo;

        file >> std::noskipws; // no skip whitespace
        ostringstream ss;
        for (char n; file >> n;) {
            ss << n;
        }

        string s = ss.str();
        if (s.empty()) // file content empty
            return pInfo;

        try {
            PNode proot = bdecode(s);
            NodeType t = proot->getType();
            if (t != T_Dict)
                thrExpt("root node is not a dict");

            PDNode pdict = static_pointer_cast<DictNode>(proot);
            // get 'announce'
            PSNode pannounce = getNode<StringNode>("announce", pdict, T_String);

            pInfo = make_shared<TorrentInfo>();
            pInfo->announce = pannounce->getValue();

            // get 'announce-list'
            PLNode announceList = getNode<ListNode>("announce-list", pdict, T_List);
            const vector<PNode>& children = announceList->getChildren();

            vector<vector<string>> list;
            for (auto& p : children)
            {
                vector<string> announceItem;
                PLNode subListNode = static_pointer_cast<ListNode>(p);
                const vector<PNode> subchildren = subListNode->getChildren();
                for (auto& subP : subchildren)
                {
                    // StringNode
                    PSNode subPS = static_pointer_cast<StringNode>(subP);
                    string v = subPS->getValue();
                    announceItem.push_back(v);
                }

                list.push_back(announceItem);
            }
            pInfo->announceList = list;
            // TODO: decode Node.value to fill TorrentInfo

            // get 'created by'
            PSNode pcreatedBy = getOptionalNode<StringNode>("created by", pdict, T_String);
            if (pcreatedBy)
            {
                pInfo->createdBy = pcreatedBy->getValue();
            }

            // get 'comment'
            PSNode pcomment = getOptionalNode<StringNode>("comment", pdict, T_String);
            if (pcomment)
            {
                pInfo->comment = pcomment->getValue();
            }

            // get 'creation date'
            PINode pcreationDate = getOptionalNode<IntegerNode>("creation date", pdict, T_Integer);
            if (pcreationDate)
            {
                pInfo->creationDate = pcreationDate->getValue();
            }

            // get 'encoding'
            PSNode pencoding = getOptionalNode<StringNode>("encoding", pdict, T_String);
            if (pencoding)
            {
                pInfo->encoding = pencoding->getValue();
            }

            // get 'info'
            PDNode pinfoNode = getNode<DictNode>("info", pdict, T_Dict);
            const string infoValueRaw = pinfoNode->getRaw();
            SHA1 checksum;
            checksum.update(infoValueRaw);
            const string hash = checksum.final();
            pInfo->info_sha1_hex = hash;
            pInfo->info_sha1 = from_hex(hash);

            // get 'piece length' of 'info'
            PINode ppieceLength = getNode<IntegerNode>("piece length", pinfoNode, T_Integer);
            pInfo->pieceLength = ppieceLength->getValue();

            // get 'pieces' of 'info'
            PSNode ppieces = getNode<StringNode>("pieces", pinfoNode, T_String);
            pInfo->pieces_raw = ppieces->getValue();

            PINode pprivate = getOptionalNode<IntegerNode>("private", pinfoNode, T_Integer);
            if (pprivate)
            {
                pInfo->xprivate = pprivate->getValue();
            }

            // get 'name' of 'info'
            PSNode pname = getNode<StringNode>("name", pinfoNode, T_String);
            pInfo->name = pname->getValue();

            // get 'length' of 'info', and check Single or Multi
            PINode plength = getOptionalNode<IntegerNode>("length", pinfoNode, T_Integer);
            if (plength) // true, Single mode
            {
                pInfo->mode = FileMode::Single;
                pInfo->length = plength->getValue();

                // get 'md5sum' of 'info'
                PSNode pmd5sum = getOptionalNode<StringNode>("md5sum", pinfoNode, T_String);
                if (pmd5sum)
                {
                    pInfo->md5sum = pmd5sum->getValue();
                }
            }
            else // false, Multi mode
            {
                pInfo->mode = FileMode::Multi;
                // get 'files' of 'info'
                PLNode pfiles = getNode<ListNode>("files", pinfoNode, T_List);
                const vector<PNode> pfileschildren = pfiles->getChildren();

                vector<TFile> files;
                for (auto& pchild : pfileschildren)
                {
                    NodeType t = pchild->getType();
                    if (t != T_Dict)
                    {
                        cout << "file not a dict: " << pchild->getRaw() << endl;
                        continue;
                    }

                    PDNode pfnode = static_pointer_cast<DictNode>(pchild);

                    // get 'path' in 'files'
                    PLNode ppath = getNode<ListNode>("path", pfnode, T_List);
                    const vector<PNode> subchildren = ppath->getChildren();
                    vector<string> path;
                    for (auto& psubchild : subchildren)
                    {
                        PSNode psubchildS = static_pointer_cast<StringNode>(psubchild);
                        path.push_back(psubchildS->getValue());
                    }
                    TFile f;
                    f.path = path;

                    // get 'length' in 'files'
                    PINode pflength = getOptionalNode<IntegerNode>("length", pfnode, T_Integer);
                    if (pflength)
                    {
                        f.length = pflength->getValue();
                    }

                    PSNode pfmd5sum = getOptionalNode<StringNode>("md5sum", pfnode, T_String);
                    if (pfmd5sum)
                    {
                        f.md5sum = pfmd5sum->getValue();
                    }
                }
            }
        }
        catch (const exception& e)
        {
            cout << "exception: " << e.what();
            return pInfo;
        }
        return pInfo;
    };
} // namespace bplan

