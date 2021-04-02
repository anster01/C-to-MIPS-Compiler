#ifndef ast_array_hpp
#define ast_array_hpp

#include <string>
#include <iostream>

class Global_Array_Declaration : public Node {
    private:
        std::string id;
        NodePtr vartype;
        int value;
        int value2;

    public:
        Global_Array_Declaration(NodePtr _vartype, const std::string &_id, const int _value, const int _value2){
            id = _id;
            _name = "GLOBAL_ARRAY_DECLARATION";
            vartype = _vartype;
            value = _value;
            value2 = _value2;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getVarType() const {
            return vartype;
        }

        int getValue() const {
            return value;
        }

        int getValue2() const {
            return value2;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
        }
};

class Array_Declaration : public Node {
    private:
        std::string id;
        NodePtr vartype;
        int value;
        int value2;

    public:
        Array_Declaration(NodePtr _vartype, const std::string &_id, const int _value, const int _value2){
            id = _id;
            _name = "ARRAY_DECLARATION";
            vartype = _vartype;
            value = _value;
            value2 = _value2;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getVarType() const {
            return vartype;
        }

        int getValue() const {
            return value;
        }

        int getValue2() const {
            return value2;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value2 == 0){
                for (int i=0;i<value;i++) {
                    dst<<"\n\t\tVARIABLE [ ";
                    vartype->print(dst,f_name,destReg,start_label,end_label);
                    dst<<" "<<id<<"("<<std::to_string(i)<<")"<<" ]";
                }
            } else {
                for (int i=0;i<value;i++) {
                    for (int j=0;j<value2;j++) {
                        dst<<"\n\t\tVARIABLE [ ";
                        vartype->print(dst,f_name,destReg,start_label,end_label);
                        dst<<" "<<id<<"("<<std::to_string(i)<<")"<<"("<<std::to_string(j)<<")"<<" ]";
                    }
                }
            }
        }
};

class Array_Assignment : public Node {
    private:
        std::string id;
        NodePtr value;
        NodePtr index;
        NodePtr index2;

    public:
        Array_Assignment(const std::string &_id, NodePtr _index, NodePtr _index2, NodePtr _value){
            id = _id;
            _name = "ARRAY_ASSIGNMENT";
            value = _value;
            index = _index;
            index2 = _index2;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getValue() const {
            return value;
        }

        NodePtr getIndex() const {
            return index;
        }

        NodePtr getIndex2() const {
            return index2;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (value->getName()=="INTEGER_CONSTANT" || value->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tASSIGN [ "<<id<<"(";
                index->print(dst,f_name,destReg,start_label,end_label);
                dst<<")";
                if (index2 != NULL) {
                    dst<<"(";
                    index2->print(dst,f_name,destReg,start_label,end_label);
                    dst<<")";
                }
                dst<<" ";
                value->print(dst,f_name,destReg,start_label,end_label);
                dst<<" ]";
            }
        }
};

class Array_Call : public Node {
    private:
        std::string id;
        NodePtr index;
        NodePtr index2;

    public:
        Array_Call(const std::string &_id, NodePtr _index, NodePtr _index2){
            id = _id;
            _name = "VARIABLE_CALL";
            index = _index;
            index2 = _index2;
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getIndex() const {
            return index;
        }

        NodePtr getIndex2() const {
            return index2;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<id<<"(";
            index->print(dst,f_name,destReg,start_label,end_label);
            dst<<")";
            if (index2 != NULL) {
                dst<<"(";
                index2->print(dst,f_name,destReg,start_label,end_label);
                dst<<")";
            }
        }
};

#endif
