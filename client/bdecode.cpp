#include "stdafx.h"
#include "bdecode.h"

namespace bplan
{
    /*--------- outside definition ----------*/

    void StringNode::collect()
    {
        if (collected) return;

        auto& it = pctx->getCurrentIterator();

        // get string length
        ostringstream rawss;
        ostringstream valuess;
        for (;; ++it)
        {
            checkEnd();
            char c = *it;
            rawss << c;
            if (c == ':')
                break;
            valuess << c;
        }

        length = stoull(valuess.str());
        valuess.clear();
        valuess.str("");

        // get string content
        for (size_t i = 0; i < length; ++i)
        {
            ++it; // when i = 0, skip ':'; when i == length, not iterate next.

            checkEnd();
            rawss << *it;
            valuess << *it;
        }

        value = valuess.str();
        raw = rawss.str();
        formatedStr = value;
        collected = true;
    }

    void IntegerNode::collect()
    {
        if (collected) return;

        auto& it = pctx->getCurrentIterator();

        ostringstream rawss;
        ostringstream valuess;

        rawss << *it;
        ++it; // skip 'i'

        for (;; ++it)
        {
            checkEnd();
            char c = *it;
            rawss << c;
            if (c == 'e') // int end
                break;
            valuess << c;
        }

        raw = rawss.str();
        string valueStr = valuess.str();
        value = stoull(valueStr);
        formatedStr = valueStr;
        collected = true;
    }

    // 如果 Children 是 ListNode 或 DictNode 则相当于递归
    void ListNode::collect()
    {
        if (collected) return;

        auto& it = pctx->getCurrentIterator();

        ostringstream rawss;
        ostringstream formatedss;

        rawss << *it;
        ++it; // skip 'l'

              // collect data
        formatedss << "[";

        for (;; ++it)
        {
            checkEnd();
            char c = *it;
            if (c == 'e') // list end
            {
                rawss << c;
                break;
            }

            switch (c)
            {
            case 'i': // integer
            {
                shared_ptr<IntegerNode> pIntNode = make_shared<IntegerNode>(pctx);
                addChild(pIntNode);
                pIntNode->collect();
                rawss << pIntNode->getRaw();
                formatedss << pIntNode->str() << ",";
            }
            break;
            case 'l': // list
            {
                shared_ptr<ListNode> pListNode = make_shared<ListNode>(pctx);
                addChild(pListNode);
                pListNode->collect();
                rawss << pListNode->getRaw();
                formatedss << pListNode->str() << ",";
            }
            break;
            case 'd': // dict
            {
                shared_ptr<DictNode> pDictNode = make_shared<DictNode>(pctx);
                addChild(pDictNode);
                pDictNode->collect();
                rawss << pDictNode->getRaw();
                formatedss << pDictNode->str() << ",";
            }
            break;
            default:  // string
            {
                if (iswdigit(c))
                {
                    shared_ptr<StringNode> pStrNode = make_shared<StringNode>(pctx);
                    addChild(pStrNode);
                    pStrNode->collect();
                    rawss << pStrNode->getRaw();
                    formatedss << "\"" << pStrNode->str() << "\",";
                }
                else
                {
                    throw exception("unkown char for NodeType " + c);
                }
            }
            break;
            }
        }

        raw = rawss.str();

        // replace last ',' to ']'
        formatedss.seekp(-1, formatedss.cur);
        formatedss << "]";
        formatedStr = formatedss.str();
        collected = true;
    }

    // 如果 Children 包含 ListNode 或 DictNode 则递归
    void DictNode::collect()
    {
        if (collected) return;

        auto& it = pctx->getCurrentIterator();

        ostringstream rawss;
        ostringstream formatedss;

        rawss << *it;
        ++it; // skip 'd'

        formatedss << "{";

        for (;; ++it)
        {
            checkEnd();
            char c = *it;
            // no need add to rawss
            if (c == 'e') // dict end
            {
                rawss << c;
                break;
            }

            // get key
            shared_ptr<StringNode> pKeyNode = make_shared<StringNode>(pctx);
            pKeyNode->collect();
            string key = pKeyNode->getValue();
            rawss << pKeyNode->getRaw();
            formatedss << "\"" << key << "\":";
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
                pIntNode->collect();
                dict[key] = pIntNode;
                rawss << pIntNode->getRaw();
                formatedss << pIntNode->str() << ",";
            }
            break;
            case 'l': // list
            {
                shared_ptr<ListNode> pListNode = make_shared<ListNode>(pctx);
                addChild(pListNode);
                pListNode->collect();
                dict[key] = pListNode;
                rawss << pListNode->getRaw();
                formatedss << pListNode->str() << ",";
            }
            break;
            case 'd': // dict
            {
                shared_ptr<DictNode> pDictNode = make_shared<DictNode>(pctx);
                addChild(pDictNode);
                pDictNode->collect();
                dict[key] = pDictNode;
                rawss << pDictNode->getRaw();
                formatedss << pDictNode->str() << ",";
            }
            break;
            default:  // string
            {
                if (iswdigit(c))
                {
                    shared_ptr<StringNode> pStrNode = make_shared<StringNode>(pctx);
                    addChild(pStrNode);
                    pStrNode->collect();
                    dict[key] = pStrNode;
                    rawss << pStrNode->getRaw();
                    formatedss << "\"" << pStrNode->str() << "\",";
                }
                else
                {
                    throw exception("unkown char for NodeType " + c);
                }
            }
            break;
            }
        }

        raw = rawss.str();

        // replace last ',' with '}'
        formatedss.seekp(-1, formatedss.cur);
        formatedss << "}";
        formatedStr = formatedss.str();
        collected = true;
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

        proot->collect(); // invoke content decode chain
        return proot;
    }
} // namespace bplan

