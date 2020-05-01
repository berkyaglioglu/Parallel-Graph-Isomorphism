#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>
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

	GraphMapper(Graph g1, Graph g2): graph1(g1), graph2(g2)
	{
		isomorphismFound = false;
		numberOfVertices = graph1.edges.size();
		finalVertexMap = vector<int>(numberOfVertices);
	}


	bool CheckForViolation(vector< unordered_set<int> > & possibleMapSet)
	{
		for (int vertexId = 0; vertexId < possibleMapSet.size(); vertexId++) {
			if (possibleMapSet[vertexId].size() == 0) {
				return true;
			}
		}
	}
	
	
	void ShortestPathFilter(vector< unordered_set<int> > & possibleMapSet, unordered_set<int> & remainingVertexSet, int vertexIdG1)
	{
		int vertexIdG2 = *possibleMapSet[vertexIdG1].begin();
	}


	void BruteForce()
	{

	}


	void Solve()
	{
		vector< unordered_set<int> > possibleMapSet(numberOfVertices);
		unordered_set<int> remainingVertexSet;

		if (graph1.edges.size() != graph2.edges.size()) {
			cout << "Graphs do not have the same number of vertices." << endl;
			return;
		}

		// check iso olabilir mi, aynı degreeler 2 graph'ta aynı sayıda node'a sahip olmalı.
		unordered_map< int, pair< unordered_set<int>, unordered_set<int> > > degreeVerticesTable;
		pair<unordered_set<int>,unordered_set<int>> pairOfSets;
		int degree;

		for (int vertexId = 0; vertexId < numberOfVertices; vertexId++) {
			degree = graph1.edges[vertexId].size();
			if (degreeVerticesTable.find(degree) == degreeVerticesTable.end()) { // if degree has not been inserted into hash table before
				degreeVerticesTable.insert({degree, pairOfSets})
			}
			auto it = degreeVerticesTable.find(degree);
			(it->second).first.insert(vertexId); // insert node id for the first graph

			degree = graph2.edges[vertexId].size();
			if (degreeVerticesTable.find(degree) == degreeVerticesTable.end()) { // if degree has not been inserted into hash table before
				degreeVerticesTable.insert({degree, pairOfSets})
			}
			auto it = degreeVerticesTable.find(degree);
			(it->second).second.insert(vertexId); // insert node id for the second graph
		}
		

		//remainingVertexSet set et
		//possibleMapSet set et
		unordered_set<int> verticesG1;
		unordered_set<int> verticesG2;
		for (auto mapIt = degreeVerticesTable.begin(); mapIt != degreeVerticesTable.end(); mapIt++) {
			verticesG1 = (mapIt->second).first;
			verticesG2 = (mapIt->second).second;
			if (verticesG1.size() == verticesG2.size()) {
				for (unordered_set<int>::iterator setIt = verticesG1.begin(); setIt != verticesG1.end(); setIt++) {
					possibleMapSet[*setIt] = verticesG2;
					remainingVertexSet.insert(*setIt)
				}
			}
			else {
				cout << "There are different number of vertices with the same degree." << endl;
				return;
			}
		}

		//graph1.SetShortestPathAllVertices ve graph2.SetShortestPathAllVertices
		graph1.SetShortestPathAllVertices();
		graph2.SetShortestPathAllVertices();


		// process posibleMapSet
		for (int vertexId = 0; vertexId < possibleMapSet.size(); vertexId++) {
			/*
			if map bulduklarımız için, 
				shortestpath i kullanarak bütün diğer setleri eliminate edebilecğimiz var mı diye kontrol et
			*/
			if (possibleMapSet[vertexId].size() == 1 && remainingVertexSet.find(vertexId) != remainingVertexSet.end()) {
				remainingVertexSet.erase(vertexId);
				ShortestPathFilter(possibleMapSet, remainingVertexSet, vertexId);
			}
		}

		if (CheckForViolation(possibleMapSet)) {
			return;
		}

		//bruteforce , brute forceda tempPossibleMapDet
		if (remainingVertexSet.size() != 0) {
			BruteForce();
		}

	}

};












int main()
{
	//Take input data and set graph1 and graph2 accordingly
	int numberOfVertices = 10;

	Graph g1(size);
	Graph g2(size);

	GraphMapper graphMapper(g1, g2);

	graphMapper.Solve();

	if (graphMapper.isomorphismFound) {
		// show solution in output file by using graphMapper.vertexMap
	}
	else {
		cout << "No isomorphism found" << endl;
	}

	return 0;
}