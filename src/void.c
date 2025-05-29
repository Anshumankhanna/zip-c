#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// prevent UAF vulnerability.
#define FREE(ptr) (free(ptr), ptr = NULL)
// prevent two pointers pointing to the same memory.
#define SHIFT(TYPE, old_ptr, new_ptr) (old_ptr = (TYPE *) new_ptr, new_ptr = NULL)

typedef struct {
    void* data;
    char* error;
} Result;

void Result_free(Result* ptr) {
    if (ptr == NULL) {
        return ;
    }

    FREE(ptr -> data);
    FREE(ptr -> error);
    FREE(ptr);
}

#define Result_error(error) Result_new(NULL, error)
#define Result_data(data) Result_new(data, NULL)
Result Result_new(void* data, char* error) {
    return (Result) { .data = data, .error = error };
}

typedef struct {
    void* ptr_val1;
    void* ptr_val2;
} ZipElem;
const size_t ZipElemSize = sizeof(ZipElem);

typedef struct {
    ZipElem* arr;
    // TODO: using `index` we must create an iterator for zip structure and zip_next() that can allow for movement of that iterator to reduce the syntax for accessing zip elements.
    size_t index;
    size_t size;
} ZipArray;
const size_t ZipArraySize = sizeof(ZipArray);

void ZipArray_free(ZipArray* ptr) {
    if (ptr == NULL) {
        return ;
    }
    if (ptr -> arr == NULL) {
        goto if_arr_null;
    }

    for (size_t index = 0; index < ptr -> size; ++index) {
        FREE(ptr -> arr[index].ptr_val1);
        FREE(ptr -> arr[index].ptr_val2);
    }

    FREE(ptr -> arr);

    if_arr_null:
    FREE(ptr);
}
Result ZipArray_new(const size_t size) {
    ZipArray* data = malloc(ZipArraySize);

    if (data == NULL) {
        return Result_error("Couldn't allocate memory for data for ZipArray.");
    }
    if ((data -> arr = malloc(size * ZipElemSize)) == NULL) {
        ZipArray_free(data);
        return Result_error("Couldn't allocate memory for data -> arr for ZipArray.");
    }

    data -> index = 0;
    data -> size = size;

    return Result_data(data);
}

#define zip(arr1, size1, arr2, size2) _zip(arr1, size1, sizeof(arr1[0]), arr2, size2, sizeof(arr2[0]))
Result _zip(
    const void* arr1,
    const size_t arr1_size,
    const size_t arr1_element_size,
    const void* arr2,
    const size_t arr2_size,
    const size_t arr2_element_size
) {
    if (arr1 == NULL || arr2 == NULL || arr1_size == 0 || arr2_size == 0) {
        return Result_error("Array not provided.");
    }
    
    const size_t min_size = arr1_size < arr2_size? arr1_size : arr2_size;
    Result result = ZipArray_new(min_size);

    if (result.error != NULL) {
        return result;
    }

    FREE(result.error);
    ZipArray* data;
    SHIFT(ZipArray, data, result.data);

    for (size_t index = 0; index < min_size; ++index) {
        if ((data -> arr[index].ptr_val1 = malloc(arr1_element_size)) == NULL) {
            return Result_error("Could not allocate memory for zip array.");
        }
        if ((data -> arr[index].ptr_val2 = malloc(arr2_element_size)) == NULL) {
            return Result_error("Could not allocate memory for zip array.");
        }

        memcpy(data -> arr[index].ptr_val1, (char*) arr1 + index * arr1_element_size, arr1_element_size);
        memcpy(data -> arr[index].ptr_val2, (char*) arr2 + index * arr2_element_size, arr2_element_size);
    }

    return Result_data(data);
}

int main() {
    int* arr1 = (int *) malloc(100 * sizeof(int));
    const size_t size1 = 100;
    long long* arr2 = (long long *) malloc(26 * sizeof(long long));
    const size_t size2 = 26;

    for (size_t index = 0; index < size1; ++index) {
        arr1[index] = (int) index + 1;
    }
    for (size_t index = 0; index < size2; ++index) {
        arr2[index] = (long long) index + 2;
    }
    
    Result result = zip(arr1, size1, arr2, size2);
    
    if (result.error) {
        perror("Couldn't get zippped array.");
        Result_free(&result);
        return 1;
    }

    ZipArray* data;
    // never should two pointers point to the same memory.
    // hence we remove the access from result.data to prevent future issues if we free one pointer and then try to free the other one.
    SHIFT(ZipArray, data, result.data);

    FREE(arr1);
    FREE(arr2);

    for (size_t index = 0; index < data -> size; ++index) {
        printf("{ %d, %lld }\n", *(int *) data -> arr[index].ptr_val1, *(long long *) data -> arr[index].ptr_val2);
    }

    ZipArray_free(data);
    // we still need to free the memory inside reuslt.
    Result_free(&result);

    printf("All memory freed.");
    
    return 0;
}
