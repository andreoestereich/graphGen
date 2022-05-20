#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#include "ran2.h"

int sign(int *x) {
        return (*x > 0) - (*x < 0);
}

int compare (const void * a, const void * b) {
      return ( *(int*)a - *(int*)b );
}

typedef struct Lista list;
struct Lista {
    int allocated;
    int leng;
    int *list;
};

list initList(int size) {
    list lista;
    lista.allocated = size;
    lista.leng = 0;
    lista.list = (int*) malloc(size* sizeof(int));
    if (!lista.list) printf("MEMORY ERROR!\n");
    return lista;
}

void resizeList(list *lista, int size) {
    //printf("List resized\n");
    lista->allocated += size;
    int *new_list = (int*) realloc(lista->list, lista->allocated* sizeof(int));
    if (!new_list)
    {
        printf("NO MEMORYYYY\n\n\n");
    }
    lista->list = new_list;
}

//this implements a binary search to find the index to which the new value should be appended
int findApIndex(list *lista, int *value) {
    int low,high,mid;
    low = 0;
    high = lista->leng-1;
    mid = (high + low)/2;
    while (high > low)
    {
        if (lista->list[mid] > *value)
            high = mid;
        else
            low = mid+1;
        mid = (high + low)/2;
    }
    return mid;
}

int finddpIndex(double *lista, int *size, double *value) {
    int low,high,mid;
    low = 0;
    high = *size -1;
    mid = (high + low)/2;
    while (high > low)
    {
        if (lista[mid] > *value)
            high = mid;
        else
            low = mid+1;
        mid = (high + low)/2;
    }
    return mid;
}

int isInList(list *lista, int value) {
    char *pItem;
    pItem = (char*) bsearch (&value, lista->list, lista->leng, sizeof(int), compare);
    if (pItem == NULL) return 0;
    else return 1;
}

void appendList(list *lista, int value) {
    //checks if the allocated space is enough
    //printf("\nleng = %i allocated = %i\n",lista->leng, lista->allocated);
    if (lista->leng >= lista->allocated - 1) resizeList(lista, 10);
    //finds where the new value should be appended
    if (lista->leng > 0 && lista->list[lista->leng-1] > value)
    {
        int index = findApIndex(lista, &value);
        //shifts all the bigger values
        //printf("index = %i old %i new %i\n",index,lista->list[index],value);
        for (int i=lista->leng;i>index;i--) {
            lista->list[i] = lista->list[i-1];
        }
        lista->list[index] = value;
    }
    else
        lista->list[lista->leng] = value;

    lista->leng++;
    //qsort (lista->list, lista->leng, sizeof(int), compare);
}

list* randomGraph(int N, float k, struct ran_state *state) {
    int i,j,node1,node2,count,nCount;
    list* Nvec;
    Nvec = (list*) malloc(sizeof(list)* N);
    int size = ((int) k)+1;
    //printf("Beginning %i\n",size);
    for (i=0;i<N;i++) Nvec[i] = initList(size);

    //printf("Beginning 0.5\n");
    int auxN[N];
    for (i=0;i<N;i++) auxN[i] = 0;

    nCount = 0;
    //printf("Beginning 2\n");
    int max = (int) N*k/2.;
    while (nCount< max)
    {
        node1 = ran2int(0,N,state);
        //if (node1>=N || node1<0) printf("node1 too big\n");
        //printf("node1 = %i\n",node1);
        //reinitializes the options
        count = 0;
        for (i=0;i<N;i++)
        {
            //check if connection is availiable
            if (i != node1 && (! isInList(&Nvec[node1], i)))
            {
                //if (count>=N || count<0) printf("count too big\n");
                //printf("before auxN.");
                auxN[count] = i;
                //printf("after auxN.");
                count++;
            }
        }
        if (count > 0)
        {
            int index2 = ran2int(0,count,state);
            //if (index2>=N || index2<0) printf("index2 too big\n");
            //printf("before index2 %i / %i",index2,count);
            node2 = auxN[index2];
            //printf("node1=%i node2=%i \n",node1,node2);
            appendList(&Nvec[node1],node2);
            //printf("between appendList.");
            appendList(&Nvec[node2],node1);
            nCount++;
        }
        //printf("Node loop ended. %i of %i\n",nCount, max);
    }
    return Nvec;
}

list* randomModularGraph(int N, int n1, float k, float mu, struct ran_state *state) {
    int i,j,node1,node2,count,nCount;
    list* Nvec;
    Nvec = (list*) malloc(sizeof(list)* N);
    for (i=0;i<N;i++) Nvec[i] = initList(((int) k)+1);
    int auxN[N];

    nCount = 0;
    while (nCount<(int) N*(1.-mu)*k/2.)
    {
        node1 = ran2int(0,N,state);
        if (node1 < n1)
        {
            //reinitializes the options
            for (i=0;i<n1;i++) auxN[i] = 0;
            count = 0;
            for (i=0;i<n1;i++)
            {
                //check if connection is availiable
                if (i != node1 && (! isInList(&Nvec[node1], i)))
                {
                    auxN[count] = i;
                    count++;
                }
            }
            if (count > 0)
            {
                node2 = auxN[ran2int(0,count,state)];
                appendList(&Nvec[node1],node2);
                appendList(&Nvec[node2],node1);
                nCount++;
            }
        }
        else
        {
            //reinitializes the options
            for (i=n1;i<N;i++) auxN[i] = 0;
            count = 0;
            for (i=n1;i<N;i++)
            {
                //check if connection is availiable
                if (i != node1 && ! isInList(&Nvec[node1], i))
                {
                    auxN[count] = i;
                    count++;
                }
            }
            if (count > 0)
            {
                node2 = auxN[ran2int(0,count,state)];
                appendList(&Nvec[node1],node2);
                appendList(&Nvec[node2],node1);
                nCount++;
            }
        }
    }
    nCount = 0;
    while (nCount<(int) N*mu*k/2.)
    {
        node1 = ran2int(0,N,state);
        if (node1 > n1)
        {
            //reinitializes the options
            for (i=0;i<n1;i++) auxN[i] = 0;
            count = 0;
            for (i=0;i<n1;i++)
            {
                if (! isInList(&Nvec[node1], i))
                {
                    auxN[count] = i;
                    count++;
                }
            }
            if (count > 0)
            {
                node2 = auxN[ran2int(0,count,state)];
                appendList(&Nvec[node1],node2);
                appendList(&Nvec[node2],node1);
                nCount++;
            }
        }
        else
        {
            //reinitializes the options
            for (i=n1;i<N;i++) auxN[i] = 0;
            count = 0;
            for (i=n1;i<N;i++)
            {
                if (! isInList(&Nvec[node1], i))
                {
                    auxN[count] = i;
                    count++;
                }
            }
            if (count > 0)
            {
                node2 = auxN[ran2int(0,count,state)];
                appendList(&Nvec[node1],node2);
                appendList(&Nvec[node2],node1);
                nCount++;
            }
        }
    }
    return Nvec;
}

list* fromFile(char *fileName) {
    int N,i, j, lineLen,buffer;
    FILE * pFile;

    pFile = fopen ("neigVec.txt","r");

    //The 1st integer gives the number of vertices
    //it is alone in the 1st line of the file
    fscanf (pFile, "%i", &N);

    list* Nvec;
    Nvec = (list*) malloc(sizeof(list)* N);

    for (j=0; j<N; j++)
    {
        //The 1st integer of each line gives the number of neighbours.
        fscanf (pFile, "%i", &lineLen);
        //here we initialize with enough memory to save all the neighbours
        Nvec[i] = initList(lineLen);

        for (i=1;i<=lineLen; i++)
        {
            fscanf (pFile, "%i", &buffer);
            appendList(&Nvec[j], buffer);
        }
    }
    fclose (pFile);
    return Nvec;
}

int main ()
{
    // enter the parallel region just as an example
    #pragma omp parallel
    {
        //this run state is very important since ran2 is not parallel safe on its own
        struct ran_state state = {-time(NULL)*(1+omp_get_thread_num()), 123456789, 0, {0} };
        //calls the setup
        #pragma omp for
        for (int i=0;i<10;i++)
        {
            printf("\n");
        }
    }
    return 0;
}
