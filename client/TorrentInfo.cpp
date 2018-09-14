#include "stdafx.h"
#include "TorrentInfo.h"
#include "bdecode.h"
#include <fstream>

const std::vector<std::string>& TorrentInfo::getHumanReadablePiecesSHA1()
{
    if (!pieces_h.empty())
        return pieces_h;

    ostringstream buf;
    size_t count = 1;
    string::const_iterator it = pieces.cbegin();
    string::const_iterator end = pieces.cend();

    for (; it != end; ++it, ++count)
    {
        char c = *it;
        // big-endian
        char high = readable_base16_high(c); // 取高位转成 char
        char low = readable_base16_low(c);  // 取低位转成 char
        buf << high;
        buf << low;
        if (count == 20)
        {
            count = 0;
            pieces_h.push_back(buf.str());
            buf.clear();
            buf.str("");
        }
    }
    return pieces_h;
}

const std::string& TorrentInfo::str()
{
    if (!collectedStr.empty()) return collectedStr;

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
    oss << indent << "name: " << "\"" << name << "\"" << endl;
    oss << indent << "md5sum: " << "\"" << md5sum << "\"" << endl;
    oss << indent << "piece length: " << pieceLength << endl;
    oss << indent << "pieces:[";

    auto& piecesHlist = getHumanReadablePiecesSHA1();
    for (auto& s : piecesHlist)
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

    collectedStr = oss.str();
    return collectedStr;
}

char readable_base16_high(char c)
{
    char h = (c & 0xF0) >> 4;
    return readable_base16(h);
}

char readable_base16_low(char c)
{
    char l = (c & 0x0F);
    return readable_base16(l);
}

char readable_base16(char c)
{
    switch (c)
    {
    case 0:
        return '0';
    case 1:
        return '1';
    case 2:
        return '2';
    case 3:
        return '3';
    case 4:
        return '4';
    case 5:
        return '5';
    case 6:
        return '6';
    case 7:
        return '7';
    case 8:
        return '8';
    case 9:
        return '9';
    case 10:
        return 'A';
    case 11:
        return 'B';
    case 12:
        return 'C';
    case 13:
        return 'D';
    case 14:
        return 'E';
    case 15:
        return 'F';
    default:
        break;
    }
    return 0;
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
                string v = subP->getValue();
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
            pInfo->creationDate = pcreationDate->getInt();
        }

        // get 'encoding'
        PSNode pencoding = getOptionalNode<StringNode>("encoding", pdict, T_String);
        if (pencoding)
        {
            pInfo->encoding = pencoding->getValue();
        }

        // get 'info'
        PDNode pinfoNode = getNode<DictNode>("info", pdict, T_Dict);

        // get 'piece length' of 'info'
        PINode ppieceLength = getNode<IntegerNode>("piece length", pinfoNode, T_Integer);
        pInfo->pieceLength = ppieceLength->getInt();

        // get 'pieces' of 'info'
        PSNode ppieces = getNode<StringNode>("pieces", pinfoNode, T_String);
        pInfo->pieces = ppieces->getValue();

        PINode pprivate = getOptionalNode<IntegerNode>("private", pinfoNode, T_Integer);
        if (pprivate)
        {
            pInfo->xprivate = pprivate->getInt();
        }

        // get 'name' of 'info'
        PSNode pname = getNode<StringNode>("name", pinfoNode, T_String);
        pInfo->name = pname->getValue();

        // get 'length' of 'info', and check Single or Multi
        PINode plength = getOptionalNode<IntegerNode>("length", pinfoNode, T_Integer);
        if (plength) // true, Single mode
        {
            pInfo->mode = FileMode::Single;
            pInfo->length = plength->getInt();

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
                    cout << "file not a dict: " << pchild->getValue() << endl;
                    continue;
                }

                PDNode pfnode = static_pointer_cast<DictNode>(pchild);

                // get 'path' in 'files'
                PLNode ppath = getNode<ListNode>("path", pfnode, T_List);
                const vector<PNode> subchildren = ppath->getChildren();
                vector<string> path;
                for (auto& psubchild : subchildren)
                {
                    path.push_back(psubchild->getValue());
                }
                TFile f;
                f.path = path;

                // get 'length' in 'files'
                PINode pflength = getOptionalNode<IntegerNode>("length", pfnode, T_Integer);
                if (pflength)
                {
                    f.length = pflength->getInt();
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
