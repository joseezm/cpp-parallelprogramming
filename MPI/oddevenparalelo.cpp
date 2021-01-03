#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <stdlib.h>
using namespace std;

#define MAX 1000

void printVector(int a[], int n){
    cout << endl;
    for(int i=0; i<n; i++){
        cout << a[i] << "-";
    }
    cout << endl;
}

int main(){
    int comm_sz;
    int my_rank;
    double start,finish, t;

    int n = MAX;
    int local_n;
    int* matrix = new int [MAX];
    int *recdata = new int[MAX];
    int *recdata2 = new int[MAX];


    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // ===================
    // INICIALIACION DE MATRIZ
    // ===================
    
    if(my_rank == 0){
        srand(time(NULL));
        for(int i=0; i<MAX; i++){
            matrix[i] = rand( ) % MAX;
        }
        //printVector(matrix,MAX);
        local_n = (n/comm_sz)+ n%comm_sz;
    }
    
    // ===================
    // INICIO DE ODD EVEN SORT
    // ===================
    start = MPI_Wtime();

    MPI_Bcast(&local_n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(matrix,local_n,MPI_INT,recdata,local_n,MPI_INT,0,MPI_COMM_WORLD);

    sort(recdata,recdata+local_n);
    // printVector(recdata,local_n);
    
    int odd_rank, even_rank;
    if(my_rank % 2 ==0){
        odd_rank = my_rank -1;
        even_rank = my_rank +1;
    }
    else{
        odd_rank = my_rank+1;
        even_rank = my_rank-1;
    }

    if (odd_rank == -1 || odd_rank == comm_sz)
        odd_rank = MPI_PROC_NULL;
    if (even_rank == -1 || even_rank == comm_sz)
        even_rank = MPI_PROC_NULL;

    MPI_Status status;
    for(int p=0; p<comm_sz-1;p++){
        if(p%2==1)
            MPI_Sendrecv(recdata,local_n,MPI_INT,odd_rank,1,recdata2,local_n,MPI_INT,odd_rank,1,MPI_COMM_WORLD,&status);
        else
            MPI_Sendrecv(recdata,local_n,MPI_INT,even_rank,1,recdata2,local_n,MPI_INT,even_rank,1,MPI_COMM_WORLD,&status);
    

        int *temp= new int[local_n];
        for(int i=0; i<local_n; i++){
            temp[i] = recdata[i];
        }

        int i,j,k;
        if(status.MPI_SOURCE==MPI_PROC_NULL)	continue;
        else if(my_rank<status.MPI_SOURCE){
            for( i=j=k=0; k<local_n; k++){
                if(j==local_n || (i<local_n && temp[i]<recdata2[j]))
                    recdata[k] = temp[i++];
                else
                    recdata[k] = recdata2[j++];
            }
        }
        else{
            for( i=j=k=local_n -1; k>=0; k--){
                if(j==-1 || (i>=0 && temp[i]>=recdata2[j]))
                    recdata[k] = temp[i--];
                else
                    recdata[k] = recdata2[j--];
            }
        }
    }

    MPI_Gather(recdata,local_n,MPI_INT,matrix,local_n,MPI_INT,0,MPI_COMM_WORLD);
    

    finish = MPI_Wtime();
    t = finish - start;
    double tMax;
    MPI_Reduce(&t, &tMax, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    // ===================
    // FIN DE ODD EVEN SORT
    // ===================

    if(my_rank == 0){
        //printVector(matrix,n);
        cout << "\nTIEMPO: " << tMax<<"\n";
    }


    MPI_Finalize();
}