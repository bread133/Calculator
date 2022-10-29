#include<iostream>
#include<stack>
#include<string>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<fstream>

using namespace std;

/*
* Что делать?
* Очевидно ввести строку (сделано)
* Постфиксно кинуть в стек токенов, но прежде распарсить
* Вычислить
* Вывести на экран
* ???
* PROFIT!!!
*/

enum types
{
	var, //все типы
	num, //число
	op_br, //(
	cl_br, //)
	sqrT, //функции, но у нас такая одна
	op //операция
};

struct token
{
	string name;
	types type;
	token(string name, types type)
	{
		this->name = name;
		this->type = type;
	}
};

double op_plus(stack<double>& s);
double op_minus(stack<double>& s);
double op_mul(stack<double>& s);
double op_div(stack<double>& s);
double op_deg(stack<double>& s);
double op_neg(stack<double>& s);
double op_sqrt(stack<double>& s);

bool is_del(char s, set<char> del);
vector<token> parse(string& h);
vector<token> postfix(vector<token>& v);
double calculate(vector<token>& z);

int main()
{
	string h;
	getline(cin, h);

	vector<token> a = parse(h); //работает
	vector<token> p = postfix(a); 
	double res = calculate(p);

	cout << res << endl;

	return 0;
}

bool is_del(char s, set<char> del) //является ли текущий знак разделителем
{
	return del.count(s) > 0;
}

vector<token> parse(string& h)
{
	vector<token> res;

	set<char> del; //разделители
	string temp = " ()+-*/^√s";
	for(int i = 0; i < temp.size(); i++)
		del.insert(temp[i]); //без to_string добавить char в set<string> нельзя

	string l = h + ' '; //для безболезненного выхода из следующего цикла
	string name; //имя токена

	int i = 0;
	while(i < l.size()) //толкаем в вектора созданные в цикле токены
	{
		name = "";
		if(is_del(l[i], del))
		{
			if (l[i] == ' ') //пробел и другие символы перепрыгиваем
			{
				i++;
				continue;
			}

			if(l[i] == 's')
			{
				name = "s";
				i += 4;
			}
			else
			{
				name = l[i];
				i++;
			}
		}
		else
		{
			if (l[i] == 'q' || l[i] == 'r' || l[i] == 't') { cout << "Вы забыли s" << endl; }
			while(!is_del(l[i], del))
			{
				name += l[i];
				i++;
			}
		}
		res.push_back(token(name, var));
	}

	for(int j = 0; j < res.size(); j++)
	{
		if (res[j].name[0] == '(')
		{
			res[j].type = op_br;
		}
		if (res[j].name[0] == ')')
		{
			res[j].type = cl_br;
		}
		if (isdigit(res[j].name[0]))
		{
			res[j].type = num;
		}
		if (res[j].name[0] == '√' || res[j].name[0] == 's')
		{
			res[j].type = sqrT;
		}
		if (res[j].name[0] == '+' || res[j].name[0] == '-' || res[j].name[0] == '/' || res[j].name[0] == '*' || res[j].name[0] == '^')
		{
			res[j].type = op;
		}
	}

	for (int j = 0; j < res.size(); j++)
		if (res[j].name[0] == '-' && (j == 0 || res[j + 1].type == op_br || res[j + 1].type == sqrT || res[j - 1].type == op_br))
			res[j].name = 'n';

	return res;
}

vector<token> postfix(vector<token>& v)
{
	map<string, int> q; //расставляем приоритеты
	q["+"] = 1;
	q["-"] = 1;
	q["*"] = 2;
	q["/"] = 2;
	q["s"] = 3;
	q["n"] = 1;
	q["√"] = 3;

	vector<token> res;
	stack<token> st;
	for(int i = 0; i < v.size(); i++) //шайтан-машина
	{
		switch (v[i].type)
		{
		case var:
		case num:
			res.push_back(v[i]);
			break;
		case op_br:
			st.push(v[i]);
			break;
		case cl_br:
			while(st.top().type != op_br) //заносим все в результирующий вектор, выставляя таким образом порядок операций
			{
				res.push_back(st.top());
				st.pop();
			}
			st.pop(); //удаляем скобку
			if (!st.empty() && st.top().type == sqrT) //тут была ошибка (при пустом стеке условие без первой части ломается)
			{ 
				res.push_back(st.top()); 
				st.pop(); 
			}
			break;
		case op:
			if(!st.empty())
				while(!st.empty() && st.top().type == op && q[v[i].name] <= q[st.top().name]) //вот тут записываем в постфиксной форме
				{
					res.push_back(st.top());
					st.pop();
				}
			st.push(v[i]);
			break;
		case sqrT:
			while(!st.empty() && st.top().type == op && q[v[i].name] <= q[st.top().name])
			{
				res.push_back(st.top());
				st.pop();
			}
			st.push(v[i]);
			break;
		}
	}

	while (!st.empty()) 
	{ 
		res.push_back(st.top()); 
		st.pop();
	}
	return res;
}

typedef double(*op_func)(stack<double>&);//это тип для функций операций, написанные ниже
double calculate(vector<token>& z)
{
	map<string, op_func> ops;//карта из обозначения операции и функции этой операции
	ops["+"] = op_plus; 
	ops["-"] = op_minus;
	ops["*"] = op_mul;
	ops["/"] = op_div;
	ops["^"] = op_deg;
	ops["n"] = op_neg;
	ops["s"] = op_sqrt;

	stack<double> res;
	for(int i = 0; i < z.size(); i++)
		switch(z[i].type)
		{
		case num:
			res.push(atof(z[i].name.c_str())); //добавляем в стек преобразованный в число элемент
			break;
		case sqrT:
		{ //без фигурных скобок студия ругается, странно, что в случае op все в порядке
			map<string, op_func>::iterator k = ops.begin();
			while (k != ops.end())
			{
				if (k->first == z[i].name)
				{
					res.push(k->second(res));
					break;
				}
				k++;
			}
		}
		break;
		case op:
			map<string, op_func>::iterator l = ops.begin(); //запускаем перебор карты ops
			while(l != ops.end())
			{
				if(l->first == z[i].name) //если операция совпадает с именем токена из вектора
				{
					res.push(l->second(res)); //выполняем операцию, которая стоит в карте после строки
					break;
				}
				l++;
			}
			break;
		}

	return res.top();
}

//все все все операции
double op_plus(stack<double>& s)
{
	double a, b;

	a = s.top();
	s.pop();

	b = s.top();
	s.pop();

	return a + b;
}

double op_minus(stack<double>& s)
{
	double a, b;

	a = s.top();
	s.pop();

	b = s.top();
	s.pop();

	return b - a;
}

double op_mul(stack<double>& s)
{
	double a, b;

	a = s.top();
	s.pop();

	b = s.top();
	s.pop();

	return a * b;
}

double op_div(stack<double>& s)
{
	double a, b;

	a = s.top();
	s.pop();

	b = s.top();
	s.pop();

	return b / a;
}

double op_deg(stack<double>& s)
{
	double a, b;

	a = s.top();
	s.pop();

	b = s.top();
	s.pop();

	return pow(b, a);
}

double op_neg(stack<double>& s)
{
	double a;

	a = s.top();
	s.pop();

	return -a;
}

double op_sqrt(stack<double>& s)
{
	double a;

	a = s.top();
	s.pop();

	return sqrt(a);
}