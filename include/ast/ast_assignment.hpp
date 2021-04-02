#ifndef ast_assignment_hpp
#define ast_assignment_hpp

#include <string>
#include <iostream>

class Variable_Add_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_Add_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_ADD_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tADD [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string add_reg = makeName("add_reg");
                value->print(dst,f_name, add_reg,start_label,end_label);
                dst<<"\n\t\tADD [ "<<id<<" "<<id<<" "<<add_reg<<" "<<" ]";
            }
        }

};

class Variable_Sub_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_Sub_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_SUB_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tSUB [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string sub_reg = makeName("sub_reg");
                value->print(dst,f_name, sub_reg,start_label,end_label);
                dst<<"\n\t\tSUB [ "<<id<<" "<<id<<" "<<sub_reg<<" "<<" ]";
            }
        }

};

class Variable_Mul_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_Mul_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_MUL_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tMUL [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string mul_reg = makeName("mul_reg");
                value->print(dst,f_name, mul_reg,start_label,end_label);
                dst<<"\n\t\tMUL [ "<<id<<" "<<id<<" "<<mul_reg<<" "<<" ]";
            }
        }

};

class Variable_Div_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_Div_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_DIV_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tDIV [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string div_reg = makeName("div_reg");
                value->print(dst,f_name, div_reg,start_label,end_label);
                dst<<"\n\t\tDIV [ "<<id<<" "<<id<<" "<<div_reg<<" "<<" ]";
            }
        }

};

class Variable_Mod_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_Mod_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_MOD_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tMODULO [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string mod_reg = makeName("mod_reg");
                value->print(dst,f_name, mod_reg,start_label,end_label);
                dst<<"\n\t\tMODULO [ "<<id<<" "<<id<<" "<<mod_reg<<" "<<" ]";
            }
        }

};

class Variable_LeftShift_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_LeftShift_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_LEFTSHIFT_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tLEFTSHIFT [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string lshift_reg = makeName("lshift_reg");
                value->print(dst,f_name, lshift_reg,start_label,end_label);
                dst<<"\n\t\tLEFTSHIFT [ "<<id<<" "<<id<<" "<<lshift_reg<<" "<<" ]";
            }
        }

};

class Variable_RightShift_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_RightShift_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_RIGHTSHIFT_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tRIGHTSHIFT [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string rshift_reg = makeName("rshift_reg");
                value->print(dst,f_name, rshift_reg,start_label,end_label);
                dst<<"\n\t\tRIGHTSHIFT [ "<<id<<" "<<id<<" "<<rshift_reg<<" "<<" ]";
            }
        }

};

class Variable_BitwiseAnd_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_BitwiseAnd_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_BITWISEAND_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tBITWISE_AND [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string bwand_reg = makeName("bwand_reg");
                value->print(dst,f_name, bwand_reg,start_label,end_label);
                dst<<"\n\t\tBITWISE_AND [ "<<id<<" "<<id<<" "<<bwand_reg<<" "<<" ]";
            }
        }

};

class Variable_BitwiseOr_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_BitwiseOr_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_BITWISEOR_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tBITWISE_OR [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string bwor_reg = makeName("bwor_reg");
                value->print(dst,f_name, bwor_reg,start_label,end_label);
                dst<<"\n\t\tBITWISE_OR [ "<<id<<" "<<id<<" "<<bwor_reg<<" "<<" ]";
            }
        }

};

class Variable_BitwiseXor_Assignment : public Node {
    protected:
        std::string id;
        NodePtr value;

    public:
        Variable_BitwiseXor_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_BITWISEXOR_ASSIGNMENT";
            value = _value;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tXOR [ "<<id<<" "<<id<<" ";
                value->print(dst,f_name, destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string xor_reg = makeName("xor_reg");
                value->print(dst,f_name, xor_reg,start_label,end_label);
                dst<<"\n\t\tXOR [ "<<id<<" "<<id<<" "<<xor_reg<<" "<<" ]";
            }
        }

};

class Increment_Operator : public Node {
    protected:
        std::string id;

    public:
        Increment_Operator(const std::string &_id){
            id = _id;
            _name = "INCREMENT_OPERATOR";
        }

        const std::string getId() const {
            return id;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tADD [ "<<id<<" "<<id<<" 1 ]";
        }

};

class Decrement_Operator : public Node {
    protected:
        std::string id;

    public:
        Decrement_Operator(const std::string &_id){
            id = _id;
            _name = "DECREMENT_OPERATOR";
        }

        const std::string getId() const {
            return id;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tSUB [ "<<id<<" "<<id<<" 1 ]";
        }

};

#endif
