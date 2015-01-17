//cont/map1.cpp
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include "lexScanner.h"
#include "expEvaluator.h"
using namespace std;


//*********************************************************************************
//*********************************************************************************
int ExpressionEvaluator::precedenceLevel(const string & opToken)
//return the precedence level of an operator token
{
				if ( opToken == "||" )
					return 0;

				if ( opToken == "&&" )
					return 1;


				if ( opToken == "<" )
					return 2;
				if ( opToken == "==" )
					return 2;
				if (opToken == "!=")
					return 2;
				if ( opToken == ">" )
					return 2;
				if ( opToken == ">=" )
					return 2;
				if ( opToken == "<=" )
					return 2;

				if ( opToken == "+" )
					return 3;
				if ( opToken == "-" )
					return 3;

				if ( opToken == "*" )
					return 4;
				if ( opToken == "/" )
					return 4;
				if (opToken == "%")
					return 4;

				if ( opToken == "!" )
					return 5;

				//Otherwise
				cout << "unknown op:" << opToken << ". precedence level?? " << endl;
				return -1;

}


//*********************************************************************************
//*********************************************************************************
bool ExpressionEvaluator::postfixEvaluator
(	const expVector & postfixExp, 
	floatVarValueTable & varTable, 
	float & expValue 
)
//Evaluate the value of a postfix expression postfixExp
//with respect to the variables and their current values  in varTable.
//finally store the value of the expresson in expValue.
//If postfixExp is not a valid postfix expression, return false; otherwsie return true.
{
	vector<float> stack;
	for ( expVector::const_iterator pos= postfixExp.begin();
		  pos != postfixExp.end();
		  pos++
		)
	{
		string token;

		token = *pos;
		if ( token.size()< 1)
		{	expValue = 0;
			cout << "Empty Token, not valid." << endl;
			return false;
		}

		if ( LexicalScanner::isNUMERICAL_LITERAL(token) )		
			//A numerical constant, convert it into a float value, and
			//store push it onto the stack
			stack.push_back(	atof( token.c_str() )		);
		else if ( LexicalScanner::isID_NAME(token) )		
			//An ID_NAME, try to look up its current value from the table.
			//If succeeds, push the value onto the stack.
			{	if (varTable.find(token) == varTable.end())
						return false; //Invalid name
				stack.push_back(	varTable[token]	);
			}
		else if (	LexicalScanner::isNUMERICAL_OP(token)  ||  
					LexicalScanner::isRELATIONAL_OP(token) || 
					LexicalScanner::isLOGICAL_OP(token)
				)
			//A numerical or relational operator
			//Try to pop two operands from the stack apply the operator.
			//If succeeds, push te result back to the stack.
			{
				float op1, op2; 

				//Try to pop op2 first
				if ( !stack.empty() )
					{	op2 = stack.back();
						stack.pop_back();
					}
				else
				{	cout << "No matching op2 for the op: " << token << endl;
					return false; //op2 not available, Not a valid expression
				}

				
				//Determine whether this is a bianry operator or a unary operator
				bool isUnaryOP, isBinaryOP;
				if	(	token == "!"	)	//unary logical negation operator
					{	isUnaryOP = true; isBinaryOP = false;
					}
				else
					{	isUnaryOP = false; isBinaryOP = true;
					}

				if (isUnaryOP) //When the operator is a unary operator
				{	//the only case at this moment: the logical negation operator ! 
					if (token == "!")
						stack.push_back(!op2);
					else
					{		cout << "Unknown unary op: " << token << endl;
							return false;
					}
				} //end the processing of a unary operator

				if (isBinaryOP)  //When the operator is a binary operator
				{
					if ( stack.empty() )
						return false; //op1 not available, Not a valid expression

					op1 = stack.back();
					stack.pop_back();
				

					//for the modulus operator
					int iop1 = static_cast <int> (op1);
					int iop2 = static_cast <int> (op2);

					//Apply the binary operator and push back the result to the stack
					if ( token == "+" )
						stack.push_back(op1+op2);
					else if ( token == "-" )
						stack.push_back(op1-op2);
					else if ( token == "*" )
						stack.push_back(op1*op2);
					else if (token == "/")
						stack.push_back(op1 / op2);


					//modulus
					else if (token == "%")
						stack.push_back((iop1%iop2));

					else if ( token == "<" )
						stack.push_back(op1<op2);
					else if (token == "==")
						stack.push_back(op1 == op2);
					else if (token == "!=")
						stack.push_back(op1 != op2);
					else if ( token == ">" )
						stack.push_back(op1>op2);
					else if ( token == ">=" )
						stack.push_back(op1>=op2);
					else if ( token == "<=" )
						stack.push_back(op1<=op2);

					else if ( token == "&&" )
						stack.push_back(op1&&op2);
					else if ( token == "||" )
						stack.push_back(op1||op2);
					else
						{	cout << "Unkown binary op: " << token << endl;
							return false;
						}
				} //end the processing of a binary operator

			}//end the processing of the case that the token is an operator

		else
			{	cout << "Unkown token type (Not an op or operand): " << token << endl;
				return false;
			}
			// Every token in the expression must be either be a numerical constant, 
			// an ID_NAME or an operator; Otherwise it is NOT a valid expression.

	}//end for


	if (stack.size() == 1)
	//The evaluated value of the expression should be the only value left on the stack
		{	expValue = stack.back();
			return true;
		}
	else
	//Other wise it is not a valis expression.
		return false;

}


//*********************************************************************************
//*********************************************************************************
bool ExpressionEvaluator::infixToPostfixConversion
(const expVector & infixExp, expVector & postfixExp)
//Convert the infix expression in infixExp into a corresponding postfix expression.
//Store the corresponding postfix expression in postfixExp.
//If infixExp is not a valid infix expression, return false; otherwsie return true.
{
	vector<string> stack;
	for (expVector::const_iterator pos = infixExp.begin();
		pos != infixExp.end();
		pos++
		)
	{
		string token;


		token = *pos;
		if (token.size() < 1)
		{
			;
		}

		else if (LexicalScanner::isNUMERICAL_LITERAL(token))
			//A numerical constant, convert it into a float value, and
			//store push it onto the stack
			postfixExp.push_back(token);
		else if (LexicalScanner::isID_NAME(token))
			postfixExp.push_back(token);
		else if (LexicalScanner::isLEFT_PARENTHESIS(token))
			stack.push_back(token);
		else if (LexicalScanner::isRIGHT_PARENTHESIS(token))
		{
			while (stack.back() != "(")
			{
				if (stack.back() != "(" && stack.size() < 1)
				{
					cout << "Incorrect expression" << endl;
					return false;
				}
				postfixExp.push_back(stack.back());
				stack.pop_back();
			}
			if (stack.back() == "(")
			{
				stack.pop_back();
			}
		}



		//*************************************************************************************
		else if (LexicalScanner::isNUMERICAL_OP(token) ||
			LexicalScanner::isRELATIONAL_OP(token) ||
			LexicalScanner::isLOGICAL_OP(token)
			)
			//A numerical or relational operator
			//Try to pop two operands from the stack apply the operator.
			//If succeeds, push te result back to the stack.
		{

			if (stack.size() < 1)
				stack.push_back(token);
			else if (stack.back() != "(")
			{
				while ((stack.size() != 0) && (stack.back() != "(") && (precedenceLevel(stack.back()) >= precedenceLevel(token)))
				{
					postfixExp.push_back(stack.back());
					stack.pop_back();
				}

				stack.push_back(token);
			}
			else
				stack.push_back(token);
		}
	}

	while (stack.size() > 0)
	{
		postfixExp.push_back(stack.back());
		stack.pop_back();
	}
	return true;
}

//*********************************************************************************
//*********************************************************************************
bool ExpressionEvaluator::infixEvaluator
(	const expVector & infixExp, 
	floatVarValueTable & varTable, 
	float& expValue
)
//Evaluate the value of the infix expression in postfixExp
//with respect to the variables and their current values in varTable.
//finally store the value of the expresson in expValue.
//If postfixExp is not a valid postfix expression, return false; otherwsie return true.
{	expVector postfixExp;
	infixToPostfixConversion(infixExp, postfixExp);
	return postfixEvaluator(postfixExp, varTable, expValue);
}



//*********************************************************************************
//*********************************************************************************
bool ExpressionEvaluator::infixEvaluator	
(	expVector::iterator begin, 
	expVector::iterator end, 
	floatVarValueTable & varTable, 
	float &expValue
)
//Evaluate the value of an infix expression 
//that is embedded within an expression vector 
//The two expVector::iterator iterators: begin and end specify 
//where the infix expression is embedded.
//
//Evaluate the value of an infix expression 
//with respect to the variables and their current values in varTable.
//finally store the value of the expresson in expValue.
//If postfixExp is not a valid postfix expression, return false; otherwsie return true.
{	expVector infixExp(begin,end);
	return infixEvaluator(infixExp, varTable, expValue);
}
