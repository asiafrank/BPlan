#include "stdafx.h"
#include "bdecode.h"

/*--------- outside definition ----------*/

wstring StringNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();

    // collect data
    // get string length
    wostringstream wss;
    for (;; ++it)
    {
        checkEnd();
        wchar_t c = *it;
        if (c == ':')
            break;
        wss << c;
    }

    length = stoi(wss.str());
    wss.clear();
    wss.str(L"");

    // get string content
    for (size_t i = 0; i < length; ++i)
    {
        ++it; // when i = 0, skip ':'; when i == length, not iterate next.

        checkEnd();
        wss << *it;
    }

    value = wss.str();
    collected = true;
    return value;
}

wstring IntegerNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();
    ++it; // skip 'i'

    // collect data
    wostringstream wss;
    for (;; ++it)
    {
        checkEnd();
        wchar_t c = *it;
        if (c == 'e') // int end
            break;
        wss << c;
    }

    value = wss.str();
    intValue = stoi(value);
    collected = true;
    return value;
}

// 如果 Children 是 ListNode 或 DictNode 则相当于递归
wstring ListNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();
    ++it; // skip 'l'

    // collect data
    wostringstream wss;
    wss << L"[";

    for (;; ++it)
    {
        checkEnd();
        wchar_t c = *it;
        if (c == 'e') // list end
            break;

        switch (c)
        {
        case 'i': // integer
        {
            shared_ptr<IntegerNode> pIntNode = make_shared<IntegerNode>(pctx);
            addChild(pIntNode);
            wss << pIntNode->getValue() << L",";
        }
        break;
        case 'l': // list
        {
            shared_ptr<ListNode> pListNode = make_shared<ListNode>(pctx);
            addChild(pListNode);
            wss << pListNode->getValue() << L",";
        }
        break;
        case 'd': // dict
        {
            shared_ptr<DictNode> pDictNode = make_shared<DictNode>(pctx);
            addChild(pDictNode);
            wss << pDictNode->getValue() << L",";
        }
        break;
        default:  // string
        {
            if (iswdigit(c))
            {
                shared_ptr<StringNode> pStrNode = make_shared<StringNode>(pctx);
                addChild(pStrNode);
                wss << L"\"" << pStrNode->getValue() << L"\",";
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
    wss.seekp(-1, wss.cur);
    wss << L"]";
    value = wss.str();
    collected = true;
    return value;
}

// 如果 Children 包含 ListNode 或 DictNode 则递归
wstring DictNode::getValue()
{
    if (collected)
        return value;

    auto& it = pctx->getCurrentIterator();
    ++it; // skip 'd'

    wostringstream wss;
    wss << L"{";

    for (;; ++it)
    {
        checkEnd();
        wchar_t c = *it;
        if (c == 'e') // dict end
            break;

        // get key
        shared_ptr<StringNode> pKeyNode = make_shared<StringNode>(pctx);
        wstring key = pKeyNode->getValue();
        wss << L"\"" << key << L"\":";

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
            wstring ws = pIntNode->getValue(); // 先调用 getValue 才能放入 dict 中
            dict[pKeyNode] = pIntNode;
            wss << ws << L",";
        }
        break;
        case 'l': // list
        {
            shared_ptr<ListNode> pListNode = make_shared<ListNode>(pctx);
            addChild(pListNode);
            dict[pKeyNode] = pListNode;
            wss << pListNode->getValue() << L",";
        }
        break;
        case 'd': // dict
        {
            shared_ptr<DictNode> pDictNode = make_shared<DictNode>(pctx);
            addChild(pDictNode);
            dict[pKeyNode] = pDictNode;
            wss << pDictNode->getValue() << L",";
        }
        break;
        default:  // string
        {
            if (iswdigit(c))
            {
                shared_ptr<StringNode> pStrNode = make_shared<StringNode>(pctx);
                addChild(pStrNode);
                dict[pKeyNode] = pStrNode;
                wss << L"\"" << pStrNode->getValue() << L"\",";
            }
            else
            {
                throw exception("unkown char for NodeType " + c);
            }
        }
        break;
        }
    }

    // replace last ',' to '}'
    wss.seekp(-1, wss.cur);
    wss << L"}";
    value = wss.str();
    collected = true;
    return value;
}

void bdecode(wstring str)
{
    if (str.empty())
        return;

    shared_ptr<wstring> pstr = make_shared<wstring>(str);
    shared_ptr<Context> pctx = make_shared<Context>(pstr);

    shared_ptr<Node> proot;
    wchar_t first = *(pctx->getCurrentIterator());
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

    wstring result = proot->getValue();
    wcout << result << endl;
}
