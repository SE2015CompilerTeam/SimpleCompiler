#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 1

#include <deque>
#include <map>
#include <string>
#include <stdarg.h>
#include <vector>

#define LEN 1024

enum Node_Type { node_norm, node_value, node_id, node_opt, node_type };
enum Value_Type { type_int = 10, type_char, type_double, type_string/* char* */, type_void, type_bool };
//vector<

//map<Value_Type, ValueVec*> TypeMap;
class Node { // 节点基类
protected:
	Node *children = nullptr; // 左孩子右兄弟
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
	static Node* createNode(int num, Node* nodes[]); // 给出一个家族
	static Node* createNode(Node* root, Node* node); // 直接给出父子俩
	static Node* createNode(int num, ...);
	static void printNode(Node* node);
	static void printTree(Node *tree, int level);
	void addChildren(Node* child);
	void addBrother(Node *brother);
	char* getName() { return this->name; }
	Node* getBrother() { return this->brother; }
	Node* getChildren() { return this->children; }
	bool isLeaf() { return children == nullptr; }
	Node_Type getNodeType() { return this->type; }
};

class ValueNode : public Node { // 存储Value类型节点 TODO： 抽象基类
protected:
	std::string value;
	Value_Type value_type;
public:
	ValueNode() :Node("Value", Node_Type::node_value) {
		//this->value = nullptr;
		this->value_type = Value_Type::type_int;
		this->value = "";
	}

	ValueNode(Value_Type value_type) :Node("Value", Node_Type::node_value) {
		this->value_type = value_type;
		this->value = "";
	}
	ValueNode(char* name, Value_Type value_type) :Node(name, Node_Type::node_value) {
		this->value_type = value_type;
		this->value = "";
	}
	ValueNode(char name, Value_Type value_type) :Node(name, Node_Type::node_value) {
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
	DoubleNode(double value) :ValueNode(Value_Type::type_double) { // 只有double类型需要再将字符串转为浮点数
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
	//ValueNode *value;
public:
	IDNode(char* name, Value_Type type = Value_Type::type_int, int linenum = 1) : ValueNode(name, type) {
		this->linenum = linenum;
		this->type = Node_Type::node_id;
	}
	void setValue(ValueNode* n) { // 先不做类型检查(可以用右值检查左值？) 直接赋值
		this->value_type = n->getValueType();
		this->value = n->getValue();
	}
	static void printNode(Node* n);
};

class ExprNode : public ValueNode {
private:
	void calculate(ValueNode* n1, ValueNode* n2);
public:
	ExprNode(char* name, ValueNode* n1, ValueNode* n2 = nullptr) { // 表达式运算符 操作数1 操作数2（optional）
		this->name = name;
		calculate(n1, n2);
	}
	void setNegative();  // 因为取负不需要创建新节点// 检测类型是否可以设为负
	void setNot();
	void autoIncre(int type = 0); // 0: ++a 1: --a 2: a++ 3: a--
	static char* calculate(const char* name, ValueNode* n1, ValueNode* n2);
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


//在lex中调用，不设置IDNode类型
void addID(char* chrName, IDNode* sym, int index = 0);
//在yacc匹配types中调用，设置当前声明语句的变量类型
void setTypes(Value_Type tp);
//在def_stmt中调用，用于设置变量类型
void setIDType(IDNode* node);
//传入结点获取到IDNode*
void getIDs(std::vector<char*> ids, Node* now);
//传入IDNode*检测是否重定义
bool isRedefined(IDNode* node);

#endif /* END __DRIVER_HPP__ */
