#include "graphio.h"
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
	int numberOfVertices;

	Graph(int numVertices) 
	{	
		numberOfVertices = numVertices;
		edges = vector<vector<int>>(numberOfVertices);
		

		/*
		set edges here from the input data that comes with parameter
		*/
		
		shortestPathAllVertices = vector<vector<int>>(numberOfVertices);
		for (int i=0; i < numberOfVertices; i++) {
			shortestPathAllVertices[i] = vector<int>(numberOfVertices);
		}
	}
	
	void fillEdges(string filename, bool first = true)
	{
		if (first)
		readEdges1(filename, edges);
	}

	void SetShortestPathAllVertices() 
	{
		// set shortestPathAllVertices by using edges
		for(int k=0; k<numberOfVertices; k++){
			for(int i=0; i<numberOfVertices; i++){
				for(int j=0; j<numberOfVertices; j++){
					shortestPathAllVertices[i][j] = floyd(shortestPathAllVertices[i][j], shortestPathAllVertices[i][k],shortestPathAllVertices[k][j]);
				}
			}
		}
	}
private: 
	int floyd(int edge, int edge1 , int edge2){
		//If there is no any path and other edge
		if((edge == -1 && edge1 == -1) || (edge == -1 && edge2 == -1) ){
			return -1;
		}
		//There is no any path until now 
		else if(edge == -1){
			return edge2 + edge1;
		}
		//if one of the edge is not exist return current shortest path
		else if(edge1 == -1 || edge2 == -1){
			return edge;
		}
		else{
			return min(edge, edge1 + edge2);
		}

	}	
};

void fillEdgesBoth(string filename,Graph & g1, Graph & g2)
{
	readEdges(filename, g1.edges, g2.edges);
}


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
		vector<int> vertexQueue;
		vertexQueue.push_back(vertexIdG1);
		int vertexIdG2;

		// BFS implemented, this part ends when the vertexQueue is processed completely
		for (int i = 0; i < vertexQueue.size(); i++) {
			vertexIdG1 = vertexQueue[i];
			vertexIdG2 = *possibleMapSet[vertexIdG1].begin();

			for (int vertexId = 0; vertexId < possibleMapSet.size(); vertexId++){
				for (unordered_set<int>::iterator setItr = possibleMapSet[vertexId].begin(); setItr != possibleMapSet[vertexId].end();) {
					int dist = graph1.shortestPathAllVertices[vertexIdG1][vertexId];
					if(graph2.shortestPathAllVertices[vertexIdG2][*setItr] != dist) {
						unordered_set<int>::iterator tempSetItr = setItr;
						setItr++;
						possibleMapSet[vertexId].erase(*tempSetItr);
					}
					else {
						setItr++;
					}
				}
				// When there is a certain map, the vertex is added to the vertexQueue
				if (possibleMapSet[vertexId].size() == 1 && remainingVertexSet.find(vertexId) != remainingVertexSet.end()) {
					remainingVertexSet.erase(vertexId);
					vertexQueue.push_back(vertexId);
				}
			}
		}
		
	}

	

	bool BruteForce(vector< unordered_set<int> > & possibleMapSet, unordered_set<int> & remainingVertexSet)
	{
		/* In case of paralelism, this part is the stop condition of child tasks in case of one of the task finds isomorphism
		if (isomorphismFound) {
			return false
		}
		*/
		if (remainingVertexSet.size() == 0) { // if we map all the vertices
			for (int vertexId = 0; vertexId < possibleMapSet.size(); vertexId++) {
				finalVertexMap[vertexId] = *possibleMapSet[vertexId].begin();
				isomorphismFound = true;
			}
			return true;
		}

		int vertexIdG1 = *remainingVertexSet.begin();

		for (unordered_set<int>::iterator setItr = possibleMapSet[vertexIdG1].begin(); setItr != possibleMapSet[vertexIdG1].end(); setItr++) {
			vector< unordered_set<int> > tempPossibleMapSet = possibleMapSet;
			unordered_set<int> tempRemainingVertexSet = remainingVertexSet;

			tempPossibleMapSet[vertexIdG1].clear();
			tempPossibleMapSet[vertexIdG1].insert(*setItr);
			tempRemainingVertexSet.erase(vertexIdG1);
			ShortestPathFilter(tempPossibleMapSet, tempRemainingVertexSet, vertexIdG1);

			if(!CheckForViolation(tempPossibleMapSet)) {
				if(BruteForce(tempPossibleMapSet, tempRemainingVertexSet))
				{
					return true;
				}
			}
		}

		return false;
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
		unordered_map< int, pair< unordered_set<int>, unordered_set<int> > >::iterator mapItr;
		pair<unordered_set<int>,unordered_set<int>> pairOfSets;
		int degree;

		for (int vertexId = 0; vertexId < numberOfVertices; vertexId++) {
			degree = graph1.edges[vertexId].size();
			if (degreeVerticesTable.find(degree) == degreeVerticesTable.end()) { // if degree has not been inserted into hash table before
				degreeVerticesTable.insert({degree, pairOfSets});
			}
			mapItr = degreeVerticesTable.find(degree);
			(mapItr->second).first.insert(vertexId); // insert node id for the first graph

			degree = graph2.edges[vertexId].size();
			if (degreeVerticesTable.find(degree) == degreeVerticesTable.end()) { // if degree has not been inserted into hash table before
				degreeVerticesTable.insert({degree, pairOfSets});
			}
			mapItr = degreeVerticesTable.find(degree);
			(mapItr->second).second.insert(vertexId); // insert node id for the second graph
		}
		

		//remainingVertexSet set et
		//possibleMapSet set et
		unordered_set<int> verticesG1;
		unordered_set<int> verticesG2;
		for (mapItr = degreeVerticesTable.begin(); mapItr != degreeVerticesTable.end(); mapItr++) {
			verticesG1 = (mapItr->second).first;
			verticesG2 = (mapItr->second).second;
			if (verticesG1.size() == verticesG2.size()) {
				for (unordered_set<int>::iterator setItr = verticesG1.begin(); setItr != verticesG1.end(); setItr++) {
					possibleMapSet[*setItr] = verticesG2;
					remainingVertexSet.insert(*setItr);
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


		// process possibleMapSet
		for (int vertexId = 0; vertexId < possibleMapSet.size(); vertexId++) {
			if (possibleMapSet[vertexId].size() == 1 && remainingVertexSet.find(vertexId) != remainingVertexSet.end()) {
				remainingVertexSet.erase(vertexId);
				ShortestPathFilter(possibleMapSet, remainingVertexSet, vertexId);
				break;
			}
		}


		if (CheckForViolation(possibleMapSet)) {
			return;
		}

		//bruteforce , brute forceda tempPossibleMapSet ve tempRemainingVertexSet
		if (remainingVertexSet.size() != 0) {
			BruteForce(possibleMapSet, remainingVertexSet);
		}

	}

};




int main()
{
	//Take input data and set graph1 and graph2 accordingly
	int numberOfVertices = 10;

	Graph g1(numberOfVertices);
	Graph g2(numberOfVertices);
	
	fillEdgesBoth("graphs/50v_9.txt",g1,g2);

	GraphMapper graphMapper(g1, g2);

	graphMapper.Solve();

	if (graphMapper.isomorphismFound) {
		// show solution in output file by using graphMapper.vertexMap
		cout << "Isomorphism found" << endl;
	}
	else {
		cout << "No isomorphism found" << endl;
	}

	return 0;
}
