#include "lexer.h"
#include "ilib.h"

static char * keywords[] = {"DIM", "AS", "SHORT", "LONG", "STRING", "BOOLEAN", "FUNCTION",
							"BYVAL", "BYREF", "IF", "THEN", "RETURN", "ELSE", "END",
							"SUB", "CALL", NULL};

static
void
lexer_die(char * message)
{
	printf(message);
	app_exit();
}

static
int
is_keyword(char * keyword)
{
	int i = 0;
	while(keywords[i] != NULL)
	{
		if(strcmp(keywords[i], keyword) == 0)
			return 1;
		i++;
	}
	return 0;
}

static
void
eof_word(	struct LEXWord * word,
			int row,
			const char * file)
{
	word->name[0] = '\0';
	word->type = LEXWORD_EOF;
	word->row = row;
	strcpy(word->file, file);
}

int lexer(	const char * code,
			const char * file,
			int max,
			struct LEXWord * words)
{
	char chr;
	int row = 0;
	eof_word(words, row, file);
	while((chr = *(code++)) != '\0')
		switch(chr)
		{
			case '\n':
				row++;
				break;
			case ' ':
			case '\t':
			case '\r':
				break;
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
			case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
			case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
			case 'v': case 'w': case 'x': case 'y': case 'z':
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
			case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
			case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
			case 'V': case 'W': case 'X': case 'Y': case 'Z':
			case '_':
			{
				words->name[0] = chr;
				int index = 1;
				while(	(chr = *(code++)) != '\0'
						&& chr != '\n'
						&& chr != ' '
						&& chr != '\t'
						&& chr != '\r')
					if((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') || chr == '_')
						words->name[index++] = chr;
					else
						break;
				words->name[index] = '\0';
				if(is_keyword(words->name))
					words->type = LEXWORD_KEYWORD;
				else
					words->type = LEXWORD_SYMBOL;
				words->row = row;
				strcpy(words->file, file);
				eof_word(++words, row, file);
				if(chr == '\0')
					return 1;
				else
					code--;
				break;
			}
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			{
				words->name[0] = chr;
				int index = 1;
				while(	(chr = *(code++)) != '\0'
						&& chr != '\n'
						&& chr != ' '
						&& chr != '\t'
						&& chr != '\r')
					if((chr >= '0' && chr <= '9') || chr == '.')
						words->name[index++] = chr;
					else
						break;
				words->name[index] = '\0';
				words->type = LEXWORD_NUMBER;
				words->row = row;
				strcpy(words->file, file);
				eof_word(++words, row, file);
				if(chr == '\0')
					return 1;
				else
					code--;
				break;
			}
			case '+': case '-': case '*': case '/': case '=':
			case '>': case '<': case '%': case '(': case ')':
			{
				char next_chr = *(code++);
				words->type = LEXWORD_OPERATOR;
				if(chr == '=' && next_chr == '=')
					strcpy(words->name, "==");
				else if(chr == '>' && next_chr == '=')
					strcpy(words->name, ">=");
				else if(chr == '<' && next_chr == '=')
					strcpy(words->name, "<=");
				else if(chr == '<' && next_chr == '>')
					strcpy(words->name, "<>");
				else if((chr == '-' || chr == '+') && (next_chr >= '0' && next_chr <= '9'))
				{
					words->name[0] = chr;
					int index = 1;
					while(	(chr = *(code++)) != '\0'
							&& chr != '\n'
							&& chr != ' '
							&& chr != '\t'
							&& chr != '\r')
					if((chr >= '0' && chr <= '9') || chr == '.')
						words->name[index++] = chr;
					else
						break;
					words->name[index] = '\0';
					words->type = LEXWORD_NUMBER;
				}
				else
					code--;
				words->row = row;
				strcpy(words->file, file);
				eof_word(++words, row, file);
				break;
			}
		}
	return 1;
}
