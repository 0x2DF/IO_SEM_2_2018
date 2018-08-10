#include <stdio.h>
#define N 5

int main()
{
  int D[N][N] = {0};
  int P[N][N] = {0};
  int t, i, j, k;

  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
    {
      scanf("%d",&D[i][j]);
    }
  }

  for (k = 0; k < N; ++k)
  {
    for (i = 0; i < N; ++i)
    {
      if (i != k)
      {
        for (j = 0; j < N; ++j)
        {
          if ((j != k) && (j != i))
          {
            t = D[i][k] + D[k][j];
            if (t < D[i][j])
            {
              P[i][j] = k+1;
              D[i][j] = t;
            }
          }
        }
      }
    }
  }
  printf("Tabla D\n");  
  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
    {
      printf("%d ", D[i][j]);
    }
    printf("\n");
  }

  printf("\nTabla P\n");
  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
    {
      printf("%d ", P[i][j]);
    }
    printf("\n");
  }


  return 0;
}
