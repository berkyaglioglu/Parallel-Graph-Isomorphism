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
			shortestPathAllVertices[i] = vector<int>(numberOfVertices, -1);
		}
	}
	
	void fillEdges(string filename, bool first = true)
	{
		if (first)
		readEdges1(filename, edges);
	}

	void RunBFS(int vertex)
	{
		vector<int> queue;
		int curr_vertex;
		queue.push_back(vertex);
		shortestPathAllVertices[vertex][vertex] = 0;
		
		for (int i=0; i < queue.size(); i++) {
			curr_vertex = queue[i];
			for (int j=0; j < edges[curr_vertex].size(); j++) {
				if (shortestPathAllVertices[vertex][edges[curr_vertex][j]] == -1) {
					queue.push_back(edges[curr_vertex][j]);
					shortestPathAllVertices[vertex][edges[curr_vertex][j]] = shortestPathAllVertices[vertex][curr_vertex] + 1;
				}
			}
		}
	}
/*
	void createShortestPathAllVertices(){
		for (int i = 0; i < edges.size(); i++)
		{
			for (int j = 0; j < edges[i].size(); j++)
			{
				shortestPathAllVertices[i][edges[i][j]] = 1;
			}
			shortestPathAllVertices[i][i] = 0;
		}
	}

	void SetShortestPathAllVertices() 
	{	
		createShortestPathAllVertices();
		// set shortestPathAllVertices by using edges
		
		for(int k=0; k<numberOfVertices; k++){
			#pragma omp parallel for collapse(2) schedule(static)
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
*/
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

	int ShortestPathFilter(vector< unordered_set<int> > & possibleMapSet, int vertexIdG1)
	{
		int vertexIdG2 = *possibleMapSet[vertexIdG1].begin();

		omp_set_nested(1);
		if (graph1.shortestPathAllVertices[vertexIdG1][vertexIdG1] != 0 && graph2.shortestPathAllVertices[vertexIdG2][vertexIdG2] != 0) {
			#pragma omp parallel
			{
				#pragma omp single
				{
					#pragma omp task
					graph1.RunBFS(vertexIdG1);
					#pragma omp task
					graph2.RunBFS(vertexIdG2);
				}
			}
		}
		else if (graph1.shortestPathAllVertices[vertexIdG1][vertexIdG1] != 0) {
			graph1.RunBFS(vertexIdG1);
		}
		else if (graph2.shortestPathAllVertices[vertexIdG2][vertexIdG2] != 0) {
			graph2.RunBFS(vertexIdG2);
		}
		

		int progress = 0;

		#pragma omp parallel for shared(possibleMapSet) reduction(+:progress)
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
			if (possibleMapSet[vertexId].size() == 1) { // count how many certain map there are
				progress++;
			}
			else if (possibleMapSet[vertexId].size() == 0) { // violating condition for isomorphism, make "progress" negative to notify
				progress += (-numberOfVertices);
			}
		}
		
		return progress;
	}

	int Process(vector< unordered_set<int> > & possibleMapSet, unordered_set<int> & remainingVertexSet)
	{
		int progress = 0;
		for (unordered_set<int>::iterator setItr = remainingVertexSet.begin(); setItr != remainingVertexSet.end(); setItr) {
			if(possibleMapSet[*setItr].size() == 1) {
				unordered_set<int>::iterator tempSetItr = setItr;
				setItr++;
				remainingVertexSet.erase(*tempSetItr);
				progress = ShortestPathFilter(possibleMapSet, *tempSetItr);
				if (progress == numberOfVertices || progress < 0) {
					break;
				}
			}
			else {
				setItr++;
			}
		}
		return progress;
	}

	void BruteForce(vector< unordered_set<int> > & possibleMapSet, unordered_set<int> & remainingVertexSet)
	{
		int vertexIdG1 = *remainingVertexSet.begin();
		int progress = 0;

		if (remainingVertexSet.size() < numberOfVertices) {
			for (unordered_set<int>::iterator setItr = possibleMapSet[vertexIdG1].begin(); setItr != possibleMapSet[vertexIdG1].end(); setItr++) {
				vector< unordered_set<int> > tempPossibleMapSet = possibleMapSet;
				unordered_set<int> tempRemainingVertexSet = remainingVertexSet;

				tempPossibleMapSet[vertexIdG1].clear();
				tempPossibleMapSet[vertexIdG1].insert(*setItr);
				tempRemainingVertexSet.erase(vertexIdG1);

				progress = ShortestPathFilter(tempPossibleMapSet, vertexIdG1);
				if (progress == numberOfVertices) {
					isomorphismFound = true;
					for (int vertexId = 0; vertexId < tempPossibleMapSet.size(); vertexId++) {
						finalVertexMap[vertexId] = *tempPossibleMapSet[vertexId].begin();
					}
					return;
				}
				else if (progress < 0) { // violating condition, continue for the next try
					continue;
				}
				
				progress = Process(tempPossibleMapSet, tempRemainingVertexSet);
				if (progress == numberOfVertices) {
					isomorphismFound = true;
					for (int vertexId = 0; vertexId < tempPossibleMapSet.size(); vertexId++) {
						finalVertexMap[vertexId] = *tempPossibleMapSet[vertexId].begin();
					}
					return;
				}
				else if (progress < 0) { // violating condition, continue for the next try
					continue;
				}
				
				BruteForce(tempPossibleMapSet, tempRemainingVertexSet);
				if (isomorphismFound) {
					return;
				}
			}
		}
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
		

		//possibleMapSet set et
		unordered_set<int> verticesG1;
		unordered_set<int> verticesG2;
		for (mapItr = degreeVerticesTable.begin(); mapItr != degreeVerticesTable.end(); mapItr++) {
			verticesG1 = (mapItr->second).first;
			verticesG2 = (mapItr->second).second;
			if (verticesG1.size() == verticesG2.size()) {
				for (unordered_set<int>::iterator setItr = verticesG1.begin(); setItr != verticesG1.end(); setItr++) {
					possibleMapSet[*setItr] = verticesG2;
				}
			}
			else {
				cout << "There are different number of vertices with the same degree." << endl;
				return;
			}
		}

		// process possibleMapSet
		//remainingVertexSet set et
		int progress = 0;
		for (int vertexId = 0; vertexId < numberOfVertices; vertexId++) {
			if (possibleMapSet[vertexId].size() == 1) {
				progress = ShortestPathFilter(possibleMapSet, vertexId);
				if (progress == numberOfVertices) {
					isomorphismFound = true;
					break;
				}
				else if (progress < 0) {
					return;
				}
			}
			else {
				remainingVertexSet.insert(vertexId);
			}
		}
		
		if (!isomorphismFound) {
			progress = Process(possibleMapSet, remainingVertexSet);
			if (progress == numberOfVertices) {
				isomorphismFound = true;
			}
			else if (progress < 0) {
				return;
			}
		}
		
		// if found, set final vertex map
		if (isomorphismFound) {
			for (int vertexId = 0; vertexId < possibleMapSet.size(); vertexId++) {
				finalVertexMap[vertexId] = *possibleMapSet[vertexId].begin();
			}
		}
		else {//bruteforce , brute forceda tempPossibleMapSet ve tempRemainingVertexSet
			double start, end;
			start = omp_get_wtime();

			BruteForce(possibleMapSet, remainingVertexSet);

			end = omp_get_wtime();
			cout << "Time brute force: " << (end - start) << endl;
		}

	}

};




int main(int argc,char* argv[])
{
	//Take input data and set graph1 and graph2 accordingly
	int numberOfVertices = stoi(argv[2]);

	Graph g1(numberOfVertices);
	Graph g2(numberOfVertices);
	cout << "Graphs initialized" << endl;
	
	fillEdgesBoth(argv[1],g1,g2);
	cout << "G1 edges:" << endl;
	//printEdges(g1.edges);
	cout << "G2 edges:" << endl;
	//printEdges(g2.edges);


	GraphMapper graphMapper(g1, g2);
	


	double start, end;

	start = omp_get_wtime();
	graphMapper.Solve();
	end = omp_get_wtime();

	cout << "Time total: " << (end - start) << endl; 


	if (graphMapper.isomorphismFound) {
		// show solution in output file by using graphMapper.vertexMap
		cout << "Isomorphism found" << endl;
		/*
		for(int i = 0; i < graphMapper.finalVertexMap.size(); i++)
			cout << graphMapper.finalVertexMap[i] << " ";
		cout << endl;*/
	}
	else {
		cout << "No isomorphism found" << endl;
	}

	return 0;
}
