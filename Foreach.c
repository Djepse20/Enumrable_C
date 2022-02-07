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


#define XSTR(x) STR(x)
#define STR(x) #x


typedef struct array {
    BYTE* Arr;

    size_t Length;
    size_t ElementSize;
}array;

#define ISCHAR(x) _Generic((x), char*: NOTVALID, default:x)
#define TYPEOF(x) _Generic((x), struct array*: ARR)


#define foreach_arr(ITEM, ARRAY,BODY) \
    for(int count = 0, size = ARRAY->Length; count < size; count++)\
    {\
        ITEM = (void*)&(*((ARRAY->Arr)+count*ARRAY->ElementSize)); \
        do\
        BODY\
        while(0);\
    }
        
#define foreach_list(ITEM, LIST,BODY)\
    for( void* item = list->head;  && item != NULL; item = item->next)\
    {\
        ITEM = item;\
        do\
        BODY\
        while(0);\
    }



#define foreach(item, Enumerable,BODY) \
    do\
    {\
        switch (TYPEOF(Enumerable))\
        {\
            case ARR:\
            foreach_arr(item, Enumerable,BODY)\
            \
            break;\
        }\
    }while(0)

#define InsertIntoEnumerable(Enumerable,TYPE,...)\
    do\
    {\
      switch (TYPEOF(Enumerable))\
      {\
            case ARR:\
            InsertIntoArr(Enumerable,sizeof((TYPE){0})/sizeof(*(TYPE){0}),(BYTE*)(TYPE){__VA_ARGS__}, (sizeof((BYTE[]){__VA_ARGS__})/sizeof(BYTE)));\
            break;\
      }  \
    } while (0)


#define NewArray(TYPE,...) CreateArray(sizeof(*(TYPE){0}),sizeof(TYPE)/sizeof(*(TYPE){0}), (BYTE*)(TYPE){__VA_ARGS__},(sizeof((BYTE[]){__VA_ARGS__})/sizeof(BYTE)))


int ChunkEmpty(BYTE* initValues,size_t ElementSize)
{
    for(size_t i =0;i<ElementSize;i++)
        if(initValues[i] != 0)
            return FALSE;

    return TRUE;
}
void* FillArray(void* dest,array* arr, size_t Elements,void* src)
{
    memcpy(dest,src,Elements*arr->ElementSize);
    return dest;
}
void InsertIntoArr(array* array,size_t NumOfElements,void* elements,size_t elementsToInit)
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


void* FillElementsInArray(array* arr,void* initValues, size_t numOfInits)
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
    // creates new array of specified TYPE and size, with a list of elements to initialize afterwards
    //ALWAYS USE Type[numElem] and not a Type*.
    //The latter will make an array of 8 bytes, the first numElem bytes.
    //using Type* will therfore as a consequence break foreach :) 
    // a fix that could (and proably should be implented would be the need to specify the type and size)
    array* arr = NewArray(char[20],"mus seee");
    //foreach variable v of TYPE T in array, do this
    
    
    foreach(char* v,arr, 
        printf("%c",*v);
    );
    return 0;
}

