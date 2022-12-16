#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

void die(int line_no = 0) {
	cout << "Syntax Error on line " << line_no << endl;
	exit(1);
}

string register_lookup(string reg, int line_no) {
	//0-3 and 14-15 are off limits
	if (reg == "A") return "R4";
	else if (reg == "B") return "R5";
	else if (reg == "C") return "R6";
	else if (reg == "I") return "R7";
	else if (reg == "J") return "R8";
	else if (reg == "X") return "R9";
	else if (reg == "Y") return "R10";
	else if (reg == "Z") return "R11";
	else die(line_no);
	return reg;
}


int main(int argc, char **argv) {
	//If we pass any parameters, we'll just generate an assembly file 
	//Otherwise we will generate an assembly file, assemble it, and run it
	bool assemble_only = false;
	if (argc > 1) assemble_only = true;
	ofstream outs("main.s"); //This is the assembly file we're creating
	if (!outs) {
		cout << "Error opening file.\n";
		return -1;
	}
	outs << ".global main\nmain:\n"; //Get the file ready to rock
	outs << "\tPUSH {LR}\n\tPUSH {R4-R12}\n\n";

	//Initialize all variables to 0
	for (int i = 0; i < 13; i++)
		outs << "\tMOV R" << i << ",#0\n";
	
	vector<string> vec;
	
	int line_no = 0;
	while (cin) {
		string s;
		getline(cin,s);
		line_no++;
		if (!cin) break;
		transform(s.begin(), s.end(), s.begin(), ::toupper); //Uppercaseify
		auto it = s.find("QUIT"); //TERMINATE COMPILER
		if (it != string::npos) break;
		stringstream ss(s); //Turn s into a stringstream
		int label;
		ss >> label;
		if (!ss) die(line_no);
		outs << "line_" << label << ":\n"; //Write each line number to the file ("line_20:")
		string command;
		ss >> command;
		if (!ss) die(line_no);

		if (command == "REM") {
			continue;
		}
		else if (command == "GOTO") {
			int target;
			ss >> target;
			if (!ss) die(line_no);
			outs << "\tBAL line_" << target << endl;
			continue;
		}
		else if (command == "EXIT") {
			outs << "\tBAL quit\n";
			continue;
		}

		//YOU: Put all of your code here, interpreting the different commands in BB8 <-----------------

		else if (command == "PRINT") {
			string var;
			string var2;
			ss >> var; 
			var2 = var; 
			if (var[0] == '\"') { 
				while (true) {
					if (var2.back() == '\"') {
						vec.push_back(var2);
						outs << "LDR R0,=string_" << vec.size() << endl;
						outs << "BL print_string" << endl;
						break;
					}
					else {
						if (!ss) die(line_no);
						ss >> var;
						var2 = var2 + " " + var;
					}
				}
			}
			else { 
				string reg = register_lookup(var, line_no);
				outs << "MOV R0, " << reg << endl;
				outs << "BL print_number" << endl;
			}
		}
		else if (command == "LET") {
			string var, equal, var2;
			int num= 0;
			bool isstring = false;
			ss >> var >> equal; 
			if (equal != "=") die(line_no);
			ss >> var2; 
			try {
				num = stoi(var2);
			}
			catch(...) {
				isstring = true;
			}

			if (isstring) { 
				string op, var3;
				ss >> op >> var3; 
				var = register_lookup(var, line_no);
				var2 = register_lookup(var2, line_no);
				var3 = register_lookup(var3, line_no);
				if (op == "+") {
					outs << "ADD " << var << ", " << var2 << ", " << var3 << endl;
				}
				else if (op == "-") {
					outs << "SUB " << var << ", " << var2 << ", " << var3 << endl;
				}
				else if (op == "*") {
					outs << "MUL " << var << ", " << var2 << ", " << var3 << endl;
				}
				else die(line_no);
			}
			else {
				outs << "MOV " << register_lookup(var, line_no) << ", #" << num << endl;
			}			
		}
		else if (command == "IF") {
			string var1,oper,var2,then,mr_goto,line,ms_else;
			int line1 = -1, line2 = -1; 
			ss >> var1 >> oper >> var2 >> then >> mr_goto >> line;
			if (!ss) die();
			if (then != "THEN") die(line_no);
			if (mr_goto != "GOTO") die(line_no);
			try { line1 = stoi(line); } catch (...) { die(line_no); } 
			bool has_else = true; 
			ss >> ms_else >> mr_goto >> line;
			try { line2 = stoi(line); } catch (...) { die(line_no); }
			if (!ss) has_else = false;
			var1 = register_lookup(var1, line_no); 
			var2 = register_lookup(var2, line_no); 
			outs << "\tCMP " << var1 << "," << var2 << endl;
			if (oper == "<")	outs << "\tBLT line_" << line1 << endl;
			else if (oper == "<=")    outs << "\tBLE line_" << line1 << endl;
			else if (oper == ">")     outs << "\tBGT line_" << line1 << endl;
			else if (oper == ">=")    outs << "\tBGE line_" << line1 << endl;
			else if (oper == "==")    outs << "\tBEQ line_" << line1 << endl;
			else if (oper == "!=")    outs << "\tBNE line_" << line1 << endl;
			else die(); 
			if (has_else) outs << "\tBAL line_" << line2 << endl;


		}
		else if (command == "INPUT") {
			string var; 
			ss >> var;
			outs << "BL read_number" << endl;
			outs << "MOV " << register_lookup(var, line_no) << ", R0" << endl;
		}
		else if (command == "END") {
		outs << "\tBAL quit\n";
		continue;
		}
		else {
		
			die(line_no);
		}
	}

	//Clean up the file at the bottom
	outs << "\nquit:\n\tMOV R0, #42\n\tPOP {R4-R12}\n\tPOP {PC}\n"; //Finish the code and return

	//Put the string at the bottom of the file
	if (vec.size() != 0) {
		outs << "\n.data\n"; 
		for (int i = 0; i <= vec.size() - 1; i++){
			outs << "string_" << i+1 << ": .asciz " << vec[i] << endl;
		}
	}

	outs.close(); //Close the file

	if (assemble_only) return 0; //When you're debugging you should run bb8 with a parameter

	//print.o is made from the Makefile, so make sure you make your code
	if (system("g++ main.s print.o")) { //Compile your assembler code and check for errors
		cout << "Assembling failed, which means your compiler screwed up.\n";
		return 1;
	}
	//We've got an a.out now, so let's run it!
	cout << "Compilation successful. Executing program now." << endl;
	execve("a.out",NULL,NULL);
}

