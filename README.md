# Zip() in C

### What's the purpose?
- zip() is a very famous function used in languages like Python and Rust for creating key-value pair like structure using two arrays.
- I wanted to see if I can create something like zip() in C just for the purpose of understanding C and pointers.

### Use?
- It can be used in two different ways:
    - **Macros**: We have `macro.c` in `src/` that uses C style macros to the absolute limits and you can create multiple functions according to your use to use this macro.
        ```c
            // Write this line at the top of your file after 'include' statements.
            DEFINE_ZIP_ARRAY(TYPE1, TYPE1, NAME1, NAME2)

            // this produces:
            // type: NAME1_NAME2_ZipArray, e_NAME1_NAME2_ZipArray
            // functions: LL_Char_ZipArray_new, e_LL_Char_ZipArray_new, e_LL_Char_ZipArray_error, e_LL_Char_ZipArray_data, e_LL_Char_ZipArray_free
        ```
    - **Void \***: We have `void.c` in `src/` that uses `void *` to create generic structs in C that can later be typecasted to use data.
        ```c
            ZipArray* zip(arr1, size1, arr2, size2);
            ZipArray = { ZipElem* data, size_t size };
            ZipElem* = { void* ptr_val1, void* ptr_val2};
        ```

**More documentation coming soon**
