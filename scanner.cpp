#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_KEYWORDS 7   // word symbol �� ����(7���� �ܾ� �ɹ�)
#define ID_LENGTH 12    // identifier �� �ִ� ����(�ĺ���,Ű������ ���̸� ����)

char id[ID_LENGTH];

enum tsymbol {
	tnull = -1,    //������ ������ tsymbol
	tnot, tnotequ, tmod, tmodAssign, tident, tnumber,
	/* 0	  1	   2		   3	       4		  5      */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	/* 6	  7	  8	      9		    10		 11	*/
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	/* 12	   13	       14         15	    16		  17	*/
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
	/* 18      19		   20		 21	  22	  23	*/
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	/* 24	 25	    26		27	   28	      29		*/
	//-------------------word symbols------------------------ //
	/* 30	31	32	33	34	35	                 */
	tconst, telse, tif, tint, treturn, tvoid,
	/* 36	    37		38	39	                  	*/
	twhile, tlbrace, tor, trbrace
};
// ����ü tokentype
struct tokenType {
	int number;	// token number
				// ����ü value
	union {
		char id[ID_LENGTH];
		int num;
	} value;		// ��ū��
};

char *keyword[NO_KEYWORDS] = {
	"const", "else", "if", "int", "return", "void", "while"
};

enum tsymbol tnum[NO_KEYWORDS] = {
	tconst, telse, tif, tint, treturn, tvoid, twhile
};

void lexicalError(int n)
{
	printf(" Lexical Error...!!! : ");          //Lexical error�� ��� ��� 
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &.\n");
		break;
	case 3: printf("next character must be |.\n");
		break;
	case 4: printf("invalid character.\n");
		break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

// ù ���� ���� (���ĺ�or����or'_') �˻� �Լ�
int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	// ���ĺ�or0~9�� �ƴϸ� "0"
	else return 0;
}

// ������� �ν��� ���� 16���� �Է� ��ȯ �Լ�
int hexValue(char ch)
{
	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');  // 0 ~ 9 ���ϴ� ��
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10); // 11 ~ 15 ���ϴ� ��(�빮�ھ��ĺ�)
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10); // 11 ~ 15 ���ϴ� ��(�ҹ��ھ��ĺ�)

	default: return -1;
	}
}

// ������� �ν��� ���� 8���� 16���� �Է� ��ȯ �Լ�
int getIntNum(char firstCharacter, FILE* source_file)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter != '0') {			// 10���� ó���κ�
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');  // ASCII �ڵ尪(char)�� ��������(int)�� ��ȯ
											   // 10*(�ڸ����� �����Ͽ� ���)
			ch = fgetc(source_file);
		} while (isdigit(ch));              // '0'~'9' ���� �Ǵ� �ƴϸ� 0(=false)

	}
	else {    // '0'���� �����ϸ� 8,16���� or 0 �̴�.

		ch = fgetc(source_file);
		if ((ch >= '0') && (ch <= '7'))	    // 8���� ó���κ�

			do {
				num = 8 * num + (int)(ch - '0');   // ASCII �ڵ尪(char)�� ��������(int)�� ��ȯ
												   // 8*(�ڸ����� �����Ͽ� ���)

				ch = fgetc(source_file);
			} while ((ch >= '0') && (ch <= '7'));   // ASCII �ڵ尪�� 8���� �����϶�����
		else if ((ch == 'X') || (ch == 'X')) {	// 16���� ó���κ�

			while ((value = hexValue(ch = fgetc(source_file))) != -1) // hexValue�� ȣ���Ͽ� ����
				num = 16 * num + value;  // 16*(�ڸ����� ����Ͽ� ���)

		}
		else num = 0;			// 0�� ��ȯ�Ѵ�.
	}
	ungetc(ch, stdin);	// retract
	return num;
}

//����ü scanner
struct tokenType scanner(FILE* source_file)
{
	struct tokenType token;  //����ü ��ū
	int i, index;
	char ch;

	token.number = tnull;  //��ū�ѹ� -1

	do {
		while (isspace(ch = fgetc(source_file))); // ������ �����ϴ� �κ�
		if (superLetter(ch)) { // �ĺ��ڳ� Ű���尡 �ִٸ�....
			i = 0;
			do {
				if (i<ID_LENGTH) id[i++] = ch;
				ch = fgetc(source_file);
			} while (superLetterOrDigit(ch));
			// superLetterOrDigit() : ù���� ���� (���ĺ�or����or'_') �˻� �Լ��� ȣ��

			if (i >= ID_LENGTH) lexicalError(1);  // ����ó��1 (id�� ���� �ʰ�)
			id[i] = '\0';  //���Ṯ�ڸ� �߰��Ѵ�
			ungetc(ch, stdin); // ���~

							   // Ű���� ���̺� �󿡼� identifier �� ã�´�
			for (index = 0; index < NO_KEYWORDS; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORDS)	// ã�Ҵٸ�, keyword exit!!
				token.number = tnum[index]; //��ū�ѹ��� Ű���尪 �Է�
			else {			// Ű���尡 �ƴ϶��=�ĺ����̴�.
				token.number = tident;  //��ū��ȣ�� 4�Է�(�ĺ���)
				strcpy(token.value.id, id); //id�� ��ū���� �Է�(�ĺ���)
			}
		} // identifier or keyword �� ��

		else if (isdigit(ch)) {			// ���� ���
			token.number = tnumber;     // ��ū��ȣ�� 5�Է�(����)
			token.value.num = getIntNum(ch, source_file);  // ���ϰ��� ��ū���� �Է�
		}                                  // getIntNum() : ������� �ν��� ���� 8���� 16���� �Է� ��ȯ �Լ�1 

		else switch (ch) {				// special character

		case '/':					// state 10
			ch = fgetc(source_file);
			if (ch == '*')			// text comment
				do {
					while (ch != '*') ch = fgetc(source_file);  // "*"���Ë����� �Է��� �޴´�.
					ch = fgetc(source_file);
				} while (ch != '/');  // '*'�� ���� �� '/'�� ������ �� �ƴϸ� �ٽ� �ݺ��Ѵ�.
			else if (ch == '/')		// line comment
				while (fgetc(source_file) != '\n');  //������ ���������� �Է��� �޴´�
			else if (ch == '=') token.number = tdivAssign;  //��ū��ȣ 19[/=]�� �ν�
			else {
				token.number = tdiv;  //��ū��ȣ 18[/]�� �ν�
				ungetc(ch, stdin); // ���
			}
			break;

		case '!':					// state 17
			ch = fgetc(source_file);
			if (ch == '=') token.number = tnotequ; // ��ū��ȣ 1[!=]�� �ν�
			else {
				token.number = tnot;   // ��ū��ȣ 0[!]�� �ν�
				ungetc(ch, stdin);	// ���
			}
			break;
		case '%':					// state 20
			ch = fgetc(source_file);
			if (ch == '=')
				token.number = tmodAssign;  // ��ū��ȣ 3[%=]�� �ν�
			else {
				token.number = tmod;  // ��ū��ȣ 3[%=]�� �ν�
				ungetc(ch, stdin);
			}
			break;

		case '&':					// state 23
			ch = fgetc(source_file);
			if (ch == '&') token.number = tand; // ��ū��ȣ 6[&&]�� �ν�
			else {
				lexicalError(2);  // ����ó��2 (&&)�� �ƴ�
				ungetc(ch, stdin);	// ���
			}
			break;

		case '*':			// state 25
			ch = fgetc(source_file);
			if (ch == '=') token.number = tmulAssign;  // ��ū��ȣ 10[*=]�� �ν�
			else {
				token.number = tmul;  // ��ū��ȣ 9[*]�� �ν�
				ungetc(ch, stdin);	// ���
			}
			break;

		case '+':					// state 28
			ch = fgetc(source_file);
			if (ch == '+') token.number = tinc;  // ��ū��ȣ 12[++]�� �ν�
			else if (ch == '=') token.number = taddAssign;  // ��ū��ȣ 13[+=]�� �ν�
			else {
				token.number = tplus;  // ��ū��ȣ 11[+]�� �ν�
				ungetc(ch, stdin);	// ���
			}

			break;
		case '-':					// stats 32
			ch = fgetc(source_file);
			if (ch == '-') token.number = tdec;  // ��ū��ȣ 16[--]�� �ν�
			else if (ch == '=') token.number = tsubAssign; // ��ū��ȣ 17[-=]�� �ν�
			else {
				token.number = tminus; // ��ū��ȣ 15[-]�� �ν�
				ungetc(ch, stdin);	// ���
			}
			break;

		case '<':					// state 36
			ch = fgetc(source_file);
			if (ch == '=') token.number = tlesse; // ��ū��ȣ 22[<=]�� �ν�
			else {
				token.number = tless;  // ��ū��ȣ 21[<]�� �ν�
				ungetc(ch, stdin);	// ���
			}
			break;

		case '=':					// state 39
			ch = fgetc(source_file);
			if (ch == '=') token.number = tequal;  // ��ū��ȣ 24[==]�� �ν�
			else {
				token.number = tassign;   // ��ū��ȣ 23[=]�� �ν�
				ungetc(ch, stdin);	// ���
			}
			break;

		case '>':			// state 42
			ch = fgetc(source_file);
			if (ch == '=') token.number = tgreate;   // ��ū��ȣ 26[>=]�� �ν�
			else {
				token.number = tgreat;  // ��ū��ȣ 25[>]�� �ν�
				ungetc(ch, stdin);	// ���
			}
			break;

		case '|':					// state 45
			ch = fgetc(source_file);
			if (ch == '|') token.number = tor;  // ��ū��ȣ 31[||]�� �ν�
			else {
				lexicalError(3);  // "3" (||)�� �ƴ�
				ungetc(ch, stdin);	// ���
			}
			break;

		case '(': token.number = tlparen; break;  // ��ū��ȣ 7[(]�� �ν�
		case ')': token.number = trparen;		break;  // ��ū��ȣ 8[)]�� �ν�
		case ',': token.number = tcomma; break;  // ��ū��ȣ 14[,]�� �ν�
		case ';': token.number = tsemicolon; break;  // ��ū��ȣ 20[;]�� �ν�
		case '[': token.number = tlbracket; break;  // ��ū��ȣ 27[[]�� �ν�
		case ']': token.number = trbracket; break;  // ��ū��ȣ 27[[]�� �ν�
		case '{': token.number = tlbrace; break;  // ��ū��ȣ 30[{]�� �ν�
		case '}': token.number = trbrace; break;  // ��ū��ȣ 32[}]�� �ν�
		case EOF: token.number = teof; break;  // ��ū��ȣ 29[EOF]�� �ν�

		default: {   // "4" ..lexical error.. (�νĵ��� �ʴ� Ư����)
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}

		} // switch
	} while (token.number == tnull);
	return token;
} // ���ֺм����� ��

void main(int argc, char *argv[])
{
	FILE *source_file;
	int i;
	struct tokenType token;

	
	if ((source_file = fopen("prime.mc", "r")) == NULL) {
		fprintf(stderr, "%s file not found \n", "prime.mc");
		exit(-1);
	}

	do {
		for (i = 0; i<ID_LENGTH; i++)
			id[i] = ' ';
		token = scanner(source_file);
		fprintf(stdout, "Token ---> ");

		if (token.number == 5) {  // ���� ��ū ���
			for (i = 0; i<ID_LENGTH; i++)
				fprintf(stdout, "%c", id[i]);
			fprintf(stdout, ": (%d, %d)\n", token.number, token.value.num);
		}
		else if (token.number == 4) { // �ĺ��� ��ū ���
			for (i = 0; i<ID_LENGTH; i++)
				fprintf(stdout, "%c", id[i]);
			fprintf(stdout, ": (%d, %s)\n", token.number, token.value.id);
		}
		else {
			for (i = 0; i<ID_LENGTH; i++) // �ĺ���, ���� ���� ���
				fprintf(stdout, "%c", id[i]);
			fprintf(stdout, ": (%d, 0)\n", token.number);
		}
	} while (!feof(source_file));
	fclose(source_file);
}
