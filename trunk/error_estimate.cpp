#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<string.h>
#include<stdlib.h>
#include<map>
using namespace std;
typedef long long LL;
int M;
const int MAX_LINE_LENGTH=2000000;
int num[128];
struct entry{
	string *s;
	bool hit;
	entry(string& str):s(&str),hit(false){}
};
vector<vector<entry> > table;
void add(string& s){
	int h=0;
	for(int i=0;i<s.size();i++){
		h*=4;
		h+=num[s[i]];
		h%=M;
	}
	for(int j=0;j<table[h].size();j++)
		if(s==*(table[h][j].s))
			return;
	table[h].push_back(entry(s));
}


int exp_mod(int k,int n){
	if(n==0)return 1;
	LL t=exp_mod(k,n>>1);
	if(n & 1)
		return (t*t*(LL)k)%M;
	else
		return (t*t)%M;
}
vector<int> hash;

typedef map<string,int> Data;
Data read_data(FILE* f){
	Data reads;
	char *buf=new char[MAX_LINE_LENGTH];
	while(fgets(buf,MAX_LINE_LENGTH,f)){
		if(buf[0]=='#' || buf[0]=='>')
			continue;
		int len=strlen(buf);
		if(len==MAX_LINE_LENGTH){
			fprintf(stderr,"too long string in a line.");
			exit(-1);
		}
		buf[len-1]='\0';	// remove '\n' at end of line
		reads[string(buf)]++;
	}
	delete[] buf;
	return reads;
}
int main(int argc,char** args){
	if(argc!=3){
		cerr<<"two argument required:"<<endl
			<<"\tcorrect sequence file"<<endl
			<<"\treads file"<<endl;
		return -1;
	}
	num['A']=0;
	num['G']=1;
	num['C']=2;
	num['T']=3;
	FILE* f1=fopen(args[1],"r"),*f2=fopen(args[2],"r");
	if(!f1 || !f2){
		cerr<<"fail to open file\n"<<endl;
		return -1;
	}
	Data d1=read_data(f1),d2=read_data(f2);
	const string& seq=d1.begin()->first;
	vector<string> reads;
	for(Data::iterator it=d2.begin();it!=d2.end();it++){
		reads.push_back(it->first);
	}

	M=reads.size();
	table.resize(M);
	for(int i=0;i<M;i++) add(reads[i]);

	hash.resize(seq.size());
	hash[0]=num[seq[0]];
	for(int i=1;i<seq.size();i++){
		hash[i]=hash[i-1]*4+num[seq[i]];
		hash[i]%=M;
	}
	int h=hash[29];
	for(int i=0;i<table[h].size();i++)
		if(seq.substr(0,30) == *(table[h][i].s)){
			table[h][i].hit=true;
			break;
		}
	for(int i=1;i<=seq.size()-30;i++){
		h=(hash[i+29]-((LL)hash[i-1]*exp_mod(4,30))%M+M)%M;
		for(int j=0;j<table[h].size();j++){
			if(seq.substr(i,30) == *(table[h][j].s)){
				table[h][j].hit=true;
				break;
			}
		}
	}
	int ac=0,wa=0;
	for(int i=0;i<M;i++)
		for(int j=0;j<table[i].size();j++)
			if(table[i][j].hit)
				ac++;
			else
				wa++;
	printf("in:\t%d\n",ac);
	printf("not in:\t%d\n",wa);
	return 0;
}
