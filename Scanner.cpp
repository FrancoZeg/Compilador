#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Token {
    string type;
    string value;
    int line;
};

vector<Token> tokens;
vector<Token> error;

char peek_char(int pos, const vector<char>& v) {
    if (pos + 1 < (int)v.size())
        return v[pos + 1];
    return '\0';
}

void get_char(int& pos) {
    pos++;
}

void get_token(const vector<char>& v, int& i, int& lin) {
    while (true) {
        Token tmp;
        if (v[i] == ' ' || v[i] == '\t') {
            get_char(i);
            continue;
        }
        if (v[i] == '\n') {
            lin++;
            get_char(i);
            continue;
        }
        if (v[i] == '>') {
            if (peek_char(i, v) == '=') { get_char(i); tmp = { "mayeq", ">=", lin }; tokens.push_back(tmp); }
            else if (peek_char(i, v) == '>') {
                get_char(i);
                while (i < (int)v.size() && v[i] != '<' && peek_char(i, v) != '<' && peek_char(i, v) != '\0') {
                    get_char(i);
                    if (i < (int)v.size() && v[i] == '\n') lin++;
                }
                get_char(i);
                if (peek_char(i, v) != '\0') {
                    get_char(i);
                }
                break;
            }
            else {
                tmp = { "mayor", ">", lin };
                tokens.push_back(tmp);
            }
        }
        else if (v[i] == '$') {
            if (peek_char(i, v) == 'n') { get_char(i); tmp = { "entero", "$n", lin }; tokens.push_back(tmp); break; }
            if (peek_char(i, v) == 's') { get_char(i); tmp = { "string", "$s", lin }; tokens.push_back(tmp); break; }
            if (peek_char(i, v) == 'b') { get_char(i); tmp = { "boolean", "$b", lin }; tokens.push_back(tmp); break; }
            get_char(i);
            tmp = { "error", "error type not accepted", lin };
            error.push_back(tmp);
        }
        else if (v[i] == 'w' && peek_char(i, v) == 'h' && peek_char(i + 1, v) == 'e' && peek_char(i + 2, v) == 'n') {
            string lex = "when"; get_char(i); get_char(i); get_char(i);
            tmp = { "when", lex, lin };
            tokens.push_back(tmp);
        }
        else if (v[i] == 'b' && peek_char(i, v) == 'u' && peek_char(i + 1, v) == 't') {
            string lex = "but"; get_char(i); get_char(i);
            tmp = { "but", lex, lin };
            tokens.push_back(tmp);
        }
        else if (v[i] == 'd' && peek_char(i, v) == 'e' && peek_char(i + 1, v) == 'f' && peek_char(i + 2, v) == 'i' && peek_char(i + 3, v) == 'n' && peek_char(i + 4, v) == 'e') {
            string lex = "define"; get_char(i); get_char(i); get_char(i); get_char(i); get_char(i);
            tmp = { "define", lex, lin };
            tokens.push_back(tmp);
        }
        else if (v[i] == 't' && peek_char(i, v) == 'r' && peek_char(i + 1, v) == 'u' && peek_char(i + 2, v) == 'e') {
            string lex = "true"; get_char(i); get_char(i); get_char(i);
            tmp = { "true", lex, lin };
            tokens.push_back(tmp);
        }
        else if (v[i] == 'f' && peek_char(i, v) == 'a' && peek_char(i + 1, v) == 'l' && peek_char(i + 2, v) == 's' && peek_char(i + 3, v) == 'e') {
            string lex = "false"; get_char(i); get_char(i); get_char(i); get_char(i);
            tmp = { "false", lex, lin };
            tokens.push_back(tmp);
        }
        else if (v[i] == '"') {
            string str;
            int tmpi = i + 1, tmp1 = lin;

            // Avanzar hasta encontrar la comilla de cierre o fin del archivo
            while (i < (int)v.size() && peek_char(i, v) != '"') {
                get_char(i);
                if (i < (int)v.size()) {
                    if (v[i] == '\n') lin++;
                    else str.push_back(v[i]);
                }
            }

            if (i >= (int)v.size() - 1) {
                // No se encontró cierre
                i = tmpi; lin = tmp1;
                error.push_back({ "error", "unclosed string", lin });
            }
            else {
                // Consumir la comilla final
                get_char(i);
                tmp = { "string_val", str, lin };
                tokens.push_back(tmp);
            }
        }
        else if (v[i] == '=') {
            if (peek_char(i, v) == '=') { get_char(i); tmp = { "asign", "==", lin };  tokens.push_back(tmp); }
            else { tmp = { "eq", "=", lin }; tokens.push_back(tmp); }
        }
        else if (v[i] == '+') { tmp = { "sum", "+", lin }; tokens.push_back(tmp); }
        else if (v[i] == '-') { tmp = { "res", "-", lin }; tokens.push_back(tmp); }
        else if (v[i] == '/') { tmp = { "div", "/", lin }; tokens.push_back(tmp); }
        else if (v[i] == '*') { tmp = { "mul", "*", lin }; tokens.push_back(tmp); }
        else if (v[i] == '<') {
            if (peek_char(i, v) == '=') { get_char(i); tmp = { "meneq", "<=", lin }; tokens.push_back(tmp); }
            else { tmp = { "menor", "<", lin }; tokens.push_back(tmp); }
        }
        else if (v[i] == '!') {
            if (peek_char(i, v) == '=') { get_char(i); tmp = { "neq", "!=", lin }; tokens.push_back(tmp); }
            else { error.push_back({ "error", "!", lin }); }
        }
        else if (v[i] == ',') { tmp = { "coma", ",", lin }; tokens.push_back(tmp); }
        else if (v[i] == ';') { tmp = { "end", ";", lin }; tokens.push_back(tmp); }
        else if (v[i] == '(') { tmp = { "lpar", "(", lin }; tokens.push_back(tmp); }
        else if (v[i] == ')') { tmp = { "rpar", ")", lin }; tokens.push_back(tmp); }
        else if (v[i] == '{') { tmp = { "lbra", "{", lin }; tokens.push_back(tmp); }
        else if (v[i] == '}') { tmp = { "rbra", "}", lin }; tokens.push_back(tmp); }
        else if (v[i] >= '0' && v[i] <= '9') {
            string num; num.push_back(v[i]);
            while (peek_char(i, v) >= '0' && peek_char(i, v) <= '9') {
                get_char(i);
                num.push_back(v[i]);
            }
            tmp = { "num_val", num, lin };
            tokens.push_back(tmp);
        }
        else if ((v[i] >= 'a' && v[i] <= 'z') || (v[i] >= 'A' && v[i] <= 'Z')) {
            string id; id.push_back(v[i]);
            while ((peek_char(i, v) >= '0' && peek_char(i, v) <= '9') ||
                (peek_char(i, v) >= 'a' && peek_char(i, v) <= 'z') ||
                (peek_char(i, v) >= 'A' && peek_char(i, v) <= 'Z') ||
                (peek_char(i, v) == '_')) {
                get_char(i);
                id.push_back(v[i]);
            }
            tmp = { "id", id, lin };
            tokens.push_back(tmp);
        }
        else {
            error.push_back({ "error", string(1, v[i]), lin });
        }
        break; // salir del while después de procesar un token
    }
}

int main() {
    ifstream archivo("code.txt");
    vector<char> buffer;
    char c;
    while (archivo.get(c)) buffer.push_back(c);

    int lin = 1;

    for (int i = 0; i < (int)buffer.size(); i++) {
        get_token(buffer, i, lin);
    }

    for (auto& t : tokens) {
        if (t.type != "error" && t.type != "comment") {
            cout << "Token: " << t.type << " | Value: " << t.value << " | Line: " << t.line << "\n";
        }
    }

    for (auto& e : error) {
        cout << "Error in line " << e.line << " with value: " << e.value << endl;
    }

    return 0;
}
