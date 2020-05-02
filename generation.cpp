extern "C" {

#include "nauty.h"
#include "naututil.h"

}
#include <fstream>
#include <iostream>

using namespace std;

sparsegraph *badcopy(sparsegraph*g1,sparsegraph*g2, int p)
{
	copy_sg(g1,g2);

}


int main(){
	int count, n, p;
	ran_init(11);
	n = 10;
	count = 1;
	p = 2;
	DYNALLSTAT(graph,g,g_sz);
	DYNALLSTAT(int, lab, lab_sz);
	DYNALLSTAT(int, ptn, ptn_sz);
	DYNALLSTAT(int,orbits,orbits_sz);
	#ifdef _NAUTY_H_
	cout << "ye";
	#endif
	int m = SETWORDSNEEDED(n);

	DYNALLOC2(graph,g,g_sz,m,n,"malloc");
	DYNALLSTAT(graph,g2,g2_sz);
	DYNALLOC2(graph,g2,g2_sz,m,n,"malloc");

	DYNALLOC1(int,lab,lab_sz,n,"malloc");
	DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
	DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
	
	rangraph(g,false,p,m,n);
	DYNALLSTAT(int,perm,p_sz);
	DYNALLOC1(int,perm,p_sz,n,"malloc");
	ranperm(perm,n);
	relabel(g,0,perm,g2,m,n);

	//rangraph2(g,false,1,p,m,n);
	printf("done\n");

	SG_DECL(sg);
	nauty_to_sg(g,& sg,m,n);
	FILE * myfile;
	myfile = fopen("example.txt","w");
	putgraph(myfile,g,0,m,n);
	put_sg(myfile, & sg, false,n*2);
	nauty_to_sg(g2,& sg, m,n);
	put_sg(myfile, &sg, false,n*2);
	for (int i = 0; i < n; i++)
	{
		fprintf(myfile,"%d ",perm[i]);
	}
	fprintf(myfile,"\n");
	for(int i = 0; i < n; i++)
	{
		fprintf(myfile, "%d ", sg.d[i]);
	}

	fclose(myfile);
	return 0;
}
