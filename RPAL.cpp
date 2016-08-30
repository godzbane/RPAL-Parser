#include <cstring>
#include <iostream>
#include <stdio.h>
#include <ctype.h>d
#include <stack>
#include <cstddef>
 #include <fstream>
#include <streambuf>
#include <stdlib.h>
#include <vector>
#include <cmath>

using namespace std;

void A();
void At();
void Ap();
void Af();

void B();
void Bt();
void Bs();
void Bp();

void D();
void Da();
void Dr();
void Db();

void Ew();

void R();
void Rn();

void T();
void Ta();
void Tc();
void Vb();


class Environment {
	public:
	Environment * parent;
	int usedVariables;
	bool stop = false;
	string variableName [100];
	string stringValue [100];
	string innerBounds [100];
	int tupleLength = 0;
	int deltaPos;
};

Environment allEnvs[100];
int activeEnvIndex = 0;

struct Token {
    // classifiers type;
    string classifier;
    string value;
    string boundVariable [100];
    Environment currentEnv;
    int deltaPos;
    string tupleValues [100];
    int tupleSize;
};

class ASTNode {
	public:
   string node;
   Token t;
   ASTNode *parent;
   ASTNode *sibling;
   ASTNode *child;
   ASTNode()
   {
	   parent=NULL;
	   sibling=NULL;
	   child=NULL;
	   node="";

   }
};


Token readNextToken();


stack <ASTNode> RPALstack;

stack <ASTNode> holdStuff;

stack <ASTNode> tempHolder;
stack <Environment> tempEnvHolder;
Environment activeEnvironment;

stack <ASTNode*> flattenedStacks[1000];
stack <ASTNode*> CSEStack;

int CSECounter = 0;

std::string input = "";

bool isEmpty = false;

bool debug = false;
bool ast = false;
bool st = false;

void cleanInput () { // Cleans input till next token

	int chv = input[0];

    while (input[0] == ' ' || input[0] == '\t' || chv == 10) {
    input.erase(0,1);
    cleanInput();
    chv= input[0];
    }
    while (input[0] == '/' && input[1] == '/') {
    	chv = input[0];
        while (input[0] != '\n' || chv != 10) {
        	input.erase(0,1);
        	chv = input[0];
        }
        input.erase (0,1);
        cleanInput();
    }
    chv = input[0];
    while (input[0] == '\n' || chv == 10) {
        input.erase (0,1);
        chv = input[0];
        cleanInput();
    }
}

inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}


bool isKeyword (string a) {
	if (a!= "let"&& a!= "in"&& a!= "where"&& a!= "rec"&& a!= "eq"&& a!= "gr"&& a!= "ls"&& a!= "ne"&& a!= "not"&& a!= "or" && a!= "@" && a!= "within" && a!= "aug"&& a!= "and")
		return false;
	return true;
}

bool isUnaryOperation (string a) {

	if (a == "Stern" || a == "Stem" || a == "Print" || a == "neg" || a == "Order")
		return true;
	else return false;

}

bool isBinaryOperation (string a) {

	if (a == "+" || a == "-" ||  a == "/" || a == "*" || a == "**" || a == "Conc" || a == "aug" || a == "gr" || a == "ls" || a == "or" || a == "and" || a=="eq")
		return true;
	else return false;

}

void CSEApply () {

	string op = CSEStack.top()->t.value;
	ASTNode * opNode = CSEStack.top();

	CSEStack.pop();
	ASTNode ob1 = *CSEStack.top();

	if (debug)
	cout << "OPERAND ONE IS " << ob1.node;

	ASTNode result =ob1;

	result.t.value = ob1.t.value;

	CSEStack.pop();

	if (isBinaryOperation(op)) {
		ASTNode ob2 = *CSEStack.top();
		if (debug)
		cout << endl << " OPERAND TWO IS " << ob2.node;

		CSEStack.pop();

		if (op == "+") {
			if (ob1.t.classifier != "Tuple" && ob2.t.classifier != "Tuple") {
				if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {
					int a = atoi(ob1.t.value.c_str()) + atoi(ob2.t.value.c_str());
					char str[20];
					sprintf(str, "%d", a);
					result.t.value = str;
					result.node = str;
				} else try {result.t.value = ob1.t.value + ob2.t.value;}
				catch(...) {cout << "Cannot add";}
			} else if (ob1.t.classifier == "Tuple" && ob2.t.classifier == "Tuple") {
				int length1 = ob1.t.tupleSize;
				int length2 = ob2.t.tupleSize;
				int l = length1;

				if (length2 > length1)
					l = length2;

				for (int i = 0; i < l; i++) {

					if (isInteger (ob1.t.tupleValues[i]) && isInteger (ob2.t.tupleValues[i])) {
						int a = atoi(ob1.t.tupleValues[i].c_str()) + atoi(ob2.t.tupleValues[i].c_str());
						char str[20];
						sprintf(str, "%d", a);
						result.t.tupleValues[i] = str;
					}

					else result.t.tupleValues[i] += ob2.t.tupleValues[i];

				}
			}

		}
		else if (op == "-") {
			if (ob1.t.classifier != "Tuple" && ob2.t.classifier != "Tuple") {
				if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {
					if (debug)
					cout << endl << "Integer subtraction" << endl;
					int a = atoi(ob1.t.value.c_str()) - atoi(ob2.t.value.c_str());
					char str[20];
					sprintf(str, "%d", a);
					result.t.value = str;
					result.node = str;
				} else try {result.t.value = ob1.t.value + ob2.t.value;}
				catch(...) {}
			} else if (ob1.t.classifier == "Tuple" && ob2.t.classifier == "Tuple") {
				int length1 = ob1.t.tupleSize;
				int length2 = ob2.t.tupleSize;
				int l = length1;

				if (length2 > length1)
					l = length2;

				for (int i = 0; i < l; i++) {

					if (isInteger (ob1.t.tupleValues[i]) && isInteger (ob2.t.tupleValues[i])) {
						int a = atoi(ob1.t.tupleValues[i].c_str()) - atoi(ob2.t.tupleValues[i].c_str());
						char str[20];
						sprintf(str, "%d", a);
						result.t.tupleValues[i] = str;
					}

					else result.t.tupleValues[i] += ob2.t.tupleValues[i];

				}
			}

		}
		else if (op == "/") {
			if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {
				int a = atoi(ob1.t.value.c_str()) / atoi(ob2.t.value.c_str());
				char str[20];
				sprintf(str, "%d", a);

				result.t.value = str;
				result.node = str;
			}
		}
		else if (op == "*") {
			if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {
				int a = atoi(ob1.t.value.c_str()) * atoi(ob2.t.value.c_str());
				char str[20];
				sprintf(str, "%d", a);
				result.t.value = str;
				result.node = str;

			}
								}
		else if (op == "**") {
			if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {
				int a = pow(atoi(ob1.t.value.c_str()), atoi(ob2.t.value.c_str()));
				char str[20];
				sprintf(str, "%d", a);

				result.t.value = str;
				result.node = str;

			}
		}

		else if (op == "ls") {

					if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {

						string v = "false";
						if(atoi(ob1.t.value.c_str()) < atoi(ob2.t.value.c_str()))
							v = "true";

						result.t.value = v;
						result.node = v;

					} else {
						result.t.value = "false";
						result.node = "false";
					}
				}
		else if (op == "gr") {
							if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {
								string v = "false";
								if(atoi(ob1.t.value.c_str()) > atoi(ob2.t.value.c_str()))
									v = "true";

								result.t.value = v;
								result.node = v;

							} else {
								result.t.value = "false";
								result.node = "false";
							}
						}
		else if (op == "eq") {
									if (isInteger (ob1.t.value) && isInteger (ob2.t.value)) {

										string v = "false";
										if(atoi(ob1.t.value.c_str()) == atoi(ob2.t.value.c_str()))
											v = "true";

										result.t.value = v;
										result.node = v;

									} else {

										if (ob1.t.value == ob2.t.value) {
											result.t.value = "true";
											result.node = "true";

										}
										else {
										result.t.value = "false";
										result.node = "false";
										}
									}

									if (debug)
									cout << endl << "EQUALS RETURNS A " << result.node << endl;
								}
					else if (op == "or") {
											if (ob1.t.value == "true" || ob2.t.value == "true" || ob1.t.value == "<true>" || ob2.t.value == "<true>") {

												result.t.value = "true";
												result.node = "true";

											} else {
												result.t.value = "false";
												result.node = "false";
											}

											if (debug)
											cout << endl << "RESULT OF OR ON " <<  ob1.t.value << "," << ob2.t.value << " IS " << result.node;

										}

						else if (op == "and") {
													if (ob2.t.value == "true" && ob2.t.value == "true") {

														result.t.value = "true";
														result.node = "true";

													} else {
														result.t.value = "false";
														result.node = "false";
													}
												}

		else if (op == "Conc")
			result.t.value = ob1.t.value + ob2.t.value;
		else if (op == "aug") {

			if (ob1.t.classifier == "Tuple" && ob2.t.classifier == "Tuple")
				result.t.tupleSize = ob1.t.tupleSize + ob2.t.tupleSize;

			else if (ob1.t.classifier == "Tuple")
				result.t.tupleValues[++result.t.tupleSize] = ob2.t.value;

			else if (ob2.t.classifier == "Tuple") {
				result.node = "Tuple";
				result.t.classifier = "Tuple";
				result.t.tupleValues[0] = result.t.value;

				for (int i = 0; i < ob2.t.tupleSize; i++)
					result.t.tupleValues[i+1] = ob2.t.tupleValues[i];

				result.t.tupleSize = ob2.t.tupleSize + 1;
			}
			else {
				result.node = "Tuple";
				result.t.classifier = "Tuple";
				result.t.tupleValues[0] = ob1.t.value;
				result.t.tupleValues[1] = ob2.t.value;
				result.t.tupleSize = 2;
			}

		}
		if (debug) {
			cout << endl <<"----------" << endl;
			if (!CSEStack.empty())
				cout << endl << "TOP OF CSE IS "<< CSEStack.top()->node;
			else cout << "CSE IS EMPTY";
				cout << endl << "Pushing RESULT OF APPLY: " << result.node;
		}

		tempHolder.push(result);
		CSEStack.push(&tempHolder.top());

		if (debug) {
			cout << endl<<"After push, top of cse is: "<< CSEStack.top()->node;
			cout << endl << "----------" << endl;
		}


	} else {

		if (op == "Print") {
			if (debug)
				cout << "----------" << "PRINTING:" << "---------" << endl;
		if (ob1.t.classifier != "Tuple" && ob1.t.classifier != "tau" && ob1.node != "tau")
		cout <<  ob1.t.value;
		else {
			cout << "(";
			for (int i = 0; i < ob1.t.tupleSize; i++)
			{
				cout<< ob1.t.tupleValues[i];
				if (i < ob1.t.tupleSize-2)
					cout << ",";
			}
			cout << ")";
		}
	}

	else if (op == "Stem") {
	    	result.t.value = ob1.t.value.substr(0, 1);

	}

	else if (op == "Order") {
				result.t.classifier = "Word/Number";
				int i = 0;
				while (ob1.t.tupleValues[i].length() > 0)
					i++;

					char str[20];
					sprintf(str, "%d", i);

		    	result.t.value = str;
		    	result.node = str;
		}
	else if (op == "Stern") {
		result.t.value = ob1.t.value.substr(1, ob1.t.value.length()-1);
		if (debug)
		cout << endl << "STERN IS " << result.t.value;
		}
	else if (op == "neg") {
			result.t.value = "-" + ob1.t.value;
			result.node = "-" + ob1.node;
			if (debug)
			cout << "Did neg";
			}
		if (debug)
			cout << "Pushing RESULT OF APPLY: " << result.node;
		tempHolder.push (result);
		CSEStack.push (&tempHolder.top());
	}
}

void CSEGamma () {

	if (debug)
	cout << endl << "In Gamma: ";

	if (!CSEStack.empty()) {

		if (isUnaryOperation(CSEStack.top()->t.value) || isBinaryOperation(CSEStack.top()->t.value)) {
			if (debug)
			cout << "Apply " << CSEStack.top()->t.value << endl;
			CSEApply();
		}

		else if (CSEStack.top()->node == "delta") {
			if (debug)
			cout << "Delta with BV: " << CSEStack.top()->t.boundVariable[0] << endl;

				// Change environment

				Environment previous;
				previous = allEnvs[activeEnvIndex];//*(&activeEnvironment);
				CSEStack.top()->t.currentEnv.parent = &previous;
				activeEnvironment = CSEStack.top()->t.currentEnv;
				allEnvs[++activeEnvIndex] = activeEnvironment;
				activeEnvironment.usedVariables = 0;

				ASTNode myDelta = *CSEStack.top();
				CSEStack.pop();

				// Add bound variable

				if (!CSEStack.empty()) {
					//cout << "Classifier is: " + CSEStack.top()->t.classifier << " Size: " << CSEStack.top()->t.tupleSize;

				if (CSEStack.top()->t.classifier != "Tuple" && CSEStack.top()->node != "delta") {
					activeEnvironment.variableName [activeEnvironment.usedVariables] = myDelta.t.boundVariable[0];
					activeEnvironment.stringValue [activeEnvironment.usedVariables++] = CSEStack.top()->t.value;

					if (debug)
					cout << endl << "Bound variable: " << activeEnvironment.variableName [activeEnvironment.usedVariables-1] << " with value " << activeEnvironment.stringValue [activeEnvironment.usedVariables-1] << endl;

				} else if (CSEStack.top()->t.classifier == "Tuple") {
					for (int i = 0; i < CSEStack.top()->t.tupleSize; i++) {
						activeEnvironment.variableName [i] = myDelta.t.boundVariable[i];
						activeEnvironment.stringValue [i] = CSEStack.top()->t.tupleValues[i];

						if (debug)
						cout << endl << "Bound variable: " << activeEnvironment.variableName [0] << " with value " << activeEnvironment.stringValue [i] << endl;
					}
					activeEnvironment.usedVariables = CSEStack.top()->t.tupleSize;
					activeEnvironment.tupleLength = CSEStack.top()->t.tupleSize;

				}
				else if (CSEStack.top()->node == "delta") {
					activeEnvironment.variableName[0] = myDelta.t.boundVariable[0];
					activeEnvironment.usedVariables = 1;
					activeEnvironment.stringValue[0] = "codeWordDelta";
					activeEnvironment.deltaPos = CSEStack.top()->t.deltaPos;

					for (int i = 0; i < 100; i++)
					activeEnvironment.innerBounds[i] = CSEStack.top()->t.boundVariable[i];

					/*if (debug)
					cout << endl << "Bound variable: " << activeEnvironment.variableName [0] << " with delta " << activeEnvironment.deltaPos << endl;
				*/
				}

				allEnvs[activeEnvIndex] = activeEnvironment;

				CSEStack.pop();


				}

				// Load the delta


				int deltaToLoad = myDelta.t.deltaPos;

				// Load env marker

						ASTNode envMarker;
						envMarker.t.value = "environmentExitMarker";
						envMarker.t.classifier = "environmentExitMarker";
						tempHolder.push (envMarker);
						flattenedStacks[0].push(&envMarker);

				stack <ASTNode*> tempDelta;


		while (!flattenedStacks[deltaToLoad].empty())
						{
							tempDelta.push (flattenedStacks[deltaToLoad].top());
							flattenedStacks[deltaToLoad].pop();
						}

						while (!tempDelta.empty())
						{
							ASTNode newNode = *tempDelta.top();
							tempHolder.push (newNode);
							flattenedStacks[0].push (&tempHolder.top());

							flattenedStacks[deltaToLoad].push(&tempHolder.top());

							tempDelta.pop();

							if (debug)
							cout << endl << "Node pushed on stack " << flattenedStacks[0].top()->node;


						}

			}

	}
	else cout << "Error: CSE stack empty on Gamma!";


}




void CSEMachine () {



if (!flattenedStacks[0].empty()) {

	if (debug) {
	cout << endl << " Leftstack " << "(" << flattenedStacks[0].top()->node << ", " << flattenedStacks[0].top()->t.value << ")"; // <<", Deltapos:" << flattenedStacks[0].top()->t.deltaPos << ") ::: ";


	if (!CSEStack.empty())
	cout << "Rightstack  "<< "(" << CSEStack.top()->node << ", " << CSEStack.top()->t.classifier << ")" << endl; //", Deltapos:" << CSEStack.top()->t.deltaPos << ") ::";
	else cout << "Rightstack empty ::";
	}


	if ((flattenedStacks[0].top()->node == "" || flattenedStacks[0].top()->node == " ")  && (flattenedStacks[0].top()->t.classifier == "" || flattenedStacks[0].top()->t.classifier == " ")) {

		flattenedStacks[0].pop();
		//CSEGamma ();
	}

	else if (flattenedStacks[0].top()->t.classifier == "beta") {
		flattenedStacks[0].pop();
		if (!CSEStack.empty()) {

			if (CSEStack.top()->node == "true" || CSEStack.top()->node == "<true>")
			 {
				if (debug)
				cout << "BETA TRUE";
				flattenedStacks[0].pop();
			}
			else {
				if (debug)
				cout << "BETA FALSE";
							ASTNode * tempStackPos = flattenedStacks[0].top();
							flattenedStacks[0].pop();
							flattenedStacks[0].pop();
							flattenedStacks[0].push (tempStackPos);
			}

			CSEStack.pop();
		}
		else cout << "FATAL ERROR STACK EMPTY";

	}

	else if (flattenedStacks[0].top()->node == "tau") {

		int numOfChildren = flattenedStacks[0].top()->t.tupleSize;
		if (debug) {
		cout << "Object is " << flattenedStacks[0].top()->t.classifier;
		cout << endl << "numOfChildren is " << flattenedStacks[0].top()->t.tupleSize << endl;
		}
		flattenedStacks[0].top()->t.classifier = "Tuple";
		flattenedStacks[0].top()->node = "Tuple";

		for (int i = 0; i < numOfChildren && !CSEStack.empty();i++) {
			flattenedStacks[0].top()->t.tupleValues[i] = CSEStack.top()->t.value;
			CSEStack.pop();
		}
		CSEStack.push(flattenedStacks[0].top());
		CSEStack.top()->node = "Tuple";
		CSEStack.top()->t.classifier = "Tuple";
		flattenedStacks[0].pop();
	}

	else if (flattenedStacks[0].top()->node == "gamma") {
		flattenedStacks[0].pop();
		CSEGamma ();
	}
	else if (flattenedStacks[0].top()->node == "delta") {
		CSEStack.push(flattenedStacks[0].top());
		flattenedStacks[0].pop();
	}

	// Binops do with single gamma

	else if (isBinaryOperation(flattenedStacks[0].top()->t.value) || flattenedStacks[0].top()->node == "neg") {
		CSEStack.push (flattenedStacks[0].top());
		flattenedStacks[0].pop();
		CSEApply ();
	}
	else if (isUnaryOperation(flattenedStacks[0].top()->t.value) ) {
			CSEStack.push (flattenedStacks[0].top());
			flattenedStacks[0].pop();
		}

	else if (flattenedStacks[0].top()->t.classifier == "environmentExitMarker") {
//TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO TO DO
		if (activeEnvIndex > 0)
			activeEnvironment = allEnvs[--activeEnvIndex];


		flattenedStacks[0].pop();

		if (debug) {
		cout << "Popped env";

		if (!flattenedStacks[0].empty())
			cout << "AFTER ENV IS " << flattenedStacks[0].top()->node;
		else cout << "ALL OVER!";
		}

	}

	// Push into CSEStack
	else {//if (flattenedStacks[0].top()->t.classifier.length() > 0) {
		bool found = false;
		bool foundDelta = false;
		bool foundTuple = false;
		string value = "";
		int tupleSize = 0;
		int usedV = 0;
		string tupleValues[100];

		Environment * envTravel = &allEnvs[activeEnvIndex];//&activeEnvironment;

		int tempEnvIndex = activeEnvIndex;

		while (tempEnvIndex >= 0 && envTravel != NULL && !found) {
			if (envTravel->variableName[0].length() > 0) {
				for (int i  = 0; i < envTravel->usedVariables; i++) {
					if (envTravel != NULL && envTravel->variableName[i] == flattenedStacks[0].top()->t.value) {
						found = true;
						if (debug)
						cout << endl << "GOT SOMETHING for " << envTravel->variableName[i] << " at " << i;
						if (envTravel->stringValue[i] != "codeWordDelta") {
							value = envTravel->stringValue[i];

							for (int j = 0; envTravel->stringValue[j].length() > 0; j++)
								tupleSize++;

							for (int j = 0; envTravel->variableName[j].length() > 0; j++)
								usedV++;


							if (tupleSize > usedV) {
								foundTuple = true;
								value = "Tuple";
								if (debug)
								cout<< endl << "Tuple:";
								for (int z = 0; z < tupleSize; z++) {
									tupleValues[z] = envTravel->stringValue[z];
										//cout << tupleValues[j];
								}
							}


							/*
							for (int j = i; i < sizeof(envTravel->stringValue) && strlen(envTravel->stringValue)[j]) > 1; j++) {
								valueTuple[j-i] = envTravel->stringValue[j];
								existingVars++;
							}
							if (existingVars > 1)
							 	*/

							if (debug)
							cout << "Got value " << value;
						}

						else {
							//WAIT NO DONT OPEN IT JUST PUT A DELTA ON TOP OF CSESTACK
							if (debug)
							cout << endl << "Found a delta";
							ASTNode delta;
							delta.node = "delta";
							delta.t.deltaPos = envTravel->deltaPos;
							//cout << endl << "envTravel->deltaPos is " << envTravel->deltaPos;
							//cout << endl << "allEnvs[envTravel->deltaPos] is " << allEnvs[envTravel->deltaPos].variableName[0];

							for (int i = 0; i < 100; i++)
							delta.t.boundVariable[i] = activeEnvironment.innerBounds[i];
							//cout << endl << "Prolly the fuckup: " << delta.t.boundVariable[0];
							foundDelta = true;
							tempHolder.push(delta);
							CSEStack.push(&tempHolder.top());
							flattenedStacks[0].pop();

				// CODE FOR A PROPER DELTA POP

							/*foundDelta = true;
							int deltaToLoad = envTravel->deltaPos;


							flattenedStacks[0].pop();
							cout << "Loading a delta..." << deltaToLoad;

							stack <ASTNode*> tempDelta;

											while (!flattenedStacks[deltaToLoad].empty())
											{
												tempDelta.push (flattenedStacks[deltaToLoad].top());
												flattenedStacks[deltaToLoad].pop();
											}

											while (!tempDelta.empty())
											{
												ASTNode newNode = *tempDelta.top();
												flattenedStacks[0].push (&newNode);
												flattenedStacks[deltaToLoad].push(tempDelta.top());

												tempDelta.pop();
												cout << endl << "Pushing onto stack node: (" << flattenedStacks[0].top()->node<< ", " <<flattenedStacks[0].top()->t.value <<")";

											}*/

						}
						break;
					}
				}

			}
			if (tempEnvIndex > 0)
				envTravel = &allEnvs[--tempEnvIndex];
			else break;

		}

		if (found) {
			if (!foundDelta) {

				if (foundTuple) {
					if (debug)
					cout << "Found tuple!";
					flattenedStacks[0].top()->t.value = value;
					flattenedStacks[0].top()->node = "Tuple";
					flattenedStacks[0].top()->t.classifier = "Tuple";
					//flattenedStacks[0].top()->t.tupleSize = "Tuple";

					for (int j = 0; j < tupleSize; j++)
						flattenedStacks[0].top()->t.tupleValues[j] = tupleValues[j];



					flattenedStacks[0].top()->t.tupleSize = tupleSize;
					CSEStack.push (flattenedStacks[0].top());
					flattenedStacks[0].pop();

				}

				else {
					if (debug)
					cout << endl << "Replacing " << flattenedStacks[0].top()->t.value << " with " << value << endl;
					flattenedStacks[0].top()->t.value = value;
					flattenedStacks[0].top()->node = value;
					CSEStack.push (flattenedStacks[0].top());
					flattenedStacks[0].pop();
					if (debug)
					cout << "Replaced a value";
				}
			}
			else if (debug)cout << endl << "Found and replaced a delta";
		}
		else if (!foundDelta) {
			CSEStack.push (flattenedStacks[0].top());
			flattenedStacks[0].pop();
		}

	}
		//CSEMachine();

}

}

void PreorderFlattenTree(ASTNode * root, int n, bool nosibling = false) { //n=numdots
        if(root != NULL) { //root!=null

        	if (root->node == "lambda") {
        		ASTNode delta;
        		delta.node = "delta";

        		int nextFreePos = 0;
        		while (!flattenedStacks[nextFreePos].empty())
        			nextFreePos++;

        		//cout << nextFreePos << endl;

        		delta.t.deltaPos = nextFreePos;
        		root->t.deltaPos = nextFreePos;

        		for (int i = 0; i < 100; i++)
        		delta.t.boundVariable[i] = "";

        		if (root->child != NULL && root->child->node != ",") {
        			delta.t.boundVariable[0] = root->child->t.value;
        				if (debug)
        			cout << "BV of Delta: "<< delta.t.boundVariable[0] << endl;
        		}
        		else {
					int i = 0;
					ASTNode* tempRoot = root->child->child;
					while (tempRoot!= NULL) {
						delta.t.boundVariable[i] = tempRoot->t.value;
						i++;
						tempRoot = tempRoot->sibling;
					}
        		}

        		Environment newEnv;
        		tempEnvHolder.push(newEnv);
        		delta.t.currentEnv = tempEnvHolder.top();

        		tempHolder.push(delta);
        		 flattenedStacks[n].push(&tempHolder.top());

        		//flattenedStacks[nextFreePos].push (root);
         		if (root->child!= NULL)
         			PreorderFlattenTree(root->child->sibling, nextFreePos);
         		if (!nosibling)
         		PreorderFlattenTree(root->sibling, n);
        	}

        	else if (root->node == "->") {

				int nextFreePos = 0;

				while (!flattenedStacks[nextFreePos].empty())
					nextFreePos++;


				ASTNode g;
				g.node = "gamma";
				tempHolder.push(g);
				 flattenedStacks[n].push(&tempHolder.top());



					ASTNode delta1;
					delta1.node = "delta";



					int basePos = nextFreePos;

					//cout << nextFreePos << endl;

					nextFreePos++;

					delta1.t.deltaPos = nextFreePos;
					root->t.deltaPos = nextFreePos-1;

					Environment newEnv;
					tempEnvHolder.push(newEnv);
					delta1.t.currentEnv = tempEnvHolder.top();

					tempHolder.push(delta1);
					 flattenedStacks[n].push(&tempHolder.top());

					//flattenedStacks[nextFreePos].push (root);
					if (root->child->sibling!= NULL)
						PreorderFlattenTree(root->child->sibling, nextFreePos, true);

					ASTNode delta2;
					delta2.node = "delta";

					cout << nextFreePos << endl;

					delta2.t.deltaPos = ++nextFreePos;

					Environment newEnv2;
					tempEnvHolder.push(newEnv2);
					delta2.t.currentEnv = tempEnvHolder.top();

					tempHolder.push(delta2);
					flattenedStacks[n].push(&tempHolder.top());

					if (root->child->sibling->sibling!= NULL)
						PreorderFlattenTree(root->child->sibling->sibling, nextFreePos, true);


					ASTNode beta;
					beta.node = "beta";
					beta.t.classifier="beta";
					tempHolder.push(beta);
					flattenedStacks[n].push(&tempHolder.top());

					 PreorderFlattenTree (root->child, basePos, true);

					//PreorderFlattenTree(root->child, n);


        	        	}

        	else  {
        	        		flattenedStacks[n].push (root);
        	        	PreorderFlattenTree(root->child, n);
                 		if (!nosibling)
        	        	PreorderFlattenTree(root->sibling, n);
        	        	}


        }
    }

void PrintTree(ASTNode * root,int n) { //n=numdots
        if(root != NULL) { //root!=null
            for(int i=0;i<n;i++)
                cout<<".";

            cout<< root->node <<endl;

            PrintTree(root->child,n+1);
            PrintTree(root->sibling,n);
        }
    }


ASTNode * StandardizeLet (ASTNode * root) {


	ASTNode *equals = root->child;
	ASTNode *P = equals->sibling;
	ASTNode *X = equals->child;
	ASTNode *E = X->sibling;

	ASTNode gamma; // new root
	gamma.node = "gamma";
	ASTNode lambda;
	lambda.node = "lambda";
	tempHolder.push(lambda);


	gamma.child = &tempHolder.top();

	tempHolder.top().sibling = E;
	tempHolder.top().child = X;
	X->sibling = P;

	tempHolder.push(gamma);


	return &tempHolder.top();
}

ASTNode * StandardizeWithin (ASTNode * root) {

	ASTNode *eq1 = root->child;
	ASTNode *eq2 = eq1->sibling;
	ASTNode *X1 = eq1->child;
	ASTNode *E1 = X1->sibling;
	ASTNode *X2 = eq2->child;
	ASTNode *E2 = X2->sibling;

	ASTNode newRoot;
	newRoot.node = "=";
	newRoot.child = X2;
	ASTNode gamma; // new root
	gamma.node = "gamma";

	ASTNode lambda;
	lambda.node = "lambda";
	tempHolder.push(lambda);

	gamma.child = &tempHolder.top();
	tempHolder.top().sibling = E1;
	tempHolder.top().child = X1;
	X1->sibling = E2;
	E1->sibling = NULL;

	tempHolder.push(gamma);
	X2->sibling = &tempHolder.top();

	tempHolder.push(newRoot);

	return &tempHolder.top();

}

ASTNode * StandardizeFcnForm (ASTNode * root) {
	ASTNode *P = root-> child;
	ASTNode *V = P->sibling;
	ASTNode *Vs = V->sibling;

	ASTNode newRoot;
	newRoot.node = "=";
	newRoot.child = P;
	ASTNode lambda; // new root
	lambda.node = "lambda";


	tempHolder.push(lambda);


	P->sibling = &tempHolder.top();


	ASTNode * lambdaHolder = &tempHolder.top();


	while(Vs->sibling != NULL){
		lambdaHolder->child = V;
		ASTNode lambdaLoop;
		lambdaLoop.node = "lambda";
		tempHolder.push(lambdaLoop);
		lambdaHolder = &tempHolder.top();
		V->sibling = lambdaHolder;
		V = Vs;
		Vs = Vs->sibling;
	}
	lambdaHolder->child = V;
	V->sibling = Vs;

	tempHolder.push(newRoot);



	return &tempHolder.top();


}

ASTNode * StandardizeAnd (ASTNode * root) {

	ASTNode *eq = root->child;

	ASTNode *X = eq->child;
	ASTNode *E = X->sibling;

	ASTNode newRoot;
	newRoot.node = "=";
	ASTNode comma;
	comma.node = ",";

	ASTNode tau;
	tau.node = "tau";
	tau.t.classifier = "Tuple";

	tempHolder.push(comma);
	ASTNode * refToComma = &tempHolder.top();

	newRoot.child = &tempHolder.top();

	tempHolder.top().child = X;

	tempHolder.push(tau);
	refToComma->sibling = &tempHolder.top();
	tempHolder.top().child = E;

	eq = eq->sibling;

	int num = 1;

	while(eq != NULL){
		num++;
		X->sibling = eq->child;
		E->sibling = eq->child->sibling;
		eq = eq->sibling;
		X = X->sibling;
		E = E->sibling;
	}
	if (debug)
	cout << "Num is " + num << endl;

	tempHolder.top().t.tupleSize = num;

	X->sibling = NULL;//last X's sibling has to be null
	E->sibling = NULL;

	tempHolder.push(newRoot);

	return &tempHolder.top();

}

ASTNode * StandardizeWhere (ASTNode * root) {

	//cout << endl << ">>>>TOP OF WHERE>>>>>" << endl;
	//PrintTree (root, 0);

	ASTNode *P = root->child;
	ASTNode *eq = P->sibling;
	ASTNode *X = eq->child;
	ASTNode *E = X->sibling;

	ASTNode gamma; // new root
	gamma.node = "gamma";
	ASTNode lambda;
	lambda.node= "lambda";
	tempHolder.push(lambda);

	gamma.child = &tempHolder.top();

	tempHolder.top().sibling = E;
	tempHolder.top().child = X;
	X->sibling = P;
	P->sibling = NULL;

	tempHolder.push(gamma);

	return &tempHolder.top();

}

ASTNode * StandardizeRec (ASTNode * root) {
	ASTNode *eq = root->child;
	ASTNode *X = eq->child;
	ASTNode *E = X->sibling;

			ASTNode newRoot; // =
			newRoot.child = X;

			ASTNode copyX;
			copyX.child = X->child;
			copyX.node = X->node;
			copyX.sibling = X->sibling;
			copyX.t = X->t;

			ASTNode gamma;
			gamma.node = "gamma";

			tempHolder.push(gamma);
			ASTNode * refToGamma = &tempHolder.top();

			X->sibling = refToGamma;

			ASTNode yStar;
			yStar.node = "Y*";
			tempHolder.push(yStar);
			ASTNode * refToYStar = &tempHolder.top();

			refToGamma->child = &tempHolder.top();

			ASTNode lambda;
			lambda.node = "lambda";
			tempHolder.push(lambda);

			refToYStar->sibling = &tempHolder.top();
			ASTNode * refToLambda = &tempHolder.top();

			tempHolder.push (copyX);

			refToLambda->child = &tempHolder.top();

			tempHolder.top().sibling = E;

			tempHolder.push(newRoot);
			return &tempHolder.top();

}

ASTNode * StandardizeAt (ASTNode * root) {

	ASTNode *E1 = root->child;
	ASTNode *N = E1->sibling;
	ASTNode *E2 = N->sibling;


	ASTNode newRoot;
	newRoot.node = "gamma";
	ASTNode gammaL;
	gammaL.node = "gamma";
	tempHolder.push(gammaL);

			newRoot.child = &tempHolder.top();
			tempHolder.top().sibling = E2;
			tempHolder.top().child = N;
			N->sibling = E1;
			E1->sibling = NULL;

			tempHolder.push(newRoot);

			return &tempHolder.top();
}






ASTNode * StandardizeTree (ASTNode * root) {

	if(root == NULL){
		return NULL;
	} else {


	root -> child = StandardizeTree(root -> child);
	if(root -> sibling != NULL){
		root -> sibling = StandardizeTree(root -> sibling);
	}
	ASTNode * nextSibling = root -> sibling;

	string cl = root-> node;

	if (cl== "let")
			root = StandardizeLet(root);
	if (cl== "within")
			root = StandardizeWithin(root);
	if (cl== "function_form")
			root = StandardizeFcnForm(root);
	if (cl== "and")
			root = StandardizeAnd(root);
	if (cl== "where")
			root = StandardizeWhere(root);
	if (cl== "rec")
			root = StandardizeRec(root);
	if (cl== "@")
			root = StandardizeAt(root);

	root->sibling = nextSibling;

	/*if (cl== "let" || cl== "within" || cl== "function_form" || cl== "and" || cl== "where" || cl== "rec" || cl== "@") {
		cout << "Standardizing " << cl << endl;

		cout << endl << ">>>>>standardized:>>>>>>>>" << endl;
		PrintTree (root, 0);
		cout << endl << ">>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	}*/

	return root;
	}
}
















Token getNextToken (bool cut = true) { // false for read-only
    cleanInput();
    string value = "";
    string classifier = "";
    int length = 0;
    if (input.length() > 0 && (isalpha(input[0]) || isdigit(input[0]) || input[0] == '_')){
        // If it is a keyword, variable or number:
        classifier = "Word/Number";
        while (length < input.length() && (isalpha(input[length]) || isdigit(input[length]) || input[length] == '_')) {
            value += input[length];
            length++;
        }
    }
    else if (input.length() > 0) {
        // If it is a string:

    	if (input[0] == '"') {
    	        	classifier = "String";
    	        	value += '"';
    	        	length++;
    	            while (length < input.length() && input[length] != '"') {
    	                value += input[length];
    	                length++;
    	            }
    	            value += input[length];
    	                         length++;
    	        }
        else if (input[0] == '\'') {
        	classifier = "String";
        	value += '\'';
        	length++;
            while (length < input.length() && input[length] != '\'') {
                value += input[length];
                length++;
            }
            value += input[length];
                         length++;
        }

        // If it is punctuation:
        else if (input[0] == '(' || input[0] == ')' || input[0] == ';' || input[0] == ',') {
            classifier = "Punctuation";
            value = input[0];
            length = 1;
        }
        else // Operator
        {
            classifier = "Operator";

            value = input[0];
            length = 1;

            if (input.length() > 1 && input[0] == '-' && input[1] == '>') {
            	value = "->";
            	length = 2;
            }

            if (input.length() > 1 && input[0] == '*' && input[1] == '*') {
                        	value = "**";
                        	length = 2;
                        }

            if (input.length() > 1 && input[0] == '>' && input[1] == '=') {
                        	value = ">=";
                        	length = 2;
                        }

            if (input.length() > 1 && input[0] == '<' && input[1] == '=') {
                                    	value = "<=";
                                    	length = 2;
                                    }
             }
    } else isEmpty = true;
    if (cut)
    	input.erase(0, length);

    Token newToken;

    if (classifier == "String")
    	value = value.substr(1, value.length()-2);

    newToken.value = value;
    newToken.classifier = classifier;

    /*if (isEmpty && value == "") {
    	newToken.value = "endOfFile";
    	newToken.classifier = "endOfFile";
    }*/

    return newToken;
}

Token readNextToken () {
     return getNextToken(false);
}

void MakeTree (string nodeName, int children) {
	ASTNode newNode;
	newNode.node = nodeName;
	newNode.sibling = NULL;
	newNode.child = NULL;
	newNode.t.value = nodeName;
	newNode.t.classifier = nodeName;
	ASTNode * temp;
	temp = &newNode;
	if (debug)
		cout << endl << "Maketree called with parent: " << nodeName  << " and " << children << " children!"<< endl;

	if (nodeName == "tau") {
		newNode.t.classifier = "Tuple";
		newNode.t.tupleSize = children;
	}

	stack <ASTNode> tempStack;

	for (int i = 0; i < children; i++)
		if (!RPALstack.empty()) {
			tempStack.push(RPALstack.top());
			RPALstack.pop();
		}


	if (!tempStack.empty()) {
		holdStuff.push(tempStack.top());
		temp->child = &holdStuff.top();
		if (debug)
			cout << endl << "Child: " << tempStack.top().node;

		tempStack.pop();
		temp = temp -> child;
	}

	for (int i = 0; i < children-1; i++) {
		if (!tempStack.empty()) {
			holdStuff.push(tempStack.top());
			temp -> sibling = &holdStuff.top(); // &holder;
			if (debug)
				cout << endl << "Child: " << tempStack.top().node;

			tempStack.pop();
			temp = temp -> sibling;
		}
	}




	RPALstack.push(newNode);


	if (debug)
	cout << endl << endl;

}

void Vl (bool addOne = false) {
	if (debug)
		cout << "Vl" << ": " << readNextToken().value << endl;

	Token v = getNextToken();

			ASTNode vn;
			vn.node = v.value;
			vn.t = v;

			if (isalpha(v.value[0]))
				vn.node = "<ID:" + v.value + ">";
			else if (isdigit(v.value[0]))
				vn.node = "<INT:" + v.value + ">";

			RPALstack.push(vn);

	int counter = 0;
	while (readNextToken().value == ",")
	{
		counter++;
		getNextToken();
		Token variable = getNextToken();

				ASTNode varN;
				varN.node = variable.value;
				varN.t = variable;

				if (isalpha(variable.value[0]))
					varN.node = "<ID:" + variable.value + ">";
				else if (isdigit(variable.value[0]))
					varN.node = "<INT:" + variable.value + ">";

				RPALstack.push(varN);
	}
	int addMore = 0;
	if (addOne)
		addMore = 1;
	MakeTree (",", counter+1+addMore);

}

void E(bool origin = false) {
	if (debug)
	cout << __FUNCTION__ << ": " << readNextToken().value << endl;

    if (readNextToken().value == "let") {
		getNextToken();
    	D();
    	Token t = readNextToken();
    	if (t.value == "in") {
    		getNextToken();
    		E();
    		MakeTree("let", 2);
    	}
    	else if (debug)	cout <<  __FUNCTION__  << ": Invalid input";
    }
    else if (readNextToken().value == "fn") {
    		getNextToken();
    		int counter = 0;

    		string a = readNextToken().value;
    		while ((readNextToken().classifier == "Word/Number" || readNextToken().value == "(") && a!= "let"&& a!= "in"&& a!= "where"&& a!= "rec"&& a!= "eq"&& a!= "gr"&& a!= "ls"&& a!= "ne"&& a!= "not"&& a!= "or" && a!= "@" && a != "within" && a!= "aug" && a!= "and") {
				counter++;
				Vb();
				a = readNextToken().value;
			}

			if (counter > 0) {
	    		Token t = getNextToken();
	    		if (t.value == ".") {
	    			E();
	    			MakeTree ("lambda", counter + 1);
				}
				else if (debug)	cout <<  __FUNCTION__  << ": Invalid input";
			}
			else if (debug)	cout <<  __FUNCTION__  << ": Invalid input";
	}
    else {
    	Ew();
    	if (readNextToken().value == ";")
    		getNextToken();
	}

    if (origin && !RPALstack.empty()) {

    	ASTNode topper = RPALstack.top();

    	if (ast)
    		PrintTree(&topper, 0);

    	ASTNode * newNode = StandardizeTree(&topper);
    	PreorderFlattenTree (newNode, 0);

    	if (st)
    		PrintTree (newNode, 0);



    	if (debug)cout << "PRINTING CSE:" << endl << "----------" << endl;

    	/*for (int i = 0; i < 50 && !flattenedStacks[i].empty(); i++) {
    	    		cout << "Function " << i << endl;
    	    		while (!flattenedStacks[i].empty()) {
    	    			cout << endl << flattenedStacks[i].top()->node;
    	    			if (flattenedStacks[i].top()->node == "delta")
    	    				cout << "(" << flattenedStacks[i].top()->t.deltaPos << ")"; // << flattenedStacks[i].top->t.boundVariable << ")";
    	    			flattenedStacks[i].pop();
    	    		}
    	    		cout << endl<<"-----------" << endl;
    	    	}
*/

    	Environment ae;
    	ae.stop = true;
    	activeEnvironment = ae;
    	allEnvs[0] = activeEnvironment;
    	ae.usedVariables = 0;

    	if (!ast)
    		for (int i = 0; i < 100000; i++)
    			CSEMachine();

    }

}

void Ew () {

	if (debug)
	cout << "Ew" << ": " << readNextToken().value << endl;

	T();
	if (readNextToken().value == "where") {
		getNextToken();

		Dr();
		MakeTree ("where", 2);
	}
			if (readNextToken().value == ";")
    			getNextToken();


}

void T () {

	if (debug)
	cout << "T" << ": " << readNextToken().value << endl;

	Ta();
	int counter = 0;
	while (readNextToken().value == ",") {
		counter ++;
		getNextToken();
		Ta();
	}

	if (counter > 0)
		MakeTree("tau", counter + 1);

	if (readNextToken().value == ";")
    		getNextToken();

}

void Ta () {

	if (debug)
	cout << "Ta" << ": " << readNextToken().value << endl;

	Tc();
	int counter = 0;
	while (readNextToken().value == "aug") {
		counter++;
		getNextToken();
		Tc();
	}
	if (counter > 0)
		MakeTree("aug", counter + 1);

	if (readNextToken().value == ";")
    		getNextToken();
}

void Tc () {

	if (debug)
	cout << "Tc" << ": " << readNextToken().value << endl;

	B();

	if (readNextToken().value == "->") {
		getNextToken();
		Tc();
		if (readNextToken().value == "|") {
			getNextToken();
			Tc();
			MakeTree("->", 3);
		}
		else if (debug)	cout <<  __FUNCTION__  << ": Invalid input on " << readNextToken().value << endl;

	}

	if (readNextToken().value == ";")
    		getNextToken();

}


void B() {

	if (debug)
	cout << "B" << ": " << readNextToken().value << endl;

	// Bt (or Bt)*

	Bt();
	int counter = 0;
	while (readNextToken().value == "or") {
		counter++;
		getNextToken();
		Bt();
	}

	if (counter > 0)
		MakeTree("or", counter + 1);

	if (readNextToken().value == ";")
    		getNextToken();
}

void Bt() {

	if (debug)
	cout << "Bt" << ": " << readNextToken().value << endl;

		// Bs (and Bs)*

	Bs();
	int counter = 0;
	while (readNextToken().value == "&") {
		counter++;
		getNextToken();
		Bs();
	}

	if (counter > 0)
		MakeTree("&", counter + 1);

	if (readNextToken().value == ";")
    		getNextToken();

}

void Bs () {

	if (debug)
	cout << "Bs" << ": " << readNextToken().value << endl;

	bool wasNot = false;

	if (readNextToken().value == "not") {
			getNextToken();
			wasNot = true;
		}

		Bp();

		if (wasNot)
			MakeTree ("not", 1);

	if (readNextToken().value == ";")
    		getNextToken();
}

void Bp () {

	if (debug)
	cout << "Bp" << ": " << readNextToken().value << endl;

	bool hadOperator = false;

	A ();

	string t = readNextToken().value;

	if (t == "gr" || t == "ge" || t == "ls" || t == "le" || t == ">" || t == ">=" || t == "<" || t == "<=" || t == "ne" || t == "eq")
	{
		hadOperator = true;
		getNextToken();
		A();
	}

	if (hadOperator) {
		if (t == "gr" || t == ">")
			MakeTree ("gr", 2);
		else if (t == "ge" || t == ">=")
			MakeTree ("ge", 2);
		else if (t == "ls" || t == "<")
			MakeTree ("ls", 2);
		else if (t == "le" || t == "<=")
			MakeTree ("le", 2);
		else if (t == "eq")
			MakeTree ("eq", 2);
		else if (t == "ne")
			MakeTree ("ne", 2);

	}
	if (readNextToken().value == ";")
		getNextToken();



}

void A () {

	if (debug)
	cout << "A" << ": " << readNextToken().value << endl;

	if ((readNextToken().value == "+" || readNextToken().value == "-")) {
		if (readNextToken().value == "+") {

			getNextToken();
			At();

		}
		else {
			getNextToken();
			At();
			MakeTree ("neg", 1);
		}
	} else
	{
		At();
		if (readNextToken().value == "+" || readNextToken().value == "-") {

			bool neg = false;
			if (readNextToken().value == "-")
				neg = true;

			getNextToken();

			A();

			if (!neg)
				MakeTree ("+", 2);
			else MakeTree ("-", 2);
		}
	}

	if (readNextToken().value == ";")
    		getNextToken();
}

void At () {

	if (debug)
	cout << "At" << ": " << readNextToken().value << endl;

	Af();
	if (readNextToken().value == "*" || readNextToken().value == "/") {

		bool div = false;
		if (readNextToken().value == "/")
			div = true;

		getNextToken();

		At();

		if (!div)
			MakeTree ("*", 2);
		else MakeTree ("/", 2);
	}

	if (readNextToken().value == ";")
    		getNextToken();
}

void Af() {

	if (debug)
	cout << "Af" << ": " << readNextToken().value << endl;

	Ap();
	if (readNextToken().value == "**") {
		getNextToken();
		Af();
		MakeTree ("**", 2);
	}

	if (readNextToken().value == ";")
    		getNextToken();
}

void Ap() {

	if (debug)
	cout << "Ap" << ": " << readNextToken().value << endl;

	R();

	while (readNextToken().value == "@") {
		getNextToken();
		ASTNode vb;
		vb.node = "<ID=" + readNextToken().value + ">";
		vb.t = getNextToken();
		RPALstack.push(vb);
		R();
		MakeTree("@", 3);
	}

	if (readNextToken().value == ";")
    		getNextToken();

}

void D () {

	if (debug)
	cout << "D" << ": " << readNextToken().value << endl;

	Da();

	if (readNextToken().value == "within") {
		getNextToken();
		D();
		MakeTree ("within", 2);
	}

	if (readNextToken().value == ";")
    		getNextToken();

}

void Da () {

	if (debug)
	cout << "Da" << ": " << readNextToken().value << endl;

	Dr ();

	int counter = 0;
	while (readNextToken().value == "and" || readNextToken().value == "&") {
		counter++;
		getNextToken();
		Dr();
	}

	if (counter > 0)
		MakeTree("and", counter + 1);

	if (readNextToken().value == ";")
    		getNextToken();
}

void Dr () {

	if (debug)
	cout << "Dr" << ": " << readNextToken().value << endl;

	if (readNextToken().value == "rec") {
		getNextToken();
		Db();
		MakeTree("rec", 1);
	}
	else Db();
	if (readNextToken().value == ";")
    		getNextToken();
}

void Db () {

	if (debug)
	cout << "Db" << ": " << readNextToken().value << endl;

	if (readNextToken().value == "(") {
		getNextToken();
				D();
				if (readNextToken().value == ")") {
					getNextToken();
					if (readNextToken().value == ";")
		    			getNextToken();
				}
				else if (debug)	cout <<  __FUNCTION__  << ": Invalid input";
			}
	else {
		Token t = getNextToken();
		ASTNode tn;
		tn.node = t.value;
		tn.t = t;

				if (isalpha(t.value[0]))
					tn.node = "<ID:" + t.value + ">";
				else if (isdigit(t.value[0]))
					tn.node = "<INT:" + t.value + ">";

				if (isalpha(t.value[0]) || isdigit(t.value[0]))
					RPALstack.push(tn);

				// case of Vl

		if (readNextToken().value == "," || readNextToken().value == "=") {
			if (debug)
				cout << "Db - Vl form.";
			if (readNextToken().value == ",") {
				getNextToken();
				Vl(true);
			}
			if (readNextToken().value == "=") {
						getNextToken();
						E();
						MakeTree("=", 2);
					}
		}
		// case of Vb
		else {

			if (debug)
				cout << "Db - IDENTIFIER Vb+ form.";
			int counter = 0;
			string a = readNextToken().value;
			    		while ((readNextToken().classifier == "Word/Number" || readNextToken().value == "(") && a!= "let"&& a!= "in"&& a!= "where"&& a!= "rec"&& a!= "eq"&& a!= "gr"&& a!= "ls"&& a!= "ne"&& a!= "not"&& a!= "or" && a!= "@" && a != "within" && a!= "aug") {
							counter++;
							Vb();
							a = readNextToken().value;
						}

			if (readNextToken().value == "=") {
				getNextToken();
				E();
				MakeTree("function_form", counter+2);
			} else if (debug) cout << "Goofed in VB!";

		}
	}


}

void R() {

	if (debug)
	cout << "R" << ": " << readNextToken().value << endl;

	Rn();

	string a = readNextToken().value;


	while ((readNextToken().classifier == "String" || readNextToken().classifier == "Word/Number" || readNextToken().value == "(") && a!= "let"&& a!= "in"&& a!= "where"&& a!= "rec"&& a!= "eq"&& a!= "gr"&& a!= "ls"&& a!= "ne"&& a!= "not"&& a!= "or" && a!= "@" && a != "within" && a!= "aug" && a!= "and")
	{
		Rn();
		MakeTree("gamma", 2);
		a = readNextToken().value;
	}

	if (readNextToken().value == ";")
    			getNextToken();

}


void Rn() {

	if (debug)
	cout << "Rn" << ": " << readNextToken().value << " " << readNextToken().classifier << endl;

	int counter = 0;

	string a = readNextToken().value;

	if ((readNextToken().classifier == "String" || readNextToken().classifier == "Word/Number" || readNextToken().value == "(") && a!= "let"&& a!= "in"&& a!= "where"&& a!= "rec"&& a!= "eq"&& a!= "gr"&& a!= "ls"&& a!= "ne"&& a!= "not"&& a!= "or" && a!= "@" && a!= "within" && a!= "aug"&& a!= "and") {

	counter++;
	Token rand = getNextToken ();

	if (rand.classifier == "Word/Number" || rand.classifier == "String") {
		ASTNode randNode;
		randNode.node = rand.value;
		randNode.t = rand;

		if (isalpha(rand.value[0]))
			randNode.node = "<ID:" + rand.value + ">";
		else if (isdigit(rand.value[0]))
			randNode.node = "<INT:" + rand.value + ">";
		else randNode.node = "<STRING:" + rand.value + ">";


		if (rand.value == "true" || rand.value == "false" || rand.value == "nil" || rand.value == "dummy")
			randNode.node = "<" + rand.value + ">";
			//MakeTree ("<" + rand.value + ">", 1);
			RPALstack.push(randNode);
	}

	else if (rand.value == "(") {
		//stopCond = true;
		E();
		if (readNextToken().value == ")")
			getNextToken();

		else if (debug)	cout <<  __FUNCTION__  << ": Invalid input on " << rand.value;
	}
	else if (debug)	cout <<  __FUNCTION__  << ": Invalid input on " << rand.value;

	a = readNextToken().value;

	}

}


void Vb () {
	if (debug)
	cout << "Vb" << ": " << readNextToken().value << endl;

	Token openBrace = readNextToken();
	if (readNextToken().value != "(") {
		Token variable = getNextToken();
		if (variable.classifier != "Word/Number") {
			if (debug)
			cout << "Invalid input: " + variable.value + " is a " + variable.classifier;
		}
		else {

		ASTNode varN;
		varN.node = variable.value;
		varN.t = variable;

		if (isalpha(variable.value[0]))
			varN.node = "<ID:" + variable.value + ">";
		else if (isdigit(variable.value[0]))
			varN.node = "<INT:" + variable.value + ">";

		RPALstack.push(varN);
		}
	}

	else {
		getNextToken();

		if (readNextToken().value != ")") {
			Vl();
			if (readNextToken().value == ")")
				getNextToken();
			else if (debug) cout << "Invalid input: " + readNextToken().value + " is a " + readNextToken().classifier;
		}
		else {
			ASTNode openBNode;
			openBNode.node = "(";
			RPALstack.push(openBNode);
			getNextToken();
			ASTNode closeBNode;
			closeBNode.node = ")";
			RPALstack.push(closeBNode);
			MakeTree("()", 2);
		}

	}

}

int main(int argc, char *argv[]) {

debug = false;
/*
	debug = false;
	st = true;

	 std::ifstream ifs("C:\\rpal\\tests\\fn2"); //"C:\\rpal\\tests\\fn2"
	  	std::string content( (std::istreambuf_iterator<char>(ifs) ),
	                       (std::istreambuf_iterator<char>()    ) );
	  	input = content;

	  	cout << input;
	  	//ast = true;
	  	//st = true;
	  	E(true);
*/



	if (argc > 1) {

		string a1 = argv[1];
			string a2 = "";
			if (argc > 2)
				a2 = argv[2];
			string a3 = "";
			if (argc > 3)
				a3 = argv[3];

			string fileName = "";
				if (argc == 2)
					fileName = argv[1];
				else fileName = argv[argc-1];


	if (fileName != "") {
    std::ifstream ifs(fileName.c_str()); //"C:\\Users\\Ankur\\workspace\\RPAL\\Debug\\tiny.1"
  	std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
  	input = content;

	if (a1 == "-debug" || a2 == "-debug" || a3 == "-debug")
		debug = true;

	if (a1 == "-ast" || a2 == "-ast" || a3 == "-ast") {
	ast = true;
	}
	if (a1 == "-st" || a2 == "-st" || a3 == "-st") {
	st = true;
	}

	if (a1 == "-l" || a2 == "-l"  || a3 == "-l")
			cout << content << endl;

	}else cout << "No file";

	E(true);
	}

    return 0;
}
