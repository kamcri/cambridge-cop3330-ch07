/*
 *  UCF COP3330 Fall 2021 Assignment 7 Solution
 *  Copyright 2021 Kameryn Cambridge
 */

const char number = '8';
const char quit = 'q';
const char print = ';';
const char let = 'L'; 
const char name = 'a';
const char constant =  'C';
const string result = "= ";
const string letkey = "let";
const string prompt = "> ";


class Token{
public: 
    char kind;
    double value;
    string name;
    Token(char k): 
        kind{k}, value{0.0} { }

    Token(char k, double v): 
        kind{k}, value{v}{ }

    Token(char k, string n):
        kind{k}, name{n}{ }
};

class Token_stream{
    public:
        Token get(); //get a token
        void putback(Token t);
        void ignore(char c);
    private:
        //implementation details the user does not have access to. 
        bool full; //check if a token is already in the buffer.
        Token buffer {'0'}; //putback will save the token here 
};

//Member function definition outside of its class for clarity
Token Token_stream::get(){
    if(full){
        full = false;
        return buffer;
    }
    char ch;
    cin >> ch;
    switch (ch){
    case quit:
    case print:
    case '(': 
    case ')': 
    case '+': 
    case '-': 
    case '/': 
    case '*': 
    case '%':
    case '=':
        return Token{ch};

    case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
        cin.putback(ch);
        double val;
        cin >> val;
        return Token{number,val}; 
    }
    default:
        if(isalpha(ch)){
            string s;
            s += ch;
            while(cin.get(ch) && (isalpha(ch) || isdigit(ch)) || ch == '_') s += ch;
            cin.putback(ch);
            if(s == letkey) {
                return Token(let);
            }
            if(s == "const") {
                return Token(constant);
            }
            return Token(name, s);
        }
        error("Bad token.");
    }
}

class Variable{
    public:
        string name;
        double value;
        bool is_constant; //true = constant false = variable
        Variable(string n, double v, bool c = false):
            name(n), value (v){}
};

Token_stream ts;

vector<Variable> var_table;


void Token_stream::putback(Token t){
    if(full) error("putback() into  full buffer");//test for a pre condition
    buffer = t;
    full = true;
}

void Token_stream::ignore(char c){
    //c reperesents the kind of token
    if(full && c==buffer.kind){
        full = false;
        return;
    }
    full = false;
    char ch = 0;
    while(cin >> ch){
        if(ch==c) return;
    }
}

//go through every variable in var_table and see if name matches the argument 
double get_value(string s){
    for(Variable& v: var_table){
        if(v.name == s) {
            return v.value;
        }
        error("get: undefined variable ", s);
    }
}

//give a variable a new value
void set_value(string s, double d){
    for(int i = 0; i < var_table.size(); ++i){
        if(var_table[i].name == s){
            if(var_table[i].is_constant == true){
                error ("cannot redefine constant ", s);
            }
            var_table[i].value = d;
            return;
        }
        error("set: undefined variable ", s);
    }
}

//check if variable exists already
bool is_declared(string var){
    for(int i = 0; i < var_table.size(); ++i){
        if(var_table[i].name == var) return true;
        return false;
    }
}

//adds a variable to the var_table
double define_name(string var, double val, bool con = false){
    if(is_declared(var)) error(var, " declared twice");
    var_table.push_back(Variable(var, val, con));
    return val;
}

//forward declare 
double expression();

double primary(){
    Token t = ts.get();
        switch (t.kind)
        {
        case '(':
        {
            double d = expression();
            t = ts.get();
            if (t.kind != ')') error("')' expected");
            return d;
        }
        case name: 
        {
            Token t2 = ts.get();
            if(t2.kind == '='){
                double d = expression();
                set_value(t.name, d);
                return d;
            }
            else{
                ts.putback(t2);
                return get_value(t.name);
            }
        }
        case number:
            return t.value;
        case '-':
            return - primary();
        case '+':
            return primary();
        default:
            error("primary expected.");
    }
}

double term(){
    double left = primary();
    Token t = ts.get();

    while(true){
        switch (t.kind){
        case '*': 
            left *= primary();
            t = ts.get();
            break;
        case '/':
        {
            double d = primary();
            if(d == 0) error("divide by zero.");
            left /= d;
            t = ts.get();
            break;
        }
        case '%':
        {
            double d = primary();
            if(d == 0) error("%: Divide by 0\n");
            left = fmod(left, d);
            t = ts.get();
            break;
        }
        default:
            ts.putback(t);
            return left;
        }
    }

}

double expression(){
    double left = term();
    Token t = ts.get();

    while(true){
        switch (t.kind)
        {
        case '+':
            left += term();
            t = ts.get();
            break;
        case '-':
            left -= term();
            t = ts.get();
            break;
        default:
            ts.putback(t);
            return left;
        }
    }
}

//ensures that whatever comes after "let" is a Name followed by an =, followed by an Expression
//variable with that name stirng will be addded to var_table
double declaration(bool is_cons = false){
    Token t = ts.get();
    if(t.kind != name) error("name expected in declaration");
    string var_name = t.name;

    Token t2 = ts.get();
    if(t2.kind != '=') error("= missing in declaration of ", var_name);

    double d = expression();
    define_name(var_name, d, is_cons);
    return d;
}

//statement will print whatever it receives from declaration
double statement(){
    Token t = ts.get();
    switch (t.kind)
    {
    case let:
        return declaration(false);
    case constant:
        return declaration(false);
    default:
        ts.putback(t);
        return expression();
    }
}

void clean_up_mess(){
    ts.ignore(print);
}


//allows for several calculations in the calculator 
void calculate(){
    while(cin){
        try{
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print)
            {
                t = ts.get();
            }
            if(t.kind == quit) return;
            ts.putback(t);
            cout << result << statement() << endl;
        }
        catch(exception& e){
            cerr << e.what() << endl;
            clean_up_mess();
        }
    }
}

int main(){
    try{
        //predefined names 
        define_name("pi", 3.1415926535, true);
        define_name("e", 2.7182818284, true);

        calculate();
        keep_window_open();
        return 0;
    }
    catch(exception& e){
        cerr << e.what() << "\n";
        keep_window_open("~~");
        return 1;
    }
    catch(...){
        cerr << "exception\n";
        keep_window_open("~~");
        return 2;
    }
}
