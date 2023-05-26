#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

// Lexical Analyzer
#define DEC_DIGITS "0123456789"
#define HEX_DIGITS "0123456789abcdefABCDEF"
#define BIN_DIGITS "01"

typedef struct token
{
    char *type;
    int columnNo;
    int rowNo;
    char *lexeme;
} tokenStruct;

int isBracket(const char *str)
{
    if (strlen(str) == 1)
    {
        if (str[0] == '(' || str[0] == ')' || str[0] == '[' || str[0] == ']' || str[0] == '{' || str[0] == '}')
        {
            return 1;
        }
    }
    return 0;
}

int isDecimal(const char *str)
{
    int i = 0;
    if (str[i] == '+' || str[i] == '-')
    {
        i++;
        if (str[i] == '\0')
        {
            return 0;
        }
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        i++;
    }
    if (str[i] == '\0')
    {
        return 1;
    }
    return 0;
}

int isHexaDecimal(const char *str)
{
    int i = 0;
    if (strncmp(str, "0x", 2) != 0)
    {
        return 0;
    }
    i += 2;
    int has_hex_digits = 0;
    while (str[i] != '\0' && strchr(HEX_DIGITS, str[i]) != NULL)
    {
        has_hex_digits = 1;
        i++;
    }
    if (!has_hex_digits || str[i] != '\0')
    {
        return 0;
    }
    return 1;
}
int isBinary(const char *str)
{
    int i = 0;
    if (strncmp(str, "0b", 2) != 0)
    {
        return 0;
    }
    i += 2;
    int has_bin_digits = 0;
    while (str[i] != '\0' && strchr(BIN_DIGITS, str[i]) != NULL)
    {
        has_bin_digits = 1;
        i++;
    }
    if (!has_bin_digits || str[i] != '\0')
    {
        return 0;
    }
    return 1;
}

int isFloatingExp1(const char *str)
{
    int i = 0;
    int hasDot = 0;
    int hasE = 0;
    int hasNumberAfterE = 0;

    if (str[i] == '-' || str[i] == '+')
    {
        i++;
        if (str[i] == '\0')
        {
            return 0;
        }
    }

    while (str[i] != '\0')
    {
        if (str[i] == '.')
        {
            if (hasDot || hasE || (str[i + 1] == '\0' || strchr(DEC_DIGITS, str[i + 1]) == NULL))
            {
                return 0;
            }
            hasDot = 1;
        }
        else if (str[i] == 'e' || str[i] == 'E')
        {
            if (hasE || i == 0 || str[i + 1] == '\0' || (str[i + 1] != '+' && str[i + 1] != '-' && strchr(DEC_DIGITS, str[i + 1]) == NULL))
            {
                return 0;
            }
            hasE = 1;
            if (str[i + 1] == '-' || str[i + 1] == '+')
            {
                i++;
            }
            if (str[i + 1] == '\0')
            {
                return 0;
            }
        }
        else if (strchr(DEC_DIGITS, str[i]) == NULL)
        {
            return 0;
        }
        else if (hasE)
        {
            hasNumberAfterE = 1;
        }

        i++;
    }

    return (hasNumberAfterE || !hasE);
}

int isFloatingExp2(const char *str)
{
    int i = 0;
    if (str[i] == '+' || str[i] == '-')
    {
        i++;
    }
    int found_digit = 0;
    while (str[i] >= '0' && str[i] <= '9')
    {
        found_digit = 1;
        i++;
    }
    if (!found_digit)
    {
        return 0;
    }
    if (str[i] == 'e' || str[i] == 'E')
    {
        i++;
        if (str[i] == '+' || str[i] == '-')
        {
            i++;
        }
        found_digit = 0;
        while (str[i] >= '0' && str[i] <= '9')
        {
            found_digit = 1;
            i++;
        }
        if (!found_digit || str[i] != '\0')
        {
            return 0;
        }
        return 1;
    }
    if (str[i] != '\0')
    {
        return 0;
    }
    return 1;
}
int isBoolean(const char *str)
{
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0)
        return 1;
    else
        return 0;
}
int isChar(const char *str)
{
    if (str[0] != '\'')
        return 0;
    if (str[1] == '\\' && str[2] == '\'' && str[3] == '\'')
        return 1;
    if (str[1] > 31 && str[1] < 127 && str[2] == '\'')
        return 1;
    return 0;
}
int isString(char *str)
{
    int i;
    if (str[0] != '"' || str[strlen(str) - 1] != '"')
    {
        return 0;
    }

    for (i = 1; i < strlen(str) - 1; i++)
    {
        if (str[i] == '\\')
        {
            if (str[i + 1] != '"' && str[i + 1] != '\\' && str[i + 1] != 'n' && str[i + 1] != 'r' && str[i + 1] != 't')
            {
                return 0;
            }
            i++;
        }
        else if (str[i] < ' ' || str[i] > '~')
        {
            return 0;
        }
    }
    return 1;
}
int isKeyword(char *str)
{
    if ((strcmp(str, "define") == 0) || (strcmp(str, "let") == 0) || (strcmp(str, "cond") == 0) ||
        (strcmp(str, "if") == 0) || (strcmp(str, "begin") == 0))
        return 1;

    return 0;
}
int isIdentifier(char *str)
{
    if ((strcmp(str, "define") == 0) || (strcmp(str, "let") == 0) || (strcmp(str, "cond") == 0) ||
        (strcmp(str, "if") == 0) || (strcmp(str, "begin") == 0))
        return 0;
    if (str[0] == '+' || str[0] == '-' || str[0] == '.')
    {
        if (str[1] == '\0')
            return 1;
        return 0;
    }
    int i;
    int len = strlen(str);
    if (len == 0)
    {
        return 0;
    }
    if (!islower(str[0]) && str[0] != '!' && str[0] != '*' && str[0] != '/' && str[0] != ':' &&
        str[0] != '<' && str[0] != '=' && str[0] != '>' && str[0] != '?')
    {
        return 0;
    }
    for (i = 1; i < len; i++)
    {
        if (!islower(str[i]) && !isdigit(str[i]) && str[i] != '.' && str[i] != '+' && str[i] != '-')
        {
            return 0;
        }
    }
    return 1;
}
int isNumLiteral(char *str)
{
    if (isDecimal(str) || isHexaDecimal(str) || isBinary(str) || isFloatingExp1(str) || isFloatingExp2(str))
        return 1;
    else
        return 0;
}
char *checkToken(char *str)
{

    if (isBracket(str) == 1)
    {
        if (strcmp(str, "(") == 0)
            return "LEFTPAR";
        else if (strcmp(str, ")") == 0)
            return "RIGHTPAR";
        else if (strcmp(str, "[") == 0)
            return "LEFTSQUAREB";
        else if (strcmp(str, "]") == 0)
            return "RIGHTSQUAREB";
        else if (strcmp(str, "{") == 0)
            return "LEFTCURLYB";
        else if (strcmp(str, "}") == 0)
            return "RIGHTCURLYB";
    }
    if (isNumLiteral(str) == 1)
    {
        return "NUMBER";
    }
    if (isBoolean(str) == 1)
    {
        return "BOOLEAN";
    }
    if (isChar(str) == 1)
    {
        return "CHAR";
    }
    if (isString(str) == 1)
    {
        return "STRING";
    }
    if (isKeyword(str) == 1)
    {
        if (strcmp(str, "define") == 0)
            return "DEFINE";
        else if (strcmp(str, "let") == 0)
            return "LET";
        else if (strcmp(str, "cond") == 0)
            return "COND";
        else if (strcmp(str, "if") == 0)
            return "IF";
        else if (strcmp(str, "begin") == 0)
            return "BEGIN";
    }
    if (isIdentifier(str) == 1)
    {
        return "IDENTIFIER";
    }
    return "ERROR";
}

void addElement(tokenStruct **array, int *size, tokenStruct newToken)
{
    *array = (tokenStruct *)realloc(*array, (*size + 1) * sizeof(tokenStruct));
    (*array)[*size] = newToken;
    (*size)++;
}

void addCharToToken(char **token, char c, int *token_len)
{
    (*token_len)++;
    *token = realloc(*token, (*token_len + 1) * sizeof(char));
    (*token)[*token_len - 1] = c;
    (*token)[*token_len] = '\0';
}
int listLength = 0;
tokenStruct *processFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        return NULL;
    }
    tokenStruct t;
    tokenStruct *tokenArray = NULL;

    int row = 1;
    int col = 0;

    while (1)
    {
        char c = (char)fgetc(fp);
        col++;
        while (c == '\n')
        {
            c = (char)fgetc(fp);
            row++;
            col = 1;
        }
        while (isspace(c))
        {
            c = (char)fgetc(fp);
            col++;
        }
        if (c == '~')
        {
            while (c != '\n')
                c = (char)fgetc(fp);
            ungetc(c, fp);
            continue;
        }

        if (c == EOF)
        {
            break;
        }

        char *token = NULL;
        int token_len = 0;

        if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}')
        {
            addCharToToken(&token, c, &token_len);
        }
        else if (c != '"')
        {
            while (!isspace(c) && c != EOF &&
                   !(c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}'))
            {
                addCharToToken(&token, c, &token_len);
                c = (char)fgetc(fp);
                col++;
            }
            ungetc(c, fp);
            col--;
        }
        else
        {
            addCharToToken(&token, c, &token_len);
            c = (char)fgetc(fp);
            col++;

            while (c != '\"' && c != EOF)
            {
                if (c == '\\')
                {
                    addCharToToken(&token, c, &token_len);
                    c = (char)fgetc(fp);
                    col++;

                    if (c != EOF)
                    {
                        addCharToToken(&token, c, &token_len);
                    }
                }
                else
                {
                    addCharToToken(&token, c, &token_len);
                }

                c = (char)fgetc(fp);
                col++;
            }
            if (c == '\"')
            {
                addCharToToken(&token, c, &token_len);
            }
        }
        t.rowNo = row;
        t.columnNo = col - token_len + 1;
        t.type = (checkToken(token));
        t.lexeme = strdup(token);
        addElement(&tokenArray, &listLength, t);
        free(token);
    }
    // printTokens(tokenArray, & listLength);
    fclose(fp);
    return tokenArray;
}

// Parser

bool Program();
bool TopLevelForm(int indentLevel);
bool SecondLevelForm(int indentLevel);
bool Definition(int indentLevel);
bool DefinitionRight(int indentLevel);
bool ArgList(int indentLevel);
bool Statements(int indentLevel);
bool Expressions(int indentLevel);
bool Expression(int indentLevel);
bool Expr(int indentLevel);
bool FunCall(int indentLevel);
bool LetExpression(int indentLevel);
bool LetExpr(int indentLevel);
bool VarDefs(int indentLevel);
bool VarDef(int indentLevel);
bool CondExpression(int indentLevel);
bool CondBranches(int indentLevel);
bool CondBranch(int indentLevel);
bool IfExpression(int indentLevel);
bool EndExpression(int indentLevel);
bool BeginExpression(int indentLevel);

int tokenIndex;

struct ParseNode
{
    int indent;
    char *string;
} typedef ParseNode;

tokenStruct *tokenList = NULL;
ParseNode **parseNodeList = NULL;
int parseNodeListSize = 0;

void addParseNode(int indent, char *string)
{
    parseNodeListSize++;
    parseNodeList = (ParseNode **)realloc(parseNodeList, parseNodeListSize * sizeof(ParseNode *));

    ParseNode *newNode = (ParseNode *)malloc(sizeof(ParseNode));
    newNode->indent = indent;
    newNode->string = string;

    parseNodeList[parseNodeListSize - 1] = newNode;
}

void removeParseNode()
{
    if (parseNodeListSize == 0)
    {
        printf("Error: List is empty!\n");
        return;
    }

    parseNodeListSize--;
    parseNodeList = (ParseNode **)realloc(parseNodeList, parseNodeListSize * sizeof(ParseNode *));
}

void printIndent(int indent, FILE *file)
{
    int i = 0;
    while (i < indent)
    {
        printf(" ");
        fprintf(file, " ");
        i++;
    }
}

char *setTokenStr(tokenStruct token)
{
    unsigned long long len = strlen(token.type) + strlen(token.lexeme) + 4;
    char *str = (char *)malloc(len * sizeof(char));
    sprintf(str, "%s (%s)", token.type, token.lexeme);

    return str;
}

bool compareStr(const char *str1,
                const char *str2)
{
    return strcmp(str1, str2) == 0;
}

// Implementing given grammar rules  of PPLL.
bool Program()
{
    if (listLength == 0)
    {
        printf("<Program>\n<Program>\n  _\n"); // Empty case.
        return true;
    }

    if (!compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(0, "<Program>");
        addParseNode(1, "_");
        return true; // First <Program>
    }

    addParseNode(0, "<Program>");
    if (TopLevelForm(1) && Program())
        return true; // Last <Program>

    return false;
}

bool TopLevelForm(int indentLevel)
{
    addParseNode(indentLevel, "<TopLevelForm>");
    if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++; // Incrementing tokenIndex means passing to the nextToken.
        if (SecondLevelForm(indentLevel + 1))
        { // Calling second level form function with indent level of top level form + 1
            if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
            {
                addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                tokenIndex++;
                return true;
            }
        }
    }
    removeParseNode(); // Remove if it is not true.
    return false;
}

bool SecondLevelForm(int indentLevel)
{
    addParseNode(indentLevel, "<SecondLevelForm>");
    if (Definition(indentLevel + 1))
    {
        return true;
    }
    else if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (FunCall(indentLevel + 1))
        {
            if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
            {
                addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                tokenIndex++;
                return true;
            }
        }
    }
    removeParseNode();
    return false;
}
bool Definition(int indentLevel)
{
    addParseNode(indentLevel, "<Definition>");
    if (compareStr(tokenList[tokenIndex].type, "DEFINE"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (DefinitionRight(indentLevel + 1))
        {
            return true;
        }
    }
    removeParseNode();
    return false;
}
bool DefinitionRight(int indentLevel)
{
    addParseNode(indentLevel, "<DefinitionRight>");
    if (compareStr(tokenList[tokenIndex].type, "IDENTIFIER"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (Expression(indentLevel + 1))
        {
            return true;
        }
    }
    else if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (compareStr(tokenList[tokenIndex].type, "IDENTIFIER"))
        {
            addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
            tokenIndex++;
            if (ArgList(indentLevel + 1))
            {
                if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
                {
                    addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                    tokenIndex++;
                    if (Statements(indentLevel + 1))
                    {
                        return true;
                    }
                }
            }
        }
    }
    removeParseNode();
    return false;
}
int argListCount = 0;
bool ArgList(int indentLevel)
{
    addParseNode(indentLevel, "<ArgList>");
    if (compareStr(tokenList[tokenIndex].type, "IDENTIFIER"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        argListCount++;
        if (ArgList(indentLevel + 1))
            return true;
    }
    else if (!compareStr(tokenList[tokenIndex].type, "IDENTIFIER"))
    {
        addParseNode(indentLevel + 1, "_");
        return true;
    }
    return false;
}

bool Statements(int indentLevel)
{
    addParseNode(indentLevel, "<Statements>");
    if (Expression(indentLevel + 1))
    {
        return true;
    }
    else if (Definition(indentLevel + 1))
    {
        if (Statements(indentLevel + 1))
        {
            return true;
        }
    }
    removeParseNode();
    return false;
}

bool Expressions(int indentLevel)
{
    addParseNode(indentLevel, "<Expressions>");
    if (Expression(indentLevel + 1))
    {
        if (Expressions(indentLevel + 1))
        {
            return true;
        }
    }
    addParseNode(indentLevel + 1, "_");
    return true;
}

bool Expression(int indentLevel)
{
    addParseNode(indentLevel, "<Expression>");
    if (compareStr(tokenList[tokenIndex].type, "IDENTIFIER") ||
        compareStr(tokenList[tokenIndex].type, "NUMBER") || compareStr(tokenList[tokenIndex].type, "CHAR") ||
        compareStr(tokenList[tokenIndex].type, "BOOLEAN") || compareStr(tokenList[tokenIndex].type, "STRING"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        return true;
    }
    else if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (Expr(indentLevel + 1))
        {
            if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
            {
                addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                tokenIndex++;
                return true;
            }
        }
    }
    removeParseNode();
    return false;
}

bool Expr(int indentLevel)
{
    addParseNode(indentLevel, "<Expr>");
    if (LetExpression(indentLevel + 1) || CondExpression(indentLevel + 1) ||
        IfExpression(indentLevel + 1) || BeginExpression(indentLevel + 1) || FunCall(indentLevel + 1))
    {
        return true;
    }
    removeParseNode();
    return false;
}

bool FunCall(int indentLevel)
{
    addParseNode(indentLevel, "<FunCall>");
    if (compareStr(tokenList[tokenIndex].type, "IDENTIFIER"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (Expressions(indentLevel + 1))
        {
            return true;
        }
    }
    removeParseNode();
    return false;
}

bool LetExpression(int indentLevel)
{
    addParseNode(indentLevel, "<LetExpression>");
    if (compareStr(tokenList[tokenIndex].type, "LET"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (LetExpr(indentLevel + 1))
        {
            return true;
        }
    }
    removeParseNode();
    return false;
}

bool LetExpr(int indentLevel)
{
    addParseNode(indentLevel, "<LetExpr>");
    if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (VarDefs(indentLevel + 1))
        {
            if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
            {
                addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                tokenIndex++;
                if (Statements(indentLevel + 1))
                {
                    return true;
                }
            }
        }
    }
    else if (compareStr(tokenList[tokenIndex].type, "IDENTIFIER"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
        {
            addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
            tokenIndex++;
            if (VarDefs(indentLevel + 1))
            {
                if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
                {
                    addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                    tokenIndex++;
                    if (Statements(indentLevel + 1))
                    {
                        return true;
                    }
                }
            }
        }
    }
    removeParseNode();
    return false;
}

bool VarDefs(int indentLevel)
{
    addParseNode(indentLevel, "<VarDefs>");
    if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (compareStr(tokenList[tokenIndex].type, "IDENTIFIER"))
        {
            addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
            tokenIndex++;
            if (Expression(indentLevel + 1))
            {
                if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
                {
                    addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                    tokenIndex++;
                    if (VarDef(indentLevel + 1))
                    {
                        return true;
                    }
                }
            }
        }
    }
    removeParseNode();
    return false;
}

bool VarDef(int indentLevel)
{
    addParseNode(indentLevel, "<VarDef>");

    if (!compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, "_");
        return true;
    }
    if (VarDefs(indentLevel + 1))
    {
        return true;
    }
    return false;
}

bool CondExpression(int indentLevel)
{
    addParseNode(indentLevel, "<CondExpression>");
    if (compareStr(tokenList[tokenIndex].type, "COND"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (CondBranches(indentLevel + 1))
        {
            return true;
        }
    }
    removeParseNode();
    return false;
}

bool CondBranches(int indentLevel)
{
    addParseNode(indentLevel, "<CondBranches>");
    if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (Expression(indentLevel + 1))
        {
            if (Statements(indentLevel + 1))
            {
                if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
                {
                    addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                    tokenIndex++;
                    if (CondBranch(indentLevel + 1))
                    {
                        return true;
                    }
                }
            }
        }
    }
    removeParseNode();
    return false;
}

bool CondBranch(int indentLevel)
{
    addParseNode(indentLevel, "<CondBranch>");

    if (!compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, "_");
        return true;
    }
    else if (compareStr(tokenList[tokenIndex].type, "LEFTPAR"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (Expression(indentLevel + 1))
        {
            if (Statements(indentLevel + 1))
            {
                if (compareStr(tokenList[tokenIndex].type, "RIGHTPAR"))
                {
                    addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
                    tokenIndex++;
                    return true;
                }
            }
        }
    }
    addParseNode(indentLevel + 1, "_");
    return true;
}

bool IfExpression(int indentLevel)
{
    addParseNode(indentLevel, "<IfExpression>");
    if (compareStr(tokenList[tokenIndex].type, "IF"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (Expression(indentLevel + 1))
        {
            if (Expression(indentLevel + 1))
            {
                if (EndExpression(indentLevel + 1))
                {
                    return true;
                }
            }
        }
    }
    removeParseNode();
    return false;
}

bool EndExpression(int indentLevel)
{
    addParseNode(indentLevel, "<EndExpression>");
    if (Expression(indentLevel + 1))
    {
        return true;
    }
    addParseNode(indentLevel + 1, "_");
    return true;
}

bool BeginExpression(int indentLevel)
{
    addParseNode(indentLevel, "<BeginExpression>");
    if (compareStr(tokenList[tokenIndex].type, "BEGIN"))
    {
        addParseNode(indentLevel + 1, setTokenStr(tokenList[tokenIndex]));
        tokenIndex++;
        if (Statements(indentLevel + 1))
        {
            return true;
        }
    }
    removeParseNode();
    return false;
}
// This function prints the parse tree by using indent and string values of parseNodeList, this list keeps the data in the recursive function order.
void printParseTree(FILE *file)
{
    int i;
    for (i = 0; i < parseNodeListSize; i++)
    {
        printIndent(parseNodeList[i]->indent, file);
        printf("%s\n", parseNodeList[i]->string);
        fprintf(file, "%s\n", parseNodeList[i]->string);
    }
}

/*This function checks if there exist any unexpected brackets. If there exists it deletes
   all tokens after the unexpected token and revise the listToken. So parser only read values
   from start to unexpected token.*/
bool checkOtherBrackets()
{

    int i;
    for (i = 0; i < listLength; i++)
    {
        if (compareStr(tokenList[i].lexeme, "[") ||
            compareStr(tokenList[i].lexeme, "]") ||
            compareStr(tokenList[i].lexeme, "{") ||
            compareStr(tokenList[i].lexeme, "}"))
        {

            int newSize = i + 1;
            tokenStruct *newList = malloc(newSize * sizeof(tokenStruct));

            int j;
            for (j = 0; j < newSize; j++)
            {
                newList[j] = tokenList[j];
            }

            free(tokenList);
            listLength = newSize;
            tokenList = newList;

            return false;
        }
    }
    return true;
}

int main()
{

    char filename[100];

    printf("Enter the name of the input file:");
    scanf("%s", filename);

    tokenList = processFile(filename);   // Store tokens in a list. We get the list from the first project. That is list of tokens.
    bool isValid = checkOtherBrackets(); // Check if there exist {,},[,]
    FILE *file;
    file = fopen("output.txt", "w");
    if (file == NULL)
    {
        printf("Error: could not open file output.txt\n");
        exit(0);
    }
    Program();            // Start parsing
    printParseTree(file); // Print parse tree

    if (!isValid)
    {
        printf("SYNTAX ERROR [%d:%d]: '%s' is not a valid character.\n", tokenList[listLength - 1].rowNo,
               tokenList[listLength - 1].columnNo, tokenList[listLength - 1].lexeme);
        fprintf(file, "SYNTAX ERROR [%d:%d]: '%s' is not a valid character.\n", tokenList[listLength - 1].rowNo,
                tokenList[listLength - 1].columnNo, tokenList[listLength - 1].lexeme);
    }
    return 0;
}
