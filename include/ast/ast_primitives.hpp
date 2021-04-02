#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

class Variable : public Node {
    private:
        std::string id;
        NodePtr vartype;

    public:
        Variable(NodePtr _vartype, const std::string &_id){
            id = _id;
            _name = "VARIABLE";
            vartype = _vartype;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getVarType() const {
            return vartype;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tVARIABLE [ ";
            vartype->print(dst,f_name, destReg,start_label,end_label);
            dst<<" "<<id<<" ]";
        }
};

class Variable_Assignment : public Node {
    private:
        std::string id;
        NodePtr value;

    public:
        Variable_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "VARIABLE_ASSIGNMENT";
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
                dst<<"\n\t\tASSIGN [ "<<id<<" ";
                value->print(dst,f_name,destReg,start_label,end_label);
                dst<<" ]";
            } else {
                value->print(dst,f_name, id,start_label,end_label);
            }
        }
};

class Variable_Initialisation : public Node {
    private:
        NodePtr vartype;
        std::string id;
        NodePtr value;

    public:
        Variable_Initialisation(NodePtr _vartype, const std::string &_id, NodePtr _value){
            vartype = _vartype;
            id = _id;
            _name = "VARIABLE_INITIALISATION";
            value = _value;
        }

        NodePtr getVarType() const {
            return vartype;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tVARIABLE [ ";
            vartype->print(dst,f_name, destReg,start_label,end_label);
            dst<<" "<<id<<" ]";
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tASSIGN [ "<<id<<" ";
                value->print(dst,f_name,destReg,start_label,end_label);
                dst<<" ]";
            } else {
                value->print(dst,f_name, id,start_label,end_label);
            }
        }
};

class Variable_Call : public Node {
    private:
        std::string id;

    public:
        Variable_Call(const std::string &_id){
            id = _id;
            _name = "VARIABLE_CALL";
        }

        const std::string getId() const {
            return id;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<id;
        }
};

class Arg : public Node {
    private:
        std::string id;
        NodePtr vartype;

    public:
        Arg(NodePtr _vartype, const std::string &_id){
            id = _id;
            _name = "ARG";
            vartype = _vartype;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getVarType() const {
            return vartype;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tPARAM [ ";
            vartype->print(dst,f_name, destReg,start_label,end_label);
            dst<<" "<<id<<" ]";
        }
};

class Type_Specifier : public Node {
    private:
        std::string type;

    public:
        Type_Specifier(const std::string &_type){
            type = _type;
            _name = "TYPE_SPECIFIER";
        }

        const std::string getType() const {
            return type;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<type;
        }
};

class Integer_Constant : public Node {
    private:
        int value;

    public:
        Integer_Constant(const int _value){
            value = _value;
            _name = "INTEGER_CONSTANT";
        }

        int getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<value;
        }
};

class Float_Constant : public Node {
    private:
        float value;

    public:
        Float_Constant(const float _value){
            value = _value;
            _name = "FLOAT_CONSTANT";
        }

        int getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<value;
        }
};

class Double_Constant : public Node {
    private:
        double value;

    public:
        Double_Constant(const double _value){
            value = _value;
            _name = "DOUBLE_CONSTANT";
        }

        int getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<value;
        }
};
#endif
