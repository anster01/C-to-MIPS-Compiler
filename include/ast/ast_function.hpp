#ifndef ast_function_hpp
#define ast_function_hpp

#include <string>
#include <iostream>

class Function : public Node {
    protected:
        std::string id;
        NodePtr vartype;
        NodePtr args;
        NodePtr wrapped_lines;

    public:
        Function(NodePtr &_vartype, const std::string &_id, NodePtr _args, NodePtr _function_lines){
            id = _id;
            vartype = _vartype;
            args = _args;
            wrapped_lines = _function_lines;
             _name = "FUNCTION";
        }

        const std::string getId() const {
            return id;
        }

        NodePtr getVarType() const {
            return vartype;
        }

        NodePtr getArguments() const {
            return args;
        }

        NodePtr getWrappedLines() const {
            return wrapped_lines;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"FUNCTION ";
            vartype->print(dst,f_name,destReg,start_label,end_label);
            dst<<" "<<id<<":";
            args->print(dst,id,destReg,start_label,end_label);
            wrapped_lines->print(dst,id,destReg,start_label,end_label);
            dst<<"\nENDFUNCTION end"<<id<<":";
        }
};

class Function_Call : public Node {
    protected:
        std::string id;
        NodePtr wrapped_params;

      public:
          Function_Call(const std::string &_id, NodePtr _wrapped_params){
              id = _id;
              wrapped_params = _wrapped_params;
               _name = "FUNCTION_CALL";
          }

          const std::string getId() const {
              return id;
          }

          NodePtr getParameters() const {
              return wrapped_params;
          }

          virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
              std::string call_reg = makeName("call_reg");
              wrapped_params->print(dst,id,destReg,start_label,end_label);
              dst<<"\n\t\tCALL [ "<<destReg<<" "<<id<<" ]";
          }
};

class Function_Declaration : public Node {
    protected:
        NodePtr vartype;
        std::string id;
        NodePtr wrapped_args;

      public:
          Function_Declaration(NodePtr &_vartype, const std::string &_id, NodePtr _wrapped_args){
              id = _id;
              vartype = _vartype;
              wrapped_args = _wrapped_args;
               _name = "FUNCTION_DECLARATION";
          }

          const std::string getId() const {
              return id;
          }

          NodePtr getVarType() const {
              return vartype;
          }

          NodePtr getArguments() const {
              return wrapped_args;
          }

          virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
          }
};

#endif
