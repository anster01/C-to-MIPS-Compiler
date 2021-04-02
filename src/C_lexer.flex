%option noyywrap

%{


#include <cstdio>
#include <cstdlib>

#include <string>
#include <iostream>
#include <vector>

//Fixing bug from Flex, this is the fix used in the labs
extern "C" int fileno(FILE *stream);

#include "C_parser.tab.hpp"

/* End the embedded code section. */
%}
IDENTIFIER [A-Za-z_][A-Za-z0-9_]*

INTEGER [0-9]+

FLOAT [0-9]+([.][0-9]*)?

DOUBLE [0-9]+([.][0-9]*)?

KEY_WORDS (int|if|return|else|while|for|char|switch|case|enum|break|continue|sizeof|float|unsigned|signed|struct|double)

OPERATORS ([*]|[+]|["/"]|[\^]|[-]|[\=]|"&"|"|"|"<<"|">>"|"%"|"~"|"++"|"--"|"+="|"-="|"*="|"/="|"%="|"<<="|">>="|"&="|"^="|"|=")

COMPARATORS ("=="|"&&"|"||"|"<"|">"|"<="|">="|"!="|"!")

MISCELLANEOUS ([;]|[,]|[(]|[)]|["{"]|["}"]|[\.]|[\[]|[\]]|[:])

%%
{KEY_WORDS} {
    std::string lexedString(yytext);
    if(lexedString == "int") {
        return KEYWORD_INT;
    }
    if(lexedString == "if") {
        return KEYWORD_IF;
    }
    if(lexedString == "return") {
        return KEYWORD_RETURN;
    }
    if(lexedString == "else") {
        return KEYWORD_ELSE;
    }
    if(lexedString == "while") {
        return KEYWORD_WHILE;
    }
    if(lexedString == "for") {
        return KEYWORD_FOR;
    }
    if(lexedString == "char") {
        return KEYWORD_CHAR;
    }
    if(lexedString == "switch") {
        return KEYWORD_SWITCH;
    }
    if(lexedString == "case") {
        return KEYWORD_CASE;
    }
    if(lexedString == "enum") {
        return KEYWORD_ENUM;
    }
    if(lexedString == "break") {
        return KEYWORD_BREAK;
    }
    if(lexedString == "continue") {
        return KEYWORD_CONTINUE;
    }
    if(lexedString == "sizeof") {
        return KEYWORD_SIZEOF;
    }
    if(lexedString == "float") {
        return KEYWORD_FLOAT;
    }
    if(lexedString == "signed") {
        return KEYWORD_SIGNED;
    }
    if(lexedString == "unsigned") {
        return KEYWORD_UNSIGNED;
    }
    if(lexedString == "struct") {
        return KEYWORD_STRUCT;
    }
    if(lexedString == "double") {
        return KEYWORD_DOUBLE;
    }
}

{IDENTIFIER} {
    yylval.string = new std::string(yytext);
    return IDENTIFIER;
}

{INTEGER} {
    yylval.integer_constant = atoi(yytext);
    return INTEGER_CONSTANT;
}

{FLOAT} {
    yylval.float_constant = atof(yytext);
    return FLOAT_CONSTANT;
}

{DOUBLE} {
    yylval.double_constant = atof(yytext); 
    return DOUBLE_CONSTANT;
}

{COMPARATORS} {
    std::string LexedComparator(yytext);
    if(LexedComparator == "==") {
        return COMPARATOR_EQUALS;
    }
    if(LexedComparator == "&&") {
        return COMPARATOR_AND;
    }
    if(LexedComparator == "||") {
        return COMPARATOR_OR;
    }
    if(LexedComparator == "<") {
        return COMPARATOR_LT;
    }
    if(LexedComparator == ">") {
        return COMPARATOR_GT;
    }
    if(LexedComparator == ">=") {
        return COMPARATOR_GTEQ;
    }
    if(LexedComparator == "<=") {
        return COMPARATOR_LTEQ;
    }
    if(LexedComparator == "!=") {
        return COMPARATOR_NE;
    }
    if(LexedComparator == "!") {
        return COMPARATOR_NOT;
    }
}

{OPERATORS} {
    std::string LexedOperator(yytext);
    if(LexedOperator == "*") {
        return OPERATOR_MULTIPLY;
    }
    if(LexedOperator == "+") {
        return OPERATOR_ADDITION;
    }
    if(LexedOperator == "/") {
        return OPERATOR_DIVIDE;
    }
    if(LexedOperator == "^") {
        return OPERATOR_XOR;
    }
    if(LexedOperator == "-") {
        return OPERATOR_MINUS;
    }
    if(LexedOperator == "=") {
        return OPERATOR_ASSIGNMENT;
    }
    if(LexedOperator == "&") {
        return OPERATOR_BITWISEAND;
    }
    if(LexedOperator == "|") {
        return OPERATOR_BITWISEOR;
    }
    if(LexedOperator == "%") {
        return OPERATOR_MODULO;
    }
    if(LexedOperator == "<<") {
        return OPERATOR_LEFTSHIFT;
    }
    if(LexedOperator == ">>") {
        return OPERATOR_RIGHTSHIFT;
    }
    if(LexedOperator == "~") {
        return OPERATOR_ONECOMP;
    }
    if(LexedOperator == "++") {
        return OPERATOR_INCREMENT;
    }
    if(LexedOperator == "--") {
        return OPERATOR_DECREMENT;
    }
    if(LexedOperator == "+=") {
        return OPERATOR_ADD_ASSIGN;
    }
    if(LexedOperator == "-=") {
        return OPERATOR_SUB_ASSIGN;
    }
    if(LexedOperator == "*=") {
        return OPERATOR_MUL_ASSIGN;
    }
    if(LexedOperator == "/=") {
        return OPERATOR_DIV_ASSIGN;
    }
    if(LexedOperator == "%=") {
        return OPERATOR_MOD_ASSIGN;
    }
    if(LexedOperator == "<<=") {
        return OPERATOR_LEFTSHIFT_ASSIGN;
    }
    if(LexedOperator == ">>=") {
        return OPERATOR_RIGHTSHIFT_ASSIGN;
    }
    if(LexedOperator == "&=") {
        return OPERATOR_BWAND_ASSIGN;
    }
    if(LexedOperator == "^=") {
        return OPERATOR_BWXOR_ASSIGN;
    }
    if(LexedOperator == "|=") {
        return OPERATOR_BWOR_ASSIGN;
    }
}

{MISCELLANEOUS} {
    std::string LexedMisc(yytext);
    if (LexedMisc == ";") {
        return MISC_SEMI_COLON;
    }
    if (LexedMisc == ",") {
        return MISC_COMMA;
    }
    if (LexedMisc == "(") {
        return MISC_L_BRACKET;
    }
    if (LexedMisc == ")") {
        return MISC_R_BRACKET;
    }
    if (LexedMisc == "{") {
        return MISC_L_CBRACKET;
    }
    if (LexedMisc == "}") {
        return MISC_R_CBRACKET;
    }
    if (LexedMisc == ".") {
        return MISC_FULL_STOP;
    }
    if (LexedMisc == "[") {
        return MISC_L_SBRACKET;
    }
    if (LexedMisc == "]") {
        return MISC_R_SBRACKET;
    }
    if (LexedMisc == ":") {
        return MISC_COLON;
    }
}

[ \t\r\n]+ {}

. {}
%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}
