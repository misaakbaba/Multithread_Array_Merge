#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <semaphore.h>
#include <ctype.h>

/*TYPEDEFS*/
typedef int16_t INT16;
typedef u_int16_t UINT16;
typedef int32_t INT32;
typedef u_int32_t UINT32;
typedef char UINT8;

/*ENUM DEFINITON*/
enum NUMBER_OF_FILE {
    NUMBER_OF_FILE_2 = 2,
    NUMBER_OF_FILE_4 = 4,
    NUMBER_OF_FILE_8 = 8
};

/*STRUCT DEFINITION*/
typedef struct {
    pthread_t tid;

    INT32 *p_arr1_s32;
    sem_t arr1_sem;

    INT32 *p_arr2_s32;
    sem_t arr2_sem;

    INT32 *p_result_s32;
    sem_t result_sem;

} thread_params_t;

/*GLOBAL VARIABLES*/
UINT16 file_number;
INT32 *p_shared_buff1_s32;
INT32 *p_shared_buff2_s32;
INT32 *p_shared_buff3_s32;
INT32 *p_shared_buff4_s32;
INT32 *p_shared_buff5_s32;
INT32 *p_shared_buff6_s32;

/*FUNCTION PROTOTYPES*/
void merge_two_array(const INT32 *arr1, const INT32 *arr2, INT32 *result, INT32 arr1_len, INT32 arr2_len);

INT32 *read_file(const UINT8 *file_name, INT32 *const array_size);

int main(int argc, char const *argv[]) {
    if (argc < 7) {
        printf("Invalid number of arguments\n");
        return -1;
    }

    if (strcmp(argv[1], "-n") != 0) {
        printf("invalid argument type");
        return -1;
    }

    file_number = atoi(argv[2]);

    switch (file_number) {
        case NUMBER_OF_FILE_2: {
            UINT8 file_name[128];
            /*read first file*/
            strcpy(file_name, argv[3]);
            INT32 *p_read_arr_s32;
            INT32 array_size_s32 = 0;
            p_read_arr_s32 = read_file(file_name, &array_size_s32);

            /*read second file*/
            strcpy(file_name, argv[4]);
            INT32 *p_read_arr2_s32;
            INT32 array_size2_s32 = 0;
            p_read_arr2_s32 = read_file(file_name, &array_size2_s32);

            INT32 *result_arr;
            result_arr = (INT32 *) malloc(sizeof(INT32) * (array_size_s32 + array_size2_s32));
//            merge_two_array(p_read_arr_s32, p_read_arr2_s32, result_arr, array_size_s32, array_size2_s32);


            break;
        }

        case NUMBER_OF_FILE_4: {

            break;
        }

        case NUMBER_OF_FILE_8: {

            break;
        }

        default:
            printf("Invalid number of files\n");
            return -1;
    }


    return 0;
}

void merge_two_array(const INT32 *arr1, const INT32 *arr2, INT32 *result, INT32 arr1_len, INT32 arr2_len) {
    INT16 arr1_index, arr2_index, result_index;
    arr1_index = arr2_index = result_index = 0;

    while (arr1_index < arr1_len && arr2_index < arr2_len) {
        if (arr1[arr1_index] < arr2[arr2_index]) {
            result[result_index] = arr1[arr1_index];
            arr1_index++;
            result_index++;
        } else {
            result[result_index] = arr2[arr2_index];
            arr2_index++;
            result_index++;
        }
    }

    while (arr1_index < arr1_len) {
        result[result_index] = arr1[arr1_index];
        arr1_index++;
        result_index++;
    }

    while (arr2_index < arr2_len) {
        result[result_index] = arr2[arr2_index];
        arr2_index++;
        result_index++;
    }
}

INT32 *read_file(const UINT8 *file_name, INT32 *const array_size) {
    static INT32 *result_arr;
    FILE *fp;
    fp = fopen(file_name, "r");

    if (fp == NULL) {
        printf("Invalid file name: %s\n", file_name);
        exit(-1);
    } else {
        INT32 temp_size;
        fscanf(fp, "%d", &temp_size); // determine array size
        *array_size = temp_size;

        result_arr = (INT32 *) malloc(sizeof(INT32) * *array_size);
        for (INT32 i = 0; i < *array_size; i++) {
            fscanf(fp, "%d", &result_arr[i]);
        }

        // for (INT32 i = 0; i < *array_size; i++)
        // {
        //     printf("%d\n",result_arr[i]);
        // }
        return result_arr;
    }
}

/*
void merge_two_array(const INT32 *arr1, const INT32 *arr2, INT32 *result, INT32 arr1_len, INT32 arr2_len)
{
    INT16 arr1_index, arr2_index, result_index;
    arr1_index = arr2_index = result_index = 0;

    while(arr1_index < arr1_len && arr2_index < arr2_len)
    {
        if(arr1[arr1_index] < arr2[arr2_index])
        {
            result[result_index] = arr1[arr1_index];
            arr1_index++;
            result_index++;
        }
        else
        {
            result[result_index] = arr2[arr2_index];
            arr2_index++;
            result_index++;
        }
    }

    while(arr1_index < arr1_len)
    {
        result[result_index] = arr1[arr1_index];
        arr1_index++;
        result_index++;
    }

    while(arr2_index < arr2_len)
    {
        result[result_index] = arr2[arr2_index];
        arr2_index++;
        result_index++;
    }
}
*/