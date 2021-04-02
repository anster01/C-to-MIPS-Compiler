#ifndef ast_unary_hpp
#define ast_unary_hpp

#include <string>
#include <iostream>

class Neg_Operator : public Node {
    protected:
        NodePtr value;

    public:
        Neg_Operator(const NodePtr _value){
            value = _value;
            _name = "NEG_OPERATOR";
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="VARIABLE_CALL"||value->getName()=="INTEGER_CONSTANT"){
                dst<<"\n\t\tNEG [ "<<destReg<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string neg_reg = makeName("neg_reg");
                dst<<"\n\t\tVARIABLE [ int "<<neg_reg<<" ]";
                value->print(dst,f_name, neg_reg,start_label,end_label);
                dst<<"\n\t\tNEG [ "<<destReg<<" "<<neg_reg<<" ]";

            }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getValue() -> evaluate(bindings);
            return -LeftArg;
        }
};

class Not_Operator : public Node {
    protected:
        NodePtr value;

    public:
        Not_Operator(const NodePtr _value){
            value = _value;
            _name = "NOT_OPERATOR";
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="VARIABLE_CALL"||value->getName()=="INTEGER_CONSTANT"){
                dst<<"\n\t\tNOT [ "<<destReg<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string not_reg = makeName("not_reg");
                dst<<"\n\t\tVARIABLE [ int "<<not_reg<<" ]";
                value->print(dst,f_name, not_reg,start_label,end_label);
                dst<<"\n\t\tNOT [ "<<destReg<<" "<<not_reg<<" ]";

            }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getValue() -> evaluate(bindings);
            return -LeftArg;
        }
};

class BitwiseONECOMP_Operator : public Node {
    protected:
        NodePtr value;

    public:
        BitwiseONECOMP_Operator(const NodePtr _value){
            value = _value;
            _name = "BITWISE_ONECOMP_OPERATOR";
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="VARIABLE_CALL"||value->getName()=="INTEGER_CONSTANT"){
                dst<<"\n\t\tBITWISE_ONECOMP [ "<<destReg<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string onecomp_reg = makeName("onecomp_reg");
                dst<<"\n\t\tVARIABLE [ int "<<onecomp_reg<<" ]";
                value->print(dst,f_name, onecomp_reg,start_label,end_label);
                dst<<"\n\t\tBITWISE_ONECOMP [ "<<destReg<<" "<<onecomp_reg<<" ]";

            }
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings
        )
        {
            double LeftArg = getValue() -> evaluate(bindings);
            return -LeftArg;
        }
};

#endif
