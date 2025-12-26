# Compiler —— Practical Project: A Simple Compiler
华东师范大学软件工程编译原理实践


## Project Structure

```
myCompiler/
├── backend/                              # 后端文件夹
│   ├── temp/                             # 临时文件夹
│   │   ├── outputs/                      # 结果文件夹
│   │   └── uploads/                      # 上传文件
│   ├── app.py                            # 词法、语法分析入口程序
│   ├── requirements.txt                  # 库函数要求文件
│   └── translationschema.py              # 语义分析入口程序
├── cpp_src/                              # C代码文件夹
│   ├── LexAnalysis.h                     # 词法分析
│   ├── LLParser.h                        # LL分析
│   ├── LRParser.h                        # LR分析
│   ├── main.cpp                          # 词法、语法分析C程序入口
│   ├── main.exe                          # 词法、语法分析C可执行文件
│   ├── TranslationSchema.h               # 语义分析
│   ├── TranslationSchema.cpp             # 语义分析C程序入口
│   └── TranslationSchema.exe             # 语义分析C程序可执行文件
├── project/                              # 头歌平台提交demo
├── frontend/                             # 前端文件夹
│   ├── static/                           # css、js文件夹
│   │   ├── css/                          # css文件夹
│   │   │   ├── style.css                 # 词法、语法分析界面css文件
│   │   │   └── style2.css                # 语义分析界面css文件
│   │   └── js/                           # js文件夹
│   │       ├── main.js                   # 词法、语法分析界面js文件
│   │       └── translationschema.js      # 语义分析界面js文件
│   └── templates/                        # html文件夹
│       ├── index.html                    # 词法、语法分析html文件
│       └── translationschema.html        # 语义分析html文件
├── test/                                 # 样例文件夹
│   ├── main/                             # 词法、语法分析样例
│   └── schema/                           # 语义分析样例
└── README.md                             # README.md
```

## Environment Setup


### Create Virtual Environment

```bash
conda create -n compiler python=3.9 -y
conda activate fedorthrus
```

### Install Dependencies

```bash
cd backend
pip install -r requirements.txt
```

## Datasets

All datasets should be placed in the `test/` folder according to the project structure shown above.
Lexical and Syntax Analysis: `test/main/`
Semantic Analysis: `test/schema/`

## Usage

### Quick Start
You can directly run the main file to start the digit experiment with default settings:

Lexical and Syntax Analysis:
```bash
python app.py
```

Semantic Analysis:
```bash
python translationschema.py
```