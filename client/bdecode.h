#pragma once

#include <memory>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>

/*
��������

�ַ����� <��ʮ���� ASCII ����Ĵ�����>��<������>
        '4:spam' �����ַ��� 'spam'

������i<��ʮ���� ASCII ���������>e
      'i3e' �������� '3'

�б�l<����ֵ>e
      'l4:spam4:eggse' �����б� ['spam','eggs']
������Ϊһ�� List �е�����Ԫ�ض���ͬһ���͵ģ�������ְ�����������Ԫ�ص�List���� [1, 'str']

�ֵ䣺d<���봮><����Ԫ��>e
      d4:spaml1:a1:bee �����ֵ� {"spam" => ["a", "b"]}

��ͼ�� Node �жϹ��̣�[] ���� Node����ͷ�ϵ����ִ��� Event��

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


������� 'd1:ad1:bi1e1:c4:abcde1:di3ee'���������������ṹ��
{
  "a": { "b": 1, "c": "abcd" },
  "d": 3
}
=> Tree��
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

���� DictNode �� Entry �Լ�Ҳ�ܹ���һ����
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

/* ���������е������� */
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
�ڵ�� base class
T_String,T_Integer �Ľڵ�ض���Ҷ�ӽڵ�
T_List,T_Dict �Ľڵ�ض�����Ҷ�ӽڵ�
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

/* T_String �� Node */
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
    bool collected; // �Ƿ��Ѿ��ռ�������
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

/* T_Integer �� Node */
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


// ǰ������
class DictNode;

/* T_List �� Node */
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

/* T_Dict �� Node */
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
