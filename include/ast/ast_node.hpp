#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <iostream>
#include <map>

static int makeNameUnq=0;

static std::string makeName(std::string base)
{
    return "_"+base+"_"+std::to_string(makeNameUnq++);
}

class Node;

typedef const Node *NodePtr;

class Node {
    protected:
        std::string _name;
    public:
        virtual ~Node() {}
        virtual const std::string &getName() const {
            return _name;
        }
        virtual double evaluate(
            const std::map<std::string,double> &bindings
        ) const
        { throw std::runtime_error("Not implemented."); }
        //Gives us the mapping we need to implement operations

        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const =0;

};

class Frame : public Node {
    protected:
        NodePtr left;
        NodePtr right;

    public:
        Frame(NodePtr _left, NodePtr _right) {
            _name = "FRAME";
            left = _left;    // Pointer to previous frame
            right = _right;  // Pointer to current frame
        }

        NodePtr getLeft() const {
            return left;
        }
        NodePtr getRight() const {
            return right;
        }
        virtual void print(std::ostream &dst,std::string f_name,std::string destReg, std::string start_label, std::string end_label) const override {
            left->print(dst,f_name,destReg,start_label,end_label);
            if (left->getName()!="FUNCTION_DECLARATION"&&left->getName()!="GLOBAL_ARRAY_DECLARATION") {
                dst<<"\n";
            }
            right->print(dst,f_name,destReg,start_label,end_label);
        }
};


#endif
