#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <stdlib.h>

using namespace std;
void readEdges(string filename,vector<vector<int> >& edges1, vector<vector<int> >& edges2 )
{
  ifstream ifs(filename.c_str());
  if (! ifs) 
  {
    cout << "Error: " << filename << " could not be opened." << endl;
    return;
  }
  
  string line;
  for (int v = 0; v < edges1.size(); v++)
  {
   
    getline(ifs, line);
    istringstream is(line);
    string word;
    is >> word; //first word is vertice number v
    is >> word; // second word is :, pass these
    while(is >> word)
    {
          if (word[word.size()-1] != ';')
          {
            int to_v = atoi(word.c_str());
            edges1[v][to_v] = 1;
          }
    }  
  }
  getline(ifs, line); //get the dummy #
  for (int v = 0; v < edges2.size(); v++)
  {
   
    getline(ifs, line);
    istringstream is(line);
    string word;
    is >> word;
    is >> word;
    while(is >> word)
    {
          if (word[word.size()-1] == ';')
          {
            word.resize(word.size() - 1);
            
          }
          
          int to_v = atoi(word.c_str());
          edges2[v][to_v] = 1;
        
    }  
  }
  return;

}

void readEdges1(string filename, vector<vector<int> >& edges)
{
  ifstream ifs(filename.c_str());
  if (! ifs) 
  {
    cout << "Error: " << filename << " could not be opened." << endl;
    return;
  }

  string line;
  for (int v = 0; v < edges.size(); v++)
  {
   
    getline(ifs, line);
    istringstream is(line);
    string word;
    is >> word;
    is >> word;
    while(is >> word)
    {
          if (word[word.size()-1] == ';')
          {
            word.resize(word.size() - 1);   
          }
 
          int to_v = atoi(word.c_str());
          edges[v][to_v] = 1;
    }  
  }
  return;
}

void printEdges(vector<vector<int> >& edges)
{
  for (int i = 0; i < edges.size(); i++)
  {
    for(int j = 0; j < edges.size(); j++)
      cout << edges[i][j] << " ";
  }
  cout << endl;
}



int main()
{
    
    int n = 50;
    vector<vector<int> > edges1(n);
    for (int i = 0; i < n; i++)
    {
      edges1[i] = vector<int> (n);
    }
    vector<vector<int> > edges2(n);
    for (int i = 0; i < n; i++)
    {
      edges2[i] = vector<int> (n);
    }
    readEdges("50v_9.txt", edges1, edges2);
    printEdges(edges1);
    return 0;
}