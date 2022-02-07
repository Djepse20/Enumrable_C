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


#define TYPEOF(x) _Generic((x), struct array*: ARR)


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

#define InsertIntoEnumerable(Enumerable,type,...)\
    do\
    {\
      switch (TYPEOF(Enumerable))\
      {\
            case ARR:\
            InsertIntoArr(Enumerable,sizeof((type){0})/sizeof(*(type){0}),(BYTE*)(type){__VA_ARGS__}, (sizeof((int[]){__VA_ARGS__})/sizeof(int)));\
            break;\
      }  \
    } while (0)
    
#define NewArray(type,...) CreateArray(sizeof(*(type){0}),sizeof((type){0})/sizeof(*(type){0}), (BYTE*)(type){__VA_ARGS__},  (sizeof((int[]){__VA_ARGS__})/sizeof(int)))
int ChunkEmpty(BYTE* initValues,size_t ElementSize)
{
    for(size_t i =0;i<ElementSize;i++)
        if(initValues[i] != 0)
            return FALSE;

    return TRUE;
}
BYTE* FillArray(BYTE* dest,array* arr, size_t Elements,BYTE* src)
{
        for(size_t i = 0;i<arr->Length && i < Elements;i++)
            for(size_t j =0;j< arr->ElementSize;j++)        
                dest[arr->ElementSize*i+j] = src[arr->ElementSize*i+j];
    return dest;
}
void InsertIntoArr(array* array,size_t NumOfElements,BYTE* elements,size_t elementsToInit)
{       

    size_t sizeOfArr = array->Length*array->ElementSize;
    BYTE* newArray = calloc(sizeOfArr+NumOfElements,array->ElementSize);
    size_t origLength = array->Length;
    array->Length += NumOfElements;
    FillArray(newArray,array,origLength,array->Arr);

    if(elementsToInit  > 1 || !ChunkEmpty(elements,array->ElementSize))  
        FillArray(newArray+(array->ElementSize*(origLength-1)),array,elementsToInit,elements);

    free(array->Arr);
    array->Arr = newArray;


}


BYTE* FillElementsInArray(array* arr,BYTE* initValues, size_t numOfInits)
{

    if(numOfInits == 1 && ChunkEmpty(initValues,arr->ElementSize))
        return arr->Arr;

    return FillArray(arr->Arr,arr,numOfInits,initValues);
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
    array* arr = NewArray(double[20],2,3);
    //foreach variable v of type in, in array, do this

    InsertIntoEnumerable(arr,double[20],0,1);
    printf("%d",arr->Length);

    foreach(double,v,arr,
    {    
        printf(" %lf",*v);
    });
    return 0;
}

