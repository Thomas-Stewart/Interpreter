

#include <iostream>     //including I/O support
#include <conio.h>		//including support to use getch()
#include <fstream>      //including file I/O support
#include <vector>		//including support of vector container
#include <string>		//including the new C++ string class support
#include <cstring>		//including old C character array support
#include <map>
#include <iterator>


//	to read one char at a time from user input

#include "interface.h"	//
#include "lexScanner.h"
#include "expEvaluator.h"
#include "Interpreter.h"
using namespace std;

typedef map<string, float> StringFloatMap;

void Interpreter::executeProgram(vector< string > sourceProgram)
{
	//declare vectors
	vectOfTokenVects myVectOfTokenVects;
	vectOfCategoryVects myVectOfCategoryVects;

	//declare map
	StringFloatMap map;

	//get lexical info and store strings into vect of tokens
	LexicalScanner::getLexicalInfo(sourceProgram, myVectOfTokenVects, myVectOfCategoryVects);


	//*****************************************************************************************************
	//stacks
	vector<int> stackLineNumbers; //line numbers
	perLineTokenVector stackKeyWords; //keywords
	int leftBraceLine; //line where "{" is found
	int rightBraceLine; //where "}" is found

	//controlling where the program picks up, either in the brackets, or past them
	int inBracketStart;
	bool insideBracket=false;

	//keep track of nested while loops
	vectOfTokenVects stackOfWhileExpressions;

	//tells if assignmnet operator is actually a relational operator
	bool isRelOp = false;

	bool goToElse = false; //if program should run else brackets after if


	bool oneLinedWhile = false; //if we are assessing if while expression is still true w/o brackets

	
	//loop for deleting spaces
	for (int q = 0; q < myVectOfCategoryVects.size(); q++)
	{
		if (myVectOfCategoryVects[q].size() == 0)
		{
			myVectOfCategoryVects.erase(myVectOfCategoryVects.begin() + q);
			myVectOfTokenVects.erase(myVectOfTokenVects.begin() + q);
			sourceProgram.erase(sourceProgram.begin() + q);
			q--;
		}

		else if ((sourceProgram[q].find_first_not_of(' ') == std::string::npos)) //deletes if not empty but just spaces
		{
			myVectOfCategoryVects.erase(myVectOfCategoryVects.begin() + q);
			myVectOfTokenVects.erase(myVectOfTokenVects.begin() + q);
			sourceProgram.erase(sourceProgram.begin() + q);
			q--;
		}

	}

	//deletes spaces before actual characters
	for (int r = 0; r < myVectOfCategoryVects.size(); r++)
	{
		sourceProgram[r] = sourceProgram[r].substr(sourceProgram[r].find_first_not_of(" "), string::npos);
		for (int t = 0; t < myVectOfCategoryVects[r].size(); t++)
		{
			myVectOfTokenVects[r][t] = myVectOfTokenVects[r][t].substr(myVectOfTokenVects[r][t].find_first_not_of(" "), string::npos);
		}
	}

	//main loops for cycling through lines of code
	for (int i = 0; i < myVectOfCategoryVects.size(); i++)
	{
		for (int j = 0; j < myVectOfCategoryVects[i].size(); j++)
		{

			if (oneLinedWhile == true)
			{
				if (stackLineNumbers.back() == i - 2)
				{
					i = stackLineNumbers.back();
					oneLinedWhile = false;
					stackKeyWords.pop_back();
					stackLineNumbers.pop_back();
				}
			}

			//check for if, else, while
			if (myVectOfCategoryVects[i][j] == LEFT_CURLYBRACE)
			{
				leftBraceLine = i; //line where "{"is found

				//if "if" expression is true
				if (myVectOfTokenVects[i - 1][j] == "if")
				{

					//check if expression is true after "if"
					string expression;
					for (int j = 0; j < myVectOfTokenVects[i - 1].size(); j++)
					{
						expression = expression + myVectOfTokenVects[i - 1][j];
					}
					int pos1; //position 1 of left parenthesis
					int pos2; //position 2 of right parenthesis

					pos1 = expression.find("("); //find left parenthesis
					pos2 = expression.rfind(")"); //find left parenthesis
					
					//loop for cutting string into just the expression
					int counter = 0;
					


					//********if expression is true or false*****************for (int i = pos1; i <= pos2; i++)
					{
						if (i==pos2)
							expression = expression.substr(pos1 + 1, counter-1); //check if +1 and -1 are correct
						counter++;
					}
					perLineTokenVector tokenExpression2;
					float expValue2;
					LexicalScanner::getPerLineTokenVectFromOneStringObject(expression, tokenExpression2); //find float value and store it in expValue


					ExpressionEvaluator::infixEvaluator(tokenExpression2, map, expValue2); //if the expression is true
					if (expValue2 == 1)
					{
						goToElse = false;
						stackLineNumbers.push_back(leftBraceLine - 1); //line num: pos of keyword
						stackKeyWords.push_back(myVectOfTokenVects[i - 1][j]); //stores keyword in stack
						inBracketStart = leftBraceLine;//do not add 1
						insideBracket = true;
						i = stackLineNumbers.back()+1;
						break;
					}

					else //if the expression is false
					{
						goToElse = true;
						if (stackLineNumbers.size() > 1)
							;
						else
							insideBracket = false;
						//I don't think this error check needs to be here
						/*if (stackLineNumbers.size() == 0)
						{
							cout << endl << "SYNTAX ERROR: Curly braces unbalanced" << endl;
							break;
						}*/
						
						
							int k;
							for (k = i; myVectOfCategoryVects[k][0] != RIGHT_CURLYBRACE; k++)
							{
								if (myVectOfCategoryVects[k][0] == RIGHT_CURLYBRACE)
									inBracketStart = k;
							}
							if (myVectOfCategoryVects[k][0] == RIGHT_CURLYBRACE)
								inBracketStart = k;
							i = inBracketStart;
							break;
						
					}
				}

				else if (myVectOfTokenVects[i - 1][j] == "else")
				{
					if (goToElse == true)
					{
						stackLineNumbers.push_back(leftBraceLine - 1); //line num: pos of keyword
						stackKeyWords.push_back(myVectOfTokenVects[i - 1][j]); //stores keyword in stack
						inBracketStart = leftBraceLine;//do not add 1
						insideBracket = true;
						i = stackLineNumbers.back()+1;
						break;
					}

					else //find the right bracket and start from there
					{
						int k;
						for (k = i; myVectOfCategoryVects[k][0] != RIGHT_CURLYBRACE; k++)
						{
							if (myVectOfCategoryVects[k][0] == RIGHT_CURLYBRACE)
								inBracketStart = k;
						}
						if (myVectOfCategoryVects[k][0] == RIGHT_CURLYBRACE)
							inBracketStart = k;
						i = inBracketStart;
						break;
					}
				}

				else if (myVectOfTokenVects[i - 1][j] == "while")
				{
					//while stuff





					//check if expression is true after "while"
					string whileExpression;
					for (int j = 0; j < myVectOfTokenVects[i - 1].size(); j++)
					{
						whileExpression = whileExpression + myVectOfTokenVects[i - 1][j];
					}
					int wpos1; //position 1 of left parenthesis
					int wpos2; //position 2 of right parenthesis

					wpos1 = whileExpression.find("("); //find left parenthesis
					wpos2 = whileExpression.find(")"); //find left parenthesis

					//loop for cutting string into just the expression
					int wcounter = 0;
					for (int z =wpos1; z <= wpos2; z++)
					{
						if (z == wpos2)
							whileExpression = whileExpression.substr(wpos1 + 1, wcounter - 1); //check if +1 and -1 are correct
						wcounter++;
					}


					//********if expression is true or false*****************
					perLineTokenVector tokenExpression3;
					float expValue3;
					LexicalScanner::getPerLineTokenVectFromOneStringObject(whileExpression, tokenExpression3); //find float value and store it in expValue

					//put expression into stack
					stackOfWhileExpressions.push_back(tokenExpression3);

					ExpressionEvaluator::infixEvaluator(tokenExpression3, map, expValue3); //if the expression is true
					if (expValue3 == 1)
					{
						stackLineNumbers.push_back(leftBraceLine - 1); //line num: pos of keyword
						stackKeyWords.push_back(myVectOfTokenVects[i - 1][j]); //stores keyword in stack
						inBracketStart = leftBraceLine;//do not add 1;
						insideBracket = true;
						i = stackLineNumbers.back()+1;
						break;
					}

					else //if the expression is false
					{
						if (stackLineNumbers.size() > 1)
							;
						else
							insideBracket = false;
						//I don't think I need this check
						/*if (stackLineNumbers.size() == 0)
						{
							cout << endl << "SYNTAX ERROR: Curly braces unbalanced" << endl;
							break;
						}*/
						//else
						//{
							int k;
							for (k = i; myVectOfCategoryVects[k][0] != RIGHT_CURLYBRACE; k++)
							{
								if (myVectOfCategoryVects[k][0] == RIGHT_CURLYBRACE)
									inBracketStart = k;
							}
							if (myVectOfCategoryVects[k][0] == RIGHT_CURLYBRACE)
								inBracketStart = k;
							i = inBracketStart;
							break;
						//}
					}






				}
			}

			//**************changes where we are in the loop, maybe skips brackets
			

			else if (myVectOfCategoryVects[i][j] == RIGHT_CURLYBRACE)
			{
				if (insideBracket)
				{
					if (stackLineNumbers.size() == 0)
						cout << endl << "SYNTAX ERROR: Curly braces unbalanced" << endl;
					else
					{


						if (stackKeyWords.back() == "while") //while stuff
						{
							//reevalueate while expression
							float expValue4;
							perLineTokenVector tempPLTV = stackOfWhileExpressions.back();
							ExpressionEvaluator::infixEvaluator(tempPLTV, map, expValue4); //if the expression is true

							if (expValue4 == 1) //if loop expression is still true
							{
								i = stackLineNumbers.back()+1; //resets to run while loop again
							}
							else
							{
								if (stackLineNumbers.size() > 1)
									;
								else
									insideBracket = false;
								stackOfWhileExpressions.pop_back();
								rightBraceLine = i; //line where "}"is found
								stackLineNumbers.pop_back();
								stackKeyWords.pop_back();
							

							}
						}

						else
						{

							rightBraceLine = i; //line where "}"is found
							if (stackLineNumbers.size() > 1)
								;
							else
								insideBracket = false;
							stackLineNumbers.pop_back();
							stackKeyWords.pop_back();
							
						}
						
						
						
					}
				}
			}
			
			else if (myVectOfCategoryVects[i][j] == KEYWORD)
			{

				if (myVectOfTokenVects[i][j] == "if")
				{
					if (myVectOfTokenVects[i + 1][j] == "{")
						; //do nothing
					else
					{



						//check if expression is true after "if"
						string expression;
						for (int j = 0; j < myVectOfTokenVects[i].size(); j++)
						{
							expression = expression + myVectOfTokenVects[i][j];
						}
						int pos1; //position 1 of left parenthesis
						int pos2; //position 2 of right parenthesis

						pos1 = expression.find("("); //find left parenthesis
						pos2 = expression.rfind(")"); //find left parenthesis

						//loop for cutting string into just the expression
						int counter = 0;



						//********if expression is true or false*****************for (int i = pos1; i <= pos2; i++)
						{
							if (i == pos2)
								expression = expression.substr(pos1 + 1, counter - 1); //check if +1 and -1 are correct
							counter++;
						}
						perLineTokenVector tokenExpression2;
						float expValue2;
						LexicalScanner::getPerLineTokenVectFromOneStringObject(expression, tokenExpression2); //find float value and store it in expValue


						ExpressionEvaluator::infixEvaluator(tokenExpression2, map, expValue2); //if the expression is true
						if (expValue2 == 1)
						{
							goToElse = false;
							stackLineNumbers.push_back(i); //line num: pos of keyword
							stackKeyWords.push_back(myVectOfTokenVects[i][j]); //stores keyword in stack
							i = stackLineNumbers.back();
							break;
						}

						else //if the expression is false
						{
							goToElse = true;
							i = i + 1;
							j = myVectOfCategoryVects[i].size() - 1;
						}




					}
				}
			


				else if (myVectOfTokenVects[i][j] == "else")
				{
					if (myVectOfTokenVects[i + 1][j] == "{")
						;//do nothing
					else
					{
						if (goToElse)
							;//business as usual
						else
						{
							i = i + 1;
							j = myVectOfCategoryVects[i].size() - 1;
						}
						
					}
				}
				else if (myVectOfTokenVects[i][j] == "while")
				{
					//j = myVectOfTokenVects[i].size()-1; I don't know why I put this here
					if (myVectOfTokenVects[i + 1][j] == "{")
						; //do nothing
					else
					{



						//check if expression is true after "while"
						string whileExpression;
						for (int j = 0; j < myVectOfTokenVects[i].size(); j++)
						{
							whileExpression = whileExpression + myVectOfTokenVects[i][j];
						}
						int wpos1; //position 1 of left parenthesis
						int wpos2; //position 2 of right parenthesis

						wpos1 = whileExpression.find("("); //find left parenthesis
						wpos2 = whileExpression.find(")"); //find left parenthesis

						//loop for cutting string into just the expression
						int wcounter = 0;
						for (int z = wpos1; z <= wpos2; z++)
						{
							if (z == wpos2)
								whileExpression = whileExpression.substr(wpos1 + 1, wcounter - 1); //check if +1 and -1 are correct
							wcounter++;
						}


						//********if expression is true or false*****************
						perLineTokenVector tokenExpression4;
						float expValue4;
						LexicalScanner::getPerLineTokenVectFromOneStringObject(whileExpression, tokenExpression4); //find float value and store it in expValue

			


						ExpressionEvaluator::infixEvaluator(tokenExpression4, map, expValue4); //if the expression is true
						if (expValue4 == 1)
						{
							stackLineNumbers.push_back(i); //line num: pos of keyword
							stackKeyWords.push_back(myVectOfTokenVects[i][j]); //stores keyword in stack
							i = stackLineNumbers.back();
							oneLinedWhile = true;
							break;
						}

						else //if the expression is false
						{
							i = i + 1;
							j = myVectOfCategoryVects[i].size() - 1;
						}










					}
				}

				else if (myVectOfTokenVects[i][j] == "display")
				{
					//display code

					//loop to display everything between "display" and ";"
					for (int k = j+1; myVectOfCategoryVects[i][k] != SEMICOLON; k++)
					{
						if (myVectOfTokenVects[i][k] == ",")
							cout << endl;

						//if is string liter
						else if (myVectOfCategoryVects[i][k] == STRING_LITERAL)
						{
							cout << myVectOfTokenVects[i][j + 1] << endl;
						}

						else
						{
							//expression evaluator
							perLineTokenVector tokenExpression;
							float expValue;



							string forLexScannerString; //need to cut off the keyword and '=' before lex scanning it
							forLexScannerString = sourceProgram[i];

							string::iterator deleteHere; //where to delete
							//find "=" so i know where to cut off the string at the beginning
							for (string::iterator m = forLexScannerString.begin()+1; *(m-1) != 'y'; m++)
								deleteHere = m + 2;

							forLexScannerString.erase(forLexScannerString.begin(), deleteHere);





							//***********************************error check
							//so my strategy is to check to see if an ID_NAME is after a "="
							//and if it is, it must also be located in my map variable
							bool AOFound = false;
							bool IDFound = false;
							bool shouldBreak = false;
							for (int r = 0; r < myVectOfCategoryVects[i].size(); r++)
							{
								if ((myVectOfCategoryVects[i][r]) == ASSIGNMENT_OP)
								{
									AOFound = true;

									for (int w = r; w < myVectOfCategoryVects[1].size(); w++)
									{
										if (myVectOfCategoryVects[i][w] == ID_NAME)
										{
											IDFound = true;
											if (IDFound && AOFound)
											{
												if (map.count(myVectOfTokenVects[i][w]) > 0)
													shouldBreak = true; //match found
												else
												{
													cout << "**********************RUNTIME ERROR*************************" << endl;
													shouldBreak = true;
													break;
												}
											}
											if (shouldBreak)
												break;
										}
										if (shouldBreak)
											break;
									}
									if (shouldBreak)
										break;

								}
								if (shouldBreak)
									break;

							}
							if (shouldBreak)
								break;









								//get lexical info
							LexicalScanner::getPerLineTokenVectFromOneStringObject
								(sourceProgram[i],
								tokenExpression
								);
								//find float value and store it in expValue
							ExpressionEvaluator::infixEvaluator(tokenExpression, map, expValue);

							if (!shouldBreak)
								cout << expValue << " ";
							if (forLexScannerString.find(" \" ") != string::npos)
							{
								int firstDubQuotes = forLexScannerString.find("\"");
								int lastDubQuotes = forLexScannerString.rfind("\"");
								//for (int i = 0; i <= firstDubQuotes; i++)
								//{
								//	if (i == firstDubQuotes)
								//		expression = expression.substr(pos1 + 1, counter - 1); //check if +1 and -1 are correct
								//	counter++;
								//}
								int count=0;
								for (int i = firstDubQuotes; i != lastDubQuotes; i++)
								{
									count++;
								}
								forLexScannerString = forLexScannerString.substr(firstDubQuotes+1, count-1);
								cout << forLexScannerString << endl;
							}
							break;
						}

					}
				}

				else if (myVectOfTokenVects[i][j] == "read")
				{
					//read code

					float readInput;
					cin >> readInput;

					map[myVectOfTokenVects[i][j + 1]] = readInput;
				}
			}


			else if (myVectOfCategoryVects[i][j] == ASSIGNMENT_OP)
			{

				//assignment code
				int k = j + 1; // k is what is after the =
				//expression evaluator
				perLineTokenVector tokenExpression;

				float expValue;

				string forLexScannerString; //need to cut off the keyword and '=' before lex scanning it
				forLexScannerString = sourceProgram[i];


				//******first check if is actually relational op
				if (!(myVectOfTokenVects[i][j - 1] == "<" ||
					myVectOfTokenVects[i][j - 1] == ">" ||
					myVectOfTokenVects[i][j - 1] == "!" ||
					myVectOfTokenVects[i][j - 1] == "=" ||
					myVectOfTokenVects[i][j + 1] == "="))
				{



					string::iterator deleteHere; //where to delete
					//find "=" so i know where to cut off the string at the beginning
					for (string::iterator m = forLexScannerString.begin() + 1; *(m - 1) != '='; m++)
						deleteHere = m + 1;

					forLexScannerString.erase(forLexScannerString.begin(), deleteHere);

				}









				else
				{

					//***********************************error check
					//so my strategy is to check to see if an ID_NAME is after a "="
					//and if it is, it must also be located in my map variable
					bool AOFound = false; //assignment op
					bool IDFound = false; //ID
					bool shouldBreak = false;

						for (int r = 0; r < myVectOfCategoryVects[i].size(); r++)
						{
							if ((myVectOfCategoryVects[i][r]) == ASSIGNMENT_OP)
							{
								AOFound = true;
								
									for (int w = r; w < myVectOfCategoryVects[1].size(); w++)
									{
										if (myVectOfCategoryVects[i][w] == ID_NAME)
										{
											IDFound = true;
											if (IDFound && AOFound)
											{
												if (map.count(myVectOfTokenVects[i][w]) > 0)
													shouldBreak = true; //match found
												else
												{
													cout << "**********************SYNTAX ERROR*************************" << endl;
													shouldBreak = true;
													break;
												}
											}
											if (shouldBreak)
												break;
										}
										if (shouldBreak)
											break;
									}
									if (shouldBreak)
										break;
								
							}
							if (shouldBreak)
								break;

						}
						if (shouldBreak)
							break;
					


				}










					//get lexical info
					LexicalScanner::getPerLineTokenVectFromOneStringObject
						(forLexScannerString,
						tokenExpression
						);
					//find float value and store it in expValue
					ExpressionEvaluator::infixEvaluator(tokenExpression, map, expValue);

					//assign it
					map[myVectOfTokenVects[i][j - 1]] = expValue;

				}
			
		}
	}

}