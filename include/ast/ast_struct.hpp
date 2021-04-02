#ifndef ast_struct_hpp
#define ast_struct_hpp

#include <string>
#include <iostream>

class Struct_Definition : public Node {
    protected:
        std::string id;
        NodePtr struct_vars;

    public:
        Struct_Definition(const std::string &_id, NodePtr _struct_vars) {
            id = _id;
            struct_vars = _struct_vars;
            _name="STRUCT_DEFINITION";
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getStructVars() const {
            return struct_vars;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            struct_vars->print(dst,f_name,id,start_label,end_label);
        }
};

class Struct_Declaration : public Node {
    protected:
        std::string id;
        std::string struct_id;

    public:
        Struct_Declaration(const std::string &_struct_id, const std::string &_id) {
            id = _id;
            struct_id = _struct_id;
            _name="STRUCT_DECLARATION";
        }

        const std::string getId() const {
            return id;
        }

        const std::string getStructId() const {
            return struct_id;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tSTRUCT_DEC [ "<<struct_id<<" "<<id<<" ]";
        }
};

class Multiple_Struct_Vars : public Node {
    protected:
        NodePtr var;
        NodePtr next_var;

    public:
        Multiple_Struct_Vars(NodePtr _var, NodePtr _next_var){
            var = _var;
            next_var = _next_var;
            _name = "MULTIPLR_DTRUCT_VARS";
        }

        NodePtr getVar() const {
            return var;
        }

        NodePtr getNextVars() const {
            return next_var;
        }
        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            var->print(dst,f_name, destReg,start_label,end_label);
            if (next_var != NULL){
                dst<<"\n";
                next_var->print(dst,f_name, destReg,start_label,end_label);
            }
        }
};

class Struct_Variable : public Node {
    protected:
        NodePtr vartype;
        std::string id;

    public:
        Struct_Variable(NodePtr _vartype, const std::string &_id){
            vartype = _vartype;
            id = _id;
            _name = "STRUCT_VARIABLE";
        }
        const std::string getId() const {
            return id;
        }
        NodePtr getVarType() const {
            return vartype;
        }
        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"STRUCT_VAR [ "<<destReg<<" ";
            vartype->print(dst,f_name, destReg,start_label,end_label);
            dst<<" "<<id<<" ]";
        }
};

class Struct_Call : public Node {
    private:
        std::string id;
        std::string id2;

    public:
        Struct_Call(const std::string &_id, const std::string &_id2){
            id = _id;
            _name = "VARIABLE_CALL";
            id2 = _id2;
        }

        const std::string getId() const {
            return id;
        }

        const std::string getId2() const {
            return id2;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<id<<"."<<id2;
        }
};

class Struct_Assignment : public Node {
    private:
        std::string id;
        std::string id2;
        NodePtr value;

    public:
        Struct_Assignment(const std::string &_id,const std::string &_id2, NodePtr _value){
            id = _id;
            _name = "STRUCT_ASSIGNMENT";
            value = _value;
            id2 = _id2;
        }

        const std::string getId() const {
            return id;
        }

        const std::string getId2() const {
            return id2;
        }

        NodePtr getValue() const {
            return value;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tASSIGN [ "<<id<<"."<<id2;
                dst<<" ";
                value->print(dst,f_name,destReg,start_label,end_label);
                dst<<" ]";
            } else {
                std::string varname = id+"."+id2;
                value->print(dst,f_name,varname,start_label,end_label);
            }
        }
};

#endif
