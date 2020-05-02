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
	ran_init(4);
	n = 100000;
	count = 2;
	int p1 = 150;
	int p2 = n;
	DYNALLSTAT(graph,g,g_sz);
	DYNALLSTAT(int, lab, lab_sz);
	DYNALLSTAT(int, ptn, ptn_sz);
	DYNALLSTAT(int,orbits,orbits_sz);
	int m = SETWORDSNEEDED(n);
	DYNALLOC2(graph,g,g_sz,m,n,"malloc");
	DYNALLSTAT(graph,g2,g2_sz);
	DYNALLOC2(graph,g2,g2_sz,m,n,"malloc");

	DYNALLOC1(int,lab,lab_sz,n,"malloc");
	DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
	DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
	int start = 1;
	string graph_dir_name = "graphs/";
	for(int c = start; c < count; c++)
	{
		FILE * myfile;
		stringstream wss;
		wss << graph_dir_name <<  n << "v_" << c << ".txt";
		cout << wss.str().c_str() << endl;
		myfile = fopen(wss.str().c_str(),"w");
		rangraph2(g,false,p1,p2,m,n);
		DYNALLSTAT(int,perm,p_sz);
		DYNALLOC1(int,perm,p_sz,n,"malloc");
		ranperm(perm,n);
		relabel(g,0,perm,g2,m,n);
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
