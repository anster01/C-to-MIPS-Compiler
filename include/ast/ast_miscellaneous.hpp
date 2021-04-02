#ifndef ast_miscellaneous_hpp
#define ast_miscellaneous_hpp

#include <string>
#include <iostream>

class WrappedArguments : public Node {
    protected:
        NodePtr args;

    public:
        WrappedArguments(NodePtr _args){
            args = _args;
            _name = "WRAPPED_ARGUMENTS";
        }

        NodePtr getArguments() const {
            return args;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (args != NULL)
                args->print(dst,f_name, destReg,start_label,end_label);
        }
};

class Arguments : public Node {
    protected:
        NodePtr arg;
        NodePtr next_arg;

    public:
        Arguments(NodePtr _args, NodePtr _next_arg){
            arg = _args;
            next_arg = _next_arg;
            _name = "ARGUMENTS";
        }

        NodePtr getArguments() const {
            return arg;
        }

        NodePtr getNextArguments() const {
            return next_arg;
        }
        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (arg != NULL)
                arg->print(dst,f_name, destReg,start_label,end_label);
            if (next_arg != NULL)
                next_arg->print(dst,f_name, destReg,start_label,end_label);
        }
};

class Wrapped_Parameters : public Node {
    protected:
        NodePtr parameters;

    public:
        Wrapped_Parameters(NodePtr _params){
            parameters = _params;
            _name = "WRAPPED_PARAMETERS";
        }

        NodePtr getParameters() const {
            return parameters;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (parameters != NULL)
                parameters->print(dst,f_name, destReg,start_label,end_label);
        }
};

class Parameters : public Node {
    protected:
        NodePtr param;
        NodePtr next_param;

    public:
        Parameters(NodePtr _param, NodePtr _next_param){
            param = _param;
            next_param = _next_param;
            _name = "PARAMETERS";
        }

        NodePtr getParameters() const {
            return param;
        }

        NodePtr getNextParameters() const {
            return next_param;
        }
        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            std::string param_reg = makeName("param_reg");
            if (param != NULL){
                if (param->getName()=="INTEGER_CONSTANT" || param->getName()=="VARIABLE_CALL"){
                    dst<<"\n\t\tARGUMENT [ ";
                    param->print(dst,f_name,destReg,start_label,end_label);
                    dst<<" ]";
                } else {
                    dst<<"\n\t\tVARIABLE [ int "<<param_reg<<" ]";
                    param->print(dst,f_name,param_reg,start_label,end_label);
                    dst<<"\n\t\tARGUMENT [ "<<param_reg<<" ]";
                }
            }
            if (next_param != NULL)
                next_param->print(dst,f_name, destReg,start_label,end_label);
        }
};

class Cases : public Node {
    protected:
        NodePtr condition;
    public:
        Cases(NodePtr _condition) {
            condition = _condition;
            _name = "CASE";
        }
    NodePtr getCondition() const {
        return condition;
    }
    virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
        if (condition != NULL) {
            condition->print(dst, f_name, destReg,start_label,end_label);
        }
    }
};

class WrappedLines : public Node {
    protected:
        NodePtr lines;

    public:
        WrappedLines(NodePtr _lines){
            lines =_lines;
            _name = "WRAPPED_LINES";
        }

        NodePtr getLines() const {
            return lines;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (lines != NULL)
                lines->print(dst,f_name, destReg,start_label,end_label);
        }
};

class Lines : public Node {
    protected:
        NodePtr line;
        NodePtr next_line;

    public:
        Lines(NodePtr _line, NodePtr _next_line){
            line = _line;
            next_line = _next_line;
            _name = "LINES";
        }

        NodePtr getLines() const {
            return line;
        }

        NodePtr getNextLines() const {
            return next_line;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (line != NULL)
                line->print(dst,f_name, destReg,start_label,end_label);
            if (next_line != NULL)
                next_line->print(dst,f_name, destReg,start_label,end_label);
        }
};

#endif
