#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <stdlib.h>

using namespace std;
#read both both graphs in a file. edges1-2 should have size n.
#result: edges1-2 are filled. edges[i] is the list of vertices i is adjacent to.
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
			edges1[v].push_back(to_v);
			edges1[to_v].push_back(v);
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
          edges2[v].push_back(to_v);
		  edges2[to_v].push_back(v);
        
    }  
  }
  return;

}

#works exactly like readEdges, just reads the first graphs in the file.
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
          edges[v].push_back(to_v);
		  edges[to_v].push_back(v);
    }  
  }
  return;
}
//prints the edges to cout, line by line
void printEdges(vector<vector<int> >& edges)
{
  for (int i = 0; i < edges.size(); i++)
  {
    for(int j = 0; j < edges[i].size(); j++)
      cout << edges[i][j] << " ";
	cout << endl;
  }
  
}


/*
//Example run
int main()
{
    
    int n = 50;
    vector<vector<int> > edges1(n);

    vector<vector<int> > edges2(n);
    for (int i = 0; i < n; i++)
    {
      edges2[i] = vector<int> (n);
    }
    readEdges("graphs/50v_9.txt", edges1, edges2);
    printEdges(edges1);
    return 0;
}
*/
