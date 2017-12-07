#include "driver.h"
extern void yyerror(const char* msg);

SymbolMap idMap;
Value_Type type;
Value_Type VALUE_TYPE;
bool defining = false;

static const char* ErrorMSG[] = {
	"只能计算非组合运算符",
	"不支持的操作数类型",
	"表达式必须是可修改的左值",
	"表达式缺少左值",
	"使用了未初始化的或初始化为空的标识符",
	"重定义的标识符",
	"不支持的运算符",
	"未定义标识符"
};

bool isSimpleOpt(const char* name){
	if (name == "+="
		|| name == "-="
		|| name == "*="
		|| name == "/="
		|| name == "%="
		|| name == "|="
		|| name == "^="
		|| name == "&=")
		return false;
	return true;
}

void ThrowERR(int index){ // 错误消息索引
	//throw new exception(ErrorMSG[index]);
	//yyerror(ErrorMSG[index]);
	fprintf(stderr, ErrorMSG[index]);
	fprintf(stderr, "\n");
}
void ThrowERR(char* msg){ // 错误消息索引
	//yyerror(msg);
	fprintf(stderr, msg);
	fprintf(stderr, "\n");
}

void checkNodeType(Node* n, Node_Type type) {
	if (n == nullptr) {
		cout << endl << "这个结点是空的啊，那他妈就无所谓类型对不对了" << endl << endl;
	}
	if (n->getNodeType() != type)
		cout << endl << "类型不匹配啊这还玩毛啊" << endl << endl;
	//throw new exception("节点类型不匹配（Node => %d）", type);
}



Node* Node::createNode(int num, Node* nodes[]) {
	Node* root = NULL;
	if (num > 0 && nodes[0] != NULL) {
		root = nodes[0];
		for (int i = 1; i < num; i++) {
			if (nodes[i] == NULL) continue;
			root->addChildren(nodes[i]);
		}
	}
	return root;
}

Node* Node::createNode(int num, ...) {
	va_list argp;
	Node* node = NULL;
	Node* root = NULL;
	va_start(argp, num);
	for (int i = 0; i < num; i++) {
		node = va_arg(argp, Node*);
		if (node == NULL) continue;
		if (i == 0)
			root = node;
		else {
			root->addChildren(node);
		}
	}
	va_end(argp);
	return root;
}

Node* Node::createNode(Node* root, Node* node) {
	root->addChildren(node);
	return root;
}

void Node::addBrother(Node *bro) {
	Node *cur = this;
	while (cur->brother != NULL){
		if (cur == bro) // 不能添加自己为兄弟
			return;
		cur = cur->brother;
	}
	cur->brother = bro;
}

void Node::addChildren(Node *child) {
	if (this != child){
		if (this->children == NULL)
			this->children = child;
		else
			this->children->addBrother(child);
	}
}

void Node::checkDefTree(Node* root, TypeNode* def_type){
	Node* child = root->getChildren();// 第一个孩子
	while (child != nullptr)
	{
		if (child->getNodeType() == Node_Type::node_id){
			IDNode* id_child = (IDNode*)child;
			if (idMap.hasDefined(id_child)){
				fprintf(stderr, "重定义变量：'%s' at line: %d\n", id_child->getName(), (getID(id_child->getName()))->getLineNum());
			}
			else{
				// 为其赋类型
				setIDType(id_child, def_type->getTypeType());
				addID(child->getName(), id_child);
			}
		}
		child = child->getBrother();
	}
}

void Node::printNode(Node *n) {
	if (n != nullptr){
		switch (n->type)
		{
		case Node_Type::node_id:
			IDNode::printNode(n);
			break;
		case Node_Type::node_value:
			ValueNode::printNode(n);
			break;
		case Node_Type::node_type:
			TypeNode::printNode(n);
			break;
		case Node_Type::node_opt:
			ExprNode::printNode(n);
			break;
		case Node_Type::node_norm:
			cout << n->getName() << endl;
			break;
		default:
			break;
		}
	}
}
void Node::printTree(Node* node, int level){
	if (node == NULL)
		return;
	for (int i = 0; i < level; i++)
		cout << "    ";
	Node::printNode(node);
	Node *child = node->getChildren();
	while (child != NULL && child != child->getBrother())
	{
		Node::printTree(child, level + 1);
		child = child->getBrother();
	}
}


void ValueNode::printNode(Node* n){
	if (n != nullptr){
		ValueNode *node = extractInterValue((ValueNode*)n);
		cout.setf(ios::left);
		if (node->getValueType() == Value_Type::type_string){
			cout << setw(PRTSPC) << "Value" << node->getValue() << '(' << node->getName() << ')' << endl;
		}
		else
			cout << setw(PRTSPC) << "Value" << node->getValue() << endl;
	}
}

void ValueNode::setValue(const char* val, Value_Type type = Value_Type::type_int){
	this->value = new char[std::strlen(val) + 1];
	strcpy_s(this->value, std::strlen(val) + 1, val);
}

bool ValueNode::checkValid(ValueNode* node){ // 节点是否为空或标识符有没有被赋值
	if (node != nullptr)
		return extractInterValue(node) != nullptr;
	return false;
}

ValueNode* ValueNode::extractInterValue(ValueNode *n){
	ValueNode* res = n;
	if (n != nullptr){
		if (n->getNodeType() == Node_Type::node_id){
			if (n->getValueType() == Value_Type::type_default){
				fprintf(stderr, "未定义或未赋值的标识符 %s\n", n->getName());
			}
			res = ((IDNode*)n)->getValue();
		}
			
		else if (n->getNodeType() == Node_Type::node_opt)
			res = ((ExprNode*)n)->getValue();
	}
	return res;
}

IDNode* IDNode::cp(IDNode* src){
	IDNode* target = new IDNode(src->getName());
	cp(target, src);
	return target;
}
void IDNode::cp(IDNode* target, IDNode* src){ // 除了不赋值孩子和兄弟（否则我就用拷贝构造了..）
	if (src != nullptr){
		target->setLineNum(src->getLineNum());
		target->setValueType(src->getValueType());
		target->setValue(src->getValue());
	}
}

int IDNode::getLineNum(){
	return this->linenum;
}
void IDNode::setLineNum(int no){
	this->linenum = no;
}

void IDNode::Increment(bool incre){
	if (this->tvalue != nullptr){
		ValueNode* invisible_one = new IntNode(1); // 临时的 1
		ValueNode* first_res = ExprNode::calcSimpleOpt(incre ? "+" : "-", this->tvalue, invisible_one);
		ExprNode::calculate("=", this, first_res); 
		delete invisible_one;
		delete first_res;
	}
}

// 会改变id的值,使其自增(减)1
ValueNode* IDNode::setAuto(bool isIncre = true, bool prefix = true){
	ValueNode* res = nullptr;
	if (prefix){ // 自增后返回深拷贝对象
		Increment(isIncre);
		res = new ValueNode(this->tvalue->getValue(), this->value_type);
		return res;
	}
	else{ // 先返回深拷贝对象再自增
		res = new ValueNode(this->tvalue->getValue(), this->value_type);
		Increment(isIncre);
		return res;
	}
}

void IDNode::printNode(Node* n) {
	if (n != nullptr){
		try {
			if (n->getNodeType() == Node_Type::node_id){
				IDNode* node = (IDNode*)n;
				cout.setf(ios::left);
				cout << setw(PRTSPC) << "VAR" << n->getName() << endl;
				/*if (node->getValue()->getValueType() == Value_Type::type_array) {
					ValueNode* valNode = node->getValue();
					ArrayNode* arrNode = (ArrayNode*)valNode;
					vector<int> size = arrNode->getSize();
					cout << n->getName();
					for (int i = 0; i < size.size(); i++) {
						cout << "[" << size[i] << "]";
					}
					cout << endl;
				}
				else {
					cout << n->getName() << endl;
				}*/
			}
		}
		catch (exception e) {
			printf(e.what());
		}
	}

}


ValueNode* IDNode::getValue(){
	//updateValue(); // 不是在每次拿值得时候update ! 是在一条语句结束以后！
	return this->tvalue;
}

ValueNode* IDNode::setValue(ValueNode* n){ // 先不做类型检查(可以用右值检查左值？) 直接赋值 TODO
	if (n != nullptr){
		if (this->tvalue != nullptr)
			delete this->tvalue; // 先清除原来保留的Value
		ValueNode* v = ValueNode::extractInterValue(n);
		char *type; 
		switch (v->getValueType())
		{
		case Value_Type::type_char:		type = "char";		break;
		case Value_Type::type_int:		type = "int";		break;
		case Value_Type::type_float:	type = "float";		break;
		case Value_Type::type_double:	type = "double";	break;
		default:						type = "other";		break;
		}
		switch (this->value_type)
		{
		case Value_Type::type_char: 	
			switch (v->getValueType())
			{
				case Value_Type::type_char:		this->tvalue = new CharNode(((CharNode*)v)->getValue());	break;
				case Value_Type::type_int:		this->tvalue = new CharNode(((IntNode*)v)->getValue());		break;
				case Value_Type::type_float:	this->tvalue = new CharNode(((FloatNode*)v)->getValue());	break;
				case Value_Type::type_double:	this->tvalue = new CharNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
				default:
					fprintf(stderr, "不支持的类型转换,从 %s 到 char\n", type);
					break;
			}
			break;
		case Value_Type::type_int:		
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new IntNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new IntNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new IntNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new IntNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "不支持的类型转换,从 %s 到 double\n", type);
				break;
			}
			break;
		case Value_Type::type_float:	
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new FloatNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new FloatNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new FloatNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new FloatNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "不支持的类型转换,从 %s 到 float\n", type);
				break;
			}
			break;
		case Value_Type::type_double:	
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new DoubleNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new DoubleNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new DoubleNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new DoubleNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "不支持的类型转换,从 %s 到 double\n", type);
				break;
			}
			break;
		case Value_Type::type_string:	
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new StringNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new StringNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new StringNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new StringNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "不支持的类型转换,从 %s 到 string\n", type);
				break;
			}
			break;
		//case Value_Type::type_pointer:	this->tvalue = new StringNode(*(StringNode*)v);	break;
		//case Value_Type::type_array:	this->tvalue = n;								break;
		default:																		break;
		}
		return this;
	}
	else
		return nullptr;
}


void ExprNode::printNode(Node* n){
	if (n != nullptr){
		if (n->getNodeType() == Node_Type::node_opt){
			cout.setf(ios::left);
			ExprNode *en = (ExprNode*)n;
			cout << setw(PRTSPC) << "EXPR" << en->getName() << "    " << en->getValue()->getValue() << endl;
		}
	}
}

ValueNode* ExprNode::setValue(ValueNode* node){
	if (node != nullptr && this->tvalue != ValueNode::extractInterValue(node)){
		if (this->tvalue != nullptr)
			delete this->tvalue;//先清除原来的值
		this->tvalue = ValueNode::extractInterValue(node);
		this->value_type = node->getValueType();
		return this;
	}
	else{
		return nullptr;
	}
}


ValueNode* ExprNode::calcBoolOpt(const char* type, ValueNode* node1, ValueNode* node2){
	// >, <, ==, !=
	if (ValueNode::checkValid(node1) && ValueNode::checkValid(node2)){
		ValueNode *n1 = ValueNode::extractInterValue(node1), *n2 = ValueNode::extractInterValue(node2);
		double res1 = atof(n1->getValue()), res2 = atof(n2->getValue());
		if (!strcmp(type, ">"))			return new IntNode(res1 > res2);
		if (!strcmp(type, "<"))			return new IntNode(res1 < res2);
		if (!strcmp(type, "=="))		return new IntNode(res1 == res2);
		if (!strcmp(type, "!="))		return new IntNode(res1 != res2);
		if (!strcmp(type, ">="))		return new IntNode(res1 >= res2);
		if (!strcmp(type, "<="))		return new IntNode(res1 <= res2);
		return nullptr;
	}
	else{
		ThrowERR(3);
		return ValueNode::ErrorNode();
	}
}


// 只有 赋值运算 可以不检查值是否为空
ValueNode* ExprNode::calcSimpleOpt(const char* name, ValueNode* node1, ValueNode* node2){
	if (!isSimpleOpt(name)){
		return ValueNode::ErrorNode();
	}
	bool b = checkValid(node1) && checkValid(node2);
	if (checkValid(node1) && checkValid(node2)){
		ValueNode *n1 = ValueNode::extractInterValue(node1), *n2 = ValueNode::extractInterValue(node2);
		if (n1 == nullptr || n2 == nullptr){
			ThrowERR(4);
		}
		if (false){ // TODO: 先判断是否有指针 或 数组
		}
		else {
			ValueNode *res = calcBoolOpt(name, n1, n2);
			if (res != nullptr){
				return res;
			}
			if (n1->getValueType() == Value_Type::type_double
				|| n2->getValueType() == Value_Type::type_double) // 任何表达式 只要含double 那结果就是 double
			{
				DoubleNode* dn = nullptr;
				if (name == "*")  dn = new DoubleNode(atof(n1->getValue()) * atof(n2->getValue()));
				else if (name == "/")  dn = new DoubleNode(atof(n1->getValue()) / atof(n2->getValue()));
				else if (name == "+")  dn = new DoubleNode(atof(n1->getValue()) + atof(n2->getValue()));
				else if (name == "-")  dn = new DoubleNode(atof(n1->getValue()) - atof(n2->getValue()));
				else if (name == "&&") dn = new DoubleNode(atof(n1->getValue()) && atof(n2->getValue()));
				else if (name == "||") dn = new DoubleNode(atof(n1->getValue()) || atof(n2->getValue()));
				else{
					ThrowERR(1);
					ValueNode::ErrorNode();
				}
				return dn;
			}
			else if (n1->getValueType() == Value_Type::type_float
				|| n2->getValueType() == Value_Type::type_float)
			{
				FloatNode* dn = nullptr;
				if (name == "*")	  dn = new FloatNode(atof(n1->getValue()) * atof(n2->getValue()));
				else if (name == "/") dn = new FloatNode(atof(n1->getValue()) / atof(n2->getValue()));
				else if (name == "+") dn = new FloatNode(atof(n1->getValue()) + atof(n2->getValue()));
				else if (name == "-") dn = new FloatNode(atof(n1->getValue()) - atof(n2->getValue()));
				else if (name == "&&") dn = new FloatNode(atof(n1->getValue()) && atof(n2->getValue()));
				else if (name == "||") dn = new FloatNode(atof(n1->getValue()) || atof(n2->getValue()));
				else {
					ThrowERR(1);
					ValueNode::ErrorNode();
				}
				return dn;
			}
			else if (n1->getValueType() == Value_Type::type_int
				|| n2->getValueType() == Value_Type::type_int)
			{
				IntNode* dn = nullptr;
				if (name == "*")	   dn = new IntNode(atoi(n1->getValue()) * atoi(n2->getValue()));
				else if (name == "/")  dn = new IntNode(atoi(n1->getValue()) / atoi(n2->getValue()));
				else if (name == "+")  dn = new IntNode(atoi(n1->getValue()) + atoi(n2->getValue()));
				else if (name == "-")  dn = new IntNode(atoi(n1->getValue()) - atoi(n2->getValue()));
				else if (name == "%")  dn = new IntNode(atoi(n1->getValue()) % atoi(n2->getValue()));
				else if (name == "|")  dn = new IntNode(atoi(n1->getValue()) | atoi(n2->getValue()));
				else if (name == "&")  dn = new IntNode(atoi(n1->getValue()) & atoi(n2->getValue()));
				else if (name == "^")  dn = new IntNode(atoi(n1->getValue()) ^ atoi(n2->getValue()));
				else if (name == "<<") dn = new IntNode(atoi(n1->getValue()) >> atoi(n2->getValue()));
				else if (name == ">>") dn = new IntNode(atoi(n1->getValue()) >> atoi(n2->getValue()));
				else if (name == "&&") dn = new IntNode(atoi(n1->getValue()) && atoi(n2->getValue()));
				else if (name == "||") dn = new IntNode(atoi(n1->getValue()) || atoi(n2->getValue()));
				else {
					ThrowERR(1);
					ValueNode::ErrorNode();
				}
				return dn;
			}
			else if (n1->getValueType() == Value_Type::type_char
				|| n2->getValueType() == Value_Type::type_char)
			{
				CharNode* dn = nullptr;
				if (name == "*")	   dn = new CharNode(atoi(n1->getValue()) *  atoi(n2->getValue()));
				else if (name == "/")  dn = new CharNode(atoi(n1->getValue()) / atoi(n2->getValue()));
				else if (name == "+")  dn = new CharNode(atoi(n1->getValue()) + atoi(n2->getValue()));
				else if (name == "-")  dn = new CharNode(atoi(n1->getValue()) - atoi(n2->getValue()));
				else if (name == "%")  dn = new CharNode(atoi(n1->getValue()) % atoi(n2->getValue()));
				else if (name == "|")  dn = new CharNode(atoi(n1->getValue()) | atoi(n2->getValue()));
				else if (name == "&")  dn = new CharNode(atoi(n1->getValue()) &  atoi(n2->getValue()));
				else if (name == "^")  dn = new CharNode(atoi(n1->getValue()) ^ atoi(n2->getValue()));
				else if (name == "<<") dn = new CharNode(atoi(n1->getValue()) << atoi(n2->getValue()));
				else if (name == ">>") dn = new CharNode(atoi(n1->getValue()) >> atoi(n2->getValue()));
				else if (name == "&&") dn = new CharNode(atoi(n1->getValue()) && atoi(n2->getValue()));
				else if (name == "||") dn = new CharNode(atoi(n1->getValue()) || atoi(n2->getValue()));
				else {
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
				return dn;
			}
		}
	}
	else{
		ThrowERR(4);
		return ValueNode::ErrorNode();
	}

}

ValueNode* ExprNode::calculate(const char* opt, ValueNode* n1, ValueNode* n2 = nullptr){
	if (n1 == nullptr)
		return ValueNode::ErrorNode();
	ValueNode *v1 = ValueNode::extractInterValue(n1);
	ValueNode *v2 = ValueNode::extractInterValue(n2); // 用来提取ID Expr的值
	if (n2 == nullptr){
		// 单目运算符 负号  ! 取地址 前++ -- 后++ -- ~(取反)
		if (v1 == nullptr){
			// 标识符还未初始化
			ThrowERR(4);
			return ValueNode::ErrorNode();
		}
		else{
			if (opt == "-"){
				IntNode* zero = new IntNode(0); // 符号临时变为 0 - ID
				ValueNode* res = calcSimpleOpt("-", zero, v1);
				delete zero;
				return res;
			}
			else if (opt == "!"){
				switch (v1->getValueType())
				{
				case Value_Type::type_char: 	return  new CharNode(!atoi(v1->getValue()));		break;
				case Value_Type::type_int:		return  new IntNode(!atoi(v1->getValue()));		break;
				case Value_Type::type_float:	return  new FloatNode(!atof(v1->getValue()));	break;
				case Value_Type::type_double:	return  new DoubleNode(!atof(v1->getValue()));	break;
				case Value_Type::type_string:
				case Value_Type::type_pointer:
				default:
					ThrowERR(2);
					return ValueNode::ErrorNode();

				}
			}
			else if (opt == "~"){
				switch (v1->getValueType())
				{
				case Value_Type::type_char: 	return  new CharNode(~atoi(v1->getValue()));		break;
				case Value_Type::type_int:		return  new IntNode(~atoi(v1->getValue()));		break;
				case Value_Type::type_float:
				case Value_Type::type_double:
				case Value_Type::type_string:
				case Value_Type::type_pointer:
				default:
					ThrowERR(2);
					return ValueNode::ErrorNode();
				}
				// 自增运算返回的是ID本身(取地址值可证明..)
			}
			else if (opt == "+++"){ // 前缀 也同之前一样需要创建节点并返回, 只是创建并赋值后要更改自身(+1)
				//  ++a => a+=1 => a = a + 1
				if (n1->getNodeType() == Node_Type::node_id){ // 只有id才能自增 类似赋值符号
					return ((IDNode*)n1)->setAuto(true, true);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
			else if (opt == "---"){
				if (n1->getNodeType() == Node_Type::node_id){
					return ((IDNode*)n1)->setAuto(false, true);
					//return new IDNode(*(IDNode*)n1);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
			else if (opt == "++"){
				if (n1->getNodeType() == Node_Type::node_id){
					return ((IDNode*)n1)->setAuto(true, false);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
			else if (opt == "--"){
				if (n1->getNodeType() == Node_Type::node_id){
					return ((IDNode*)n1)->setAuto(true, false);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
		}
	}
	else{
		if (opt == "="){
			if (n1->getNodeType() != Node_Type::node_id){
				ThrowERR(2);
				return ValueNode::ErrorNode();
			}
			// 先检查 指针/数组 赋值关系
			assign((IDNode*)n1, n2);
			return (IDNode*)n1;
		}
		else{
			if (v1 == nullptr){
				ThrowERR(4);
				return  ValueNode::ErrorNode();
			}
			else{
				if (isSimpleOpt(opt)){
					return calcSimpleOpt(opt, v1, v2);
				}
				else{
					if (n1->getNodeType() != Node_Type::node_id) // TODO:数组也可以做左值
						ThrowERR(3);
					ValueNode* first_result = nullptr;
					if (opt == "+=")			first_result = calculate("+", n1, n2);
					else if (opt == "-=")		first_result = calculate("-", n1, n2);
					else if (opt == "*=")		first_result = calculate("*", n1, n2);
					else if (opt == "/=")		first_result = calculate("/", n1, n2);
					else if (opt == "%=")		first_result = calculate("%", n1, n2);
					else if (opt == "&=")		first_result = calculate("&", n1, n2);
					else if (opt == "|=")		first_result = calculate("|", n1, n2);
					else if (opt == "^=")		first_result = calculate("^", n1, n2);
					else {
						ThrowERR(6);
						return ValueNode::ErrorNode(); //throw new myOptFault("不支持的组合运算符");
					}
					return calculate("=", n1, first_result);
				}
			}
		}
	}
}

void ExprNode::assign(IDNode* target, ValueNode* src){ // 检查左值是否未定义
	IDNode* symbol = getID(target->getName());//根据要插入值的name 查找
	if (symbol != nullptr){
		target->setValue(src);  
	}
	else{
		fprintf(stderr, "未定义标识符: ", target->getName(),'\n');
		//ThrowERR(4); // 使用了未定义变量
	}
}

void TypeNode::printNode(Node *n){
	if (n != nullptr){
		cout.setf(ios::left);
		TypeNode* node = (TypeNode*)n;
		cout << setw(PRTSPC) << "TYPE";
		switch (node->type_type)
		{
		case Value_Type::type_char:		cout << "CHAR" << endl;		break;
		case Value_Type::type_int:		cout << "INT" << endl;		break;
		case Value_Type::type_double:	cout << "DOUBLE" << endl;		break;
		case Value_Type::type_float:	cout << "FLOAT" << endl;		break;
		case Value_Type::type_string:	cout << "STRING" << endl;		break;
		case Value_Type::type_pointer:	cout << "PINTER" << endl;		break;
		default:	cout << "Un F**king known.Fuck Me.." << endl;		break;
		}
	}
}

void ArrayNode::printNode(Node* n) {
	try {
		if (n == nullptr) { //如果当前是空结点就不打印
			return;
		}
		checkNodeType(n, Node_Type::node_array); //这个地方如果最后不是arraynode会误报不匹配
		ValueNode* nodeTmp = (ValueNode*)n;
		ArrayNode* node = (ArrayNode*)nodeTmp; //转换成ArrayNode才能获取维数和每维空间大小
		cout << "ARRAY    " << node->getDimension() << "  ";//打印维数
		vector<int> vector = node->getSize();
		for (int i = 0; vector.size(); i++) {//打印每维空间大小
			cout << vector[i] << "  ";
		}
		Node* child = node->getChildren(); //获取子节点
		while (child != nullptr) {
			Node::printNode(child);
			child = child->getBrother(); //获取兄弟结点
		}
	}
	catch (exception e) {
		Node::printNode(n);
	}
}


int ArrayNode::getDimension() {
	return this->dimension;
}

void ArrayNode::setDimension(int i) {
	this->dimension = i;
}

vector<int> ArrayNode::getSize() {
	return this->size;
}

void ArrayNode::addSize(int tmp) {
	this->size.push_back(tmp);
}

void ArrayNode::addCount() {
	this->count += 1;
}

Node* ArrayNode::getChild(int i) {
	Node* child = this->getChildren();
	if (child == nullptr) { //如果没有孩子结点
		cout << "数组越界" << endl;
		return nullptr;
	}
	else {
		if (i == 0) { //如果返回下标为0
			return child;
		}
		else {
			for (int k = 0; k < i; i++) {
				child = child->getBrother(); //获取兄弟结点
				if (child == nullptr) {
					cout << "数组越界" << endl;
					return nullptr;
					break;
				}
			}
			return child;
		}
	}
}

void SymbolMap::insert(string name, IDNode* sym) {
	if (this->find(name) != nullptr) {
		return;
	}
	this->Map.insert(pair<string, IDNode>(name, *sym));
}

IDNode* SymbolMap::find(string name) {
	map<string, IDNode>::iterator it = Map.find(name);
	if (it == Map.end())
		return nullptr;
	else
		return &(it->second);
}

void SymbolMap::updateSymbol(IDNode* src){
	IDNode* target = find(src->getName());
	target->setValue(ValueNode::extractInterValue(src));
}
bool SymbolMap::hasDefined(std::string name){
	IDNode* target = this->find(name);
	if (target != nullptr && target->getValueType() != Value_Type::type_default)// 找到了,如果已经有类型了
		return true;
	return false; // 未找到, 返回未定义
}
bool SymbolMap::hasDefined(IDNode* symbol){
	return hasDefined(symbol->getName());
}

IDNode* SymbolTable::find(string name) {
	IDNode* symbol;
	for (deque<SymbolMap>::iterator it = MapStack.begin(); it != MapStack.end(); it++) {
		symbol = it->find(name);
		if (symbol != nullptr)
			return symbol;
	}
	return nullptr;
}


//判断条件有问题，设置的默认是type_int，但是这显然不合理啊
bool isRedefined(IDNode* node) {
	return idMap.find(node->getName()) != nullptr;
}
//判断条件有问题，设置的默认是type_int，但是这显然不合理啊
bool isUndefined(IDNode* node) {
	return idMap.find(node->getName()) == nullptr;
}

//用index区分不同的作用域下的表,待扩展
void addID(char* chrName, IDNode* sym, int index) {
	string strNmae = chrName;
	if (idMap.find(strNmae) == nullptr && sym != nullptr) {
		idMap.insert(strNmae, sym);
	}
}

void setTypes(Value_Type tp) { // 全局的type
	VALUE_TYPE = tp;
}

void setIDType(IDNode* node, Value_Type type){
	node->setValueType(type);
}
void setIDType(IDNode* node){
	setIDType(node, VALUE_TYPE);
}

//enum Node_Type { node_norm, node_value, node_id, node_opt, node_type };
//这个地方还有一个问题，就是int a = b;的话会把b也获取到，我猜测左值全都是child结点，不可能是brother结点，如果是这样的话就可以只获取child
void getIDs(vector<Node*> ids, Node* now) {
	if (now == NULL)
		return;
	Node* child = now->getChildren();
	while (child != NULL) {
		if (child->getNodeType() == node_id) {
			ids.push_back(child);
		}
		getIDs(ids, child);
		child = child->getBrother();
	}
}

void updateIDInMap(ValueNode* src){
	if (src->getNodeType() == Node_Type::node_id)
		idMap.updateSymbol((IDNode*)src);
}

void convert2Pointer(Node* n) {
	ValueNode* tmp = (ValueNode*)n;
	IDNode* node = (IDNode*)tmp;
	node->setValueType(Value_Type::type_pointer);
}

void setStatus(bool status) {
	defining = status;
}

bool isDefining() {
	return defining;
}

bool hasID(string name) {
	return idMap.find(name) != nullptr;
}

IDNode* getID(string name) {
	return idMap.find(name);
}
IDNode* ExprNode::handleVarExpr(IDNode* node){
	IDNode* symbol = getID(node->getName());//根据要插入值的name 查找
	//IDNode* nn = idMap.find(node->getName());
	if (symbol != nullptr){ // 找到了,继承找到的值
		IDNode::cp(node, symbol);
	}
	Value_Type t = node->getValueType();
	return node;
	/*if (isDefining()){
		if (symbol != nullptr){
			// 正在定义的变量已经插入了符号表  =>  重定义
			cout << "重定义的标识符: " << symbol->getName() << " 已在 " << symbol->getLineNum() << " 行定义" << endl;
			return nullptr;
		}
		else{
			addID(node->getName(), node);
			symbol = getID(node->getName()); // 这不能直接用node赋值！！！ 必须从map取！！
			setIDType(symbol);
			return symbol;
		}
	}
	else{
		// 正在赋值
		if (symbol == nullptr){
			// 未声明的变量
			cout << "使用未声明的变量 " << node->getName() << endl;
			//ThrowERR(4);
			return nullptr;
		}
		return symbol;
	}*/
}