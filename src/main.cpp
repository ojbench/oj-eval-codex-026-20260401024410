#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

// Simple arithmetic expression evaluator supporting +,-,*,/ and parentheses.
// Assumptions:
// - Input is a single line expression.
// - Integers (optionally with leading +/-), spaces allowed.
// - Division is integer division truncating toward zero.
// - Unary + and - are supported.

namespace {

struct Token {
    enum Type { NUM, PLUS, MINUS, MUL, DIV, LP, RP, END } type;
    long long val;
};

struct Lexer {
    const string &s; size_t i{0};
    explicit Lexer(const string &str) : s(str) {}
    void skip() { while (i < s.size() && isspace((unsigned char)s[i])) ++i; }
    Token next() {
        skip();
        if (i >= s.size()) return {Token::END, 0};
        char c = s[i];
        if (isdigit((unsigned char)c)) {
            long long v = 0;
            while (i < s.size() && isdigit((unsigned char)s[i])) {
                v = v * 10 + (s[i] - '0');
                ++i;
            }
            return {Token::NUM, v};
        }
        ++i;
        switch (c) {
            case '+': return {Token::PLUS, 0};
            case '-': return {Token::MINUS, 0};
            case '*': return {Token::MUL, 0};
            case '/': return {Token::DIV, 0};
            case '(': return {Token::LP, 0};
            case ')': return {Token::RP, 0};
            default: return {Token::END, 0};
        }
    }
};

struct Parser {
    Lexer lex;
    Token cur;
    explicit Parser(const string &s) : lex(s) { cur = lex.next(); }
    void consume(Token::Type t){ if(cur.type==t) cur=lex.next(); }

    // expr = term { (+|-) term }
    long long parse_expr(){
        long long v = parse_term();
        while (cur.type == Token::PLUS || cur.type == Token::MINUS) {
            auto op = cur.type; consume(op);
            long long rhs = parse_term();
            if (op == Token::PLUS) v += rhs; else v -= rhs;
        }
        return v;
    }

    // term = factor { (*|/) factor }
    long long parse_term(){
        long long v = parse_factor();
        while (cur.type == Token::MUL || cur.type == Token::DIV) {
            auto op = cur.type; consume(op);
            long long rhs = parse_factor();
            if (op == Token::MUL) v *= rhs; else {
                // integer division truncating toward zero
                if (rhs == 0) throw runtime_error("division by zero");
                long long q = v / rhs; // C++ truncates toward zero
                v = q;
            }
        }
        return v;
    }

    // factor = number | '(' expr ')' | ('+'|'-') factor
    long long parse_factor(){
        if (cur.type == Token::PLUS) { consume(Token::PLUS); return +parse_factor(); }
        if (cur.type == Token::MINUS) { consume(Token::MINUS); return -parse_factor(); }
        if (cur.type == Token::NUM) { long long v = cur.val; consume(Token::NUM); return v; }
        if (cur.type == Token::LP) { consume(Token::LP); long long v = parse_expr(); consume(Token::RP); return v; }
        return 0; // fallback on malformed input
    }
};

} // namespace

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<string> lines;
    string line;
    while (getline(cin, line)) {
        // keep all lines to support multi-line input (one expr per line)
        // skip purely whitespace lines
        bool allspace = true;
        for (char c : line) if (!isspace((unsigned char)c)) { allspace = false; break; }
        if (!allspace) lines.push_back(line);
    }
    if (lines.empty()) return 0;
    for (size_t idx = 0; idx < lines.size(); ++idx) {
        Parser p(lines[idx]);
        try {
            long long ans = p.parse_expr();
            cout << ans;
        } catch (...) {
            cout << 0;
        }
        if (idx + 1 < lines.size()) cout << '\n';
    }
    return 0;
}
