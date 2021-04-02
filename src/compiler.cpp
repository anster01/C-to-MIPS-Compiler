#include "ast.hpp"
#include "translate.hpp"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    FILE *infile;

    if (!(infile = fopen(argv[2], "r"))) {
        throw std::runtime_error("[ERROR]");
    }
    yyset_in(infile);

    const Node *ast=parseAST();

    std::ofstream midfile;
    midfile.open (argv[2]+std::string("ref"));

    ast->print(midfile,"", "","","");
    midfile<<std::endl;
    midfile.close();
    std::string threeaddresscode(argv[2]+std::string("ref"));
    std::ofstream outfile;
    outfile.open(argv[4]);
    Translate(threeaddresscode,outfile);
    return 0;
}
