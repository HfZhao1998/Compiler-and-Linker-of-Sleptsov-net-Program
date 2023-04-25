// generator of HSN for matrix multiplication of a given length k,  k>=2,  parallel version
//
// USAGE: >gen-k-matrix-par-hsn 5 matrix5.txt

#include <stdio.h>
#include <stdlib.h>

#define rand_term (rand()%10+1)   //生成随机数1-10范围 

int main(int argc, char *argv[]) {
	int k=atoi(argv[1]), t, i, j, q, r;
	//生成k维随机矩阵
	//generate  a k*k matrix
	int A[k][k],B[k][k],C[k][k];
	for(i=0; i<k; i++) {
		for(j=0; j<k; j++) {
			r=rand_term;
			A[i][j]=r;
			r=rand_term;
			B[i][j]=r;
		}
	}


	FILE *f;
	f = fopen(argv[2], "w" );
	//m n k l nst  总数
	int m=6*k*k*k-k*k+2;
	int n=2*k*k*k-k*k+2;
	int arcs=12*k*k*k-3*k*k+3;
	int l=4*k*k*k+1;
	int nst = 2*k*k*k-k*k;
	fprintf(f,"; matrix multiplication(parallel)\n");
	fprintf(f,"; m n k l nst\n");
	fprintf(f,"%d %d %d %d %d\n",m,n,arcs,l,nst);


	//单行数量
	int rowm=6*k-1;//11  单行库所数量
	int rown=2*k-1; //3 单行变迁数量
	// arcs
	fprintf(f,"; arcs\n");

	//总控制流
	fprintf(f,"%d %d %d\n", 1, 1, -1); // p->t inh
	fprintf(f,"%d %d %d\n", -1, 1, 1); // p<-t
	fprintf(f,"%d %d %d\n", 2, 2, 1); // t<-p

	//一共k^2行
	int startT=3;
	int p=3;
	for(i=1; i<=k*k; i++) {
		//奇数行
		if(i%2==1) {
			//与分裂变迁相连的弧
			fprintf(f,"%d %d %d\n", p, 1, 1);
			//单行控制流
			//p=3 ,t=3					t<6
			for(t=startT; t<startT+rown; t++,p++) {
				fprintf(f,"%d %d %d\n", p, t, -1); // p->t inh
				fprintf(f,"%d %d %d\n", -p, t, 1); // p<-t
				fprintf(f,"%d %d %d\n", p+1, t, 1); // t<-p
			}
			//与合并变迁相连的弧  p=6
			fprintf(f,"%d %d %d\n", p, 2, -1);
			fprintf(f,"%d %d %d\n", -p, 2, 1);
			p++;

			//dashed arc 参数
			//t=3,p=7			t<6
			for(t=startT; t<startT+rown; t++) {
				//前两个乘法特殊处理
				if(t==startT) {
					fprintf(f,"%d %d %d\n", p, t, 1); // p->t
					fprintf(f,"%d %d %d\n", p+1, t, 1); // p->t
					fprintf(f,"%d %d %d\n", -(p+2), t, 1); // t->p
					fprintf(f,"%d %d %d\n", p+2, t+2, 1); // p->t
					p+=3;
				} else if(t==startT+1) {
					fprintf(f,"%d %d %d\n", p, t, 1); // p->t
					fprintf(f,"%d %d %d\n", p+1, t, 1); // p->t
					fprintf(f,"%d %d %d\n", -(p+2), t, 1); // t->p
					fprintf(f,"%d %d %d\n", p+2, t+1, 1); // p->t
					p+=3;
				} else if(t==startT+rown-1) {//最后一个加法
					//最后一个add单独处理，只有输出，不需要向后边输入
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					p+=1;
				} else if(t%2==1) {
					//变迁奇数编号为add
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					fprintf(f,"%d %d %d\n", p, t+2, 1); // p->t
					p+=1;
				} else if(t%2==0) {
					//变迁偶数编号为mul
					fprintf(f,"%d %d %d\n", p, t, 1); // p->t
					fprintf(f,"%d %d %d\n", p+1, t, 1); // p->t
					fprintf(f,"%d %d %d\n", -(p+2), t, 1); // t->p
					fprintf(f,"%d %d %d\n", p+2, t+1, 1); // p->t
					p+=3;
				}
			}
			startT+=rown;
		} else {
			//与分裂变迁相连的弧
			fprintf(f,"%d %d %d\n", p, 1, 1);
			//单行控制流

			for(t=startT; t<startT+rown; t++,p++) {
				fprintf(f,"%d %d %d\n", p, t, -1); // p->t inh
				fprintf(f,"%d %d %d\n", -p, t, 1); // p<-t
				fprintf(f,"%d %d %d\n", p+1, t, 1); // t<-p
			}
			//与合并变迁相连的弧
			fprintf(f,"%d %d %d\n", p, 2, -1);
			fprintf(f,"%d %d %d\n", -p, 2, 1);
			p++;

			//dashed arc 参数
			for(t=startT; t<startT+rown; t++) {
				//前两个乘法特殊处理
				if(t==startT) {
					fprintf(f,"%d %d %d\n", p, t, 1); // p->t
					fprintf(f,"%d %d %d\n", p+1, t, 1); // p->t
					fprintf(f,"%d %d %d\n", -(p+2), t, 1); // t->p
					fprintf(f,"%d %d %d\n", p+2, t+2, 1); // p->t
					p+=3;
				} else if(t==startT+1) {
					fprintf(f,"%d %d %d\n", p, t, 1); // p->t
					fprintf(f,"%d %d %d\n", p+1, t, 1); // p->t
					fprintf(f,"%d %d %d\n", -(p+2), t, 1); // t->p
					fprintf(f,"%d %d %d\n", p+2, t+1, 1); // p->t
					p+=3;
				} else if(t==startT+rown-1) {//最后一个加法
					//最后一个add单独处理，只有输出，不需要向后边输入
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					p+=1;
				} else if(t%2==0) {
					//变迁奇数编号为add
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					fprintf(f,"%d %d %d\n", p, t+2, 1); // p->t
					p+=1;
				} else if(t%2==1) {
					//变迁偶数编号为mul
					fprintf(f,"%d %d %d\n", p, t, 1); // p->t
					fprintf(f,"%d %d %d\n", p+1, t, 1); // p->t
					fprintf(f,"%d %d %d\n", -(p+2), t, 1); // t->p
					fprintf(f,"%d %d %d\n", p+2, t+1, 1); // p->t
					p+=3;
				}
			}
			startT+=rown;
		}

	}

	// making mapping
	//控制流库所编号和数量1
	fprintf(f,"; mu(p)\n");
	fprintf(f,"%d %d\n",2, 1);
	//所有控制库所
	startT=3;
	for(i=1; i<=k*k; i++) {
		for(t=startT; t<=startT+rown; t++) {
			fprintf(f,"%d %d\n",t, 1);
		}
		startT+=rowm;
	}

	//参数库所
	int	startP=2+rown+2;//7

	for(i=0; i<k; i++) {
		for(j=0; j<k; j++) {
			//记录结果
			int res=0;
			for(p=startP,q=0; p<startP+rowm-rown-1; ) {
				//第一个乘法特殊处理
				if(p==startP) {
					fprintf(f,";input places:A[%d][%d]:p%d=%d, B[%d][%d]:p%d=%d\n",i,q,p,A[i][q],q,j,p+1,B[q][j]);
					fprintf(f,"%d %d\n", p, A[i][q]);
					fprintf(f,"%d %d\n", p+1, B[q][j]);
					res+=A[i][q]*B[q][j];
					q++;
					p+=3;
				} else  {
					fprintf(f,";input places:A[%d][%d]:p%d=%d, B[%d][%d]:p%d=%d\n",i,q,p,A[i][q],q,j,p+1,B[q][j]);
					fprintf(f,"%d %d\n", p, A[i][q]);
					fprintf(f,"%d %d\n", p+1, B[q][j]);
					res+=A[i][q]*B[q][j];
					q++;
					p+=4;
				}
			}
			C[i][j]=res;
			fprintf(f,";result place:C[%d][%d]:p%d=%d\n",i,j ,p-1, res);
			fprintf(f,";\n");
			startP+=rowm;
		}
	}




	//变迁替代信息列举
	fprintf(f,"; transition substitution \n");
	startP=2+rown+2;//7
	int conP=3;
	int controlP;
	startT=3;
	for(i=1; i<=k*k; i++) {
		for(p=startP,controlP=conP,t=startT; p<startP+rowm-rown-1; ) {
			if(p==startP) { //第一个添加一个乘法替代
				fprintf(f,"%d %d %s\n", t, 5, "mul.lsn"); //substitution info
				fprintf(f,"%d %d\n", p, 1);
				fprintf(f,"%d %d\n", p+1, 2);
				fprintf(f,"%d %d\n", p+2, -4);
				fprintf(f,"%d %d\n", -controlP, 3);
				fprintf(f,"%d %d\n", -(controlP+1),-5);
				p+=3;
				controlP++;
				t++;
			} else { //其余添加一个mul，一个add
				fprintf(f,"%d %d %s\n", t, 5, "mul.lsn"); //substitution info
				fprintf(f,"%d %d\n", p, 1);
				fprintf(f,"%d %d\n", p+1, 2);
				fprintf(f,"%d %d\n", p+2, -4);
				fprintf(f,"%d %d\n", -controlP, 3);
				fprintf(f,"%d %d\n", -(controlP+1),-5);
				p+=3;
				controlP++;
				t++;
				fprintf(f,"%d %d %s\n", t, 5, "add.lsn"); //substitution info
				fprintf(f,"%d %d\n", p-4, 1);
				fprintf(f,"%d %d\n", p-1, 2);
				fprintf(f,"%d %d\n", p, -3);
				fprintf(f,"%d %d\n", -controlP, 4);
				fprintf(f,"%d %d\n", -(controlP+1),-5);
				p+=1;
				controlP++;
				t++;
			}
		}
		startP+=rowm;
		conP+=rowm;
		startT+=rown;
	}
	fprintf(f,"; input matrix:\n");
	//A:
	for(i=0; i<k; i++) {
		for(j=0; j<k; j++) {
			fprintf(f,";A[%d][%d]=%d\n", i,j,A[i][j]);
		}
	}
	//B:
	for(i=0; i<k; i++) {
		for(j=0; j<k; j++) {
			fprintf(f,";B[%d][%d]=%d\n", i,j,B[i][j]);
		}
	}
	fprintf(f,"; output matrix:\n");
	//C:
	for(i=0; i<k; i++) {
		for(j=0; j<k; j++) {
			fprintf(f,";C[%d][%d]=%d\n", i,j,C[i][j]);
		}
	}
}
