#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<map>
#include<set>
#include<list>
#include<assert.h>
#include<time.h>
using namespace std;
bool cmp(const char* s1,const char* s2){
	for(int i=0;s1[i] || s2[i];i++){
		if(s1[i] != s2[i])
			return s1[i]<s2[i];
	}
	return false;	//equal
}
bool r_cmp(const char *s1,const char *s2){
	for(int i=0;s1[i] || s2[i];i--){
		if(s1[i] != s2[i])
			return s1[i]<s2[i];
	}
	return false;	//equal
}
struct Read{
	const char* s;
	int tms,w;
	Read(const char* _s,int _t,int _w):s(_s),tms(_t),w(_w){}
};
typedef map<string,int> Data;
typedef Data::iterator Data_it;

FILE* OUTPUT_FILE=stdout;
const int MAX_LINE_LENGTH=20000;
char ID[128];
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

struct Prefix{	//	reverse prefix
	const char* s;
	Prefix(const char*_s):s(_s){}
	char operator[](int i){return s[-i];}
	inline bool operator<(const Prefix& x)const{
		return r_cmp(s,x.s);
	}
};
struct Suffix{
	const char *s;
	Suffix(const char* _s):s(_s){}
	char operator[](int i){return s[i];}
	inline bool operator<(const Suffix& x)const{
		return cmp(s,x.s);
	}
};
void puts_r(const char *s){
	for(int i=0;s[i];i--)
		putchar(s[i]);
	putchar(10);
}
int LCP(const vector<Prefix>& p_arr,const char* s){	// longest common prefix(reversely)
	int l=0,r=p_arr.size()-1,mid;
	int ll=0, rl=0;
	while(l<=r){
		mid=(l+r)>>1;
		const char* ts=p_arr[mid].s;
		int lb=min(ll,rl);
		while(s[-lb] && ts[-lb]==s[-lb])lb++;
		if(!s[-lb]){
			rl=lb;
			break;
		}
		if(ts[-lb]>s[-lb]){
			r=mid-1;
			rl=lb;
		}else{
			l=mid+1;
			ll=lb;
		}
	}
	int res=max(ll,rl);
	return res;
}
int LCS(const vector<Suffix>& s_arr,const char* s){	// longest common suffix
	int l=0,r=s_arr.size()-1,mid;
	int ll=0, rl=0;
	while(l<=r){
		mid=(l+r)>>1;
		const char* ts=s_arr[mid].s;
		int lb=min(ll,rl);
		while(s[lb] && ts[lb]==s[lb])lb++;
		if(!s[lb]){
			rl=lb;
			break;
		}
		if(ts[lb]>s[lb]){
			r=mid-1;
			rl=lb;
		}else{
			l=mid+1;
			ll=lb;
		}
	}
	int res=max(ll,rl);
	return res;
}
void suffix_counting_sort(vector<Suffix> &arr,int k){	// previous k characters are the same
	if(arr.size()<50 || k==30){
		sort(arr.begin(),arr.end());	
		return;
	}
	vector<Suffix> ct[5];
	for(int idx=0;idx<arr.size();idx++){
		int i=arr[idx][k] ? ID[arr[idx][k]]+1 : 0;
		ct[i].push_back(arr[idx]);
	}
	arr.clear();
	for(int i=1;i<5;i++)
		suffix_counting_sort(ct[i],k+1);
	for(int i=0;i<5;i++)
		for(int j=0;j<ct[i].size();j++)
			arr.push_back(ct[i][j]);
}
void prefix_counting_sort(vector<Prefix> &arr,int k){	// previous k characters are the same
	if(arr.size()<50 || k==30){
		sort(arr.begin(),arr.end());	
		return;
	}
	vector<Prefix> ct[5];
	for(int idx=0;idx<arr.size();idx++){
		int i=arr[idx][k] ? ID[arr[idx][k]]+1 : 0;
		ct[i].push_back(arr[idx]);
	}
	arr.clear();
	for(int i=1;i<5;i++)
		prefix_counting_sort(ct[i],k+1);
	for(int i=0;i<5;i++)
		for(int j=0;j<ct[i].size();j++)
			arr.push_back(ct[i][j]);
}
void build_suffix_array(vector<Suffix>& arr){
	suffix_counting_sort(arr,0);
}
void build_prefix_array(vector<Prefix>& arr){
	prefix_counting_sort(arr,0);
}
void print_reads(const vector<Read>& new_reads,FILE* f){
	int read_num=0;
	for(int i=0;i<new_reads.size();i++){
		int times=new_reads[i].tms;
		while(times--){
			fprintf(f,">read_%d\n%s\n",read_num++,new_reads[i].s);
		}
	}
}
int RANGE_ST=14,RANGE_ED=26,STEP_SIZE=2;	//default
int main(int argc,char** args){
	if(argc<2){
		cerr<<"Usage: ./error1 read_file_name"<<endl;
		cerr<<"\t[-s <RANGE_ST>(14 by default)]"<<endl;
		cerr<<"\t[-e <RANGE_ED>(27 by default)]"<<endl;
		cerr<<"\t[-d <STEP_SIZE>(2 by default)]"<<endl;
		cerr<<"\t[-o <output file name>]"<<endl;
		return -1;
	}
	for(int i=2;i<argc;i++){
		if(args[i][0]!='-')continue;
		switch(args[i][1]){
			case 's': case 'S': RANGE_ST=atoi(args[i+1]);i++;break;
			case 'e': case 'E': RANGE_ED=atoi(args[i+1]);i++;break;
			case 'd': case 'D': STEP_SIZE=atoi(args[i+1]);i++;break;
			case 'o': case 'O': OUTPUT_FILE=fopen(args[i+1],"w");
						if(!OUTPUT_FILE){
							fprintf(stderr,"cannot open file %s\n",args[i+1]);
							exit(-1);
						}
						break;
			default: fprintf(stderr,"unknown flag: %s\n",args[i]);return -1;
		}
	}
	ID['A']=0;
	ID['C']=1;
	ID['G']=2;
	ID['T']=3;
	FILE* f=fopen(args[1],"r");
	if(!f){
		fprintf(stderr,"fail to open file: %s\n",args[1]);
		return -1;
	}
	Data d=read_data(f);
	fclose(f);
	vector<Read> lst,new_reads;
	vector<Suffix> suffix_arr;
	vector<Prefix> prefix_arr;

	for(Data_it it=d.begin();it!=d.end();it++){
		char *s=new char[32];
		s[0]=0;
		s++;
		strcpy(s,it->first.c_str());
		lst.push_back(Read(s,it->second,strchr(s,'N')?0:100));
	}
	for(int MIN_OVERLAP_LEN=RANGE_ST;MIN_OVERLAP_LEN<=RANGE_ED;MIN_OVERLAP_LEN+=STEP_SIZE){
		cerr<<"MIN_OVERLAP_LEN = "<<MIN_OVERLAP_LEN<<endl;
		new_reads.clear();
		suffix_arr.clear();
		prefix_arr.clear();

		for(int i=0;i<lst.size();i++){
			if(lst[i].w <= MIN_OVERLAP_LEN * 2)continue;
			const char* s=lst[i].s;
			for(int j=MIN_OVERLAP_LEN;j<30;j++){
				prefix_arr.push_back(Prefix(s+j-1));	
				suffix_arr.push_back(Suffix(s+30-j));	//assume reads has length 30
			}
		}
		cerr<<"building suffix arr..."<<suffix_arr.size()<<endl;
		build_suffix_array(suffix_arr);
		//sort(suffix_arr.begin(),suffix_arr.end());
		cerr<<"building prefix arr..."<<prefix_arr.size()<<endl;
		build_prefix_array(prefix_arr);
		//sort(prefix_arr.begin(),prefix_arr.end());
		const char* cand="AGTC";

		char* s=new char[32];
		s++;
		cerr<<"fixing reads..."<<endl;
		for(int idx=0;idx<lst.size();idx++){
			strcpy(s,lst[idx].s);
			int times=lst[idx].tms;
			for(int i=0;i<30;i++)
				if(s[i]=='N')
					s[i]=cand[rand()%4];
			const int a=LCP(prefix_arr,s+29),b=LCS(suffix_arr,s);
			int ll= (a>=MIN_OVERLAP_LEN ? a:0) + (b>=MIN_OVERLAP_LEN ? b:0);
			char* best_s=new char[32];
			int best_ll=ll;
			best_s[0]=0;
			best_s++;
			strcpy(best_s,s);
			int ta,tb,tl;
			if(ll <= 2*MIN_OVERLAP_LEN){
				//	make one change
				for(int i=0;i<30;i++){
					char c=s[i];
					for(int k=0;k<4;k++){
						if(c==cand[k])continue;
						s[i]=cand[k];
						if(29-i > a){
							ta=a;
						}else{
							ta=LCP(prefix_arr,s+29);
						}
						if(i > b){
							tb=b;
						}else{
							tb=LCS(suffix_arr,s);
						}
						tl= (ta>=MIN_OVERLAP_LEN ? ta:0) + (tb>=MIN_OVERLAP_LEN ? tb:0);
						if(tl > best_ll){
							best_ll=tl;
							strcpy(best_s,s);
						}
					}
					s[i]=c;
				}
			}
			new_reads.push_back(Read(best_s,times,best_ll+times));
		}
		delete[] --s;
		for(int i=0;i<lst.size();i++)
			delete[] (char*)(lst[i].s-1);
		lst.clear();
		lst=new_reads;
	}
	print_reads(new_reads,OUTPUT_FILE);
	if(OUTPUT_FILE!=stdout){
		fclose(OUTPUT_FILE);	
	}
	return 0;
}


