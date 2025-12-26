// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

enum type {
    TYPE_IF,       // if
    TYPE_THEN,     // then
    TYPE_ELSE,     // else
    TYPE_WHILE,    // while      
    TYPE_ID,       // ID
    TYPE_NUM,      // NUM   
    TYPE_LBRACE,   // {
    TYPE_RBRACE,   // }
    TYPE_LPAREN,   // (
    TYPE_RPAREN,   // )
    TYPE_SEMI,     // ;
    TYPE_ASSIGN,   // =
    TYPE_PLUS,     // +
    TYPE_MINUS,    // -
    TYPE_MUL,      // *
    TYPE_DIV,      // /
    TYPE_LT,       // <
    TYPE_GT,       // >
    TYPE_LE,       // <=
    TYPE_GE,       // >=
    TYPE_EQ,       // ==
    TYPE_EOF,      // E
    TYPE_ERROR
};

// 记录每个符号
struct token {
	string value;
    type t;
    int line;
};

// 词法分析结果记录
vector<token> token_list;
// 错误信息记录
vector<string> errors;
// 语法树构建记录
vector<string> parseTree;

int currentIdx = 0;
int currentLine = 0;

// 词法分析
void lex_analysis() {
	string prog;
	int lineNumber = 0;
	while (getline(cin, prog)) {
		int idx = 0;
		int prog_len = prog.length();
		lineNumber++;
		while (idx < prog_len) {
			// 跳过空白字符（空格、制表符、换行等）
	        if (isspace(prog[idx])) {
	            idx++;
	            continue;
	        }
	        
	        // 处理标识符和关键字
	        if (isalpha(prog[idx]) || prog[idx] == '_') {
	        	// 提取标识符或关键字
	            string phrase = "";
	            while (idx < prog_len && (isalpha(prog[idx]) || isdigit(prog[idx]) || prog[idx] == '_')) {
	                phrase = phrase + prog[idx];
	                idx++;
	            }
	            
	            if (phrase == "if") token_list.push_back({phrase, TYPE_IF, lineNumber});
	            else if (phrase == "else") token_list.push_back({phrase, TYPE_ELSE, lineNumber});
	            else if (phrase == "then") token_list.push_back({phrase, TYPE_THEN, lineNumber});
	            else if (phrase == "while") token_list.push_back({phrase, TYPE_WHILE, lineNumber});
	            else if (phrase == "ID") token_list.push_back({phrase, TYPE_ID, lineNumber});
	            else if (phrase == "NUM") token_list.push_back({phrase, TYPE_NUM, lineNumber});
	            else token_list.push_back({phrase, TYPE_ID, lineNumber});
	            
	            continue;
	        }
	        
	        // 处理数字
	        if (isdigit(prog[idx])) {
	        	// 提取数字
	        	string phrase = "";
	            while (idx < prog_len && isdigit(prog[idx])) {
	                phrase += prog[idx];
	                idx++;
	            }
	            token_list.push_back({phrase, TYPE_NUM, lineNumber});
	            continue;
			}
			
			// 处理运算符、分隔符
			if (prog[idx] == '{') {
				token_list.push_back({string(1, prog[idx]), TYPE_LBRACE, lineNumber});
				idx++;
			} else if (prog[idx] == '}') {
				token_list.push_back({string(1, prog[idx]), TYPE_RBRACE, lineNumber});
				idx++;
			} else if (prog[idx] == '(') {
				token_list.push_back({string(1, prog[idx]), TYPE_LPAREN, lineNumber});
				idx++;
			} else if (prog[idx] == ')') {
				token_list.push_back({string(1, prog[idx]), TYPE_RPAREN, lineNumber});
				idx++;
			} else if (prog[idx] == '+') {
				token_list.push_back({string(1, prog[idx]), TYPE_PLUS, lineNumber});
				idx++;
			} else if (prog[idx] == '-') {
				token_list.push_back({string(1, prog[idx]), TYPE_MINUS, lineNumber});
				idx++;
			} else if (prog[idx] == '*') {
				token_list.push_back({string(1, prog[idx]), TYPE_MUL, lineNumber});
				idx++;
			} else if (prog[idx] == '/') {
				token_list.push_back({string(1, prog[idx]), TYPE_DIV, lineNumber});
				idx++;
			} else if (prog[idx] == ';') {
				token_list.push_back({string(1, prog[idx]), TYPE_SEMI, lineNumber});
				idx++;
			} else if (prog[idx] == '>') {
				if (idx + 1 < prog_len && prog[idx + 1] == '='){
					token_list.push_back({">=", TYPE_GE, lineNumber});
					idx += 2;
				} else {
					token_list.push_back({string(1, prog[idx]), TYPE_GT, lineNumber});
					idx++;
				}
			} else if (prog[idx] == '<') {
				if (idx + 1 < prog_len && prog[idx + 1] == '='){
					token_list.push_back({"<=", TYPE_LE, lineNumber});
					idx += 2;
				} else {
					token_list.push_back({string(1, prog[idx]), TYPE_LT, lineNumber});
					idx++;
				}
			} else if (prog[idx] == '=') {
				if (idx + 1 < prog_len && prog[idx + 1] == '='){
					token_list.push_back({"==", TYPE_EQ, lineNumber});
					idx += 2;
				} else {
					token_list.push_back({string(1, prog[idx]), TYPE_ASSIGN, lineNumber});
					idx++;
				}
			} else {
				token_list.push_back({string(1, prog[idx]), TYPE_ERROR, lineNumber});
				idx++;
			}
		}
	}
	// 添加EOF表示结束
	lineNumber++;
	token_list.push_back({"EOF", TYPE_EOF, lineNumber});
}

void advanceIdx() {
    if (currentIdx < token_list.size() - 1) {
        currentIdx++;
    }
}

void syntaxError(string s) {
	stringstream ss;
    ss << "语法错误,第" << currentLine << "行," << s ;
    errors.push_back(ss.str());
    
    type syncTokens[] = { TYPE_SEMI, TYPE_RBRACE, TYPE_EOF };
    while (currentIdx < token_list.size()) {
        token tmp = token_list[currentIdx];
        bool isSyncToken = false;
        for (type syncToken : syncTokens) {
            if (tmp.t == syncToken) {
                isSyncToken = true;
                break;
            }
        }
        if (isSyncToken) break;
        advanceIdx();
    }
}

// 匹配期望token类型
bool match(type expectedType) {
	if (token_list[currentIdx].t == expectedType) {// 匹配成功，前进到下一个token
		currentLine = token_list[currentIdx].line;
		advanceIdx();
		return true;
	} else {// 匹配不成功，进行错误处理
		string expectedStr;
		if (expectedType == TYPE_SEMI) expectedStr = ";";
		else if (expectedType == TYPE_THEN) expectedStr = "then";
		else if (expectedType == TYPE_ELSE) expectedStr = "else";
		else if (expectedType == TYPE_LPAREN) expectedStr = "(";
		else if (expectedType == TYPE_RPAREN) expectedStr = ")";
		else if (expectedType == TYPE_LBRACE) expectedStr = "{";
		else if (expectedType == TYPE_RBRACE) expectedStr = "}";
		else if (expectedType == TYPE_ASSIGN) expectedStr = "=";
		else expectedStr = "expected str";
		
		stringstream ss;
		ss << "语法错误,第" << currentLine << "行,缺少\"" << expectedStr << "\"";
		errors.push_back(ss.str());
		
		token_list.insert(token_list.begin() + currentIdx, {expectedStr, expectedType, currentLine});
		advanceIdx();
		return true;
	}
}

// 方便后续函数调用
void program(int height);
void compoundstmt(int height);
void stmt(int height);
void stmts(int height);
void ifstmt(int height);
void whilestmt(int height);
void assgstmt(int height);
void boolexpr(int height);
void boolop(int height);
void arithexpr(int height);
void arithexprprime(int height);
void multexpr(int height);
void multexprprime(int height);
void simpleexpr(int height);

// 缩进字符串
string getHeight(int height) {
    return string(height, '\t');
}

void program(int height) {
	parseTree.push_back(getHeight(height) + "program");
	compoundstmt(height + 1);
}

void compoundstmt(int height){
	parseTree.push_back(getHeight(height) + "compoundstmt");
	if (match(TYPE_LBRACE)) {
		parseTree.push_back(getHeight(height + 1) + "{");
		stmts(height + 1);
		if (match(TYPE_RBRACE)) {
			parseTree.push_back(getHeight(height + 1) + "}");
		} else {
			syntaxError("缺少\"}\"");
			parseTree.push_back(getHeight(height + 1) + "}");
		}
	} else {
		syntaxError("缺少\"{\"");
		advanceIdx();
	}
}

void stmts(int height) {
	parseTree.push_back(getHeight(height) + "stmts");
	token tmp = token_list[currentIdx];
	// 若下一个token等于stmt的FIRST集
	if (tmp.t == TYPE_IF || tmp.t == TYPE_ID || tmp.t == TYPE_WHILE || tmp.t == TYPE_LBRACE) {
		stmt(height + 1);
		stmts(height + 1);
	} else {
		parseTree.push_back(getHeight(height + 1) + "E");
	}
}

void stmt(int height) {
	parseTree.push_back(getHeight(height) + "stmt");
	token tmp = token_list[currentIdx];
	if (tmp.t == TYPE_IF) ifstmt(height + 1);
	else if (tmp.t == TYPE_WHILE) whilestmt(height + 1);
	else if (tmp.t == TYPE_ID) assgstmt(height + 1);
	else if (tmp.t == TYPE_LBRACE) compoundstmt(height + 1);
	else {
		syntaxError("不合法的语句");
		advanceIdx();
	}
}

void ifstmt(int height) {
	parseTree.push_back(getHeight(height) + "ifstmt");
	if (match(TYPE_IF)) {
        parseTree.push_back(getHeight(height + 1) + "if");
        if (match(TYPE_LPAREN)) {
            parseTree.push_back(getHeight(height + 1) + "(");
            boolexpr(height + 1);
            if (match(TYPE_RPAREN)) {
                parseTree.push_back(getHeight(height + 1) + ")");
                if (match(TYPE_THEN)) {
                    parseTree.push_back(getHeight(height + 1) + "then");
                    stmt(height + 1);
                    if (match(TYPE_ELSE)) {
                        parseTree.push_back(getHeight(height + 1) + "else");
                        stmt(height + 1);
                    } 
                    else syntaxError("缺少\"else\"");
                } 
                else syntaxError("缺少\"then\"");
            } 
            else syntaxError("缺少\")\"");
        } 
        else syntaxError("缺少\"(\"");
    } 
    else syntaxError("缺少\"if\"");
}

void whilestmt(int height) {
    parseTree.push_back(getHeight(height) + "whilestmt");
    if (match(TYPE_WHILE)) {
        parseTree.push_back(getHeight(height + 1) + "while");
        if (match(TYPE_LPAREN)) {
            parseTree.push_back(getHeight(height + 1) + "(");
            boolexpr(height + 1);
            if (match(TYPE_RPAREN)) {
                parseTree.push_back(getHeight(height + 1) + ")");
                stmt(height + 1);
            } 
            else syntaxError("缺少\")\"");
        } 
        else syntaxError("缺少\"(\"");
    } 
    else syntaxError("缺少\"while\"");
}

void assgstmt(int height) {
    parseTree.push_back(getHeight(height) + "assgstmt");
    if (match(TYPE_ID)) {
        parseTree.push_back(getHeight(height + 1) + "ID");
        if (match(TYPE_ASSIGN)) {
            parseTree.push_back(getHeight(height + 1) + "=");
            arithexpr(height + 1);
            if (match(TYPE_SEMI)) parseTree.push_back(getHeight(height + 1) + ";");
            else syntaxError("缺少\";\"");
        } 
        else syntaxError("缺少\"=\"");
    } 
    else syntaxError("缺少标识符");
}

void boolexpr(int height) {
    parseTree.push_back(getHeight(height) + "boolexpr");
    arithexpr(height + 1);
    boolop(height + 1);
    arithexpr(height + 1);
}

void arithexpr(int height) {
    parseTree.push_back(getHeight(height) + "arithexpr");
    multexpr(height + 1);
    arithexprprime(height + 1);
}

void boolop(int height) {
    parseTree.push_back(getHeight(height) + "boolop");
    token tmp = token_list[currentIdx];
    if (tmp.t == TYPE_LT || tmp.t == TYPE_GT || tmp.t == TYPE_LE || tmp.t == TYPE_GE || tmp.t == TYPE_EQ) {
        string op;
        if (tmp.t == TYPE_LT) op = "<";
        else if (tmp.t == TYPE_LE) op = "<=";
        else if (tmp.t == TYPE_GT) op = ">";
        else if (tmp.t == TYPE_GE) op = ">=";
        else op = "==";
        parseTree.push_back(getHeight(height + 1) + op);
        advanceIdx();
    } 
    else syntaxError("缺少关系运算符");
}

void multexpr(int height) {
    parseTree.push_back(getHeight(height) + "multexpr");
    simpleexpr(height + 1);
    multexprprime(height + 1);
}

void simpleexpr(int height) {
    parseTree.push_back(getHeight(height) + "simpleexpr");
    token tmp = token_list[currentIdx];
    if (tmp.t == TYPE_ID) {
        parseTree.push_back(getHeight(height + 1) + "ID");
        advanceIdx();
    } 
    else if (tmp.t == TYPE_NUM) {
        parseTree.push_back(getHeight(height + 1) + "NUM");
        advanceIdx();
    } 
    else if (tmp.t == TYPE_LPAREN) {
        parseTree.push_back(getHeight(height + 1) + "(");
        advanceIdx();
        arithexpr(height + 1);
        if (match(TYPE_RPAREN)) {
            parseTree.push_back(getHeight(height + 1) + ")");
        } 
        else syntaxError("缺少\")\"");
    } 
    else {
        syntaxError("缺少简单表达式");
        parseTree.push_back(getHeight(height + 1) + "ID");
    }
}

void multexprprime(int height) {
    parseTree.push_back(getHeight(height) + "multexprprime");
    token tmp = token_list[currentIdx];
    if (tmp.t == TYPE_MUL || tmp.t == TYPE_DIV) {
        string op = (tmp.t == TYPE_MUL) ? "*" : "/";
        parseTree.push_back(getHeight(height + 1) + op);
        advanceIdx();
        simpleexpr(height + 1);
        multexprprime(height + 1);
    } 
    else parseTree.push_back(getHeight(height + 1) + "E");
}

void arithexprprime(int height) {
    parseTree.push_back(getHeight(height) + "arithexprprime");
    token tmp = token_list[currentIdx];
    if (tmp.t == TYPE_PLUS || tmp.t == TYPE_MINUS) {
        string op = (tmp.t == TYPE_PLUS) ? "+" : "-";
        parseTree.push_back(getHeight(height + 1) + op);
        advanceIdx();
        multexpr(height + 1);
        arithexprprime(height + 1);
    } 
    else parseTree.push_back(getHeight(height + 1) + "E");
}

void Analysis(){
	/* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    lex_analysis();// 词法分析
    
    program(0);// 语法分析
    
    for (auto& msg : errors) {// 输出错误信息
        cout << msg << endl;
    }

    for (auto& line : parseTree) {// 输出语法树
        cout << line << endl;
    }
    
    /********* End *********/
	
}