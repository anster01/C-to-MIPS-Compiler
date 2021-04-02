#ifndef translate_hpp
#define translate_hpp

#include "ast.hpp"

#include <iostream>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <regex>

using namespace std;

int numberofvariables(string inname) {
    ifstream infile(inname);
    int number_of_variables = 0;
    string line;

    if(infile.is_open()){
        while(getline(infile,line)){
            char delims[] = " []\t:"; //These are the standard delimiters in a MIPs file, we can split these into tokens.
            char linearr[50]="\0"; //Char array, because I can use the C string operations on them
            line.copy(linearr, line.size() + 1); //Copies the line from the .txt string, casts it into the array
            char * firstparam;
            firstparam = strtok(linearr, delims);

            if (strcmp(firstparam, "VARIABLE") == 0) {
                number_of_variables++;
            }
        }
    }
    infile.close();
    return number_of_variables;
}

void Translate (string inname, ofstream & outfile) { //Pass a reference to the file
   regex reInt("^-?[0-9]+$");
   regex reFloat("^-?[0-9]+([.][0-9]*)?$");
   regex reId("^[_a-zA-Z][_a-zA-Z0-9]*(\\([a-zA-Z_][a-zA-Z_0-9]*\\))?(\\([a-zA-Z_][a-zA-Z_0-9]*\\))?$");

   string line;
   vector<string>instructions; //This stores all the instructions we do line by line, we will use an auto to spit this onto the outfile
   int count = 0; //Something to do if we hit white space
   int no_of_vars = numberofvariables(inname); //Just a random approximation for now
   ifstream infile(inname);
   int z = -8; //keep 2 bytes between stack frame

   map<string,pair<string,int>> variable_mem_loc; //mapping variable names to <vartype,mem_location>
   map<string,pair<string,int>>::iterator variable_it;
   map<string,pair<string,string>> parameters_loc; //mapping parameters to <vartype,register>
   map<string,pair<string,string>>::iterator params_it;
   map<string,int> enum_values; //mapping enum variables to value
   map<string,int>::iterator enum_it;

   int curr_enum_val = 0;
   int no_of_params = 0; //keeps count of parameters passed to function
   int no_of_args = 0; //keeps count of arguments passed in function call
   int curr_sp = no_of_vars*4; //keeping the current location of $sp compared to $fp
   int sp_move = 0; //how much $sp has moved for a function call to store arguments

   vector<string> struct_attr;
   map<string,string> struct_map; //mapping declared struct name to struct name

   if (infile.is_open()) {
       while(getline(infile,line)) {
        if(line.empty()) { //This part is the base case, if we hit white space we just increment this count, basically do nothing
            count = count + 1;
        }
        else { //Otherwise, we do the line splitting as normal
           char delims[] = " []\t:"; //These are the standard delimiters in a MIPs file, we can split these into tokens.
           char linearr[50]="\0"; //Char array, because I can use the C string operations on them
           line.copy(linearr, line.size() + 1); //Copies the line from the .txt string, casts it into the array
           char * firstparam; //Four tokens we need to parse the individual parts
           char * secondparam;
           char * thirdparam;
           char * fourthparam;
           char * fifthparam = NULL;
           firstparam = strtok(linearr, delims); //Generate the tokens we need
           if(firstparam != NULL)
                secondparam = strtok(NULL,delims); //This splits it into four, whenever we hit a delimiter we call the function again.
           if(secondparam != NULL)
                thirdparam = strtok(NULL,delims);
           if(thirdparam != NULL)
                fourthparam = strtok(NULL,delims);
           if(fourthparam != NULL)
                fifthparam = strtok(NULL,delims); //This splits the 3AC code into tokens of individual parameters, given a max number of 4 tokens, we do this.
            //cout << firstparam << endl;
            //cout << secondparam << endl;
            //cout << thirdparam << endl;
            //cout << fourthparam << endl; //This is to test that we got the right tokens, need data structure to hold these.
           //Should do a loop over the whole file to count the number of operations/additions over memory, keep a tally of this and use this to allocate number of bytes. etc.
            if (strcmp(firstparam, "FUNCTION") == 0) {
                instructions.push_back("\n.globl "+string(thirdparam));
                instructions.push_back("\n"+string(thirdparam)+":");
                instructions.push_back("\n\t\taddiu   $sp,$sp," + to_string(z));
                instructions.push_back("\n\t\tsw      $fp," + to_string(-z-4) + "($sp)");
                instructions.push_back("\n\t\tmove    $fp,$sp");
                instructions.push_back("\n\t\taddiu   $sp,$sp,-" + to_string(no_of_vars*4));
            }
            else if(strcmp(firstparam, "ENDFUNCTION") == 0) {
                instructions.push_back("\n"+string(secondparam)+":");
                instructions.push_back("\n\t\tmove    $sp,$fp");
                instructions.push_back("\n\t\tlw      $fp," + to_string(-z-4) + "($sp)");
                instructions.push_back("\n\t\taddiu   $sp,$sp," + to_string(-z));
                instructions.push_back("\n\t\tjr      $ra");
                instructions.push_back("\n\t\tnop");
            }
            else if(strcmp(firstparam, "LABEL") == 0) {
                instructions.push_back("\n"+string(secondparam)+":");
            }
            else if(strcmp(firstparam, "PARAM") == 0) {
                if (no_of_params < 4) { //only supports up to 4 parameters
                    parameters_loc[thirdparam].first = string(secondparam);
                    if (string(secondparam)=="float" && no_of_params < 2){
                        parameters_loc[thirdparam].second = "$f"+to_string(12+2*no_of_params);
                    } else if (string(secondparam)=="double" && no_of_params < 2){
                        parameters_loc[thirdparam].second = "$f"+to_string(12+2*no_of_params);
                    } else {
                        parameters_loc[thirdparam].second = "$a"+to_string(no_of_params); //stores the register the parameter is in
                    }
                }
                no_of_params += 1;
            }
            else if(strcmp(firstparam, "CALL") == 0) {
                no_of_args = 0;
                instructions.push_back("\n\t\tsw      $ra,8($fp)");
                instructions.push_back("\n\t\tjal     "+string(thirdparam));
                instructions.push_back("\n\t\tnop");
                instructions.push_back("\n\t\tlw      $ra,8($fp)"); //keep a store of the previous return address
                while(sp_move < 0) { //returned to original place, restore the arguments and $sp
                    instructions.push_back("\n\t\tlw       $a"+to_string(-1-sp_move/4)+",4($sp)");
                    instructions.push_back("\n\t\taddiu    $sp,$sp,"+to_string(4));
                    for ( auto &p : variable_mem_loc ) p.second.second -= 4;
                    sp_move += 4;
                }
                variable_it = variable_mem_loc.find(string(secondparam)); //store the return value
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $v0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$v0   #parameter "+secondparam);
                }
            }
            else if(strcmp(firstparam, "ARGUMENT") == 0) {
                if (no_of_args < 5) { //only supports up to 5 arguments
                    if (no_of_params >= no_of_args) { //store all of the current parameters on the stack frame
                        instructions.push_back("\n\t\tsw      $a"+to_string(no_of_args)+","+"0($sp)");
                        instructions.push_back("\n\t\taddiu   $sp,$sp,-4");
                        sp_move -= 4;
                        for ( auto &p : variable_mem_loc ) p.second.second += 4;
                    }
                    if (regex_match(secondparam, reInt)) { //put the new arguments into $a regs
                        instructions.push_back("\n\t\tli       $a"+to_string(no_of_args)+","+string(secondparam));
                    } else {
                        variable_it = variable_mem_loc.find(string(secondparam));
                        if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                            instructions.push_back("\n\t\tlw      $a"+to_string(no_of_args)+","+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                        } else { //if its a parameter
                            instructions.push_back("\n\t\tmove    $a"+to_string(no_of_args)+","+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        }
                    }
                }
                no_of_args += 1;
            }
            else if(strcmp(firstparam,"STRUCT_VAR") == 0) {
                struct_attr.push_back(string(secondparam));
                struct_attr.push_back(string(thirdparam));
                struct_attr.push_back(string(fourthparam));
                /*for (vector<string>::const_iterator i = struct_attr.begin(); i != struct_attr.end(); ++i)
                    cout << *i << ' ' << endl;*/
            }
            else if(strcmp(firstparam,"STRUCT_DEC") == 0) {
                for (unsigned i=0;i<struct_attr.size();i+=3){
                    if (struct_attr[i]==string(secondparam)) {
                        string attr_name = string(thirdparam)+"."+struct_attr[i+2];
                        //cout << attr_name << endl;
                        variable_mem_loc[attr_name].first = struct_attr[i+1];
                        variable_mem_loc[attr_name].second = curr_sp;
                        instructions.push_back("\n\t\taddiu    $sp,$sp,-"+to_string(4));
                        for ( auto &p : variable_mem_loc ) p.second.second += 4;
                        struct_map[string(thirdparam)] = struct_attr[i];
                    }
                }
            }
            else if(strcmp(firstparam,"ENUM_VAR") == 0) {
                if (fourthparam == NULL) {
                    enum_values[string(thirdparam)] = curr_enum_val;
                    curr_enum_val += 1;
                } else {
                    enum_values[string(thirdparam)] = atoi(fourthparam);
                    curr_enum_val = atoi(fourthparam) + 1;
                }
                //for ( const auto &p : enum_values ) cout << p.first << ' ' << p.second << '\n';
            }
            else if(strcmp(firstparam,"ADDRESS_OF") == 0) { //store address of variable into pointer variable
                instructions.push_back("\n\t\taddiu    $t0,$sp,"+to_string(variable_mem_loc[thirdparam].second)+"   #address of "+string(thirdparam));
                instructions.push_back("\n\t\tsw       $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
            }
            else if(strcmp(firstparam,"POINTER_ASSIGN") == 0) { //assign to value pointed to
                instructions.push_back("\n\t\tlw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)    #variable "+secondparam);
                if (regex_match(thirdparam,reInt)) {
                    instructions.push_back("\n\t\tli     $t1,"+string(thirdparam));
                } else {
                    instructions.push_back("\n\t\tlw     $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                }
                instructions.push_back("\n\t\tsw     $t1,0($t0)");
            }
            else if(strcmp(firstparam,"POINTER_CALL") == 0) { //get pointed value
                variable_it = variable_mem_loc.find(string(thirdparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tlw     $t0,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)    #variable "+thirdparam);
                } else {
                    instructions.push_back("\n\t\tmove   $t0,"+parameters_loc[thirdparam].second+"    #parameter "+thirdparam);
                }
                instructions.push_back("\n\t\tlw     $t0,0($t0)");
                instructions.push_back("\n\t\tsw     $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
            }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            else if(strcmp(firstparam, "ADD") == 0) {
                if (regex_match( thirdparam, reInt )) { //first operand is a number

                    if(regex_match( fourthparam, reInt )) { //both operands are a number
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\taddiu   $t0,$t1,"+string(fourthparam));
                    } else { //second operand is a variable
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){ //checks if its a vairable
                            instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            if (variable_mem_loc[fourthparam].first=="intpointer" || variable_mem_loc[fourthparam].first=="floatpointer"){ //check if its a pointer to check if pointer arithmetic needs to be done
                                instructions.push_back("\n\t\tli     $t2,"+string(thirdparam));
                                instructions.push_back("\n\t\tsll    $t2,2");
                                instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                            } else if (variable_mem_loc[fourthparam].first=="doublepointer") {
                                instructions.push_back("\n\t\tli     $t2,"+string(thirdparam));
                                instructions.push_back("\n\t\tsll    $t2,3");
                                instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\taddiu   $t0,$t1,"+string(thirdparam));
                            }
                        } else { //otherwise check if it a parameter
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                if (parameters_loc[fourthparam].first=="intpointer" || parameters_loc[fourthparam].first=="floatpointer"){ //check if parameter is a pointer
                                    instructions.push_back("\n\t\tli     $t2,"+string(thirdparam));
                                    instructions.push_back("\n\t\tsll    $t2,2");
                                    instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                                } else if (parameters_loc[fourthparam].first=="doublepointer"){ //check if parameter is a pointer
                                    instructions.push_back("\n\t\tli     $t2,"+string(thirdparam));
                                    instructions.push_back("\n\t\tsll    $t2,3");
                                    instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                                } else {
                                    instructions.push_back("\n\t\taddiu   $t0,$t1,"+string(thirdparam));
                                }
                            } else { //otherwise it is an enum value
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\taddiu   $t0,$t1,"+to_string(enum_values[string(fourthparam)]));
                            }
                        }
                    }
                } else { //first operand is a variable
                    bool ispointer = false; //stores if first operand is a pointer for pointer arithmetic
                    bool isdoublepointer = false;
                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        if(variable_mem_loc[thirdparam].first=="float"){
                            variable_mem_loc[secondparam].first="float";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }else if(variable_mem_loc[thirdparam].first=="double"){
                            variable_mem_loc[secondparam].first="double";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                            instructions.push_back("\n\t\tlwc1    $f3,"+to_string(variable_mem_loc[thirdparam].second-4)+"($sp)   #variable "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }
                        if (variable_mem_loc[thirdparam].first=="intpointer" || variable_mem_loc[thirdparam].first=="floatpointer"){ //if its a pointer
                            ispointer = true;
                        } else if (variable_mem_loc[thirdparam].first=="doublepointer") {
                            isdoublepointer = true;
                        }
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){ //if its a pointer
                            if (parameters_loc[thirdparam].first=="float") {
                                variable_mem_loc[secondparam].first="float";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.s   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            } else if (parameters_loc[thirdparam].first=="double") {
                                variable_mem_loc[secondparam].first="double";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.d   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            } else {
                                instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                            }
                            if (parameters_loc[thirdparam].first=="intpointer"||parameters_loc[thirdparam].first=="floatpointer"){
                                ispointer = true;
                            } else if (parameters_loc[thirdparam].first=="doublepointer"){
                                isdoublepointer = true;
                            }
                        } else { //otherwise it is an enum value
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) { //if second operand is a number
                        if (ispointer) { //if a pointer needs to multiplied by 4 hence sll by 2
                            instructions.push_back("\n\t\tli     $t2,"+string(fourthparam));
                            instructions.push_back("\n\t\tsll    $t2,2");
                            instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                        }else if (isdoublepointer) { //if a pointer needs to multiplied by 4 hence sll by 2
                            instructions.push_back("\n\t\tli     $t2,"+string(fourthparam));
                            instructions.push_back("\n\t\tsll    $t2,3");
                            instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                        } else{
                            instructions.push_back("\n\t\taddiu   $t0,$t1,"+string(fourthparam));
                        }
                    } else { //if second operand is also a variable
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){ //check if its a variable
                            if (variable_mem_loc[fourthparam].first=="float") {
                                instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                                instructions.push_back("\n\t\tadd.s   $f0,$f2,$f4");
                            }else if (variable_mem_loc[fourthparam].first=="double") {
                                instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                                instructions.push_back("\n\t\tlwc1    $f5,"+to_string(variable_mem_loc[fourthparam].second-4)+"($sp)   #variable "+fourthparam);
                                instructions.push_back("\n\t\tadd.d   $f0,$f2,$f4");
                            } else {
                                instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                                if (ispointer) { //if ispointer then need to multiply the value by 4
                                    instructions.push_back("\n\t\tsll    $t2,2");
                                } else if (isdoublepointer) { //if ispointer then need to multiply the value by 4
                                    instructions.push_back("\n\t\tsll    $t2,3");
                                } else {
                                    if (variable_mem_loc[fourthparam].first=="intpointer" || variable_mem_loc[fourthparam].first=="floatpointer"){
                                        instructions.push_back("\n\t\tsll    $t1,2");
                                    } else if (variable_mem_loc[fourthparam].first=="doublepointer"){
                                        instructions.push_back("\n\t\tsll    $t1,3");
                                    }
                                }
                                instructions.push_back("\n\t\taddu    $t0,$t1,$t2");
                            }
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                if (parameters_loc[fourthparam].first=="float") {
                                    if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                        instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    } else {
                                        instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                    }
                                    instructions.push_back("\n\t\tadd.s   $f0,$f2,$f4");
                                } else if (parameters_loc[fourthparam].first=="double") {
                                    if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                        instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    } else {
                                        instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                    }
                                    instructions.push_back("\n\t\tadd.d   $f0,$f2,$f4");
                                } else {
                                    instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    if (ispointer) { //if ispointer then need to multiply the value by 4
                                        instructions.push_back("\n\t\tsll    $t2,2");
                                    } else if (isdoublepointer) { //if ispointer then need to multiply the value by 4
                                        instructions.push_back("\n\t\tsll    $t2,3");
                                    } else {
                                        if (parameters_loc[fourthparam].first=="intpointer"||parameters_loc[fourthparam].first=="floatpointer"){
                                            instructions.push_back("\n\t\tsll    $t1,2");
                                        } else if (parameters_loc[fourthparam].first=="doublepointer"){
                                            instructions.push_back("\n\t\tsll    $t1,3");
                                        }
                                    }
                                    instructions.push_back("\n\t\taddu    $t0,$t1,$t2");
                                }
                            } else {
                                if (ispointer) {
                                    instructions.push_back("\n\t\tli     $t2,"+to_string(enum_values[fourthparam]));
                                    instructions.push_back("\n\t\tsll    $t2,2");
                                    instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                                }else if (isdoublepointer) {
                                    instructions.push_back("\n\t\tli     $t2,"+to_string(enum_values[fourthparam]));
                                    instructions.push_back("\n\t\tsll    $t2,3");
                                    instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                                } else {
                                    instructions.push_back("\n\t\taddiu    $t0,$t1,"+to_string(enum_values[fourthparam]));
                                }
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){//check if variable so it can be stored back into its place in memory
                    if (variable_mem_loc[secondparam].first=="float") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    } else if (variable_mem_loc[secondparam].first=="double") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                        instructions.push_back("\n\t\tswc1    $f1,"+to_string(variable_mem_loc[secondparam].second-4)+"($sp)   #variable "+secondparam);
                    } else {
                        instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    }
                } else {//check if its a parameter so it can be stored in its $a reg
                    if (parameters_loc[secondparam].first=="float") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    }else if (parameters_loc[secondparam].first=="double") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    } else {
                        instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                    }
                }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "MUL") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                    } else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                            }
                        }
                    }
                } else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        if(variable_mem_loc[thirdparam].first=="float"){
                            variable_mem_loc[secondparam].first="float";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }else if(variable_mem_loc[thirdparam].first=="double"){
                            variable_mem_loc[secondparam].first="double";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                            instructions.push_back("\n\t\tlwc1    $f3,"+to_string(variable_mem_loc[thirdparam].second-4)+"($sp)   #variable "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            if (parameters_loc[thirdparam].first=="float") {
                                variable_mem_loc[secondparam].first="float";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.s   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            }else if (parameters_loc[thirdparam].first=="double") {
                                variable_mem_loc[secondparam].first="double";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.d   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            } else {
                                instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                            }
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                    } else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                          if (variable_mem_loc[thirdparam].first=="float") {
                              instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                              instructions.push_back("\n\t\tmul.s   $f0,$f2,$f4");
                          }else if (variable_mem_loc[fourthparam].first=="double") {
                              instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                              instructions.push_back("\n\t\tlwc1    $f5,"+to_string(variable_mem_loc[fourthparam].second-4)+"($sp)   #variable "+fourthparam);
                              instructions.push_back("\n\t\tmul.d   $f0,$f2,$f4");
                          } else {
                              instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                              instructions.push_back("\n\t\tnop");
                              instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                          }
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                if (parameters_loc[thirdparam].first=="float") {
                                    if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                        instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    } else {
                                        instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                    }
                                    instructions.push_back("\n\t\tmul.s   $f0,$f2,$f4");
                                }else if (parameters_loc[fourthparam].first=="double") {
                                    if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                        instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    } else {
                                        instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                    }
                                    instructions.push_back("\n\t\tmul.d   $f0,$f2,$f4");
                                } else {
                                    instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                                }
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tmul     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){//check if variable so it can be stored back into its place in memory
                    if (variable_mem_loc[secondparam].first=="float") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    }else if (variable_mem_loc[secondparam].first=="double") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                        instructions.push_back("\n\t\tswc1    $f1,"+to_string(variable_mem_loc[secondparam].second-4)+"($sp)   #variable "+secondparam);
                    } else {
                        instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    }
                } else {//check if its a parameter so it can be stored in its $a reg
                    if (parameters_loc[secondparam].first=="float") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    }else if (parameters_loc[secondparam].first=="double") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    } else {
                        instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                    }
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "NEG") == 0) {
                if (regex_match( thirdparam, reInt )) {
                    instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                    instructions.push_back("\n\t\tneg     $t0,$t1");
                } else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        if(variable_mem_loc[thirdparam].first=="float"){
                            variable_mem_loc[secondparam].first="float";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }else if(variable_mem_loc[thirdparam].first=="double"){
                            variable_mem_loc[secondparam].first="double";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                            instructions.push_back("\n\t\tlwc1    $f3,"+to_string(variable_mem_loc[thirdparam].second-4)+"($sp)   #variable "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            if (parameters_loc[thirdparam].first=="float") {
                                variable_mem_loc[secondparam].first="float";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.s   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            }else if (parameters_loc[thirdparam].first=="double") {
                                variable_mem_loc[secondparam].first="double";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.d   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            } else {
                                instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                            }
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if (parameters_loc[thirdparam].first=="float") {
                        instructions.push_back("\n\t\tneg.s   $f0,$f2");
                    }else if (parameters_loc[thirdparam].first=="double") {
                        instructions.push_back("\n\t\tneg.d   $f0,$f2");
                    } else {
                        instructions.push_back("\n\t\tneg     $t0,$t1");
                    }


                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){//check if variable so it can be stored back into its place in memory
                    if (variable_mem_loc[secondparam].first=="float") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    }else if (variable_mem_loc[secondparam].first=="double") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                        instructions.push_back("\n\t\tswc1    $f1,"+to_string(variable_mem_loc[secondparam].second-4)+"($sp)   #variable "+secondparam);
                    } else {
                        instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    }
                } else {//check if its a parameter so it can be stored in its $a reg
                    if (parameters_loc[secondparam].first=="float") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    }else if (parameters_loc[secondparam].first=="double") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    } else {
                        instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                    }
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "SUB") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\taddiu   $t0,$t1,-"+string(fourthparam));
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tsubu    $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tsubu    $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                                instructions.push_back("\n\t\taddiu   $t0,$t1,-"+string(fourthparam));
                            }
                        }
                    }
                } else {
                    bool ispointer = false;
                    bool isdoublepointer = false;
                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        if(variable_mem_loc[thirdparam].first=="float"){
                            variable_mem_loc[secondparam].first="float";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }else if(variable_mem_loc[thirdparam].first=="double"){
                            variable_mem_loc[secondparam].first="double";
                            instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                            instructions.push_back("\n\t\tlwc1    $f3,"+to_string(variable_mem_loc[thirdparam].second-4)+"($sp)   #variable "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                        }
                        if (variable_mem_loc[thirdparam].first=="intpointer"||variable_mem_loc[thirdparam].first=="floatpointer"){
                            ispointer = true;
                        } else if (variable_mem_loc[thirdparam].first=="doublepointer"){
                            isdoublepointer = true;
                        }
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            if (parameters_loc[thirdparam].first=="float") {
                                variable_mem_loc[secondparam].first="float";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.s   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            }else if (parameters_loc[thirdparam].first=="double") {
                                variable_mem_loc[secondparam].first="double";
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.d   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                                }
                            } else {
                                instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                            }
                            if (parameters_loc[thirdparam].first=="intpointer"||parameters_loc[thirdparam].first=="floatpointer"){
                                ispointer = true;
                            }else if (parameters_loc[thirdparam].first=="doublepointer"){
                                isdoublepointer = true;
                            }
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        if (ispointer) {
                            instructions.push_back("\n\t\tli     $t2,"+string(fourthparam));
                            instructions.push_back("\n\t\tsll    $t2,2");
                            instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                        }else if (isdoublepointer) {
                            instructions.push_back("\n\t\tli     $t2,"+string(fourthparam));
                            instructions.push_back("\n\t\tsll    $t2,3");
                            instructions.push_back("\n\t\taddu   $t0,$t1,$t2");
                        } else{
                            instructions.push_back("\n\t\taddiu   $t0,$t1,-"+string(fourthparam));
                        }
                    } else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            if (variable_mem_loc[thirdparam].first=="float") {
                                instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                                instructions.push_back("\n\t\tsub.s   $f0,$f2,$f4");
                            }else if (variable_mem_loc[fourthparam].first=="double") {
                                instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                                instructions.push_back("\n\t\tlwc1    $f5,"+to_string(variable_mem_loc[fourthparam].second-4)+"($sp)   #variable "+fourthparam);
                                instructions.push_back("\n\t\tsub.d   $f0,$f2,$f4");
                            } else {
                                instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            }
                            if (ispointer) {
                                instructions.push_back("\n\t\tsll    $t2,2");
                            }else if (isdoublepointer) {
                                instructions.push_back("\n\t\tsll    $t2,3");
                            }
                            instructions.push_back("\n\t\tsubu    $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                if (parameters_loc[thirdparam].first=="float") {
                                    if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                        instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    } else {
                                        instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                    }
                                    instructions.push_back("\n\t\tsub.s   $f0,$f2,$f4");
                                }else if (parameters_loc[fourthparam].first=="double") {
                                    if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                        instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    } else {
                                        instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                    }
                                    instructions.push_back("\n\t\tsub.d   $f0,$f2,$f4");
                                } else {
                                    instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                    instructions.push_back("\n\t\tsubu    $t0,$t1,$t2");
                                }
                                if (ispointer) {
                                    instructions.push_back("\n\t\tsll    $t2,2");
                                    instructions.push_back("\n\t\tsubu    $t0,$t1,$t2");
                                } else if (isdoublepointer) {
                                    instructions.push_back("\n\t\tsll    $t2,3");
                                    instructions.push_back("\n\t\tsubu    $t0,$t1,$t2");
                                }

                            } else {
                                if (ispointer) {
                                    instructions.push_back("\n\t\tli     $t2,"+to_string(enum_values[fourthparam]));
                                    instructions.push_back("\n\t\tsll    $t2,2");
                                    instructions.push_back("\n\t\tsubu   $t0,$t1,$t2");
                                }else if (isdoublepointer) {
                                    instructions.push_back("\n\t\tli     $t2,"+to_string(enum_values[fourthparam]));
                                    instructions.push_back("\n\t\tsll    $t2,3");
                                    instructions.push_back("\n\t\tsubu   $t0,$t1,$t2");
                                } else {
                                    instructions.push_back("\n\t\taddiu    $t0,$t1,-"+to_string(enum_values[fourthparam]));
                                }
                            }
                        }
                    }


                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){//check if variable so it can be stored back into its place in memory
                    if (variable_mem_loc[secondparam].first=="float") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    }else if (variable_mem_loc[secondparam].first=="double") {
                        instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                        instructions.push_back("\n\t\tswc1    $f1,"+to_string(variable_mem_loc[secondparam].second-4)+"($sp)   #variable "+secondparam);
                    } else {
                        instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                    }
                } else {//check if its a parameter so it can be stored in its $a reg
                    if (parameters_loc[secondparam].first=="float") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    }else if (parameters_loc[secondparam].first=="double") {
                        if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                            instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                        }
                    } else {
                        instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                    }
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "DIV") == 0) {
              if (regex_match( thirdparam, reInt )) {

                  if(regex_match( fourthparam, reInt )) {
                      instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                      instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                      instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                  } else {
                      variable_it = variable_mem_loc.find(string(fourthparam));
                      if (variable_it != variable_mem_loc.end()){
                          instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                          instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                          instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                      } else {
                          params_it = parameters_loc.find(string(fourthparam));
                          if (params_it != parameters_loc.end()){
                              instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                              instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                              instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                          } else {
                              instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                              instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                              instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                          }
                      }
                  }
              } else {

                  variable_it = variable_mem_loc.find(string(thirdparam));
                  if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                      if(variable_mem_loc[thirdparam].first=="float"){
                          variable_mem_loc[secondparam].first="float";
                          instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                      }else if(variable_mem_loc[thirdparam].first=="double"){
                          variable_mem_loc[secondparam].first="double";
                          instructions.push_back("\n\t\tlwc1    $f2,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                          instructions.push_back("\n\t\tlwc1    $f3,"+to_string(variable_mem_loc[thirdparam].second-4)+"($sp)   #variable "+thirdparam);
                      } else {
                          instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                      }
                  } else { //if its a parameter
                      params_it = parameters_loc.find(string(thirdparam));
                      if (params_it != parameters_loc.end()){
                          if (parameters_loc[thirdparam].first=="float") {
                              variable_mem_loc[secondparam].first="float";
                              if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                  instructions.push_back("\n\t\tmov.s   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                              } else {
                                  instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                              }
                          }else if (parameters_loc[thirdparam].first=="double") {
                              variable_mem_loc[secondparam].first="double";
                              if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                  instructions.push_back("\n\t\tmov.d   $f2,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                              } else {
                                  instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f2   #parameter "+thirdparam);
                              }
                          } else {
                              instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                          }
                      } else {
                          instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                      }
                  }

                  if(regex_match( fourthparam, reInt )) {
                      instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                      instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                  } else {
                      variable_it = variable_mem_loc.find(string(fourthparam));
                      if (variable_it != variable_mem_loc.end()){
                        if (variable_mem_loc[thirdparam].first=="float") {
                            instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tdiv.s   $f0,$f2,$f4");
                        }else if (variable_mem_loc[fourthparam].first=="double") {
                            instructions.push_back("\n\t\tlwc1    $f4,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tlwc1    $f5,"+to_string(variable_mem_loc[fourthparam].second-4)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tdiv.d   $f0,$f2,$f4");
                        } else {
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                        }
                      } else {
                          params_it = parameters_loc.find(string(fourthparam));
                          if (params_it != parameters_loc.end()){
                              if (parameters_loc[thirdparam].first=="float") {
                                  if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                      instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                  } else {
                                      instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                  }
                                  instructions.push_back("\n\t\tdiv.s   $f0,$f2,$f4");
                              }else if (parameters_loc[fourthparam].first=="double") {
                                  if (parameters_loc[fourthparam].second=="$f12"||parameters_loc[fourthparam].second=="$f14"){
                                      instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                  } else {
                                      instructions.push_back("\n\t\tmtc1    "+parameters_loc[fourthparam].second+",$f4   #parameter "+fourthparam);
                                  }
                                  instructions.push_back("\n\t\tdiv.d   $f0,$f2,$f4");
                              } else {
                                  instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                  instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                              }
                          } else {
                              instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                              instructions.push_back("\n\t\tdiv     $t0,$t1,$t2");
                          }
                      }
                  }

              }

              variable_it = variable_mem_loc.find(string(secondparam));
              if (variable_it != variable_mem_loc.end()){//check if variable so it can be stored back into its place in memory
                  if (variable_mem_loc[secondparam].first=="float") {
                      instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                  }else if (variable_mem_loc[secondparam].first=="double") {
                      instructions.push_back("\n\t\tswc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                      instructions.push_back("\n\t\tswc1    $f1,"+to_string(variable_mem_loc[secondparam].second-4)+"($sp)   #variable "+secondparam);
                  } else {
                      instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                  }
              } else {//check if its a parameter so it can be stored in its $a reg
                  if (parameters_loc[secondparam].first=="float") {
                      if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                          instructions.push_back("\n\t\tmov.s   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                      } else {
                          instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                      }
                  }else if (parameters_loc[secondparam].first=="double") {
                      if (parameters_loc[secondparam].second=="$f12"||parameters_loc[secondparam].second=="$f14"){
                          instructions.push_back("\n\t\tmov.d   $f4,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                      } else {
                          instructions.push_back("\n\t\tmtc1    "+parameters_loc[secondparam].second+",$f4   #parameter "+secondparam);
                      }
                  } else {
                      instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                  }
              }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "MODULO") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                    } else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                            }
                        }
                    }
                } else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                    } else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\trem     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "AND") == 0) {
                string not_label = makeName("not_label");
                string end_label = makeName("end_label");
                variable_it = variable_mem_loc.find(string(thirdparam));
                if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                    instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                } else { //if its a parameter
                    params_it = parameters_loc.find(string(thirdparam));
                    if (params_it != parameters_loc.end()){
                        instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                    } else {
                        instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                    }
                }
                instructions.push_back("\n\t\tbeq       $t1,$zero,"+not_label);
                instructions.push_back("\n\t\tnop");

                variable_it = variable_mem_loc.find(string(fourthparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                } else {
                    params_it = parameters_loc.find(string(fourthparam));
                    if (params_it != parameters_loc.end()){
                        instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                    } else {
                        instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                    }
                }
                instructions.push_back("\n\t\tbeq       $t2,$zero,"+not_label);
                instructions.push_back("\n\t\tnop");

                instructions.push_back("\n\t\tli        $t0,1");
                instructions.push_back("\n\t\tj         "+end_label);
                instructions.push_back("\n\t\tnop");
                instructions.push_back("\n"+not_label+":");
                instructions.push_back("\n\t\tli        $t0,0");
                instructions.push_back("\n"+end_label+":");

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "OR") == 0) {
                string corr_label = makeName("corr_label");
                string end_label = makeName("end_label");
                variable_it = variable_mem_loc.find(string(thirdparam));
                if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                    instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                } else { //if its a parameter
                    params_it = parameters_loc.find(string(thirdparam));
                    if (params_it != parameters_loc.end()){
                        instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                    } else {
                        instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                    }
                }
                instructions.push_back("\n\t\tbne       $t1,$zero,"+corr_label);
                instructions.push_back("\n\t\tnop");

                variable_it = variable_mem_loc.find(string(fourthparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                } else {
                    params_it = parameters_loc.find(string(fourthparam));
                    if (params_it != parameters_loc.end()){
                        instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                    } else {
                        instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                    }
                }
                instructions.push_back("\n\t\tbne       $t2,$zero,"+corr_label);
                instructions.push_back("\n\t\tnop");

                instructions.push_back("\n\t\tli        $t0,0");
                instructions.push_back("\n\t\tj         "+end_label);
                instructions.push_back("\n\t\tnop");
                instructions.push_back("\n"+corr_label+":");
                instructions.push_back("\n\t\tli        $t0,1");
                instructions.push_back("\n"+end_label+":");

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "NOT") == 0) {
                string corr_label = makeName("corr_label");
                string end_label = makeName("end_label");
                variable_it = variable_mem_loc.find(string(thirdparam));
                if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                    instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                } else { //if its a parameter
                    params_it = parameters_loc.find(string(thirdparam));
                    if (params_it != parameters_loc.end()){
                        instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                    } else {
                        instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                    }
                }
                instructions.push_back("\n\t\tbeq       $t1,$zero,"+corr_label);
                instructions.push_back("\n\t\tnop");

                instructions.push_back("\n\t\tli        $t0,0");
                instructions.push_back("\n\t\tj         "+end_label);
                instructions.push_back("\n\t\tnop");
                instructions.push_back("\n"+corr_label+":");
                instructions.push_back("\n\t\tli        $t0,1");
                instructions.push_back("\n"+end_label+":");

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "BITWISE_AND") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                    } else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                            }
                        }
                    }
                } else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tand     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "BITWISE_OR") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tor      $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "XOR") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\txor     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "BITWISE_ONECOMP") == 0) {
                if (regex_match( thirdparam, reInt )) {
                    instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                    instructions.push_back("\n\t\tnot     $t0,$t1");
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    instructions.push_back("\n\t\tnot     $t0,$t1");

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "GREATER_THAN") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsgt     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              } else if(strcmp(firstparam, "LESS_THAN") == 0) {
                  if (regex_match( thirdparam, reInt )) {

                      if(regex_match( fourthparam, reInt )) {
                          instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                          instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                          instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                      }else {
                          variable_it = variable_mem_loc.find(string(fourthparam));
                          if (variable_it != variable_mem_loc.end()){
                              instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                              instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                              instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                          } else {
                              params_it = parameters_loc.find(string(fourthparam));
                              if (params_it != parameters_loc.end()){
                                  instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                  instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                  instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                              } else {
                                  instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                  instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                  instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                              }
                          }
                      }
                  }else {

                      variable_it = variable_mem_loc.find(string(thirdparam));
                      if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                          instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                      } else { //if its a parameter
                          params_it = parameters_loc.find(string(thirdparam));
                          if (params_it != parameters_loc.end()){
                              instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                          } else {
                              instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                          }
                      }

                      if(regex_match( fourthparam, reInt )) {
                          instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                          instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                      }else {
                          variable_it = variable_mem_loc.find(string(fourthparam));
                          if (variable_it != variable_mem_loc.end()){
                              instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                              instructions.push_back("\n\t\tnop");
                              instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                          } else {
                              params_it = parameters_loc.find(string(fourthparam));
                              if (params_it != parameters_loc.end()){
                                  instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                  instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                              } else {
                                  instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                  instructions.push_back("\n\t\tslt     $t0,$t1,$t2");
                              }
                          }
                      }

                  }

                  variable_it = variable_mem_loc.find(string(secondparam));
                  if (variable_it != variable_mem_loc.end()){
                      instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                  } else {
                      instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "LESS_THANEQ") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsle     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "GREATER_THANEQ") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsge     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tsge    $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsge     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsge     $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsge     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tsge     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsge     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsge     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "EQUAL_COMP") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tseq     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "NOT_EQUALS") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsne     $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "LEFTSHIFT") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsllv    $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "RIGHTSHIFT") == 0) {
                if (regex_match( thirdparam, reInt )) {

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t1,"+string(thirdparam));
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                            }
                        }
                    }
                }else {

                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){ //check if first operand is a variable
                        instructions.push_back("\n\t\tlw      $t1,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else { //if its a parameter
                        params_it = parameters_loc.find(string(thirdparam));
                        if (params_it != parameters_loc.end()){
                            instructions.push_back("\n\t\tmove    $t1,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                        } else {
                            instructions.push_back("\n\t\tli      $t1,"+to_string(enum_values[string(thirdparam)]));
                        }
                    }

                    if(regex_match( fourthparam, reInt )) {
                        instructions.push_back("\n\t\tli      $t2,"+string(fourthparam));
                        instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                    }else {
                        variable_it = variable_mem_loc.find(string(fourthparam));
                        if (variable_it != variable_mem_loc.end()){
                            instructions.push_back("\n\t\tlw      $t2,"+to_string(variable_mem_loc[fourthparam].second)+"($sp)   #variable "+fourthparam);
                            instructions.push_back("\n\t\tnop");
                            instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                        } else {
                            params_it = parameters_loc.find(string(fourthparam));
                            if (params_it != parameters_loc.end()){
                                instructions.push_back("\n\t\tmove    $t2,"+parameters_loc[fourthparam].second+"   #parameter "+fourthparam);
                                instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                            } else {
                                instructions.push_back("\n\t\tli      $t2,"+to_string(enum_values[string(fourthparam)]));
                                instructions.push_back("\n\t\tsrav    $t0,$t1,$t2");
                            }
                        }
                    }

                }

                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            } else if(strcmp(firstparam, "VARIABLE") == 0) {
                variable_it = variable_mem_loc.find(string(thirdparam));
                if (variable_it == variable_mem_loc.end()){
                    //instructions.push_back("\n\t\taddiu   $sp,$sp,-4");
                    //for ( auto &p : variable_mem_loc ) p.second += 4;
                    variable_mem_loc[thirdparam].first = string(secondparam);
                    variable_mem_loc[thirdparam].second = curr_sp;
                    curr_sp -= 4;
                    if (string(secondparam)=="double"){
                        curr_sp -= 4;
                    }
                }
                //for ( const auto &p : variable_mem_loc ) std::cout << p.first << ' ' << p.second << '\n';

            }
            else if(strcmp(firstparam, "ASSIGN") == 0) {
                if (regex_match( thirdparam, reInt )){
                    instructions.push_back("\n\t\tli      $t0,"+string(thirdparam));
                } else {
                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()){
                        instructions.push_back("\n\t\tlw      $t0,"+to_string(variable_mem_loc[thirdparam].second)+"($sp)   #variable "+thirdparam);
                    } else {
                        instructions.push_back("\n\t\tmove    $t0,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                    }
                }
                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
            }
            else if(strcmp(firstparam, "RETURN") == 0) {
              if (regex_match( secondparam, reInt )){
                  instructions.push_back("\n\t\tli      $v0,"+string(secondparam));
              }else{
                    variable_it = variable_mem_loc.find(string(secondparam));
                    if (variable_it != variable_mem_loc.end()){
                        if (variable_mem_loc[secondparam].first=="float") {
                            instructions.push_back("\n\t\tlwc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                        }else if (variable_mem_loc[secondparam].first=="double") {
                            instructions.push_back("\n\t\tlwc1    $f0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                            instructions.push_back("\n\t\tlwc1    $f1,"+to_string(variable_mem_loc[secondparam].second-4)+"($sp)   #variable "+secondparam);
                        } else {
                            instructions.push_back("\n\t\tlw      $v0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                        }
                    } else {
                        params_it = parameters_loc.find(string(secondparam));
                        if (params_it != parameters_loc.end()){
                            if (parameters_loc[secondparam].first=="float"){
                                if (parameters_loc[thirdparam].second=="$f12"||parameters_loc[thirdparam].second=="$f14"){
                                    instructions.push_back("\n\t\tmov.s   $f0,"+parameters_loc[thirdparam].second+"   #parameter "+thirdparam);
                                } else {
                                    instructions.push_back("\n\t\tmtc1    "+parameters_loc[thirdparam].second+",$f0   #parameter "+thirdparam);
                                }
                            } else{
                                instructions.push_back("\n\t\tmove    $v0,"+parameters_loc[secondparam].second+"   #parameter "+secondparam);
                            }
                        } else {
                            instructions.push_back("\n\t\tli      $v0,"+to_string(enum_values[string(secondparam)]));
                        }
                    }
                }
            }
            else if(strcmp(firstparam, "GOTO") == 0) {
                //cout << secondparam << endl;
                instructions.push_back("\n\t\tj       "+string(secondparam));
                instructions.push_back("\n\t\tnop");
            }
            else if(strcmp(firstparam, "IFFALSE") == 0) {
                instructions.push_back("\n\t\tlw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                instructions.push_back("\n\t\tbeq     $t0,$zero,"+string(fourthparam));
                instructions.push_back("\n\t\tnop");
            }
            else if(strcmp(firstparam, "SIZEOF") == 0) {
                if (strcmp(thirdparam, "int") == 0||strcmp(thirdparam, "intpointer") == 0||strcmp(thirdparam, "floatpointer") == 0) {
                    instructions.push_back("\n\t\tli      $t0,4");
                } else if (strcmp(thirdparam, "char") == 0||strcmp(thirdparam, "charpointer") == 0) {
                    instructions.push_back("\n\t\tli      $t0,1");
                }else if (strcmp(thirdparam, "float") == 0) {
                    instructions.push_back("\n\t\tli      $t0,4");
                }else if (strcmp(thirdparam, "unsigned") == 0) {
                    instructions.push_back("\n\t\tli      $t0,4");
                }else if (strcmp(thirdparam, "signed") == 0) {
                    instructions.push_back("\n\t\tli      $t0,4");
                }else if (strcmp(thirdparam, "double") == 0||strcmp(thirdparam, "doublepointer") == 0) {
                    instructions.push_back("\n\t\tli      $t0,8");
                }else {
                    variable_it = variable_mem_loc.find(string(thirdparam));
                    if (variable_it != variable_mem_loc.end()) {
                        if (variable_mem_loc[thirdparam].first == "int"||variable_mem_loc[thirdparam].first == "intpointer"||variable_mem_loc[thirdparam].first == "floatpointer") {
                            instructions.push_back("\n\t\tli      $t0,4");
                        } else if (variable_mem_loc[thirdparam].first == "char"||variable_mem_loc[thirdparam].first == "charpointer") {
                            instructions.push_back("\n\t\tli      $t0,1");
                        } else if (variable_mem_loc[thirdparam].first == "float") {
                            instructions.push_back("\n\t\tli      $t0,4");
                        }else if (variable_mem_loc[thirdparam].first == "signed") {
                            instructions.push_back("\n\t\tli      $t0,4");
                        }else if (variable_mem_loc[thirdparam].first == "unsigned") {
                            instructions.push_back("\n\t\tli      $t0,4");
                        }else if (variable_mem_loc[thirdparam].first == "double"||variable_mem_loc[thirdparam].first == "doublepointer") {
                                instructions.push_back("\n\t\tli      $t0,8");
                        }else {
                            instructions.push_back("\n\t\tli      $t0,4");
                        }
                    } else {
                        int struct_size = 0;
                        for (unsigned i=0;i<struct_attr.size();i+=3) {
                            if (struct_attr[i]==struct_map[string(thirdparam)]){
                                if (struct_attr[i+1] == "int"||struct_attr[i+1] == "intpointer"||struct_attr[i+1] == "floatpointer") {
                                    struct_size += 4;
                                } else if (struct_attr[i+1] == "char"||struct_attr[i+1] == "charpointer") {
                                    struct_size += 1;
                                } else if (struct_attr[i+1] == "float") {
                                    struct_size += 4;
                                }else if (struct_attr[i+1] == "signed") {
                                    struct_size += 4;
                                }else if (struct_attr[i+1] == "unsigned") {
                                    struct_size += 4;
                                }else if (struct_attr[i+1] == "double"||struct_attr[i+1] == "doublepointer") {
                                    struct_size += 8;
                                }else {
                                    struct_size += 4;
                                }
                            }
                        }
                        instructions.push_back("\n\t\tli      $t0,"+to_string(struct_size));
                    }
                }
                variable_it = variable_mem_loc.find(string(secondparam));
                if (variable_it != variable_mem_loc.end()){
                    instructions.push_back("\n\t\tsw      $t0,"+to_string(variable_mem_loc[secondparam].second)+"($sp)   #variable "+secondparam);
                } else {
                    instructions.push_back("\n\t\tmove    "+parameters_loc[secondparam].second+",$t0   #parameter "+secondparam);
                }
            }
          }
        }
    }
    else {
        cout << "Sorry! Cannot resolve that input file. Try checking to see if the file is named appropriately, it should be named 'in.txt'" << endl;
    }
    //outfile.close();
    //ofstream outfile("out.txt"); //Destination file
    for (const auto &e : instructions) outfile << e; //Cast the elements of the array instructions onto the output file
    //This just outputs line by line easily into the output file.
    outfile << "\n";
}


#endif
