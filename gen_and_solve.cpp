extern "C" {

#include "nauty.h"
#include "naututil.h"

}
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
using namespace std;

int main(){
	int count, n, p;
	ran_init(0);
	n = 100;
	count = 1;
	int p1 = 4;
	int p2 = n;
	

	int m = SETWORDSNEEDED(n);
	size_t k;
	
	DYNALLSTAT(int,lab1,lab1_sz);
	DYNALLSTAT(int,lab2,lab2_sz);
	DYNALLSTAT(int,ptn,ptn_sz);
	DYNALLSTAT(int,orbits,orbits_sz);
	DYNALLSTAT(int,map,map_sz);

	DYNALLSTAT(graph,g1,g_sz);
	DYNALLSTAT(graph,g2,g2_sz);
	DYNALLSTAT(graph,cg1,cg1_sz);
	DYNALLSTAT(graph,cg2,cg2_sz);
	
	DYNALLOC2(graph,g1,g_sz,m,n,"malloc");
	DYNALLOC2(graph,g2,g2_sz,m,n,"malloc");
	
	static DEFAULTOPTIONS_GRAPH(options);
	statsblk stats;
	
	options.getcanon = TRUE;

	DYNALLOC1(int,lab1,lab1_sz,n,"malloc");
	DYNALLOC1(int,lab2,lab2_sz,n,"malloc");
	DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
	DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
	DYNALLOC1(int,map,map_sz,n,"malloc");
	DYNALLOC2(graph,g1,g1_sz,n,m,"malloc");
	DYNALLOC2(graph,g2,g2_sz,n,m,"malloc");
	DYNALLOC2(graph,cg1,cg1_sz,n,m,"malloc");
	DYNALLOC2(graph,cg2,cg2_sz,n,m,"malloc");
	int start = 0;
	string graph_dir_name = "graphs/";
	for(int c = start; c < count; c++)
	{	
		
		FILE * myfile;
		stringstream wss;
		wss << graph_dir_name <<  n << "v_" << c << ".txt";
		cout << wss.str().c_str() << endl;
		myfile = fopen(wss.str().c_str(),"w");
		rangraph2(g1,false,p1,p2,m,n);
		densenauty(g1,lab1,ptn,orbits,&options,&stats,m,n,cg1);

		DYNALLSTAT(int,perm,p_sz);
		DYNALLOC1(int,perm,p_sz,n,"malloc");
		ranperm(perm,n);
		relabel(g1,0,perm,g2,m,n);
		densenauty(g2,lab2,ptn,orbits,&options,&stats,m,n,cg2);
		for (k = 0; k < m*(size_t)n; ++k)
			if (cg1[k] != cg2[k]) break;
		if (k == m*(size_t)n)
		{
		printf("Isomorphic.\n");
		if (n <= 1000)
		{
			/* Write the isomorphism. For each i, vertex lab1[i]
			of sg1 maps onto vertex lab2[i] of sg2. We compute
			the map in order of labelling because it looks better. */
			for (int i = 0; i < n; ++i) map[lab1[i]] = lab2[i];
			for (int i = 0; i < n; ++i) printf(" %d-%d",i,map[i]);
			printf("\n");
		}
		}
		else
			printf("Not isomorphic.\n");
		putgraph(myfile,g,0,m,n);
		fprintf(myfile,"#\n");
		putgraph(myfile,g2,0,m,n);
		cout << "closing file" << endl;
		fclose(myfile);
	}
	/*
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
	*/
	return 0;
}
