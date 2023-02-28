/**
 * @file lex_automata.hpp
 * @author George S. (https://github.com/TorgaW)
 * @brief sigle-header FSM for lexing (UTF-8 support)
 * @version 1.0
 * @date 2023-02-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef LEX_AUTOMATA_HPP
#define LEX_AUTOMATA_HPP

#include <iostream>
#include <vector>
#include <stdexcept>

/**
 * @brief uses basic vector to create "byte-vector"
 * 
 */
typedef std::vector<unsigned char> bytes;

/**
 * @brief token types
 * 
 */
enum Tokens {
    //special types of tokens
    ID, NUMBER, FLNUMBER, CHAR, STRING,

    //all possible types of data in Theia language
    TYPE_BOOL,
    TYPE_BYTE, TYPE_SHORT, TYPE_INT, TYPE_LONG,
    TYPE_UINT32, TYPE_UINT64, TYPE_UINT128, TYPE_UINT256,
    TYPE_DOUBLE,
    TYPE_CHAR, TYPE_STRING,
    TYPE_WADDRESS,
    TYPE_VOID,

    //all keywords in Theia language
    KW_CLASS, KW_EXTENDS, KW_PUBLIC, KW_PRIVATE, KW_CONST,
    KW_IF, KW_ELSE, KW_RETURN, KW_BREAK, KW_WHILE, KW_CONTINUE,
    KW_ENUM, KW_SWITCH, KW_CASE, KW_DEFAULT, KW_FOR,

    /**
     * @brief all possible operators in Theia language
     * 
     */
    //basic math
    //    =       +         -       *       /       %       ++      --
    OP_ASSIGN, OP_PLUS, OP_MINUS, OP_MUL, OP_DIV, OP_MOD, OP_INC, OP_DEC,
    //      +=            -=            *=              /=          %=
    OP_PLUSASSIGN, OP_MINUSASSIGN, OP_MULASSIGN, OP_DIVASSIGN, OP_MODASSIGN,
    //logical
    //==        <       >           <=      >=      !       &&      ||
    OP_EQL, OP_LESS, OP_BIGGER, OP_LSEQ, OP_BGEQ, OP_NOT, OP_AND, OP_OR,
    //bitwise
    //  ~         <<          >>         ^          &       |
    OP_B_NOT, OP_B_SHFTL, OP_B_SHFTR, OP_B_XOR, OP_B_AND, OP_B_OR,
    //dot operator
    OP_DOT,

    /**
     * @brief brackets
     * 
     */
//    (        )       {        }       [         ]
   BRKT_L, BRKT_R, BRACE_L, BRACE_R, SQBRKT_L, SQBRKT_R,

    /**
     * @brief miscellaneous
     * 
     */
    MES_COMMA,//    ,
    MES_SEMI,//     ;
    MES_COLON,//    :

    /**
     * @brief for comments: "/_* ...  *_/" (replace _ with "") - will be the best option
     * COMMENT_OPEN, COMMENT_CLOSE
     */
};

const std::string stringTokens[72] = {
        //special types of tokens
    "ID", "NUMBER", "FLNUMBER", "CHAR", "STRING",

    //all possible types of data in Theia language
    "TYPE_BOOL",
    "TYPE_BYTE", "TYPE_SHORT", "TYPE_INT", "TYPE_LONG",
    "TYPE_UINT32", "TYPE_UINT64", "TYPE_UINT128", "TYPE_UINT256",
    "TYPE_DOUBLE",
    "TYPE_CHAR", "TYPE_STRING",
    "TYPE_WADDRESS",
    "TYPE_VOID",

    //all keywords in Theia language
    "KW_CLASS", "KW_EXTENDS", "KW_PUBLIC", "KW_PRIVATE", "KW_CONST",
    "KW_IF", "KW_ELSE", "KW_RETURN", "KW_BREAK", "KW_WHILE", "KW_CONTINUE",
    "KW_ENUM", "KW_SWITCH", "KW_CASE", "KW_DEFAULT", "KW_FOR",

    /**
     * @brief all possible operators in Theia language
     * 
     */
    //basic math
    //    =       +         -       *       /       %
    "OP_ASSIGN", "OP_PLUS", "OP_MINUS", "OP_MUL", "OP_DIV", "OP_MOD", "OP_INC", "OP_DEC",
    //      +=            -=            *=              /=          %=
    "OP_PLUSASSIGN", "OP_MINUSASSIGN", "OP_MULASSIGN", "OP_DIVASSIGN", "OP_MODASSIGN",
    //logical
    //==        <       >           <=      >=                  !       &&          ||
    "OP_EQL", "OP_LESS", "OP_BIGGER", "OP_LSEQ", "OP_BGEQ", "OP_NOT", "OP_AND", "OP_OR", 
    //bitwise
    //  ~         <<          >>         ^          &       |
    "OP_B_NOT", "OP_B_SHFTL", "OP_B_SHFTR", "OP_B_XOR", "OP_B_AND", "OP_B_OR",
    //dot operator
    "OP_DOT",

    /**
     * @brief brackets
     * 
     */
//    (        )       {        }       [         ]
    "BRKT_L", "BRKT_R", "BRACE_L", "BRACE_R", "SQBRKT_L", "SQBRKT_R",

    /**
     * @brief miscellaneous
     * 
     */
    "MES_COMMA",//    ,
    "MES_SEMI",//     ;
    "MES_COLON",//    :

    /**
     * @brief for comments: "/_* ...  *_/" (replace _ with "") - will be the best option
     * COMMENT_OPEN, COMMENT_CLOSE
     */
};

class LexToken
{
private:
    bytes data;
    Tokens type;
    long lineNum;
    long lineCol;
public:
    LexToken(bytes _data, Tokens _type, long _lineNum, long _lineCol);
    ~LexToken();
    inline bytes getData() {return data;}
    inline Tokens getType() {return type;}
    inline long getLn() {return lineNum;}
    inline long getCol() {return lineCol;}
};

LexToken::LexToken(bytes _data, Tokens _type, long _lineNum, long _lineCol)
{
    data =_data;
    type = _type;
    lineNum = _lineNum;
    lineCol = _lineCol;
}

LexToken::~LexToken()
{
}


class LexAutomata
{
private:
    FILE* f;
    bytes buffer = {};
    bytes lineBuffer = {};
    bool exponentNumber = false;
    bool isChar = false;
    bool isString = false;
    int currentByte;
    long lineNum;
    long lineCol;
    long fileLength;
    std::string parsingState;
public:
    /**
     * @brief Construct a new Lex Automata object
     * 
     * @param _f pointer to file
     */
    LexAutomata(FILE* _f);

    /**
     * @brief Construct a new Lex Automata object
     * 
     * @param _path path to file
     */
    LexAutomata(std::string _path);
    ~LexAutomata();

    void scanTokens(std::vector<LexToken>* _dest);

private:
    /**
     * @brief checks for [a-zA-Z]
     * 
     * @param _c byte
     */
    inline bool isAlpha(unsigned char _c) {return ((_c >= 'a' && _c <= 'z') || (_c >= 'A' && _c <= 'Z'));}

    /**
     * @brief checks for [0-9]
     * 
     * @param _c byte
     */
    inline bool isNumber(unsigned char _c) {return (_c >= '0' && _c <= '9');}

    /**
     * @brief checks for [' ', '\\n', '\\t', '\\r', '\\v']
     * 
     * @param _c byte
     */
    inline bool isSpace(unsigned char _c) {return _c == ' ' || _c <= '\n' || _c == '\t' || _c == '\r' || _c == '\v';}

    /**
     * @brief checks for operators
     * 
     * @param _c byte
     */
    inline bool isOperator(unsigned char _c) {return _c == '=' || _c == '+' || _c == '-' || _c == '*' || _c == '/' || _c == '%' || _c == '&' || _c == '|' || _c == '^' || _c == '~' || _c == '.';}

    /**
     * @brief checks for quotes
     * 
     * @param _c byte
     */
    inline bool isQuotes(unsigned char _c) {return _c == '\'' || _c == '"';}

    /**
     * @brief checks for brackets
     * 
     * @param _c byte
     */
    inline bool isBrackets(unsigned char _c) {return _c == '{' || _c == '}' || _c == '(' || _c == ')' || _c == '[' || _c == ']';}

    /**
     * @brief compare bytes with string
     * 
     * @param _source 
     * @param _with 
     */
    bool compare(bytes _source, std::string _with)
    {
        if(_source.size() != _with.size()) return false;
        for (size_t i = 0; i < _source.size(); i++)
        {
            if(_source[i] != _with[i]) return false;
        }
        return true;
    }

    /**
     * @brief Get the Next Byte object
     * 
     */
    inline void getNextByte() {
        currentByte = getc(f);
        lineCol++;
        lineBuffer.push_back(currentByte);
    };

    /**
     * @brief Return back current byte
     * 
     */
    inline void ungetByte() {
        currentByte = ungetc(currentByte, f);
        lineCol--;
        lineBuffer.pop_back();
    };

};

LexAutomata::LexAutomata(FILE* _f)
{
    if(_f != nullptr)
    {
        f = _f;
        fseek(f, 0, SEEK_END);
        fileLength = ftell(f);
        rewind(f);
        buffer = {};
        lineBuffer = {};
        exponentNumber = false;
        isChar = false;
        isString = false;
        currentByte = 0;
        lineNum = 1;
        lineCol = 0;
        parsingState = "";
    }
    else throw std::invalid_argument("argument '_f' is not invalid");
}

LexAutomata::LexAutomata(std::string _path)
{
    f = fopen(_path.c_str(), "rb");
    if(f != nullptr)
    {
        fseek(f, 0, SEEK_END);
        fileLength = ftell(f);
        rewind(f);
        buffer = {};
        lineBuffer = {};
        exponentNumber = false;
        isChar = false;
        isString = false;
        currentByte = 0;
        lineNum = 1;
        lineCol = 0;
        parsingState = "";
    }
    else throw std::invalid_argument("argument '_path' is not invalid");
}

LexAutomata::~LexAutomata()
{
    delete f;
}

void LexAutomata::scanTokens(std::vector<LexToken> *_dest)
{
    if(_dest == nullptr) throw std::invalid_argument("argument '_dest' is invalid");
    bool signedExponent = false;

    goto START;

    SELECT_NEXT:
    {
        parsingState = "Selecting next routine";
        if(isAlpha(currentByte)) goto ALPHABET;
        if(isSpace(currentByte)) goto SPACE;
        if(isBrackets(currentByte)) goto BRACKETS;
        if(isNumber(currentByte)) goto NUMBER;
        if(isOperator(currentByte)) goto OPERATOR;
        if(isQuotes(currentByte)) goto STRING;
        if(currentByte == ',' || currentByte == ';' || currentByte == ':') goto MES;
        if(currentByte == EOF) goto AUTOMATA_END;
        goto ERROR;
    }

    START:
    {
        getNextByte();
        goto SELECT_NEXT;
    }

    NUMBER:
    {   
        parsingState = "Parsing number";
        buffer.push_back(currentByte);
        getNextByte();
        if(isNumber(currentByte)) goto NUMBER;
        if(currentByte == '.') goto NUMBERDOT;
        if(isAlpha(currentByte)) goto ERROR;
        _dest->push_back(LexToken(buffer, Tokens::NUMBER, lineNum, lineCol));
        buffer = {};
        goto SELECT_NEXT;
    }

    NUMBERDOT:
    {   
        getNextByte();
        if(isNumber(currentByte))
        {
            buffer.push_back('.');
            goto MANTISSA;
        }
        goto ERROR;
    }

    MANTISSA:
    {
        parsingState = "Parsing mantissa";
        buffer.push_back(currentByte);
        getNextByte();
        if(isNumber(currentByte)) goto MANTISSA;
        if(currentByte == 'e' || currentByte == 'E') goto MNTSEXP;
        if(isAlpha(currentByte)) goto ERROR;
        _dest->push_back(LexToken(buffer, Tokens::FLNUMBER, lineNum, lineCol));
        buffer = {};
        goto SELECT_NEXT;
    }

    MNTSEXP:
    {
        parsingState = "Parsing exp mantissa";
        //this will be 'e' or 'E' at first time
        buffer.push_back(currentByte);
        getNextByte();
        if(currentByte == '-' || isNumber(currentByte))
        {
            exponentNumber = true;
            if(currentByte == '-')
            {
                if(signedExponent)
                {
                    _dest->push_back(LexToken(buffer, Tokens::FLNUMBER, lineNum, lineCol));
                    buffer = {};
                    exponentNumber = false;
                    signedExponent = false;
                    goto SELECT_NEXT;
                }
                signedExponent = true;
                buffer.push_back(currentByte);
                getNextByte();
                if(isNumber(currentByte)) goto MNTSEXP;
                else goto ERROR;
            }
            goto MNTSEXP;
        }
        if(isAlpha(currentByte)) goto ERROR;
        _dest->push_back(LexToken(buffer, Tokens::FLNUMBER, lineNum, lineCol));
        buffer = {};
        exponentNumber = false;
        signedExponent = false;
        goto SELECT_NEXT;
    }

    SPACE:
    {
        if(currentByte == '\n') 
        {
            lineNum++;
            lineCol = 0;
            lineBuffer = {};
        }
        getNextByte();
        goto SELECT_NEXT;
    }

    ALPHABET:
    {
        parsingState = "Parsing literals";
        buffer.push_back(currentByte);
        getNextByte();
        if(isAlpha(currentByte) || isNumber(currentByte)) goto ALPHABET;
        else
        {
            switch (buffer.size())
            {
            case 0:
                goto ERROR;
            case 1:
                _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
                break;
            case 2:
                if(compare(buffer, "if")) _dest->push_back(LexToken(buffer, Tokens::KW_IF, lineNum, lineCol - 1));
                else _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
                break;
            case 3:
                if(compare(buffer, "int")) _dest->push_back(LexToken(buffer, Tokens::TYPE_INT, lineNum, lineCol - 1));
                else if(compare(buffer, "for")) _dest->push_back(LexToken(buffer, Tokens::KW_FOR, lineNum, lineCol - 1));
                else _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol));
                break;
            case 4:
                if(compare(buffer, "bool")) _dest->push_back(LexToken(buffer, Tokens::TYPE_BOOL, lineNum, lineCol - 1));
                else if(compare(buffer, "byte")) _dest->push_back(LexToken(buffer, Tokens::TYPE_BYTE, lineNum, lineCol - 1));
                else if(compare(buffer, "long")) _dest->push_back(LexToken(buffer, Tokens::TYPE_LONG, lineNum, lineCol - 1));
                else if(compare(buffer, "char")) _dest->push_back(LexToken(buffer, Tokens::TYPE_CHAR, lineNum, lineCol - 1));
                else if(compare(buffer, "void")) _dest->push_back(LexToken(buffer, Tokens::TYPE_VOID, lineNum, lineCol - 1));
                else if(compare(buffer, "else")) _dest->push_back(LexToken(buffer, Tokens::KW_ELSE, lineNum, lineCol - 1));
                else if(compare(buffer, "enum")) _dest->push_back(LexToken(buffer, Tokens::KW_ENUM, lineNum, lineCol - 1));
                else if(compare(buffer, "case")) _dest->push_back(LexToken(buffer, Tokens::KW_CASE, lineNum, lineCol - 1));
                else _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
                break;
            case 5:
                if(compare(buffer, "short")) _dest->push_back(LexToken(buffer, Tokens::TYPE_SHORT, lineNum, lineCol - 1));
                else if(compare(buffer, "class")) _dest->push_back(LexToken(buffer, Tokens::KW_CLASS, lineNum, lineCol - 1));
                else if(compare(buffer, "const")) _dest->push_back(LexToken(buffer, Tokens::KW_CONST, lineNum, lineCol - 1));
                else if(compare(buffer, "break")) _dest->push_back(LexToken(buffer, Tokens::KW_BREAK, lineNum, lineCol - 1));
                else if(compare(buffer, "while")) _dest->push_back(LexToken(buffer, Tokens::KW_WHILE, lineNum, lineCol - 1));
                else _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
                break;
            case 6:
                if(compare(buffer, "uint32")) _dest->push_back(LexToken(buffer, Tokens::TYPE_UINT32, lineNum, lineCol - 1));
                else if(compare(buffer, "uint64")) _dest->push_back(LexToken(buffer, Tokens::TYPE_UINT64, lineNum, lineCol - 1));
                else if(compare(buffer, "double")) _dest->push_back(LexToken(buffer, Tokens::TYPE_DOUBLE, lineNum, lineCol - 1));
                else if(compare(buffer, "string")) _dest->push_back(LexToken(buffer, Tokens::STRING, lineNum, lineCol - 1));
                else if(compare(buffer, "public")) _dest->push_back(LexToken(buffer, Tokens::KW_PUBLIC, lineNum, lineCol - 1));
                else if(compare(buffer, "return")) _dest->push_back(LexToken(buffer, Tokens::KW_RETURN, lineNum, lineCol - 1));
                else if(compare(buffer, "switch")) _dest->push_back(LexToken(buffer, Tokens::KW_SWITCH, lineNum, lineCol - 1));
                else _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
                break;
            case 7:
                if(compare(buffer, "uint128")) _dest->push_back(LexToken(buffer, Tokens::TYPE_UINT128, lineNum, lineCol - 1));
                else if(compare(buffer, "uint256")) _dest->push_back(LexToken(buffer, Tokens::TYPE_UINT256, lineNum, lineCol - 1));
                else if(compare(buffer, "extends")) _dest->push_back(LexToken(buffer, Tokens::KW_EXTENDS, lineNum, lineCol - 1));
                else if(compare(buffer, "private")) _dest->push_back(LexToken(buffer, Tokens::KW_PRIVATE, lineNum, lineCol - 1));
                else if(compare(buffer, "default")) _dest->push_back(LexToken(buffer, Tokens::KW_DEFAULT, lineNum, lineCol - 1));
                else _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
                break;
            case 8:
                if(compare(buffer, "waddress")) _dest->push_back(LexToken(buffer, Tokens::TYPE_WADDRESS, lineNum, lineCol - 1));
                else if(compare(buffer, "continue")) _dest->push_back(LexToken(buffer, Tokens::KW_CONTINUE, lineNum, lineCol - 1));
                else _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
                break;
            default:
                _dest->push_back(LexToken(buffer, Tokens::ID, lineNum, lineCol - 1));
            }
            buffer = {};
            goto SELECT_NEXT;
        }
    }

    MES:
    {
        parsingState = "Miscellaneous lexing";
        buffer.push_back(currentByte);
        if(currentByte == ',') _dest->push_back(LexToken(buffer, Tokens::MES_COMMA, lineNum, lineCol));
        else if(currentByte == ';') _dest->push_back(LexToken(buffer, Tokens::MES_SEMI, lineNum, lineCol));
        else if(currentByte == ':') _dest->push_back(LexToken(buffer, Tokens::MES_COLON, lineNum, lineCol));
        else goto ERROR;
        buffer = {};
        getNextByte();
        goto SELECT_NEXT;
    }

    BRACKETS:
    {
        buffer.push_back(currentByte);
        parsingState = "Brackets lexing";
        if(currentByte == '{') _dest->push_back(LexToken(buffer, Tokens::BRACE_L, lineNum, lineCol));
        else if(currentByte == '}') _dest->push_back(LexToken(buffer, Tokens::BRACE_R, lineNum, lineCol));
        else if(currentByte == '(') _dest->push_back(LexToken(buffer, Tokens::BRKT_L, lineNum, lineCol));
        else if(currentByte == ')') _dest->push_back(LexToken(buffer, Tokens::BRKT_R, lineNum, lineCol));
        else if(currentByte == '[') _dest->push_back(LexToken(buffer, Tokens::SQBRKT_L, lineNum, lineCol));
        else if(currentByte == ']') _dest->push_back(LexToken(buffer, Tokens::SQBRKT_R, lineNum, lineCol));
        else goto ERROR;
        buffer = {};
        getNextByte();
        goto SELECT_NEXT;
    }

    OPERATOR:
    {
        parsingState = "Parsing operator";
        buffer.push_back(currentByte);
        if(buffer.size() == 1 && (buffer[0] == '^' || buffer[0] == '~' || buffer[0] == '.'))
        {
            getNextByte();
            if(isOperator(currentByte)) goto ERROR;
            if(buffer[0] == '.' && isNumber(currentByte))
            {
                buffer = {};
                ungetByte();
                // buffer.push_back(currentByte);
                goto NUMBERDOT;
            }
            if(buffer[0] == '^') _dest->push_back(LexToken(buffer, Tokens::OP_B_XOR, lineNum, lineCol));
            else if(buffer[0] == '~') _dest->push_back(LexToken(buffer, Tokens::OP_B_NOT, lineNum, lineCol));
            else if(buffer[0] == '.') _dest->push_back(LexToken(buffer, Tokens::OP_DOT, lineNum, lineCol));
            else goto ERROR;
            buffer = {};
            goto SELECT_NEXT;
        }
        getNextByte();
        if(isOperator(currentByte))
        {
            buffer.push_back(currentByte);
            if(buffer[0] == '=' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_EQL, lineNum, lineCol));
            else if(buffer[0] == '-' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_MINUSASSIGN, lineNum, lineCol));
            else if(buffer[0] == '+' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_PLUSASSIGN, lineNum, lineCol));
            else if(buffer[0] == '*' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_MULASSIGN, lineNum, lineCol));
            else if(buffer[0] == '/' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_DIVASSIGN, lineNum, lineCol));
            else if(buffer[0] == '%' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_MODASSIGN, lineNum, lineCol));
            else if(buffer[0] == '+' && buffer[1] == '+')
                _dest->push_back(LexToken(buffer, Tokens::OP_INC, lineNum, lineCol));
            else if(buffer[0] == '-' && buffer[1] == '-')
                _dest->push_back(LexToken(buffer, Tokens::OP_DEC, lineNum, lineCol));
            else if(buffer[0] == '>' && buffer[1] == '>')
                _dest->push_back(LexToken(buffer, Tokens::OP_B_SHFTR, lineNum, lineCol));
            else if(buffer[0] == '<' && buffer[1] == '<')
                _dest->push_back(LexToken(buffer, Tokens::OP_B_SHFTL, lineNum, lineCol));
            else if(buffer[0] == '>' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_BGEQ, lineNum, lineCol));
            else if(buffer[0] == '<' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_LSEQ, lineNum, lineCol));
            else if(buffer[0] == '&' && buffer[1] == '&')
                _dest->push_back(LexToken(buffer, Tokens::OP_AND, lineNum, lineCol));
            else if(buffer[0] == '|' && buffer[1] == '|')
                _dest->push_back(LexToken(buffer, Tokens::OP_OR, lineNum, lineCol));
            else if(buffer[0] == '/' && buffer[1] == '*')
            {
                lineBuffer = {};
                buffer = {};
                goto COMMENT;
            }
            else goto ERROR;

            buffer = {};
            getNextByte();
            goto SELECT_NEXT;
        }
        else
        {
            if(buffer[0] == '=') _dest->push_back(LexToken(buffer, Tokens::OP_ASSIGN, lineNum, lineCol));
            else if(buffer[0] == '+') _dest->push_back(LexToken(buffer, Tokens::OP_PLUS, lineNum, lineCol));
            else if(buffer[0] == '-') _dest->push_back(LexToken(buffer, Tokens::OP_MINUS, lineNum, lineCol));
            else if(buffer[0] == '*') _dest->push_back(LexToken(buffer, Tokens::OP_MUL, lineNum, lineCol));
            else if(buffer[0] == '/') _dest->push_back(LexToken(buffer, Tokens::OP_DIV, lineNum, lineCol));
            else if(buffer[0] == '%') _dest->push_back(LexToken(buffer, Tokens::OP_MOD, lineNum, lineCol));
            else if(buffer[0] == '^') _dest->push_back(LexToken(buffer, Tokens::OP_B_XOR, lineNum, lineCol));
            else if(buffer[0] == '~') _dest->push_back(LexToken(buffer, Tokens::OP_B_NOT, lineNum, lineCol));
            else if(buffer[0] == '&') _dest->push_back(LexToken(buffer, Tokens::OP_B_AND, lineNum, lineCol));
            else if(buffer[0] == '|') _dest->push_back(LexToken(buffer, Tokens::OP_B_OR, lineNum, lineCol));
            else if(buffer[0] == '>') _dest->push_back(LexToken(buffer, Tokens::OP_BIGGER, lineNum, lineCol));
            else if(buffer[0] == '<') _dest->push_back(LexToken(buffer, Tokens::OP_LESS, lineNum, lineCol));
            else if(buffer[0] == '.') _dest->push_back(LexToken(buffer, Tokens::OP_DOT, lineNum, lineCol));
            else goto ERROR;

            buffer = {};
            goto SELECT_NEXT;
        }
    }

    //TO DO utf-8 support
    STRING:
    {
        parsingState = "Parsing string";
        if(currentByte == '\n') 
        {
            lineNum++;
            lineCol = 0;
            lineBuffer = {};
        }
        if(currentByte == '\'' && !isString)
        {
            if(isChar)
            {
                _dest->push_back(LexToken(buffer, Tokens::CHAR, lineNum, lineCol));
                buffer = {};
                getNextByte();
                isChar = false;
                goto SELECT_NEXT;
            }
            else
            {
                isChar = true;
                getNextByte();
                goto STRING;
            }
        }
        else if(currentByte == '"' && !isChar)
        {
            if(isString)
            {
                _dest->push_back(LexToken(buffer, Tokens::STRING, lineNum, lineCol));
                buffer = {};
                getNextByte();
                isString = false;
                goto SELECT_NEXT;
            }
            else
            {
                isString = true;
                getNextByte();
                goto STRING;
            }
        }
        else if(currentByte != EOF)
        {
            buffer.push_back(currentByte);
            if(isChar)
            {
                if(buffer.size() > 2)
                {
                    parsingState = "Char must be only one symbol";
                    goto ERROR;
                }
                if(buffer.size() == 2)
                {
                    if(buffer[0] != '\\')
                    {
                        parsingState = "Char must be only one symbol";
                        goto ERROR;
                    }
                }
            }
            getNextByte();
            goto STRING;
        }
        goto ERROR;
    }

    COMMENT:
    {
        parsingState = "Parsing comment";
        getNextByte();
        buffer.push_back(currentByte);
        if(buffer[0] == '*') 
        {
            getNextByte();
            buffer.push_back(currentByte);
            if(buffer[1] == '/')
            {
                //end of comment
                lineBuffer = {};
                buffer = {};
                getNextByte();
                goto SELECT_NEXT;
            }
            else
            {
                ungetByte();
                buffer = {};
                goto COMMENT;
            }
        }
        if(isSpace(currentByte)) {
            if(currentByte == '\n')
            {
                lineNum++;
                lineCol = 0;
            }
        }
        buffer = {};
        goto COMMENT;
    }

    AUTOMATA_END:
    {
        std::cout << "Lines: " << lineNum << "\n";
        for (size_t i = 0; i < _dest->size(); i++)
        {
            std::cout << "[" << i << "]: ";
            std::cout << "\033[33m";
            for (size_t j = 0; j < (*_dest)[i].getData().size(); j++)
            {
                std::cout << (*_dest)[i].getData()[j];
            }
            std::cout << "\033[39m";
            std::cout << "; Type: " << "\033[32m" << stringTokens[(*_dest)[i].getType()] << "\033[39m" << "; (";
            std::cout << (*_dest)[i].getLn() << ", " << (*_dest)[i].getCol() << ")\n";
            // std::cout << "Ln: " << "\033[32m" << (*_dest)[i].getLn() << "\033[39m" << "; ";
            // std::cout << "Col: " << "\033[32m" << (*_dest)[i].getCol() << "\033[39m" << ";\n";
        }
        return;
    }

    ERROR:
    {
        std::cout << "Error at state: " << parsingState << "!\n";
        std::cout << "Error at: (Ln " << lineNum << ", Col " << lineCol << ")!\n\n";
        std::cout << "\033[31m";
        for (size_t i = 0; i < lineBuffer.size(); i++)
        {
            // if(i <= lineCol-1 && i >= lineCol - buffer.size()-1) std::cout << "\033[31m";
            // else std::cout << "\033[39m";
            std::cout << lineBuffer[i];
        }
        std::cout << "\n";
        for (size_t i = 0; i < lineBuffer.size(); i++)
        {
            if(i < lineCol-1 && i >= lineCol - buffer.size()-1) std::cout << "~";
            else if(i == lineCol - 1) std::cout << "\033[31m" << "^";
            else std::cout << " ";
        }
        std::cout << " Unexpected token!\n\n";
    }
    
}

#endif