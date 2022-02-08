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

typedef struct list list;
typedef struct array {
    BYTE* data;
}array;

struct list {
    BYTE* data;
    list* next;
    list* curr;
};
typedef struct Enumerable{
    union 
    {
        array* Arr;
        list* list;
    };
    LIST_TYPES Type;
    size_t Length;
    size_t ElementSize;
}Enumerable;



#define ISCHAR(x) _Generic((x), char*: NOTVALID, default:x)



#define foreach(ITEM, ENUMERABLE) \
    for(int count = 0, size = ENUMERABLE->Length, keep = TRUE; keep && count < size; count++, keep = !keep)\
        for(ITEM = GetNextElement((ENUMERABLE),count); keep; keep = FALSE)
         



void* GetNextElement(Enumerable*Enumerable,size_t count)
{
    BYTE* data;
    switch (Enumerable->Type)
    {
    case ARR:
        return (void*)&(*((Enumerable->Arr->data)+count*Enumerable->ElementSize));
    case LIST:
    //YES
        if(count == 0)
        {
            Enumerable->list->curr = Enumerable->list->next;
            return (void*)&(*(Enumerable->list->data));
        }
        data = Enumerable->list->curr->data;
        Enumerable->list->curr = Enumerable->list->curr->next;
        return (void*)&(*(data));
        break;
    }
    return NULL;
}
#define InsertIntoEnumerable(Enumerable,TYPE,...)\
    do\
    {\
      switch (Enumerable->Type)\
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
void* FillArray(void* dest,Enumerable* arr, size_t Elements,void* src)
{
    
    memcpy(dest,src,Elements*arr->ElementSize);
    return dest;
}
void InsertIntoArr(Enumerable* array,size_t NumOfElements,void* elements,size_t elementsToInit)
{       

    size_t sizeOfArr = array->Length*array->ElementSize;
    BYTE* newArray = calloc(sizeOfArr+NumOfElements,array->ElementSize);
    size_t origLength = array->Length;
    array->Length += NumOfElements;
    FillArray(newArray,array,origLength,array->Arr->data);

    if(elementsToInit  > 1 || !ChunkEmpty(elements,array->ElementSize))  
        FillArray(newArray+(array->ElementSize*(origLength-1)),array,elementsToInit,elements);


    free(array->Arr->data);
    array->Arr->data = newArray;
}


void* FillElementsInArray(Enumerable* arr,void* initValues, size_t numOfInits)
{
    if(numOfInits == 1 && ChunkEmpty(initValues,arr->ElementSize))
        return arr->Arr;
    return FillArray(arr->Arr->data,arr,numOfInits,initValues);
}
Enumerable* CreateArray(size_t element_size, size_t length,BYTE* initValues, size_t numOfInits)
{
    
    Enumerable* arr = malloc(sizeof(Enumerable));
    arr->Arr = malloc(sizeof(Enumerable));
    arr->Arr->data = calloc(length,element_size);    
    arr->Type = ARR;
    arr->Length = length;
    arr->ElementSize = element_size;
    
    arr->Arr->data = FillElementsInArray(arr,initValues,numOfInits);

    return arr;
}


int main(void)
{
    // creates new array of specified TYPE and size, with a list of elements to initialize afterwards
    //ALWAYS USE Type[numElem] and not a Type*.
    //The latter will make an   of 8 bytes, the first numElem bytes.
    //using Type* will therfore as a consequence break foreach :) 
    // a fix that could (and proably should be implented would be the need to specify the type and size)
    Enumerable* arr = NewArray(char[20],"test1");
    
    //foreach variable v of TYPE T in array, do this
    InsertIntoEnumerable(arr,char[20],"test2");
    foreach(char* v,arr)
        printf("%c",*v);
    return 0;
}

