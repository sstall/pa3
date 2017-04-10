//pa3.cpp
//Samuel Stall

#include "pa3.h"

using namespace std;

std::vector<Stack*> proccessFile(Stack file);

int main(int argc, char* argv[]) {

	string fileName;
	Stack *stack;

	do {

		cout << "Please enter the name of the input file: " << endl;
		cin >> fileName;
		cout << endl;
		cin.ignore(100000, '\n');
		stack = readFile(fileName);

		if(stack == NULL) {
			cout << "Error: File not found\n" << endl;
		}

	} while(stack == NULL);
	
	Stack file;

	//Transfers the stack to another stack, reversing it so it is in the correct order
	while(!stack->isEmpty()) {
		file.push(stack->pop());
	}

	//Deletes old stack
	delete stack;

	//Calls the processFile function, giving the new stack to it to find all keywords, identifiers, constants, operators, delimiters, syntax errors, and depths of loops
	vector<Stack*> results = proccessFile(file); 

	cout << "Keywords: ";
	results[0]->printStack();
	cout << endl;

	cout << "Identifier: ";
	results[1]->printStack();
	cout << endl;

	cout << "Constant: ";
	results[2]->printStack();
	cout << endl;

	cout << "Operators: ";
	results[3]->printStack();
	cout << endl;

	cout << "Delimiter: ";
	results[4]->printStack();
	cout << endl;

	cout << endl;
	cout << "Syntax error(s): "; 
	if(results[5]->isEmpty()) {
		cout << "NA";
	}
	else {
		results[5]->printStack();
	}
	cout << endl;

	return 0;
}

std::vector<Stack*> proccessFile(Stack file) {

	Stack loop;
	Stack *keywords = new Stack();
	Stack *identifiers = new Stack();
	Stack *constants = new Stack();
	Stack *operators = new Stack();
	Stack *delimiters = new Stack();
	Stack *syntaxErr = new Stack();

	string process;
	int maxNest = 0;
	int nest = 0;
	int openP = 0;
	int closeP = 0;

	while(!file.isEmpty()) {

		string temp = file.pop();
		process = temp;

		for(unsigned int i = 0; i < temp.length(); i++) {

			char c = temp.at(i);
			string s(1,c);

			if(s == ";" || s == ",") {

				process = process.substr(0,i) + " " + process.substr(i + 1);

				addToContainer(delimiters, s);

			}
			else if(s == "+" || s == "-" || s == "*" || s == "/" || s == "%" || s == "=" ) {

				if(s == "+") {
					if (i < temp.length() - 1)
					{
						char c1 = temp.at(i+1);
						string s1(1,c1);
						if(s1 == "+") {
							addToContainer(identifiers, "++");
							process = process.substr(0,i) + "  " + process.substr(i + 2);
							i++;							
						}
						else {
							addToContainer(identifiers, s);
							process = process.substr(0,i) + " " + process.substr(i + 1);
						}
					}
					else {
						addToContainer(identifiers, s);
						process = process.substr(0,i) + " " + process.substr(i + 1);
					}
				}
				else if(s == "-") {
					if (i < temp.length() - 1)
					{
						char c1 = temp.at(i+1);
						string s1(1,c1);
						if(s1 == "-") {
							addToContainer(identifiers, "--");
							process = process.substr(0,i) + "  " + process.substr(i + 2);
							i++;
						}
						else {
							addToContainer(identifiers, s);
							process = process.substr(0,i) + " " + process.substr(i + 1);
						}
					}
					else { 
						addToContainer(identifiers, s);
						process = process.substr(0,i) + " " + process.substr(i + 1);
					}
					
				}
				else {
					addToContainer(identifiers, s);
					process = process.substr(0,i) + " " + process.substr(i + 1);
				}
				

				
			}
			if(s == "(") {
				openP++;
				process = process.substr(0,i) + " " + process.substr(i + 1);
			}	
			else if(s == ")") {
				closeP++;
				process = process.substr(0,i) + " " + process.substr(i + 1);
			}
			
		}

		//Seperates all words left in the string into seperate strings in a vector
		vector<string> array;
		size_t pos = 0, found;

		while((found = process.find_first_of(" ", pos)) != string::npos) {
			array.push_back(process.substr(pos, found - pos));
			pos = found + 1;
		}
		array.push_back(process.substr(pos));


		//Checks for keywords and keeps track of the depth of nested loops
		for(unsigned int i = 0; i < array.size(); i++) {

			if(array[i].length() == 0) {
				array[i].erase();
				continue;
			}

			if(array[i] == "FOR" || array[i] == "BEGIN" || array[i] == "END") {
				if(array[i] == "FOR") {
					loop.push(array[i]);
					addToContainer(keywords, array[i]);
					nest++;
				}
				else if(array[i] == "BEGIN") {
					loop.push(array[i]);
					addToContainer(keywords, array[i]);
				}
				else if(array[i] == "END") {
					if(!loop.isEmpty()) {
						string s1 = loop.pop();
						if(!loop.isEmpty() && s1 == "BEGIN") {
							string s2 = loop.pop();
							if(s2 == "FOR") {
								if(nest > maxNest) {
									maxNest = nest;
									nest--;
								}
								nest--;
							}
							else if(!loop.isEmpty()) {
								loop.pop();
								nest--;
							}	
							else {
								nest--;
							}						
						}
						else if(!loop.isEmpty()) {
							loop.pop();
							nest--;
						}
						else {
							nest--;
						}

					}
					else {
						addToContainer(syntaxErr, "END");
					}
					addToContainer(keywords, array[i]);
				}

				continue;

			}

			//Tests if the word is a constant
			try {
				if(stoi(array[i], nullptr, 0) != -1) {
					addToContainer(constants, array[i]);
					continue;
				}				
			} catch(invalid_argument e) {

			}

			//Checks if the word is an operator
			if(isupper(array[i].at(0)) == 0) {
				addToContainer(operators, array[i]);
				continue;
			}

			if(isupper(array[i].at(0)) != 0) {
				nest--;
				if(nest > maxNest) {
					maxNest = nest;
				}
			}

			//Puts the word into the syntax error list if it fell through every other test
			addToContainer(syntaxErr, array[i]);

		}		

	}

	if(!loop.isEmpty()) {
		addToContainer(syntaxErr, "END");
	}

	if(openP != closeP) {
		if(openP > closeP) {
			addToContainer(syntaxErr, "(");
		}
		else if(openP < closeP) {
			addToContainer(syntaxErr, ")");
		}
	}


	cout << "The depth of nested loop(s) is " << maxNest << endl << endl;
 

	return vector<Stack*> {keywords, operators, constants, identifiers, delimiters, syntaxErr};
}

//Attempts to read a file given the file name, and then pushes all contents of the file to a stack object and then returns the stack object
//returns NULL if no file of the given name was found
Stack* readFile(string fileName) {
	Stack *read = new Stack();

	ifstream txtFile(fileName);

	if(txtFile.is_open()) {
		string s;

		while(txtFile >> s) {
			read->push(s);
		}
		txtFile.close();
		return read;
	}
	txtFile.close();
	return NULL;
}

void addToContainer(Stack* stack, string s) {
	if(!stack->contains(s)){
		stack->push(s);
	}
}

//Initializes Stack object by setting size to 0
Stack::Stack() {
	stackSize = 0;
}

//Returns size of stack
int Stack::size() {
	return stackSize;
}

//Adds a string to the stack object at the top of the container vector
void Stack::push(string addData) {
	stack.push_back(addData);
	stackSize++;
}

//Returns the top most string from the container vector, deleting it from the container
//Whenever called, must have checks to see if the stack size goes to 0
string Stack::pop() {
	string s = stack.back();
	stack.pop_back();
	stackSize--;
	return s;
}

//Checks if a given element is contained in the stack
bool Stack::contains(string data) {
	if(stackSize != 0) {
		for(int i = 0; i < stackSize; i++) {
			if(stack[i] == data) {
				return true;
			}
		}
	}
	return false;
}

//Returns whether the stack is empty or not
bool Stack::isEmpty() {
	if(stackSize == 0) {
		return true;
	}
	return false;
}

void Stack::printStack() {
	if(stackSize != 0) {
		for(int i = 0; i < stackSize; i++) {
			cout << " " << stack[i];
		}
	}
}