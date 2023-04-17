// generator of HSN for sequential multiplication of a given length k
// 
// USAGE: >gen-k-mul-hsn 5 mul5.txt

#include <stdio.h>
#include <stdlib.h>

#define rand_term (rand()%10+1)   //生成随机数1-10范围 

int main(int argc, char *argv[])
{
	int k=atoi(argv[1]), t, i, sum=1, r, off;
	FILE *f; 
	f = fopen(argv[2], "w" );
	//header m n k l nst
	fprintf(f,"; multiplication of random numbers\n");
	fprintf(f,"; m n k l nst\n");
	fprintf(f,"%d %d %d %d %d\n",3*k+2,k,6*k,2*k+1,k);
	
	// arcs
	fprintf(f,"; arcs:\n");
	for(t=1; t<=k; t++) {
		fprintf(f,"%d %d %d\n", t, t, -1); // p->t inh
		fprintf(f,"%d %d %d\n", -t, t, 1); // p<-t
		fprintf(f,"%d %d %d\n", t+1, t, 1); // t<-p
	}

	off=k+1;
	for(t=1; t<=k; t++) {
		fprintf(f,"%d %d %d\n", t+off, t, 1); // p->t
		fprintf(f,"%d %d %d\n", t+off+1, t, 1); // p->t
		fprintf(f,"%d %d %d\n", -(t+off+2), t, 1); // t->p
		off+=1;
	}

	// making
	fprintf(f,"; mu(p) \n");
	//control flow
	for(t=2; t<=k+1; t++)
		fprintf(f,"%d %d\n",t, 1); // mu(p_{2}) mu(p_{k+1})

	r=rand_term;
	//first param  第一个参数 单独设置 
	fprintf(f,"%d %d\n",k+2, r); // mu(p_{k+2})

	for(t=k+3; t<=3*k+2; t+=2) {
		r=rand_term;
		fprintf(f,"%d %d\n", t, r); // mu(p_{k+3})...mu(p_{3k+2})
	}
    

	fprintf(f,"; transition substitution \n");
    for(t=1;t<=k;t++){
    	fprintf(f,"%d %d %s\n", t, 5, "mul_lsn.txt"); //substitution info
    
		fprintf(f,"%d %d\n", k+2*t,1);
		fprintf(f,"%d %d\n", k+2*t+1,2);
		fprintf(f,"%d %d\n", k+2*t+2,-4);
		fprintf(f,"%d %d\n", -t,3);
		fprintf(f,"%d %d\n", -(t+1),-5);

	}

}
