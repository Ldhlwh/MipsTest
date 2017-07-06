#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <deque>
#include <map>
using namespace std;

//ifstream infile1("test.s"), infile2("test.in");
//ofstream outfile("test.out");

deque <string> text;
const int sp = 29;
bool dot;// data = 0, text = 1
const int mem_size = 4000000;
char mem[4000000] = {0};
int PC[6];
int PD = 0;//present data
int occ[2];

map <string, int> t_label, d_label;
map <string, int> op_type;

string regname[] = {"ze", "at", "v0", "v1", "a0", "a1", "a2", "a3",
"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9",
"k0", "k1", "gp", "sp", "fp", "ra", "lo", "hi"};

union Word
{
	int w;
	unsigned int uw;	
	short h[2];
	unsigned short uh[2];
	char c[4];
	Word(int in) {w = in;}
	Word(unsigned int uin) {uw = uin;}
	Word(short in1, short in2) {h[0] = in1; h[1] = in2;}
	Word(unsigned short uin1, unsigned short uin2) {uh[0] = uin1; uh[1] = uin2;}
	Word(char in1, char in2, char in3, char in4) {c[0] = in1; c[1] = in2; c[2] = in3; c[3] = in4;}
	Word(int pos, char x) {c[0] = mem[pos]; c[1] = mem[pos + 1]; c[2] = mem[pos + 2]; c[3] = mem[pos + 3];}
};

union HalfWord
{
	short h;
	unsigned short uh;
	char c[2];
	HalfWord(short in) {h = in;}
	HalfWord(unsigned short uin) {uh = uin;}
	HalfWord(char in1, char in2) {c[0] = in1; c[1] = in2;}
	HalfWord(int pos, char x) {c[0] = mem[pos]; c[1] = mem[pos + 1];}
};

union LongWord
{
	long long l;
	int w[2];
	LongWord(long long in) {l = in;}
	LongWord(int in1, int in2) {w[0] = in1; w[1] = in2;}	
};

class Register
{
public:
	//int type;
	int elem;
	int occupied;
	Register()
	{
		elem = 0;
		occupied = 0;
		//type = 4;
	}
}reg[34];

void offspace(string &str)
{
	int l = str.length();
	int cnt = 0;
	while(str[cnt] == ' ' || str[cnt] == '\t')
		cnt++;
	str = str.substr(cnt, l - cnt);
}

void AddOpType()
{
	op_type["add"] = 1;
	op_type["addu"] = 1;
	op_type["addiu"] = 1;
	op_type["sub"] = 1;
	op_type["subu"] = 1;
	op_type["mul"] = 1;
	op_type["mulu"] = 1;
	op_type["tmul"] = 2;
	op_type["tmulu"] = 2;
	op_type["div"] = 1;
	op_type["divu"] = 1;
	op_type["tdiv"] = 9;
	op_type["tdivu"] = 9;
	op_type["xor"] = 1;
	op_type["xoru"] = 1;
	op_type["neg"] = 2;
	op_type["negu"] = 2;
	op_type["rem"] = 1;
	op_type["remu"] = 1;
	op_type["li"] = 2;
	op_type["seq"] = 1;
	op_type["sge"] = 1;
	op_type["sgt"] = 1;
	op_type["sle"] = 1;
	op_type["slt"] = 1;
	op_type["sne"] = 1;
	op_type["b"] = 12;
	op_type["beq"] = 3;
	op_type["bne"] = 3;
	op_type["bge"] = 3;
	op_type["ble"] = 3;
	op_type["bgt"] = 3;
	op_type["blt"] = 3;
	op_type["beqz"] = 4;
	op_type["bnez"] = 4;
	op_type["blez"] = 4;
	op_type["bgez"] = 4;
	op_type["bgtz"] = 4;
	op_type["bltz"] = 4;
	op_type["j"] = 12;
	op_type["jr"] = 5;
	op_type["jal"] = 12;
	op_type["jalr"] = 5;
	op_type["la"] = 6;
	op_type["lb"] = 6;
	op_type["lh"] = 6;
	op_type["lw"] = 6;
	op_type["sb"] = 10;
	op_type["sh"] = 10;
	op_type["sw"] = 10;
	op_type["move"] = 2;
	op_type["mfhi"] = 11;
	op_type["mflo"] = 11;
	op_type["nop"] = 7;
	op_type["syscall"] = 8;
}

int num(string name)
{
	if(name == "ze") return 0;
	if(name == "at") return 1;
	if(name == "v0") return 2;
	if(name == "v1") return 3;
	if(name == "a0") return 4;
	if(name == "a1") return 5;
	if(name == "a2") return 6;
	if(name == "a3") return 7;
	if(name == "t0") return 8;
	if(name == "t1") return 9;
	if(name == "t2") return 10;
	if(name == "t3") return 11;
	if(name == "t4") return 12;
	if(name == "t5") return 13;
	if(name == "t6") return 14;
	if(name == "t7") return 15;
	if(name == "s0") return 16;
	if(name == "s1") return 17;
	if(name == "s2") return 18;
	if(name == "s3") return 19;
	if(name == "s4") return 20;
	if(name == "s5") return 21;
	if(name == "s6") return 22;
	if(name == "s7") return 23;
	if(name == "t8") return 24;
	if(name == "t9") return 25;
	if(name == "k0") return 26;
	if(name == "k1") return 27;
	if(name == "gp") return 28;
	if(name == "sp") return 29;
	if(name == "s8") return 30;
	if(name == "fp") return 30;
	if(name == "ra") return 31;
	if(name == "lo") return 32;
	if(name == "hi") return 33;
}

void Write(int pos, int a)
{
	for(int i = 0; i < 4; i++)
		mem[pos + i] = Word(a).c[i];
}

void Write(int pos, unsigned int a)
{
	for(int i = 0; i < 4; i++)
		mem[pos + i] = Word(a).c[i];
}

void Write(int pos, unsigned short a)
{
	for(int i = 0; i < 2; i++)
		mem[pos + i] = HalfWord(a).c[i];
}

void Write(int pos, short a)
{
	for(int i = 0; i < 2; i++)
		mem[pos + i] = HalfWord(a).c[i];
}

void Write(int pos, char a)
{
	mem[pos] = a;
}

void WR(int r, int a)
{
	reg[r].elem = a;
	//reg[r].type = 4;
}

void WR(int r, unsigned int a)
{
	reg[r].elem = a;
	//reg[r].type = -4;
}

void WR(int r, unsigned short a)
{
	reg[r].elem = a;
	//reg[r].type = -2;
}

void WR(int r, short a)
{
	reg[r].elem = a;
	//reg[r].type = 2;
}

void WR(int r, char a)
{
	reg[r].elem = a;
	//reg[r].type = 1;
}

class I;
class II;
class III;
class IV;
class V;

class I
{
public:
	int todo;//PC register
	string str;
	void GetInfo(int to)
	{
		todo = to;
	}
	int DoOp()
	{
		str = text[todo];
		return 1;
	}
}IF;

class II
{
public:
	string str, op;
	string Rt, R1, R2;//register name without $
	int RtNum, R1Num, R2Num;
	string lab, el;
	int opl;
	bool is2reg = 1, islabel = 1;
	int tarline, tarmem;
	long long n1, n2, n3;
	int len;
	void GetInfo(string to)
	{
		str = to;
		len = str.length();
		Rt = R1 = R2 = "xx";
		opl = 0;
	}
	int DoOp()
	{
		op.clear();
		opl = 0;
		for(int i = 0; i < len; i++)
		{
			if(str[i] == ' ')
				break;
			else
			{
				op += str[i];
				opl++;
			}
		}
		int dhcnt = 0;
		for(int i = opl; i < len; i++)
			if(str[i] == ',')
				dhcnt++;
		if(dhcnt == 1)
			if(op == "mul" || op == "mulu" || op == "div" || op == "divu")
			{
				op = "t" + op;
			}
		switch(op_type[op])
		{
			case 1 :
			{ 
				Rt = str.substr(opl + 2, 2);
				R1 = str.substr(opl + 7, 2);
				if(reg[num(R1)].occupied)
					return 0;
				n1 = reg[num(R1)].elem;
				if(str[opl + 11] == '$')
				{
					R2 = str.substr(opl + 12, 2);
					if(reg[num(R2)].occupied)
						return 0;
					n2 = reg[num(R2)].elem;
				}
				else
				{
					is2reg = 0;
					string anum;
					for(int i = opl + 11; i < len; i++)
						anum += str[i];
					n2 = stoi(anum);
				}
				reg[num(Rt)].occupied++;
				occ[0] = num(Rt);
				break;
			}
			case 2 :
			{
				Rt = str.substr(opl + 2, 2);
				if(str[opl + 6] == '$')
				{
					R2 = str.substr(opl + 7, 2);
					if(reg[num(R2)].occupied)
						return 0;
					n2 = reg[num(R2)].elem;
				}
				else
				{
					is2reg = 0;
					string anum;
					for(int i = opl + 6; i < len; i++)
						anum += str[i];
					n2 = stoi(anum);
				}
				if(op[0] == 't')
				{
					reg[32].occupied++;
					reg[33].occupied++;
					occ[0] = 32;
					occ[1] = 33;
				}
				else
				{
					reg[num(Rt)].occupied++;
					occ[0] = num(Rt);
				}
				break;
			}
			case 3 :
			{
				R1 = str.substr(opl + 2, 2);
				if(reg[num(R1)].occupied)
					return 0;
				n1 = reg[num(R1)].elem;
				if(str[opl + 6] == '$')
				{
					R2 = str.substr(opl + 7, 2);
					if(reg[num(R2)].occupied)
						return 0;
					n2 = reg[num(R2)].elem;
					lab = str.substr(opl + 11, len - opl - 11);
					tarline = t_label[lab];
				}
				else
				{
					is2reg = 0;
					string anum;
					int i;
					for(i = opl + 6; i < len; i++)
					{
						if(str[i] == ',')
							break;
						anum += str[i];
					}
					n2 = stoi(anum);
					lab = str.substr(i + 2, len - i - 2);
					tarline = t_label[lab];
				}
				break;
			}
			case 4 :
			{
				R1 = str.substr(opl + 2, 2);
				if(reg[num(R1)].occupied)
					return 0;
				n1 = reg[num(R1)].elem;
				lab = str.substr(opl + 6, len - opl - 6);
				tarline = t_label[lab];
				break;
			}
			case 5 :
			{
				R1 = str.substr(opl + 2, 2);
				if(reg[num(R1)].occupied)
					return 0;
				tarline = reg[num(R1)].elem;
				break;
			} 
			case 6 :
			{
				Rt = str.substr(opl + 2, 2);
				if(reg[num(Rt)].occupied)
					return 0;
				el = str.substr(opl + 6, len - opl - 6);
				int ll = el.length();
				if(el[ll - 1] == ')')
				{
					string tt = el.substr(ll - 3, 2);
					if(reg[num(tt)].occupied)
						return 0;	
				}
				reg[num(Rt)].occupied++;
				occ[0] = num(Rt);
				break;
			}
			case 8:
			{
				if(reg[2].occupied || reg[4].occupied || reg[5].occupied)
					return 0;
				n1 = reg[2].elem;
				n2 = reg[4].elem;
				n3 = reg[5].elem;
				if(n1 == 5 || n1 == 9)
					reg[2].occupied++;
				occ[0] = 2;
				break;
			}
			case 9:
			{
				R1 = str.substr(opl + 2, 2);
				if(reg[num(R1)].occupied)
					return 0;
				n1 = reg[num(R1)].elem;
				R2 = str.substr(opl + 7, 2);
				if(reg[num(R2)].occupied)
					return 0;
				n2 = reg[num(R2)].elem;
				reg[32].occupied++;
				reg[33].occupied++;
				occ[0] = 32;
				occ[1] = 33;
				break;
			}
			case 10:
			{
				R1 = str.substr(opl + 2, 2);
				if(reg[num(R1)].occupied)
					return 0;
				n1 = reg[num(R1)].elem;
				el = str.substr(opl + 6, len - opl - 6);
				int ll = el.length();
				if(el[ll - 1] == ')')
				{
					string tt = el.substr(ll - 3, 2);
					if(reg[num(tt)].occupied)
						return 0;	
				}
				break;
			}
			case 11:
			{
				if(op == "mfhi" && reg[33].occupied)
					return 0;
				if(op == "mflo" && reg[32].occupied)
					return 0;
				Rt = str.substr(opl + 2, 2);
				reg[num(Rt)].occupied++;
				occ[0] = num(Rt);
				break;
			}
			case 12:
			{
				lab = str.substr(opl + 1, len - opl - 1);
				tarline = t_label[lab];	
				break;
			}
		}
		return 1;
	}
}IDnDP;

class III
{
public:
	string op, el;
	int RtNum, R1Num, R2Num;
	long long n1, n2, n3, ans, anslo, anshi, anssys;
	int tarline, tarmem, nextline;
	bool jump;
	void GetInfo(string str, string Rt, string R1, string R2, string elel, long long in1, long long in2, long long in3, int hs, int nc)
	{
		el.clear();
		op.clear();
		op = str;
		RtNum = R1Num = R2Num = -1;
		int len = op.length();
		if(Rt != "xx")
		{
			RtNum = num(Rt);
		}
		if(R1 != "xx")
		{
			R1Num = num(R1);
		}
		if(R2 != "xx")
		{
			R2Num = num(R2);
		}
		n1 = in1;
		n2 = in2;
		n3 = in3;
		tarline = hs;
		tarmem = nc;
		el = elel;
		jump = 0;
		anssys = 0;
	}
	int DoOp()
	{
		if(op == "add")	{ans = n1 + n2;}
		else if(op == "addu")	{ans = n1 + n2;}
		else if(op == "addiu")	{ans = n1 + n2;}
		else if(op == "sub")	{ans = n1 - n2;}
		else if(op == "subu")	{ans = n1 - n2;}
		else if(op == "mul")	{ans = n1 * n2;}
		else if(op == "mulu")	{ans = n1 * n2;}
		else if(op == "tmul")	{anslo = LongWord(n1 * n2).w[0]; anshi = LongWord(n1 * n2).w[1];}
		else if(op == "tmulu")	{anslo = LongWord(n1 * n2).w[0]; anshi = LongWord(n1 * n2).w[1];}
		else if(op == "div")	{ans = n1 / n2;}
		else if(op == "divu")	{ans = n1 / n2;}
		else if(op == "tdiv")	{anslo = n1 / n2; anshi = n1 % n2;}
		else if(op == "tdivu")	{anslo = n1 / n2; anshi = n1 % n2;}
		else if(op == "xor")	{ans = n1 ^ n2;}
		else if(op == "xoru")	{ans = n1 ^ n2;}
		else if(op == "neg")	{ans = -n2;}
		else if(op == "negu")	{ans = -n2;}
		else if(op == "rem")	{ans = n1 % n2;}
		else if(op == "remu")	{ans = n1 % n2;}
		else if(op == "li")	{ans = n2;}
		else if(op == "seq")	{ans = n1 == n2;}
		else if(op == "sge")	{ans = n1 >= n2;}
		else if(op == "sgt")	{ans = n1 > n2;}
		else if(op == "sle")	{ans = n1 <= n2;}
		else if(op == "slt")	{ans = n1 < n2;}
		else if(op == "sne")	{ans = n1 != n2;}
		else if(op == "b")	{jump = 1;}
		else if(op == "beq")	{if(n1 == n2)	jump = 1;}
		else if(op == "bne")	{if(n1 != n2)	jump = 1;}
		else if(op == "bge")	{if(n1 >= n2)	jump = 1;}
		else if(op == "ble")	{if(n1 <= n2)	jump = 1;}
		else if(op == "bgt")	{if(n1 > n2)	jump = 1;}
		else if(op == "blt")	{if(n1 < n2)	jump = 1;}
		else if(op == "beqz")	{if(n1 == 0)	jump = 1;}
		else if(op == "bnez")	{if(n1 != 0)	jump = 1;}
		else if(op == "blez")	{if(n1 <= 0)	jump = 1;}
		else if(op == "bgez")	{if(n1 >= 0)	jump = 1;}
		else if(op == "bgtz")	{if(n1 > 0)	jump = 1;}
		else if(op == "bltz")	{if(n1 < 0)	jump = 1;}
		else if(op == "j")	{jump = 1;}
		else if(op == "jr")	{jump = 1;}
		else if(op == "jal")	{nextline = PC[3] + 1; jump = 1;}
		else if(op == "jalr")	{nextline = PC[3] + 1; jump = 1;}
		else if(op == "la" || op == "lb" || op == "lh" || op == "lw")
		{
			int len = el.length();
			if(d_label.count(el))
				tarmem = d_label[el];
			else if(t_label.count(el))
				tarline = t_label[el];
			else
			{
				string anum;
				int i;
				for(i = 0; i < len; i++)
				{
					if(el[i] == '(')
						break;
					anum += el[i];
				}
				R1Num = num(el.substr(i + 2, 2));
				if(R1Num != 31)
					tarmem = reg[R1Num].elem + stoi(anum);
				else
					tarline = reg[31].elem + stoi(anum);
			}
		}
		else if(op == "sb" || op == "sw" || op == "sh")
		{
			int len = el.length();
			if(d_label.count(el))
				tarmem = d_label[el];
			else
			{
				string anum;
				int i;
				for(i = 0; i < len; i++)
				{
					if(el[i] == '(')
						break;
					anum += el[i];
				}
				R2Num = num(el.substr(i + 2, 2));
				tarmem = reg[R2Num].elem + stoi(anum);
			}
		}
		else if(op == "move")	{ans = reg[R2Num].elem;}
		else if(op == "mfhi")	{ans = reg[33].elem;}
		else if(op == "mflo")	{ans = reg[32].elem;}
		else if(op == "nop")	{;}
		else if(op == "syscall")	
		{
			anssys = n1;
			switch(n1)
			{
				case 1:	cout << n2; break;
				case 4:	
				{
					//cerr << "MEM:\n";
					//for(int i = 0; i < PD; i++)
					//	cerr << mem[i] << endl;
					int now = (int)n2;
					while(mem[now] != 0)
						cout << mem[now++];
					break;
				}
				case 5:	cin >> ans; break;
				case 8: 
				{
					char x;
					int now = n2;
					bool flag = 0;
					for(int i = 1; i < n3; i++)
					{
						cin.get(x);
						if(x == '\n')
						{
							if(!flag)
								continue;
							else
								break;
						}
						else
						{
							flag = 1;
							mem[now++] = x;	
						}
					}
					mem[now++] = '\0';
					break;
				}
				case 9:
				{
					ans = PD;
					for(int i = 1; i <= n2; i++)	
						mem[PD++] = 0;
					break;
				}
				case 10:
					//for(int i = 0; i < PD; i++)
					//	cout << (int)mem[i] << endl;
					return 10;
				case 17:
					//for(int i = 0; i < PD; i++)
					//	cout << (int)mem[i] << endl;
					return 17;
			}
		}
		if(jump)
			return -500;
		return 1;
	}
}Ex;

class IV
{
public:
	string op;
	int RtNum, R1Num, R2Num;
	long long ans, anslo, anshi, anssys;
	int tarline, tarmem, nextline;
	int w;
	short h;
	char c;
	bool jump;
	int regtype;//0 t_address, 1 char, 2 short(-2 unsigned), 4 int(-4 unsigned), -1 d_address
	void GetInfo(string str, int Rtnum, int R1num, int R2num, long long a, long long al, long long ah, long long as, int tl, int tm, bool jp, int nl)
	{
		op = str;
		RtNum = Rtnum;
		R1Num = R1num;
		R2Num = R2num;
		ans = a;
		anslo = al;
		anshi = ah;
		anssys = as;
		jump = jp;
		tarline = tl;
		tarmem = tm;
		nextline = nl;
	}
	int DoOp()
	{
		if(op == "la")
		{
			if(RtNum == 31)	
			{
				w = tarline;
				regtype	= 4;
			}
			else
			{
				w = tarmem;
				regtype = 4;
			}
		}
		else if(op == "lb")
		{
			c = mem[tarmem];
			regtype = 1;	
		}
		else if(op == "lh")
		{
			h = HalfWord(tarmem, 'A').h;
			regtype = 2;	
		}
		else if(op == "lw")
		{
			w = Word(tarmem, 'A').w;
			regtype = 4;
		}
		else if(op == "sb")
		{
			mem[tarmem] = Word(reg[R1Num].elem).c[0];
		}
		else if(op == "sh")
		{
			mem[tarmem] = Word(reg[R1Num].elem).c[0];
			mem[tarmem + 1] = Word(reg[R1Num].elem).c[1];
		}
		else if(op == "sw")
		{
			for(int i = 0; i < 4; i++)
				mem[tarmem + i] = Word(reg[R1Num].elem).c[i];	
		}
		return 1;
	}
}MA;

class V
{
public:
	string op;
	int RtNum, R1Num, R2Num;
	long long ans, anslo, anshi, anssys;
	int tarline, tarmem, nextline;
	int w;
	short h;
	char c;
	int regtype;
	bool jump;
	void GetInfo(string str, int Rtnum, int R1num, int R2num, long long a, long long al, long long ah, long long as, int tl, int tm, int ww, short hh, char cc, bool jp, int regtt, int nl)
	{
		op = str;
		RtNum = Rtnum;
		R1Num = R1num;
		R2Num = R2num;
		ans = a;
		anslo = al;
		anshi = ah;
		anssys = as;
		w = ww;	h = hh;	c = cc;
		jump = jp;
		regtype = regtt;
		tarline = tl;
		tarmem = tm;
		nextline = nl;
	}
	int DoOp()
	{
		if(jump)
		{
			WR(31, nextline);
			return 1;	
		}
		else if(op == "add" || op == "sub" || op == "mul" || op == "div" || op == "xor" || op == "neg" || op == "rem" || op == "seq" || op == "sge" || op == "sgt" || op == "sle" || op == "slt" || op == "sne")
		{
			WR(RtNum, (int)ans);
			reg[RtNum].occupied--;
		}
		else if(op == "addu" || op == "addiu" || op == "subu" || op == "mulu" || op == "divu" || op == "xoru" || op == "negu" || op == "remu")	
		{
			WR(RtNum, (int)ans);
			reg[RtNum].occupied--;
		}
		else if(op == "tmul")	{WR(32, (int)anslo);	WR(33, (int)anshi); reg[32].occupied--; reg[33].occupied--;}
		else if(op == "tmulu")	{WR(32, (unsigned int)anslo);	WR(33, (unsigned int)anshi); reg[32].occupied--; reg[33].occupied--;}
		else if(op == "tdiv")	{WR(32, (int)anslo);	WR(33, (int)anshi); reg[32].occupied--; reg[33].occupied--;}
		else if(op == "tdivu")	{WR(32, (unsigned int)anslo);	WR(33, (unsigned int)anshi); reg[32].occupied--; reg[33].occupied--;}
		else if(op == "li")	{WR(RtNum, (int)ans); reg[RtNum].occupied--;}
		else if(op == "la")	{WR(RtNum, w); reg[RtNum].occupied--;}	//reg[RtNum].type = regtype;}
		else if(op == "lb")	{WR(RtNum, c); reg[RtNum].occupied--;}//	reg[RtNum].type = regtype;}
		else if(op == "lh")	{WR(RtNum, h); reg[RtNum].occupied--;}//	reg[RtNum].type = regtype;}
		else if(op == "lw")	{WR(RtNum, w); reg[RtNum].occupied--;}//	reg[RtNum].type = regtype;}
		else if(op == "sb")	{;}
		else if(op == "sh")	{;}
		else if(op == "sw")	{;}
		else if(op == "move")	{WR(RtNum, (int)ans); reg[RtNum].occupied--;}//	reg[RtNum].type = reg[R2Num].type;}
		else if(op == "mfhi")	{WR(RtNum, (int)ans); reg[RtNum].occupied--;}
		else if(op == "mflo")	{WR(RtNum, (int)ans); reg[RtNum].occupied--;}
		else if(op == "syscall")
			if(anssys == 5 || anssys == 9)
			{
				WR(2, (int)ans);
				reg[2].occupied--;
			}
		//PC[0]++;
		return 1;
	}
}WB;

int main(int argc, char *argv[])
{
	ifstream infile1(argv[1]);
	AddOpType(); 
	reg[29].elem = mem_size;
	//freopen("test.s", "r", stdin);
	//freopen("copy.s", "w", stdout);
	string input;
	while(!infile1.eof())
	{
		getline(infile1, input);
		offspace(input);
		if(input == ".data")
		{
			dot = 0;
			continue;	
		}
		if(input == ".text")
		{
			dot = 1;
			continue;	
		}
		int len = input.length();
		if(!dot)//data
		{
			if(input[len - 1] == ':')
			{
				input.pop_back();
				d_label[input] = PD;
			}
			else
			{
				if(input.substr(0, 7) == ".ascii ")
				{
					string ss = input.substr(8, len - 9);
					for(int i = 0; i < len - 9; i++)
					{
						if(ss[i] == '\\')
						{
							if(ss[i + 1] == 'n')
								mem[PD++] = '\n'; 
							else if(ss[i + 1] == 't')
								mem[PD++] = '\t';
							i++;
						}
						else
							mem[PD++] = ss[i];	
					}
				}
				else if(input.substr(0, 7) == ".asciiz")
				{
					string ss = input.substr(9, len - 10);
					//cerr << "string! " << ss << endl;
					for(int i = 0; i < len - 10; i++)
					{
						if(ss[i] == '\\')
						{
							//cerr << "HERE" << endl;
							if(ss[i + 1] == 'n')
								mem[PD++] = '\n'; 
							else if(ss[i + 1] == 't')
								mem[PD++] = '\t';
							i++;
							continue;
						}
						else
							mem[PD++] = ss[i];	
					}
					mem[PD++] = '\0';	
				}
				else if(input.substr(0, 6) == ".align")
				{
					int cnt = stoi(input.substr(7, len - 7));
					int two = 1;
					for(int i = 1; i <= cnt; i++)
						two *= 2;
					int k = ceil((double)PD / two);
					PD = k * two;
				}
				else if(input.substr(0, 5) == ".byte")
				{
					string anum;
					for(int i = 6; i < len; i++)
					{
						if(isdigit(input[i]))
							anum += input[i];
						else if(input[i] == ',')
						{
							char a = stoi(anum);
							mem[PD++] = a;
							anum.clear();	
						}
					}
					char a = stoi(anum);
					mem[PD++] = a;
				}
				else if(input.substr(0, 5) == ".half")
				{
					string anum;
					for(int i = 6; i < len; i++)
					{
						if(isdigit(input[i]))
							anum += input[i];
						else if(input[i] == ',')
						{
							short a = stoi(anum);
							mem[PD++] = HalfWord(a).c[0];
							mem[PD++] = HalfWord(a).c[1];
							anum.clear();	
						}
					}
					short a = stoi(anum);
					mem[PD++] = HalfWord(a).c[0];
					mem[PD++] = HalfWord(a).c[1];
				}
				else if(input.substr(0, 5) == ".word")
				{
					string anum;	
					for(int i = 6; i < len; i++)
					{
						if(isdigit(input[i]))
							anum += input[i];
						else if(input[i] == ',')
						{
							int a = stoi(anum);
							mem[PD++] = Word(a).c[0];
							mem[PD++] = Word(a).c[1];
							mem[PD++] = Word(a).c[2];
							mem[PD++] = Word(a).c[3];
							anum.clear();	
						}
					}
					int a = stoi(anum);
					mem[PD++] = Word(a).c[0];
					mem[PD++] = Word(a).c[1];
					mem[PD++] = Word(a).c[2];
					mem[PD++] = Word(a).c[3];
				}
				else if(input.substr(0, 6) == ".space")
				{
					int cnt = stoi(input.substr(7, len - 7));
					for(int i = 1; i <= cnt; i++)
						mem[PD++] = 0;	
				}
				
			}
		}
		else//text
		{
			if(input[len - 1] == ':')
			{
				input.pop_back();
				t_label[input] = text.size();
			}
			else if(input.empty())
				continue;
			else
			{
				int len = input.length();
				for(int i = 0; i < len - 1; i++)
				{
					if(input[i] == '$' && input[i + 1] == 'z')
					{
						input = input.substr(0, i + 3) + input.substr(i + 5, len - i - 5);
						len -= 2;
					}
					else if(input[i] == '$' && isdigit(input[i + 1]))
					{
						string anum;
						int j;
						for(j = i + 1; j < len; j++)
						{
							if(isdigit(input[j]))
								anum += input[j];
							else
								break;
						}
						if(stoi(anum) / 10 == 0)
							len++;
						input = input.substr(0, i + 1) + regname[stoi(anum)] + input.substr(j, len - j);
					}
				}
				text.push_back(input);
			}
		}
	}
	//for(int i = 0; i < text.size(); i++)
	//	outfile << i << "\t" << text[i] << endl;
	PC[0] = t_label["main"];
	PC[1] = PC[2] = PC[3] = PC[4] = PC[5] = -1;
	/*cout << "MEM:\n";
	for(int i = 0; i < PD; i++)
		cout << i << '\t' << (int)mem[i] << endl;
	cout << "---OVER\n";
	cout << mem[4] << endl;*/
	int status[6];// 0 未操作	1 已操作	2空线程 
	bool loaded[6];
	for(int i = 0; i <= 5; i++)
	{
		status[i] = 2;
		loaded[i] = 1;
	}
	const int text_size = text.size();
	while(true)
	{
		/*
		cout << "MEM:\n";
		for(int i = 0; i < PD; i++)
			cout << i << '\t' << (int)mem[i] << endl;
		cout << "---\nREG:\n";*/
		//for(int i = 0; i < 34; i++)
		//	cout << "reg " << i << " = " << reg[i].elem << endl;
		//cout << "---OVER\n";
		//cerr << PC[0] << endl;
		//system("pause");
		if(PC[4] != -1 && status[4] == 1 && status[5] >= 1)
		{
			WB.GetInfo(MA.op, MA.RtNum, MA.R1Num, MA.R2Num, MA.ans, MA.anslo, MA.anshi, MA.anssys, MA.tarline, MA.tarmem, MA.w, MA.h, MA.c, MA.jump, MA.regtype, MA.nextline);
			status[5] = 0;
			PC[5] = PC[4];
			loaded[4] = 1;
		}
		else if(status[5] >= 1)
		{
			PC[5] = -1;
			status[5] = 2;
		}
		else
			loaded[4] = 0;
		if(PC[3] != -1 && status[3] == 1 && status[4] >= 1 && loaded[4])
		{
			MA.GetInfo(Ex.op, Ex.RtNum, Ex.R1Num, Ex.R2Num, Ex.ans, Ex.anslo, Ex.anshi, Ex.anssys, Ex.tarline, Ex.tarmem, Ex.jump, Ex.nextline);
			status[4] = 0;
			PC[4] = PC[3];
			loaded[3] = 1;
			loaded[4] = 0;
		}
		else if(!loaded[4])
		{
			loaded[3] = 0;	
		}
		else if(status[4] >= 1)
		{
			PC[4] = -1;
			status[4] = 2;
			loaded[4] = 1;
		}
		if(PC[2] != -1 && status[2] == 1 && status[3] >= 1 && loaded[3])	
		{
			Ex.GetInfo(IDnDP.op, IDnDP.Rt, IDnDP.R1, IDnDP.R2, IDnDP.el, IDnDP.n1, IDnDP.n2, IDnDP.n3, IDnDP.tarline, IDnDP.tarmem);
			status[3] = 0;
			PC[3] = PC[2];
			loaded[2] = 1;
			loaded[3] = 0;
		}
		else if(!loaded[3])
			loaded[2] = 0;
		else if(status[3] >= 1)
		{
			PC[3] = -1;
			status[3] = 2;
			loaded[3] = 1;
		}
		if(PC[1] != -1 && status[1] == 1 && status[2] >= 1 && loaded[2])	
		{
			IDnDP.GetInfo(IF.str);
			status[2] = 0;
			PC[2] = PC[1];
			loaded[1] = 1;
			loaded[2] = 0;
			occ[0] = occ[1] = -1; 
		}
		else if(!loaded[2])
			loaded[1] = 0;
		else if(status[2] >= 1)
		{
			PC[2] = -1;
			status[2] = 2;
			loaded[2] = 1;
		}
		if(status[1] >= 1 && loaded[1] && PC[0] < text_size)
		{
			IF.GetInfo(PC[0]);
			status[1] = 0;
			PC[1] = PC[0];
			loaded[1] = 0;
			PC[0]++;
		}
		int tmp;
		if(status[1] == 1)
			true;
		else if(status[4] >= 1 || (op_type[MA.op] != 6 && op_type[MA.op] != 10))
		{
			IF.DoOp();
			status[1] = 1;
		}
		else
			status[1] = 0;
		if(status[2] == 0)
		{
			tmp = IDnDP.DoOp();
			status[2] = tmp == 1 ? 1 : 0;
		}
		if(status[3] == 0)
		{
			tmp = Ex.DoOp();
			switch(tmp)
			{
				case 0 : status[3] = 0; break;
				case 1 : status[3] = 1; break;
				case 10 : MA.DoOp(); WB.DoOp();	WB.GetInfo(MA.op, MA.RtNum, MA.R1Num, MA.R2Num, MA.ans, MA.anslo, MA.anshi, MA.anssys, MA.tarline, MA.tarmem, MA.w, MA.h, MA.c, MA.jump, MA.regtype, MA.nextline); WB.DoOp(); exit(0);
				case 17 : MA.DoOp(); WB.DoOp();	WB.GetInfo(MA.op, MA.RtNum, MA.R1Num, MA.R2Num, MA.ans, MA.anslo, MA.anshi, MA.anssys, MA.tarline, MA.tarmem, MA.w, MA.h, MA.c, MA.jump, MA.regtype, MA.nextline); WB.DoOp(); exit(Ex.n2);
				case -500 : 
					status[1] = status[2] = 2;
					loaded[1] = loaded[2] = 1;
					status[3] = 1;
					if(occ[0] != -1)
						reg[occ[0]].occupied--;
					if(occ[1] != -1)
						reg[occ[1]].occupied--;
					PC[1] = PC[2] = -1;
					PC[0] = Ex.tarline;
					break;
			}
		}
		if(status[4] == 0)
		{
			tmp = MA.DoOp();
			status[4] = tmp == 1 ? 1 : 0;
		}
		if(status[5] == 0)
		{
			tmp = WB.DoOp();
			status[5] = tmp == 1 ? 1 : 0;
		}
	}
	return 0;
}
