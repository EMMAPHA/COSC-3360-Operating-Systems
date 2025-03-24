#include <pthread.h>
#include <iostream>
#include <unistd.h>

using namespace std;

struct arguments
{
	int rowSize;
	int colSize;
	double **matrix;
};

void *print_matrix(void *arg_void_ptr)
{

	struct arguments *arg_ptr = (struct arguments *)arg_void_ptr;
	cout << "Size = (" << arg_ptr->rowSize << ", " << arg_ptr->colSize << ")" << endl;
	for(int i=0;i<arg_ptr->rowSize;i++)
	{
		for(int j=0;j<arg_ptr->colSize;j++)
			cout << arg_ptr->matrix[i][j] << " ";
		cout << endl;
        }
	return NULL;
}

int main()
{
	double **a;
	double **b;
	static struct arguments arg[2];
 	pthread_t tid[2];
	int row[2];
	int col[2];

	row[0]=2;
	col[0]=3;
	a=new double*[row[0]];
	for(int i=0;i<row[0];i++)
		a[i]=new double[col[0]];
	
	for (int i=0; i< row[0];i++)
		for(int j=0;j<col[0];j++)
			a[i][j]=i*col[0]+j;
	row[1]=2;
	col[1]=5;        
        b=new double*[row[1]];
        for(int i=0;i<row[1];i++)
                b[i]=new double[col[1]];

        for (int i=0; i< row[1];i++)
                for(int j=0;j<col[1];j++)
                        b[i][j]=i*col[1]+j;

	for(int i=0;i<2;i++)
	{
		arg[i].rowSize=row[i];
		arg[i].colSize=col[i];
		if (i==0)
			arg[i].matrix = a;
		else
			arg[i].matrix = b;

		if(pthread_create(&tid[i], NULL, print_matrix, &arg[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}
		sleep(1);		
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < 2; i++)
        	pthread_join(tid[i], NULL);
	for(int i=0;i<row[0];i++)
        delete a[i];
	delete[] a;
	for(int i=0;i<row[1];i++)
        delete b[i];
	delete[] b;
	return 0;
}
