#include "stdafx.h"
#include "bdecode.h"

/*--------- outside definition ----------*/

string StringNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();

    // collect data
    // get string length
    ostringstream ss;
    for (;; ++it)
    {
        checkEnd();
        char c = *it;
        if (c == ':')
            break;
        ss << c;
    }

    length = stoull(ss.str());
    ss.clear();
    ss.str("");

    // get string content
    for (size_t i = 0; i < length; ++i)
    {
        ++it; // when i = 0, skip ':'; when i == length, not iterate next.

        checkEnd();
        ss << *it;
    }

    value = ss.str();
    collected = true;
    return value;
}

string IntegerNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();
    ++it; // skip 'i'

    // collect data
    ostringstream ss;
    for (;; ++it)
    {
        checkEnd();
        char c = *it;
        if (c == 'e') // int end
            break;
        ss << c;
    }

    value = ss.str();
    intValue = stoull(value);
    collected = true;
    return value;
}

// 如果 Children 是 ListNode 或 DictNode 则相当于递归
string ListNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();
    ++it; // skip 'l'

    // collect data
    ostringstream ss;
    ss << "[";

    for (;; ++it)
    {
        checkEnd();
        char c = *it;
        if (c == 'e') // list end
            break;

        switch (c)
        {
        case 'i': // integer
        {
            shared_ptr<IntegerNode> pIntNode = make_shared<IntegerNode>(pctx);
            addChild(pIntNode);
            ss << pIntNode->getValue() << ",";
        }
        break;
        case 'l': // list
        {
            shared_ptr<ListNode> pListNode = make_shared<ListNode>(pctx);
            addChild(pListNode);
            ss << pListNode->getValue() << ",";
        }
        break;
        case 'd': // dict
        {
            shared_ptr<DictNode> pDictNode = make_shared<DictNode>(pctx);
            addChild(pDictNode);
            ss << pDictNode->getValue() << ",";
        }
        break;
        default:  // string
        {
            if (iswdigit(c))
            {
                shared_ptr<StringNode> pStrNode = make_shared<StringNode>(pctx);
                addChild(pStrNode);
                ss << "\"" << pStrNode->getValue() << "\",";
            }
            else
            {
                throw exception("unkown char for NodeType " + c);
            }
        }
        break;
        }
    }

    // replace last ',' to ']'
    ss.seekp(-1, ss.cur);
    ss << "]";
    value = ss.str();
    collected = true;
    return value;
}

// 如果 Children 包含 ListNode 或 DictNode 则递归
string DictNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();
    ++it; // skip 'd'

    ostringstream ss;
    ss << "{";

    for (;; ++it)
    {
        checkEnd();
        char c = *it;
        if (c == 'e') // dict end
            break;

        // get key
        shared_ptr<StringNode> pKeyNode = make_shared<StringNode>(pctx);
        string key = pKeyNode->getValue();
        ss << "\"" << key << "\":";
        pKeyNode = nullptr; // release StringNode obj

        // get value
        checkEnd();
        ++it; // value field
        c = *it;
        if (c == 'e') // illegal end
            break;

        switch (c)
        {
        case 'i': // integer
        {
            shared_ptr<IntegerNode> pIntNode = make_shared<IntegerNode>(pctx);
            addChild(pIntNode);
            string ws = pIntNode->getValue(); // 先调用 getValue 才能放入 dict 中
            dict[key] = pIntNode;
            ss << ws << ",";
        }
        break;
        case 'l': // list
        {
            shared_ptr<ListNode> pListNode = make_shared<ListNode>(pctx);
            addChild(pListNode);
            dict[key] = pListNode;
            ss << pListNode->getValue() << ",";
        }
        break;
        case 'd': // dict
        {
            shared_ptr<DictNode> pDictNode = make_shared<DictNode>(pctx);
            addChild(pDictNode);
            dict[key] = pDictNode;
            ss << pDictNode->getValue() << ",";
        }
        break;
        default:  // string
        {
            if (iswdigit(c))
            {
                shared_ptr<StringNode> pStrNode = make_shared<StringNode>(pctx);
                addChild(pStrNode);
                dict[key] = pStrNode;
                ss << "\"" << pStrNode->getValue() << "\",";
            }
            else
            {
                throw exception("unkown char for NodeType " + c);
            }
        }
        break;
        }
    }

    // replace last ',' with '}'
    ss.seekp(-1, ss.cur);
    ss << "}";
    value = ss.str();
    collected = true;
    return value;
}

shared_ptr<Node> DictNode::findNode(string key)
{
    return dict[key];
}

shared_ptr<Node> bdecode(const string& content)
{
    shared_ptr<string> pcontent = make_shared<string>(content);
    shared_ptr<Context> pctx = make_shared<Context>(pcontent);

    std::shared_ptr<Node> proot;
    char first = *(pctx->getCurrentIterator());
    switch (first)
    {
    case 'i': // integer
    {
        proot = make_shared<IntegerNode>(pctx);
    }
    break;
    case 'l': // list
    {
        proot = make_shared<ListNode>(pctx);
    }
    break;
    case 'd': // dict
    {
        proot = make_shared<DictNode>(pctx);
    }
    break;
    default:  // string
    {
        if (iswdigit(first))
        {
            proot = make_shared<StringNode>(pctx);
        }
        else
        {
            throw exception("unkown char for NodeType " + first);
        }
    }
    break;
    }

    proot->getValue(); // invoke content decode chain, ignore return
    return proot;
}
