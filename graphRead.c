#include <stdio.h>
#include <stdlib.h>

int main ()
{
    int N,i, j, lineLen,buffer;
    FILE * pFile;
  
    pFile = fopen ("neigVec.txt","r");

    //The 1st integer gives the number of vertices
    //it is alone in the 1st line of the file
    fscanf (pFile, "%i", &N);

    int** Nvec = (int**) malloc(N* sizeof(int*));
  
    for (j=0; j<N; j++)
    {
        //The 1st integer of each line gives the number of neighbours.
        fscanf (pFile, "%i", &lineLen);

        //here we allocate only enough memory to save all the neighbours
        Nvec[j] = (int*) malloc((lineLen+1)*sizeof(int));


        //VERY IMPORTANT
        //The 0 element saves the number of neighbours so we can loop only over the right amount of memory
        Nvec[j][0] = lineLen;
        for (i=1;i<=Nvec[j][0]; i++)
        {
            fscanf (pFile, "%i", &buffer);
            Nvec[j][i] = buffer;
        }
    }

    //This just prints the contents of Nvec
    printf("The graph has %i vertices\n\nvertex : neighbours",N);
    for (j=0; j<N; j++)
    {
        printf("\n%i:",j);
        for (i=1;i<=Nvec[j][0]; i++)
        {
            printf(" %i",Nvec[j][i]);
        }
    }
    printf("\n");

    fclose (pFile);
    free (Nvec);
    return 0;
}
