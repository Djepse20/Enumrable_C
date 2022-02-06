#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#define FALSE 0
#define TRUE 1
typedef unsigned char BYTE;
typedef enum LIST_TYPES{
    ARR = 0,
    LIST = 1
}LIST_TYPES;





typedef struct array {
    BYTE* Arr;

    size_t Length;
    size_t ElementSize;
}array;


#define TYPEOF(x) _Generic((x), array*: ARR)


#define foreach_arr(TYPE,ITEM, ARRAY,BODY) \
    for(int count = 0, size = ARRAY->Length; count < size; count++)\
    {\
        TYPE* ITEM = (TYPE*)&(*((ARRAY->Arr)+count*ARRAY->ElementSize)); \
        do\
        BODY\
        while(0);\
    }
        
#define foreach_list(TYPE,ITEM, LIST,BODY)\
    for( ITEM = list->head; item != NULL; ITEM = ITEM->next)\
    {\
        do\
        BODY\
        while(0);\
    }



#define foreach(TYPE,item, Enumerable, BODY) \
    do\
    {\
        switch (TYPEOF(Enumerable))\
        {\
            case ARR:\
            foreach_arr(TYPE,item, Enumerable, BODY);\
            break;\
        }\
    }while(0)


#define NewArray(type,...) CreateArray(sizeof(*(type){0}),sizeof((type){0})/sizeof(*(type){0}), (BYTE*)(type){__VA_ARGS__},  (sizeof((int[]){__VA_ARGS__})/sizeof(int)))
int ChunkEmpty(BYTE* initValues,size_t ElementSize)
{
    for(size_t i =0;i<ElementSize;i++)
        if(initValues[i] != 0)
            return FALSE;

    return TRUE;
}
BYTE* FillElementsInArray(array* arr,BYTE* initValues, size_t numOfInits)
{

    if(numOfInits == 1 && ChunkEmpty(initValues,arr->ElementSize))
        return arr->Arr;
    for(size_t i = 0;i<arr->Length && i < numOfInits;i++)
        for(size_t j =0;j< arr->ElementSize;j++)        
            arr->Arr[arr->ElementSize*i+j] = initValues[arr->ElementSize*i+j];
    return arr->Arr;
}
array* CreateArray(size_t element_size, size_t length,BYTE* initValues, size_t numOfInits)
{
    array* arr = malloc(sizeof(arr));
    arr->Arr = calloc(length,element_size);    

    arr->Length = length;
    arr->ElementSize = element_size;

    arr->Arr = FillElementsInArray(arr,initValues,numOfInits);

    return arr;
}


int main(void)
{
    // creates new array of specified type and size, with a list of elements to initialize afterwards
    array* arr = NewArray(double[20],0);
    //foreach variable v of type in, in array, do this
    foreach(double,v,arr,
    {    
        printf(" %lf",*v);
    });
    return 0;
}

