
#include "LLDriver.h"
#include "grammarAnalyzer.h"
#include "sets.h"
#include "scanner.h"
#include "scannerDriver.h"
#include <iomanip>
#include <sstream>
#include <locale>
#include <string>
#include <stack>
using namespace std;

extern std::stack<string> myStack;
extern Grammar g;
extern std::vector<TermSet> firstSet;
extern std::vector<TermSet> followSet;
extern MarkedVocabulary myVocab;
extern std::vector<std::vector<string> > tableVect;

extern int tokenCode;
extern string tokenText;

int LLDriver()
{
	string currentThing;

	int myState = 0;

	cout << setw(20) << left <<"PARSE ACTION" << setw(80) << "REMAINING INPUT" << "PARSE STACK" << endl;
	std::vector<string> remainingInput;
	do
	{
		remainingInput.push_back(scanner());
	}while(remainingInput[remainingInput.size()-1] != "$");


	cout << endl;

	myStack.push(g.startSymbol);

	scannerDriver(tokenCode, tokenText);
	currentThing = tokenText;

	while(myStack.size() != 0)
	{
		if(myStack.top() == "(<expression>)")
		{
			myStack.pop();
			myStack.push("<expression>");
		}
		if (existsIn(myStack.top(), g.nonTerminalsVect))
		{
			if (T(myStack.top(), currentThing) != -1)
			{
				myState = T(myStack.top(), currentThing);
				stringstream ss;
				ss << myState;
				string stateOut = "Predict " + ss.str();
				cout << setw(20) << left;
				cout << stateOut;//"predict # is " << myState << endl;
				cout << setw(80);
				display(remainingInput);
				displayStack(myStack);
				cout << endl;
				int index = myState - 1;


				myStack.pop();
				std::vector<string> RHSBroken;
				RHSBroken = getRealProductions(g.RHS[index]);
				for(int i = (int)RHSBroken.size()-1; i >= 0; --i)
				{
					myStack.push(RHSBroken[i]);
				}
			}
			else
			{
				if(currentThing == "(")
				{
					myStack.push("(");
					//scannerDriver(tokenCode, tokenText);
					//currentThing = tokenText;
					continue;
				}
				else
				{
					cout << "syntax error on nonTerminal" << endl;
					return -1;
				}


				//scannerDriver(tokenCode, tokenText);
				//currentThing = tokenText;
			}
		}
		else		//meaning X is in terminals
		{
			if((myStack.top() == ";" || myStack.top() == "λ") && (myStack.top() != currentThing))
			{
				myStack.pop();
				continue;
			}
			if(myStack.top() == "(<expression>)")
			{
				if(myState == 18)
				{
					myStack.pop();
					myStack.push("Id");
				}
				else if(myState == 19)
				{
					myStack.pop();
					myStack.push("IntLiteral");
				}
			}
			else if(myStack.top() == "<primary>")
			{
				if(myState == 18)
				{
					myStack.pop();
					myStack.push("Id");
				}
				else if(myState == 19)
				{
					myStack.pop();
					myStack.push("IntLiteral");
				}
			}
			else if(myStack.top() == "READ" || myStack.top() == "read" || myStack.top() == "Read")
			{
				cout << "in here\n";
				//myStack.pop();
				//myStack.push("read");

			}


			if(myStack.top() == currentThing || (currentThing == ";" && myStack.top() == "λ"))
			{
				scannerDriver(tokenCode, tokenText);
				currentThing = tokenText;
				cout <<setw(20) << left<< "match";
				cout << setw(80);
				display(remainingInput);
				displayStack(myStack);
				cout << endl;
				elimFirst(remainingInput);
				myStack.pop();
			}
			else
			{
				cout << "currentThing is " << currentThing << " mystack top is " << myStack.top() << endl;
				cout << "\n\n in terminal syntax error \n\n";
				return -1;
			}
		}
	}
	return 0;
}

void elimFirst(std::vector<string>& vect)
{
	vect.erase(vect.begin());//should eliminate first thing in the vector
}

void display(std::vector<string>& vect)
{
	string concat;
	for(int i = 0; i < vect.size(); ++i)
	{
		concat += vect[i];
	}
	cout << concat;
}

void displayStack(std::stack<string> thisStack)
{
	string concat;
	for(int i = 0; i <= thisStack.size(); ++i)
	{
		string temp = thisStack.top();
		if(temp == "+")
		{
			temp = "PlusOp";
		}
		else if(temp == "-")
		{
			temp = "MinusOp";
		}
		else if(temp == "Id" || temp == ";")
		{
			temp = temp;
		}
		else if(temp == ":=")
		{
			temp = "AssignOp";
		}
		else if(temp == "end")
		{
			temp = "EndSym";
		}
		else if(temp == "begin")
		{
			temp = "BeginSym";
		}
			else if(temp == "(")
		{
			temp = "LParen";
		}
			else if(temp == "read")
		{
			temp = "ReadSym";
		}
		else
		{
			if(temp[0] != '<' && temp != "$" && temp != "read")
			{
				temp = "IntLiteral";
			}
		}
		concat += temp;
		thisStack.pop();
	}
	cout << concat;

}

void displayLine(int parseAction, std::vector<string> remInput, std::stack<string> myStack) {
	if (parseAction == -1) {
		cout << setw(30) << left << "match";
	}
	else
	{
		cout << setw(30) << left << parseAction;
	}
	//cout << setw(30);
	display(remInput);
	cout << setw(70) << right;
	displayStack(myStack);
	cout << endl;
}

std::vector<string> getRealProductions(string rhs)
{
	stringstream ss (rhs);
	std::vector<string>rhsBrokenUp;

	string temp;

	while(ss >> temp)
	{
		if(temp[0] == '<' && temp[temp.size()-1] != '>')
		{
			string tempThing;
			ss >> tempThing;
			temp += " ";
			temp += tempThing;
		}
		if((temp.compare(0, 4, "read") == 0))
		{
			temp = "read";
			string userT;
			getline(ss, userT, ')');
			ss.get();
		}
		else if (temp.compare(0, 5, "write") == 0)
		{
			temp = "write";
			string userT;
			getline(ss, userT, ')');
			ss.get();
		}
		else if(temp[0] == '(')
		{
			cout << "temp is " << temp << endl;
			/*
			temp = "(";
			rhsBrokenUp.push_back("(");
			string userT;
			getline(ss, userT, ')');
			int count = 1;
			while(temp[count] != ')')
			{
				userT += temp[count];
				count++;
			}
			cout << "userT is " << userT << endl;
			rhsBrokenUp.push_back(userT);

			//rhsBrokenUp.push_back(temp);
			temp = ")";
			 */

		}
		rhsBrokenUp.push_back(temp);
	}

	return rhsBrokenUp;
}