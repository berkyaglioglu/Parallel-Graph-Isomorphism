#include <stdio.h>
#include "Test.cuh"

__global__ void floyd_kernel(int** path_vertices, int k){
	int i =  blockIdx.x;
	int j = threadIdx.x;
	
	int edge = path_vertices[i][j];
	int edge1 = path_vertices[i][k];
	int edge2 = path_vertices[k][j];
	if((edge == -1 && edge1 == -1) || (edge == -1 && edge2 == -1) ){
		path_vertices[i][j] = -1;
	}
	//There is no any path until now 
	else if(edge == -1){
		path_vertices[i][j] = edge2 + edge1;
	}
	//if one of the edge is not exist return current shortest path
	else if(edge1 == -1 || edge2 == -1){
		path_vertices[i][j] =  edge;
	}
	else{
		path_vertices[i][j] = min(edge, edge1 + edge2);
	}
	
}
namspace Wrapper{
	void wrapperfunction(int** shortestPathAllVertices, int numVertives){
 	
	cudaMallocManaged(&shortestPathAllVertices, sizeof(int)*numVertices*numVertices);

	for(int k=0; k<numberOfVertices; k++){
		floyd_kernel<<numVertices, numVertices>>(&shortestPathAllVertices, k);
	}	
		
cudaDeviceSynchronize();
 }
}