// In this program we will create a zip() function that is generic i.e. supports multiple types.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFINE_ERROR_STRUCT(TYPE)\
typedef struct {\
    TYPE* data;\
    size_t* size;\
    char* error;\
} e_##TYPE;\
\
void e_##TYPE##_free(e_##TYPE* ptr) {\
    if (ptr == NULL) {\
        return ;\
    }\
\
    free(ptr -> data);\
    free(ptr -> size);\
    free(ptr -> error);\
    free(ptr);\
}

#define DEFINE_ZIP_ARRAY(TYPE1, TYPE2, NAME1, NAME2)\
typedef struct {\
    TYPE1* ptr_val1;\
    TYPE2* ptr_val2;\
} NAME1##_##NAME2##_ZipArray;\
\
DEFINE_ERROR_STRUCT(NAME1##_##NAME2##_ZipArray)\
\
e_##NAME1##_##NAME2##_ZipArray* _e_##NAME1##_##NAME2##_ZipArray_new(NAME1##_##NAME2##_ZipArray* data, size_t* size, char* err) {\
    e_##NAME1##_##NAME2##_ZipArray* result = malloc(sizeof(e_##NAME1##_##NAME2##_ZipArray));\
\
    result -> data = data;\
    result -> size = size;\
    result -> error = err;\
    \
    return result;\
}\
e_##NAME1##_##NAME2##_ZipArray* e_##NAME1##_##NAME2##_ZipArray_data(NAME1##_##NAME2##_ZipArray* data, size_t* size) {\
    return _e_##NAME1##_##NAME2##_ZipArray_new(data, size, NULL);\
}\
e_##NAME1##_##NAME2##_ZipArray* e_##NAME1##_##NAME2##_ZipArray_error(char* err) {\
    return _e_##NAME1##_##NAME2##_ZipArray_new(NULL, NULL, err);\
}\
e_##NAME1##_##NAME2##_ZipArray* NAME1##_##NAME2##_ZipArray_new(TYPE1* arr1, const size_t size1, TYPE2* arr2, const size_t size2) {\
    if (arr1 == NULL || arr2 == NULL || size1 == 0 || size2 == 0) {\
        return e_##NAME1##_##NAME2##_ZipArray_error("No array provided.");\
    }\
    \
    size_t* min_size = malloc(sizeof(size_t));\
    *min_size = size1 < size2? size1 : size2;\
\
    const size_t elem_size1 = sizeof(TYPE1);\
    const size_t elem_size2 = sizeof(TYPE2);\
    NAME1##_##NAME2##_ZipArray* result = malloc(*min_size * sizeof(NAME1##_##NAME2##_ZipArray));\
\
    if (result == NULL) {\
        return e_##NAME1##_##NAME2##_ZipArray_error("Failed to assign memory for zipped array.");\
    }\
    \
    for (size_t index = 0; index < *min_size; ++index) {\
        result[index].ptr_val1 = malloc(elem_size1);\
\
        if (result[index].ptr_val1 == NULL) {\
            return e_##NAME1##_##NAME2##_ZipArray_error("Failed to allocate memory for zipped val.");\
        }\
\
        result[index].ptr_val2 = malloc(elem_size2);\
\
        if (result[index].ptr_val2 == NULL) {\
            return e_##NAME1##_##NAME2##_ZipArray_error("Failed to allocate memory for zipped val.");\
        }\
\
        memcpy(result[index].ptr_val1, (char*) arr1 + index * elem_size1, elem_size1);\
        memcpy(result[index].ptr_val2, (char*) arr2 + index * elem_size2, elem_size2);\
    }\
\
    return e_##NAME1##_##NAME2##_ZipArray_data(result, min_size);\
}

DEFINE_ZIP_ARRAY(long long*, char, LL, Char);

int main() {
    long long** arr1 = malloc(3 * sizeof(long long*));
    const size_t size1 = 3;
    char* arr2 = calloc(26, sizeof(char));
    const size_t size2 = 26;

    for (size_t index = 0; index < 3; ++index) {
        long long* sub_arr = malloc(sizeof(long long));
        sub_arr[0] = (long long) index + 1;
        arr1[index] = sub_arr;
    }
    for (size_t index = 0; index < 26; ++index) {
        arr2[index] = (char) index + 'a';
    }

    e_LL_Char_ZipArray* result = LL_Char_ZipArray_new(arr1, size1, arr2, size2);

    if (result -> error != NULL) {
        perror(result -> error);
        return 1;
    }

    for (size_t index = 0; index < *result -> size; ++index) {
        printf("{ %lld, %c }\n", *(result -> data[index].ptr_val1[0]), *result -> data[index].ptr_val2);
    }

    e_LL_Char_ZipArray_free(result);

    for (size_t index = 0; index < 3; ++index) {
        free(arr1[index]);
    }
    free(arr1);
    free(arr2);

    return 0;
}
