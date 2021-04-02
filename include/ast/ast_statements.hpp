#ifndef ast_statements_hpp
#define ast_statements_hpp

#include <string>
#include <iostream>


class If_Statement : public Node {
    protected:
        NodePtr condition;
        NodePtr if_lines;
        NodePtr else_lines2;

    public:
        If_Statement(NodePtr _condition, NodePtr _if_lines, NodePtr _else_lines){
            condition = _condition;
            if_lines = _if_lines;
            else_lines2 = _else_lines;
            _name = "IF_STATEMENT";
        }

        NodePtr getCondition() const {
            return condition;
        }

        NodePtr getWrappedLines() const {
            return if_lines;
        }

        NodePtr getWrappedLines2() const {
            return else_lines2;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            std::string start_else = makeName("startelse");
            std::string end_if = makeName("endif");
            std::string cond_reg = makeName("cond");
            dst<<"\n\t\tVARIABLE [ int "<<cond_reg<<" ]";
            if (condition->getName()=="INTEGER_CONSTANT" || condition->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tASSIGN [ "<<cond_reg<< " ";
                condition->print(dst,f_name,destReg,start_label,end_label);
                dst<<" ]";
            } else {
                condition->print(dst,f_name,cond_reg,start_label,end_label);

            }
            if (else_lines2 != NULL)
                dst<<"\n\t\tIFFALSE "<<cond_reg<<" GOTO "<<start_else;
            else
                dst<<"\n\t\tIFFALSE "<<cond_reg<<" GOTO "<<end_if;
            if_lines->print(dst,f_name,destReg,start_label,end_label);
            dst<<"\n\t\tGOTO "<<end_if;
            if (else_lines2 != NULL) {
                dst<<"\nLABEL "<<start_else<<":";
                else_lines2->print(dst,f_name,destReg,start_label,end_label);
            }
            dst<<"\nLABEL "<<end_if<<":";
        }
};

class Switch_Statement : public Node {
    protected:
        NodePtr condition;
        NodePtr case_lines;
        NodePtr break_line;

    public:
        Switch_Statement(NodePtr _condition, NodePtr _case_lines, NodePtr _break_lines){
            condition = _condition;
            case_lines = _case_lines;
            break_line = _break_lines;
            _name = "SWITCH_STATEMENT";
        }

        NodePtr getCondition() const {
            return condition;
        }

        NodePtr getCaseLines() const {
            return case_lines;
        }

        NodePtr getBreakLine() const {
            return break_line;
        }
        
        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            std::string end_case = makeName("endcase");
            std::string cond_reg = makeName("cond");
            std::string end_switch = makeName("endSwitch");
            /*
            dst<<"\n\t\tVARIABLE [ int "<<cond_reg<<" ]"; //Declare the cond_reg variable that we will compare the value against 
            condition->print(dst,f_name,destReg,start_label,end_label);
            dst<<"\n\t\tIFFALSE "<<cond_reg<<" GOTO "<<end_case;
            case_lines->print(dst,f_name,destReg,start_label,end_label);
            dst<<"\n\t\tGOTO "<<end_switch;
            dst<<"\nLABEL "<<end_switch<<":";
            */
            dst << "1";
        }


};

class While_Loop : public Node {
    protected:
        NodePtr condition;
        NodePtr wrapped_lines;

    public:
        While_Loop(NodePtr _condition, NodePtr _while_lines){
            condition = _condition;
            wrapped_lines = _while_lines;
            _name = "WHILE_LOOP";
        }

        NodePtr getCondition() const {
            return condition;
        }

        NodePtr getWrappedLines() {
            return wrapped_lines;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            std::string startloop = makeName("startwhile");
            std::string endloop = makeName("endwhile");
            std::string cond_reg = makeName("cond");
            dst<<"\n\t\tVARIABLE [ int "<<cond_reg<<" ]";
            dst<<"\nLABEL "<<startloop<<":";
            if (condition->getName()=="INTEGER_CONSTANT" || condition->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tASSIGN [ "<<cond_reg<<" ";
                condition->print(dst,f_name,destReg,startloop,endloop);
                dst<<" ]";
            } else {
                condition->print(dst,f_name,cond_reg,startloop,endloop);

            }
            dst<<"\n\t\tIFFALSE "<<cond_reg<<" GOTO "<<endloop;
            if (wrapped_lines != NULL){
                wrapped_lines->print(dst,f_name,destReg,startloop,endloop);
                dst<<"\n\t\tGOTO "<<startloop;
            }
            dst<<"\nLABEL "<<endloop<<":";
        }
};

class For_Loop : public Node {
    protected:
        NodePtr cond1;
        NodePtr cond2;
        NodePtr cond3;
        NodePtr wrapped_lines;

    public:
        For_Loop(NodePtr _cond1, NodePtr _cond2, NodePtr _cond3, NodePtr _for_lines){
            cond1 = _cond1;
            cond2 = _cond2;
            cond3 = _cond3;
            wrapped_lines = _for_lines;
            _name = "FOR_LOOP";
        }

        NodePtr getCondition1() const {
            return cond1;
        }
        NodePtr getCondition2() const {
            return cond2;
        }
        NodePtr getCondition3() const {
            return cond3;
        }

        NodePtr getWrappedLines() {
            return wrapped_lines;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            std::string startloop = makeName("startfor");
            std::string cond_reg = makeName("cond");
            std::string endloop = makeName("endfor");
            cond1->print(dst,f_name,destReg,startloop,endloop);
            dst<<"\n\t\tVARIABLE [ int "<<cond_reg<<" ]";
            dst<<"\nLABEL "<<startloop<<":";
            if (cond2->getName()=="INTEGER_CONSTANT" || cond2->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\tASSIGN [ "<<cond_reg<<" ";
                cond2->print(dst,f_name,destReg,startloop,endloop);
                dst<<" ]";
            } else {
                cond2->print(dst,f_name,cond_reg,startloop,endloop);

            }
            dst<<"\n\t\tIFFALSE "<<cond_reg<<" GOTO "<<endloop;
            if (wrapped_lines != NULL){
                wrapped_lines->print(dst,f_name,destReg,startloop,endloop);
            }
            cond3->print(dst,f_name,destReg,startloop,endloop);
            dst<<"\n\t\tGOTO "<<startloop;
            dst<<"\nLABEL "<<endloop<<":";
        }
};

class Return_Statement : public Node {
    protected:
        NodePtr expr;

    public:
        Return_Statement(NodePtr _expr){
            expr = _expr;
            _name="RETURN_STATEMENT";
        }

        NodePtr getExpression() const {
            return expr;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            if (expr->getName()=="INTEGER_CONSTANT" || expr->getName()=="VARIABLE_CALL"){
                dst<<"\n\t\t RETURN [ ";
                expr->print(dst,f_name,destReg,start_label,end_label);
                dst<<" ]";
                dst<<"\n\t\tGOTO end"<<f_name;
            } else {
                std::string ret = makeName("ret");
                dst<<"\n\t\tVARIABLE [ int "<<ret<<" ]";
                expr->print(dst,f_name,ret,start_label,end_label);
                dst<<"\n\t\tRETURN [ "<<ret<<" ]";
                dst<<"\n\t\tGOTO end"<<f_name;
            }
        }
};

class Break_Statement : public Node {
    public:
        Break_Statement() {
            _name="BREAK_STATEMENT";
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tGOTO "<<end_label;
        }
};

class Continue_Statement : public Node {
    public:
        Continue_Statement() {
            _name="CONTINUE_STATEMENT";
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tGOTO "<<start_label;
        }
};

class Sizeof_Statement : public Node {
    protected:
        std::string id;

    public:
        Sizeof_Statement(const std::string &_id) {
            id = _id;
            _name="BREAK_STATEMENT";
        }

        const std::string getId() const {
            return id;
        }

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            dst<<"\n\t\tSIZEOF [ "<<destReg<<" "<<id<<" ]";
        }

};


#endif
