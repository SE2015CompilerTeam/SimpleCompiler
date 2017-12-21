%{
#pragma warning( disable : 4996)
#include <fstream>
#include <stdio.h>
#include "mylexer.h"
#include "inc/driver.h"
#include "inc/codeGenerate.h"
#include "myparser.h"

extern int yylex();
//extern void yyerror(const char* msg);
extern int yylineno;
extern int lineno;
//extern char* yytext;
//extern int yyleng;
using namespace std;


%}
%union {
    class Node *nodes;
    class ValueNode *value_n;
    class ExprNode *expr_n;
    class TypeNode *type_n;
    class IntNode *int_n;
    class DoubleNode *double_n;
    class CharNode *char_n;
    class StringNode *str;
    class IDNode *id_n;
    char* name;
    int token;
}

%name myparser
/*
%include {
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
}
*/
%token<int_n>            INTEGER TRUE FALSE NUL
%token<double_n>         DBL
%token<char_n>           CHR
%token<str>              STR
%token<id_n>             ID
%token<token>            PROC_CONTROL  ACCESS_CONTROL ERR_CONTROL USING NAMESPACE DEFINE
%token<nodes>            READ WRITE 
%token<token>            CLASS RETURN NEW DEL THIS
%token<token>            ';'
%token<token>            '#'

%token<token>            '{' '}'
%left <token>           '(' ')'
%left<token>            ','
%right<token>           '=' AA MNA MA DA MOA ORA XORA PA
%right<token>           '?' ':'
%left<token>            OR
%left<token>            AND
%left<token>            '|'
%left<token>            '^'
%left<token>            '&'
%left<token>            EQ NEQ
%left<name>             CMP
%left<token>            '+' '-'
%left<token>            '*' '/' '%'
%left<token>            '!' PP MM '~' LL RR
%left<token>            RA NSP '[' ']'

%token<token>               FUNC_PRE
%token<type_n>            INT DOUBLE CHAR FLOAT BOOL VOID STRUCT MAIN
%token<nodes>            IF ELSE THEN WHILE DO FOR GOTO SWITCH CASE DEFAULT
//%type <value_n> vals
%type <nodes> stmts block
%type <nodes> stmt def_stmt if_stmt while_stmt do_while_stmt for_stmt return_stmt
%type <nodes> for_init_args for_action_args cond_args
%type <value_n> expr expritem
%type <nodes> types
%type <nodes> initlist // 这个该是啥type呢。。
%type <id_n>    var
%type <value_n> vals varexpr
%type <nodes> ids  exprlist

%start program
%%
program : /*block {Node::printTree($1, 0);}
        | program block {Node::printTree($2, 0);}
        ;*/
        | program INT MAIN '(' ')' block {
            freopen("e:\\out.txt", "w", stdout);
            Node::printTree($6, 0);
            generateTAC($6);
            freopen("e://TAC.txt", "w", stdout);
            printTAC();
            CodeGenerator::readFile("e://TAC.txt", "e://out_code.cpp");
         } // TODO: main args
        | program def_stmt ';'{
            freopen("e:\\out.txt", "w", stdout);
            Node::printTree($2, 0);
            generateTAC($2);
            freopen("e://TAC.txt", "w", stdout);
            printTAC();
            CodeGenerator::readFile("e://TAC.txt", "e://out_code.txt");
            }
        ;
types: INT  {$$ = $1;setTypes(Value_Type::type_int); setStatus(true);}
     | DOUBLE   {$$ = $1; setTypes(Value_Type::type_double); setStatus(true);}
     | FLOAT    {$$ = $1; setTypes(Value_Type::type_double); setStatus(true);}
     | CHAR {$$ = $1; setTypes(Value_Type::type_char); setStatus(true);}
     | VOID {$$ = $1; setTypes(Value_Type::type_void); setStatus(true);}
     | BOOL {$$ = $1; setTypes(Value_Type::type_bool); setStatus(true);}
     //| types '*' { }
     ;

block : '{' stmts '}' {$$ = Node::createNode(new Node("Block"), $2);}
      ;
stmts   : /* empty */ {$$ = new Node("NULL");}
        | stmt {$$ = new Node("Stmts"); $$->addChildren($1);}
        | stmts stmt {$$ = $1; $$->addChildren($2);}
        ;
stmt    : def_stmt ';'{$$ = $1; }
        | if_stmt {$$ = $1;}
        | while_stmt {$$ = $1;}
        | do_while_stmt {$$ = $1;}
        | for_stmt {$$ = $1;}
        | return_stmt ';'{$$ = $1; }
        | exprlist ';' {$$ = $1; }
        | block {$$ = $1;}
        | READ '(' var ')' ';' {    
                                    $$ = $1;
                                    $3->setRuntime(true);
                                    //$3->setValue(new IntNode(1)); //senseless
                                    updateIDInMap($3);
                                    $$->addChildren($3);   
                                }
        | WRITE '(' expr ')' ';'{ 
                                    $$ = $1;
                                    $$->addChildren($3);
                                }
        | ';' {$$ = new Node("Empty Stmt");  }
        ;
initlist: '{' exprlist '}' {    $$ = Node::createNode(new Node("Initlist"), $2);}
        | '{' exprlist ',' '}' {$$ = Node::createNode(new Node("Initlist"), $2);}
        ;
exprlist: exprlist ',' expritem {$$ = $1; $$->addChildren($3);}
        | expritem {    
                        $$ = new Node("Expr List");
                        $$->addChildren($1);
                        }
        ;
expritem: expr {$$ = $1; }
        | initlist { 
                    //$$ = $1; 
        }
        ;
vals:     INTEGER   {$$ = $1;}
        | DBL   {$$ = $1;}
        | CHR   {$$ = $1;}
        | STR   {$$ = $1;}
        | TRUE  {$$ = $1;}
        | FALSE {$$ = $1;}
        | NUL   {$$ = $1;}
        ;
/*func_def_stmt: types ID '(' def_stmt ')' block   {printf("func_def\n");}
        ;*/
expr    : '(' expr ')' {    $$ = $2; 
                            ValueNode* v = ValueNode::extractInterValue($$);
                            
                            }
        | expr PA  expr {
                            $$ = new ExprNode("+=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr MNA expr {   // 先计算expr值 再赋给左侧ID
                            $$ = new ExprNode("-=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           }
        | expr MA  expr {   
                            $$ = new ExprNode("*=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr DA  expr {   $$ = new ExprNode("/=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr MOA expr {   $$ = new ExprNode("%=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                          
                            }
        | expr ORA expr {   $$ = new ExprNode("|=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr XORA expr{   $$ = new ExprNode("^=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr AA  expr {   $$ = new ExprNode("&=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr '+' expr {  
                            $$ = new ExprNode("+", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            // 操作符、左右值、结果
                            }
                            
        | expr '-' expr {   $$ = new ExprNode("-", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           }
                            
        | expr '*' expr {   $$ = new ExprNode("*", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                             }
                             
        | expr '/' expr {
                            $$ = new ExprNode("/", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
                            
        | expr '%' expr {   $$ = new ExprNode("%", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr LL expr  {   $$ = new ExprNode("<<", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                         
                            }
        | expr RR expr  {   $$ = new ExprNode("<<", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr '|' expr {   
                            $$ = new ExprNode("|", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr '^' expr {   
                            $$ = new ExprNode("^", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr '&' expr {   $$ = new ExprNode("&", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr EQ expr  {   $$ = new ExprNode("==", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr NEQ expr {   $$ = new ExprNode("!=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr CMP expr {   
                            $$ = new ExprNode($2, $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
         }
        | expr OR expr  {   $$ = new ExprNode("||", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
        }
        | expr AND expr {   $$ = new ExprNode("&&", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
         }
        /*| PP expr  %prec RA{//$$ = $2;
                            
                            $$ = new ExprNode("+++", $2, nullptr);
                            $$->addChildren($2);
                        }
        | MM expr  %prec RA{
                            $$ = new ExprNode("---", $2, nullptr);
                            $$->addChildren($2);
                        }*/
        | '-' expr %prec '!' {
                            $$ = new ExprNode("-", $2, nullptr);
                            $$->addChildren($2);}
        | '+' expr %prec '!' {
                            $$ = new ExprNode("+", $2, nullptr);
                            $$->addChildren($2);}
        | expr PP{      
                        $$ = new ExprNode("++", $1, nullptr);
                        updateIDInMap($1);
                        $$->addChildren($1);
                       // printf("expr PP\n");
                    }
        | expr MM{      
                        $$ = new ExprNode("--", $1, nullptr);
                        updateIDInMap($1);
                        $$->addChildren($1);
                    }
        | '!' expr {    
                        $$ = new ExprNode("!", $2, nullptr);
                        $$->addChildren($2);
                        } // TODO: temp..
        | '~' expr {  
                        $$ = new ExprNode("~", $2, nullptr);
                        $$->addChildren($2);
        }
        | vals  {
                    $$ = $1;
        }
        | varexpr {
                    $$ = $1;
                    
                }
        //| MUL expr %prec UDEREF { }
        //| BITAND expr %prec UREF { }
        //| expr LBRACK expr RBRACK %prec SUB {}
        ;
ids     : varexpr {
                    $$ = Node::createNode(new Node("ID List"), $1);
                   
                    }
        | ids ',' varexpr {
                    $$ = $1; $$->addChildren($3);  
                   
                    }
        ;
varexpr : var { 
                    $$ = ExprNode::handleVarExpr((IDNode*)$1);// 找到了,赋:值、类型、行号否则不变 
               }
        | var '=' expritem {                  
                            // 检查重(未)定义, 返回找到的结果(未找到时插入符号表并返回传入参数)
                            if(isDefining()){
                                if($1->getValueType() == Value_Type::type_default){
                                    // 正在定义尚未定义的变量,没毛病
                                    setIDType($1); // 先设上全局的变量类型
                                    //$1->setValue($3);
                                    addID($1->getName(), $1);
                                    $$ = new ExprNode("=", $1, $3);
                                    updateIDInMap($1);
                                    $$->addChildren($1); $$->addChildren($3); 
                                }else{
                                    // $1已经含有变量类型了,证明先前已被定义过
                                    fprintf(stderr, "重定义变量：'%s' at line: %d\n", $1->getName(), (getID($1->getName()))->getLineNum());
                                 }
                            }else{
                                // 正在赋值,要检查左边是否被定义并赋值过
                                IDNode* target = ExprNode::handleVarExpr((IDNode*)$1);
                                if(target->getValueType() == Value_Type::type_default){
                                    fprintf(stderr, "未定义的标识符: %s\n",target->getName() );
                                   // yyerror(std::strcat("未定义的标识符 ",$1->getName()));
                                }
                                $$ = new ExprNode("=", target, $3);
                                updateIDInMap(target);
                                $$->addChildren(target); $$->addChildren($3); 
                            }
                        }
        ;
var     : ID {
                $$ = ExprNode::handleVarExpr((IDNode*)$1);
                //$$ = $1;
             }
        | var '[' INTEGER ']' {
                                if(isDefining()){//声明语句
                                    //把每一维空间大小压入进去
                                    IDNode* idNode = (IDNode*)$1;//先转换成IDNode*
                                    if(idNode->getValue()==nullptr){//如果没有初始化成员变量ValueNode就先初始化
                                        ArrayNode* arrNode = new ArrayNode();
                                        idNode->mySetValue(arrNode);
                                    }
                                    ValueNode* temp = idNode->getValue();
                                    ArrayNode* arrNodeInUsed = (ArrayNode*) temp;
                                    arrNodeInUsed->addSize($3->getValue());//压入当前维度的空间大小
                                    $$ = idNode;//给$$赋值
                                }
                                else {//赋值语句
                                    // $$ = $1->getChild($3->getValue());
                                }
                              }
        | var '[' ']' {}
        | '(' var ')' { $$ = $2; }
        | PP var  %prec RA{
                            ExprNode::handleVarExpr((IDNode*)$2); // 载入符号表内的值
                            ExprNode* temp = new ExprNode("+++", $2, nullptr);
                            temp->addChildren(IDNode::cp($2));
                            updateIDInMap($2); // 更新后重新插入
                            $$ = $2;
                            //temp->addChildren(IDNode::cp($2));// 没意义,只是为了让所有expr都有操作数做孩子
                            $$->addBrother(temp);
                        }
        | MM var  %prec RA{
                            ExprNode::handleVarExpr((IDNode*)$2);
                            ExprNode* temp = new ExprNode("---", $2, nullptr);
                            temp->addChildren(IDNode::cp($2));
                            $$ = $2;
                            updateIDInMap($$);
                           // temp->addChildren(IDNode::cp($2));
                            $$->addBrother(temp);
                        }
        ;
def_stmt: types ids  {  
                        setStatus(false);
                        Node::checkDefTree($2, (TypeNode*)$1);
                        $$ = Node::createNode(3, new Node("Def_Stmt"), $1, $2);
                     }
        ;
if_stmt : IF '(' cond_args ')' stmt {
                    Node *cond = Node::createNode(new Node("If Condition"), $3);
                    Node *body = Node::createNode(new Node("If Body"), $5);
                    $$ = Node::createNode(3, new Node("IF"), cond , body );
                    }
        | IF '(' cond_args ')' stmt ELSE stmt { 
                    Node *cond = Node::createNode(new Node("IF Condition"), $3);
                    Node *ifbody = Node::createNode(new Node("IF Body"), $5);
                    Node *elsebody = Node::createNode(new Node("ELSE"), $7);
                    $$ = Node::createNode(4, new Node("IF"), cond, ifbody, elsebody);
                 }
        //| expr '?' stmt ':' stmt {} put it in expr
        ;
while_stmt   : WHILE '(' cond_args ')' stmt  {
                    Node *cond = Node::createNode(new Node("While Condition"), $3);
                    Node *whilebody = Node::createNode(new Node("While body"), $5);
                    $$ = Node::createNode(3, new Node("WHILE"), cond, whilebody);
                }
        ;
do_while_stmt: DO stmt WHILE '(' cond_args ')'{
                    Node *body = Node::createNode(new Node("Do While Body"), $2);
                    Node *cond = Node::createNode(new Node("Do While Condition"), $5);
                    $$ = Node::createNode(3, new Node("DO WHILE"), body, cond);
                }
        ;
for_stmt: FOR '(' for_init_args ';' cond_args ';' for_action_args ')' stmt {
                    Node *init = Node::createNode(new Node("FOR args1"), $3);
                    Node *cond = Node::createNode(new Node("FOR args2"), $5);
                    Node *action = Node::createNode(new Node("FOR arg3"), $7);
                    Node *body = Node::createNode(new Node("FOR body"), $9);
                    $$ = Node::createNode(5, new Node("FOR"), init, cond, action, body);
                }
        ;
for_init_args: { $$ = new Node("NULL"); }
        | def_stmt {$$ = $1;}
        | ids   {$$ = $1;}
        ;
cond_args: expr         {$$ = $1;}
        | for_init_args {$$ = $1;} // TODO: def_stmt must have '='
        ;
for_action_args:    {$$ = new Node("NULL");}
        | expr      {$$ = $1;}
        ;
return_stmt  : RETURN expr {$$ = Node::createNode(new Node("RETURN"), $2);}
        ; 

%%

/////////////////////////////////////////////////////////////////////////////
// programs section

int main(void)
{
    int n = 1;
    mylexer lexer;
    myparser parser;
    //freopen("e:\\read.txt", "r", stdin);
    
    if (parser.yycreate(&lexer)) {
        if (lexer.yycreate(&parser)) {
            n = parser.yyparse();
        }
    }
    return n;
}

