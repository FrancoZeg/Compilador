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

vector<int> error;

char peek_char(int pos, const vector<char>& v) {
    if (pos + 1 < (int)v.size())
        return v[pos + 1];
    return '\0';
}

void get_char(int& pos) {
    pos++;
}

Token get_token(const vector<char>& v, int& i, int& lin) {
    while (true) {
        if (v[i] == ' ' || v[i] == '\t') {
            get_char(i);
            continue;
        }
        if (v[i] == '\n') {
            lin++;
            get_char(i);
            continue;
        }

        if (v[i] == '$') {
            if (peek_char(i, v) == 'n') { get_char(i); return { "entero", "$n", lin }; }
            if (peek_char(i, v) == 's') { get_char(i); return { "string", "$s", lin }; }
            if (peek_char(i, v) == 'b') { get_char(i); return { "boolean", "$b", lin }; }
            get_char(i); error.push_back(lin); return { "error", "$?", lin };
        }

        if (v[i] == 'w' && peek_char(i, v) == 'h' && peek_char(i + 1, v) == 'e' && peek_char(i + 2, v) == 'n') {
            string lex = "when"; get_char(i); get_char(i); get_char(i);
            return { "when", lex, lin };
        }
        if (v[i] == 'b' && peek_char(i, v) == 'u' && peek_char(i + 1, v) == 't') {
            string lex = "but"; get_char(i); get_char(i);
            return { "but", lex, lin };
        }
        if (v[i] == 'd' && peek_char(i, v) == 'e' && peek_char(i + 1, v) == 'f' && peek_char(i + 2, v) == 'i' && peek_char(i + 3, v) == 'n' && peek_char(i + 4, v) == 'e') {
            string lex = "define"; get_char(i); get_char(i); get_char(i); get_char(i); get_char(i);
            return { "define", lex, lin };
        }
        if (v[i] == 't' && peek_char(i, v) == 'r' && peek_char(i + 1, v) == 'u' && peek_char(i + 2, v) == 'e') {
            string lex = "true"; get_char(i); get_char(i); get_char(i);
            return { "true", lex, lin };
        }
        if (v[i] == 'f' && peek_char(i, v) == 'a' && peek_char(i + 1, v) == 'l' && peek_char(i + 2, v) == 's' && peek_char(i + 3, v) == 'e') {
            string lex = "false"; get_char(i); get_char(i); get_char(i); get_char(i);
            return { "false", lex, lin };
        }

        if (v[i] == '"') {
            string str; int tmp = i + 1, tmp1 = lin;
            while (peek_char(i, v) != '"' && i < (int)v.size()) {
                get_char(i);
                if (v[i] == '\n') lin++;
                else str.push_back(v[i]);
            }
            if (i == (int)v.size()) {
                i = tmp; lin = tmp1; error.push_back(lin);
                return { "error", "unterminated_string", lin };
            }
            return { "string_val", str, lin };
        }

        if (v[i] == '=') {
            if (peek_char(i, v) == '=') { get_char(i); return { "asign", "==", lin }; }
            return { "eq", "=", lin };
        }
        if (v[i] == '+') return { "sum", "+", lin };
        if (v[i] == '-') return { "res", "-", lin };
        if (v[i] == '/') return { "div", "/", lin };
        if (v[i] == '*') return { "mul", "*", lin };
        if (v[i] == '<') {
            if (peek_char(i, v) == '=') { get_char(i); return { "meneq", "<=", lin }; }
            return { "menor", "<", lin };
        }
        if (v[i] == '>') {
            if (peek_char(i, v) == '=') { get_char(i); return { "mayeq", ">=", lin }; }
            if (peek_char(i, v) == '>') {
                get_char(i);
                while (i < (int)v.size() && v[i] != '<' && peek_char(i, v) != '<') {
                    get_char(i);
                    if (v[i] == '\n') lin++;
                }
                get_char(i);
                return { "comment", ">>...<<", lin };
            }
            return { "mayor", ">", lin };
        }
        if (v[i] == '!') {
            if (peek_char(i, v) == '=') { get_char(i); return { "neq", "!=", lin }; }
            error.push_back(lin); return { "error", "!", lin };
        }

        if (v[i] == ',') return { "coma", ",", lin };
        if (v[i] == ';') return { "end", ";", lin };
        if (v[i] == '(') return { "lpar", "(", lin };
        if (v[i] == ')') return { "rpar", ")", lin };
        if (v[i] == '{') return { "lbra", "{", lin };
        if (v[i] == '}') return { "rbra", "}", lin };

        if (v[i] >= '0' && v[i] <= '9') {
            string num; num.push_back(v[i]);
            while (peek_char(i, v) >= '0' && peek_char(i, v) <= '9') {
                get_char(i);
                num.push_back(v[i]);
            }
            return { "num_val", num, lin };
        }

        if ((v[i] >= 'a' && v[i] <= 'z') || (v[i] >= 'A' && v[i] <= 'Z')) {
            string id; id.push_back(v[i]);
            while ((peek_char(i, v) >= '0' && peek_char(i, v) <= '9') ||
                (peek_char(i, v) >= 'a' && peek_char(i, v) <= 'z') ||
                (peek_char(i, v) >= 'A' && peek_char(i, v) <= 'Z') ||
                (peek_char(i, v) == '_')) {
                get_char(i);
                id.push_back(v[i]);
            }
            return { "id", id, lin };
        }

        error.push_back(lin);
        return { "error", string(1, v[i]), lin };
    }
}

int main() {
    ifstream archivo("code.txt");
    if (!archivo.is_open()) {
        cerr << "Error al abrir code.txt\n";
        return 1;
    }

    vector<char> buffer;
    char c;
    while (archivo.get(c)) buffer.push_back(c);

    int lin = 1;
    vector<Token> tokens;

    for (int i = 0; i < (int)buffer.size(); i++) {
        Token tok = get_token(buffer, i, lin);
        tokens.push_back(tok);
    }

    for (auto& t : tokens) {
        cout << "Token: " << t.type << " | Valor: " << t.value << " | Línea: " << t.line << "\n";
    }

    return 0;
}
