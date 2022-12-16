#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <vector>
using namespace std;

void die(int line_no = 0) {
	cout << "Syntax Error on line " << line_no << endl;
	exit(1);
}

string register_lookup(string reg) {
	//0-3 and 14-15 are off limits
	if (reg == "A") return "R4";
	else if (reg == "B") return "R5";
	else if (reg == "C") return "R6";
	else if (reg == "I") return "R7";
	else if (reg == "J") return "R8";
	else if (reg == "X") return "R9";
	else if (reg == "Y") return "R10";
	else if (reg == "Z") return "R12";
	else die();
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
	
	vector<string> Strings; //<--------this is mine

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
			try{
				int x;
				ss >> x;
				//cout << "check 1" << endl;
				outs << "mov R0, #" << x << "\nBL print_number\n";
				//cout << "check 2" << endl;
				//cout << "check 3" << endl;
			}
			catch (...){
				string x;
				ss >> x;
				Strings.push_back(x);
				outs << "LDR R0,=string_" << Strings.size() - 1 <<endl;
				outs << "BL print_string" << endl;
				
			}
			continue;
		}
		else if (command == "LET") {
			string x;
			ss >> x;
			if (x == "A" || x == "B" || x == "C" || x == "I" || x == "J" || x == "X" || x == "Y" || x == "Z"){
				string y;
				ss >> y;
				bool isInt = true;
				try {
					ss >> y;
					isInt = false;
				}
				catch(...) {
					int y;
					ss >> y;
				}
				
				if (isInt){
					if (x == "A") outs << "MOV R4, #" << y << endl;
					else if (x == "B") outs << "MOV R5, #" << y << endl;
					else if (x == "C") outs << "MOV R6, #" << y << endl;
					else if (x == "I") outs << "MOV R7, #" << y << endl;
					else if (x == "J") outs << "MOV R8, #" << y << endl;
					else if (x == "X") outs << "MOV R9, #" << y << endl;
					else if (x == "Y") outs << "MOV R10, #" << y << endl;
					else if (x == "Z") outs << "MOV R12, #" << y << endl;
					else die(line_no);
					continue;
				}

				if (y == "A" || y == "B" || y == "C" || y == "I" || y == "J" || y == "X" || y == "Y" || y == "Z"){
					string z;
					ss >> z;
					string w;
					ss >> w;
					if (z == "+")outs << "ADD ";
					else if (z == "-")outs << "SUB ";
					else if (z == "*") outs << "MUL ";
					else die(line_no);

					if (x == "A") outs << "R4, ";
					else if (x == "B") outs << "R5, ";
					else if (x == "C") outs << "R6, ";
					else if (x == "I") outs << "R7, ";
					else if (x == "J") outs << "R8, ";
					else if (x == "X") outs << "R9, ";
					else if (x == "Y") outs << "R10, ";
					else if (x == "Z") outs << "R12, ";
					
					if (y == "A") outs << "R4, ";
					else if (y == "B") outs << "R5, ";
					else if (y == "C") outs << "R6, ";
					else if (y == "I") outs << "R7, ";
					else if (y == "J") outs << "R8, ";
					else if (y == "X") outs << "R9, ";
					else if (y == "Y") outs << "R10, ";
					else if (y == "Z") outs << "R12, ";

					if (z == "A") outs << "R4";
                    else if (z == "B") outs << "R5";
					else if (z == "C") outs << "R6";
					else if (z == "I") outs << "R7";
					else if (z == "J") outs << "R8";
					else if (z == "X") outs << "R9";
					else if (z == "Y") outs << "R10";
					else if (z == "Z") outs << "R12";
					else die(line_no);
					continue;

				}
				else{
					cout << "here 1" << endl;
					die(line_no);
				}
			}
			else{
				cout << "here 2" << endl;
				die(line_no);
			}
		}
		else if (command == "IF") {
			string var1,oper,var2,then,mr_goto,line,ms_else;
			int line1 = -1, line2 = -1; //Initalized to invalid values
			ss >> var1 >> oper >> var2 >> then >> mr_goto >> line;
			if (!ss) die();
			if (then != "THEN") die();
			if (mr_goto != "GOTO") die();
			try { line1 = stoi(line); } catch (...) { die(); } //Convert line into integer format
			bool has_else = true; //IF statements have an optional ELSE GOTO
			ss >> ms_else >> mr_goto >> line; //Read the ELSE GOTO 12
			try { line2 = stoi(line); } catch (...) { die(); } //Read second destination in
			if (!ss) has_else = false;
			var1 = register_lookup(var1); //Convert from "X" to "R9"
			var2 = register_lookup(var2); //Convert second variable
			//OK, input is all processed, now time to emit some ASM
			outs << "\tCMP " << var1 << "," << var2 << endl;
			if (oper == "<")	outs << "\tBLT line_" << line1 << endl;
			else if (oper == "<=")    outs << "\tBLE line_" << line1 << endl;
			else if (oper == ">")     outs << "\tBGT line_" << line1 << endl;
			else if (oper == ">=")    outs << "\tBGE line_" << line1 << endl;
			else if (oper == "==")    outs << "\tBEQ line_" << line1 << endl;
			else if (oper == "!=")    outs << "\tBNE line_" << line1 << endl;
			else die(); //invalid operator
			if (has_else) outs << "\tBAL line_" << line2 << endl;


		}
		else if (command == "INPUT") {

		}
		else if (command == "END") {
		outs << "\tBAL quit\n";
		continue;
		}
		else {
		
			die(line_no);
		}

		//End code <-----------------------------------------------------------------------------------
	}

	//Clean up the file at the bottom
	outs << "\nquit:\n\tMOV R0, #42\n\tPOP {R4-R12}\n\tPOP {PC}\n"; //Finish the code and return

	outs << "\n.data\n"; //String info <-------------This is also mine
	for (int i = 0; i <= Strings.size(); i++){
		outs << "string_" << i << " \"" << Strings[i] << "\"" << endl;
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

