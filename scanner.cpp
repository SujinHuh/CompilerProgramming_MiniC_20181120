#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_KEYWORDS 7   // word symbol 의 갯수(7개의 단어 심벌)
#define ID_LENGTH 12    // identifier 의 최대 길이(식별자,키워드의 길이를 제한)

char id[ID_LENGTH];

enum tsymbol {
	tnull = -1,    //열거형 변수인 tsymbol
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
// 구조체 tokentype
struct tokenType {
	int number;	// token number
				// 공용체 value
	union {
		char id[ID_LENGTH];
		int num;
	} value;		// 토큰값
};

char *keyword[NO_KEYWORDS] = {
	"const", "else", "if", "int", "return", "void", "while"
};

enum tsymbol tnum[NO_KEYWORDS] = {
	tconst, telse, tif, tint, treturn, tvoid, twhile
};

void lexicalError(int n)
{
	printf(" Lexical Error...!!! : ");          //Lexical error일 경우 출력 
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

// 첫 문자 이후 (알파벳or숫자or'_') 검사 함수
int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	// 알파벳or0~9가 아니면 "0"
	else return 0;
}

// 정수상수 인식을 위한 16진수 입력 변환 함수
int hexValue(char ch)
{
	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');  // 0 ~ 9 구하는 곳
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10); // 11 ~ 15 구하는 곳(대문자알파벳)
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10); // 11 ~ 15 구하는 곳(소문자알파벳)

	default: return -1;
	}
}

// 정수상수 인식을 위한 8진수 16진수 입력 변환 함수
int getIntNum(char firstCharacter, FILE* source_file)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter != '0') {			// 10진수 처리부분
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');  // ASCII 코드값(char)를 정수형태(int)로 변환
											   // 10*(자리수를 생각하여 계산)
			ch = fgetc(source_file);
		} while (isdigit(ch));              // '0'~'9' 인지 판단 아니면 0(=false)

	}
	else {    // '0'으로 시작하면 8,16진수 or 0 이다.

		ch = fgetc(source_file);
		if ((ch >= '0') && (ch <= '7'))	    // 8진수 처리부분

			do {
				num = 8 * num + (int)(ch - '0');   // ASCII 코드값(char)를 정수형태(int)로 변환
												   // 8*(자리수를 생각하여 계산)

				ch = fgetc(source_file);
			} while ((ch >= '0') && (ch <= '7'));   // ASCII 코드값이 8진수 범위일때까지
		else if ((ch == 'X') || (ch == 'X')) {	// 16진수 처리부분

			while ((value = hexValue(ch = fgetc(source_file))) != -1) // hexValue를 호출하여 구함
				num = 16 * num + value;  // 16*(자리수를 고려하여 계산)

		}
		else num = 0;			// 0을 반환한다.
	}
	ungetc(ch, stdin);	// retract
	return num;
}

//구조체 scanner
struct tokenType scanner(FILE* source_file)
{
	struct tokenType token;  //구조체 토큰
	int i, index;
	char ch;

	token.number = tnull;  //토큰넘버 -1

	do {
		while (isspace(ch = fgetc(source_file))); // 공백을 제거하는 부분
		if (superLetter(ch)) { // 식별자나 키워드가 있다면....
			i = 0;
			do {
				if (i<ID_LENGTH) id[i++] = ch;
				ch = fgetc(source_file);
			} while (superLetterOrDigit(ch));
			// superLetterOrDigit() : 첫문자 이후 (알파벳or숫자or'_') 검사 함수를 호출

			if (i >= ID_LENGTH) lexicalError(1);  // 예외처리1 (id의 길이 초과)
			id[i] = '\0';  //종료문자를 추가한다
			ungetc(ch, stdin); // 취소~

							   // 키워드 테이블 상에서 identifier 를 찾는다
			for (index = 0; index < NO_KEYWORDS; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORDS)	// 찾았다면, keyword exit!!
				token.number = tnum[index]; //토큰넘버에 키워드값 입력
			else {			// 키워드가 아니라면=식별자이다.
				token.number = tident;  //토큰번호에 4입력(식별자)
				strcpy(token.value.id, id); //id를 토큰값에 입력(식별자)
			}
		} // identifier or keyword 의 끝

		else if (isdigit(ch)) {			// 정수 상수
			token.number = tnumber;     // 토큰번호에 5입력(숫자)
			token.value.num = getIntNum(ch, source_file);  // 리턴값을 토큰갑에 입력
		}                                  // getIntNum() : 정수상수 인식을 위한 8진수 16진수 입력 변환 함수1 

		else switch (ch) {				// special character

		case '/':					// state 10
			ch = fgetc(source_file);
			if (ch == '*')			// text comment
				do {
					while (ch != '*') ch = fgetc(source_file);  // "*"나올떄까지 입력을 받는다.
					ch = fgetc(source_file);
				} while (ch != '/');  // '*'이 나온 후 '/'이 나오면 끝 아니면 다시 반복한다.
			else if (ch == '/')		// line comment
				while (fgetc(source_file) != '\n');  //한줄이 끝날때까지 입력을 받는다
			else if (ch == '=') token.number = tdivAssign;  //토큰번호 19[/=]로 인식
			else {
				token.number = tdiv;  //토큰번호 18[/]로 인식
				ungetc(ch, stdin); // 취소
			}
			break;

		case '!':					// state 17
			ch = fgetc(source_file);
			if (ch == '=') token.number = tnotequ; // 토큰번호 1[!=]로 인식
			else {
				token.number = tnot;   // 토큰번호 0[!]로 인식
				ungetc(ch, stdin);	// 취소
			}
			break;
		case '%':					// state 20
			ch = fgetc(source_file);
			if (ch == '=')
				token.number = tmodAssign;  // 토큰번호 3[%=]로 인식
			else {
				token.number = tmod;  // 토큰번호 3[%=]로 인식
				ungetc(ch, stdin);
			}
			break;

		case '&':					// state 23
			ch = fgetc(source_file);
			if (ch == '&') token.number = tand; // 토큰번호 6[&&]로 인식
			else {
				lexicalError(2);  // 예외처리2 (&&)가 아님
				ungetc(ch, stdin);	// 취소
			}
			break;

		case '*':			// state 25
			ch = fgetc(source_file);
			if (ch == '=') token.number = tmulAssign;  // 토큰번호 10[*=]로 인식
			else {
				token.number = tmul;  // 토큰번호 9[*]로 인식
				ungetc(ch, stdin);	// 취소
			}
			break;

		case '+':					// state 28
			ch = fgetc(source_file);
			if (ch == '+') token.number = tinc;  // 토큰번호 12[++]로 인식
			else if (ch == '=') token.number = taddAssign;  // 토큰번호 13[+=]로 인식
			else {
				token.number = tplus;  // 토큰번호 11[+]로 인식
				ungetc(ch, stdin);	// 취소
			}

			break;
		case '-':					// stats 32
			ch = fgetc(source_file);
			if (ch == '-') token.number = tdec;  // 토큰번호 16[--]로 인식
			else if (ch == '=') token.number = tsubAssign; // 토큰번호 17[-=]로 인식
			else {
				token.number = tminus; // 토큰번호 15[-]로 인식
				ungetc(ch, stdin);	// 취소
			}
			break;

		case '<':					// state 36
			ch = fgetc(source_file);
			if (ch == '=') token.number = tlesse; // 토큰번호 22[<=]로 인식
			else {
				token.number = tless;  // 토큰번호 21[<]로 인식
				ungetc(ch, stdin);	// 취소
			}
			break;

		case '=':					// state 39
			ch = fgetc(source_file);
			if (ch == '=') token.number = tequal;  // 토큰번호 24[==]로 인식
			else {
				token.number = tassign;   // 토큰번호 23[=]로 인식
				ungetc(ch, stdin);	// 취소
			}
			break;

		case '>':			// state 42
			ch = fgetc(source_file);
			if (ch == '=') token.number = tgreate;   // 토큰번호 26[>=]로 인식
			else {
				token.number = tgreat;  // 토큰번호 25[>]로 인식
				ungetc(ch, stdin);	// 취소
			}
			break;

		case '|':					// state 45
			ch = fgetc(source_file);
			if (ch == '|') token.number = tor;  // 토큰번호 31[||]로 인식
			else {
				lexicalError(3);  // "3" (||)이 아님
				ungetc(ch, stdin);	// 취소
			}
			break;

		case '(': token.number = tlparen; break;  // 토큰번호 7[(]로 인식
		case ')': token.number = trparen;		break;  // 토큰번호 8[)]로 인식
		case ',': token.number = tcomma; break;  // 토큰번호 14[,]로 인식
		case ';': token.number = tsemicolon; break;  // 토큰번호 20[;]로 인식
		case '[': token.number = tlbracket; break;  // 토큰번호 27[[]로 인식
		case ']': token.number = trbracket; break;  // 토큰번호 27[[]로 인식
		case '{': token.number = tlbrace; break;  // 토큰번호 30[{]로 인식
		case '}': token.number = trbrace; break;  // 토큰번호 32[}]로 인식
		case EOF: token.number = teof; break;  // 토큰번호 29[EOF]로 인식

		default: {   // "4" ..lexical error.. (인식되지 않는 특수어)
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}

		} // switch
	} while (token.number == tnull);
	return token;
} // 어휘분석기의 끝

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

		if (token.number == 5) {  // 숫자 토큰 출력
			for (i = 0; i<ID_LENGTH; i++)
				fprintf(stdout, "%c", id[i]);
			fprintf(stdout, ": (%d, %d)\n", token.number, token.value.num);
		}
		else if (token.number == 4) { // 식별자 토큰 출력
			for (i = 0; i<ID_LENGTH; i++)
				fprintf(stdout, "%c", id[i]);
			fprintf(stdout, ": (%d, %s)\n", token.number, token.value.id);
		}
		else {
			for (i = 0; i<ID_LENGTH; i++) // 식별자, 숫자 제외 출력
				fprintf(stdout, "%c", id[i]);
			fprintf(stdout, ": (%d, 0)\n", token.number);
		}
	} while (!feof(source_file));
	fclose(source_file);
}
