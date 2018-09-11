#pragma once

#include <memory>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>

/*
数据类型

字符串： <以十进制 ASCII 编码的串长度>：<串数据>
        '4:spam' 代表字符串 'spam'

整数：i<以十进制 ASCII 编码的整数>e
      'i3e' 代表数字 '3'

列表：l<编码值>e
      'l4:spam4:eggse' 代表列表 ['spam','eggs']
我们认为一个 List 中的所有元素都是同一类型的，不会出现包含多种类型元素的List，像 [1, 'str']

字典：d<编码串><编码元素>e
      d4:spaml1:a1:bee 代表字典 {"spam" => ["a", "b"]}

下图是 Node 判断过程，[] 代表 Node，箭头上的文字代表 Event：

             number
         |----------------> [StringNode]
[Start] -|
         |    'i'
         |----------------> [IntegerNode]
         |
         |    'l'
         |----------------> [ListNode]
         |
         |    'd'
         |----------------> [DictNode]


例：针对 'd1:ad1:bi1e1:c4:abcde1:di3ee'，构建成以下树结构：
{
  "a": { "b": 1, "c": "abcd" },
  "d": 3
}
=> Tree：
                                          RootNode
                                             |
                                        +----------+
                                        | DictNode |
                                        +----------+
                                        /         \
                        +------------------+   +--------------------+
                        | Entry<DictNode>  |   | Entry<IntegerNode> |
                        | key = "a"        |   | key = "d"          |
                        | value = DictNode |   | value = 3          |
                        +------------------+   +--------------------+
                                |                        |
                            +----------+               +-----+
                            | DictNode |               | Nil |
                            +----------+               +-----+
                             /     \
        +--------------------+    +-------------------+
        | Entry<IntegerNode> |    | Entry<StringNode> |
        | key = "b"          |    | key = "c"         |
        | value = 1          |    | value = "abcd"    |
        +--------------------+    +-------------------+
                |                          |
            +-----+                    +-----+
            | Nil |                    | Nil |
            +-----+                    +-----+

其中 DictNode 中 Entry 自己也能构成一棵树
*/
using namespace std;

enum NodeType
{
    T_Nil = 0,
    T_Root,
    T_String,
    T_Integer,
    T_List,
    T_Dict
};

/* 解析过程中的上下文 */
class Context
{
public:
    Context(shared_ptr<wstring> data) : data(data),
        currIt(data->cbegin()),
        end(data->cend())
    {}

    wstring::const_iterator& getCurrentIterator()
    { return currIt; }

    const wstring::const_iterator& getEndIterator()
    { return end; }
private:
    shared_ptr<wstring> data;
    wstring::const_iterator currIt;
    const wstring::const_iterator end;
};

/*
节点的 base class
T_String,T_Integer 的节点必定是叶子节点
T_List,T_Dict 的节点必定不是叶子节点
*/
class Node
{
public:
    Node(shared_ptr<Context> pctx, NodeType type)
        : pctx(pctx), type(type), children(), value()
    {}

    NodeType getType() { return type; };
    void addChild(shared_ptr<Node> pnode) { children.push_back(pnode); };
    bool leaf() { return children.empty(); };
    void checkEnd()
    {
        if (pctx->getCurrentIterator() == pctx->getEndIterator())
            throw exception("ERROR: iterate end before get value complete");
    };

    virtual wstring getValue() { return value; };
protected:
    shared_ptr<Context> pctx;
    NodeType type;
    vector<shared_ptr<Node>> children;
    wstring value;
};

/* T_String 的 Node */
class StringNode : public Node
{
public:
    StringNode(shared_ptr<Context> pctx)
        : Node(pctx, T_String), length(0), collected(false)
    {}

    virtual wstring getValue() override;

    // for unordered_map
    bool operator==(const StringNode& rhs) const {
        return value == rhs.value;
    }
private:
    bool collected; // 是否已经收集过数据
    size_t length;
};

struct StringNodePtrEq
{
    bool operator()(const shared_ptr<StringNode>& lhs,
        const shared_ptr<StringNode>& rhs) const
    {
        return lhs->getValue() == rhs->getValue();
    }
};

struct StringNodeHash
{
    std::size_t operator()(const shared_ptr<StringNode>& p) const noexcept
    {
        return hash<wstring>{}(p->getValue());
    }
};

/* T_Integer 的 Node */
class IntegerNode : public Node
{
public:
    IntegerNode(shared_ptr<Context> pctx)
        : Node(pctx, T_Integer), collected(false), intValue(0)
    {}

    virtual wstring getValue() override;

    int getInt()
    {
        return intValue;
    }
private:
    bool collected;
    int intValue;
};


// 前置声明
class DictNode;

/* T_List 的 Node */
class ListNode : public Node
{
public:
    ListNode(shared_ptr<Context> pctx)
        : Node(pctx, T_List), collected(false)
    {}

    virtual wstring getValue() override;
private:
    bool collected;
};

/* T_Dict 的 Node */
class DictNode : public Node
{
public:
    DictNode(shared_ptr<Context> pctx)
        : Node(pctx, T_Dict), collected(false)
    {}

    virtual wstring getValue() override;
private:
    bool collected;
    unordered_map<shared_ptr<StringNode>, shared_ptr<Node>, StringNodeHash, StringNodePtrEq> dict;
};

void bdecode(wstring str);
