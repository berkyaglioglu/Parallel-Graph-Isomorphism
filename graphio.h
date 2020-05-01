#ifndef GRAPH_IO_H
#define GRAPH_IO_H
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <stdlib.h>
using namespace std;
void readEdges(string,vector<vector<int> >&,vector<vector<int> >&);
void readEdges1(string, vector<vector<int> >&);
void printEdges(vector<vector<int> >&);

#endif