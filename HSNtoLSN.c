// Command line:HSNtoLSN hsn_file.txt result_lsn_file.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
/**
����ʹ��˵�����û���ͨ�����롰-h����ȡ����ʹ��˵�������ճ����÷�������/����ĸ�ʽҪ����ȷʹ�ó���

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
���Ľṹ�壺���ṹ�����ڴ洢һ���������ͺ�ָ����Ϣ��
����ָ���Ǩ�ĳ��满��p>0,t>0,w>0
��Ǩָ������ĳ��满��p<0,t>0,w>0
���ƻ��� 			  p>0,t>0,w<0
t1ָ��t2�����Ȼ���	  t1<0,t2<0

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
�򵥻�ͼ�Sleptsov net�Ľṹ�壺���ṹ�����ڴ洢һ��LSN�Ľṹ��Ϣ������������������m����Ǩ��������n
����������k��ÿ��������ʼ���п�������mu���Լ����ڴ洢������Ļ�����Ϣ�Ľṹ��ָ�룺struct arc *a;

 Plain or low-level Sleptsov net structure: This structure is used to store the structural
 information of a LSN, including the number of places: m, the number of transitions: n,
 the number of arcs: k, the initial number of tokens for each place : mu, and a struct pointer
 to store information about the arcs of this network: struct arc *a;
**/
struct lsn { //structure of low-level sleptsov net
	int m;  //number of places
	int n;  //number of transitions
	int k;	//number of arcs
	int *mu; //marking
	struct arc *a; //arcs
};

/**
����ӳ��Ľṹ�壺���ṹ�����ڴ洢�߼�Sleptsov net�Ŀ�����hp��ͼ�Sleptsov net�Ŀ�����lp֮���ӳ���ϵ��
����ͨ��hp��lp���ŵ���ϱ�ʾ�����Ĳ�ͬ��;��
�������������hp>0,lp>0,
�������������hp>0,lp<0��
��������ʼ������hp<0,lp>0,
����������������hp<0,lp<0.

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
��Ǩ����Ϳ���ӳ��ṹ�壺���ṹ�����ڴ洢��Ǩ�������Ϣ�������������Ǩ�ı�ţ�tnum������ӳ���������pmnum,
��Ҫ����ĵͼ���������ƣ��ļ�������name[],�Լ����ڴ洢����ӳ����Ϣ�Ľṹ��ָ��struct pm *pm1.

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
�߼�sleptsov net�Ľṹ�壺���ṹ�����ڴ洢һ��HSN�Ľṹ��Ϣ�������ͼ����粿�֣�struct lsn *l,
��Ǩ�������������nst�� ��Ǩ����Ϳ���ӳ�䲿�֣�struct tspm *t

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
�����б�Ľṹ�壺���ṹ�����ڴ洢һ��HSNת��ΪLSN�����У���Ҫ�õ�������LSN����Ϣ��
����:ÿһ��LSN��Ӧ�ı�ţ�file_n, �������ƣ�filename[][], �Լ��洢ÿ��LSN�Ľṹ�壺struct lsn *l_nt.

The structure of the network list: This structure is used to store all the LSN information that needs
to be used in the process of converting an HSN to an LSN.
Including: the number corresponding to each LSN: file_n, the network name: filename[][],
and the structure storing each LSN: struct lsn *l_nt.
**/
struct net_table { //structure of net table
	int file_n; //number of file
	char filename[50][50]; //record file name
	struct lsn *l_nt;
};

/**
������ŵĽṹ�壺���ṹ�����ڴ洢HSN��LSNת�����������п����ı�ţ�������ת��ǰ�Ŀ�����ţ�pre_p,��ת��
�����±��ŵ���ţ�new_p.

The structure of the place number: This structure is used to store the number of all places in the
process of HSN to LSN conversion, including: the place number before conversion: pre_p,
and the sequence number rearranged after conversion: new_p.
**/
struct place_num { //structure of places number
	int pre_p; //previous place number
	int new_p; //new place number
};

/**
��Ǩ��ŵĽṹ�壺���ṹ�����ڴ洢HSN��LSnת�����������б�Ǩ�ı�ţ�������ת��ǰ�ı�Ǩ��ţ�pre_t,��ת��
�����±��ŵ���ţ�new_t.

The structure of transition number: This structure is used to store the number of all transitions in
the process of HSN to LSn conversion, including: the transition number before the conversion: pre_t,
and the sequence number rearranged after the conversion: new_t.
**/
struct transition_num { //structure of transitions number
	int pre_t; //previous transition number
	int new_t; //new transition number
};

/**
����ת�������д洢��Ϣ�Ľṹ�壺���ṹ�����ڴ洢HSN��LSNת��ʱ��������Ϣ���������������:struct place_num *pn,
��Ǩ���:struct transition_num *tn,��ʼ�п�����:*mark,�������ͺ�ָ���ϵ:struct arc *new_a��
�Լ�ת����ɺ��LSN:struct lsn *l_result.

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
���������б��������ݶ����HSN�ļ�����������Ҫ��ȫ������ģ�飬����COPY��CLEAN_MOVE�������Ӧ��Ǩ��
����ģ�飬��Ϊÿ��������ţ�ͬʱ��ÿ������ģ�����һ���ṹ������С�

Create a network table function: According to the read HSN file, link all the required subnet modules,
including COPY, CLEAN_MOVE and the subnet modules that substitute the corresponding transitions,
and number each subnet, and store each subnet module in a structure variable.
**/
//create a table of nets and read nets into structure
int create_net_table(struct hsn *h,struct net_table *nt);

/**
�������յ�LSN���������������б�Ĵ洢��Ϣ���������ݴ���ķ���Ϳ������ķ��򣬽����õ�������ģ�����±��ţ�
�������������ں����п����ĸ�ֵ���������Ǩ�����±�ţ��������ӣ��Լ������µı�Ǩ���ڷ�����ϲ���������
���յõ�LSN����������SN�Ĵ������ϡ�

Compile the final LSN function: Use the storage information of the network list to rearrange the used
subnet modules according to the direction of data transmission and the direction of control flow,
Including: merging of places and assignment of token numbers, renumbering of places and transitions,
connection of arcs, and adding new transitions for splitting and merging control flow,
and finally obtaining LSN, which can run on the processor of SN .
**/
//create result lsn,include places,marking transitions and arcs
int create_mapping_result(struct mapping_result *mt,struct net_table *nt,struct hsn *h);

/**
��HSN��Ϣ��������������LSN���ֺͱ�Ǩ�������

Read HSN information function: including: read LSN part and transition substitution part
**/
int read_hsn(struct hsn *h,FILE *f); //read a hsn file

/**
дHSN��Ϣ������������дLSN���ֺͱ�Ǩ�������

Write HSN information function: including: write LSN part and transition substitution part
**/
int write_hsn(struct hsn *h,FILE *f); //write hsn into a file

/**
��ȡLSN�ļ������������ļ�ָ���ȡ�ļ���һ�е�m��n��k��ֵ������ṹ�����l�С�����place����mΪmu����
�ڴ�ռ䣬�����ȡ�ڶ��е�muֵ����ṹ�����l�С����ݻ�������kΪ���Ľṹ����������ڴ�ռ䣬
���ж�ȡ������Ϣ����ṹ�����l�С�

Read LSN file function: use the file pointer to obtain the m, n, and k values of the first line of
the file, and store them in the structure variable l. Allocate mu according to the number of places m
Memory space, read the mu value of the second line one by one and store it in the structure variable l.
Allocate memory space for arc structure variables according to the number of arcs k,
The information of the arc read line by line is stored in the structure variable l.
**/
int read_lsn(struct lsn * l, FILE *f); //read a lsn file

/**
дLSN�ļ�����������LSN�ļ�Ҫ��ĸ�ʽ�������ļ�ָ��ӽṹ������л�ȡm��n��k��ֵ����д�����ļ��С�
����place����m�������ȡ�ṹ������е�muֵ��д�����ļ��С����ݻ�������k�������ȡ�ṹ������л�����Ϣ��
��д�����ļ��С�

Write LSN file function: According to the format required by the LSN file, use the file pointer to
obtain m, n, k values from the structure variables, and write them out to the file.
According to the number of places m, read the mu value in the structure variable one by one and write
it out to the file. According to the number of arcs k, read the information of the arcs in the
structure variable one by one, and write them out to the file.
**/
int write_lsn(struct lsn * l, FILE *f); //write lsn into a file

/**
����Ǩ������ֵĺ���������HSN�ļ��ĸ�ʽҪ�������ļ�ָ���ȡ��Ǩ�������������nst��
����nst����ֵ������ѭ������ȡ�������Ǩ�ı�ţ�tnum������ӳ���������pmnum������ģ�飺name��
����pmnum��ֵ������ѭ������ȡÿ�Կ���ӳ����HSN�Ŀ�����ź�LSN�Ŀ�����š�
��������Ϣ������ṹ������С�

The function of reading the transition substitution part: According to the format requirements
of the HSN file, use the file pointer to obtain the total number of transition substitutions: nst,
according to the value of nst, use a loop to read the number of the substituted transition: tnum,
the number of place mappings: pmnum ,and subnet modules: name. According to the value of pmnum,
use a loop to read the place number of the HSN and the place number of the LSN in each pair of place
mappings. The above information is stored in the structure variable.
**/
int read_ts(struct tspm * tm,FILE *f); //read transitions subsititution of hsn

/**
д��Ǩ������ֵĺ���������HSN�ļ��ĸ�ʽҪ�������ļ�ָ���ȡ�ṹ������еı�Ǩ�������������nst��
����nst����ֵ������ѭ������ȡ�������Ǩ�ı�ţ�tnum������ӳ���������pmnum������ģ�飺name��
����pmnum��ֵ������ѭ������ȡÿ�Կ���ӳ����HSN�Ŀ�����ź�LSN�Ŀ�����š���������Ϣ��һд���ļ��С�

The function of writing the transition substitution part: according to the format requirements of
the HSN file, use the file pointer to read the total number of transition substitutions "nst" in the
structure variable��
According to the value of nst, use a loop to read the number of the substituted transition: tnum,
the number mapped by the library: pmnum and the subnet module: name.
According to the value of pmnum, use a loop to read the place number of the HSN and the place number
of the LSN in each pair of place mappings. Write the above information into the file one by one.
**/
int write_ts(struct tspm * tm,FILE *f); //write transitions subsititution of hsn

//�����Ŀ���ӳ�亯��
void sub_pmapping(int i, int a, int *start_p, int *p_total, struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                  int *add_io, int *addi, int *add_cf,int *add_cf_i, int *a_p_total);
//�����ı�Ǩӳ�亯��
void sub_tmapping(int i, int a, int *start_t, int *t_total, struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                  int *a_t_total);
//����ģ�����ӳ�亯��
void copy_pmapping(int i,int a,int *start_p,int *p_total,struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                   int *split,int *sp_in,int *joint,int *jo_in,int *a_p_total,int p_n,int *add_io,int *addo);
//����ģ���Ǩӳ�亯��
void copy_tmapping(int a,int *start_t,int *t_total,struct mapping_result *mt,struct net_table *nt,int *a_t_total);
//clean_moveģ�����ӳ�亯��
void cm_pmapping(int i,int a,int *start_p,int *p_total,struct mapping_result *mt,struct net_table *nt,struct hsn *h,
                 int *add_io, int *addo,int *a_p_total, int p_n,int *add_cf, int *add_cf_o,int *cf_num,int *cf_n_in,int *c_m_cf,int *cf_in);
//clean_moveģ���Ǩӳ�亯��
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

	FILE *f3;
	for(i=1; i<nt->file_n; i++) {
		f3 = fopen(nt->filename[i], "r" );
		if(f3==NULL) {
			printf( "Cannot open %s file\n" ,nt->filename[i]);
			return -1;
		}
		read_lsn(&(nt->l_nt[i-1]),f3); 
		fclose(f3);
	}
	/*************************************/
	return 0;
}

//��������ӳ�亯��
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
			if(abs(nt->l_nt[a].a[b].p)==mt->pn[*p_total].pre_p && nt->l_nt[a].a[b].w!=0) { //p of arc == pre_p, let p of arc =new_p,�������Ȼ�
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
//������Ǩӳ�亯��
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
			//���б�Ǩ���Ϊ����
			if(nt->l_nt[a].a[b].t==mt->tn[*t_total].pre_t) { //p of arc == pre_p, let p of arc =new_p
				mt->new_a[*a_t_total+b].t=mt->tn[*t_total].new_t;
			} else if(abs(nt->l_nt[a].a[b].t)==mt->tn[*t_total].pre_t) { //���Ȼ��ڶ�λ
				mt->new_a[*a_t_total+b].t=-(mt->tn[*t_total].new_t);
				mt->new_a[*a_t_total+b].w=nt->l_nt[a].a[b].w; //���Ȼ���λ����ѡһ�θ�ֵ����
			} else if(abs(nt->l_nt[a].a[b].p)==mt->tn[*t_total].pre_t && nt->l_nt[a].a[b].w==0) { //���Ȼ���һλ
				mt->new_a[*a_t_total+b].p=-(mt->tn[*t_total].new_t);
			}
		}
		(*t_total)++;
	}
	*a_t_total+=nt->l_nt[a].k;
}
//����ģ�����ӳ�亯��
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
//����ģ���Ǩӳ�亯��
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
//clean_moveģ�����ӳ�亯��
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
//clean_moveģ���Ǩӳ��
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

	memory_p+=h->l->m; //regular part ; unsubstitute part ����Ҫ�滻�Ĳ���
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

	int add_io[20];//record subsitituted net input/output places array index. (P1,P2,P3 of add_lsn)
	int addi=0;
	int addo=0; //i:record index. o:use index
	int add_cf[20];//record subsitituted net control flow places array index (P4,P5 of add_lsn)
	int add_cf_i=0;
	int add_cf_o=0; //i:record index. o:use index

	int split[20]; //record copy net place array index. split[]:places connect with split transition.
	int sp_in=0,sp_out=0; //in:record index. out:use index

	int	joint[20]; //record copy net place array index. joint[]: places connect with joint transition
	int jo_in=0,jo_out=0; //in:record index. out:use index

	int t_split[20]; //record transition array index
	int	t_sp_in=0,t_sp_out=0;  //in:record index. out:use index

	int c_m_cf[10]; //record clean_move control flow places array index
	int cf_in=0,cf_out=0; //in:record index. out:use index

	int cf_num[10]; //record number of control flow place
	int cf_n_in=0,cf_n_out=0; //in:record index. out:use index

	int p_n;//place mapping number
	int copy_count;//to add split and joint transitions
	int sp_count; //to add split  transitions
	int jo_count;//to add joint transitions
	for(i=0; i<h->nst; i++) {
		/*********subsititute net**********/
		/*************places mapping****************/
		//�����Ŀ���ӳ�亯��
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
				//����ģ�����ӳ�亯��
				copy_pmapping(i, a, &start_p, &p_total,mt,nt,h,split,&sp_in,joint,&jo_in,&a_p_total,p_n,add_io,&addo);
				/***********transition mapping******************/
				//����ģ���Ǩӳ�亯��
				copy_tmapping(a,&start_t,&t_total,mt,nt,&a_t_total);
				a++;
			}
			/*********clean_move_lsn**********/
			else if((h->t[i].pm1[p_n].hp)>0&&(h->t[i].pm1[p_n].lp)<0) {
				/**************Add Split and Joint trantition********************/
				if(copy_count>0) {//������ ����չ���ǰ�������룬��ϱ�Ǩ
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
					//�����������������ƿ��������ĳ��满
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
				//clean_moveģ�����ӳ��
				cm_pmapping(i,a,&start_p, &p_total,mt,nt,h,add_io, &addo,&a_p_total,p_n,add_cf, &add_cf_o,cf_num,&cf_n_in,c_m_cf,&cf_in);
				/*************transition mapping********************/
				//clean_moveģ���Ǩӳ��
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
	//����Ҫ����ı�Ǩ����HSN������LSN
	for(i=0; i<(h->l->k); i++) {
		for(j=0; j<(h->nst); j++) {
			if((h->l->a[i].t)==(h->t[j].tnum)) {
				break;   //���򻡵ı�Ǩ�뱻����ı�Ǩ��һ�£�������ѭ����jʼ��С��nst�����Ϊnst-1
			}
		}
		if(j==(h->nst)) { //��Ϊ�������jѭ����Ϊ����break����j==nst
			//places
			mt->pn[p_total].pre_p=h->l->a[i].p; //ֱ������HSN�еı��
			mt->pn[p_total].new_p=h->l->a[i].p;
			mt->mark[abs(mt->pn[p_total].new_p)-1]=h->l->mu[abs(h->l->a[i].p)-1];//token��HSNһ��
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
	for(i=0; i<start_p-1; i++) {
		mt->l_result->mu[i]=mt->mark[i];
	}

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
	read_lsn((h->l),f);
	fscanf(f,"%d", &(h->nst));
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
	fprintf(f,"%d\n", (h->nst));
	for(i=0; i<h->nst; i++) {
		write_ts(&(h->t[i]),f);
	}
	return 0;
}

//read transition subsititution part of hsn
int read_ts(struct tspm * tm, FILE *f) { //read lsn
	int i;
	fscanf(f,"%d %d %s", &(tm->tnum),&(tm->pmnum),&(tm->name));

	tm->pm1 = malloc((tm->pmnum)*sizeof(struct pm));
	if((tm->pm1)==NULL) {
		printf("***error: no enough memory for tm->pm1\n");
		exit(2);
	}
	for(i=0; i< tm->pmnum; i++) {
		fscanf(f,"%d %d",&(tm->pm1[i].hp),&(tm->pm1[i].lp));
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
int read_lsn(struct lsn * l, FILE *f) { //read lsn
	int i,j;

	fscanf(f, "%d %d %d", &(l->m), &(l->n), &(l->k));
	header_check_err((l->m), (l->n), (l->k));

	l->mu = malloc( (l->m)*sizeof(int) ); // marking
	if((l->mu)==NULL) {
		printf("***error: no enough memory for l->mu\n");
		exit(2);
	}

	for(i=0; i < l->m; i++) {
		fscanf(f, "%d ", &(l->mu[i]));
	}

	l->a = malloc( (l->k)*sizeof(struct arc) ); // arcs
	if((l->a)==NULL) {
		printf("***error: no enough memory for l->a\n");
		exit(2);
	}
	for(i=0; i < (l->k); i++) { // read arcs

		fscanf(f, "%d %d %d", &(l->a[i].p), &(l->a[i].t), &(l->a[i].w) );
		arc_check_err((l->a[i].p), (l->a[i].t), (l->a[i].w),(l->m), (l->n), (l->k));
		for(j = 0; j < i; j++) {
			duplicate_check(l->a[i].p,l->a[i].t,l->a[j].p,l->a[j].t);
		}
	}
	return 0;
}

// write lsn from memory into a .txt file;
int write_lsn(struct lsn * l, FILE *f) {
	int i;

	fprintf(f, "%d %d %d\n", (l->m), (l->n), (l->k));

	for(i=0; i < l->m; i++) {
		fprintf(f, "%d ", l->mu[i]);
	}
	fprintf(f,"\n");

	for(i=0; i < l->k; i++) {
		fprintf(f, "%d %d %d\n", l->a[i].p, l->a[i].t, l->a[i].w);
	}

	return 0;
}

