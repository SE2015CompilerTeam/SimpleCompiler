#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 1

#include <deque>
#include <map>
#include <string>
#include <string>
#include <stdarg.h>
#define LEN 1024
enum Node_Type { node_norm, node_value, node_id, node_opt, node_type };
enum Value_Type   { type_int, type_char, type_double, type_string, type_void };
class Node{ // 节点基类
protected:
	Node *children = nullptr; // 左孩子右兄弟
	Node *brother = nullptr;
	Node_Type type;
public:
	char* name;
	Node(){
		name = "";
		type = Node_Type::node_norm;
	}
	Node(char name) : Node(name, Node_Type::node_norm){ }
	Node(char* name) : Node(name, Node_Type::node_norm){ }
	Node(char name, Node_Type type){
		this->name = new char[2];
		this->name[0] = name; this->name[1] = '\0';
		this->type = Node_Type::node_norm;
	}
	Node(char* name, Node_Type type){ this->name = name; this->type = type; }
	static Node* createNode(int num, Node* nodes[]); // 给出一个家族
	static Node* createNode(Node* root, Node* node); // 直接给出父子俩
	static Node* createNode(int num, ...);
	static void printNode(Node* node);
	static void printTree(Node *tree, int level);
	void addChildren(Node* child);
	void addBrother(Node *brother);
	Node* getBrother(){ return this->brother; }
	Node* getChildren(){ return this->children; }
	bool isLeaf(){ return children == nullptr; }
	Node_Type getNodeType(){ return this->type; }
};
class ValueNode : public Node{ // 存储Value类型节点 TODO： 抽象基类
protected:
	std::string *value;
public:
	Value_Type value_type;
	//Value value;
	ValueNode(){
		Node("Value", Node_Type::node_value);
		//this->value = nullptr;
		this->value_type = Value_Type::type_int;
	}

	ValueNode(Value_Type value_type){
		Node("Value", Node_Type::node_value);
		//this->value = value;
		this->value_type = value_type;
	}
	void setValueType(Value_Type type){
		this->value_type = type;
	}
	Value_Type getValueType(){ return this->value_type; }
	static void printNode(Node* n);
};
class ExprNode : public Node{
public:
	ExprNode(char* name, Node* opt1, Node* opt2 = NULL){ // 表达式运算符 操作数1 操作数2（optional）
		this->name = name;
	}
	void setNegative(){ // 因为取负不需要创建新节点
		// 检测类型是否可以设为负
	}
	void setNot(){}
	void autoIncre(int type = 0){ // 0: ++a 1: --a 2: a++ 3: a--
	}
};

class DoubleNode : public ValueNode{
public:
	double value;
	DoubleNode(double value) :ValueNode(Value_Type::type_double) { // 只有double类型需要再将字符串转为浮点数
		this->value = value;
	}
	
};
class StringNode : public ValueNode{
public:
	std::string value;
	StringNode(char name) : ValueNode(Value_Type::type_string) {
		this->value = name;
	}
	StringNode(char* name) :ValueNode(Value_Type::type_string) {
		this->value = name;
	}
};
class IntNode :public ValueNode{
public:
	int value;
	IntNode(int value) :ValueNode(Value_Type::type_int) {
		this->value = value;
	}
};
class CharNode : public ValueNode{
public:
	char value;
	CharNode(char value) : ValueNode(Value_Type::type_char) {
		this->value = value;
	}
};
class TypeNode : public Node{
	Value_Type type_type;
public:
	TypeNode(Value_Type type = Value_Type::type_int): Node("Type", Node_Type::node_type){
		this->type_type = type;
	}
	static void printNode(Node* node);
};
class IDNode : public ValueNode{
	bool isFunc = false;
	ValueNode *value;
public:
	IDNode(char* name, Value_Type type = Value_Type::type_int): ValueNode(type){
		this->name = name;
		this->value = NULL;
	}
	IDNode(char  name, Value_Type type = Value_Type::type_int): ValueNode(type){
		char *c = new char[2];
		c[0] = name; c[1] = '\0';
		this->name = c;
		this->value = NULL;
	}
	void setValue(ValueNode* n){
		if (this->value != NULL)
			delete this->value;
		this->value = n;
	}
	static void printNode(Node* n);
};

/*class Symbol {
	int linenum;
	std::string name;
	Value_Type type;
	void* data;
};*/
class SymbolMap{
public:
	IDNode* find(std::string name);
	void insert(std::string name, IDNode* symbol);
private:
	std::map<std::string, IDNode> Map;
};
class SymbolTable{
public:
	IDNode* find(std::string name);
	void push(SymbolMap& map){
		MapStack.push_back(map);
	}
	void pop(){
		MapStack.pop_back();
	}
private:
	std::deque<SymbolMap> MapStack;
};
#endif /* END __DRIVER_HPP__ */
