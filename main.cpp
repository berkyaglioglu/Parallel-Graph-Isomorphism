#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include <omp.h>
using namespace std;



class Graph
{
public:
	vector<vector<int>> edges;
	vector<vector<int>> shortestPathAllVertices;

	Graph(int numberOfVertices) 
	{
		edges = vector<vector<int>>(numberOfVertices);

		/*
		set edges here from the input data that comes with parameter
		*/
		
		shortestPathAllVertices = vector<vector<int>>(numberOfVertices);
		for (int i=0; i < numberOfVertices; i++) {
			shortestPathAllVertices[i] = vector<int>(numberOfVertices);
		}
	}

	void SetShortestPathAllVertices() 
	{
		// set shortestPathAllVertices by using edges
	}
};



class GraphMapper
{
public:
	Graph graph1;
	Graph graph2;

	bool isomorphismFound;
	int numberOfVertices;
	vector<int> finalVertexMap;

	GraphMapper(Graph g1, Graph g2, int noVertices): graph1(g1), graph2(g2), numberOfVertices(noVertices)
	{

		finalVertexMap = vector<int>(numberOfVertices);
	}

	void Solve()
	{
		vector<set<int>> possibleMapSet(numberOfVertices);

		// check iso olabilir mi, aynı degreeler 2 graph'ta aynı sayıda node'a sahip olmalı.

		//remainingNodeSet set et
		//possibleMapset ini set et

		//graph1.SetShortestPathAllVertices ve graph2.SetShortestPathAllVertices

		// process posibleMapSet

		/*
		if sette tek elamanı olan varsa, 
			tek elemanı map olarak kabul et
			mapini tamamlayan nodu remainingNodeSet ten çıkar
			shortestpath i kullanarak bütün diğer setleri eliminate edebilecğimiz var mı diye kontrol et
		*/

		//bruteforce , brute forceda tempPossibleMapDet


		for(int )
	}

};












int main()
{
	//Take input data and set graph1 and graph2 accordingly
	int numberOfVertices = 10;

	Graph g1(size);
	Graph g2(size);

	GraphMapper graphMapper(g1, g2, size);

	graphMapper.Solve();

	if (graphMapper.isomorphismFound) {
		// show solution in output file by using graphMapper.vertexMap
	}
	else {
		cout << "No isomorphism found" << endl;
	}

	return 0;
}