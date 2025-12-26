#include <cstdio>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

void read_prog(string &prog) {
    char c;
    while (scanf("%c", &c) != EOF) {
        prog += c;
    }
}

class TOKEN{
public:
    int is_terminal; // 是否为终结符
    int value; // 终结符的编号
    int no;
    
    TOKEN(){
        this->is_terminal = -1;
        this->value = -1;
        this->no = -1;
    }
    TOKEN(int is_terminal, int value, int no){
        this->is_terminal = is_terminal;
        this->value = value;
        this->no = no;
    }
    void clear(){
        this->is_terminal = -1;
        this->value = -1;
        this->no = -1;
    }
};

class State {
public:
    map<char, State *> next;
    //0表示中间节点，1表示接受节点
    int is_acc;

    State() {
        this->next = map<char, State *>();
        this->is_acc = 0;
    }

    State(int state) {
        this->next = map<char, State *>();
        this->is_acc = state;
    }

    void add_next(State *n, char c) {
        this->next[c] = n;
    }
};

class NFA {
public:
    State start;
    State end;

    NFA() {
        this->start = State();
        this->end = State();
    }

    NFA(char condition) {
        this->start = State();
        this->end = State();
        this->start.add_next(&(this->end), condition);
    }

    void link(NFA *nfa1, NFA *nfa2) {
        this->start = nfa1->start;
        nfa1->end.add_next(&(nfa2->start), -1);
        nfa2->end = this->end;
    }
};

class DFA {
public:
    State *start;
    State *cur_state;
    vector<State *> end_list;
    int type;

    DFA() {
        this->start = new State();
        this->end_list = vector<State *>();
        this->cur_state = this->start;
        this->type = -1;
    }

    void construct_key(const string &s){
        State *cur = this->start;
        for (auto c: s) {
            if (cur->next.find(c) == cur->next.end()) {
                cur->next[c] = new State();
            }
            cur = cur->next[c];
        }
        cur->is_acc = 1;
        this->cur_state = this->start;
    }

    void construct_comment_type1() {
        // 构造 /*注释*/ 类型的注释的DFA
        State *cur = this->start;
        cur->next['/'] = new State();
        cur = cur->next['/'];
        cur->next['*'] = new State();
        cur = cur->next['*'];
        for (int i = -128; i < 128; i++) {
            if (i != '*') {
                cur->next[(char) i] = cur;
            }
        }
        cur->next['*'] = new State();
        State *temp = cur;

        cur = cur->next['*'];
        for (int i = -128; i < 128; i++) {
            if (i != '/') {
                cur->next[(char) i] = temp;
            }
        }

        cur->next['/'] = new State();
        cur = cur->next['/'];
        cur->is_acc = 1;
        this->type = -79;
        this->end_list.push_back(cur);
        this->cur_state = this->start;
    }

    void construct_comment_type2() {
        // 构造 //注释 类型的注释的DFA
        State *cur = this->start;
        cur->next['/'] = new State();
        cur = cur->next['/'];
        cur->next['/'] = new State();
        cur = cur->next['/'];
        for (int i = -128; i < 128; i++) {
            if (i != '\n') {
                cur->next[(char) i] = cur;
            }
        }
        cur->next['\n'] = new State();
        cur = cur->next['\n'];
        cur->is_acc = 1;
        this->type = 79;
        this->end_list.push_back(cur);
        this->cur_state = this->start;
    }

    void construct_id() {
        // 构造变量名称的DFA，第一个字符必须是字母或者下划线，后面的字符可以是字母、数字或者下划线
        State *cur = this->start;
        State *next = new State();
        cur->next['_'] = next;
        for (int i = 'a'; i <= 'z'; i++) {
            cur->next[(char) i] = next;
        }
        for (int i = 'A'; i <= 'Z'; i++) {
            cur->next[(char) i] = next;
        }
        cur = next;
        for (int i = 'a'; i <= 'z'; i++) {
            cur->next[(char) i] = cur;
        }
        for (int i = 'A'; i <= 'Z'; i++) {
            cur->next[(char) i] = cur;
        }
        for (int i = '0'; i <= '9'; i++) {
            cur->next[(char) i] = cur;
        }
        cur->next['_'] = cur;
        cur->is_acc = 1;
        this->type = 81;
        this->end_list.push_back(cur);
        this->cur_state = this->start;
    }

    void construct_num() {
        // 构造数字的DFA 数字的形式为：(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?
        auto *A = new State();
        auto *B = new State();
        auto *C = new State();
        auto *D = new State();
        auto *E = new State();
        auto *F = new State();
        auto *G = new State();
        auto *H = new State();
        // A状态转换
        A->next['0'] = C;
        for (int i = '1'; i < '9'; ++i) {
            A->next[(char) i] = B;
        }
        // B状态转换
        for (int i = '0'; i < '9'; ++i) {
            B->next[(char) i] = B;
        }
        B->next['.'] = D;
        B->next['e'] = E;
        B->next['E'] = E;
        // C状态转换
        C->next['.'] = D;
        C->next['e'] = E;
        C->next['E'] = E;
        // D状态转换
        for (int i = '0'; i < '9'; ++i) {
            D->next[(char) i] = F;
        }
        // E状态转换
        E->next['+'] = G;
        E->next['-'] = G;
        for (int i = '0'; i < '9'; ++i) {
            E->next[(char) i] = H;
        }
        // F状态转换
        for (int i = '0'; i < '9'; ++i) {
            F->next[(char) i] = F;
        }
        F->next['e'] = E;
        F->next['E'] = E;
        // G状态转换
        for (int i = '0'; i < '9'; ++i) {
            G->next[(char) i] = H;
        }
        // H状态转换
        for (int i = '0'; i < '9'; ++i) {
            H->next[(char) i] = H;
        }
        H->is_acc = 1;
        F->is_acc = 1;
        B->is_acc = 1;
        C->is_acc = 1;
        this->start = A;
        this->type = 80;
        this->end_list.push_back(H);
        this->end_list.push_back(F);
        this->end_list.push_back(B);
        this->end_list.push_back(C);
        this->cur_state = this->start;
    }

    void construct_string() {
        // 构造字符串的DFA,字符串的形式为："([^"\n]|(\\"))*"
        State *cur = this->start;
        cur->next['\"'] = new State();
        cur = cur->next['\"'];
        
        for (int i = 0; i < 128; i++) {
            if (i != '\"' && i != '\\') {
                cur->next[(char) i] = cur;
            }
        }
        cur->next['\"'] = new State();
        cur->next['\\'] = new State();
        State *tmp = cur;
        cur = cur->next['\\'];
        for (int i = 0; i < 128; ++i) {
            cur->next[(char) i] = tmp;
        }
        cur = tmp;
        cur->next['\"'] = new State();
        cur = cur->next['\"'];
        cur->is_acc = 1;
        this->type = 78;
        this->end_list.push_back(cur);
        this->cur_state = this->start;
    }
};

vector<TOKEN> detection_result;
vector<int> index2line;
vector<int> grammar_result;
vector<DFA *> dfa_list;
string key_list[78] = {
	" ", "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
    "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short",
	"signed", "sizeof", "static", "struct", "switch", "typedef", "union", "then", "ID", "NUM",
	"while", "-", "--", "-=", "->", "!", "!=", "%", "%=", "&", "&&", "&=", "(", ")", "*",
	"*=", ",", ".", "/", "/=", ":", ";", "?", "[", "]", "^", "^=", "{", "|", "|=", "||",
	"}", "~", "+", "++", "+=", "<", "<<", "<<=", "<=", "=", "==", ">", ">=", ">>", ">>="
};
vector<vector<TOKEN>> rules;
vector<string> non_terminal_index = { // 非终结符编号：
	"",                               
	"program",                        // 1: program
	"stmt",                           // 2: stmt
	"compoundstmt",                   // 3: compoundstmt
	"stmts",                          // 4: stmts
	"ifstmt",                         // 5: ifstmt
	"whilestmt",                      // 6: whilestmt
	"assgstmt",                       // 7: assgstmt
	"boolexpr",                       // 8: boolexpr
	"boolop",                         // 9: boolop
	"arithexpr",                      // 10: arithexpr
	"arithexprprime",                 // 11: arithexprprime
	"multexpr",                       // 12: multexpr
	"multexprprime",                  // 13: multexprprime
	"simpleexpr"                      // 14: simpleexpr
};
// LR分析表列的顺序为: else 10,if 16,then 29,ID 30,NUM 31,while 32,- 33,+ 65,* 46,/ 50,{ 59,} 63,( 44,) 45,= 72,; 53,< 68,> 74,<= 71,>= 75,== 73
vector<vector<int>> SLR_table;
vector<vector<int>> GOTO_table;
map<int, int> type2col = {{10, 1}, {16, 2}, {29, 3}, {30, 4}, {31, 5}, {32, 6}, {33, 7}, {65, 8}, {46, 9}, {50, 10}, {59, 11}, {63, 12}, {44, 13}, {45, 14}, {72, 15}, {53, 16}, {68, 17}, {74, 18}, {71, 19}, {75, 20}, {73, 21}, {-3, 22}};
map<int, int> col2type;

vector<int> grammar_belong = {0,1,2,2,2,2,3,4,4,5,6,7,8,9,9,9,9,9,10,11,11,11,12,13,13,13,14,14,14};
vector<pair<int, int>> error_msg;

void generate_DFA() {

    for (int i = 0; i < 78; ++i) {
        DFA *dfa = new DFA();
        dfa->construct_key(key_list[i]);
        dfa->type = i;
        dfa_list.push_back(dfa);
    }
    DFA *dfa = new DFA();
    dfa->construct_comment_type1();
    dfa_list.push_back(dfa);

    dfa = new DFA();
    dfa->construct_comment_type2();
    dfa_list.push_back(dfa);

    dfa = new DFA();
    dfa->construct_id();
    dfa_list.push_back(dfa);

    dfa = new DFA();
    dfa->construct_num();
    dfa_list.push_back(dfa);

    dfa = new DFA();
    dfa->construct_string();
    dfa_list.push_back(dfa);
    
    dfa = new DFA();
    dfa->construct_key("\n");
    dfa->type = -2;
    dfa_list.push_back(dfa);

    dfa = new DFA();
    dfa->construct_key("\t");
    dfa->type = 0;
    dfa_list.push_back(dfa);
}

void error_handler(string& prog, int line, int pos){
    cout << "Error occur on line " << line << ":" << endl;
    int end = pos + 30;
    for (int i = pos; i < pos + 30; ++i) {
        if (prog[i] == '\n'){
            end = i;
            break;
        }
    }

    cout <<  "\033[32m" + prog.substr(pos, end - pos) + "\033[0m" << endl;
    exit(0);
}


void process_word(string &prog) {
    int cnt = 0;
    int line = 1;
    string detected_s;
    vector<int> left_dfa;

    for (int i = 0; i < dfa_list.size(); ++i) {
        left_dfa.push_back(i);
    }
    for (int i = 0; i < prog.size(); ++i) {
        vector<int> matched_dfa;
        for (int j = 0; j < left_dfa.size(); ++j) {
            if (dfa_list[left_dfa[j]]->cur_state->next[prog[i]] != nullptr) {
                matched_dfa.push_back(left_dfa[j]);
                dfa_list[left_dfa[j]]->cur_state = dfa_list[left_dfa[j]]->cur_state->next[prog[i]];
            }
        }
        if (!matched_dfa.empty()) {
            left_dfa = matched_dfa;
            detected_s += prog[i];
        } else {
            if (!detected_s.empty()) {
                DFA *final_dfa = nullptr;
                for (int j = 0; j < left_dfa.size(); ++j) {
                    if (dfa_list[left_dfa[j]]->cur_state->is_acc == 1) {
                        final_dfa = dfa_list[left_dfa[j]];
                        break;
                    }
                    if (j == left_dfa.size() - 1) {
                        error_handler(prog, line, i-detected_s.length());
                    }
                }
                if (final_dfa->type != 0) {
                    if (final_dfa->type == -2){
                        line++;
                    }else {
                        cnt++;
                        auto *element = new TOKEN();
                        element->is_terminal = 1;
                        element->value = final_dfa->type;
                        detection_result.push_back(*element);
                        index2line.push_back(line);
                    }

                }
                detected_s = "";
                left_dfa.clear();
                for (int j = 0; j < dfa_list.size(); ++j) {
                    left_dfa.push_back(j);
                }
                for (int j = 0; j < dfa_list.size(); ++j) {
                    dfa_list[j]->cur_state = dfa_list[j]->start;
                }
                i--;
            }else{
                error_handler(prog, line, i);
            }
        }
    }
    if (!detected_s.empty()) {
        DFA *final_dfa = nullptr;
        for (int j = 0; j < left_dfa.size(); ++j) {
            if (dfa_list[left_dfa[j]]->cur_state->is_acc == 1) {
                final_dfa = dfa_list[left_dfa[j]];
                break;
            }
            if (j == left_dfa.size() - 1) {
                error_handler(prog, line, prog.length()-detected_s.length());
            }
        }
        if (final_dfa->type != 0 && final_dfa->type != -2) {
            cnt++;
            auto *element = new TOKEN();
            element->is_terminal = 1;
            element->value = dfa_list[left_dfa[0]]->type;
            detection_result.push_back(*element);
            index2line.push_back(line);
        }
    }
}

void init_rules(){

    vector<TOKEN> temp;
    rules.push_back(temp);
    int cfg_num = 28;
    string rules_string = "n1 n3\n"
                        "n2 n5\n"
                        "n2 n6\n"
                        "n2 n7\n"
                        "n2 n3\n"
                        "n3 t59 n4 t63\n"
                        "n4 n2 n4\n"
                        "n4 E\n"
                        "n5 t16 t44 n8 t45 t29 n2 t10 n2\n"
                        "n6 t32 t44 n8 t45 n2\n"
                        "n7 t30 t72 n10 t53\n"
                        "n8 n10 n9 n10\n"
                        "n9 t68\n"
                        "n9 t74\n"
                        "n9 t71\n"
                        "n9 t75\n"
                        "n9 t73\n"
                        "n10 n12 n11\n"
                        "n11 t65 n12 n11\n"
                        "n11 t33 n12 n11\n"
                        "n11 E\n"
                        "n12 n14 n13\n"
                        "n13 t46 n14 n13\n"
                        "n13 t50 n14 n13\n"
                        "n13 E\n"
                        "n14 t30\n"
                        "n14 t31\n"
                        "n14 t44 n10 t45\n";
    stringstream ss(rules_string);
    for (int i = 1; i <= cfg_num; ++i) {
        string begin;
        ss >> begin;

        char type;
        int num;
        while (true){
            type = (char) ss.get();

            if (type == '\n'){
                rules.push_back(temp);
                temp.clear();
                break;
            } else type = (char) ss.get();

            if (type == 't') {
                ss >> num;
                auto *element = new TOKEN();
                element->is_terminal = 1;
                element->value = num;
                temp.push_back(*element);
            } else if (type == 'n'){
                ss >> num;
                auto *element = new TOKEN();
                element->is_terminal = 0;
                element->no = num;
                temp.push_back(*element);
            } else if (type == 'E'){
                auto *element = new TOKEN();
                element->is_terminal = 1;
                element->value = -1;
                temp.push_back(*element);
            }
        }
    }
}

void create_table(){
    for (auto it = type2col.begin(); it != type2col.end(); it++) {
        col2type[it->second] = it->first;
    }

    SLR_table.resize(57);
    GOTO_table.resize(57);
    string table_csv = ",,,,,,,,,,,S2,,,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,,,,,,,,,R1,\n"
                       ",,S9,R8,S11,R8,S10,R8,R8,R8,R8,S2,R8,R8,R8,R8,R8,R8,R8,R8,R8,R8,R8,\n"
                       ",,,,,,,,,,,,S53,,,,,,,,,,,\n"
                       ",R8,S9,R8,S11,R8,S10,R8,R8,R8,R8,S2,R8,R8,R8,R8,R8,R8,R8,R8,R8,R8,R8,\n"
                       ",,R2,,R2,,R2,,,,,R2,R2,,,,,,,,,,,\n"
                       ",,R3,,R3,,R3,,,,,R3,R3,,,,,,,,,,,\n"
                       ",,R4,,R4,,R4,,,,,R4,R4,,,,,,,,,,,\n"
                       ",,R5,,R5,,R5,,,,,R5,R5,,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,S13,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,S14,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,,S15,,,,,,,,\n"
                       ",,,,,,,,,,,,R7,,,,,,,,,,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,S25,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,,,,S27,S28,S29,S30,S31,,\n"
                       ",R21,R21,R21,R21,R21,R21,S34,S33,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,\n"
                       ",R25,R25,R25,R25,R25,R25,R25,R25,S36,S37,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,\n"
                       ",,,,,,,R26,R26,R26,R26,,,,R26,,R26,R26,R26,R26,R26,R26,,\n"
                       ",,,,,,,R27,R27,R27,R27,,,,R27,,R27,R27,R27,R27,R27,R27,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,S38,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,,,S56,,,,,,,\n"
                       ",,,S39,,,,,,,,,,,,,,,,,,,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,R13,R13,,,,,,,,R13,,,,,,,,,,\n"
                       ",,,,R14,R14,,,,,,,,R14,,,,,,,,,,\n"
                       ",,,,R15,R15,,,,,,,,R15,,,,,,,,,,\n"
                       ",,,,R16,R16,,,,,,,,R16,,,,,,,,,,\n"
                       ",,,,R17,R17,,,,,,,,R17,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,R18,,R18,R18,R18,R18,R18,R18,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,,,,R22,R22,,,,,,R22,,R22,R22,R22,R22,R22,R22,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,,,S20,S21,,,,,,,,S22,,,,,,,,,,\n"
                       ",,S9,,S11,,S10,,,,,S2,,,,,,,,,,,,\n"
                       ",,S9,,S11,,S10,,,,,S2,,,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,R12,,,,,,,,,\n"
                       ",,R18,R19,R20,R21,R21,S34,S33,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,\n"
                       ",,,,,,,,,,,,,,R19,,R19,R19,R19,R19,R19,R19,,\n"
                       ",,R21,R21,R21,R21,R21,S34,S33,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,R21,\n"
                       ",,,,,,,,,,,,,,R20,,R20,R20,R20,R20,R20,R20,,\n"
                       ",,R25,R25,R25,R25,R25,R25,R25,S36,S37,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,\n"
                       ",,,,,,,R23,R23,,,,,,R23,,R23,R23,R23,R23,R23,R23,,\n"
                       ",,R25,R25,R25,R25,R25,R25,R25,S36,S37,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,R25,\n"
                       ",,,,,,,R24,R24,,,,,,R24,,R24,R24,R24,R24,R24,R24,,\n"
                       ",,R10,,R10,,R10,,,,,R10,R10,,,,,,,,,,,\n"
                       ",S51,,,,,,,,,,,,,,,,,,,,,,\n"
                       ",,S9,,S11,,S10,,,,,S2,,,,,,,,,,,,\n"
                       ",,R9,,R9,,R9,,,,,R9,R9,,,,,,,,,,,\n"
                       ",,R6,,R6,,R6,,,,,R6,R6,,,,,,,,,,R6,\n"
                       ",,,,,,,,,,,,,,S55,,,,,,,,,\n"
                       ",,,,,,,R28,R28,R28,R28,,,,R28,,R28,R28,R28,R28,R28,R28,,\n"
                       ",,R11,,R11,,R11,,,,,R11,R11,,,,,,,,,,,";

    string goto_csv = ",,,S1,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,S4,S8,S3,S5,S6,S7,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,S4,S8,S12,S5,S6,S7,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,S16,,S17,,S18,,S19\n"
                      ",,,,,,,,S23,,S17,,S18,,S19\n"
                      ",,,,,,,,,,S24,,S18,,S19\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,S26,,,,,\n"
                      ",,,,,,,,,,,S32,,,\n"
                      ",,,,,,,,,,,,,S35,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,S54,,S18,,S19\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,S40,,S18,,S19\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,S41,,S19\n"
                      ",,,,,,,,,,,,S43,,S19\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,S45\n"
                      ",,,,,,,,,,,,,,S47\n"
                      ",,S49,S8,,S5,S6,S7,,,,,,,\n"
                      ",,S50,S8,,S5,S6,S7,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,S42,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,S44,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,S46,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,S48,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,S52,S8,,S5,S6,S7,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,\n"
                      ",,,,,,,,,,,,,,";

    stringstream ss(table_csv);
    for (int i = 0; i < SLR_table.size(); ++i) {
        string line;
        getline(ss, line);
        stringstream line_ss(line);
        int cnt = 0;
        while (true){
            char symbol;
            line_ss >> symbol;
            if (line_ss.fail())break;
            if (symbol == ','){
                SLR_table[i].push_back(0);
                cnt++;
            } else if (symbol == 'S'){
                int num;
                line_ss >> num;
                SLR_table[i].push_back(num);
                line_ss.get();
                cnt++;
            } else if (symbol == 'R'){
                int num;
                line_ss >> num;
                SLR_table[i].push_back(-num);
                line_ss.get();
            } else break;
        }
    }
    stringstream sgo(goto_csv);
    for (int i = 0; i < GOTO_table.size(); ++i) {
        string line;
        getline(sgo, line);
        stringstream line_ss(line);
        int cnt = 0;
        while (true){
            char symbol;
            line_ss >> symbol;
            if (line_ss.fail())break;
            if (symbol == ','){
                GOTO_table[i].push_back(0);
                cnt++;
            } else if (symbol == 'S'){
                int num;
                line_ss >> num;
                GOTO_table[i].push_back(num);
                line_ss.get();
                cnt++;
            } else if (symbol == 'R'){
                int num;
                line_ss >> num;
                GOTO_table[i].push_back(-num);
                line_ss.get();
            } else break;
        }
    }
}

int search_table(int row, int num){
    return SLR_table[row][type2col[num]];
}

int search_goto(int row, int col){
    return GOTO_table[row][col];
}

void error_handler(int type, int index, int value){
    // type 0: 缺少符号, type 1: 查表为空
    int line;
    if (type == 1){
        cout << "cannot find table item!" << endl;
        exit(0);
    } else {
        line = index2line[index - 1];
        error_msg.emplace_back(line, value);
    }
}

void grammar_check(){
    stack<TOKEN> q;
    // 添加终止符
    TOKEN end;
    end.is_terminal = 1;
    end.value = -3;
    detection_result.push_back(end);

    // 初始状态压栈
    TOKEN begin;
    begin.is_terminal = 2;
    begin.no = 0;
    q.push(begin);
    int now = 0;

    while (!q.empty()){
        TOKEN state = q.top();
        TOKEN input = detection_result[now];
        int table_value = search_table(state.no, input.value);
        if (table_value == 0){
            bool found = false;
            for (int i = 1; i < SLR_table.size(); ++i) {
                if (SLR_table[state.no][i] != 0){
                    if (SLR_table[state.no][i] > 0){
                        table_value = SLR_table[state.no][i];

                        TOKEN new_token;
                        new_token.is_terminal = 1;
                        int new_value;
                        new_value = col2type[i];
                        new_token.value = new_value;
                        error_handler(0, now, new_value);
                        detection_result.insert(detection_result.begin() + now, new_token);
                        int line = index2line[now - 1];
                        index2line.insert(index2line.begin() + now, line);
                        found = true;
                        break;
                    } else{
                        table_value = SLR_table[state.no][i];
                        found = true;
                        break;
                    }
                }
            }
            if (!found){
                error_handler(1, now, input.value);
            }
        }
        if (table_value > 0){
            // 移进
            TOKEN new_state;
            new_state.is_terminal = 2;
            new_state.no = table_value;
            q.push(input);
            q.push(new_state);
            now++;
        } else{
            // 规约
            int grammar_index = -table_value;
            vector<TOKEN> grammar = rules[grammar_index];
            grammar_result.push_back(grammar_index);
            if (grammar_index == 1){
                break;
            }
            for (int i = 0; i < 2*grammar.size(); ++i) {
                if (grammar[i].is_terminal == 1 && grammar[i].value == -1)break;
                q.pop();
            }
            int goto_value = search_goto(q.top().no, grammar_belong[grammar_index]);
            if (goto_value == 0){
                error_handler(1, now, input.value);
            } else {
                TOKEN new_n;
                new_n.is_terminal = 0;
                new_n.no = grammar_belong[grammar_index];
                q.push(new_n);
                TOKEN new_state;
                new_state.is_terminal = 2;
                new_state.no = goto_value;
                q.push(new_state);
            }
        }
    }
}

void output_error(){
    for (int i = 0; i < error_msg.size(); ++i) {
        // 格式为：语法错误,第x行,缺少"xxx"
        cout << "语法错误，第" << error_msg[i].first << "行，缺少\"" << key_list[error_msg[i].second] << "\"" << endl;
    }
}

void output_rightmost_deviation(){
    vector<TOKEN> output;
    TOKEN ele;
    ele.is_terminal = 0;
    ele.no = 1;
    output.push_back(ele);
    cout << "program ";
    for (int i = grammar_result.size()-1; i >= 0; --i) {
        cout << "=> " << endl;
        for (int j = output.size()-1; j >= 0; --j) {
            if (output[j].is_terminal == 0){
                vector<TOKEN> new_output;
                for (int k = 0; k < j; ++k) {
                    new_output.push_back(output[k]);
                }
                for (int k = 0; k < rules[grammar_result[i]].size(); ++k) {
                    if (rules[grammar_result[i]][k].is_terminal == 1 && rules[grammar_result[i]][k].value == -1)break;
                    new_output.push_back(rules[grammar_result[i]][k]);
                }
                for (int k = j+1; k < output.size(); ++k) {
                    new_output.push_back(output[k]);
                }
                output = new_output;
                break;
            }
        }
        for (int j = 0; j < output.size(); ++j) {
            string name;
            if (output[j].is_terminal == 0){
                name = non_terminal_index[output[j].no];
            } else{
                name = key_list[output[j].value];
            }
            cout << name << ' ';
        }
    }
}

void Analysis() {
    string prog;
    read_prog(prog);
    generate_DFA();
    process_word(prog);
    
    init_rules();
    create_table();
    grammar_check();
    output_error();
    output_rightmost_deviation();
}