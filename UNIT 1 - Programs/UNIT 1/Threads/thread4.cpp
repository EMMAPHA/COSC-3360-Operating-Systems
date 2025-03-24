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

void *fillMatrix(void *arg_void_ptr)
{

	struct arguments *arg_ptr = (struct arguments *)arg_void_ptr;
	for(int i=0;i<arg_ptr->rowSize;i++)
		for(int j=0;j<arg_ptr->colSize;j++)
			arg_ptr->matrix[i][j] = i*arg_ptr->colSize + j;
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
	
	row[1]=2;
	col[1]=5;        
        b=new double*[row[1]];
        for(int i=0;i<row[1];i++)
                b[i]=new double[col[1]];


	for(int i=0;i<2;i++)
	{
		arg[i].rowSize=row[i];
		arg[i].colSize=col[i];
		if (i==0)
			arg[i].matrix = a;
		else
			arg[i].matrix = b;

		if(pthread_create(&tid[i], NULL, fillMatrix, &arg[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < 2; i++)
        	pthread_join(tid[i], NULL);
	
	for(int m=0;m<2;m++)
	{
		cout << "Matrix " << m+1 << endl;
		for (int i=0;i<row[m];i++)
		{
			for(int j=0;j<col[m];j++)
				if (m==0)
					cout << a[i][j] << " ";
				else
					cout << b[i][j] << " ";
			cout << endl;
		}
	}
	for(int i=0;i<row[0];i++)
        delete a[i];
	delete[] a;
	for(int i=0;i<row[1];i++)
        delete b[i];
	delete[] b;
	return 0;
}
