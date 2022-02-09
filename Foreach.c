
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



typedef struct Enumerable{

    List_TYPES Type;
    size_t Length;
    size_t ElementSize;
    size_t count;
    int keep;
}Enumerable;

typedef struct ConcreteArray {
        BYTE* Data;

}ConcreteArray;
typedef struct Array {
    Enumerable* Enumerable;
    ConcreteArray* Array;
}Array;


typedef struct List {
    BYTE* Data;
    struct List* Next;

}List;

typedef struct LinkedList 
{

    List* Curr;
    Enumerable* Enumerable;
    List* List;
    
}LinkedList;








#define foreach(ITEM, ENUMERABLE) \
    for(ENUMERABLE->Enumerable->count = 0, ENUMERABLE->Enumerable->keep = TRUE; ENUMERABLE->Enumerable->keep && ENUMERABLE->Enumerable->count < ENUMERABLE->Enumerable->Length;ENUMERABLE->Enumerable->count++, ENUMERABLE->Enumerable->keep = !ENUMERABLE->Enumerable->keep)\
        for(ITEM = GetNextElement((ENUMERABLE),ENUMERABLE->Enumerable->count,ENUMERABLE->Enumerable); ENUMERABLE->Enumerable->keep; ENUMERABLE->Enumerable->keep = FALSE)
         




void* GetNextElement_Array(Array* array,size_t count)
{
    return &array->Array->Data[count*array->Enumerable->ElementSize];
}

void* GetNextElement_List(LinkedList* List,size_t count)
{
    BYTE* Data;
    if(count == 0)
        {
            List->Curr = List->List->Next;
            return (void*)&(*(List->List->Data));
        }
        Data = (&(*List->Curr))->Data;
        List->Curr =(&(*List->Curr))->Next;
        return (void*)&(*(Data));
}


void* GetNextElement(void* Collection,size_t count, Enumerable* Enumerable)
{

    switch (Enumerable->Type)
    {
    case ARR:
        return GetNextElement_Array(Collection, count);
    case LIST:
        return GetNextElement_List(Collection, count);
        break;
    }
    return NULL;
}


#define InsertIntoEnumerable(ENUMERABLE,TYPE,...)\
    do\
    {\
      switch (ENUMERABLE->Enumerable->Type)\
      {\
            case ARR:\
            InsertIntoArr(ENUMERABLE,sizeof((TYPE){0})/sizeof(*(TYPE){0}),(BYTE*)(TYPE){__VA_ARGS__}, (sizeof((BYTE[]){__VA_ARGS__})/sizeof(BYTE)));\
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
void* FillArray(void* dest,Array* arr, size_t Elements,void* src)
{
    
    memcpy(dest,src,Elements*arr->Enumerable->ElementSize);
    return dest;
}
void InsertIntoArr(Array* array,size_t NumOfElements,void* elements,size_t elementsToInit)
{       

    size_t sizeOfArr = array->Enumerable->Length*array->Enumerable->ElementSize;
    BYTE* newArray = calloc(sizeOfArr+NumOfElements,array->Enumerable->ElementSize);
    size_t origLength = array->Enumerable->Length;
    array->Enumerable->Length += NumOfElements;
    FillArray(newArray,array,origLength,array->Array->Data);

    if(elementsToInit  > 1 || !ChunkEmpty(elements,array->Enumerable->ElementSize))  
        FillArray(newArray+(array->Enumerable->ElementSize*(origLength-1)),array,elementsToInit,elements);


    free(array->Array->Data);
    array->Array->Data = newArray;
}

void * SepcialMemcpy(void* dest, void* src,size_t n)
{
    if(src == NULL)
        return memset(dest,0,n);

    return memcpy(dest,src,n);
}
void AddNode(LinkedList* list,List** Current,void* data)
{
    List* _newnode = malloc(sizeof(List));

    _newnode->Data = malloc(list->Enumerable->ElementSize);
    //copy Data into new node
    SepcialMemcpy(_newnode->Data,data,list->Enumerable->ElementSize);
    _newnode->Next = (*Current)->Next;


    //set the Next node of the previous Curr to new node
    printf("%d\n",(int*)_newnode->Data);
    (*Current)->Next = _newnode;
    (*Current) = _newnode;
    //set the Next of new node to null
}
int AddNewElements(LinkedList* list,List* start,size_t numOfElements,BYTE* elements ,size_t elementsToInit)
{

    size_t count = 0;
    for(count = 0;count < numOfElements;count++)
    {
  
        if(count < elementsToInit)
            AddNode(list,&start,elements+list->Enumerable->ElementSize*count);
        else
            AddNode(list,&start,NULL);
    }
    return count;

}
int InsertIntoEnumerable_List(LinkedList* list,size_t index,size_t numOfElements,BYTE* elements,size_t elementsToInit)
{
    List* current = list->List;
    if(index > list->Enumerable->Length-1) 
        return 0;
    size_t count= 0;
    
    while(current != NULL)
    {

        printf("sus: %d\n",index);
        if(index == count)
        {
            AddNewElements(list,current,numOfElements,elements,elementsToInit);
            list->Enumerable->Length += numOfElements;
            return numOfElements;
        }

        count++;
 
        current = current->Next;
  
    }
    return 0;
}
#define InsertIntoList(ENUMERABLE,INDEX,TYPE,...) InsertIntoEnumerable_List(ENUMERABLE,INDEX,sizeof((TYPE){0})/sizeof(*(TYPE){0}),(BYTE*)(TYPE){__VA_ARGS__},(sizeof((BYTE[]){__VA_ARGS__})/sizeof(BYTE)));

void* FillElementsInArray(Array* arr,void* initValues, size_t numOfInits)
{
    if(numOfInits == 1 && ChunkEmpty(initValues,arr->Enumerable->ElementSize))
        return arr->Array;
    return FillArray(arr->Array->Data,arr,numOfInits,initValues);
}
Array* CreateArray(size_t element_size, size_t length,BYTE* initValues, size_t numOfInits)
{

    Array* arr = malloc(sizeof(Array));
    arr->Enumerable = malloc(sizeof(Enumerable));
    arr->Array = malloc(sizeof(ConcreteArray));
    arr->Array->Data = calloc(length,element_size);   

    arr->Enumerable->Type = ARR;
    arr->Enumerable->Length = length;

    arr->Enumerable->ElementSize = element_size;
    
    arr->Array->Data = FillElementsInArray(arr,initValues,numOfInits);

    return arr;
}


void AddNewNode(LinkedList* list,void* Data)
{
    List* linkedList = (List*)list->Curr;

  
    List* _newnode = malloc(sizeof(List));

    _newnode->Data = malloc(list->Enumerable->ElementSize);
    //copy Data into new node
    SepcialMemcpy(_newnode->Data,Data,list->Enumerable->ElementSize);
    _newnode->Next = NULL;
    list->Curr = _newnode;

    if(list->List == NULL)
    {
        list->List = _newnode;
        return;
    }
    //set the Next node of the previous Curr to new node
    linkedList->Next = _newnode;
    //set the Next of new node to null
}


List* initLinkedList(LinkedList* list,BYTE* initValues, size_t numOfInits)
{
    for(size_t i =0;i<list->Enumerable->Length;i++)
    {
        if(i < numOfInits)
            AddNewNode(list,initValues+list->Enumerable->ElementSize*i);
        else
            AddNewNode(list,NULL);
    }

    return list->List;
}

LinkedList* CreateLinkedList (size_t element_size, size_t length,BYTE* initValues, size_t numOfInits)
{
    LinkedList* list = malloc(sizeof(LinkedList));
    list->List = malloc(sizeof(List));
    list->Enumerable = malloc(sizeof(Enumerable));
    list->Enumerable->Length = length;
    list->Enumerable->Type = LIST;
    list->Curr = NULL;
    list->List = NULL;

    list->Enumerable->ElementSize = element_size;
    list->List = initLinkedList(list,initValues,numOfInits);

    return list;
}


#define indexof(COLLECTION,TYPE,VAR) _indexof(COLLECTION,(BYTE*)(TYPE[]){VAR},COLLECTION->Enumerable)

int _EnumerableCount(Enumerable* enumerable)
{
    return enumerable->count;
}

#define EnumerableCount(ENUMERABLE) _EnumerableCount(ENUMERABLE->Enumerable)

int indexof_Arr(Array* array,BYTE* var)
{
    foreach(BYTE* value,array)
    {
        if(memcmp(value,var,array->Enumerable->ElementSize) == 0)
            return EnumerableCount(array);
    }
    return -1;
}
int IndexOf_List(LinkedList* list,BYTE* var)
{
    foreach(BYTE* value,list)
    {
        if(memcmp(value,var,list->Enumerable->ElementSize) == 0)
            return EnumerableCount(list);
    }
    return -1;
}

int _indexof(void* Collection,BYTE* var,Enumerable* enumrable)
{
    switch(enumrable->Type)
    {
        case ARR:
            return indexof_Arr(Collection,var);
        case LIST:
            return IndexOf_List(Collection,var);
    }
    return -1;
}


#define GetValueAtIndex(ENUMRABLE,TYPE,INDEX) *(TYPE*)GetValueAtIndex_arr(ENUMRABLE,INDEX)
BYTE* GetValueAtIndex_arr(Array* arr,int index)
{
    

    if(index > arr->Enumerable->Length-1)
        return 0;
    return &arr->Array->Data[index*arr->Enumerable->ElementSize];
}

int main(void)
{
    // creates new array of specified TYPE and size, with a List of elements to initialize afterwards
    //ALWAYS USE Type[numElem] and not a Type*.
    //The latter will make an   of 8 bytes, the first numElem bytes.
    //using Type* will therfore as a consequence break foreach :) 
    // a fix that could (and proably should be implented would be the need to specify the type and size)
    Array* arr = NewArray(int[20],1,2,3,4,5);

    //creates a new linked list that can be enumerated over
    LinkedList* List = NewLinkedList(int[20],1,2,3,4,5);

    printf("index of 2 is %d ",indexof(List,int,2));
    //foreach variable v of TYPE T in array, do this
    InsertIntoEnumerable(arr,int[20],1,2,3,4);
    InsertIntoList(List,2,int[20],3,6,7,8);

        foreach(int* v, List)
            printf("%d",*v);   
        printf("\n");
        foreach(int* v,arr)
           printf("%d",*v);
        printf("\n");
    return 0;
}

