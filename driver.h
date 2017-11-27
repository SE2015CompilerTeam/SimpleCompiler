#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 1

#include <deque>
#include <map>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <vector>

#define LEN 1024

enum Node_Type { node_norm, node_value, node_id, node_opt, node_type, node_array };
enum Value_Type { type_int = 10, type_char, type_double, type_string/* char* */, type_void, type_bool , type_array, type_pointer};
//vector<

//map<Value_Type, ValueVec*> TypeMap;
class Node { // �ڵ����
protected:
	Node *children = nullptr; // �������ֵ�
	Node *brother = nullptr;
	Node_Type type;
public:
	char* name;
	Node() {
		name = "";
		type = Node_Type::node_norm;
	}
	Node(char name) : Node(name, Node_Type::node_norm) { }
	Node(char* name) : Node(name, Node_Type::node_norm) { }
	Node(char name, Node_Type type) {
		this->name = new char[2];
		this->name[0] = name; this->name[1] = '\0';
		this->type = Node_Type::node_norm;
	}
	Node(char* name, Node_Type type) { this->name = name; this->type = type; }
	static Node* createNode(int num, Node* nodes[]); // ����һ������
	static Node* createNode(Node* root, Node* node); // ֱ�Ӹ���������
	static Node* createNode(int num, ...);
	static void printNode(Node* node);
	static void printTree(Node *tree, int level);
	void addChildren(Node* child);
	void addBrother(Node *brother);
	char* getName() { return this->name; }
	Node* getBrother() { return this->brother; }
	Node* getChildren() { return this->children; }
	bool isLeaf() { return children == nullptr; }
	Node_Type getNodeType() {
		if (this != nullptr) { return this->type; }
		else {
			std::cout << std::endl << "�����û��nodetype ��������˿�ָ�벢���Դ�ӡ����nodetype" << std::endl << std::endl;
			return Node_Type::node_id;
		}
	}
};

class ValueNode : public Node { // �洢Value���ͽڵ� TODO�� �������
protected:
	std::string value;
	Value_Type value_type;
public:
	ValueNode() :Node("Value", Node_Type::node_value) {
		//this->value = nullptr;
		this->value_type = Value_Type::type_int;
		this->value = "";
	}

	ValueNode(Value_Type value_type, Node_Type node_type = Node_Type::node_value) :Node("Value", node_type) {
		this->value_type = value_type;
		this->value = "";
	}
	ValueNode(char* name, Value_Type value_type, Node_Type node_type = Node_Type::node_value) :Node(name, node_type) {
		this->value_type = value_type;
		this->value = "";
	}
	ValueNode(char name, Value_Type value_type, Node_Type node_type = Node_Type::node_value) :Node(name, node_type) {
		this->value_type = value_type;
		this->value = "";
	}
	void setValueType(Value_Type type) { this->value_type = type; }
	Value_Type getValueType() { return this->value_type; }
	std::string getValue() { return this->value; }
	static void printNode(Node* n);
	static bool checkValueType(ValueNode* n1, ValueNode* n2);
	static bool checkValueType(Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, ValueNode* t1, ValueNode* t2);
};

class DoubleNode : public ValueNode {
public:
	double value;
	DoubleNode(double value) :ValueNode(Value_Type::type_double) { // ֻ��double������Ҫ�ٽ��ַ���תΪ������
		this->value = value;
	}

};

class StringNode : public ValueNode {
public:
	std::string value;
	StringNode(char name) : ValueNode(Value_Type::type_string) {
		this->value = name;
	}
	StringNode(char* name) :ValueNode(Value_Type::type_string) {
		this->value = name;
	}
};

class IntNode :public ValueNode {
public:
	int value;
	IntNode(int value) :ValueNode(Value_Type::type_int) {
		this->value = value;
	}
	void test();
};

class CharNode : public ValueNode {
public:
	char value;
	CharNode(char value) : ValueNode(Value_Type::type_char) {
		this->value = value;
	}
};

class TypeNode : public Node {
	Value_Type type_type;
public:
	TypeNode(Value_Type type = Value_Type::type_int) : Node("Type", Node_Type::node_type) {
		this->type_type = type;
	}
	static void printNode(Node* node);
};

class IDNode : public ValueNode {
	bool isFunc = false;
	int linenum;
	ValueNode *value;
public:
	IDNode(char* name, Value_Type type = Value_Type::type_int, int linenum = 1) : ValueNode(name, type) {
		this->linenum = linenum;
		this->type = Node_Type::node_id;
	}
	void setValue(ValueNode* n) { // �Ȳ������ͼ��(��������ֵ�����ֵ��) ֱ�Ӹ�ֵ
		this->value_type = n->getValueType();
		this->value = n;
	}
	ValueNode* getValue() {
		return value;
	}
	static void printNode(Node* n);
};

class ExprNode : public ValueNode {
private:
	void calculate(ValueNode* n1, ValueNode* n2);
public:
	ExprNode(char* name, ValueNode* n1, ValueNode* n2 = nullptr) { // ���ʽ����� ������1 ������2��optional��
		this->name = name;
		calculate(n1, n2);
	}
	void setNegative();  // ��Ϊȡ������Ҫ�����½ڵ�// ��������Ƿ������Ϊ��
	void setNot();
	void autoIncre(int type = 0); // 0: ++a 1: --a 2: a++ 3: a--
	static char* calculate(const char* name, ValueNode* n1, ValueNode* n2);
};

class ArrayNode : public ValueNode {
private:
	int dimension; //���ڼ�¼�ж���ά��
	int count; //���ڵ�����¼��ǰά���ж��
	std::vector<int> size; //���ڼ�¼ÿһά�ж��
public:
	int getDimension();
	void setDimension(int i);
	std::vector<int> getSize(); 
	void addSize(int tmp); //�������ά��ʱ��ά�ȵĴ�С��ӽ�ȥ
	void addCount(); //��ǰά�ȿռ��һ
	static void printNode(Node* n);
	Node* getChild(int i); //��ȡ��ǰ����ĵ�i��Ԫ�أ���0��ʼ��
	ArrayNode(int tmpCount = 1) : ValueNode(Value_Type::type_array, Node_Type::node_array) {
		this->count = tmpCount;
		this->dimension = dimension;
	}
};

class SymbolMap {
public:
	IDNode* find(std::string name);
	void insert(std::string name, IDNode* symbol);
private:
	std::map<std::string, IDNode> Map;
};

class SymbolTable {
public:
	IDNode* find(std::string name);
	void push(SymbolMap& map) {
		MapStack.push_back(map);
	}
	void pop() {
		MapStack.pop_back();
	}
private:
	std::deque<SymbolMap> MapStack;
};


//��lex�е��ã�������IDNode����
void addID(char* chrName, IDNode* sym, int index = 0);
//��yaccƥ��types�е��ã����õ�ǰ�������ı�������
void setTypes(Value_Type tp);
//����������е��ã����õ�ǰID��Value_Type
void setIDType(IDNode* node);
//�������ȡ��Node*
void getIDs(std::vector<char*> ids, Node* now);
//����IDNode*����Ƿ��ض���
bool isRedefined(IDNode* node);
//����IDNode*����Ƿ�δ����
bool isUndefined(IDNode* node);
//ת����ָ������
void convert2Pointer(Node* n);
//���������Ƿ����ڽ�������
void setStatus(bool status);
//��ȡ���ڵ�״̬
bool isDefining();
//�жϷ��ű�����û�д����ID
bool hasID(std::string name);
//���ط��ű���ID��IDNode
IDNode* getID(std::string name);
#endif /* END __DRIVER_HPP__ */
