#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "TranslationSchema.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "用法: ./myCompiler <input_file> [output_file]" << endl;
        return 1;
    }
    
    // 读取输入文件
    ifstream input_file(argv[1]);
    if (!input_file) {
        cerr << "无法打开文件: " << argv[1] << endl;
        return 1;
    }
    
    string prog, line;
    while (getline(input_file, line)) {
        prog += line + '\n';
    }
    input_file.close();
    
    // 重定向输出到文件
    if (argc == 3) {
        ofstream output_file(argv[2]);
        if (!output_file) {
            cerr << "无法创建输出文件: " << argv[2] << endl;
            return 1;
        }
        
        // 保存原始cout缓冲区
        streambuf* old_cout = cout.rdbuf();
        // 重定向cout到文件
        cout.rdbuf(output_file.rdbuf());
        
        Analysis(prog);
        
        // 恢复cout
        cout.rdbuf(old_cout);
        output_file.close();
    } else {
        // 输出到标准输出
        Analysis(prog);
    }
    return 0;
}