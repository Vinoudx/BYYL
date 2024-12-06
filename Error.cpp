#include <stdio.h>
#include <cstdlib>
#include "Error.h"


void ehandler(ErrorType e, int errorLine, char* iName) {
	switch (e) {
	case NO_LEFT_PARENTHESIS:
		printf("%s in line %d\n", "expect left parenthesis", errorLine);
		break;
	case NO_RIGHT_PARENTHESIS:
		printf("%s in line %d\n", "expect right parenthesis", errorLine);
		break;
	case NO_WHILE_AFTER_DO_STATEMENT:
		printf("%s in line %d\n", "expect while after do statement", errorLine);
		break;
	case NO_SEMICOLON:
		printf("%s in line %d\n", "expect senicolon after statement", errorLine - 1);
		break;
	case NO_IDENTIFIER:
		printf("%s in line %d\n", "expect identifier", errorLine - 1);
		break;
	case NO_IDENTIFIER_IN_STATEMENT:
		printf("%s in line %d\n", "expect identifier", errorLine);
		break;
	case NO_EQUAL:
		printf("%s in line %d\n", "expect a '='", errorLine);
		break;
	case NO_NUMBER:
		printf("%s in line %d\n", "expect a number", errorLine);
		break;
	case NO_ASSIGNMENT:
		printf("%s in line %d\n", "expect a ':=", errorLine);
		break;
	case NO_DO:
		printf("%s in line %d\n", "expect do after while", errorLine);
		break;
	case NO_END:
		printf("%s in line %d\n", "expect end after begin", errorLine);
		break;
	case NO_PERIOD:
		printf("%s", "expect '.' at the end of file");
		break;
	case REPEAT_IDENTIFIER:
		printf("\n\n%s %s\n\n", "repeat declearation of identifier",iName);
		break;
	case UNKOWN_IDENTIFIER:
		printf("\n\n%s %s %s\n\n", "use identifier ", iName, " before declearation");
		break;
	default:
		break;
	}
	exit(1);
}