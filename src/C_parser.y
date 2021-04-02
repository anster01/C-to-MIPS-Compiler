%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Node *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  const Node *node;
  int integer_constant;
  float float_constant;
  double double_constant;
  std::string *string;

}

%token KEYWORD_RETURN KEYWORD_INT KEYWORD_IF KEYWORD_ELSE KEYWORD_WHILE KEYWORD_SWITCH KEYWORD_CASE KEYWORD_FOR KEYWORD_CHAR KEYWORD_BREAK KEYWORD_ENUM KEYWORD_CONTINUE
%token KEYWORD_SIZEOF KEYWORD_FLOAT KEYWORD_UNSIGNED KEYWORD_SIGNED KEYWORD_STRUCT KEYWORD_DOUBLE
%token IDENTIFIER INTEGER_CONSTANT FLOAT_CONSTANT DOUBLE_CONSTANT
%token COMPARATOR_GT COMPARATOR_LT COMPARATOR_OR COMPARATOR_AND COMPARATOR_GTEQ COMPARATOR_EQUALS COMPARATOR_LTEQ COMPARATOR_NE COMPARATOR_NOT
%token OPERATOR_MULTIPLY OPERATOR_ADDITION OPERATOR_DIVIDE OPERATOR_XOR OPERATOR_MINUS OPERATOR_ASSIGNMENT OPERATOR_ONECOMP OPERATOR_LEFTSHIFT OPERATOR_RIGHTSHIFT
%token OPERATOR_BITWISEAND OPERATOR_BITWISEOR OPERATOR_MODULO OPERATOR_INCREMENT OPERATOR_DECREMENT OPERATOR_ADD_ASSIGN OPERATOR_SUB_ASSIGN OPERATOR_MUL_ASSIGN OPERATOR_DIV_ASSIGN
%token OPERATOR_MOD_ASSIGN OPERATOR_LEFTSHIFT_ASSIGN OPERATOR_RIGHTSHIFT_ASSIGN OPERATOR_BWAND_ASSIGN OPERATOR_BWOR_ASSIGN OPERATOR_BWXOR_ASSIGN
%token MISC_SEMI_COLON MISC_COMMA MISC_L_BRACKET MISC_R_BRACKET MISC_L_CBRACKET MISC_R_CBRACKET MISC_FULL_STOP MISC_L_SBRACKET MISC_R_SBRACKET MISC_COLON

%type <node> ROOT FRAME FUNCTION WRAPPED_ARGUMENTS WRAPPED_LINES MULTIPLE_ARGUMENTS SINGLE_ARGUMENT MULTIPLE_LINES SINGLE_LINE RETURN_LINE SINGLE_CASE MULTIPLE_CASES
%type <node> VARIABLE_DECLARATION UNARY TERM VARIABLE_ASSIGNMENT FACTOR CONDITION IF_STATEMENT WHILE_LOOP EXPRESSION COMPOUND_CONDITION SWITCH_STATEMENT
%type <node> TYPE_SPECIFIER VARIABLE_INITIALISATION FOR_LOOP FUNCTION_CALL MULTIPLE_PARAMETERS WRAPPED_PARAMETERS FUNCTION_DECLARATION GLOBAL_ARRAY_DECLARATION
%type <node> ARRAY_DECLARATION ARRAY_ASSIGNMENT STRUCT_DEFINITION MULTIPLE_STRUCT_VARS STRUCT_VAR STRUCT_DECLARATION STRUCT_CALL STRUCT_ASSIGNMENT ENUM_DEFINITION
%type <node> ENUM_MULTIPLE_VARIABLES ENUM_SINGLE_VARIABLE POINTER_DECLARATION

%type <string> IDENTIFIER
%type <integer_constant> INTEGER_CONSTANT
%type <float_constant> FLOAT_CONSTANT
%type <float_constant> DOUBLE_CONSTANT

%start ROOT

%%

ROOT : FRAME { g_root = $1; }

FRAME : FUNCTION { $$ = $1; }
      | FUNCTION_DECLARATION { $$ = $1; }
      | GLOBAL_ARRAY_DECLARATION { $$ = $1; }
      | STRUCT_DEFINITION { $$ = $1; }
      | ENUM_DEFINITION { $$ = $1; }
      | FUNCTION FRAME { $$ = new Frame($1,$2); }
      | FUNCTION_DECLARATION FRAME { $$ = new Frame($1,$2); }
      | GLOBAL_ARRAY_DECLARATION FRAME { $$ = new Frame($1,$2); }
      | STRUCT_DEFINITION FRAME { $$ = new Frame($1,$2); }
      | ENUM_DEFINITION FRAME { $$ = new Frame($1,$2); }

/*enum X {...};*/
ENUM_DEFINITION : KEYWORD_ENUM IDENTIFIER MISC_L_CBRACKET ENUM_MULTIPLE_VARIABLES MISC_R_CBRACKET MISC_SEMI_COLON { $$ = new Enum_Definition(*$2,$4); }

/*vv=1,yy=2*/
ENUM_MULTIPLE_VARIABLES : ENUM_SINGLE_VARIABLE MISC_COMMA ENUM_MULTIPLE_VARIABLES { $$ = new Enum_Multiple_Variables($1,$3); }
                        | ENUM_SINGLE_VARIABLE { $$ = new Enum_Multiple_Variables($1,NULL); }

ENUM_SINGLE_VARIABLE : IDENTIFIER OPERATOR_ASSIGNMENT INTEGER_CONSTANT { $$ = new Enum_Variable(*$1,$3); }
                     | IDENTIFIER { $$ = new Enum_Variable(*$1,0); }

/*struct x {int a;int b;}; */
STRUCT_DEFINITION : KEYWORD_STRUCT IDENTIFIER MISC_L_CBRACKET MULTIPLE_STRUCT_VARS MISC_R_CBRACKET MISC_SEMI_COLON { $$ = new Struct_Definition(*$2,$4); }

/* int x; int y;*/
MULTIPLE_STRUCT_VARS : STRUCT_VAR MULTIPLE_STRUCT_VARS { $$ = new Multiple_Struct_Vars($1,$2); }
                     | STRUCT_VAR { $$ = new Multiple_Struct_Vars($1,NULL); }

/*int x;*/
STRUCT_VAR : TYPE_SPECIFIER IDENTIFIER MISC_SEMI_COLON { $$ = new Struct_Variable($1,*$2); }

/*int a[3];*/
GLOBAL_ARRAY_DECLARATION : TYPE_SPECIFIER IDENTIFIER MISC_L_SBRACKET INTEGER_CONSTANT MISC_R_SBRACKET MISC_SEMI_COLON { $$ = new Global_Array_Declaration($1,*$2,$4,0); }
                         | TYPE_SPECIFIER IDENTIFIER MISC_L_SBRACKET INTEGER_CONSTANT MISC_R_SBRACKET MISC_R_SBRACKET INTEGER_CONSTANT MISC_L_SBRACKET MISC_SEMI_COLON { $$ = new Global_Array_Declaration($1,*$2,$4,$7); }

/* int foo (int a, int b) {something;...} */
FUNCTION : TYPE_SPECIFIER IDENTIFIER WRAPPED_ARGUMENTS WRAPPED_LINES { $$ = new Function($1,*$2,$3,$4); }

TYPE_SPECIFIER : KEYWORD_INT { $$ = new Type_Specifier("int"); }
               | KEYWORD_CHAR { $$ = new Type_Specifier("char"); }
               | KEYWORD_UNSIGNED { $$ = new Type_Specifier("unsigned"); }
               | KEYWORD_SIGNED { $$ = new Type_Specifier("signed"); }
               | KEYWORD_FLOAT { $$ = new Type_Specifier("float"); }
               | KEYWORD_DOUBLE { $$ = new Type_Specifier("double"); }

/* (int a, int b)       () */
WRAPPED_ARGUMENTS : MISC_L_BRACKET MULTIPLE_ARGUMENTS MISC_R_BRACKET { $$ = new WrappedArguments($2); }
                  | MISC_L_BRACKET MISC_R_BRACKET { $$ = new WrappedArguments(NULL); }

/* int a, int b         int a */
MULTIPLE_ARGUMENTS : SINGLE_ARGUMENT MISC_COMMA MULTIPLE_ARGUMENTS { $$ = new Arguments($1,$3); }
                   | SINGLE_ARGUMENT { $$ = new Arguments($1,NULL); }

/* int a */
SINGLE_ARGUMENT : TYPE_SPECIFIER IDENTIFIER { $$ = new Arg($1,*$2);}
                | TYPE_SPECIFIER OPERATOR_MULTIPLY IDENTIFIER { $$ = new Pointer_Arg($1,*$3); }

/* {do something}       {} */
WRAPPED_LINES : MISC_L_CBRACKET MULTIPLE_LINES MISC_R_CBRACKET { $$ = new WrappedLines($2); }
              | MISC_L_CBRACKET MISC_R_BRACKET { $$ = new WrappedLines(NULL); }

/* line1;line2;line3;       line1; */
MULTIPLE_LINES : SINGLE_LINE MULTIPLE_LINES { $$ = new Lines($1,$2); }
               | SINGLE_LINE { $$ = new Lines($1,NULL); }

//MULTIPLE_CASES: SINGLE_CASE MULTIPLE_CASES { $$ = new Cases($1, $2); }
//              | SINGLE_LINE { $$ = new Cases($1, NULL);}

/* CASE(1) { something; break;} */
SINGLE_CASE: KEYWORD_CASE EXPRESSION MISC_COLON WRAPPED_LINES KEYWORD_BREAK {$$ = new Cases($2);}

/* return exp; int a; a=1;*/
SINGLE_LINE : RETURN_LINE MISC_SEMI_COLON { $$ = $1; }
            | VARIABLE_DECLARATION MISC_SEMI_COLON { $$ = $1; }
            | VARIABLE_ASSIGNMENT MISC_SEMI_COLON { $$ = $1; }
            | VARIABLE_INITIALISATION MISC_SEMI_COLON { $$ = $1; }
            | ARRAY_DECLARATION MISC_SEMI_COLON { $$ = $1; }
            | ARRAY_ASSIGNMENT MISC_SEMI_COLON { $$ = $1; }
            | STRUCT_DECLARATION MISC_SEMI_COLON { $$ = $1; }
            | STRUCT_ASSIGNMENT MISC_SEMI_COLON { $$ = $1; }
            | POINTER_DECLARATION MISC_SEMI_COLON { $$ = $1; }
            | IF_STATEMENT { $$ = $1; }
            /*| SWITCH_STATEMENT { $$ = $1; }*/
            | WHILE_LOOP { $$ = $1; }
            | FOR_LOOP { $$ = $1; }
            | KEYWORD_CONTINUE MISC_SEMI_COLON { $$ = new Continue_Statement(); }
            | KEYWORD_BREAK MISC_SEMI_COLON {$$ = new Break_Statement(); }
            | WRAPPED_LINES { $$ = $1; }

POINTER_DECLARATION : TYPE_SPECIFIER OPERATOR_MULTIPLY IDENTIFIER { $$ = new Pointer_Declaration($1,*$3); }

STRUCT_DECLARATION : KEYWORD_STRUCT IDENTIFIER IDENTIFIER { $$ = new Struct_Declaration(*$2,*$3); }

/*int a[3]*/
ARRAY_DECLARATION : TYPE_SPECIFIER IDENTIFIER MISC_L_SBRACKET INTEGER_CONSTANT MISC_R_SBRACKET { $$ = new Array_Declaration($1,*$2,$4,0); }
                  | TYPE_SPECIFIER IDENTIFIER MISC_L_SBRACKET INTEGER_CONSTANT MISC_R_SBRACKET MISC_L_SBRACKET INTEGER_CONSTANT MISC_R_SBRACKET { $$ = new Array_Declaration($1,*$2,$4,$7); }

/*a[2]=3*/
ARRAY_ASSIGNMENT : IDENTIFIER MISC_L_SBRACKET EXPRESSION MISC_R_SBRACKET OPERATOR_ASSIGNMENT COMPOUND_CONDITION { $$ = new Array_Assignment(*$1,$3,NULL,$6); }
                 | IDENTIFIER MISC_L_SBRACKET EXPRESSION MISC_R_SBRACKET MISC_L_SBRACKET EXPRESSION MISC_R_SBRACKET OPERATOR_ASSIGNMENT COMPOUND_CONDITION { $$ = new Array_Assignment(*$1,$3,$6,$9); }

/*a.x=2;*/
STRUCT_ASSIGNMENT : IDENTIFIER MISC_FULL_STOP IDENTIFIER OPERATOR_ASSIGNMENT COMPOUND_CONDITION { $$ = new Struct_Assignment(*$1,*$3,$5); }

/* if (condition) {...} else {...}      if (condition) {...}*/
IF_STATEMENT : KEYWORD_IF MISC_L_BRACKET COMPOUND_CONDITION MISC_R_BRACKET WRAPPED_LINES KEYWORD_ELSE WRAPPED_LINES { $$ = new If_Statement($3,$5,$7); }
             | KEYWORD_IF MISC_L_BRACKET COMPOUND_CONDITION MISC_R_BRACKET WRAPPED_LINES { $$ = new If_Statement($3,$5,NULL); }

/* while (condition) {...} */
WHILE_LOOP: KEYWORD_WHILE MISC_L_BRACKET COMPOUND_CONDITION MISC_R_BRACKET WRAPPED_LINES {$$ = new While_Loop($3, $5);}
          | KEYWORD_WHILE MISC_L_BRACKET COMPOUND_CONDITION MISC_R_BRACKET MISC_L_CBRACKET MISC_R_CBRACKET { $$ = new While_Loop($3,NULL); }

/* switch(something) {case(etc) { something... break;} ....} */
SWITCH_STATEMENT: KEYWORD_SWITCH MISC_L_BRACKET IDENTIFIER MISC_R_BRACKET SINGLE_CASE {$$ = new Switch_Statement($3,$5);}

/*for(x=0;x<10;x=x+1) {...} */
FOR_LOOP : KEYWORD_FOR MISC_L_BRACKET VARIABLE_ASSIGNMENT MISC_SEMI_COLON CONDITION MISC_SEMI_COLON VARIABLE_ASSIGNMENT MISC_R_BRACKET WRAPPED_LINES { $$ = new For_Loop($3,$5,$7,$9); }
         | KEYWORD_FOR MISC_L_BRACKET VARIABLE_ASSIGNMENT MISC_SEMI_COLON CONDITION MISC_SEMI_COLON VARIABLE_ASSIGNMENT MISC_R_BRACKET MISC_L_CBRACKET MISC_R_CBRACKET { $$ = new For_Loop($3,$5,$7,NULL); }

/*expr == expr    expr > expr     expr < expr     expr >= expr      expr <= expr    expr != expr*/
CONDITION : EXPRESSION COMPARATOR_EQUALS EXPRESSION { $$ = new Equals_Operator($1,$3); }
          | EXPRESSION COMPARATOR_GT EXPRESSION { $$ = new GreaterThan_Operator($1,$3); }
          | EXPRESSION COMPARATOR_LT EXPRESSION { $$ = new LessThan_Operator($1,$3); }
          | EXPRESSION COMPARATOR_GTEQ EXPRESSION { $$ = new GreaterThanEqualTo_Operator($1,$3); }
          | EXPRESSION COMPARATOR_LTEQ EXPRESSION { $$ = new LessThanEqualTo_Operator($1,$3); }
          | EXPRESSION COMPARATOR_NE EXPRESSION { $$ = new NotEquals_Operator($1,$3); }
          | EXPRESSION { $$ = $1; }

COMPOUND_CONDITION : CONDITION { $$ = $1; }
                   | COMPARATOR_NOT CONDITION { $$ = new Not_Operator($2); }
                   | CONDITION COMPARATOR_AND COMPOUND_CONDITION { $$ = new And_Operator($1,$3); }
                   | CONDITION COMPARATOR_OR COMPOUND_CONDITION { $$ = new Or_Operator($1,$3); }

/* a=1 a++ a--*/
VARIABLE_ASSIGNMENT : IDENTIFIER OPERATOR_ASSIGNMENT COMPOUND_CONDITION { $$ = new Variable_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_INCREMENT { $$ = new Increment_Operator(*$1); }
                    | IDENTIFIER OPERATOR_DECREMENT { $$ = new Decrement_Operator(*$1); }
                    | IDENTIFIER OPERATOR_ADD_ASSIGN COMPOUND_CONDITION { $$ = new Variable_Add_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_SUB_ASSIGN COMPOUND_CONDITION { $$ = new Variable_Sub_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_MUL_ASSIGN COMPOUND_CONDITION { $$ = new Variable_Mul_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_DIV_ASSIGN COMPOUND_CONDITION { $$ = new Variable_Div_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_MOD_ASSIGN COMPOUND_CONDITION { $$ = new Variable_Mod_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_LEFTSHIFT_ASSIGN COMPOUND_CONDITION { $$ = new Variable_LeftShift_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_RIGHTSHIFT_ASSIGN COMPOUND_CONDITION { $$ = new Variable_RightShift_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_BWAND_ASSIGN COMPOUND_CONDITION { $$ = new Variable_BitwiseAnd_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_BWOR_ASSIGN COMPOUND_CONDITION { $$ = new Variable_BitwiseOr_Assignment(*$1,$3); }
                    | IDENTIFIER OPERATOR_BWXOR_ASSIGN COMPOUND_CONDITION { $$ = new Variable_BitwiseXor_Assignment(*$1,$3); }
                    | OPERATOR_MULTIPLY IDENTIFIER OPERATOR_ASSIGNMENT COMPOUND_CONDITION { $$ = new Pointer_Assignment(*$2,$4); }

/* int a */
VARIABLE_DECLARATION : TYPE_SPECIFIER IDENTIFIER { $$ = new Variable($1,*$2); }

/* int a = 1; */
VARIABLE_INITIALISATION : TYPE_SPECIFIER IDENTIFIER OPERATOR_ASSIGNMENT EXPRESSION { $$ = new Variable_Initialisation($1,*$2,$4); }
                        | TYPE_SPECIFIER OPERATOR_MULTIPLY IDENTIFIER OPERATOR_ASSIGNMENT OPERATOR_BITWISEAND IDENTIFIER { $$ = new Pointer_Initialisation($1,*$3,*$6); }

/* return 123; */
RETURN_LINE : KEYWORD_RETURN COMPOUND_CONDITION { $$ = new Return_Statement($2); }

/*f(int a, int b);*/
FUNCTION_DECLARATION : TYPE_SPECIFIER IDENTIFIER WRAPPED_ARGUMENTS MISC_SEMI_COLON { $$ = new Function_Declaration($1,*$2,$3); }

/*f(1,a,3,4)*/
FUNCTION_CALL : IDENTIFIER WRAPPED_PARAMETERS { $$ = new Function_Call(*$1,$2); }

/*x.y*/
STRUCT_CALL : IDENTIFIER MISC_FULL_STOP IDENTIFIER { $$ = new Struct_Call(*$1,*$3); }

/*(1,a,3,b-1)*/
WRAPPED_PARAMETERS : MISC_L_BRACKET MULTIPLE_PARAMETERS MISC_R_BRACKET { $$ = new Wrapped_Parameters($2); }
                   | MISC_L_BRACKET MISC_R_BRACKET { $$ = new Wrapped_Parameters(NULL); }

/*1,a,3,b-1*/
MULTIPLE_PARAMETERS : EXPRESSION  MISC_COMMA MULTIPLE_PARAMETERS { $$ = new Parameters($1,$3); }
                    | EXPRESSION { $$ = new Parameters($1,NULL); }

/* 123 123+a */
EXPRESSION : EXPRESSION OPERATOR_ADDITION TERM { $$ = new Add_Operator($1,$3); }
           | EXPRESSION OPERATOR_MINUS TERM { $$ = new Sub_Operator($1,$3); }
           | EXPRESSION OPERATOR_XOR TERM {$$ = new BitwiseXOR_Operator($1,$3);}
           | EXPRESSION OPERATOR_BITWISEAND TERM {$$ = new BitwiseAND_Operator($1,$3);}
           | EXPRESSION OPERATOR_BITWISEOR TERM {$$ = new BitwiseOR_Operator($1,$3);}
           | EXPRESSION OPERATOR_LEFTSHIFT TERM {$$ = new LEFTSHIFT_Operator($1,$3);}
           | EXPRESSION OPERATOR_RIGHTSHIFT TERM {$$ = new RIGHTSHIFT_Operator($1,$3);}
           | TERM { $$ = $1; }

TERM : TERM OPERATOR_MULTIPLY UNARY { $$ = new Mul_Operator($1,$3); }
     | TERM OPERATOR_DIVIDE UNARY { $$ = new Div_Operator($1,$3); }
     | TERM OPERATOR_MODULO UNARY { $$ = new Modulo_Operator($1,$3); }
     | UNARY { $$ = $1; }

UNARY : OPERATOR_MINUS FACTOR { $$ = new Neg_Operator($2); }
      | FACTOR { $$ = $1; }
      | OPERATOR_ONECOMP FACTOR {$$ = new BitwiseONECOMP_Operator($2); }

FACTOR : INTEGER_CONSTANT { $$ = new Integer_Constant($1); }
       | FLOAT_CONSTANT {$$ = new Float_Constant($1);}
       | DOUBLE_CONSTANT {$$ = new Double_Constant($1);}
       | FUNCTION_CALL { $$ = $1; }
       | IDENTIFIER { $$ = new Variable_Call(*$1); }
       | IDENTIFIER MISC_L_SBRACKET EXPRESSION MISC_R_SBRACKET { $$ = new Array_Call(*$1,$3,NULL); }
       | IDENTIFIER MISC_L_SBRACKET EXPRESSION MISC_R_SBRACKET MISC_L_SBRACKET EXPRESSION MISC_R_SBRACKET { $$ = new Array_Call(*$1,$3,$6); }
       | OPERATOR_MULTIPLY IDENTIFIER { $$ = new Pointer_Call(*$2); }
       | STRUCT_CALL { $$ = $1; }
       | KEYWORD_SIZEOF MISC_L_BRACKET IDENTIFIER MISC_R_BRACKET { $$ = new Sizeof_Statement(*$3); }
       | KEYWORD_SIZEOF MISC_L_BRACKET KEYWORD_INT MISC_R_BRACKET { $$ = new Sizeof_Statement("int"); }
       | KEYWORD_SIZEOF MISC_L_BRACKET KEYWORD_CHAR MISC_R_BRACKET { $$ = new Sizeof_Statement("char"); }
       | KEYWORD_SIZEOF MISC_L_BRACKET KEYWORD_FLOAT MISC_R_BRACKET { $$ = new Sizeof_Statement("float"); }
       | KEYWORD_SIZEOF MISC_L_BRACKET KEYWORD_UNSIGNED MISC_R_BRACKET { $$ = new Sizeof_Statement("unsigned"); }
       | KEYWORD_SIZEOF MISC_L_BRACKET KEYWORD_SIGNED MISC_R_BRACKET { $$ = new Sizeof_Statement("signed"); }
       | KEYWORD_SIZEOF MISC_L_BRACKET KEYWORD_DOUBLE MISC_R_BRACKET { $$ = new Sizeof_Statement("double"); }
       | OPERATOR_MINUS UNARY { $$ = new Neg_Operator($2); }
       | MISC_L_BRACKET EXPRESSION MISC_R_BRACKET { $$ = $2; }

%%

const Node *g_root;
const Node *parseAST() {
    g_root=0;
    yyparse();
    return g_root;
}
