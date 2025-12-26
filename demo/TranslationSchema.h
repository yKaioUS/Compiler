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
    int is_terminal; // 是否为terminal
    int value; // terminal 的 index
    int no;
    string attr;
    int line;
    TOKEN(){
        this->is_terminal = -1;
        this->value = -1;
        this->no = -1;
        attr = "";
        line = 0;
    }
    TOKEN(int is_terminal, int value, int no, string attr, int l){
        this->is_terminal = is_terminal;
        this->value = value;
        this->no = no;
        this->attr = attr;
        this->line = l;
    }
    void clear(){
        this->is_terminal = -1;
        this->value = -1;
        this->no = -1;
        this->attr = "";
        this->line = 0;
    }
};

class State {
public:
    map<char, State *> next;
    //0表示中间节点，1表示规约节点
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
        // 构造变量的DFA
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

    // 构造数字的DFA
    void construct_num() {
        // 数字的形式为：(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?
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

class Node{
public:
    int num;
    string attr;
    Node* fa;
    deque<Node*> son;
    int solve_num;

    Node(){
        attr = "";
        num = 0;
        fa = nullptr;
        solve_num = 0;
    }
};

vector<TOKEN> detection_result;
vector<int> index2line;
vector<DFA *> dfa_list;
string key_list[78] = {
	" ", "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
    "enum", "extern", "float", "for", "goto", "if", "int", "real", "register", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "then",
    "ID", "NUM", "while", "-", "--", "-=", "->", "!", "!=", "%", "%=", "&", "&&", "&=", "(",
    ")", "*", "*=", ",", ".", "/", "/=", ":", ";", "?", "[", "]", "^", "^=", "{", "|", "|=",
    "||", "}", "~", "+", "++", "+=", "<", "<<", "<<=", "<=", "=", "==", ">", ">=", ">>", ">>="
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
// 分析表列顺序为: else 10,if 16,then 29,ID 30,NUM 31,while 32,- 33,+ 65,* 46,/ 50,{ 59,} 63,( 44,) 45,= 72,; 53,< 68,> 74,<= 71,>= 75,== 73
vector<vector<int>> LL1_table;
map<int, int> type2col = {{10, 1}, {16, 2}, {29, 3}, {81, 4}, {80, 5}, {33, 6}, {65, 7}, {46, 8}, {50, 9}, {59, 10}, {63, 11}, {44, 12}, {45, 13}, {72, 14}, {53, 15}, {68, 16}, {74, 17}, {71, 18}, {75, 19}, {73, 20}, {17, 21}, {18, 22}, {-3, 23}};
vector<pair<int, int>> error_msg;
vector<pair<TOKEN, int>> grammar_result;
map<int, int> value2no;
vector<vector<TOKEN>> grammar_result_list;
vector<string> inherent_attr;
Node* Root = new Node();
map<string, pair<string, double>> value_table;
vector<pair<int, int>> semantic_error;

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
                        auto *token = new TOKEN();
                        token->is_terminal = 1;
                        token->value = final_dfa->type;
                        token->attr = detected_s;
                        token->line = line;
                        detection_result.push_back(*token);
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
            auto *token = new TOKEN();
            token->is_terminal = 1;
            token->value = dfa_list[left_dfa[0]]->type;
            token->attr = detected_s;
            token->line = line;
            detection_result.push_back(*token);
            index2line.push_back(line);
        }
    }
}

void init_rules(){

    vector<TOKEN> temp;
    rules.push_back(temp);
    int cfg_num = 30;
    string rules_string="n1 n2 n5\n"
                        "n2 n3 t53 n2\n"
                        "n2 E\n"
                        "n3 t17 t81 t72 t80\n"
                        "n3 t18 t81 t72 t80\n"
                        "n4 n7\n"
                        "n4 n8\n"
                        "n4 n5\n"
                        "n5 t59 n6 t63\n"
                        "n6 n4 n6\n"
                        "n6 E\n"
                        "n7 t16 t44 n9 t45 t29 n4 t10 n4\n"
                        "n8 t81 t72 n11 t53\n"
                        "n9 n11 n10 n11\n"
                        "n10 t68\n"
                        "n10 t74\n"
                        "n10 t71\n"
                        "n10 t75\n"
                        "n10 t73\n"
                        "n11 n13 n12\n"
                        "n12 t65 n13 n12\n"
                        "n12 t33 n13 n12\n"
                        "n12 E\n"
                        "n13 n15 n14\n"
                        "n14 t46 n15 n14\n"
                        "n14 t50 n15 n14\n"
                        "n14 E\n"
                        "n15 t81\n"
                        "n15 t80\n"
                        "n15 t44 n11 t45\n";
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
                auto *token = new TOKEN();
                token->is_terminal = 1;
                token->value = num;
                temp.push_back(*token);
            } else if (type == 'n'){
                ss >> num;
                auto *token = new TOKEN();
                token->is_terminal = 0;
                token->no = num;
                temp.push_back(*token);
            } else if (type == 'E'){
                auto *token = new TOKEN();
                token->is_terminal = 1;
                token->value = -1;
                temp.push_back(*token);
            }
        }
    }
}

void create_table(){
    LL1_table.resize(16);
    string table_csv = ",,,,,,,,,,1,,,,,,,,,,,1,1,\n"
                       ",,,,,,,,,,3,,,,,,,,,,,2,2,\n"
                       ",,,,,,,,,,,,,,,,,,,,,4,5,\n"
                       ",,6,,7,,,,,,8,,,,,,,,,,,,,\n"
                       ",,,,,,,,,,9,,,,,,,,,,,,,\n"
                       ",,10,,10,,,,,,10,11,,,,,,,,,,,,\n"
                       ",,12,,,,,,,,,,,,,,,,,,,,,\n"
                       ",,,,13,,,,,,,,,,,,,,,,,,,\n"
                       ",,,,14,14,,,,,,,14,,,,,,,,,,,\n"
                       ",,,,,,,,,,,,,,,,15,16,17,18,19,,,\n"
                       ",,,,20,20,,,,,,,20,,,,,,,,,,,\n"
                       ",,,,,,22,21,,,,,,23,,23,23,23,23,23,23,,,\n"
                       ",,,,24,24,,,,,,,24,,,,,,,,,,,\n"
                       ",,,,,,27,27,25,26,,,,27,,27,27,27,27,27,27,,,\n"
                       ",,,,28,29,,,,,,,30,,,,,,,,,,,";

    stringstream ss(table_csv);
    for (int i = 1; i <= 15; ++i) {
        string line;
        getline(ss, line);
        stringstream line_ss(line);
        int cnt = 0;
        while (true){
            int num;
            line_ss >> num;
            if (line_ss.fail()){
                line_ss.clear();
                char c = (char) line_ss.get();
                if (c == ','){
                    LL1_table[i].push_back(-1);
                    cnt++;
                    line_ss.clear();
                } else break;
            } else{
                LL1_table[i].push_back(num);
                line_ss.get();
                cnt++;
            }
        }
    }
}

int search_table(int row, int num){
    return LL1_table[row][type2col[num]];
}

int error_handler(int type, int index, int value){
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
    TOKEN end;
    end.is_terminal = 1;
    end.value = -3;
    q.push(end);
    detection_result.push_back(end);
    grammar_result_list.resize(1000);

    TOKEN begin;
    begin.is_terminal = 0;
    begin.no = 1;
    begin.value = 0;
    q.push(begin);

    Root->num = 0;
    value2no[0] = 1;
    Node* now_node = Root;

    int now = 0;
    int level = 0;
    stack<pair<int, int>> nt_stack;
    int nt_num = 1;
    bool first_time = true;

    while (!q.empty()){
        TOKEN top = q.top();
        q.pop();
        if (!(top.is_terminal == 1 && top.value == -2)){
            grammar_result.emplace_back(top, level);
            if (!nt_stack.empty()) grammar_result_list[nt_stack.top().first].push_back(top);
        }
        if (top.is_terminal == 1 && top.value == -1){
            continue;
        }
        if (top.is_terminal == 1){
            if (top.value == -2){
                level--;
                nt_stack.pop();
                now_node = now_node->fa;
                continue;
            }else if (top.value == -3){
                if (detection_result[now].value == -3){
                    break;
                } else{
                    error_handler(0, now, detection_result[now].value);
                    continue;
                }
            }
            else{
                if (top.value == detection_result[now].value){
                    if (!nt_stack.empty()) {
                        grammar_result_list[nt_stack.top().first].back().attr = detection_result[now].attr;
                        grammar_result_list[nt_stack.top().first].back().line = detection_result[now].line;
                    }
                    now++;
                } else{
                    error_handler(0, now, top.value);
                    continue;
                }
            }
        } else{
            int row = top.no;
            int num = search_table(row, detection_result[now].value);
            if (num == -1){
                error_handler(1, now, detection_result[now].value);
                return;
            }else{
                nt_stack.push({top.value, num});
                value2no[top.value] = num;
                if (first_time){
                    first_time = false;
                } else{
                    now_node->solve_num++;
                    now_node = now_node->son[now_node->solve_num-1];
                }

                TOKEN lower;
                lower.is_terminal = 1;
                lower.value = -2;
                q.push(lower);
                level++;
                for (int i = rules[num].size() - 1; i >= 0; --i) {
                    if (rules[num][i].is_terminal == 0){
                        TOKEN temp;
                        temp.is_terminal = 0;
                        temp.no = rules[num][i].no;
                        temp.value = nt_num;
                        nt_num++;

                        q.push(temp);
                        Node* tmp_node = new Node();
                        tmp_node->num = temp.value;
                        tmp_node->fa = now_node;
                        tmp_node->solve_num = 0;
                        now_node->son.push_front(tmp_node);

                    }else{
                        q.push(rules[num][i]);
                    }
                }
            }
        }
    }
    inherent_attr.resize(nt_num);
}

void output_grammar_error(){
    for (int i = 0; i < error_msg.size(); ++i) {
        // 格式为：语法错误,第x行,缺少"xxx"
        cout << "语法错误,第" << error_msg[i].first << "行,缺少\"" << key_list[error_msg[i].second] << "\"" << endl;
    }
}

void process_grammar(){
    init_rules();
    create_table();
    grammar_check();
    output_grammar_error();
}

void semantic_error_handler(int line, int type){
    // 为int赋值real 不存在的变量  除0错误
    semantic_error.emplace_back(line, type);
}

double process_value(Node* node){
    string id, id2;
    string num_string, num_string2;
    double num_value, num_value2;
    switch (value2no[node->num]) {
        case 1:
            process_value(node->son[0]);
            process_value(node->son[1]);
            break;
        case 2:
            process_value(node->son[0]);
            process_value(node->son[1]);
            break;
        case 3:
            break;
        case 4:
            id = grammar_result_list[node->num][1].attr;
            num_string = grammar_result_list[node->num][3].attr;
            num_value = stod(num_string);
            if (num_value == (int)num_value){
                value_table[id] = {"int", num_value};
            }else{
                semantic_error_handler(grammar_result_list[node->num][1].line, 1);
                value_table[id] = {"int", (int)num_value};
            }
            break;
        case 5:
            id = grammar_result_list[node->num][1].attr;
            num_string = grammar_result_list[node->num][3].attr;
            num_value = stod(num_string);
            value_table[id] = {"real", num_value};
            break;
        case 6:
            process_value(node->son[0]);
            break;
        case 7:
            process_value(node->son[0]);
            break;
        case 8:
            process_value(node->son[0]);
            break;
        case 9:
            process_value(node->son[0]);
            break;
        case 10:
            process_value(node->son[0]);
            process_value(node->son[1]);
            break;
        case 11:
            break;
        case 12:
            process_value(node->son[0]);
            num_string = node->son[0]->attr;
            if (isdigit(num_string[0])){
                num_value = stod(num_string);
            } else{
                num_value = value_table[num_string].second;
            }
            if (num_value != 0){
                process_value(node->son[1]);
            } else{
                process_value(node->son[2]);
            }
            break;
        case 13:
            process_value(node->son[0]);
            id = grammar_result_list[node->num][0].attr;
            num_string = node->son[0]->attr;
            if (isdigit(num_string[0])){
                num_value = stod(num_string);
            } else{
                if (value_table.count(num_string) == 0){
                    semantic_error_handler(grammar_result_list[node->num][0].line, 2);
                    break;
                }else {
                    num_value = value_table[num_string].second;
                }
            }
            if (value_table.count(id) == 0){
                semantic_error_handler(grammar_result_list[node->num][0].line, 2);
            } else{
                if (value_table[id].first == "int" && (num_value != (int) num_value)){
                    semantic_error_handler(grammar_result_list[node->num][0].line, 1);
                    value_table[id].second = (int)num_value;
                } else{
                    value_table[id].second = num_value;
                }
            }
            break;
        case 14:
            process_value(node->son[0]);
            process_value(node->son[1]);
            process_value(node->son[2]);

            num_string = node->son[0]->attr;
            num_string2 = node->son[2]->attr;
            if (isdigit(num_string[0])){
                num_value = stod(num_string);
            } else{
                if (value_table.count(num_string) == 0){
                    semantic_error_handler(grammar_result_list[node->num][0].line, 2);
                    num_value = -1;
                }else {
                    num_value = value_table[num_string].second;
                }
            }
            if (isdigit(num_string2[0])){
                num_value2 = stod(num_string2);
            } else{
                if (value_table.count(num_string2) == 0){
                    semantic_error_handler(grammar_result_list[node->num][0].line, 2);
                    num_value2 = -1;
                }else {
                    num_value2 = value_table[num_string2].second;
                }
            }
            if (node->son[1]->attr == "<"){
                if (num_value < num_value2){
                    node->attr = "1";
                } else{
                    node->attr = "0";
                }
            } else if (node->son[1]->attr == ">"){
                if (num_value > num_value2){
                    node->attr = "1";
                } else{
                    node->attr = "0";
                }
            } else if (node->son[1]->attr == "<="){
                if (num_value <= num_value2){
                    node->attr = "1";
                } else{
                    node->attr = "0";
                }
            } else if (node->son[1]->attr == ">="){
                if (num_value >= num_value2){
                    node->attr = "1";
                } else{
                    node->attr = "0";
                }
            } else if (node->son[1]->attr == "=="){
                if (num_value == num_value2){
                    node->attr = "1";
                } else{
                    node->attr = "0";
                }
            }
            break;
        case 15:
            node->attr = "<";
            break;
        case 16:
            node->attr = ">";
            break;
        case 17:
            node->attr = "<=";
            break;
        case 18:
            node->attr = ">=";
            break;
        case 19:
            node->attr = "==";
            break;
        case 20:
            process_value(node->son[0]);
            inherent_attr[node->son[1]->num] = node->son[0]->attr;
            process_value(node->son[1]);
            node->attr = node->son[1]->attr;
            break;
        case 21:
            process_value(node->son[0]);
            num_value = stod(node->son[0]->attr) + stod(inherent_attr[node->num]);
            inherent_attr[node->son[1]->num] = to_string(num_value);
            process_value(node->son[1]);
            node->attr = node->son[1]->attr;
            break;
        case 22:
            process_value(node->son[0]);
            num_value = stod(inherent_attr[node->num]) - stod(node->son[0]->attr);
            inherent_attr[node->son[1]->num] = to_string(num_value);
            process_value(node->son[1]);
            node->attr = node->son[1]->attr;
            break;
        case 23:
            node->attr = inherent_attr[node->num];
            break;
        case 24:
            process_value(node->son[0]);
            inherent_attr[node->son[1]->num] = node->son[0]->attr;
            process_value(node->son[1]);
            node->attr = node->son[1]->attr;
            break;
        case 25:
            process_value(node->son[0]);
            num_value = stod(node->son[0]->attr) * stod(inherent_attr[node->num]);
            inherent_attr[node->son[1]->num] = to_string(num_value);
            process_value(node->son[1]);
            node->attr = node->son[1]->attr;
            break;
        case 26:
            process_value(node->son[0]);
            if (stod(node->son[0]->attr) != 0){
                num_value = stod(inherent_attr[node->num]) / stod(node->son[0]->attr);
            }else{
                semantic_error_handler(grammar_result_list[node->num][0].line, 3);
                num_value = 1;
            }
            inherent_attr[node->son[1]->num] = to_string(num_value);
            process_value(node->son[1]);
            node->attr = node->son[1]->attr;
            break;
        case 27:
            node->attr = inherent_attr[node->num];
            break;
        case 28:
            num_string = grammar_result_list[node->num][0].attr;
            if (value_table.count(num_string) == 0){
                semantic_error_handler(grammar_result_list[node->num][0].line, 2);
                num_value = -1;
            }else {
                num_value = value_table[num_string].second;
            }
            node->attr = to_string(num_value);
            break;
        case 29:
            node->attr = grammar_result_list[node->num][0].attr;
            break;
        case 30:
            process_value(node->son[0]);
            node->attr = node->son[0]->attr;
            break;
    }
}

void output_semantic_error(){
    for (int i = 0; i < semantic_error.size(); ++i) {
        if (semantic_error[i].second == 2) continue;

        cout << "error message:line " << semantic_error[i].first << ",";
        if (semantic_error[i].second == 1){
            cout << "realnum can not be translated into int type" << endl;
        } else if (semantic_error[i].second == 2){
            cout << "undefined variable" << endl;
        } else if (semantic_error[i].second == 3){
            cout << "division by zero" << endl;
        }
    }
}

void output_semantic(){
    for (const auto& i:value_table) {
        cout << i.first << ": " << i.second.second << endl;
    }
}

void Analysis() {
    string prog;
    read_prog(prog);
    generate_DFA();
    process_word(prog);
    process_grammar();
    process_value(Root);
    output_semantic_error();
    if (semantic_error.empty()) output_semantic();
}
