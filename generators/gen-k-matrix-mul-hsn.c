// generator of HSN for matrix multiplication of a given length k,  k>=2
//
// USAGE: >gen-k-matrix-mul-hsn 5 matrix5.txt

#include <stdio.h>
#include <stdlib.h>

#define rand_term (rand()%10+1)   //生成随机数1-10范围 

int main(int argc, char *argv[]) {
	int k=atoi(argv[1]), t, i, j, q, r, param;
	//生成k维随机矩阵
	//generate  a k*k matrix
	int A[k][k],B[k][k];
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
	//m n k l nst
	int m=6*k*k*k-2*k*k+1;
	int n=2*k*k*k-k*k;
	int arcs=12*k*k*k-6*k*k;
	int l=4*k*k*k-k*k;
	int nst = 2*k*k*k-k*k;
	fprintf(f,"; matrix multiplication\n");
	fprintf(f,"; m n k l nst\n");
	fprintf(f,"%d %d %d %d %d\n",m,n,arcs,l,nst);

	// arcs
	// control flow arcs
	fprintf(f,"; arcs\n");
	for(t=1; t<=n; t++) {
		fprintf(f,"%d %d %d\n", t, t, -1); // p->t inh
		fprintf(f,"%d %d %d\n", -t, t, 1); // p<-t
		fprintf(f,"%d %d %d\n", t+1, t, 1); // t<-p
	}

//偶数维矩阵
	if(k%2==0) {
		//乘积结果每一项用到的变迁数
		int num=2*k-1;// k+k-1
		//本组连接开始的变迁编号
		int startT=1;
		// dashed arcs
		//参数开始的库所编号
		param=n+2; //114
		//外层循环，k阶矩阵共需要连接k*k组
		for(j=1; j<=k*k; j++) {
			//第奇数个相乘组
			if(j%2==1) {
				for(t=startT; t<startT+num; t++) {
					//前两个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+2, 1); // p->t
						param+=3;
					} else if(t==startT+1) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					} else if(t==startT+num-1) {
						//最后一个add单独处理，只有输出，不需要向后边输入
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						param+=1;
					} else if(t%2==1) {
						//变迁奇数编号为add
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						fprintf(f,"%d %d %d\n", param, t+2, 1); // p->t
						param+=1;
					} else if(t%2==0) {
						//变迁偶数编号为mul
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					}

				}
				startT+=num;
			} else {
				for(t=startT; t<startT+num; t++) {
					//前两个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+2, 1); // p->t
						param+=3;
					} else if(t==startT+1) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					} else if(t==startT+num-1) {
						//最后一个add单独处理，只有输出，不需要向后边输入
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						param+=1;
					} else if(t%2==0) {
						//变迁偶数编号为add
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						fprintf(f,"%d %d %d\n", param, t+2, 1); // p->t
						param+=1;
					} else if(t%2==1) {
						//变迁奇数编号为mul
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					}

				}
				startT+=num;
			}
		}

		// making mapping
		//控制流库所编号和数量1
		fprintf(f,"; mu(p)\n");
		for(t=2; t<=2*k*k*k-k*k+1; t++)
			fprintf(f,"%d %d\n",t, 1); // mu(p_2)...mu(p_{2k^3-k^2+1})

		//参数库所赋值
		startT=1;
		param=n+2; //114
		for(i=0; i<k; i++) {
			for(j=0; j<k; j++) {
				//偶数组
				if(j%2==0) {
					for(t=startT,q=0; t<startT+num; t++) {
						//第一个乘法特殊处理
						if(t==startT) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=3;
						} else if(t%2==0) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
							//0 token 占位但不输出
//					fprintf(f,"%d %d\n", param+2, 0);
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=4;
						}

					}
					startT+=num;
				} else {
					for(t=startT,q=0; t<startT+num; t++) {
						//第一个乘法特殊处理
						if(t==startT) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=3;
						} else if(t%2==1) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
							//0 token 占位但不输出
//					fprintf(f,"%d %d\n", param+2, 0);
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=4;
						}

					}
					startT+=num;
				}

			}
		}


		//变迁替代信息列举
		startT=1;
		param=n+2; //114
		fprintf(f,"; transition substitution \n");
		for(j=1; j<=k*k; j++) {
			if(j%2==1) {
				for(t=startT; t<startT+num; t++) {
					//第一个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==0) { //mul
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==1) { //add
						fprintf(f,"%d %d %s\n", t, 5, "add_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param-4, 1);
						fprintf(f,"%d %d\n", param-1, 2);
						fprintf(f,"%d %d\n", param, -3);
						fprintf(f,"%d %d\n", -t, 4);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=1;
					}

				}
				startT+=num;
			}else {
				for(t=startT; t<startT+num; t++) {
					//第一个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==1) { //mul
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==0) { //add
						fprintf(f,"%d %d %s\n", t, 5, "add_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param-4, 1);
						fprintf(f,"%d %d\n", param-1, 2);
						fprintf(f,"%d %d\n", param, -3);
						fprintf(f,"%d %d\n", -t, 4);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=1;
					}

				}
				startT+=num;
			}
		}
	}
	//k为奇数 
	else {
		//乘积结果每一项用到的变迁数
		int num=2*k-1;// k+k-1
		//本组连接开始的变迁编号
		int startT=1;
		// dashed arcs
		//参数开始的库所编号
		param=n+2; //114
		//外层循环，k阶矩阵共需要连接k*k组
		for(j=1; j<=k*k; j++) {
			//第奇数个相乘组
			if(j%2==1) {
				for(t=startT; t<startT+num; t++) {
					//前两个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+2, 1); // p->t
						param+=3;
					} else if(t==startT+1) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					} else if(t==startT+num-1) {
						//最后一个add单独处理，只有输出，不需要向后边输入
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						param+=1;
					} else if(t%2==1) {
						//变迁奇数编号为add
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						fprintf(f,"%d %d %d\n", param, t+2, 1); // p->t
						param+=1;
					} else if(t%2==0) {
						//变迁偶数编号为mul
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					}

				}
				startT+=num;
			} else {
				for(t=startT; t<startT+num; t++) {
					//前两个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+2, 1); // p->t
						param+=3;
					} else if(t==startT+1) {
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					} else if(t==startT+num-1) {
						//最后一个add单独处理，只有输出，不需要向后边输入
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						param+=1;
					} else if(t%2==0) {
						//变迁偶数编号为add
						fprintf(f,"%d %d %d\n", -param, t, 1); // t->p
						fprintf(f,"%d %d %d\n", param, t+2, 1); // p->t
						param+=1;
					} else if(t%2==1) {
						//变迁奇数编号为mul
						fprintf(f,"%d %d %d\n", param, t, 1); // p->t
						fprintf(f,"%d %d %d\n", param+1, t, 1); // p->t
						fprintf(f,"%d %d %d\n", -(param+2), t, 1); // t->p
						fprintf(f,"%d %d %d\n", param+2, t+1, 1); // p->t
						param+=3;
					}

				}
				startT+=num;
			}
		}

		// making mapping
		//控制流库所编号和数量1
		fprintf(f,"; mu(p)\n");
		for(t=2; t<=2*k*k*k-k*k+1; t++)
			fprintf(f,"%d %d\n",t, 1); // mu(p_2)...mu(p_{2k^3-k^2+1})

		//参数库所赋值
		startT=1;
		param=n+2; //114
		for(i=0; i<k; i++) {
			for(j=0; j<k; j++) {
				//偶数组
				if((i+j)%2==0) {
					for(t=startT,q=0; t<startT+num; t++) {
						//第一个乘法特殊处理
						if(t==startT) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=3;
						} else if(t%2==0) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
							//0 token 占位但不输出
//					fprintf(f,"%d %d\n", param+2, 0);
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=4;
						}

					}
					startT+=num;
				} else {
					for(t=startT,q=0; t<startT+num; t++) {
						//第一个乘法特殊处理
						if(t==startT) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=3;
						} else if(t%2==1) {
							fprintf(f,"%d %d\n", param, A[i][q]);
							fprintf(f,"%d %d\n", param+1, B[q][j]);
							q++;
							//0 token 占位但不输出
//					fprintf(f,"%d %d\n", param+2, 0);
//					fprintf(f,"%d %d\n", param+2, 0);
							param+=4;
						}

					}
					startT+=num;
				}

			}
		}


		//变迁替代信息列举
		startT=1;
		param=n+2; //114
		fprintf(f,"; transition substitution \n");
		for(j=1; j<=k*k; j++) {
			if(j%2==1) {
				for(t=startT; t<startT+num; t++) {
					//第一个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==0) { //mul
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==1) { //add
						fprintf(f,"%d %d %s\n", t, 5, "add_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param-4, 1);
						fprintf(f,"%d %d\n", param-1, 2);
						fprintf(f,"%d %d\n", param, -3);
						fprintf(f,"%d %d\n", -t, 4);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=1;
					}

				}
				startT+=num;
			}else {
				for(t=startT; t<startT+num; t++) {
					//第一个乘法特殊处理
					if(t==startT) {
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==1) { //mul
						fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param, 1);
						fprintf(f,"%d %d\n", param+1, 2);
						fprintf(f,"%d %d\n", param+2, -4);
						fprintf(f,"%d %d\n", -t, 3);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=3;
					} else if(t%2==0) { //add
						fprintf(f,"%d %d %s\n", t, 5, "add_lsn.txt"); //substitution info
						fprintf(f,"%d %d\n", param-4, 1);
						fprintf(f,"%d %d\n", param-1, 2);
						fprintf(f,"%d %d\n", param, -3);
						fprintf(f,"%d %d\n", -t, 4);
						fprintf(f,"%d %d\n", -(t+1),-5);
						param+=1;
					}

				}
				startT+=num;
			}
		}
	}
}
