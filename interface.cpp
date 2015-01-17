
#include <iostream>     //including I/O support
#include <conio.h>		//including support to use getch()
#include <fstream>      //including file I/O support
#include <vector>		//including support of vector container
#include <string>		//including the new C++ string class support
#include <cstring>		//including old C character array support

						//	to read one char at a time from user input

#include "interface.h"	//
#include "lexScanner.h"
#include "expEvaluator.h"
#include "Interpreter.h"
using namespace std;
ofstream fout;
ifstream fin;

// 1. Read in a program (lines of statements) from a file
void Interface::loadProgramFromFile()
{
	char fileName[MaxLengthFileName];	
	// this char array stores the name of the file the program reads from.

	cout<<"Name of file to upload program from (ex. file.txt): ";
	cin >> fileName;
	while( strlen(fileName) == 0 )
	{
		cin >> fileName;
	}

	ifstream inFile;
	inFile.open( fileName );
	if ( inFile.fail() )
	{ cout << endl << "Cannot open this file!" << endl;
	return;
	}

	lines.clear();  //Delete all the lines of statements
	string inputBuffer;
	while( !inFile.eof() )
	{
		getline(inFile, inputBuffer);
		while( !inFile.eof() && inputBuffer.size() == 0 )
		{
			getline(inFile, inputBuffer);
		}

		if (!inFile.eof())
		{
  			cout<< "Line["
				<< lines.size()
				<< "] read:"
				<< inputBuffer << endl;
			lines.push_back(inputBuffer);
		}

	}

	inFile.close();
}



// 2. Display the lines of statements
void Interface::displayProgram()
{
	for (int i = 0; i < lines.size(); i++)
		cout << "Line[" << i << "] " << lines[i] << endl;
}



// 3. Enter new lines of code to the end of the program
void Interface::appendLines()
{
	string appendTxt;
	cout << "What would you like to append to the end of the program? (Enter \".\" to stop)" << endl;
	while (appendTxt != ".")
	{
		appendTxt.clear();

		while (appendTxt.length() == 0)
			getline(cin, appendTxt);
		if (appendTxt != ".")
			lines.push_back(appendTxt);
		else
			;
	}
}


// 4. Insert new lines after an existing line
void Interface::insertLines()
{	
	string appendTxt = "";
	int lineNumber=-1;
	vector<string>::iterator it;
	if (lines.size() == 0)
		cout << "Try appending since your vector is empty still" << endl;
	else
	{
		while (lineNumber < 0 || lineNumber >= lines.size())
		{
			cout << "Which line of code would you like to place this before?" << endl;
			cin >> lineNumber;
		}
		
		cout << "What would you like to insert into the program? (enter \".\" to exit)" << endl;
		
		for (int i = 0; appendTxt != "."; i++)
		{
			it = lines.begin() + lineNumber+i; //tells where to append
			appendTxt.clear();
			while (appendTxt.length() == 0)
				getline(cin, appendTxt);
			if (appendTxt != ".")
				lines.insert(it, appendTxt);
			else
				;
		}



	}
}


// 5. Delete a range of existing line
void Interface::deleteLines()
{
	int lineNumber1=-1;
	int lineNumber2 = -1;
	char choice = 'm';
	while (!(choice == 's' || choice == 'S' || choice == 'R' || choice == 'r'))
	{
		cout << "Would you like to delete a single line or range of lines? (S or R)" << endl;
		cin >> choice;
	}
	//single line deletion
	if (choice == 's' || choice == 'S')
	{
		cout << "Which line would you like to delete?" << endl;
		while (!(lineNumber1 < lines.size() && lineNumber1 >= 0))
			cin >> lineNumber1;
		lines.erase(lines.begin() + lineNumber1);
	}
	//mulitiple line deletion
	else if (choice == 'r' || choice == 'R')
	{
		//start line
		cout << "Which line would you like to start deleting at?" << endl;
		while (!(lineNumber1 < lines.size()-1 && lineNumber1 >= 0))
			cin >> lineNumber1;
		//end line
		cout << "Which line would you like to end deleting at?" << endl;
		while (!(lineNumber2 < lines.size() && lineNumber2 > lineNumber1))
			cin >> lineNumber2;
		//delete lines
		for (int i = lineNumber1; i <= lineNumber2; i++)
		{
			lines.erase(lines.begin() +lineNumber1);
		}
	}
}


// 6. Replace the contents of an existing line
void Interface::replaceOneLine()
{
	int lineNumber = -1;
	string newLine;

	cout << "Which line would you like to replace?" << endl;
	while (!(lineNumber < lines.size() && lineNumber >= 0))
		cin >> lineNumber;
	while (newLine == "." || newLine.size() < 1)
	{
		cout << "What would you like line " << lineNumber << " to read?" << endl;
		cout << "It currently reads: " << lines[lineNumber] << endl;
		getline(cin, newLine);
	}

	lines[lineNumber] = newLine;
}


// 7. Save program to a file
void Interface::saveProgramIntoFile()
{
	string fileName;
	cout << "What would you like to call the file?" << endl;
	
	cin >> fileName;
	
	//open file and write out individual lines
	fout.open(fileName);
	for (int i = 0; i < lines.size(); i++)
		fout << lines[i] << endl;
	fout.close();
}

//An auxilliary message displaying function
void Interface::endOfService(const string service)
{
	cout<<endl << service << " done." << endl << "Press any key to continue.";
	_getch();
	cout << endl;
	cout << "****************************************************" <<endl<< endl;

}

// This menu() function is the interface
void Interface::startInterface() {

	// "Then I suppose now is the time for me to say something profound....
	// Nothing comes to mind. Let's do it."

	bool inMenu = true;	// while the user is using the program this var is set to true and the menu keeps looping.
						// when the user wants to quit, it is set to false and the loop ends.
	char keyPressed;	// This var stores which menu selection the user picks.

	while ( inMenu )
	{
		cout<< endl << endl << endl;
		cout << "****************************************************" << endl;
		cout  <<"**  MENU:(press a character to select an option)  **" << endl;
 		cout << "****************************************************" << endl;
		cout<<"Q. [QUIT]     Quit."<<endl;
		cout<<"L. [LOAD}     Read in a program (lines of statements) from a file"<<endl;
  		cout<<"S. [SAVE]     Save lines of statement as a program into a file"<<endl;
		cout << endl;

		cout<<"C. [CODE DISPLAY]  Display the source code as lines of statements"<<endl;
		cout << endl;

		cout << "N. [INTERPRETER]   Executes statements with \"read\" \"display\" and assignment operators" << endl;

		cout << "F. [INFIX-TO-POSTFIX]   Turn any infix expression into postfix" << endl;
		cout << "U. [CALCULATOR]   Calculates and evaluates whatever arithmetic "
			<< "or logic expression involving numerical constants, parentheses, and arithmetic or logical operators" << endl;

		cout<<"A. [APPEND]   Append new lines to the end of the program"<<endl;
		cout<<"I. [INSERT}   Insert new lines before an existent line"<<endl;
		cout<<"D. [DELETE]   Delete a range of existent lines"<<endl;
		cout<<"R. [REPLACE]  Replace the contents of an existent line"<<endl;
		cout << endl;
		cout << "X. [Lexical]	Get lexical info" << endl;
		cout<<"P. [PARSE]    Parse and ident the code" << endl;
		cout<<"E. [EXECUTE]  Execute (run) the program"<<endl;
		cout<<"T. [TOGGLE]   Toggle the execution debug mode"<<endl;

		cout << "****************************************************" <<endl<< endl;

		cout<<"Your choice is: ";
		keyPressed = _getch();
		cout<<keyPressed << endl;

		switch(keyPressed) {

		case 'N': case 'n':
		{
					 Interpreter:: executeProgram(lines);
					 cout << "Press any key and enter to continue" << endl;
					 char input;
					 cin >> input;

		}
			break;
		case 'U': case 'u':
		{
					  string rawExpressionString;
					  expVector infixExp, postfixExp;

					  floatVarValueTable varTable;

					

					  perLineTokenVector tokenExpression;
					  rawExpressionString.clear();
					  while (rawExpressionString != ".")
					  {
						  float expValue;
						  cout << "The following is the current list of values of the variables" << endl;
						  for (floatVarValueTable::iterator i = varTable.begin();
							  i != varTable.end();
							  ++i
							  )
						  {
							  cout << "[" << (*i).first << "," << (*i).second << "]" << endl;
						  }
						  cout << "Give me an infix expression involving numerical constants, parentheses, and arithmetic"
							  << " or logical operators."
							  << endl << "Type in a . to quit" << endl << endl;
						  getline(cin, rawExpressionString);
						  LexicalScanner::getPerLineTokenVectFromOneStringObject
							  (rawExpressionString,
							  tokenExpression
							  );

						  if (ExpressionEvaluator::infixEvaluator(tokenExpression, varTable, expValue))
							  cout << "\n\nThe value of " << rawExpressionString << " is " << expValue << endl<<endl;
						  else
							  cout << rawExpressionString << " not a valid expression" << endl;
					  }
		}
			break;

		case 'F': case 'f':
		{
					  cout << "******************************************************************" << endl;
					  cout << "Infix-to-Postfix conversion and " << endl;
					  cout << "******************************************************************" << endl;

					  string rawExpressionString;
					  expVector infixExp, postfixExp;

					  while (rawExpressionString != ".")
					  {
						  float expValue;
						  cout << endl << "Type in an expression of a single period . to quit" << endl << endl;
						  getline(cin, rawExpressionString);
						  LexicalScanner::getPerLineTokenVectFromOneStringObject
							  (rawExpressionString,
							  infixExp
							  );

						  cout << endl
							  << "The infix expression:" << rawExpressionString << endl
							  << "# of tokens in it:" << infixExp.size() << endl
							  << "Token of this infix expression:" << endl;

						  for (expVector::iterator pos = infixExp.begin();
							  pos != infixExp.end();
							  pos++
							  )
							  cout << '\t' << *pos << endl;

						  cout << endl << endl;

						  ExpressionEvaluator::infixToPostfixConversion(infixExp, postfixExp);

						  cout << "# of tokens in the corresponding postfix expression:"
							  << postfixExp.size() << endl
							  << "Tokens of this postfix expression:" << endl;
						  for (expVector::iterator pos = postfixExp.begin();
							  pos != postfixExp.end();
							  pos++
							  )
							  cout << '\t' << *pos << endl;

					  }


		}
			break;
		case 'X': case 'x':
		{
					  vectOfTokenVects myVectOfTokenVects;
					  vectOfCategoryVects myVectOfCategoryVects;


					  //Get the lexical information 
					  LexicalScanner::getLexicalInfo(
						  lines,
						  myVectOfTokenVects,
						  myVectOfCategoryVects);

					  //Display the lexical information 
					  LexicalScanner::displayLexicalInfo(
						  myVectOfTokenVects,
						  myVectOfCategoryVects);

					  cout << "Press any key to continue ";
					  char quit;
					  quit=_getch();

		}
			break;
		case 'Q': case 'q'://Quit
			cout << "[QUIT]:" << endl;
			inMenu = false;
			break;

		case 'L': case 'l':// 1. Read in a program (lines of statements) from a file
 			cout << "[LOAD PROGRAM]:" << endl;
			loadProgramFromFile();
			endOfService("[LOAD PROGRAM]");
			break;

		case 'C': case 'c':// 2. Display the lines of statements
			cout << "[CODE DISPLAY]:" << endl;
			displayProgram();
			endOfService("[CODE DISPLAY]");
			break;

		case 'A': case 'a':// 3. Append new lines of code to the end
			cout << "[APPEND]:" << endl;
			appendLines();
			endOfService("[APPEND LINES]");
			break;

		case 'I': case 'i':// 4. Insert a new line after an existing line
			cout << "[INSERT LINES]:" << endl;
			insertLines();
			endOfService("[INSERT LINES]");
			break;

		case 'D': case 'd':// 5. Delete an existing line
			cout << "[DELETE LINES]:" << endl;
			deleteLines();
			endOfService("[DELETE LINES]");
			break;

		case 'R': case 'r':// 6. Replace the contents of an existing line
			cout << "[REPLACE]:" << endl;
			replaceOneLine();
			endOfService("[REPLACE ONE LINE]");
			break;

		case 'S': case 's':// 7. Save program to a file
			cout << "[SAVE]:" << endl;
			saveProgramIntoFile();
			endOfService("[SAVE CODE INTO A FILE]");
			break;

		case 'P': case 'p':// 8. Parse and Indent the program
			cout << "[PARSE AND INDENT SOURCE CODE]:" << endl;
			endOfService("[PARSE AND INDENT SOURCE CODE]");
			break;

		case 'E': case 'e':// 9. Run program
			cout << "[EXECUTE TO RUN PROGRAM]:" << endl;
			endOfService("[EXECUTE TO RUN PROGRAM]");
			break;

		case 'T': case 't':// Toggle the debug mode
			cout << "[TOGGLE the debug mode]:" << endl;
			break;


		default:
			cout << "[?? UNKNOWN COMMAND]:" << endl;
			// Unrecognized key, do nothing
			break;
		}

	} // End of while loop.
}
