///////////////////////////////// HSN to LSN //////////////////////////////////////////////////////
//  Compiler and linker of Sleptsov net program.                                                 //
//  To complete the conversion from high-level Sleptsov net (HSN) to low-level Sleptsov net (LSN)//
//  @ 2023 Hongfei Zhao										 //
///////////////////////////////////////////////////////////////////////////////////////////////////

// Command line:  HSNtoLSN hsn_file.txt result_lsn_file.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#define MAXSTRLEN 255

#define SKIP_COMM do{fgets(input_buffer,MAXSTRLEN,f);} while(input_buffer[0]==';') ;//skip comments
#define MAX_NUM 7000
/**
程序使用说明：用户可通过输入“-h”获取程序使用说明，按照程序用法及输入/输出的格式要求，正确使用程序

Program usage instructions: Users can obtain program usage instructions by entering "-h",
and use the program correctly according to the program usage and input/output format requirements
**/
static char Help[] =
    "Sleptsov net compiler and linker\n"
    "action: read high-level sleptsov net and create low-level sleptsov net\n"
    "usage: 		 Program_hsn_to_lsn [-h]\n"
    "                       		    [hsn_file.hsn] [lsn_file.lsn]\n"
    "hsn_file.hsn						.txt file of high-level sleptsov net\n"
    "lsn_file.lsn						.txt file of result low-level sleptsov net\n"
    ;

double magma_wtime( void ) {
	struct timeval t;
	gettimeofday( &t, NULL );
	return t.tv_sec + t.tv_usec*1e-6;
}
/**
弧的结构体：本结构体用于存储一条弧的类型和指向信息。
库所指向变迁的常规弧：p>0,t>0,w>0
变迁指向库所的常规弧：p<0,t>0,w>0
抑制弧： 			  p>0,t>0,w<0
t1指向t2的优先弧：	  t1<0,t2<0

Arc structure: This structure is used to store the type and pointing information of an arc.
A regular arc where a place points to a transition: p>0, t>0, w>0
A regular arc where a transition points to a place: p<0,t>0,w>0
Inhibitor arc: p>0,t>0,w<0
**/
struct arc { //structure of arcs
	int p;   // place number
	int t;   //transition number
	int w;   //arc weight
};

/**
简单或低级Sleptsov net的结构体：本结构体用于存储一个LSN的结构信息，包括库所的数量：m，变迁的数量：n
弧的数量：k，每个库所初始的托肯数量：mu，以及用于存储此网络的弧的信息的结构体指针：struct arc *a;

 Plain or low-level Sleptsov net structure: This structure is used to store the structural
 information of a LSN, including the number of places: m, the number of transitions: n,
 the number of arcs: k, the initial number of tokens for each place : mu, and a struct pointer
 to store information about the arcs of this network: struct arc *a;
**/
struct lsn { //structure of low-level sleptsov net
	int m;  //number of places
	int n;  //number of transitions
	int k;	//number of arcs
	int l;  //number of nonzero markings
	int *mu; //marking
	struct arc *a; //arcs
};

/**
库所映射的结构体：本结构体用于存储高级Sleptsov net的库所：hp与低级Sleptsov net的库所：lp之间的映射关系，
并且通过hp与lp符号的组合表示库所的不同用途：
数据输入库所：hp>0,lp>0,
数据输出库所：hp>0,lp<0，
控制流开始库所：hp<0,lp>0,
控制流结束库所：hp<0,lp<0.

Place mapping structure: This structure is used to store the mapping relationship between
the high-level Sleptsov net's place: hp and the low-level Sleptsov net's place: lp,
And the different uses of place are represented by the combination of hp and lp symbols:
Data input place: hp>0,lp>0,
Data output place: hp>0, lp<0,
Control flow start place: hp<0,lp>0,
Control flow finish place: hp<0,lp<0.
**/
struct pm { //structure of places mapping
	int hp; //high-level places
	int lp; //low-level places
};

/**
变迁替代和库所映射结构体：本结构体用于存储变迁替代的信息，包括被替代变迁的编号：tnum，库所映射的数量：pmnum,
需要替代的低级网络的名称（文件名）：name[],以及用于存储库所映射信息的结构体指针struct pm *pm1.

Transition substitution and place mapping structure: This structure is used to store the information of
transition substitution, including the number of the substituted transition: tnum,
the number of places mapping: pmnum, the name (file name) of the low-level network: name[],
and the struct pointer struct pm *pm1.
**/
struct tspm { //structure of transitions subsititution and places mapping
	int tnum; //number of subsitituted transition
	int pmnum; //number of places mapping
	char name[50]; //name of subnet file
	struct pm *pm1; //pointer to places mapping
};

/**
高级sleptsov net的结构体：本结构体用于存储一个HSN的结构信息，包括低级网络部分：struct lsn *l,
变迁替代的总数量：nst， 变迁替代和库所映射部分：struct tspm *t

Structure of high-level sleptsov net: This structure is used to store the structure information of an HSN,
including the low-level network part: struct lsn *l, Total number of transition substitutions: nst,
transitions substituiton and places mapping part: struct tspm *t
**/
struct hsn { //structure of high-level sleptsov net
	struct lsn *l; //low-level part of hsn
	int nst; //total number of subsititution transition
	struct tspm *t; //pointer to transitions subsititution and places mapping
};

/**
网络列表的结构体：本结构体用于存储一个HSN转换为LSN过程中，需要用到的所有LSN的信息，
包括:每一个LSN对应的编号：file_n, 网络名称：filename[][], 以及存储每个LSN的结构体：struct lsn *l_nt.

The structure of the network list: This structure is used to store all the LSN information that needs
to be used in the process of converting an HSN to an LSN.
Including: the number corresponding to each LSN: file_n, the network name: filename[][],
and the structure storing each LSN: struct lsn *l_nt.
**/
struct net_table { //structure of net table
	int file_n; //number of file
	char filename[7000][20]; //record file name
	struct lsn *l_nt;
};

/**
库所编号的结构体：本结构体用于存储HSN向LSN转换过程中所有库所的编号，包括：转换前的库所编号：pre_p,和转换
后重新编排的序号：new_p.

The structure of the place number: This structure is used to store the number of all places in the
process of HSN to LSN conversion, including: the place number before conversion: pre_p,
and the sequence number rearranged after conversion: new_p.
**/
struct place_num { //structure of places number
	int pre_p; //previous place number
	int new_p; //new place number
};

/**
变迁编号的结构体：本结构体用于存储HSN向LSn转换过程中所有变迁的编号，包括：转换前的变迁编号：pre_t,和转换
后重新编排的序号：new_t.

The structure of transition number: This structure is used to store the number of all transitions in
the process of HSN to LSn conversion, including: the transition number before the conversion: pre_t,
and the sequence number rearranged after the conversion: new_t.
**/
struct transition_num { //structure of transitions number
	int pre_t; //previous transition number
	int new_t; //new transition number
};

/**
网络转换过程中存储信息的结构体：本结构体用于存储HSN向LSN转换时的网络信息，包括：库所编号:struct place_num *pn,
变迁编号:struct transition_num *tn,初始托肯数量:*mark,弧的类型和指向关系:struct arc *new_a，
以及转换完成后的LSN:struct lsn *l_result.

Structure for storing information during network conversion: This structure is used to store network
information during HSN to LSN conversion, including: place number: struct place_num *pn,
Transition number: struct transition_num *tn, initial token number: *mark,
arc type and pointing relationship: struct arc *new_a,
And the converted LSN:struct lsn *l_result.
**/
struct mapping_result { //structure of mapping
	int *mark; //result marking
	struct place_num *pn;
	struct transition_num *tn;
	struct arc *new_a; //result arcs
	struct lsn *l_result; //record result lsn
};

/**
创建网络列表函数：根据读入的HSN文件，链接所需要的全部子网模块，包括COPY，CLEAN_MOVE和替代相应变迁的
子网模块，并为每个子网编号，同时将每个子网模块存入一个结构体变量中。

Create a network table function: According to the read HSN file, link all the required subnet modules,
including COPY, CLEAN_MOVE and the subnet modules that substitute the corresponding transitions,
and number each subnet, and store each subnet module in a structure variable.
**/
//create a table of nets and read nets into structure
int create_net_table(struct hsn *h,struct net_table *nt);

/**
编译最终的LSN函数：根据网络列表的存储信息，按照数据传输的方向和控制流的方向，将所用到的子网模块重新编排，
包括：库所的融合与托肯数的赋值，库所与变迁的重新编号，弧的连接，以及增添新的变迁用于分裂与合并控制流，
最终得到LSN，可运行于SN的处理器上。

Compile the final LSN function: Use the storage information of the network list to rearrange the used
subnet modules according to the direction of data transmission and the direction of control flow,
Including: merging of places and assignment of token numbers, renumbering of places and transitions,
connection of arcs, and adding new transitions for splitting and merging control flow,
and finally obtaining LSN, which can run on the processor of SN .
**/
//create result lsn,include places,marking transitions and arcs
int create_mapping_result(struct mapping_result *mt,struct net_table *nt,struct hsn *h);

/**
读HSN信息函数：包括：读LSN部分和变迁替代部分

Read HSN information function: including: read LSN part and transition substitution part
**/
int read_hsn(struct hsn *h,FILE *f); //read a hsn file

/**
写HSN信息函数：包括：写LSN部分和变迁替代部分

Write HSN information function: including: write LSN part and transition substitution part
**/
int write_hsn(struct hsn *h,FILE *f); //write hsn into a file

/**
读取LSN文件函数：利用文件指针获取文件第一行的m，n，k,l,nst数值，存入结构体变量l和h中。根据place数量m为mu分配
内存空间，根据弧的数量k为弧的结构体变量分配内存空间，逐行读取弧的信息存入结构体变量l中。再为mu的每个元素赋值 

Read LSN file function: use the file pointer to obtain the m, n, k，l,nst values of the first line of
the file, and store them in the structure variable l and h. Allocate mu according to the number of places m
Memory space, 
Allocate memory space for arc structure variables according to the number of arcs k,
The information of the arc read line by line is stored in the structure variable l.
**/
int read_lsn(struct lsn * l,int *nst, FILE *f); //read a lsn file

/**
写LSN文件函数：根据LSN文件要求的格式，利用文件指针从结构体变量中获取m，n，k数值，并写出至文件中。
根据place数量m，逐个读取结构体变量中的mu值并写出至文件中。根据弧的数量k，逐个读取结构体变量中弧的信息，
并写出至文件中。

Write LSN file function: According to the format required by the LSN file, use the file pointer to
obtain m, n, k values from the structure variables, and write them out to the file.
According to the number of places m, read the mu value in the structure variable one by one and write
it out to the file. According to the number of arcs k, read the information of the arcs in the
structure variable one by one, and write them out to the file.
**/
int write_lsn(struct lsn * l, FILE *f); //write lsn into a file

/**
读变迁替代部分的函数：根据HSN文件的格式要求，利用文件指针获取变迁替代的总数量：nst，
根据nst的数值，利用循环，读取被替代变迁的编号：tnum，库所映射的数量：pmnum和子网模块：name。
根据pmnum的值，利用循环，读取每对库所映射中HSN的库所编号和LSN的库所编号。
将上述信息均存入结构体变量中。

The function of reading the transition substitution part: According to the format requirements
of the HSN file, use the file pointer to obtain the total number of transition substitutions: nst,
according to the value of nst, use a loop to read the number of the substituted transition: tnum,
the number of place mappings: pmnum ,and subnet modules: name. According to the value of pmnum,
use a loop to read the place number of the HSN and the place number of the LSN in each pair of place
mappings. The above information is stored in the structure variable.
**/
int read_ts(struct tspm * tm,FILE *f); //read transitions subsititution of hsn

/**
写变迁替代部分的函数：根据HSN文件的格式要求，利用文件指针读取结构体变量中的变迁替代的总数量：nst，
根据nst的数值，利用循环，读取被替代变迁的编号：tnum，库所映射的数量：pmnum和子网模块：name。
根据pmnum的值，利用循环，读取每对库所映射中HSN的库所编号和LSN的库所编号。将上述信息逐一写入文件中。

The function of writing the transition substitution part: according to the format requirements of
the HSN file, use the file pointer to read the total number of transition substitutions "nst" in the
structure variable，
According to the value of nst, use a loop to read the number of the substituted transition: tnum,
the number mapped by the library: pmnum and the subnet module: name.
According to the value of pmnum, use a loop to read the place number of the HSN and the place number
of the LSN in each pair of place mappings. Write the above information into the file one by one.
**/
int write_ts(struct tspm * tm,FILE *f); //write transitions subsititution of hsn

//子网的库所映射函数
void sub_pmapping(int i, int a, int *start_p, int *p_total, struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                  int *add_io, int *addi, int *add_cf,int *add_cf_i, int *a_p_total);
//子网的变迁映射函数
void sub_tmapping(int i, int a, int *start_t, int *t_total, struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                  int *a_t_total);
//复制模块库所映射函数
void copy_pmapping(int i,int a,int *start_p,int *p_total,struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                   int *split,int *sp_in,int *joint,int *jo_in,int *a_p_total,int p_n,int *add_io,int *addo);
//复制模块变迁映射函数
void copy_tmapping(int a,int *start_t,int *t_total,struct mapping_result *mt,struct net_table *nt,int *a_t_total);
//clean_move模块库所映射函数
void cm_pmapping(int i,int a,int *start_p,int *p_total,struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                 int *add_io, int *addo,int *a_p_total, int p_n,int *add_cf, int *add_cf_o,int *cf_num,int *cf_n_in,int *c_m_cf,int *cf_in);
//clean_move模块变迁映射函数
void cm_tmapping(int a,int *start_t,int *t_total,struct mapping_result *mt,struct net_table *nt,int *a_t_total);

int main (int argc,char *argv[]) {

	double time1,time2;
	struct hsn h;
	struct net_table nt;
	struct mapping_result mt;

	if( strcmp( argv[1], "-h" )==0 ) {
		printf( "%s", Help );
		return 0;
	}

	FILE *f1,*f2; //f1: read hsn file; f2: write result lsn file.
	f1 = fopen(argv[1], "r" );
	time1=magma_wtime();
	if(f1==NULL) {
		printf( "Cannot open hsn file\n" );
		return -1;
	}
	read_hsn(&h, f1);
	fclose (f1);
	create_net_table(&h,&nt);
	
	create_mapping_result(&mt,&nt,&h);

	f2 = fopen(argv[2], "w" );
	if(f2==NULL) {
		printf( "Cannot open result file\n" );
		return -1;
	}
	write_lsn(mt.l_result, f2);
	fclose (f2);
	time2=magma_wtime();
	printf("####Result lsn has been created!####\n ");
	printf("####The total time is %#fs\n", time2-time1 );
	return 0;
}

//create a table of nets and read nets into structure
int create_net_table(struct hsn *h,struct net_table *nt) {
	int i,j;
	nt->file_n=0;

	strcpy(nt->filename[(nt->file_n)++],"lsn of hsn"); //record lsn of hsn

	for(i=0; i<h->nst; i++) { //record each net name and number
		strcpy(nt->filename[(nt->file_n)++],h->t[i].name);
		for(j=0; j<h->t[i].pmnum; j++) {
			if((h->t[i].pm1[j].hp)>0&&(h->t[i].pm1[j].lp)>0) { //input places :use copy subnet
				strcpy(nt->filename[(nt->file_n)++],"copy_lsn.txt");
			} else if((h->t[i].pm1[j].hp)>0&&(h->t[i].pm1[j].lp)<0) {
				strcpy(nt->filename[(nt->file_n)++],"clean_move_lsn.txt"); //output places :use clean_move subnet
			} else { //control flow
				continue;
			}
		}
	}
	/******read lsn into structure*******/
	nt->l_nt=malloc((nt->file_n)*sizeof(struct lsn));

	FILE *f3; //将所需lsn读入内存 
	int temp;
	for(i=1; i<nt->file_n; i++) {
		f3 = fopen(nt->filename[i], "r" );
		if(f3==NULL) {
			printf( "Cannot open %s file\n" ,nt->filename[i]);
			return -1;
		}
		read_lsn(&(nt->l_nt[i-1]),&temp,f3);
		fclose(f3);
	}
	/*************************************/

	return 0;
}

//子网库所映射函数
void sub_pmapping(int i, int a, int *start_p, int *p_total, struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                  int *add_io, int *addi, int *add_cf,int *add_cf_i, int *a_p_total) {
	int j,b,c;

	for(j=0; j<nt->l_nt[a].m; j++) {
		mt->pn[*p_total].pre_p=j+1; //number of previous places
		for(c=0; c<h->t[i].pmnum; c++) {//To record array index of each subsititution net places
			if((h->t[i].pm1[c].hp)>0&&(h->t[i].pm1[c].lp)>0) { //input places
				if(mt->pn[*p_total].pre_p==h->t[i].pm1[c].lp) {
					add_io[(*addi)++]=*p_total;
				}
			} else if((h->t[i].pm1[c].hp)>0&&(h->t[i].pm1[c].lp)<0) { //output places
				if(mt->pn[*p_total].pre_p==abs(h->t[i].pm1[c].lp)) {
					add_io[(*addi)++]=*p_total;
				}
			} else if((h->t[i].pm1[c].hp)<0&&(h->t[i].pm1[c].lp)>0) { //in-control flow
				if(mt->pn[*p_total].pre_p==h->t[i].pm1[c].lp) {
					add_cf[(*add_cf_i)++]=*p_total;
				}
			} else { //out-control flow
				if(mt->pn[*p_total].pre_p==abs(h->t[i].pm1[c].lp)) {
					add_cf[(*add_cf_i)++]=*p_total;
				}
			}
		}
		mt->pn[*p_total].new_p=*start_p; //number of result places
		mt->mark[mt->pn[*p_total].new_p-1]=nt->l_nt[a].mu[j];//result marking
		for(b=0; b<nt->l_nt[a].k; b++) { //result arcs
			if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p && nt->l_nt[a].a[b].w!=0) { //p of arc == pre_p, let p of arc =new_p,不含优先弧
				if(nt->l_nt[a].a[b].p>0) {
					mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
				} else {
					mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
				}
				mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;
			}
		}
		(*start_p)++;
		(*p_total)++;
	}
	*a_p_total+=nt->l_nt[a].k;

}
//子网变迁映射函数
void sub_tmapping(int i, int a, int *start_t, int *t_total, struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                  int *a_t_total) {
	int j,b;
	for(j=0; j<nt->l_nt[a].n; j++) {
		mt->tn[*t_total].pre_t=j+1; //number of previous transitons
		if(mt->tn[*t_total].pre_t==1) { //use hsn transition number
			mt->tn[*t_total].new_t=h->t[i].tnum; //number of result transitions
		} else {
			mt->tn[*t_total].new_t=*start_t; //number of result transition
			(*start_t)++;
		}
		for(b=0; b<nt->l_nt[a].k; b++) { //result arcs
			//弧中变迁编号为正数
			if(nt->l_nt[a].a[b].t==mt->tn[*t_total].pre_t) { //p of arc == pre_p, let p of arc =new_p
				mt->new_a[*a_t_total+b].t=mt->tn[*t_total].new_t;
			} else if(abs(nt->l_nt[a].a[b].t)==mt->tn[*t_total].pre_t) { //优先弧第二位
				mt->new_a[*a_t_total+b].t=-(mt->tn[*t_total].new_t);
				mt->new_a[*a_t_total+b].w=nt->l_nt[a].a[b].w; //优先弧两位，任选一次赋值即可
			} else if(abs(nt->l_nt[a].a[b].p)==mt->tn[*t_total].pre_t && nt->l_nt[a].a[b].w==0) { //优先弧第一位
				mt->new_a[*a_t_total+b].p=-(mt->tn[*t_total].new_t);
			}
		}
		(*t_total)++;
	}
	*a_t_total+=nt->l_nt[a].k;
}
//复制模块库所映射函数
void copy_pmapping(int i,int a,int *start_p,int *p_total,struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                   int *split,int *sp_in,int *joint,int *jo_in,int *a_p_total,int p_n,int *add_io,int *addo) {

	int j,b;
	for(j=0; j<nt->l_nt[a].m; j++) {
		mt->pn[*p_total].pre_p=j+1; //number of previous places
		if(mt->pn[*p_total].pre_p==1) { //input place
			mt->pn[*p_total].new_p=h->t[i].pm1[p_n].hp; //use hsn place number
			mt->mark[mt->pn[*p_total].new_p-1]=h->l->mu[h->t[i].pm1[p_n].hp-1]; //result marking
			for(b=0; b<nt->l_nt[a].k; b++) { //result arcs
				if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p) { //p of arc == pre_p, let p of arc =new_p
					if(nt->l_nt[a].a[b].p>0) {
						mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
					} else {
						mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
					}
					mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;

				}
			}
			(*p_total)++;
		} else if(mt->pn[*p_total].pre_p==2) { //output place
			mt->pn[*p_total].new_p=mt->pn[add_io[(*addo)++]].new_p;//*************
			for(b=0; b<nt->l_nt[a].k; b++) { //arc of copy
				if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p) { //p of arc == pre_p, let p of arc =new_p
					if(nt->l_nt[a].a[b].p>0) {
						mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
					} else {
						mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
					}
					mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;

				}
			}
			(*p_total)++;

		} else {
			if(mt->pn[*p_total].pre_p==4) { //copy net places which connect with split transition
				split[(*sp_in)++]=*p_total;
			}
			if(mt->pn[*p_total].pre_p==6) { //copy net places which connect with joint transition
				joint[(*jo_in)++]=*p_total;
			}
			mt->pn[*p_total].new_p=*start_p; //number of result places
			mt->mark[mt->pn[*p_total].new_p-1]=nt->l_nt[a].mu[j]; //result marking
			for(b=0; b<nt->l_nt[a].k; b++) { //result arcs
				if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p) { //p of arc == pre_p, let p of arc =new_p
					if(nt->l_nt[a].a[b].p>0) {
						mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
					} else {
						mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
					}
					mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;

				}
			}
			(*start_p)++;
			(*p_total)++;
		}
	}

	*a_p_total+=nt->l_nt[a].k; //array+
}
//复制模块变迁映射函数
void copy_tmapping(int a,int *start_t,int *t_total,struct mapping_result *mt,struct net_table *nt,int *a_t_total) {
	int j,b;
	for(j=0; j<nt->l_nt[a].n; j++) {
		mt->tn[*t_total].pre_t=j+1; //number of previous transitons
		mt->tn[*t_total].new_t=*start_t;
		(*start_t)++;

		for(b=0; b<nt->l_nt[a].k; b++) {
			if(nt->l_nt[a].a[b].t==mt->tn[*t_total].pre_t) { //p of arc == pre_p, let p of arc =new_p
				mt->new_a[*a_t_total+b].t=mt->tn[*t_total].new_t;
			}
		}
		(*t_total)++;
	}
	*a_t_total+=nt->l_nt[a].k;
}
//clean_move模块库所映射函数
void cm_pmapping(int i,int a,int *start_p,int *p_total,struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                 int *add_io, int *addo,int *a_p_total, int p_n,int *add_cf, int *add_cf_o,int *cf_num,int *cf_n_in,int *c_m_cf,int *cf_in) {

	int j,b;
	for(j=0; j<nt->l_nt[a].m; j++) {
		mt->pn[*p_total].pre_p=j+1; //number of previous places
		if(mt->pn[*p_total].pre_p==1) { //input place
			mt->pn[*p_total].new_p=mt->pn[add_io[(*addo)++]].new_p; //use output places index
			for(b=0; b<nt->l_nt[a].k; b++) { //result arc of clean_move
				if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p) { //p of arc == pre_p, let p of arc =new_p
					if(nt->l_nt[a].a[b].p>0) {
						mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
					} else {
						mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
					}
					mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;

				}
			}
			(*p_total)++;

		} else if(mt->pn[*p_total].pre_p==2) { //output place
			mt->pn[*p_total].new_p=h->t[i].pm1[p_n].hp;
			mt->mark[mt->pn[*p_total].new_p-1]=h->l->mu[h->t[i].pm1[p_n].hp-1];
			for(b=0; b<nt->l_nt[a].k; b++) {
				if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p) { //p of arc == pre_p, let p of arc =new_p
					if(nt->l_nt[a].a[b].p>0) {
						mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
					} else {
						mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
					}
					mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;

				}
			}
			(*p_total)++;

		} else if(mt->pn[*p_total].pre_p==3) { //input place
			mt->pn[*p_total].new_p=mt->pn[add_cf[(*add_cf_o)++]].new_p;//use control flow places index
			for(b=0; b<nt->l_nt[a].k; b++) { //result arc
				if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p) { //p of arc == pre_p, let p of arc =new_p
					if(nt->l_nt[a].a[b].p>0) {
						mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
					} else {
						mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
					}
					mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;

				}
			}
			(*p_total)++;

		} else {
			mt->pn[*p_total].new_p=*start_p; //number of result places
			mt->mark[mt->pn[*p_total].new_p-1]=nt->l_nt[a].mu[j];
			for(b=0; b<nt->l_nt[a].k; b++) {
				if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p) { //p of arc == pre_p, let p of arc =new_p
					if(mt->pn[*p_total].pre_p==5) {
						cf_num[(*cf_n_in)++]=*p_total; //index of clean_move cf places***************
						c_m_cf[(*cf_in)++]=*a_p_total+b; //arc index of clean_move out-cf
					}
					if(nt->l_nt[a].a[b].p>0) {
						mt->new_a[*a_p_total+b].p=mt->pn[*p_total].new_p;
					} else {
						mt->new_a[*a_p_total+b].p=-(mt->pn[*p_total].new_p);
					}
					mt->new_a[*a_p_total+b].w=nt->l_nt[a].a[b].w;

				}
			}
			(*start_p)++;
			(*p_total)++;

		}
	}

	*a_p_total+=nt->l_nt[a].k; //array+
}
//clean_move模块变迁映射
void cm_tmapping(int a,int *start_t,int *t_total,struct mapping_result *mt,struct net_table *nt,int *a_t_total) {

	int j,b;
	for(j=0; j<nt->l_nt[a].n; j++) {
		mt->tn[*t_total].pre_t=j+1; //number of previous transitons
		mt->tn[*t_total].new_t=*start_t;
		(*start_t)++;

		for(b=0; b<nt->l_nt[a].k; b++) {
			if(nt->l_nt[a].a[b].t==mt->tn[*t_total].pre_t) { //p of arc == pre_p, let p of arc =new_p
				mt->new_a[*a_t_total+b].t=mt->tn[*t_total].new_t;
			}
		}
		(*t_total)++;
	}
	*a_t_total+=nt->l_nt[a].k;
}

//create result lsn,include places,marking transitions and arcs
int create_mapping_result(struct mapping_result *mt,struct net_table *nt,struct hsn *h) {
	int i,j,c;
	int memory_p=0; //memory for places
	int memory_t=0; //memory for transitions
	int memory_a=0; //memory for arcs
	for(i=0; i<nt->file_n-1; i++) { //substitution part
		memory_p+=(nt->l_nt[i].m);
		memory_t+=(nt->l_nt[i].n);
		memory_a+=(nt->l_nt[i].k);
	}

	memory_p+=h->l->m; //regular part ; unsubstitute part 不需要替换的部分
	memory_t+=h->l->n;
	memory_a+=h->l->k;

	mt->pn=malloc(1.5*memory_p*sizeof(struct place_num)); //malloc for places (need control flow places)
	mt->mark=malloc(1.5*memory_p*sizeof(int) ); //malloc for marking
	mt->tn=malloc(1.5*memory_t*sizeof(struct transition_num)); //malloc for transition.  (need split and joint trantition)
	mt->new_a=malloc(1.5*memory_a*sizeof(struct arc));//malloc for arcs

	int start_p=(h->l->m)+1; //renumerate from hsn->num of places +1
	int p_total=0; //array index of result places

	int start_t=(h->l->n)+1; //renumerate from hsn->num of transitions +1
	int t_total=0; //array index of result transitions

	int a_p_total=0;//array index of arc's place
	int a_t_total=0;//array index of arc's transiton
	int a=0;//intermediate variable
	int b;//intermediate variable

	int add_io[MAX_NUM];//record subsitituted net input/output places array index. (P1,P2,P3 of add_lsn)
	int addi=0;
	int addo=0; //i:record index. o:use index
	int add_cf[MAX_NUM];//record subsitituted net control flow places array index (P4,P5 of add_lsn)
	int add_cf_i=0;
	int add_cf_o=0; //i:record index. o:use index

	int split[MAX_NUM]; //record copy net place array index. split[]:places connect with split transition.
	int sp_in=0,sp_out=0; //in:record index. out:use index

	int	joint[MAX_NUM]; //record copy net place array index. joint[]: places connect with joint transition
	int jo_in=0,jo_out=0; //in:record index. out:use index

	int t_split[MAX_NUM]; //record transition array index
	int	t_sp_in=0,t_sp_out=0;  //in:record index. out:use index

	int c_m_cf[MAX_NUM]; //record clean_move control flow places array index
	int cf_in=0,cf_out=0; //in:record index. out:use index

	int cf_num[MAX_NUM]; //record number of control flow place
	int cf_n_in=0,cf_n_out=0; //in:record index. out:use index

	int p_n;//place mapping number
	int copy_count;//to add split and joint transitions
	int sp_count; //to add split  transitions
	int jo_count;//to add joint transitions

	for(i=0; i<h->nst; i++) {
		/*********subsititute net**********/
		/*************places mapping****************/
		//子网的库所映射函数

		sub_pmapping(i, a, &start_p, &p_total, mt, nt, h, add_io, &addi, add_cf, &add_cf_i, &a_p_total);
	
		/*************transitions mapping****************/
		sub_tmapping(i, a, &start_t, &t_total, mt, nt, h, &a_t_total);
		a++;

		copy_count=0;
		for(p_n=0; p_n<h->t[i].pmnum; p_n++) {
			/*********copy_lsn**********/
			if((h->t[i].pm1[p_n].hp)>0&&(h->t[i].pm1[p_n].lp)>0) { //copy_lsn
				copy_count++;
				/*************places mapping****************/
				//复制模块库所映射函数
				copy_pmapping(i, a, &start_p, &p_total,mt,nt,h,split,&sp_in,joint,&jo_in,&a_p_total,p_n,add_io,&addo);
				/***********transition mapping******************/
				//复制模块变迁映射函数
				copy_tmapping(a,&start_t,&t_total,mt,nt,&a_t_total);
				a++;
			}
			/*********clean_move_lsn**********/
			else if((h->t[i].pm1[p_n].hp)>0&&(h->t[i].pm1[p_n].lp)<0) {
				/**************Add Split and Joint trantition********************/
				if(copy_count>0) {//有输入 ，扩展输出前先连分离，结合变迁
					mt->tn[t_total].pre_t=0; //number of previous transitons
					mt->tn[t_total].new_t=start_t;//split transition
					t_split[t_sp_in++]=t_total;//record the split transition index to connect with control flow places
					start_t++;
					//add split transition
					for(sp_count=0; sp_count<copy_count; sp_count++) {
						mt->new_a[a_p_total].p=mt->pn[split[sp_out++]].new_p;
						mt->new_a[a_t_total].t=mt->tn[t_total].new_t;
						mt->new_a[a_p_total].w=1;
						a_p_total+=1;
						a_t_total+=1;
					}
					t_total++;

					mt->tn[t_total].pre_t=0; //number of previous transitons
					mt->tn[t_total].new_t=start_t;//joint transition
					start_t++;
					//add joint transition
					//与替代子网的输入控制库所相连的常规弧
					mt->new_a[a_p_total+0].p=mt->pn[add_cf[add_cf_o++]].new_p;
					mt->new_a[a_t_total+0].t=mt->tn[t_total].new_t;
					mt->new_a[a_p_total+0].w=1;
					a_p_total+=1;
					a_t_total+=1;
					for(jo_count=0; jo_count<copy_count; jo_count++) {
						mt->new_a[a_p_total].p=-(mt->pn[joint[jo_out]].new_p);
						mt->new_a[a_t_total].t=mt->tn[t_total].new_t;
						mt->new_a[a_p_total].w=1;
						mt->new_a[a_p_total+1].p=mt->pn[joint[jo_out++]].new_p;
						mt->new_a[a_t_total+1].t=mt->tn[t_total].new_t;
						mt->new_a[a_p_total+1].w=-1;
						a_p_total+=2;
						a_t_total+=2;
					}
					t_total++;
					copy_count=0;
				}
				/*************places mapping****************/
				//clean_move模块库所映射
				cm_pmapping(i,a,&start_p, &p_total,mt,nt,h,add_io, &addo,&a_p_total,p_n,add_cf, &add_cf_o,cf_num,&cf_n_in,c_m_cf,&cf_in);
				/*************transition mapping********************/
				//clean_move模块变迁映射
				cm_tmapping(a,&start_t,&t_total,mt,nt,&a_t_total);

				a++;
			}

			/***************control flow*******************/
			else if((h->t[i].pm1[p_n].hp)<0&&(h->t[i].pm1[p_n].lp)>0) { //in control flow
				//use hsn places number and marking
				mt->pn[p_total].pre_p=abs((h->t[i].pm1[p_n].hp));
				mt->pn[p_total].new_p=abs((h->t[i].pm1[p_n].hp));
				mt->mark[mt->pn[p_total].new_p-1]=h->l->mu[abs(h->t[i].pm1[p_n].hp)-1];
				//connect with split transition
				mt->new_a[a_p_total+0].p=-(mt->pn[p_total].new_p);
				mt->new_a[a_t_total+0].t=mt->tn[t_split[t_sp_out]].new_t;
				mt->new_a[a_p_total+0].w=1;
				mt->new_a[a_p_total+1].p=mt->pn[p_total].new_p;
				mt->new_a[a_t_total+1].t=mt->tn[t_split[t_sp_out++]].new_t;
				mt->new_a[a_p_total+1].w=-1;
				a_p_total+=2;
				a_t_total+=2;
				p_total++;

			} else { //out control flow
				//use hsn places number and marking
				//replace 5th place number of clean_move net with hsn control flow number
				mt->pn[cf_num[cf_n_out]].new_p=abs((h->t[i].pm1[p_n].hp));
				mt->mark[mt->pn[cf_num[cf_n_out++]].new_p-1]=h->l->mu[abs(h->t[i].pm1[p_n].hp)-1];
				mt->new_a[c_m_cf[cf_out++]].p=abs((h->t[i].pm1[p_n].hp));
				start_p--;

			}
		}

	}
	
	//不需要替代的变迁，由HSN传递至LSN
	for(i=0; i<(h->l->k); i++) {
		for(j=0; j<(h->nst); j++) {
			if((h->l->a[i].t)==(h->t[j].tnum)) {
				break;   //若因弧的变迁与被替代的变迁号一致，则跳出循环，j始终小于nst，最大为nst-1
			}
		}
		if(j==(h->nst)) { //即为完成上述j循环，为运行break；则j==nst
			//places
			mt->pn[p_total].pre_p=h->l->a[i].p; //直接利用HSN中的编号
			mt->pn[p_total].new_p=h->l->a[i].p;
			mt->mark[abs(mt->pn[p_total].new_p)-1]=h->l->mu[abs(h->l->a[i].p)-1];//token与HSN一致
			//transitions
			mt->tn[t_total].pre_t=h->l->a[i].t; //number of previous transitons
			mt->tn[t_total].new_t=h->l->a[i].t;
			//arcs
			mt->new_a[a_p_total+0].p=h->l->a[i].p;
			mt->new_a[a_t_total+0].t=h->l->a[i].t;
			mt->new_a[a_p_total+0].w=h->l->a[i].w;
			p_total++;
			t_total++;
			a_p_total++;
			a_t_total++;
		}
	}
	/******write result net into a structure of lsn*********/
	mt->l_result=malloc(sizeof(struct lsn));
	mt->l_result->m=start_p-1;
	mt->l_result->n=start_t-1;
	mt->l_result->k=a_t_total;

	mt->l_result->mu = malloc( (mt->l_result->m)*sizeof(int) ); // marking
	if((mt->l_result->mu)==NULL) {
		printf("***error: no enough memory for result marking\n");
		exit(2);
	}
	
	//记录非0库所总数
	int noneZero=0; 
	for(i=0; i<start_p-1; i++) {
		mt->l_result->mu[i]=mt->mark[i];
		if(mt->mark[i]!=0){
			noneZero++;
		}
	}
	mt->l_result->l=noneZero;

	mt->l_result->a = malloc( (mt->l_result->k)*sizeof(struct arc) ); // arcs
	if((mt->l_result->a)==NULL) {
		printf("***error: no enough memory for result arc\n");
		exit(2);
	}
	for(i=0; i<a_t_total; i++) {
		mt->l_result->a[i].p=mt->new_a[i].p;
		mt->l_result->a[i].t=mt->new_a[i].t;
		mt->l_result->a[i].w=mt->new_a[i].w;
	}
	return 0;
}
//read hsn
int read_hsn(struct hsn *h,FILE *f) {
	int i;
	
	h->l=malloc(sizeof(struct lsn));
	read_lsn((h->l),&(h->nst),f);
	
	h->t=malloc((h->nst)*sizeof(struct tspm));
	for(i=0; i<h->nst; i++) {
		read_ts(&(h->t[i]),f);
	}
	
	return 0;
}
//write hsn
int write_hsn(struct hsn *h,FILE *f) {
	int i;
	write_lsn((h->l),f);
	//fprintf(f,"%d\n", (h->nst));
	for(i=0; i<h->nst; i++) {
		write_ts(&(h->t[i]),f);
	}
	return 0;
}

//read transition subsititution part of hsn
int read_ts(struct tspm * tm, FILE *f) { //read lsn
	int i;
	char input_buffer[255];
	SKIP_COMM
	sscanf(input_buffer,"%d %d %s", &(tm->tnum),&(tm->pmnum),&(tm->name));

	tm->pm1 = malloc((tm->pmnum)*sizeof(struct pm));
	if((tm->pm1)==NULL) {
		printf("***error: no enough memory for tm->pm1\n");
		exit(2);
	}
	for(i=0; i< tm->pmnum; i++) {
		SKIP_COMM
		sscanf(input_buffer,"%d %d",&(tm->pm1[i].hp),&(tm->pm1[i].lp));
	}
	return 0;
}
//write transition subsititution part of hsn
int write_ts(struct tspm * tm,FILE *f) {
	int i;
	fprintf(f,"%d %d %s\n", (tm->tnum),(tm->pmnum),(tm->name));
	for(i=0; i< tm->pmnum; i++) {
		fprintf(f,"%d %d\n",(tm->pm1[i].hp),(tm->pm1[i].lp));
	}
	return 0;
}
//error check function
int header_check_err(int m, int n, int k) {
	if(m < 0 || n < 0 || k < 0) {
		printf("Wrong header: Negative numbers");
		exit(-1);
	}
	if(m == 0 || n == 0 || k == 0) {
		printf("Wrong header: less than 3 or value zero"); //
		exit(-1);
	}
}

int arc_check_err(int p, int t, int w, int m, int n ,int k) {
	if(p == 0 || t==0) {
		printf("Wrong arc: %d %d %d, p or t should start from 1",p,t,w);
		exit(-1);
	}
	if(p > 0 && t < 0) {
		printf("Wrong arc format:  %d %d %d, p > 0 and t < 0",p,t,w);
		exit(-1);
	}
	if(p > 0 && t > 0 && w<0 && w != -1) {
		printf("Wrong arc: %d %d %d, The weight of inhibitior arc is not -1",p,t,w);
		exit(-1);
	}
	if(p > m) {
		printf("Exceed place number: %d %d %d, Exceed the number of m",p,t,w);
		exit(-1);
	}
	if(t > n) {
		printf("Exceed transition number: %d %d %d, Exceed the number of n",p,t,w);
		exit(-1);
	}
}
//Duplicate arc
int duplicate_check(int p1,int t1,int p2,int t2) {
	if(p1 == p2 && t1 == t2) {
		printf("Duplicate arc %d %d w1 and %d %d w2",p1,t1,p2,t2);
		exit(-1);
	}
}
//read lsn
int read_lsn(struct lsn * l,int *nst, FILE *f) { //read lsn
	int i,j;
	char input_buffer[255];
	SKIP_COMM
	sscanf(input_buffer, "%d %d %d %d %d", &(l->m), &(l->n), &(l->k), &(l->l), nst);
	header_check_err((l->m), (l->n), (l->k));
	
	l->a = malloc( (l->k)*sizeof(struct arc) ); // arcs
	if((l->a)==NULL) {
		printf("***error: no enough memory for l->a\n");
		exit(2);
	}
	for(i=0; i < (l->k); i++) { // read arcs
		SKIP_COMM
		sscanf(input_buffer, "%d %d %d", &(l->a[i].p), &(l->a[i].t), &(l->a[i].w) );
		arc_check_err((l->a[i].p), (l->a[i].t), (l->a[i].w),(l->m), (l->n), (l->k));
		for(j = 0; j < i; j++) {
			duplicate_check(l->a[i].p,l->a[i].t,l->a[j].p,l->a[j].t);
		}
	}
	
	l->mu = malloc( (l->m)*sizeof(int) ); // marking
	if((l->mu)==NULL) {
		printf("***error: no enough memory for l->mu\n");
		exit(2);
	}
	
	//set all markings zero  先将token全置为0 
	for(i=0; i < l->m; i++) {
		l->mu[i]=0;
	}
	//set none zero markings  赋值非0token 
	int pnum,marking;//非0库所的编号和token数 
	for(i=0; i < (l->l); i++) {
		SKIP_COMM
		sscanf(input_buffer, "%d %d", &pnum,&marking );
		l->mu[pnum-1]=marking;
	}
	
	return 0;
}

// write lsn from memory into a .txt file;
int write_lsn(struct lsn * l, FILE *f) {
	int i;
	
	fprintf(f,";m n k l nst\n") ;
	fprintf(f, "%d %d %d %d 0\n", (l->m), (l->n), (l->k), (l->l));

	fprintf(f,";arcs\n") ;
	//arcs
	for(i=0; i < l->k; i++) {
		fprintf(f, "%d %d %d\n", l->a[i].p, l->a[i].t, l->a[i].w);
	}

	
	fprintf(f,"; mu(p):\n") ;
	//非0库所编号及token数 
	for(i=0; i < l->m; i++) {
		if(l->mu[i]!=0) {
			fprintf(f, "%d %d\n", i+1, l->mu[i]);
		}
	}

	return 0;
}

//  @ 2023 Hongfei Zhao	
