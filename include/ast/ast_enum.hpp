#ifndef ast_enum_hpp
#define ast_enum_hpp

#include <string>
#include <iostream>

class Enum_Definition : public Node {
    protected:
        std::string id;
        NodePtr enum_vars;

    public:
        Enum_Definition(const std::string &_id, NodePtr _enum_vars) {
            id = _id;
            enum_vars = _enum_vars;
            _name="ENUM_DEFINITION";
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getEnumVars() const {
            return enum_vars;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            enum_vars->print(dst,f_name,id,start_label,end_label);
        }
};

class Enum_Multiple_Variables : public Node {
    protected:
        NodePtr var;
        NodePtr next_var;

    public:
        Enum_Multiple_Variables(NodePtr _var, NodePtr _next_var){
            var = _var;
            next_var = _next_var;
            _name = "ENUM_MULTIPLE_VARIABLES";
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

class Enum_Variable : public Node {
    protected:
        std::string id;
        int value;

    public:
        Enum_Variable(const std::string &_id, const int _value){
            id = _id;
            _name = "ENUM_VARIABLE";
            value = _value;
        }
        const std::string getId() const {
            return id;
        }
        int getValue() const {
            return value;
        }
        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value==0) {
                dst<<"ENUM_VAR [ "<<destReg<<" ";
                dst<<" "<<id<<" ]";
            } else {
                dst<<"ENUM_VAR [ "<<destReg<<" ";
                dst<<" "<<id<<" "<<value<<" ]";
            }

        }
};

#endif
