#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 1

#include <deque>
#include <map>
#include <string>
#include <stdarg.h>
#include <vector>
#include <stdio.h>

#define LEN 1024

enum Node_Type { node_norm, node_value, node_id, node_opt, node_type,/* node_expr exprNode可由type_opt替代 */ };
enum Value_Type   { type_int = 10, type_char, type_double,type_float, type_string/* char* */, type_void, type_pointer, type_array };
//vector<

//map<Value_Type, ValueVec*> TypeMap;
class Node{ // 节点基类
protected:
	Node *children = nullptr; // 左孩子右兄弟
	Node *brother = nullptr;
	//Node *lastChild = nullptr;
	//Node *lastBrother = nullptr;
	Node_Type type;
	char* name;
public:
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
	static void printTree(Node *tree, int level);
	void addChildren(Node* child);
	void addBrother(Node *brother);
	Node* getBrother(){ return this->brother; }
	Node* getChildren(){ return this->children; }
	const char* getName(){ return this->name; }
	bool isLeaf(){ return children == nullptr; }
	Node_Type getNodeType(){ return this->type; }
	static Node* createNode(int num, Node* nodes[]); // 给出一个家族
	static Node* createNode(Node* root, Node* node); // 直接给出父子俩
	static Node* createNode(int num, ...);
	static void printNode(Node* node);
	static bool checkNodeType(Node* n1, Node *n2);
	static bool checkNodeType(Node* n1, Node_Type type);
};
class ValueNode : public Node{ // 存储Value类型节点 
protected:
	char* value;
	Value_Type value_type;
public:
	ValueNode() :Node("Value", Node_Type::node_value){
		this->value_type = Value_Type::type_int;
	}

	ValueNode(const char* value, Value_Type value_type= Value_Type::type_int) :Node("Value", Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value)+1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	ValueNode(const char* value, char* name, Value_Type value_type = Value_Type::type_int) :Node(name, Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	ValueNode(const char* value, char name, Value_Type value_type = Value_Type::type_int) :Node(name, Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	void setValueType(Value_Type type){ this->value_type = type; }
	void setValue(const char* val, Value_Type type);
	Value_Type getValueType(){ return this->value_type; }
	const char* getValue(){ return this->value; }
	static void printNode(Node* n);
	static bool checkValueType(ValueNode* n1, ValueNode* n2);
	static bool checkValueType(Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, ValueNode* t1, ValueNode* t2);
	static ValueNode* extractInterValue(ValueNode* n);
};

class DoubleNode : public ValueNode{
	double tvalue;
public:
	double getValue(){ return this->tvalue; }
	DoubleNode(double val) :ValueNode(std::to_string(val).c_str() , Value_Type::type_double) { // 只有double类型需要再将字符串转为浮点数
		this->tvalue = val;
	}
};

class FloatNode :public ValueNode{
	float tvalue;
public:
	float getValue(){ return this->tvalue; }
	FloatNode(float val) :ValueNode(std::to_string(val).c_str(), Value_Type::type_float) { // 只有double类型需要再将字符串转为浮点数
		this->tvalue = val;
	}
};
class StringNode : public ValueNode{
	char* tvalue;
public:
	char* getValue(){ return tvalue; }
	StringNode(char name) : ValueNode(std::to_string(name).c_str(), Value_Type::type_string) {
		this->tvalue = new char[2];
		this->tvalue[0] = name; this->tvalue[1] = '\0';
	}
	StringNode(char* name) :ValueNode(name, Value_Type::type_string) {
		this->tvalue = new char[std::strlen(name) + 1];
		strcpy_s(this->tvalue, std::strlen(name) + 1, name);
	}
};
class IntNode :public ValueNode{
	int tvalue;
public:
	int getValue(){ return this->tvalue; }
	IntNode(int val) :ValueNode(std::to_string(val).c_str(), Value_Type::type_int) {
		this->tvalue = val;
	}
};
class CharNode : public ValueNode{
	char tvalue;
public:
	char getValue(){ return this->tvalue; }
	CharNode(char val) : ValueNode(std::to_string(val).c_str(), Value_Type::type_char) {
		this->tvalue = val;
	}
};

class TypeNode : public Node{
	Value_Type type_type;
public:
	TypeNode(Value_Type type = Value_Type::type_int) : Node("Type", Node_Type::node_type){
		this->type_type = type;
	}
	Value_Type getTypeType(){ return this->type_type; }
	static void printNode(Node* node);
};

class IDNode : public ValueNode{ // ID的类型定义好后就不会再变,故可直接继承
	int linenum;
	ValueNode *value = nullptr;
	int autoFlag = 0; // 后缀++ 的标志位
	void updateValue();
public:
	IDNode(char* name, Value_Type type = Value_Type::type_int, int linenum = 1) : ValueNode(name, type){
		this->linenum = linenum;
		this->type = Node_Type::node_id;
		autoFlag = 0;
		//临时加的 之后要检查符号表
		this->value = new IntNode(999);
	}
	void setAutoFlag(bool needIncre);
	ValueNode* setValue(ValueNode* n);
	//ValueNode* setValue(char* val, Value_Type type);
	ValueNode* getValue();
	static void printNode(Node* n);
	void setAuto(bool isIncre);
};

class ExprNode : public ValueNode{
private:
	ValueNode *tvalue = nullptr; // 保存表达式的值(根据type强制转化成各种类型)
	//void calculate(ValueNode* n1, ValueNode* n2);
public:
	ExprNode(char* name, ValueNode* n1, ValueNode* n2 = nullptr):ValueNode(){ // 表达式运算符 操作数1 操作数2（optional）
		this->name = name;
		this->type = Node_Type::node_opt;
		bool b = this->tvalue == nullptr;
		setValue(ExprNode::calculate(name, n1, n2));
		bool c = this->tvalue == nullptr;
	}
	ExprNode(ValueNode* n):ValueNode(*n){
		setValue(n);
	}
	ValueNode* getValue(){ return this->tvalue; }
	ValueNode* setValue(ValueNode* n);
	void setNegative();  // 因为取负不需要创建新节点// 检测类型是否可以设为负
	void setNot();
	void autoIncre(int type = 0); // 0: ++a 1: --a 2: a++ 3: a--
	static ValueNode* calculate(const char* name, ValueNode* n1, ValueNode* n2);
	static ValueNode* calcSimpleOpt(const char* name, ValueNode* n1, ValueNode* n2); // 计算非组合运算符
	static void printNode(Node* n);
};

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