#include <bits/stdc++.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <limits.h>
#include <mpi.h>
#include <time.h>
using namespace std;
typedef long long int ll;
#define INF 10000000
typedef struct{
		 int free;
		 int id;
		} st;
void swap(int *arr, int a,int b)
{
	int tmp = arr[a];
	arr[a] = arr[b];
	arr[b] = tmp;
}

char **adj; // adjacency matrix
char *adj_matrix; // actual store
int num_color=0;

void memory_init(int n){
 int i,j;
 adj=new char*[n];
 adj_matrix=new char[n*n];
 for(i=0;i<n;i++)
   adj[i]=adj_matrix+(i*n);
 for(i=0;i<n;++i)
   for(j=0;j<n;++j)
     adj[i][j]=0;
}

int main(int argc, char **argv){
 int i,j,c,min_free, min_sat, min_id;
 int curr_id, nproc, id_from,n,m,u,v,total;
 int startval, endval, sv, ev;
 int **colors, **free_color; // color classes and free color classes of nodes
int *free_num, *sat,*OK; //number of free color classes for nodes, saturation of a node and is the node ready
	st p, tmp_p;

 MPI_Status status;
 // Initialize MPI:
 MPI_Init(&argc, &argv);
 // Get my rank:
 MPI_Comm_rank(MPI_COMM_WORLD, &curr_id);
 // Get the total number of processors:
 MPI_Comm_size(MPI_COMM_WORLD, &total);

 if(curr_id == 0){
 	
		ifstream fin(string(argv[1]).c_str());

		fin>>n; // vertexes
		fin>>m; // edges
		memory_init(n);

		for(i=0;i<m;i++)
		{
			fin>>u;
			fin>>v;
			adj[u][v]=1;
			adj[v][u]=1;
		}
		fin.close();
 }
 MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
 free_color=new int*[n];
 colors=new int*[n];
 free_num = (int *)malloc(n*sizeof(int));
 OK = (int *)malloc(n*sizeof(int));
 for(i=0;i<n;i++)
 {
   colors[i]=new int[n];
   free_color[i]=new int[n];
   free_num[i]=0;
   OK[i]=0;
 }
 sat = (int *)malloc(n*sizeof(int));
 
 if(curr_id!=0)
   memory_init(n);
 startval = n*curr_id/total;
 MPI_Bcast(adj_matrix, n*n, MPI_CHAR, 0, MPI_COMM_WORLD);
//  for(i=0;i<N*N;i++)
//   cout<<int(adj_matrix[i])<<" ";
// cout<<endl;

 int sum;
 endval = n*(curr_id+1);
 endval /= total;
 for(int i=0;i<n;i++){
   sum=0;
   for(int j=0;j<n;j++){
     sum += adj[i][j];
     colors[i][j]=0;
     free_color[i][j]=0;
   }
   sat[i]=sum;
 }

 //we color N nodes each after other:
	for(v=0;v<n;v++)
	{
		min_free=n;
		min_id=-1;
		min_sat=n;
		//find node of minimum freedom:
		for(i=startval;i<endval;++i)
			if(OK[i] == 0)
				if(min_free>free_num[i] || min_free==free_num[i] && sat[i]<min_sat)
				{
					min_id=i;
					min_free=free_num[i];
					min_sat=sat[i];
				}
		p.id = min_id;
		p.free = min_free;
		//not optimal,
		//we would've need to look for minimum of free and sat together
		MPI_Allreduce(&p, &tmp_p, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);


		OK[tmp_p.id]=1; //ready to color
		min_free=tmp_p.free;
		min_id=tmp_p.id;
		//color it:
		if(!min_free)
		{
			i=startval;
		 //We need a new color class.
		 //the new color class posible class for the rest:
			
			colors[num_color][min_id]=1;
			for(;i<endval;i++)
				if(!OK[i])
				{
					free_num[i]++;
					free_color[i][num_color]=1;
				}
		 //but not for the connected nodes:
			i=startval;
			for(;i<endval;i++)
				if(!OK[i] && adj[i][min_id]){
					free_num[i]--;
					free_color[i][num_color]=0;
		    	}
		 	num_color++;
		}
		else
		{
		 //We put node into an old color class.
		 //find the class:
		 	i=startval;
		 	id_from=0;
			for(;id_from<total;id_from++)
			 	if(n*id_from/total<=min_id)
			 		if(min_id<n*(id_from+1)/total)
			 			break;
			for(c=0;(curr_id==id_from) && (!free_color[min_id][c]);c++);
			MPI_Bcast(&c, 1, MPI_INT, id_from, MPI_COMM_WORLD);
			colors[c][min_id]=1;
			 //the connected nodes' freedom decreases:
			for(;i<endval;i++)
			{
				if(!OK[i])
					if(free_color[i][c] && adj[i][min_id])
					{
						free_num[i]--;
						free_color[i][c]=0;
					}
		 	}
		}
	}
 // cout<<"number of DSATUR colors: "<<num_color<<endl;
 if(curr_id==0)
 {
   int ans[n];
   for(i=0;i<n;i++)
    for(j=0;j<n;j++)
      if(colors[i][j] == 1)
        ans[j] = i;
    cout<<num_color<<endl;
    for(i=0;i<n;i++)
      cout<<ans[i]<<endl;
 }
 // Terminate MPI:
 MPI_Finalize();

}
