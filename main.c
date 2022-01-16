#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INCREMENT 1024
#define INT_ALLOC 32
#define DOUBLE_ALLOC 32

enum ValueType {INTEGER = 0, DOUBLE = 1, STRING = 2, LIST = 3};

int notPrimitiveValue(enum ValueType type) {
    if (type != INTEGER && type != DOUBLE) {
        return 1;
    }
    return 0;
}

char null[] = "NULL, ";

struct Converter {
    int allocSize;
    char format[12];
};

typedef struct Converter Converter;
Converter converterHelper[] = {
        {.allocSize = INT_ALLOC, .format = "%d, "},
        {.allocSize = DOUBLE_ALLOC, .format = "%f, "},
        {.allocSize = -1, .format = "'%s', "},
        {.allocSize = INCREMENT, .format = ""}
};

struct List {
    enum ValueType type;
    void* value;
    struct List* next;
};

typedef struct List List;

char* toStringInt(void* intValue, int* returnedSize) {
    char* result = (char*) malloc(converterHelper[INTEGER].allocSize * sizeof (char));
    if (result == NULL) {
        return NULL;
    }
    sprintf(result, converterHelper[INTEGER].format, *((int*) intValue));
    return result;
}
char* toStringDouble(void* doubleValue, int* returnedSize) {
    char* result = (char*) malloc(converterHelper[DOUBLE].allocSize * sizeof (char));
    if (result == NULL) {
        return NULL;
    }
    sprintf(result, converterHelper[DOUBLE].format, *((int*) doubleValue));
    return result;
}

char* toStringString(void* stringValue, int* returnedSize) {
    int allocSize = strlen((char*) stringValue) + 4;
    char* result = (char*) malloc(allocSize * sizeof (char));
    if (result == NULL) {
        return NULL;
    }
    sprintf(result, converterHelper[STRING].format, (char*)stringValue);
    return result;
}

char* convertToString(List* list, int* returnedSize);
char* toStringSimple(List* list);
char* toString(List* list, int* stringSize);


List* createNode(void* value, enum ValueType type) {
    List* result = (List*) malloc(sizeof (List));
    result->type = type;
    result->value = value;
    result->next = NULL;
}

List* createNodeInt(int value) {
    int* container = (int*) malloc(sizeof(int));
    if (container == NULL) {
        return NULL;
    }
    *container = value;
    return createNode((void*)container, INTEGER);
}

List* createNodeString(char* str) {
    char* container = (char*) malloc(strlen(str) * sizeof(char));
    if (container == NULL) {
        return NULL;
    }
    container[0] = '\0';
    container = strcat(container, str);
    return createNode((void*)container, STRING);
}

List* createNodeDouble(double value) {
    double* container = (double*) malloc(sizeof(double));
    if (container == NULL) {
        return NULL;
    }
    *container = value;
    return createNode((void*)container, DOUBLE);
}

List* addToTail(List* list, List* node) {
    if (list == NULL) {
        return node;
    }
    List* iterator = list;
    while (iterator->next != NULL) {
        iterator = iterator->next;
    }
    iterator->next = node;
}

void destroyNode(List* node);
void destroyList(List* list);

char* CARString(List* list, int* returnedValue) {
    return convertToString(list, returnedValue);
}

char* CDRString(List* list, int* returnedValue) {
    return toString(list, returnedValue);
}

List* CAR(List* list) {
    return createNode(list->value, list->type);
}

List* CDR(List* list) {
    return list->next;
}

int main() {
    List* myList = createNodeInt(7);
    addToTail(myList, createNodeInt(63));
    addToTail(myList, createNodeString("myListElement"));

    printf("%s\n", toStringSimple(myList));

    List* innerList = createNodeString("inner");
    addToTail(innerList, createNodeString("inside inner"));
    addToTail(myList, createNode(innerList, LIST));
    addToTail(innerList, createNode(createNode(createNodeString("deepest"), LIST), LIST));
    addToTail(myList, createNodeInt(73));
    addToTail(innerList, createNodeString("go up"));

    innerList->value = NULL;

    printf("%s\n", toStringSimple(myList));
    printf("%s\n", toStringSimple(CDR(myList)));

    //destroyList(myList);
    return 0;
}

char* convertToString(List* list, int* returnedSize) {
    if (list->type == INTEGER) {
        return toStringInt(list->value, returnedSize);
    } else if (list->type == DOUBLE) {
        return toStringDouble(list->value, returnedSize);
    } else if (list->type == STRING) {
        return toStringString(list->value, returnedSize);
    } else if (list->type == LIST) {
        char* resultedString = toString(list->value, returnedSize);
        char* addedComma = (char*) malloc((*returnedSize + 2) * sizeof(char));
        if (addedComma == NULL) {
            return NULL;
        }
        sprintf(addedComma, "%s, ", resultedString);
        free(resultedString);
        *returnedSize += 2;
        return addedComma;
    }
}

char* toString(List* list, int* stringSize) {
    char* result = (char*)malloc(INCREMENT * sizeof(char));
    if (result == NULL) {
        return NULL;
    }
    result[0] = '\0';
    int actualSize = INCREMENT;
    while (list != NULL) {
        if (notPrimitiveValue(list->type) && list->value == NULL) {
            result = realloc(result, (actualSize + sizeof(null)) * sizeof(char));
            if (result == NULL) {
                return NULL;
            }
            result = strcat(result, null);
            actualSize += sizeof (null);
        } else {
            int returnedSize;
            char* returnedString = convertToString(list, &returnedSize);
            result = realloc(result, (actualSize + returnedSize) * sizeof(char));
            if (result == NULL) {
                return NULL;
            }
            result = strcat(result, returnedString);
            actualSize += returnedSize;
        }
        list = list->next;
    }
    int length = strlen(result);
    if (result[length - 1] != ')') {
        result[length - 2] = '\0';
    }
    char* pharathesis = (char*)malloc((length + 2) * sizeof (char));
    sprintf(pharathesis, "(%s)", result);
    *stringSize = strlen(pharathesis);
    return pharathesis;
}

char* toStringSimple(List* list) {
    int size = 0;
    return toString(list, &size);
}

void destroyNode(List* node) {
    if (node->type == LIST) {
        destroyList(node);
    } else {
        free(node->value);
        free(node);
    }
}

void destroyList(List* list) {
    if (list != NULL) {
        destroyList(list->next);
        destroyNode(list);
    }
}