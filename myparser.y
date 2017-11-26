%{
#pragma warning( disable : 4996)

#include <fstream>
#include <stdio.h>
#include "mylexer.h"
#include "driver.h"
#include "myparser.h"

extern int yylex();
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
%type <value_n> vals var varexpr
%type <nodes> ids  exprlist
//%type <nodes> func_def_stmt

%start program
%%
program : //types MAIN '(' ')' block {Node::printTree($5, 0);printf("main\n");} // TODO: main args
         block {Node::printTree($1, 0);}
         | program block {Node::printTree($2, 0);}
        ;
types: INT  {$$ = $1; printf("types INT\n"); }
     | DOUBLE   {$$ = $1;}
     | FLOAT    {$$ = $1;}
     | CHAR {$$ = $1;}
     | VOID {$$ = $1;}
     | BOOL {$$ = $1;}
     //| types '*' { }
     ;
block : '{' stmts '}' { 
                        $$ = Node::createNode(new Node("Block"), $2);
                        printf("block\n");
                        }
      ;
stmts   : /* empty */ {$$ = new Node("NULL");}
        | stmt {    
                    //$$ = new Node("STMT");
                    $$ = Node::createNode(new Node("STMT"), $1);
                    printf("stmt\n");
                }
        | stmts stmt {$$ = $1; $$->addChildren($2);}
        ;
stmt    : def_stmt ';'{$$ = $1;  printf("stmt def_stmt ;\n");}
        | if_stmt {$$ = $1; printf("stmt if_stmt\n");}
        | while_stmt {$$ = $1; printf("stmt while_stmt\n");}
        | do_while_stmt {$$ = $1;}
        | for_stmt {$$ = $1; printf("stmt for_stmt\n");}
        | return_stmt ';'{$$ = $1; printf("stmt return_stmt ;\n");}
        | exprlist ';' {$$ = $1; printf("stmt exprlist ;\n");  }
        | block {$$ = $1; printf("stmt block \n"); }
        | ';' { 
                $$ = new Node("Empty Stmt"); 
                printf("stmt just a ;\n"); 
                }
        ;
initlist: '{' exprlist '}' { 
                                //$$ = Node::createNode(new ValueNode("Initlist", Value_Type::type_array), $2); 
                                printf("initlist {list}\n"); 
                                }
        | '{' exprlist ',' '}' {
                               // $$ = Node::createNode(new ValueNode("Initlist", Value_Type::type_array), $2);
                                printf("initlist {list ,}\n");
                                }
        ;
exprlist: exprlist ',' expritem {$$ = $1; $$->addChildren($3);printf("exprlist , item\n"); }
        | expritem {    
                        $$ = new Node("Expr List");
                        $$->addChildren($1);
                        printf("exprlist item\n");
                        }
        ;
expritem: expr {$$ = $1; printf("expritem : expr\n");}
        | initlist { //$$ = $1; 
        }
        ;
vals:     INTEGER   {
                printf("integer\n");
                //$$ = $1; printf("vals INTEGER %d name %s \n", $1->getValue(),$1->getName());
            }
        | DBL   {$$ = $1; printf("vals DBL %f\n", $1->getValue());}
        | CHR   {$$ = $1;printf("vals CHR %c\n", $1->getValue());}
        | STR   {$$ = $1;}
        | TRUE  {$$ = $1;}
        | FALSE {$$ = $1;}
        | NUL   {$$ = $1;}
        ;
/*func_def_stmt: types ID '(' def_stmt ')' block   {printf("func_def\n");}
        ;*/
expr    : '(' expr ')' {    $$ = $2; 
                            ValueNode* v = ValueNode::extractInterValue($$);
                            //printf("(expr %s:%d %s)\n", $$->getName(),$$->getNodeType(), v->getValue());
                            }
        | expr PA  expr {
                            $$ = new ExprNode("+=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            printf("%s += %s\n", $1->getName(), $3->getName());
                            }
        | expr MNA expr {   // 先计算expr值 再赋给左侧ID
                            $$ = new ExprNode("-=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            printf("%s -= %s\n", $1->getName(), $3->getName());}
        | expr MA  expr {   
                            //$$ = Node::createNode(3,new Node("*=", Node_Type::node_opt), $1, $3);
                            }
        | expr DA  expr {   
                            //$$ = Node::createNode(3,new Node("/=", Node_Type::node_opt), $1, $3);
                            }
        | expr MOA expr {   //$$ = Node::createNode(3,new Node("%=", Node_Type::node_opt), $1, $3);
                            }
        | expr ORA expr {   //$$ = Node::createNode(3,new Node("|=", Node_Type::node_opt), $1, $3);
                            }
        | expr XORA expr{   //$$ = Node::createNode(3,new Node("^=", Node_Type::node_opt), $1, $3);
                            }
        | expr AA  expr {   //$$ = Node::createNode(3,new Node("&=", Node_Type::node_opt), $1, $3);
                            }
        | expr '+' expr {   
                            
                            /*$$ = new ValueNode("123");
                            //$$ = new ExprNode("+", $1, $3);
                            //$$ = new ValueNode("1", Value_Type::type_int);
                            $$->addChildren($1);  $$->addChildren($3);*/
                            printf("expr + expr\n");
                            }
                            
        | expr '-' expr {   $$ = new ExprNode("-", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            printf("%s - %s\n", $1->getName(), $3->getName());}
                            
        | expr '*' expr {   $$ = new ExprNode("*", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                             }
                             
        | expr '/' expr {
                            $$ = new ExprNode("/", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            printf("%s / %s\n", $1->getName(), $3->getName());
                            }
                            
        | expr '%' expr {   $$ = new ExprNode("%", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            printf("%s % %s\n", $1->getName(), $3->getName());
                            }
        | expr LL expr  {   //$$ = Node::createNode(3,new Node("<<", Node_Type::node_opt), $1, $3); 
                            }
        | expr RR expr  {   //$$ = Node::createNode(3,new Node(">>", Node_Type::node_opt), $1, $3); 
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
                            printf("Expr ==\n");
                            }
        | expr NEQ expr {   $$ = new ExprNode("!=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr CMP expr {   //$$ = new ExprNode($2, $1, $3);
                            //$$->addChildren($1);  $$->addChildren($3);
         }
        | expr OR expr  {//$$ = Node::createNode(3,new Node("||", Node_Type::node_opt), $1, $3); 
        }
        | expr AND expr {//$$ = Node::createNode(3,new Node("&&", Node_Type::node_opt), $1, $3);
         }
        /*| expr '=' expr {   $$ = new ExprNode("=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            printf("expr assign '=' : %s = %s\n", $1->getName(), $3->getName());}
        */
        | PP expr  %prec '!' {//$$ = $2;
                            $$ = new ExprNode("+++", $2, nullptr);
                            $$->addChildren($2);
                        }
        | MM expr  %prec '!' {
                            $$ = new ExprNode("---", $2, nullptr);
                            $$->addChildren($2);
                        }
        | '-' expr %prec '!' {
                            $$ = new ExprNode("-", $2, nullptr);
                            $$->addChildren($2);}
        | expr PP{      
                       // $$ = Node::createNode(new Node("++", Node_Type::node_opt), $1);
                       // printf("expr PP\n");
                             }
        | expr MM{//$$ = Node::createNode(new Node("--", Node_Type::node_opt), $1);
        }
        | '!' expr {    
                        //$$ = $2;
                        } // TODO: temp..
        | '~' expr {    //$$ = Node::createNode(new Node("~", Node_Type::node_opt), $2);
        }
        | vals  {
                    //$$ = $1; 
                    printf("vals\n");
                 //   $$ = new ValueNode("1234");
        }
        | var   {$$ = $1;printf("expr ID \n");}
        //| {}
        //| MUL expr %prec UDEREF { }
        //| BITAND expr %prec UREF { }
        //| expr LBRACK expr RBRACK %prec SUB {}
        ;
ids     : varexpr {
                    $$ = Node::createNode(new Node("ID List"), $1);
                    printf("ids varexpr\n");
                    }
        | ids ',' varexpr {
                    $$ = $1; $$->addChildren($3);  
                    printf("ids , varexpr\n");
                    }
        ;
varexpr : var { $$ = $1;printf("varexpr var\n");}
        | var '=' expr { 
                            printf("匹配到了赋值语句 %s = %s\n",$1->getName(), $3->getValue());
                            $$ = new ExprNode("=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            printf("varexpr: var = expr\n");
                        }
        | var '=' initlist {
                            // SetValue
                            /*ExprNode::calculate("=", $1, $3);
                            $$ = $1;
                            printf("varexpr: var = initlist\n");*/
                        }
        ;
var     : ID {  
                $$ = $1;
                printf("var ID %s\n", $1->getName());
                }
        //| '*' var %prec '!' {}
        | var '[' INTEGER ']' {  }
        | var '[' ']' {}
        | '(' var ')' { $$ = $2; }
        ;
def_stmt: types ids  { 
                        //Node *nodes[] = {new Node("Def_Stmt"), $1, $2};
                        //$$ = Node::createNode(new Node("Def_Stmt"), new Node("TEST"));
                        $$ = Node::createNode(3, new Node("Def_Stmt"), $1, $2);
                        printf("def_stmt types ids\n");
                     }
        //| def_stmt ',' def_stmt    {printf("def_stmt second\n");}
        ;
if_stmt      : IF '(' cond_args ')' stmt {
                    Node *cond = Node::createNode(new Node("If Condition"), $3);
                    Node *body = Node::createNode(new Node("If Body"), $5);
                    $$ = Node::createNode(3, new Node("IF"), cond , body );
                    }
        | IF '(' cond_args ')' stmt ELSE stmt { 
                    Node *cond = Node::createNode(new Node("IF Condition"), $3);
                    Node *ifbody = Node::createNode(new Node("IF Body"), $5);
                    Node *elsebody = Node::createNode(new Node("ELSE Body"), $7);
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
    if (parser.yycreate(&lexer)) {
        if (lexer.yycreate(&parser)) {
            n = parser.yyparse();
        }
    }
    return n;
}

