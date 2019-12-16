#include<bits/stdc++.h>
// #include <algorithm>
// #include <iostream>
// #include <limits.h>
#include "mpi.h"
typedef long long int ll;
#define INF 10000000
void swap(int *arr, int a,int b)
{
    int tmp = arr[a];
    arr[a] = arr[b];
    arr[b] = tmp;
}
bool small_dis(int wt, int *loc_dist, int u, int v)
{
    if((loc_dist[u]+wt) < loc_dist[v] && wt < INF)
        return true;
    return false;
}
void bellman(int curr_id, int total, MPI_Comm comm, int n, int *arr, int *dist, bool *neg)
{
    int i,u,v,var_ch,begin_point,end_point;
    int load[total], begin[total];
    MPI_Bcast(&n, 1, MPI_INT, 0, comm);
    // if(curr_id == 0)
    //     for(int i=0;i<n*n;i++)
    //         printf("%d\n",arr[i]);
    int *loc_dist = (int *)malloc(n*sizeof(int));
    for (int i = 0; i < n; i++)
        loc_dist[i] = INF;
    int loc_n = (n/total);
    int rem = n%total;

    load[0] = loc_n;
    begin[0] = 0;
    for (i = 1; i < total; i++)
    {
        load[i] = loc_n;
        if(i<=rem)
            load[i]++;
        begin[i] = begin[i - 1] + load[i - 1];
    }
    // int *my_work = new int[load[curr_id]];
    int *my_work = (int *)malloc(load[curr_id]*sizeof(int));
    if (curr_id != 0)
        arr = (int *)malloc(n*n*sizeof(int));
    begin_point = begin[curr_id];
    end_point = begin_point + load[curr_id];
    MPI_Bcast(arr, n*n, MPI_INT, 0, comm);
    MPI_Barrier(comm);
    loc_dist[0] = 0;
    // n-1 iterations to ensure that an edge is always added in each of them
    for(i = 0; i < n-1; i++)
    {
        var_ch = 0;
        for(u = 0; u < n; u++)
        {
            if(loc_dist[u] != INF)
            {
                v = begin_point;
                for(; v < end_point; v++)
                {
                    int lc = u*n + v;
                    if(small_dis(arr[lc],loc_dist,u,v))
                    {
                        loc_dist[v] = loc_dist[u] + arr[lc];
                        // CANNOT : my_work[v - my_begin] = loc_dist[u] + weight;
                        var_ch = 1;
                    }
                }
            }
        }
        MPI_Allreduce(MPI_IN_PLACE, &var_ch, 1, MPI_C_BOOL, MPI_LOR, comm);
        if (var_ch == 0)
            break;
        MPI_Allreduce(MPI_IN_PLACE, loc_dist, n, MPI_INT, MPI_MIN, comm);
    }
    if(i == n - 1)
    {
        var_ch = 0;
        for(u = 0; u < n; u++)
        {
            v = begin_point;
            for(; v < end_point; v++)
            {
                int lc = u*n + v;
                // Negative cycle exists
                if(small_dis(arr[lc],loc_dist,u,v))
                {
                    var_ch = true;
                    break;
                }
            }
            if(var_ch)
                break;
        }
        MPI_Allreduce(&var_ch, neg, 1, MPI_C_BOOL, MPI_LOR, comm);
    }

    if(curr_id == 0)
        for(i=0;i<n;i++)
            dist[i] = loc_dist[i];
        // memcpy(dist, loc_dist, n * sizeof(int));
        // copy(loc_dist,loc_dist+n,dist);

    // if(curr_id != 0)
    //     delete[] arr;
    // delete[] loc_dist;
}

int print_result(bool neg, int *dist,int n)
{
    if(!neg)
    {
        for(int i = 0; i < n; i++)
        {
            if(dist[i] > INF)
                dist[i] = INF;
            printf("%d\n",dist[i]);
        }
    }
    else
        printf("FOUND NEGATIVE CYCLE!\n");
    return 0;
}

int main(int argc, char **argv)
{
    int i,total,j,curr_id,n,m,w,u,v,src;
    int *arr = NULL;
    int *dist = NULL;
    MPI_Init(&argc,&argv);
    MPI_Comm comm;
    bool neg = false;
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &curr_id);
    MPI_Comm_size(comm, &total);
    MPI_Barrier(comm);
    if(curr_id == 0)
    {
        // printf("Enter number of nodes and number of edges and source: ");
        // fflush(stdout);
        scanf("%d %d %d",&n,&m,&src);
        // int gr[n][n];
        // for(i=0;i<n;i++)
        //     for(j=0;j<n;j++)
        //         gr[i][j] = INF;
        // for(i=0;i<m;i++)
        // {
        //     scanf("%d %d %d",&u,&v,&w);
        //     gr[u][v] = w;
        // }
        arr = (int *)malloc(n*n*sizeof(int));
        for(i=0;i<n;i++)
            for(j=0;j<n;j++)
                scanf("%d",&arr[i*n+j]);
                // arr[i*n+j] = gr[i][j];
        printf("\n");

        // if(src != 0)
        //     for(j=0;j<n;j++)
        //         swap(arr, j, (src*n+j));
        dist = (int *)malloc(n * sizeof(int));
    }
    MPI_Barrier(comm);
    bellman(curr_id, total, comm, n, arr, dist, &neg);
    MPI_Barrier(comm);
    if(src != 0)
        swap(dist,0,src);
    print_result(neg,dist,n);
    MPI_Finalize();
    return 0;
}