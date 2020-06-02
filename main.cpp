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
        // #pragma omp parallel
        // for (int i=0; i < queue.size(); i++) {
        //     // #pragma omp critical
        //     // {
        //     //     curr_vertex = queue.front();
        //     //     queue.erase(queue.begin());
        //     // }
        //     #pragma omp atomic read
        //     curr_vertex = queue[i];
        //     //#pragma omp for private(curr_vertex)
        //     for (int j=0; j < edges[curr_vertex].size(); j++) {
        //         if (shortestPathAllVertices[vertex][edges[curr_vertex][j]] == -1) {
        //             #pragma omp critical
        //             {
        //                 queue.push_back(edges[curr_vertex][j]);
        //                 shortestPathAllVertices[vertex][edges[curr_vertex][j]] = shortestPathAllVertices[vertex][curr_vertex] + 1;
        //             }
        //         }
        //     }
        // }
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

	void ShortestPathFilter(char * possibleMapSet, int * currFinalVertexMap, int vertexIdG1)
	{
		int vertexIdG2 = currFinalVertexMap[vertexIdG1];

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
		

		#pragma omp parallel for collapse(2) shared(possibleMapSet)
		for (int v1 = 0; v1 < numberOfVertices; v1++){
			for (int v2 = 0; v2 < numberOfVertices; v2++) {
				int dist = graph1.shortestPathAllVertices[vertexIdG1][v1];
				if (possibleMapSet[v1*numberOfVertices + v2] == '1') {
					if(graph2.shortestPathAllVertices[vertexIdG2][v2] != dist) {
						possibleMapSet[v1*numberOfVertices + v2] = '0';
					}
				}
			}
		}
	}


	int CheckProgress(char * possibleMapSet, int * currFinalVertexMap) {
		int progress = 0;
		#pragma omp parallel for reduction(+:progress)
		for(int i = 0; i < numberOfVertices; i++) {
			if (currFinalVertexMap[i] == -1) {
				int numberOfPossibility = 0;
				for (int j = 0; j < numberOfVertices; j++) {
					if (possibleMapSet[i*numberOfVertices + j] == '1') {
						numberOfPossibility++;
						if (numberOfPossibility > 1) {
							break;
						}
					}
				}
				if (numberOfPossibility == 0) {
					progress -= numberOfVertices;
				}
				else if (numberOfPossibility == 1) {
					progress++;
				}
			}
			else {
				progress++;
			}
		}
		return progress;
	}


	int Process(char * possibleMapSet, int * currFinalVertexMap)
	{
		int progress = 0;
		int numberOfPossibility;
		int onlyMap;
		for (int i = 0; i < numberOfVertices; i++) {
			if (currFinalVertexMap[i] == -1) {
				numberOfPossibility = 0;
				#pragma omp parallel for reduction(+:numberOfPossibility)
				for (int j = 0; j < numberOfVertices; j++) {
					if (possibleMapSet[i*numberOfVertices + j] == '1') {
						onlyMap = j;
						numberOfPossibility++;
					}
				}
				if(numberOfPossibility == 1) {
					currFinalVertexMap[i] = onlyMap;
					ShortestPathFilter(possibleMapSet, currFinalVertexMap, i);
					progress = CheckProgress(possibleMapSet, currFinalVertexMap);
					if (progress < 0 || progress == numberOfVertices) {
						break;
					}
				}
				else if (numberOfPossibility == 0) {
					progress = -1;
					break;
				}
			}
		}
		return progress;
	}

	void BruteForce(char * possibleMapSet, int * currFinalVertexMap)
	{
		int vertexIdG1;
		for (int i = 0; i < numberOfVertices; i++) {
			if (currFinalVertexMap[i] == -1) {
				vertexIdG1 = i;
				break;
			}
		}
		
		int progress = 0;

		for (int vertexIdG2 = 0; vertexIdG2 < numberOfVertices; vertexIdG2++) {
			if (possibleMapSet[vertexIdG1*numberOfVertices + vertexIdG2] == '1') {

				possibleMapSet[vertexIdG1*numberOfVertices + vertexIdG2] = '1';
				currFinalVertexMap[vertexIdG1] = vertexIdG2;

				ShortestPathFilter(possibleMapSet, currFinalVertexMap, vertexIdG1);
				progress = Process(possibleMapSet, currFinalVertexMap);
				
				if (progress == numberOfVertices) {
					isomorphismFound = true;
					for (int vertexId = 0; vertexId < numberOfVertices; vertexId++) {
						if (currFinalVertexMap[vertexId] == -1) {
							int onlyMap;
							for (int i = 0; i < numberOfVertices; i++) {
								if (possibleMapSet[vertexId*numberOfVertices + i] == '1') {
									onlyMap = i;
									break;
								}
							}
							finalVertexMap[vertexId] = onlyMap;
						}
						finalVertexMap[vertexId] = currFinalVertexMap[vertexId];
					}

					return;
				}
				else if (progress < 0) { // violating condition, continue for the next try
					continue;
				}

				BruteForce(possibleMapSet, currFinalVertexMap);
				if (isomorphismFound) {
					return;
				}
			}
		}
	}
	
	
	void Solve()
	{
		if (graph1.edges.size() != graph2.edges.size()) {
			cout << "Graphs do not have the same number of vertices." << endl;
			return;
		}

		// check iso olabilir mi, aynı degreeler 2 graph'ta aynı sayıda node'a sahip olmalı.
		unordered_map< int, pair< vector<int>, vector<int> > > degreeVerticesTable;
		unordered_map< int, pair< vector<int>, vector<int> > >::iterator mapItr;
		pair<vector<int>,vector<int>> pairOfVecs;
		int degree;

		for (int vertexId = 0; vertexId < numberOfVertices; vertexId++) {
			degree = graph1.edges[vertexId].size();
			if (degreeVerticesTable.find(degree) == degreeVerticesTable.end()) { // if degree has not been inserted into hash table before
				degreeVerticesTable.insert({degree, pairOfVecs});
			}
			mapItr = degreeVerticesTable.find(degree);
			(mapItr->second).first.push_back(vertexId); // insert node id for the first graph

			degree = graph2.edges[vertexId].size();
			if (degreeVerticesTable.find(degree) == degreeVerticesTable.end()) { // if degree has not been inserted into hash table before
				degreeVerticesTable.insert({degree, pairOfVecs});
			}
			mapItr = degreeVerticesTable.find(degree);
			(mapItr->second).second.push_back(vertexId); // insert node id for the second graph
		}
		

		char * possibleMapSet = new char[numberOfVertices*numberOfVertices];
        #pragma omp parallel for shared(possibleMapSet)
		for (int i = 0; i < numberOfVertices*numberOfVertices; i++) {
			possibleMapSet[i] = '0';
		}
		//possibleMapSet set et
		vector<int> verticesG1;
		vector<int> verticesG2;
        
        for (mapItr = degreeVerticesTable.begin(); mapItr != degreeVerticesTable.end(); mapItr++) {
            verticesG1 = (mapItr->second).first;
            verticesG2 = (mapItr->second).second;
            if (verticesG1.size() == verticesG2.size()) {
                #pragma omp parallel for collapse(2) shared(possibleMapSet)
                for (int i = 0; i < verticesG1.size(); i++) {
                    for (int j = 0; j < verticesG2.size(); j++) {
                        possibleMapSet[verticesG1[i]*numberOfVertices + verticesG2[j]] = '1';
                    }
                } 
            }
            else {
                delete[] possibleMapSet;
                cout << "There are different number of vertices with the same degree." << endl;
                return;
            }
        }
		
		int * currFinalVertexMap = new int[numberOfVertices];
        #pragma omp parallel for shared(currFinalVertexMap)
		for (int i = 0; i < numberOfVertices; i++) {
			currFinalVertexMap[i] = -1;
		}


///------------------------
		int progress;

		progress = Process(possibleMapSet, currFinalVertexMap);
		if (progress == numberOfVertices) {
			isomorphismFound = true;
			for (int vertexId = 0; vertexId < numberOfVertices; vertexId++) {
				if (currFinalVertexMap[vertexId] == -1) {
					int onlyMap;
					for (int i = 0; i < numberOfVertices; i++) {
						if (possibleMapSet[vertexId*numberOfVertices + i] == '1') {
							onlyMap = i;
							break;
						}
					}
					finalVertexMap[vertexId] = onlyMap;
				}
				finalVertexMap[vertexId] = currFinalVertexMap[vertexId];
			}
			delete[] possibleMapSet;
			delete[] currFinalVertexMap;
			return;
		}
		else if (progress < 0) { // violating condition
			delete[] possibleMapSet;
			delete[] currFinalVertexMap;
			return;
		}


		double start, end;
		start = omp_get_wtime();

		BruteForce(possibleMapSet, currFinalVertexMap);

		end = omp_get_wtime();
		cout << "Time brute force: " << (end - start) << endl;
		
		delete[] possibleMapSet;
		delete[] currFinalVertexMap;
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
