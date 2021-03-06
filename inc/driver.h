#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 1

#include <queue>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdarg.h>
#include <string>
#include <vector>
#define LEN 1024
#define PRTSPC 10
#define OPTSIZE 3
#define IDSIZE 4
using namespace std;

enum Node_Type { node_norm, node_value, node_id, node_opt, node_type, node_array };
enum Value_Type { type_int = 10, type_char, type_double, type_float, type_string, type_void, type_bool, type_array, type_pointer, type_default };

class TypeNode;
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
	Node(const char* name) : Node(name, Node_Type::node_norm) { }
	Node(char name, Node_Type type) {
		this->name = new char[2];
		this->name[0] = name; this->name[1] = '\0';
		this->type = Node_Type::node_norm;
	}
	Node(const char* name, Node_Type type) {
		this->name = new char[std::strlen(name) + 1];
		strcpy_s(this->name, std::strlen(name) + 1, name);
		this->type = type;
	}
	static Node* createNode(int num, Node* nodes[]); // 给出一个家族
	static Node* createNode(Node* root, Node* node); // 直接给出父子俩
	static Node* createNode(int num, ...);
	static void printNode(Node* node);
	static void printTree(Node *tree, int level);
	static void checkDefTree(Node* root, TypeNode* type);
	void addChildren(Node* child);
	void addBrother(Node *brother);
	char* getName() { return this->name; }
	Node* getBrother() { return this->brother; }
	Node* getChildren() { return this->children; }
	bool isLeaf() { return children == nullptr; }
	Node_Type getNodeType() {
		if (this != nullptr) { return this->type; }
		else {
			std::cout << std::endl << "你麻痹没有nodetype 你遍历到了空指针并尝试打印它的nodetype" << std::endl << std::endl;
			return Node_Type::node_id;
		}
	}
};

class ValueNode : public Node{ // 存储Value类型节点 
protected:
	char* value;
	Value_Type value_type;
	bool runtime = false; // 是否在运行时刻赋值 
public:
	ValueNode() :Node("Value", Node_Type::node_value){
		this->value_type = Value_Type::type_int;
	}
	// 连值都不传 怎么可能再传值类型
	ValueNode(const char* name) :Node(name, Node_Type::node_value){
		this->value = "";
		this->value_type = Value_Type::type_int;
	}
	ValueNode(const char* value, Value_Type value_type = Value_Type::type_int) :Node("Value", Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	ValueNode(const char* value, const char* name, Value_Type value_type = Value_Type::type_int) :Node(name, Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	ValueNode(const char* value, char name, Value_Type value_type = Value_Type::type_int) :Node(name, Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	bool isRuntime(){ return runtime; }
	void setRuntime(bool runtime){ this->runtime = runtime; }
	void setValueType(Value_Type type){ this->value_type = type; }
	void setValue(const char* val, Value_Type type);
	Value_Type getValueType(){ return this->value_type; }
	const char* getValue(){ return this->value; }
	static void printNode(Node* n);
	static bool checkValueType(ValueNode* n1, ValueNode* n2);
	static bool checkValueType(Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, ValueNode* t1, ValueNode* t2);
	static bool checkValid(ValueNode* node);
	static ValueNode* extractInterValue(ValueNode* n);
	static int isTrue(ValueNode* value);
	static ValueNode* ErrorNode(bool isRuntime = false, Value_Type type = Value_Type::type_int){
		const char* c = isRuntime ? "" : "Error Node";
		ValueNode* err = new ValueNode(c, "", type);
		err->setRuntime(isRuntime);
		return err;
	}
};


class IDNode : public ValueNode{ // ID的类型定义好后就不会再变,故可直接继承
	int linenum;
	ValueNode *tvalue = nullptr;
	
	//int autoFlag = 0; // 后缀++ 的标志位
	//void updateValue();
public:
	IDNode(const char* name, int linenum = 1, Value_Type type = Value_Type::type_default) : ValueNode("", name, type){
		this->linenum = linenum;
		this->type = Node_Type::node_id;
	}
	
	static IDNode* cp(IDNode* src);
	static void cp(IDNode* target, IDNode* src);
	static string genVarName();
	int getLineNum();
	void setLineNum(int no);
	//void setAutoFlag(bool needIncre);
	ValueNode* setValue(ValueNode* n);
	void mySetValue(ValueNode* n){ // 临时给数组加的
		//this->value_type = n->getValueType();
		this->tvalue = n;
	}
	//ValueNode* setValue(char* val, Value_Type type);
	ValueNode* getValue();
	static void printNode(Node* n);
	ValueNode*  setAuto(bool isIncre, bool prefix);
	void Increment(bool incre);
};


class TypeNode : public Node{
	Value_Type type_type;
public:
	TypeNode(Value_Type type = Value_Type::type_int) : Node("Type", Node_Type::node_type){
		this->type_type = type;
	}
	Value_Type getTypeType(){ return this->type_type; }
	static void printNode(Node* node);
	static string typeName(Value_Type type);
};

//void pushTAC(const char* opt, ValueNode* arg1, ValueNode* arg2, ValueNode* result);
class ExprNode : public ValueNode{
private:
	ValueNode *tvalue = nullptr; // 保存表达式的值(根据type强制转化成各种类型)
	//void calculate(ValueNode* n1, ValueNode* n2);
public:
	ExprNode(char* name, ValueNode* n1, ValueNode* n2 = nullptr) :ValueNode(){ // 表达式运算符 操作数1 操作数2（optional）
		this->name = name;
		this->type = Node_Type::node_opt;
		ValueNode* res = ExprNode::calculate(name, n1, n2);
		setValue(res);
	}
	ExprNode(ValueNode* n) :ValueNode(*n){
		setValue(n);
	}
	ValueNode* getValue(){ return this->tvalue; }
	ValueNode* setValue(ValueNode* n);
	void setNegative();  // 因为取负不需要创建新节点// 检测类型是否可以设为负
	void setNot();
	void autoIncre(int type = 0); // 0: ++a 1: --a 2: a++ 3: a--
	static void assign(IDNode* target, ValueNode* src);
	static Value_Type chooseValueType(ValueNode *n1, ValueNode* n2);
	static ValueNode* calculate(const char* name, ValueNode* n1, ValueNode* n2);
	static ValueNode* calcSimpleOpt(const char* name, ValueNode* n1, ValueNode* n2); // 计算非组合运算符
	static ValueNode* calcBoolOpt(const char* type, ValueNode* n1, ValueNode *n2);
	static IDNode* handleVarExpr(IDNode* node);
	static void printNode(Node* n);
};

class CharNode : public ValueNode{
	char tvalue;
public:
	char getValue(){ return this->tvalue; }
	CharNode(char val) : ValueNode(std::to_string(val).c_str(), Value_Type::type_char) {
		this->tvalue = val;
	}
	CharNode(CharNode &n) :ValueNode(std::to_string((char)n.getValue()).c_str(), Value_Type::type_char){
		this->tvalue = (char)n.getValue();
	}
};

class DoubleNode : public ValueNode {
public:
	double tvalue;
	DoubleNode(double value) :ValueNode(std::to_string(value).c_str(), Value_Type::type_double) { // 只有double类型需要再将字符串转为浮点数
		this->tvalue = value;
	}
	DoubleNode(DoubleNode& n) :ValueNode(std::to_string((double)n.tvalue).c_str(), Value_Type::type_double) { // 只有double类型需要再将字符串转为浮点数
		this->tvalue = (double)n.tvalue;
	}
	double getValue(){ return this->tvalue; }
};

class FloatNode :public ValueNode{
	float tvalue;
public:
	float getValue(){ return this->tvalue; }
	FloatNode(float val) :ValueNode(std::to_string(val).c_str(), Value_Type::type_float) { // 只有double类型需要再将字符串转为浮点数
		this->tvalue = val;
	}
	FloatNode(FloatNode& n) :ValueNode(std::to_string((float)n.tvalue).c_str(), Value_Type::type_float) { // 只有double类型需要再将字符串转为浮点数
		this->tvalue = (float)n.tvalue;
	}
};

class IntNode :public ValueNode{
	int tvalue;
public:
	int getValue(){ return this->tvalue; }
	IntNode(int val) :ValueNode(std::to_string(val).c_str(), Value_Type::type_int) {
		this->tvalue = val;
	}
	IntNode(IntNode &n) :ValueNode(std::to_string((int)n.tvalue).c_str(), Value_Type::type_int){
		this->tvalue = (int)n.tvalue;
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

class ArrayNode : public ValueNode {
private:
	int dimension; //用于记录有多少维度
	int count; //用于迭代记录当前维度有多大
	std::vector<int> size; //用于记录每一维有多大
public:
	int getDimension();
	void setDimension(int i);
	std::vector<int> getSize();
	void addSize(int tmp); //添加提升维度时把维度的大小添加进去
	void addCount(); //当前维度空间加一
	static void printNode(Node* n);
	Node* getChild(int i); //获取当前数组的第i个元素，从0开始计
	ArrayNode(int tmpCount = 1) : ValueNode("", Value_Type::type_array) { // value, Value_Type
		this->type = Node_Type::node_array;
		this->count = tmpCount;
		this->dimension = dimension;
	}
};



class SymbolMap {
public:
	IDNode* find(std::string name);
	bool hasDefined(std::string name);
	bool hasDefined(IDNode*);
	void insert(std::string name, IDNode* symbol);
	void updateSymbol(IDNode* src);
	std::map<std::string, IDNode> getMap(){ return Map; }
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

class TAC{
	string opt;
	/*ValueNode* arg1;
	ValueNode* arg2;
	ValueNode* result;*/
	string arg1;
	string arg2;
	string result;
public:
	TAC(const string o, string v1, string v2, string res){
		opt = o;
		arg1 = v1 ;
		arg2 = v2 ;
		result = res ;
	}
	TAC(const string o, string v1, string v2){// 有时候并不能马上知道结果
		opt = o;
		arg1 = v1;
		arg2 = v2;
		result = nullptr; // 这绝对是个废话..
	}
	void setResult(string res){ 
		result = res;
	}
	void printSelf();
	static void printTitle(); 
	static const char* genVarName();
};

class CodeGenerator{
	static string readLine(string line, char delim);
public:
	static void readFile(string read_from, string write_to);
	static void initVar(SymbolMap &map);
};

class myOptFault :public exception
{
public:
	myOptFault(char* msg = "defualt msg\n") :exception(msg){}
};
void ThrowERR(int index);
void ThrowERR(char* msg);
void checkNodeType(Node* n, Node_Type type);
bool isSimpleOpt(const char* name);
IDNode* handleVarExpr(IDNode* node);
//在lex中调用，不设置IDNode类型
void addID(string chrName, IDNode* sym, int index = 0);
//在yacc匹配types中调用，设置当前声明语句的变量类型
void setTypes(Value_Type tp);
//在声明语句中调用，设置当前ID的Value_Type
void setIDType(IDNode* node, Value_Type def_type);
void setIDType(IDNode* node);
// 更新符号表内符号的值
void updateIDInMap(ValueNode* src, int index = 0);
//传入结点获取到Node*
void getIDs(std::vector<char*> ids, Node* now);
//传入IDNode*检测是否重定义
bool isRedefined(IDNode* node);
//传入IDNode*检测是否未定义
bool isUndefined(IDNode* node);
//转换成指针类型
void convert2Pointer(Node* n);
//表明现在是否正在进行声明
void setStatus(bool status);
//获取现在的状态
bool isDefining();
//判断符号表内有没有存这个ID
bool hasID(std::string name, int index = 0);
const string insertHead(string head, string src);
//返回符号表里ID的IDNode
const string newLabel();
IDNode* getID(std::string name, int index = 0);
string newTempID();
string generateTAC(Node* root); // 递归下降遍历语法树,生成TAC队列
void pushTAC(string opt, string arg1, string arg2, string result);
void pushTAC(TAC* tac);
void printTAC();
#endif /* END __DRIVER_HPP__ */
