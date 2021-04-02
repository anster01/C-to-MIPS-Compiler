#ifndef ast_operators_hpp
#define ast_operators_hpp

#include <string>
#include <iostream>
#include <map>
#include <cmath>


class Add_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Add_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "ADD_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT" || left->getName() == "FLOAT_CONSTANT"){
                if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                    dst<<"\n\t\tADD [ "<<destReg<<" ";
                    left->print(dst,f_name, destReg,start_label,end_label);
                    dst<<" ";
                    right->print(dst,f_name, destReg,start_label,end_label);
                    dst<<" ]";
                } else if(right->getName() == "FLOAT_CONSTANT"){
                    std::string add_reg = makeName("add_reg");
                    dst<<"\n\t\tVARIABLE [ float "<<add_reg<<" ]";
                    right->print(dst,f_name, add_reg,start_label,end_label);
                    dst<<"\n\t\tADD [ "<<destReg<<" ";
                    left->print(dst,f_name, destReg,start_label,end_label);
                    dst<<" "<<add_reg;
                    dst<<" ]";  
                }
                else {
                    std::string add_reg = makeName("add_reg");
                    dst<<"\n\t\tVARIABLE [ int "<<add_reg<<" ]";
                    right->print(dst,f_name, add_reg,start_label,end_label);
                    dst<<"\n\t\tADD [ "<<destReg<<" ";
                    left->print(dst,f_name, destReg,start_label,end_label);
                    dst<<" "<<add_reg;
                    dst<<" ]";
                } 
                
            } else{
                if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT" || right->getName() == "FLOAT_CONSTANT"){
                    std::string add_reg = makeName("add_reg");
                    dst<<"\n\t\tVARIABLE [ int "<<add_reg<<" ]";
                    left->print(dst,f_name, add_reg,start_label,end_label);
                    dst<<"\n\t\tADD [ "<<destReg<<" "<<add_reg<<" ";
                    right->print(dst,f_name, destReg,start_label,end_label);
                    dst<<" ]";
                } 
                else if(left->getName() == "FLOAT_CONSTANT"){
                    std::string add_reg = makeName("add_reg");
                    dst<<"\n\t\tVARIABLE [ float "<<add_reg<<" ]";
                    right->print(dst,f_name, add_reg,start_label,end_label);
                    dst<<"\n\t\tADD [ "<<destReg<<" ";
                    left->print(dst,f_name, destReg,start_label,end_label);
                    dst<<" "<<add_reg;
                    dst<<" ]";  
                }
                else {
                    std::string add_reg = makeName("add_reg");
                    std::string add_reg2 = makeName("add_reg");
                    dst<<"\n\t\tVARIABLE [ int "<<add_reg<<" ]";
                    left->print(dst,f_name, add_reg,start_label,end_label);
                    dst<<"\n\t\tVARIABLE [ int "<<add_reg2<<" ]";
                    right->print(dst,f_name, add_reg2,start_label,end_label);
                    dst<<"\n\t\tADD [ "<<destReg<<" "<<add_reg<<" "<<add_reg2<<" ]";
                }
            }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg + RightArg;
        }
};

class Sub_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Sub_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "SUB_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"||left->getName() == "FLOAT_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tSUB [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
              else if(right->getName() == "FLOAT_CONSTANT") {
                  std::string sub_reg = makeName("sub_reg");
                  dst<<"\n\t\tVARIABLE [ float "<<sub_reg<<" ]";
                  right->print(dst,f_name, sub_reg,start_label,end_label);
                  dst<<"\n\t\tSUB [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<sub_reg;
                  dst<<" ]";            
              }
               else {
                  std::string sub_reg = makeName("sub_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<sub_reg<<" ]";
                  right->print(dst,f_name, sub_reg,start_label,end_label);
                  dst<<"\n\t\tSUB [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<sub_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"||right->getName() == "FLOAT_CONSTANT"){
                  std::string sub_reg = makeName("sub_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<sub_reg<<" ]";
                  left->print(dst,f_name, sub_reg,start_label,end_label);
                  dst<<"\n\t\tSUB [ "<<destReg<<" "<<sub_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
                else if(left->getName() == "FLOAT_CONSTANT"){
                    std::string add_reg = makeName("add_reg");
                    dst<<"\n\t\tVARIABLE [ float "<<add_reg<<" ]";
                    right->print(dst,f_name, add_reg,start_label,end_label);
                    dst<<"\n\t\tADD [ "<<destReg<<" ";
                    left->print(dst,f_name, destReg,start_label,end_label);
                    dst<<" "<<add_reg;
                    dst<<" ]";  
                }
              else {
                  std::string sub_reg = makeName("sub_reg");
                  std::string sub_reg2 = makeName("sub_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<sub_reg<<" ]";
                  left->print(dst,f_name, sub_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<sub_reg2<<" ]";
                  right->print(dst,f_name, sub_reg2,start_label,end_label);
                  dst<<"\n\t\tSUB [ "<<destReg<<" "<<sub_reg<<" "<<sub_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg - RightArg;
        }
};

class Mul_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Mul_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "MUL_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"||left->getName() == "FLOAT_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tMUL [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
            else if(right->getName() == "FLOAT_CONSTANT") {
                  std::string mul_reg = makeName("mul_reg");
                  dst<<"\n\t\tVARIABLE [ float "<<mul_reg<<" ]";
                  right->print(dst,f_name, mul_reg,start_label,end_label);
                  dst<<"\n\t\tMUL [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<mul_reg;
                  dst<<" ]";
            }
              else {
                  std::string mul_reg = makeName("mul_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<mul_reg<<" ]";
                  right->print(dst,f_name, mul_reg,start_label,end_label);
                  dst<<"\n\t\tMUL [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<mul_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"||right->getName()=="FLOAT_CONSTANT"){
                  std::string mul_reg = makeName("mul_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<mul_reg<<" ]";
                  left->print(dst,f_name, mul_reg,start_label,end_label);
                  dst<<"\n\t\tMUL [ "<<destReg<<" "<<mul_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
              else if(left->getName() == "FLOAT_CONSTANT") {
                  std::string mul_reg = makeName("mul_reg");
                  std::string mul_reg2 = makeName("mul_reg");
                  dst<<"\n\t\tVARIABLE [ float "<<mul_reg<<" ]";
                  left->print(dst,f_name, mul_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ float "<<mul_reg2<<" ]";
                  right->print(dst,f_name, mul_reg2,start_label,end_label);
                  dst<<"\n\t\tMUL [ "<<destReg<<" "<<mul_reg<<" "<<mul_reg2<<" ]";
              }
              else {
                  std::string mul_reg = makeName("mul_reg");
                  std::string mul_reg2 = makeName("mul_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<mul_reg<<" ]";
                  left->print(dst,f_name, mul_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<mul_reg2<<" ]";
                  right->print(dst,f_name, mul_reg2,start_label,end_label);
                  dst<<"\n\t\tMUL [ "<<destReg<<" "<<mul_reg<<" "<<mul_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg * RightArg;
        }
};

class Div_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Div_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "DIV_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"||left->getName() == "FLOAT_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tDIV [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
              else if(right->getName() == "FLOAT_CONSTANT") {
                  std::string div_reg = makeName("div_reg");
                  dst<<"\n\t\tVARIABLE [ float "<<div_reg<<" ]";
                  right->print(dst,f_name, div_reg,start_label,end_label);
                  dst<<"\n\t\tDIV [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<div_reg;
                  dst<<" ]";
              }
              else {
                  std::string div_reg = makeName("div_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<div_reg<<" ]";
                  right->print(dst,f_name, div_reg,start_label,end_label);
                  dst<<"\n\t\tDIV [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<div_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"|| right->getName() == "FLOAT_CONSTANT"){
                  std::string div_reg = makeName("div_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<div_reg<<" ]";
                  left->print(dst,f_name, div_reg,start_label,end_label);
                  dst<<"\n\t\tDIV [ "<<destReg<<" "<<div_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
              else if(left->getName() == "FLOAT_CONSTANT") {
                  std::string div_reg = makeName("div_reg");
                  std::string div_reg2 = makeName("div_reg");
                  dst<<"\n\t\tVARIABLE [ float "<<div_reg<<" ]";
                  left->print(dst,f_name, div_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ float "<<div_reg2<<" ]";
                  right->print(dst,f_name, div_reg2,start_label,end_label);
                  dst<<"\n\t\tDIV [ "<<destReg<<" "<<div_reg<<" "<<div_reg2<<" ]";  
              }
              else {
                  std::string div_reg = makeName("div_reg");
                  std::string div_reg2 = makeName("div_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<div_reg<<" ]";
                  left->print(dst,f_name, div_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<div_reg2<<" ]";
                  right->print(dst,f_name, div_reg2,start_label,end_label);
                  dst<<"\n\t\tDIV [ "<<destReg<<" "<<div_reg<<" "<<div_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg / RightArg;
        }
};

class Modulo_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Modulo_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "MODULO_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tMODULO [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string mod_reg = makeName("mod_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<mod_reg<<" ]";
                  right->print(dst,f_name, mod_reg,start_label,end_label);
                  dst<<"\n\t\tMODULO [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<mod_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string mod_reg = makeName("mod_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<mod_reg<<" ]";
                  left->print(dst,f_name, mod_reg,start_label,end_label);
                  dst<<"\n\t\tMODULO [ "<<destReg<<" "<<mod_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string mod_reg = makeName("mod_reg");
                  std::string mod_reg2 = makeName("mod_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<mod_reg<<" ]";
                  left->print(dst,f_name, mod_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<mod_reg2<<" ]";
                  right->print(dst,f_name, mod_reg2,start_label,end_label);
                  dst<<"\n\t\tMODULO [ "<<destReg<<" "<<mod_reg<<" "<<mod_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg / RightArg;
        }
};

class And_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        And_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "AND_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tAND [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string and_reg = makeName("and_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<and_reg<<" ]";
                  right->print(dst,f_name, and_reg,start_label,end_label);
                  dst<<"\n\t\tAND [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<and_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string and_reg = makeName("and_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<and_reg<<" ]";
                  left->print(dst,f_name, and_reg,start_label,end_label);
                  dst<<"\n\t\tAND [ "<<destReg<<" "<<and_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string and_reg = makeName("and_reg");
                  std::string and_reg2 = makeName("and_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<and_reg<<" ]";
                  left->print(dst,f_name, and_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<and_reg2<<" ]";
                  right->print(dst,f_name, and_reg2,start_label,end_label);
                  dst<<"\n\t\tAND [ "<<destReg<<" "<<and_reg<<" "<<and_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg && RightArg;
        }
};

class Or_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Or_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "OR_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tOR [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string or_reg = makeName("or_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<or_reg<<" ]";
                  right->print(dst,f_name, or_reg,start_label,end_label);
                  dst<<"\n\t\tOR [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<or_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string or_reg = makeName("or_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<or_reg<<" ]";
                  left->print(dst,f_name, or_reg,start_label,end_label);
                  dst<<"\n\t\tOR [ "<<destReg<<" "<<or_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string or_reg = makeName("or_reg");
                  std::string or_reg2 = makeName("or_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<or_reg<<" ]";
                  left->print(dst,f_name, or_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<or_reg2<<" ]";
                  right->print(dst,f_name, or_reg2,start_label,end_label);
                  dst<<"\n\t\tOR [ "<<destReg<<" "<<or_reg<<" "<<or_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg || RightArg;
        }
};

class Equals_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Equals_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "EQUALS_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"||left->getName() == "FLOAT_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tEQUAL_COMP [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
              else if(right->getName() == "FLOAT_CONSTANT") {
                  std::string eq_reg = makeName("eq_reg");
                  dst<<"\n\t\tVARIABLE [ float "<<eq_reg<<" ]";
                  right->print(dst,f_name, eq_reg,start_label,end_label);
                  dst<<"\n\t\tEQUAL_COMP [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<eq_reg;
                  dst<<" ]";
              }
              else {
                  std::string eq_reg = makeName("eq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<eq_reg<<" ]";
                  right->print(dst,f_name, eq_reg,start_label,end_label);
                  dst<<"\n\t\tEQUAL_COMP [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<eq_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string eq_reg = makeName("eq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<eq_reg<<" ]";
                  left->print(dst,f_name, eq_reg,start_label,end_label);
                  dst<<"\n\t\tEQUAL_COMP [ "<<destReg<<" "<<eq_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } 
              else if(left->getName() == "FLOAT_CONSTANT") {
                  std::string eq_reg = makeName("eq_reg");
                  std::string eq_reg2 = makeName("eq_reg");
                  dst<<"\n\t\tVARIABLE [ float "<<eq_reg<<" ]";
                  left->print(dst,f_name, eq_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ float "<<eq_reg2<<" ]";
                  right->print(dst,f_name, eq_reg2,start_label,end_label);
                  dst<<"\n\t\tEQUAL_COMP [ "<<destReg<<" "<<eq_reg<<" "<<eq_reg2<<" ]";
              }
              else {
                  std::string eq_reg = makeName("eq_reg");
                  std::string eq_reg2 = makeName("eq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<eq_reg<<" ]";
                  left->print(dst,f_name, eq_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<eq_reg2<<" ]";
                  right->print(dst,f_name, eq_reg2,start_label,end_label);
                  dst<<"\n\t\tEQUAL_COMP [ "<<destReg<<" "<<eq_reg<<" "<<eq_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg == RightArg;
        }
};

class LessThan_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        LessThan_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "LT_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tLESS_THAN [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string lt_reg = makeName("lt_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lt_reg<<" ]";
                  right->print(dst,f_name, lt_reg,start_label,end_label);
                  dst<<"\n\t\tLESS_THAN [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<lt_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string lt_reg = makeName("lt_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lt_reg<<" ]";
                  left->print(dst,f_name, lt_reg,start_label,end_label);
                  dst<<"\n\t\tLESS_THAN [ "<<destReg<<" "<<lt_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string lt_reg = makeName("lt_reg");
                  std::string lt_reg2 = makeName("lt_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lt_reg<<" ]";
                  left->print(dst,f_name, lt_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<lt_reg2<<" ]";
                  right->print(dst,f_name, lt_reg2,start_label,end_label);
                  dst<<"\n\t\tLESS_THAN [ "<<destReg<<" "<<lt_reg<<" "<<lt_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg < RightArg;
        }
};

class GreaterThan_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        GreaterThan_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "GT_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tGREATER_THAN [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string gt_reg = makeName("gt_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<gt_reg<<" ]";
                  right->print(dst,f_name, gt_reg,start_label,end_label);
                  dst<<"\n\t\tGREATER_THAN [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<gt_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string gt_reg = makeName("gt_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<gt_reg<<" ]";
                  left->print(dst,f_name, gt_reg,start_label,end_label);
                  dst<<"\n\t\tGREATER_THAN [ "<<destReg<<" "<<gt_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string gt_reg = makeName("gt_reg");
                  std::string gt_reg2 = makeName("gt_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<gt_reg<<" ]";
                  left->print(dst,f_name, gt_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<gt_reg2<<" ]";
                  right->print(dst,f_name, gt_reg2,start_label,end_label);
                  dst<<"\n\t\tGREATER_THAN [ "<<destReg<<" "<<gt_reg<<" "<<gt_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg > RightArg;
        }
};

class LessThanEqualTo_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        LessThanEqualTo_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "LTEQ_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tLESS_THANEQ [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string lteq_reg = makeName("lteq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lteq_reg<<" ]";
                  right->print(dst,f_name, lteq_reg,start_label,end_label);
                  dst<<"\n\t\tLESS_THANEQ [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<lteq_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string lteq_reg = makeName("lteq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lteq_reg<<" ]";
                  left->print(dst,f_name, lteq_reg,start_label,end_label);
                  dst<<"\n\t\tLESS_THANEQ [ "<<destReg<<" "<<lteq_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string lteq_reg = makeName("lteq_reg");
                  std::string lteq_reg2 = makeName("lteq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lteq_reg<<" ]";
                  left->print(dst,f_name, lteq_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<lteq_reg2<<" ]";
                  right->print(dst,f_name, lteq_reg2,start_label,end_label);
                  dst<<"\n\t\tLESS_THANEQ [ "<<destReg<<" "<<lteq_reg<<" "<<lteq_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg <= RightArg;
        }
};

class GreaterThanEqualTo_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        GreaterThanEqualTo_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "GTEQ_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tGREATER_THANEQ [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string gteq_reg = makeName("gteq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<gteq_reg<<" ]";
                  right->print(dst,f_name, gteq_reg,start_label,end_label);
                  dst<<"\n\t\tGREATER_THANEQ [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<gteq_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string gteq_reg = makeName("gteq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<gteq_reg<<" ]";
                  left->print(dst,f_name, gteq_reg,start_label,end_label);
                  dst<<"\n\t\tGREATER_THANEQ [ "<<destReg<<" "<<gteq_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string gteq_reg = makeName("gteq_reg");
                  std::string gteq_reg2 = makeName("gteq_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<gteq_reg<<" ]";
                  left->print(dst,f_name, gteq_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<gteq_reg2<<" ]";
                  right->print(dst,f_name, gteq_reg2,start_label,end_label);
                  dst<<"\n\t\tGREATER_THANEQ [ "<<destReg<<" "<<gteq_reg<<" "<<gteq_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg <= RightArg;
        }
};

class NotEquals_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        NotEquals_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "NOTEQUALS_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tNOT_EQUALS [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string ne_reg = makeName("ne_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<ne_reg<<" ]";
                  right->print(dst,f_name, ne_reg,start_label,end_label);
                  dst<<"\n\t\tNOT_EQUALS [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<ne_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string ne_reg = makeName("ne_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<ne_reg<<" ]";
                  left->print(dst,f_name, ne_reg,start_label,end_label);
                  dst<<"\n\t\tNOT_EQUALS [ "<<destReg<<" "<<ne_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string ne_reg = makeName("ne_reg");
                  std::string ne_reg2 = makeName("ne_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<ne_reg<<" ]";
                  left->print(dst,f_name, ne_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<ne_reg2<<" ]";
                  right->print(dst,f_name, ne_reg2,start_label,end_label);
                  dst<<"\n\t\tNOT_EQUALS [ "<<destReg<<" "<<ne_reg<<" "<<ne_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getLeftValue() -> evaluate(bindings);
            double RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg <= RightArg;
        }
};

class BitwiseAND_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        BitwiseAND_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "BITWISE_AND_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tBITWISE_AND [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string bwand_reg = makeName("bwand_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<bwand_reg<<" ]";
                  right->print(dst,f_name, bwand_reg,start_label,end_label);
                  dst<<"\n\t\tBITWISE_AND [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<bwand_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string bwand_reg = makeName("bwand_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<bwand_reg<<" ]";
                  left->print(dst,f_name, bwand_reg,start_label,end_label);
                  dst<<"\n\t\tBITWISE_AND [ "<<destReg<<" "<<bwand_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string bwand_reg = makeName("bwand_reg");
                  std::string bwand_reg2 = makeName("bwand_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<bwand_reg<<" ]";
                  left->print(dst,f_name, bwand_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<bwand_reg2<<" ]";
                  right->print(dst,f_name, bwand_reg2,start_label,end_label);
                  dst<<"\n\t\tBITWISE_AND [ "<<destReg<<" "<<bwand_reg<<" "<<bwand_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            int LeftArg = getLeftValue() -> evaluate(bindings);
            int RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg & RightArg; //Changed datatypes here
        }
};

class BitwiseOR_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        BitwiseOR_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "BITWISE_OR_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tBITWISE_OR [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string bwor_reg = makeName("bwor_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<bwor_reg<<" ]";
                  right->print(dst,f_name, bwor_reg,start_label,end_label);
                  dst<<"\n\t\tBITWISE_OR [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<bwor_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string bwor_reg = makeName("bwor_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<bwor_reg<<" ]";
                  left->print(dst,f_name, bwor_reg,start_label,end_label);
                  dst<<"\n\t\tBITWISE_OR [ "<<destReg<<" "<<bwor_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string bwor_reg = makeName("bwor_reg");
                  std::string bwor_reg2 = makeName("bwor_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<bwor_reg<<" ]";
                  left->print(dst,f_name, bwor_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<bwor_reg2<<" ]";
                  right->print(dst,f_name, bwor_reg2,start_label,end_label);
                  dst<<"\n\t\tBITWISE_OR [ "<<destReg<<" "<<bwor_reg<<" "<<bwor_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            int LeftArg = getLeftValue() -> evaluate(bindings);
            int RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg | RightArg; //Changed datatypes here
        }
};

class BitwiseXOR_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        BitwiseXOR_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "XOR_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tXOR [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string xor_reg = makeName("xor_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<xor_reg<<" ]";
                  right->print(dst,f_name, xor_reg,start_label,end_label);
                  dst<<"\n\t\tXOR [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<xor_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string xor_reg = makeName("xor_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<xor_reg<<" ]";
                  left->print(dst,f_name, xor_reg,start_label,end_label);
                  dst<<"\n\t\tXOR [ "<<destReg<<" "<<xor_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string xor_reg = makeName("xor_reg");
                  std::string xor_reg2 = makeName("xor_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<xor_reg<<" ]";
                  left->print(dst,f_name, xor_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<xor_reg2<<" ]";
                  right->print(dst,f_name, xor_reg2,start_label,end_label);
                  dst<<"\n\t\tXOR [ "<<destReg<<" "<<xor_reg<<" "<<xor_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            int LeftArg = getLeftValue() -> evaluate(bindings);
            int RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg ^ RightArg; //Only XOR takes ints not double.
        }
};

class LEFTSHIFT_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        LEFTSHIFT_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "LEFTSHIFT_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tLEFTSHIFT [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string lshift_reg = makeName("lshift_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lshift_reg<<" ]";
                  right->print(dst,f_name, lshift_reg,start_label,end_label);
                  dst<<"\n\t\tLEFTSHIFT [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<lshift_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string lshift_reg = makeName("lshift_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lshift_reg<<" ]";
                  left->print(dst,f_name, lshift_reg,start_label,end_label);
                  dst<<"\n\t\tLEFTSHIFT [ "<<destReg<<" "<<lshift_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string lshift_reg = makeName("lshift_reg");
                  std::string lshift_reg2 = makeName("lshift_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<lshift_reg<<" ]";
                  left->print(dst,f_name, lshift_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<lshift_reg2<<" ]";
                  right->print(dst,f_name, lshift_reg2,start_label,end_label);
                  dst<<"\n\t\tLEFTSHIFT [ "<<destReg<<" "<<lshift_reg<<" "<<lshift_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            int LeftArg = getLeftValue() -> evaluate(bindings);
            int RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg << RightArg; //Changed datatypes here
        }
};

class RIGHTSHIFT_Operator : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        RIGHTSHIFT_Operator(const NodePtr _left, const NodePtr _right){
            left = _left;
            right = _right;
            _name = "RIGHTSHIFT_OPERATOR";
        }

        NodePtr getLeftValue() const {
            return left;
        }

        NodePtr getRightValue() const {
            return right;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          if (left->getName()=="VARIABLE_CALL"||left->getName()=="INTEGER_CONSTANT"){
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  dst<<"\n\t\tRIGHTSHIFT [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string rshift_reg = makeName("rshift_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<rshift_reg<<" ]";
                  right->print(dst,f_name, rshift_reg,start_label,end_label);
                  dst<<"\n\t\tRIGHTSHIFT [ "<<destReg<<" ";
                  left->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" "<<rshift_reg;
                  dst<<" ]";
              }
          } else{
              if (right->getName()=="VARIABLE_CALL"||right->getName()=="INTEGER_CONSTANT"){
                  std::string rshift_reg = makeName("rshift_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<rshift_reg<<" ]";
                  left->print(dst,f_name, rshift_reg,start_label,end_label);
                  dst<<"\n\t\tRIGHTSHIFT [ "<<destReg<<" "<<rshift_reg<<" ";
                  right->print(dst,f_name, destReg,start_label,end_label);
                  dst<<" ]";
              } else {
                  std::string rshift_reg = makeName("rshift_reg");
                  std::string rshift_reg2 = makeName("rshift_reg");
                  dst<<"\n\t\tVARIABLE [ int "<<rshift_reg<<" ]";
                  left->print(dst,f_name, rshift_reg,start_label,end_label);
                  dst<<"\n\t\tVARIABLE [ int "<<rshift_reg2<<" ]";
                  right->print(dst,f_name, rshift_reg2,start_label,end_label);
                  dst<<"\n\t\tRIGHTSHIFT [ "<<destReg<<" "<<rshift_reg<<" "<<rshift_reg2<<" ]";
              }
          }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            int LeftArg = getLeftValue() -> evaluate(bindings);
            int RightArg = getRightValue() -> evaluate(bindings);
            return LeftArg >> RightArg; //Changed datatypes here
        }
};

#endif
