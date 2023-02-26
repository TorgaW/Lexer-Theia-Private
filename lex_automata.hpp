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
    OP_EQL, OP_LESS, OP_BIGGER, OP_LSEQ, OP_BGEQ, OP_NOT, OP_END, OP_OR,
    //bitwise
    //  ~         <<          >>         ^          &       |
    OP_B_NOT, OP_B_SHFTL, OP_B_SHFTR, OP_B_XOR, OP_B_AND, OP_B_OR,

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
    MES_DOT,//      .
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
    "OP_EQL", "OP_LESS", "OP_BIGGER", "OP_LSEQ", "OP_BGEQ", "OP_NOT", "OP_END", "OP_OR", 
    //bitwise
    //  ~         <<          >>         ^          &       |
    "OP_B_NOT", "OP_B_SHFTL", "OP_B_SHFTR", "OP_B_XOR", "OP_B_AND", "OP_B_OR",

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
    "MES_DOT",//      .
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
    int currentByte;
    long lineNum;
    long lineCol;
    long fileLength;
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
    inline bool isOperator(unsigned char _c) {return _c == '=' || _c == '+' || _c == '-' || _c == '*' || _c == '/' || _c == '%' || _c == '&' || _c == '|' || _c == '^' || _c == '~';}


    /**
     * @brief Get the Next Byte object
     * 
     */
    inline void getNextByte() {
        currentByte = getc(f);
        lineCol++;
        lineBuffer.push_back(currentByte);
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
        currentByte = 0;
        lineNum = 1;
        lineCol = 0;
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
        currentByte = 0;
        lineNum = 1;
        lineCol = 0;
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
        if(currentByte == EOF) goto AUTOMATA_END;
        if(isAlpha(currentByte)) goto ALPHABET;
        if(isNumber(currentByte)) goto NUMBER;
        if(isSpace(currentByte)) goto SPACE;
        if(isOperator(currentByte)) goto OPERATOR;
        goto ERROR;
    }

    START:
    {
        getNextByte();
        goto SELECT_NEXT;
    }

    NUMBER:
    {   
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

    OPERATOR:
    {
        buffer.push_back(currentByte);
        if(buffer.size() == 1 && (buffer[0] == '^' || buffer[0] == '~'))
        {
            getNextByte();
            if(isOperator(currentByte)) goto ERROR;
            goto SELECT_NEXT;
        }
        getNextByte();
        if(isOperator(currentByte))
        {
            buffer.push_back(currentByte);
            if(buffer[0] == '=' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_EQL, lineNum, lineCol));
            if(buffer[0] == '-' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_MINUSASSIGN, lineNum, lineCol));
            if(buffer[0] == '+' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_PLUSASSIGN, lineNum, lineCol));
            if(buffer[0] == '*' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_MULASSIGN, lineNum, lineCol));
            if(buffer[0] == '/' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_DIVASSIGN, lineNum, lineCol));
            if(buffer[0] == '%' && buffer[1] == '=')
                _dest->push_back(LexToken(buffer, Tokens::OP_MODASSIGN, lineNum, lineCol));
            if(buffer[0] == '+' && buffer[1] == '+')
                _dest->push_back(LexToken(buffer, Tokens::OP_INC, lineNum, lineCol));
            if(buffer[0] == '-' && buffer[1] == '-')
                _dest->push_back(LexToken(buffer, Tokens::OP_DEC, lineNum, lineCol));

            buffer = {};
            getNextByte();
            goto SELECT_NEXT;
        }

        
    }

    STRING:

    COMMENT:

    AUTOMATA_END:
    {
        std::cout << "Lines: " << lineNum << "\n";
        for (size_t i = 0; i < _dest->size(); i++)
        {
            std::cout << "[" << i << "]: ";
            for (size_t j = 0; j < (*_dest)[i].getData().size(); j++)
            {
                std::cout << (*_dest)[i].getData()[j];
            }
            std::cout << "; Type: " << stringTokens[(*_dest)[i].getType()] << ";\n";
        }
        return;
    }

    ERROR:
    std::cout << "Error at: (Ln " << lineNum << ", Col " << lineCol << ")!\n";
    for (size_t i = 0; i < lineBuffer.size(); i++)
    {
        if(i <= lineCol-1 && i >= lineCol - buffer.size()-1) std::cout << "\033[31m";
        else std::cout << "\033[39m";
        std::cout << lineBuffer[i];
    }
    std::cout << "\n";
    for (size_t i = 0; i < lineBuffer.size(); i++)
    {
        if(i < lineCol-1 && i >= lineCol - buffer.size()-1) std::cout << "~";
        else if(i == lineCol - 1) std::cout << "\033[31m" << "^";
        else std::cout << " ";
    }
    std::cout << " Undefiend token!\n";
    
}

#endif