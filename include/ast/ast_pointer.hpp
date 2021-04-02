#ifndef ast_pointer_hpp
#define ast_pointer_hpp

#include <string>
#include <iostream>

class Pointer_Declaration : public Node {
    private:
        std::string id;
        NodePtr vartype;

    public:
        Pointer_Declaration(NodePtr _vartype, const std::string &_id){
            id = _id;
            _name = "POINTER";
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
            dst<<"pointer"<<" "<<id<<" ]";
        }
};

class Pointer_Initialisation : public Node {
    private:
        NodePtr vartype;
        std::string id;
        std::string value;

    public:
        Pointer_Initialisation(NodePtr _vartype, const std::string &_id, const std::string _value){
            vartype = _vartype;
            id = _id;
            _name = "POINTER_INITIALISATION";
            value = _value;
        }

        NodePtr getVarType() const {
            return vartype;
        }

        const std::string getId() const {
            return id;
        }

        const std::string getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tVARIABLE [ ";
            vartype->print(dst,f_name, destReg,start_label,end_label);
            dst<<"pointer"<<" "<<id<<" ]";
            dst<<"\n\t\tADDRESS_OF [ "<<id<<" "<<value<<" ]";
        }
};

class Pointer_Assignment : public Node {
    private:
        std::string id;
        NodePtr value;

    public:
        Pointer_Assignment(const std::string &_id, NodePtr _value){
            id = _id;
            _name = "POINTER_ASSIGNMENT";
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
                dst<<"\n\t\tPOINTER_ASSIGN [ "<<id<<" ";
                value->print(dst,f_name,destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string pointer_reg = makeName("pointer_reg");
                dst<<"\n\t\tVARIABLE [ int "<<pointer_reg<<" ]";
                value->print(dst,f_name, pointer_reg,start_label,end_label);
                dst<<"\n\t\tPOINTER_ASSIGN [ "<<id<<" "<<pointer_reg<<" ]";
            }
        }
};

class Pointer_Call : public Node {
    private:
        std::string id;

    public:
        Pointer_Call(const std::string &_id){
            id = _id;
            _name = "POINTER_CALL";
        }

        const std::string getId() const {
            return id;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tPOINTER_CALL [ "<<destReg<<" "<<id<<" ]";
        }
};

class Pointer_Arg : public Node {
    private:
        std::string id;
        NodePtr vartype;

    public:
        Pointer_Arg(NodePtr _vartype, const std::string &_id){
            id = _id;
            _name = "POINTER_ARG";
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
            dst<<"pointer "<<id<<" ]";
        }
};

#endif
