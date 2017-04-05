//pa3.h

#ifndef pa3
#define pa3

#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <stdexcept>

class Stack {
private:
	int stackSize;
	std::vector<std::string> stack;
public:
	Stack();
	int size();
	void push(std::string addData);
	std::string pop();
	bool contains(std::string data);
	bool isEmpty();
	void printStack();
};

Stack* readFile(std::string fileName);
std::vector<Stack*> processFile(Stack file);
void addToContainer(Stack* stack, std::string s);

#endif