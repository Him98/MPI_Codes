#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
typedef long long int ll;
void swap(int arr[],int i,int j)
{
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}
int partition(int arr[], int l, int h)
{
	int pi = l;
	int i = l,j = h;
	while(i<j)
	{
		while(i<h && arr[i]<=arr[pi])
			i++;
		while(arr[j]>arr[pi])
			j--;
		if(i<j)
			swap(arr,i,j);
	}
	swap(arr,pi,j);
	return j;
}
void quicksort(int arr[],int l,int h)
{
	if(l<h)
	{
		int p = partition(arr,l,h);
		quicksort(arr,l,p-1);
		quicksort(arr,p+1,h);	
	}
}
int main(int argc, char **argv)
{
	int i,total,curr_id,partsize;
	int *arr = NULL;
	int *part_arr = NULL;
	MPI_Init(&argc,&argv);
  	MPI_Comm_rank(MPI_COMM_WORLD, &curr_id);
	MPI_Comm_size(MPI_COMM_WORLD, &total);
	int n = atoi(argv[1]);
	// printf("%d\n", n);
	if(curr_id == 0)
	{
		arr = malloc(n*sizeof(int));
		for(i=0;i<n;i++)
			arr[i] = atoi(argv[i+3]);
		// for(i=0;i<n;i++)
		// 	printf("%d ",arr[i]);
	}
	partsize = (n/total);
	part_arr = (int*)malloc(partsize*sizeof(int));
	// MPI_Scatter divides the array named arr into part_arr with each of size partsize
	MPI_Scatter(arr,partsize,MPI_INT,part_arr,partsize,MPI_INT,0,MPI_COMM_WORLD);
	// Performing quicksort on the part_arr
	quicksort(part_arr,0,partsize-1);
	// MPI_Gather is used to gather all the results achieved from sorting different part_arr
	MPI_Gather(part_arr,partsize,MPI_INT,arr,partsize,MPI_INT,0,MPI_COMM_WORLD);

	if(curr_id == 0)
	{
		// Last sort is performed after all the part_arr are combined back to form arr.
		quicksort(arr,0,n-1);
		for(i=0;i<n;i++)
			printf("%d ", arr[i]);
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}