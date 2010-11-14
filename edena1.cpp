#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<queue>
#include<algorithm>
#include<vector>
#include<string>
#include<map>
#include<set>
#include<list>
#include<ctime>
using namespace std;
typedef map<string,int> Data;
typedef Data::iterator Data_it;
typedef Data::reverse_iterator Data_rit;
typedef vector<const string*> STR_LST;
typedef	vector<pair<STR_LST*,int> > STR_LSTS;

size_t MIN_OVERLAP_SIZE=22,READ_LEN=30,MIN_CONTIG_SIZE=100,MIN_DEAD_END_LEN=10;	//default
int NON_STRICT_ENABLED=0;
FILE* OUTPUT_FILE=stdout;

const int MAX_LINE_LENGTH=20000;
char ID[128];
bool str_equal(const char* s1,const char* s2,size_t len){
	for(size_t i=0;i<len;i++)
		if(s1[i]!=s2[i])
			return false;
	return true;
}
int overlap(const string& s1,const string& s2){
	size_t l1=s1.size(),l2=s2.size();
	int n=min(l1,l2), res=0;
	const char *ss1=s1.c_str(),*ss2=s2.c_str();
	for(size_t len=n-1;len>0;len--)
		if(str_equal(ss1+l1-len,ss2,len)){
			res=len;
			break;
		}

	return res;
}
struct Trie{
	struct Node{
		Node* child[4];
		STR_LST lst;
		Node(){
			memset(child,0,sizeof(child));
		}
	}*root;
	Trie(){
		root=new Node();
	}
	void clear(Node* cur){
		if(!cur)return;
		for(size_t i=0;i<4;i++)	
			clear(cur->child[i]);
		delete cur;
	}
	~Trie(){
		clear(root);
	}
	void insert_reverse_prefix(const string &s,int n){
		Node* cur=root;
		int id;
		for(int i=n-1;i>=0;i--){
			id=ID[(size_t)s[i]];
			if(!cur->child[id])cur->child[id]=new Node();
			cur=cur->child[id];
		}
		cur->lst.push_back(&s);
	}
	STR_LSTS find_max_overlap(const string &s){
		Node* cur=root;
		STR_LSTS res;
		int id,len=0;
		for(size_t i=s.size()-1;i>=0;i--){
			id=ID[(size_t)s[i]];
			if(cur->lst.size()){
				res.push_back(make_pair(&(cur->lst),len));
			}
			cur=cur->child[id];
			len++;
			if(!cur)break;
		}
		return res;
	}
};

Data read_data(FILE* f){
	Data reads;
	char *buf=new char[MAX_LINE_LENGTH];
	int len;
	const char* cand="ATGC";
	while(fgets(buf,MAX_LINE_LENGTH,f)){
		if(buf[0]=='#' || buf[0]=='>')
			continue;
		for(size_t i=0;buf[i];i++)
			if(buf[i]=='N'){
				buf[i]=cand[rand()%4];
			}
		len=strlen(buf);
		if(len==MAX_LINE_LENGTH){
			fprintf(stderr,"too long string in a line.");
			exit(-1);
		}
		buf[len-1]='\0';	// remove '\n' at end of line
		reads[string(buf)]++;
	}
	READ_LEN=len;
	delete[] buf;
	return reads;
}
struct Prefix{	//	reverse prefix
	const char* s;
	size_t len;
	unsigned int h;
	Prefix(const char* _s,size_t _len):s(_s),len(_len){
		h=hash();	
	}
	unsigned int hash(){
		unsigned int res=0;
		for(size_t i=0;i<len;i++){
			res*=133;
			res+=s[i];
		}
		return res;
	}
	inline bool operator==(const Prefix& x)const{
		size_t l=min(len,x.len);
		for(size_t i=0;i<l;i++)
			if(s[i]!=x.s[i])
				return false;
		return len==x.len;
	}
};
/*
	T must override operator==
	and has field h, which is hash code
*/
template<class T>
struct HashSet{
	vector<T> *table;
	unsigned int table_size;
	HashSet(unsigned int size):table_size(size){
		table=new vector<T>[size];
	}	
	~HashSet(){
		delete[] table;	
	}
	bool contains(const T &p){
		unsigned int h=p.h%table_size;
		for(size_t i=0;i<table[h].size();i++)
			if(table[h][i]==p)
				return true;
		return false;
	}
	void add(const T &p){
		table[p.h%table_size].push_back(p);
	}
};
template<class T,class K>
struct HashMap{
	vector<pair<T,K> > *table;
	unsigned int table_size;
	HashMap(unsigned int size):table_size(size){
		table=new vector<pair<T,K> >[size];
	}	
	~HashMap(){
		delete[] table;	
	}
	void add(const T& key,const K& value){
		 table[key.h%table_size].push_back(pair<T,K>(key,value));
	}

	K& get(const T& key){
		unsigned int h=key.h%table_size;
		for(size_t i=0;i<table[h].size();i++)
			if(table[h][i].first==key)
				return table[h][i].second;
		cerr<<"Key  not in hash map"<<endl;
		exit(-1);
	}
};
struct Read{
	const string* ps;
	unsigned int h;
	Read(const string* _ps):ps(_ps){
		h=hash();	
	}
	unsigned int hash(){
		unsigned int res=0;
		const string& s=*ps;
		for(size_t i=0;i<s.size();i++){
			res*=133;
			res+=s[i];
		}
		return res;
	}
	bool operator==(const Read &r)const{
		return *ps==*r.ps;	
	}
};
struct CompressedGraph{
	struct Edge{
		string w;
		int u,v;	
		double trust;
		bool operator<(const Edge& e)const{
			return w.size() > e.w.size();	
		}
		Edge(int _u,int _v,const string& _w):
			u(_u),v(_v),w(_w){}
	};
	const string** id2r;
	list<Edge> *E;
	map<int,int> forward;
	int new_node_num;
	int *indegree;
	int N;
	CompressedGraph(int n,const string** _id2r):N(n),id2r(_id2r){
		E=new list<Edge>[N];	
		indegree=new int[N];
		memset(indegree,0,N*sizeof(int));
		new_node_num=0;
	}
	~CompressedGraph(){
		delete[] E;	
		delete[] indegree;
	}
	void add_edge(int u,int v,int ovl){
		E[u].push_back(Edge(u,v,id2r[v]->substr(ovl)));
		indegree[v]++;
	}
	void add_edge(int u,int v,const string& w){
		E[u].push_back(Edge(u,v,w));
		indegree[v]++;
	}
	int new_id(int i){
		if(forward.find(i)==forward.end())
			return forward[i]=new_node_num++;
		return forward[i];
	}
	list<Edge>::iterator remove_edge(list<Edge>::iterator e){
		indegree[e->v]--;
		return E[e->u].erase(e);
	}
	bool compress(){
		bool flag=false;
		for(int i=0;i<N;i++){
			if(E[i].size()==0 && indegree[i]==0)continue;
			for(list<Edge>::iterator it1=E[i].begin();it1!=E[i].end();){
				int v=it1->v;
				if(indegree[v]!=1 || E[v].size()!=1){
					it1++;
					continue;	//indeg or out deg larger than 1
				}
				list<Edge>::iterator it2=E[v].begin();
				if(it1==it2){
					it1++;
					continue;
				}
				add_edge(it1->u,it2->v,it1->w+it2->w);
				it1=remove_edge(it1);
				it2=remove_edge(it2);
				flag=true;
			}
		}	
		return flag;
	}
	bool clear_bubble(){
		size_t ms=4*READ_LEN-2*MIN_OVERLAP_SIZE-1;	//suggested in the paper
		for(int i=0;i<N;i++){
			if(E[i].size()==0 && indegree[i]==0)continue;
			E[i].sort();
			for(list<Edge>::iterator it1=E[i].begin();it1!=E[i].end();it1++){
				list<Edge>::iterator it2=it1;
				it2++;
				for(;it2!=E[i].end();){
					if(it1->v==it2->v && it2->w.size() < ms){
						it2=remove_edge(it2);	
					}else{
						it2++;	
					}
				}
			}
		}
		return compress();
	}
	bool clear_short_edges(){
		size_t ms=MIN_DEAD_END_LEN;
		for(int i=0;i<N;i++){
			for(list<Edge>::iterator e=E[i].begin();e!=E[i].end();)
				if(e->w.size() <= ms && 
					(indegree[e->u]==0 	// short incomming end
					|| E[e->v].size()==0)){ 	// short outgoing end
					e=remove_edge(e);
				}else{
					e++;
				}
		}
		return compress();
	}
	void get_contigs(vector<string> &contigs){
		for(int i=0;i<N;i++)	
			for(list<Edge>::iterator e=E[i].begin();e!=E[i].end();e++){
				string c=*(id2r[e->u])+e->w;
				if(c.size() >= MIN_CONTIG_SIZE)
					contigs.push_back(c);
			}
	}
	void print(const char* file_name){
		ofstream out(file_name);
		if(out.fail()){
			cerr<<"fail to output compressed graph"<<endl;
			return;
		}
		out<<"digraph G{"<<endl;
		for(int i=0;i<N;i++){
			if(E[i].size()==0)continue;
			for(list<Edge>::iterator it=E[i].begin();it!=E[i].end();it++){
				out<<new_id(i)<<" -> "<<new_id(it->v) << "[label=\""<<it->w.size()<<"\"];"<<endl;
			}
		}
		out<<"}"<<endl;
		out.close();
	}
};
struct OverlapGraph{
	struct Edge{
		int u,v,w;
		Edge(int _u,int _v,int _w):
			u(_u),v(_v),w(_w){}
		bool operator<(const Edge& e)const{
			return w > e.w;	//large overlap comes first	
		}
	};
	list<Edge>* E;
	int *indegree;
	const string** id2r;
	vector<int> node_lst;
	int N,M;
	void add_edge(int u,int v,int w){
		E[u].push_back(Edge(u,v,w));
		indegree[v]++;
		M++;
	}
	OverlapGraph(Data& data){
		HashMap<Read,int> r2id(data.size());
		id2r=new const string*[data.size()];
		int nodeID=0;
		N=data.size();
		M=0;
		for(Data_it it=data.begin();it!=data.end();it++){
			r2id.add(Read(&(it->first)),nodeID);
			id2r[nodeID]=&(it->first);
			nodeID++;
		}
		E=new list<Edge>[N];
		indegree=new int[N];
		memset(indegree,0,sizeof(int)*N);

		cerr<<"building prefix trie...";
		Trie prefix_trie;
		for(Data_it it=data.begin();it!=data.end();it++){
			const string& s=it->first;
			for(size_t l=MIN_OVERLAP_SIZE;l<s.size();l++){
				prefix_trie.insert_reverse_prefix(s,l);
			}
		}
		cerr<<"done"<<endl;
		cerr<<"building graph...";

		set<int> seen;	//	donot want the same edge u v appear twice

		for(Data_it it=data.begin();it!=data.end();it++){
			const string& s=it->first;
			int u,v,w;
			STR_LSTS ll=prefix_trie.find_max_overlap(s);
			u=r2id.get(Read(&s));
			seen.clear();
			for(size_t i=0;i<ll.size();i++){
				STR_LST &l=*ll[i].first;
				w=ll[i].second;
				for(size_t j=0;j<l.size();j++){
					v=r2id.get(Read(l[j]));
					if(u==v)continue;	//donot want self-loop
					if(seen.find(v)==seen.end()){
						add_edge(u,v,w);
						seen.insert(v);
					}
				}
			}
		}
		cerr<<"done"<<endl;

		cerr<<"start removing transitive edges...";
		remove_transitive_edges();
		cerr<<"done"<<endl;
		if(NON_STRICT_ENABLED==1){
			cerr<<"start non-strict clearing...";
			non_strict_clear();
			cerr<<"done"<<endl;
		}else if(NON_STRICT_ENABLED==2){
			cerr<<"start very non-strict clearing, good luck...";
			very_non_strict_clear();
			cerr<<"done"<<endl;
		}
	}
	void very_non_strict_clear(){
		vector<list<Edge>::iterator> *RE=new vector<list<Edge>::iterator>[N];
		for(int i=0;i<N;i++)
			for(list<Edge>::iterator e=E[i].begin();e!=E[i].end();e++)
				RE[e->v].push_back(e);
		for(int i=0;i<N;i++){
			if(RE[i].size()>2){
				int max_w=0;
				for(int j=0;j<RE[i].size();j++)
					max_w=max(max_w,RE[i][j]->w);
				for(int j=0;j<RE[i].size();j++)
					if(RE[i][j]->w < max_w)
						remove_edge(RE[i][j]);
			}
		}
		delete[] RE;

		for(int i=0;i<N;i++){
			if(E[i].size()<2)continue;
			int max_w=0;
			for(list<Edge>::iterator e=E[i].begin();e!=E[i].end();e++)
				max_w=max(max_w,e->w);
			for(list<Edge>::iterator e=E[i].begin();e!=E[i].end();){
				if(e->w < max_w)
					e=remove_edge(e);
				else
					e++;
			}
		}
	}
	void non_strict_clear(){
		int ms=min(MIN_OVERLAP_SIZE+4,(size_t)28);
		vector<list<Edge>::iterator> *RE=new vector<list<Edge>::iterator>[N];
		for(int i=0;i<N;i++)
			for(list<Edge>::iterator e=E[i].begin();e!=E[i].end();e++)
				RE[e->v].push_back(e);
		for(int i=0;i<N;i++){
			if(RE[i].size()>2){
				for(int j=0;j<RE[i].size();j++)
					if(RE[i][j]->w < ms)
						remove_edge(RE[i][j]);
			}
		}
		delete[] RE;

		for(int i=0;i<N;i++){
			if(E[i].size()<2)continue;
			list<Edge>::iterator e=E[i].begin();
			for(;e!=E[i].end();)
				if(e->w <= ms)
					e=remove_edge(e);
				else
					e++;
		}
	}
	~OverlapGraph(){
		delete[] id2r;
		delete[] indegree;
		for(int u=0;u<N;u++)
			E[u].clear();
		delete[] E;
	}
	list<Edge>::iterator remove_edge(list<Edge>::iterator e){
		indegree[e->v]--;
		M--;
		return E[e->u].erase(e);
	}
	bool prefix_of(const char* s1,const char* s2){
		for(size_t i=0;s1[i];i++)
			if(s1[i]!=s2[i])
				return false;
		return true;
	}
	void remove_transitive_edges(){
		for(int u=0;u<N;u++){
			E[u].sort();
			list<Edge>::iterator e1,e2;
			for(e1=E[u].begin();e1!=E[u].end();e1++){
				e2=e1;
				e2++;
				for(;e2!=E[u].end();){
					const char* s1=id2r[e1->v]->c_str()+e1->w;	
					const char* s2=id2r[e2->v]->c_str()+e2->w;	
					if(prefix_of(s1,s2)){
						e2=remove_edge(e2);
					}else{
						e2++;	
					}
				}
			}
		}
	}
	string path2contig(const vector<Edge*> &p){
		if(p.size()==0)return "";
		string res=*id2r[p[0]->u];
		for(size_t i=0;i<p.size();i++){
			Edge* e=p[i];
			res+=id2r[e->v]->substr(e->w);
		}
		return res;
	}
	void produce_contigs(){
		cerr<<"building compressed graph...";
		CompressedGraph cg=buildCompressedGraph();
		cerr<<"done"<<endl;
		cerr<<"compressing graph...";
		cg.compress();
		cerr<<"done"<<endl;

		cg.print("g0.dot");

		cerr<<"clearing short ends...";
		cg.clear_short_edges();
		cerr<<"done"<<endl;

		cg.print("g1.dot");

		cerr<<"clearing bubbles...";
		cg.clear_bubble();
		cerr<<"done"<<endl;

		cg.print("g2.dot");

		cerr<<"producing contigs...";
		vector<string> contigs;
		cg.get_contigs(contigs);
		cerr<<"done"<<endl;
		for(size_t i=0;i<contigs.size();i++){
			fprintf(OUTPUT_FILE,">contig_%d\n",i+1);
			fprintf(OUTPUT_FILE,"%s\n",contigs[i].c_str());
		}
	}
	CompressedGraph buildCompressedGraph(){
		CompressedGraph res(N,id2r);
		for(int i=0;i<N;i++)
			for(list<Edge>::iterator e=E[i].begin();e!=E[i].end();e++)
				res.add_edge(e->u,e->v,e->w);
		return res;
	}
};

void print_status(const char* prefix){
	char cmd[100];
	sprintf(cmd,"cat /proc/%d/status | grep Vm > %s.stat",getpid(),prefix);
	system(cmd);
}
int main(int argc,char** args){
	if(argc<2){
		cerr<<"Usage:"<<endl;
		cerr<<"./edena1 <file name> [-m <MIN_OVERLAP_SIZE>(22 by default)]"<<endl;
		cerr<<"\t[-c <MIN_CONTIG_SIZE>(100 by default)]"<<endl;
		cerr<<"\t[-d <MIN_DEAD_END_LEN>(10 by default)]"<<endl;
		cerr<<"\t[-n (enable non-strict node)]"<<endl;
		cerr<<"\t[-N (enable very-non-strict node)]"<<endl;
		cerr<<"\t[-o <output file name>]"<<endl;
		return -1;
	}
	for(int i=2;i<argc;i++){
		if(args[i][0]!='-'){
			fprintf(stderr,"unknown flag: %s\n",args[i]);
			exit(-1);
		}
		switch(args[i][1]){
			case 'q':
				freopen("dev/null","w",stderr);
				i++;
				break;
			case 'm':
				MIN_OVERLAP_SIZE=atoi(args[i+1]);
				i++;
				break;
			case 'c':
				MIN_CONTIG_SIZE=atoi(args[i+1]);
				i++;
				break;
			case 'd':
				MIN_DEAD_END_LEN=atoi(args[i+1]);
				i++;
				break;
			case 'n':
				NON_STRICT_ENABLED=1;
				break;
			case 'N':
				NON_STRICT_ENABLED=2;
				break;
			case 'o':
				OUTPUT_FILE=fopen(args[i+1],"w");
				i++;
				break;
			default:
				fprintf(stderr,"unknown flag: %s\n",args[i]);
				exit(-1);
		}
	}
	FILE* f=fopen(args[1],"r");
	if(!f){
		fprintf(stderr,"fail to open file\n");
		return -1;
	}
	ID['A']=0;
	ID['T']=1;
	ID['C']=2;
	ID['G']=3;
	cerr<<"Reading in data...";
	Data d=read_data(f);
	fclose(f);
	cerr<<"done"<<endl;
	OverlapGraph g(d);
	g.produce_contigs();

	//print_status(args[1]);

	if(OUTPUT_FILE!=stdout)fclose(OUTPUT_FILE);
	return 0;
}


