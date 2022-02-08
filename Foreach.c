
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#define FALSE 0
#define TRUE 1
typedef unsigned char BYTE;
typedef enum List_TYPES{
    ARR = 0,
    LIST = 1
}List_TYPES;


#define XSTR(x) STR(x)
#define STR(x) #x


typedef struct _LinkedList _LinkedList;
typedef struct array {
        BYTE* data;
    
}array;
struct arrayimpl {
    array* impl;
};


struct _LinkedList {
    BYTE* data;
    _LinkedList* next;

};
struct LinkedListimpl 
{

    BYTE* curr;
    _LinkedList* impl;
};

typedef struct Enumerable{
    union 
    {
        struct arrayimpl* Arr;
        struct LinkedListimpl* List;
    };
    List_TYPES Type;
    size_t Length;
    size_t ElementSize;
    size_t count;
    int keep;
}Enumerable;






#define foreach(ITEM, ENUMERABLE) \
    for(ENUMERABLE->count = 0, ENUMERABLE->keep = TRUE; ENUMERABLE->keep && ENUMERABLE->count < ENUMERABLE->Length;ENUMERABLE->count++, ENUMERABLE->keep = !ENUMERABLE->keep)\
        for(ITEM = GetNextElement((ENUMERABLE),ENUMERABLE->count); ENUMERABLE->keep; ENUMERABLE->keep = FALSE)
         









void* GetNextElement(Enumerable*Enumerable,size_t count)
{
    BYTE* data;
    switch (Enumerable->Type)
    {
    case ARR:
        return &Enumerable->Arr->impl->data[count*Enumerable->ElementSize];
    case LIST:

        if(count == 0)
        {
            Enumerable->List->curr = (BYTE*)Enumerable->List->impl->next;
            return (void*)&(*(Enumerable->List->impl->data));
        }
        data = ((_LinkedList*)(&(*Enumerable->List->curr)))->data;
        Enumerable->List->curr = (BYTE*)((_LinkedList*)(&(*Enumerable->List->curr)))->next;
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

#define NewLinkedList(TYPE,...) CreateLinkedList(sizeof(*(TYPE){0}),sizeof(TYPE)/sizeof(*(TYPE){0}), (BYTE*)(TYPE){__VA_ARGS__},(sizeof((BYTE[]){__VA_ARGS__})/sizeof(BYTE)))
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
    FillArray(newArray,array,origLength,array->Arr->impl->data);

    if(elementsToInit  > 1 || !ChunkEmpty(elements,array->ElementSize))  
        FillArray(newArray+(array->ElementSize*(origLength-1)),array,elementsToInit,elements);


    free(array->Arr->impl->data);
    array->Arr->impl->data = newArray;
}


void* FillElementsInArray(Enumerable* arr,void* initValues, size_t numOfInits)
{
    if(numOfInits == 1 && ChunkEmpty(initValues,arr->ElementSize))
        return arr->Arr;
    return FillArray(arr->Arr->impl->data,arr,numOfInits,initValues);
}
Enumerable* CreateArray(size_t element_size, size_t length,BYTE* initValues, size_t numOfInits)
{
    
    Enumerable* arr = malloc(sizeof(Enumerable));
    arr->Arr = malloc(sizeof(array));
    arr->Arr->impl->data = calloc(length,element_size);    
    arr->Type = ARR;
    arr->Length = length;

    arr->ElementSize = element_size;
    
    arr->Arr->impl->data = FillElementsInArray(arr,initValues,numOfInits);

    return arr;
}
void * SepcialMemcpy(void* dest, void* src,size_t n)
{
    if(src == NULL)
        return memset(dest,0,n);

    return memcpy(dest,src,n);
}
void AddNewNode(Enumerable* list,void* data)
{
    _LinkedList* linkedList = (_LinkedList*)list->List->curr;
    _LinkedList* _newnode = malloc(sizeof(_LinkedList));
    _newnode->data = malloc(list->ElementSize);
    //copy data into new node
    SepcialMemcpy(_newnode->data,data,list->ElementSize);
    _newnode->next = NULL;
    list->List->curr = (BYTE*)_newnode;

    if(list->List->impl == NULL)
    {
        list->List->impl = _newnode;
        return;
    }
    //set the next node of the previous curr to new node
    linkedList->next = _newnode;
    //set the next of new node to null
    return;
}


_LinkedList* initLinkedList(Enumerable* list,BYTE* initValues, size_t numOfInits)
{
    for(size_t i =0;i<list->Length;i++)
    {
        if(i < numOfInits)
            AddNewNode(list,initValues+list->ElementSize*i);
        else
            AddNewNode(list,NULL);
    }

    return list->List->impl;
}

Enumerable* CreateLinkedList (size_t element_size, size_t length,BYTE* initValues, size_t numOfInits)
{
    Enumerable* list = malloc(sizeof(Enumerable));
    list->Length = length;
    list->Type = LIST;
    list->List->curr = NULL;
    list->List = NULL;
    list->ElementSize = element_size;
    list->List->impl = initLinkedList(list,initValues,numOfInits);
    return list;
}
#define indexof(ENUMERABLE,TYPE,VAR) indexof_arr(ENUMERABLE,(BYTE*)(TYPE[]){VAR})
inline int EnumerableCount(Enumerable* enumrable)
{
    return enumrable->count;
}
int indexof_arr(Enumerable* enumrable,BYTE* var)
{


    foreach(BYTE* value,enumrable)
    {
        if(memcmp(value,var,enumrable->ElementSize) == 0)
            return EnumerableCount(enumrable);
    }
    return -1;
}
#define GetValueAtIndex(ENUMRABLE,TYPE,INDEX) *(TYPE*)GetValueAtIndex_arr(ENUMRABLE,INDEX)
BYTE* GetValueAtIndex_arr(Enumerable* enumrable,int index)
{
    
    if(enumrable->Type != ARR)
        return 0;
    if(index > enumrable->Length-1)
        return 0;
    return &enumrable->Arr->impl->data[index*enumrable->ElementSize];
}

int main(void)
{
    // creates new array of specified TYPE and size, with a List of elements to initialize afterwards
    //ALWAYS USE Type[numElem] and not a Type*.
    //The latter will make an   of 8 bytes, the first numElem bytes.
    //using Type* will therfore as a consequence break foreach :) 
    // a fix that could (and proably should be implented would be the need to specify the type and size)
    Enumerable * arr = NewArray(int[20],1,2,3,4,5);
    //creates a new linked list that can be enumerated over
    Enumerable* List = NewLinkedList(int[20],1,2,3,4,5);
    printf("index of 2 is %d ",indexof(List,int,2));
    //foreach variable v of TYPE T in array, do this
    //InsertIntoEnumerable(arr,char[20],"test2");
        foreach(int* v, List)
            printf("%d",*v);  
        printf("\n");  
        foreach(int* v,arr)
           printf("%d",*v);
        printf("\n");
    return 0;
}

