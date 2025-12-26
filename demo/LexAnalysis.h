// C/C++词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct token{
	string value;
	int number;
};

/* 不要修改这个标准输入函数 */
void read_prog(string& prog){
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
/* 你可以添加其他函数 */

void Analysis(){
	string prog;
	read_prog(prog);
	/* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    
    // 构建所有C语言元素的编码映射表
    map<string, int> m;
    
    // 读取编码配置文件 c_keys.txt
    ifstream key_file("c_keys.txt");
    string token_str;
    int token_code;
    while (key_file >> token_str >> token_code) {
    	if (token_str == "/*注释*/") {
            m["//"] = token_code;
        }
        m[token_str] = token_code;
    }
    key_file.close();

    vector<token> token_list;// 存储分析出的词法单元
    int idx = 0;
    int prog_len = prog.length();

    while (idx < prog_len) {
        // 跳过空白字符（空格、制表符、换行等）
        if (isspace(prog[idx])) {
            idx++;
            continue;
        }
        
        // 处理标识符和关键字（字母/下划线开头，后跟字母/数字/下划线）
        if (isalpha(prog[idx]) || prog[idx] == '_') {
            string phrase = "";
            while (idx < prog_len && (isalpha(prog[idx]) || isdigit(prog[idx]) || prog[idx] == '_')) {
                phrase += prog[idx];
                idx++;
            }
            // 判断是否为关键字
            if (m.find(phrase) != m.end()) {
                token_list.push_back({phrase, m[phrase]});
            } else {
                token_list.push_back({phrase, 81});  // 普通标识符编码81
            }
            continue;
        }
        
        // 处理字符串常量（双引号包裹）
        if (prog[idx] == '"') {
            token_list.push_back({string(1, prog[idx]), m["\""]});
            idx++;
            string phrase = "";
            while (idx < prog_len && prog[idx] != '"') {
                // 处理转义字符
                if (prog[idx] == '\\' && idx + 1 < prog_len) {
                    phrase += prog[idx];
                    phrase += prog[idx+1];
                    idx += 2;
                } else {
                    phrase += prog[idx++];
                }
            }
            if (!phrase.empty()) {
                token_list.push_back({phrase, 81});  // 字符串内容归为标识符（编码81）
            }
            if (idx < prog_len && prog[idx] == '"') {
                token_list.push_back({string(1, prog[idx]), m["\""]});
                idx++;
            }
            continue;
        }

        // 处理字符常量（单引号包裹）
        if (prog[idx] == '\'') {
            token_list.push_back({string(1, prog[idx]), m["/*注释*/"]});
            idx++;
            string phrase = "";
            if (idx < prog_len && prog[idx] != '\'') {
                // 处理转义字符
                if (prog[idx] == '\\' && idx + 1 < prog_len) {
                    phrase += prog[idx];
                    phrase += prog[idx+1];
                    idx += 2;
                } else {
                    phrase += prog[idx++];
                }
                token_list.push_back({phrase, 81});  // 字符内容归为标识符
            }
            if (idx < prog_len && prog[idx] == '\'') {
                token_list.push_back({string(1, prog[idx]), m["/*注释*/"]});
                idx++;
            }
            continue;
        }

        // 处理数字常量（纯数字）
        if (isdigit(prog[idx])) {
            string phrase = "";
            while (idx < prog_len && isdigit(prog[idx])) {
                phrase += prog[idx];
                idx++;
            }
            token_list.push_back({phrase, 80});  // 数字常量编码80
            continue;
        }

        // 处理注释逻辑
        if (prog[idx] == '/' && idx + 1 < prog_len) {
            // 单行注释
            if (prog[idx + 1] == '/') {
                string phrase = "//";
                idx += 2;
                while (idx < prog_len && prog[idx] != '\n') {
                    phrase += prog[idx];
                    idx++;
                }
                token_list.push_back({phrase, m["//"]});
                continue;
            }
            // 多行注释
            else if (prog[idx+1] == '*') {
                string phrase = "/*";
                idx += 2;
                while (idx + 1 < prog_len && !(prog[idx] == '*' && prog[idx+1] == '/')) {
                    phrase += prog[idx++];
                }
                if (idx + 1 < prog_len) {
                    phrase += "*/";
                    idx += 2;
                }
                token_list.push_back({phrase, m["/*注释*/"]});
                continue;
            }
        }

        // 处理运算符和界符（最长匹配）
        bool flag = false;
        for (int len = 3; len >= 1; len--) {
            if (idx + len > prog_len) continue;
            string phrase = prog.substr(idx, len);
            // 跳过已处理的注释符号
            if (phrase == "/*" || phrase == "//" || phrase == "*/") continue;
            if (m.find(phrase) != m.end()) {
                token_list.push_back({phrase, m[phrase]});
                idx += len;
                flag = true;
                break;
            }
        }
        if (flag) continue;

        // 未知字符直接跳过
        idx++;
    }

    // 输出词法分析结果
    for (int i = 0; i < token_list.size(); i++) {
        cout << (i + 1) << ": <" << token_list[i].value << "," << token_list[i].number << ">";
        if (i != token_list.size() - 1) {
            cout << endl;
        }
    }
    /********* End *********/
}