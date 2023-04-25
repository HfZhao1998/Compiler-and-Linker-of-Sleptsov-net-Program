// generator of HSN for matrix multiplication of a given length k,  k>=2,  parallel version
//
// USAGE: >gen-k-matrix-par-hsn 5 matrix5.txt

#include <stdio.h>
#include <stdlib.h>

#define rand_term (rand()%10+1)   //���������1-10��Χ 

int main(int argc, char *argv[]) {
	int k=atoi(argv[1]), t, i, j, q, r;
	//����kά�������
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
	//m n k l nst  ����
	int m=6*k*k*k-k*k+2;
	int n=2*k*k*k-k*k+2;
	int arcs=12*k*k*k-3*k*k+3;
	int l=4*k*k*k+1;
	int nst = 2*k*k*k-k*k;
	fprintf(f,"; matrix multiplication(parallel)\n");
	fprintf(f,"; m n k l nst\n");
	fprintf(f,"%d %d %d %d %d\n",m,n,arcs,l,nst);


	//��������
	int rowm=6*k-1;//11  ���п�������
	int rown=2*k-1; //3 ���б�Ǩ����
	// arcs
	fprintf(f,"; arcs\n");

	//�ܿ�����
	fprintf(f,"%d %d %d\n", 1, 1, -1); // p->t inh
	fprintf(f,"%d %d %d\n", -1, 1, 1); // p<-t
	fprintf(f,"%d %d %d\n", 2, 2, 1); // t<-p

	//һ��k^2��
	int startT=3;
	int p=3;
	for(i=1; i<=k*k; i++) {
		//������
		if(i%2==1) {
			//����ѱ�Ǩ�����Ļ�
			fprintf(f,"%d %d %d\n", p, 1, 1);
			//���п�����
			//p=3 ,t=3					t<6
			for(t=startT; t<startT+rown; t++,p++) {
				fprintf(f,"%d %d %d\n", p, t, -1); // p->t inh
				fprintf(f,"%d %d %d\n", -p, t, 1); // p<-t
				fprintf(f,"%d %d %d\n", p+1, t, 1); // t<-p
			}
			//��ϲ���Ǩ�����Ļ�  p=6
			fprintf(f,"%d %d %d\n", p, 2, -1);
			fprintf(f,"%d %d %d\n", -p, 2, 1);
			p++;

			//dashed arc ����
			//t=3,p=7			t<6
			for(t=startT; t<startT+rown; t++) {
				//ǰ�����˷����⴦��
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
				} else if(t==startT+rown-1) {//���һ���ӷ�
					//���һ��add��������ֻ�����������Ҫ��������
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					p+=1;
				} else if(t%2==1) {
					//��Ǩ�������Ϊadd
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					fprintf(f,"%d %d %d\n", p, t+2, 1); // p->t
					p+=1;
				} else if(t%2==0) {
					//��Ǩż�����Ϊmul
					fprintf(f,"%d %d %d\n", p, t, 1); // p->t
					fprintf(f,"%d %d %d\n", p+1, t, 1); // p->t
					fprintf(f,"%d %d %d\n", -(p+2), t, 1); // t->p
					fprintf(f,"%d %d %d\n", p+2, t+1, 1); // p->t
					p+=3;
				}
			}
			startT+=rown;
		} else {
			//����ѱ�Ǩ�����Ļ�
			fprintf(f,"%d %d %d\n", p, 1, 1);
			//���п�����

			for(t=startT; t<startT+rown; t++,p++) {
				fprintf(f,"%d %d %d\n", p, t, -1); // p->t inh
				fprintf(f,"%d %d %d\n", -p, t, 1); // p<-t
				fprintf(f,"%d %d %d\n", p+1, t, 1); // t<-p
			}
			//��ϲ���Ǩ�����Ļ�
			fprintf(f,"%d %d %d\n", p, 2, -1);
			fprintf(f,"%d %d %d\n", -p, 2, 1);
			p++;

			//dashed arc ����
			for(t=startT; t<startT+rown; t++) {
				//ǰ�����˷����⴦��
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
				} else if(t==startT+rown-1) {//���һ���ӷ�
					//���һ��add��������ֻ�����������Ҫ��������
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					p+=1;
				} else if(t%2==0) {
					//��Ǩ�������Ϊadd
					fprintf(f,"%d %d %d\n", -p, t, 1); // t->p
					fprintf(f,"%d %d %d\n", p, t+2, 1); // p->t
					p+=1;
				} else if(t%2==1) {
					//��Ǩż�����Ϊmul
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
	//������������ź�����1
	fprintf(f,"; mu(p)\n");
	fprintf(f,"%d %d\n",2, 1);
	//���п��ƿ���
	startT=3;
	for(i=1; i<=k*k; i++) {
		for(t=startT; t<=startT+rown; t++) {
			fprintf(f,"%d %d\n",t, 1);
		}
		startT+=rowm;
	}

	//��������
	int	startP=2+rown+2;//7

	for(i=0; i<k; i++) {
		for(j=0; j<k; j++) {
			//��¼���
			int res=0;
			for(p=startP,q=0; p<startP+rowm-rown-1; ) {
				//��һ���˷����⴦��
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




	//��Ǩ�����Ϣ�о�
	fprintf(f,"; transition substitution \n");
	startP=2+rown+2;//7
	int conP=3;
	int controlP;
	startT=3;
	for(i=1; i<=k*k; i++) {
		for(p=startP,controlP=conP,t=startT; p<startP+rowm-rown-1; ) {
			if(p==startP) { //��һ�����һ���˷����
				fprintf(f,"%d %d %s\n", t, 5, "mul.lsn"); //substitution info
				fprintf(f,"%d %d\n", p, 1);
				fprintf(f,"%d %d\n", p+1, 2);
				fprintf(f,"%d %d\n", p+2, -4);
				fprintf(f,"%d %d\n", -controlP, 3);
				fprintf(f,"%d %d\n", -(controlP+1),-5);
				p+=3;
				controlP++;
				t++;
			} else { //�������һ��mul��һ��add
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
