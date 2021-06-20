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

    INT32 *p_input_arr1_s32;
    INT32 input_arr1_len_s32;
    sem_t input_arr1_sem;

    INT32 *p_input_arr2_s32;
    INT32 input_arr2_len_s32;
    sem_t input_arr2_sem;

    INT32 *p_result_s32;
    INT32 result_len_s32;
    sem_t result_sem;

    UINT8 *output_file_name;
} thread_params_t;

/*GLOBAL VARIABLES*/
UINT16 file_number;
UINT8 output_file_name[128];

INT32 *p_shared_buff1_s32;
INT32 *p_shared_buff2_s32;
INT32 *p_shared_buff3_s32;
INT32 *p_shared_buff4_s32;
INT32 *p_shared_buff5_s32;
INT32 *p_shared_buff6_s32;

INT32 shared_buff1_len_s32;
INT32 shared_buff2_len_s32;
INT32 shared_buff3_len_s32;
INT32 shared_buff4_len_s32;
INT32 shared_buff5_len_s32;
INT32 shared_buff6_len_s32;

sem_t shared_buff1_sem;
sem_t shared_buff2_sem;
sem_t shared_buff3_sem;
sem_t shared_buff4_sem;
sem_t shared_buff5_sem;
sem_t shared_buff6_sem;

/*FUNCTION PROTOTYPES*/
void *merge_two_array(void *params);

INT32 *read_file(const UINT8 *file_name, INT32 *const array_size);

void *merge_two_array_to_file(void *params);

int main(int argc, char const *argv[]) {
    if (argc < 7) {
        printf("Invalid number of arguments\n");
        return -1;
    }

    if (strcmp(argv[1], "-n") != 0) {
        printf("invalid argument type, expected -n");
        return -1;
    }

    file_number = atoi(argv[2]); // take number of files

    if (strcmp(argv[file_number + 3], "-o") != 0) {
        printf("invalid argument type, expected -o");
        return -1;
    }

    strcpy(output_file_name, argv[file_number + 4]); //take output file name

    switch (file_number) {
        case NUMBER_OF_FILE_2: {
            UINT8 file_name[128];
            int rc;
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

            thread_params_t t_params = {0};
            t_params.p_input_arr1_s32 = p_read_arr_s32;
            t_params.input_arr1_len_s32 = array_size_s32;
            sem_init(&t_params.input_arr1_sem, 0, array_size_s32);
            t_params.p_input_arr2_s32 = p_read_arr2_s32;
            t_params.input_arr2_len_s32 = array_size2_s32;
            sem_init(&t_params.input_arr2_sem, 0, array_size2_s32);
            p_shared_buff1_s32 = malloc(sizeof(INT32) * (array_size_s32 + array_size2_s32));
            t_params.p_result_s32 = p_shared_buff1_s32;
            sem_init(&t_params.result_sem, 0, 0);
            t_params.output_file_name = output_file_name;
//            merge_two_array(p_read_arr_s32, p_read_arr2_s32, result_arr, array_size_s32, array_size2_s32);
            rc = pthread_create(&t_params.tid, NULL, merge_two_array, (void *) &t_params);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }

            pthread_join(t_params.tid, NULL);
            for (int i = 0; i < array_size_s32 + array_size2_s32; ++i) {
                printf("%d\n", t_params.p_result_s32[i]);
            }
            break;
        }

        case NUMBER_OF_FILE_4: {
            UINT8 file_name[128];
            int rc;

            /*read file1*/
            INT32 *p_file1_s32;
            INT32 file1_len_s32 = 0;
            strcpy(file_name, argv[3]);
            p_file1_s32 = read_file(file_name, &file1_len_s32);

            /*read file2*/
            INT32 *p_file2_s32;
            INT32 file2_len_s32 = 0;
            strcpy(file_name, argv[4]);
            p_file2_s32 = read_file(file_name, &file2_len_s32);

            /*read file3*/
            INT32 *p_file3_s32;
            INT32 file3_len_s32 = 0;
            strcpy(file_name, argv[5]);
            p_file3_s32 = read_file(file_name, &file3_len_s32);

            /*read file4*/
            INT32 *p_file4_s32;
            INT32 file4_len_s32 = 0;
            strcpy(file_name, argv[6]);
            p_file4_s32 = read_file(file_name, &file4_len_s32);

            /*thread1 parameters
             * inputs: file1, file2
             * output: p_shared_buff1_s32*/
            thread_params_t t1_params = {0};

            t1_params.p_input_arr1_s32 = p_file1_s32;
            t1_params.input_arr1_len_s32 = file1_len_s32;
            sem_init(&t1_params.input_arr1_sem, 0, file1_len_s32);

            t1_params.p_input_arr2_s32 = p_file2_s32;
            t1_params.input_arr2_len_s32 = file2_len_s32;
            sem_init(&t1_params.input_arr2_sem, 0, file2_len_s32);

            shared_buff1_len_s32 = file1_len_s32 + file2_len_s32;
            p_shared_buff1_s32 = (INT32 *) malloc(sizeof(INT32) * shared_buff1_len_s32);
            t1_params.p_result_s32 = p_shared_buff1_s32;
            t1_params.result_len_s32 = shared_buff1_len_s32;
            sem_init(&shared_buff1_sem, 0, 0);
            t1_params.result_sem = shared_buff1_sem;

            /*thread2 parameters
             * inputs: file3, file4
             * output: p_shared_buff2_s32*/
            thread_params_t t2_params = {0};

            t2_params.p_input_arr1_s32 = p_file3_s32;
            t2_params.input_arr1_len_s32 = file3_len_s32;
            sem_init(&t2_params.input_arr1_sem, 0, file3_len_s32);

            t2_params.p_input_arr2_s32 = p_file4_s32;
            t2_params.input_arr2_len_s32 = file4_len_s32;
            sem_init(&t2_params.input_arr2_sem, 0, file4_len_s32);


            shared_buff2_len_s32 = file3_len_s32 + file4_len_s32;
            p_shared_buff2_s32 = (INT32 *) malloc(sizeof(INT32) * shared_buff2_len_s32);
            t2_params.p_result_s32 = p_shared_buff2_s32;
            t2_params.result_len_s32 = shared_buff2_len_s32;
            sem_init(&shared_buff2_sem, 0, 0);
            t2_params.result_sem = shared_buff2_sem;

            /*thread3 parameters
             * inputs: p_shared_buff1_s32, p_shared_buff2_s32
             * output: outputfile
             * semaphores: (p_shared_buff1_s32,t1_params.result_sem) (p_shared_buff2_s32,t2_params.result_sem)*/
            thread_params_t t3_params = {0};

            t3_params.p_input_arr1_s32 = p_shared_buff1_s32;
            t3_params.input_arr1_len_s32 = shared_buff1_len_s32;
            t3_params.input_arr1_sem = shared_buff1_sem;

            t3_params.p_input_arr2_s32 = p_shared_buff2_s32;
            t3_params.input_arr2_len_s32 = shared_buff2_len_s32;
            t3_params.input_arr2_sem = shared_buff2_sem;

            t3_params.output_file_name = output_file_name;

            //deneme
            printf("before threads\n");
            fflush(stdout);

            rc = pthread_create(&t1_params.tid, NULL, merge_two_array, (void *) &t1_params);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }

            rc = pthread_create(&t2_params.tid, NULL, merge_two_array, (void *) &t2_params);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }

            rc = pthread_create(&t3_params.tid, NULL, merge_two_array_to_file, (void *) &t3_params);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }

            pthread_join(t1_params.tid, NULL);
            pthread_join(t2_params.tid, NULL);
            pthread_join(t3_params.tid, NULL);


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

void *merge_two_array(void *params) {
    thread_params_t *p_t_params = (thread_params_t *) params;
    INT16 arr1_index, arr2_index, result_index;
    arr1_index = arr2_index = result_index = 0;

//    INT32 sem1, sem2;
//    sem1 = sem2 = 0;
//    sem_getvalue(&p_t_params->input_arr1_sem, &sem1);
//    sem_getvalue(&p_t_params->input_arr2_sem, &sem2);
//    printf("sem1:%d  sem2:%d\n",sem1,sem2);

    /*iterate arrays until someone's lenght*/
    while (arr1_index < p_t_params->input_arr1_len_s32 && arr2_index < p_t_params->input_arr2_len_s32) {
        sem_wait(&p_t_params->input_arr1_sem); // wait first array
        sem_wait(&p_t_params->input_arr2_sem); // wait second array
        /*if current value of first array less than second*/
        if (p_t_params->p_input_arr1_s32[arr1_index] < p_t_params->p_input_arr2_s32[arr2_index]) {
            p_t_params->p_result_s32[result_index++] = p_t_params->p_input_arr1_s32[arr1_index++];
            sem_post(&p_t_params->input_arr2_sem);
        }
            /*if current value of second array less than first*/
        else {
            p_t_params->p_result_s32[result_index++] = p_t_params->p_input_arr2_s32[arr2_index++];
            sem_post(&p_t_params->input_arr1_sem);
        }
        sem_post(&p_t_params->result_sem); // increase result buffer semaphore
//        printf("%d\n", p_t_params->p_result_s32[result_index-1]);
    }


//    printf("out_while\n");
//    fflush(stdout);
    /*if second array reach its limit, add first array's remaining values to result array*/
    while (arr1_index < p_t_params->input_arr1_len_s32) {
//        printf("arr1_while\n");
//        fflush(stdout);
        sem_wait(&p_t_params->input_arr1_sem); // wait first array
        p_t_params->p_result_s32[result_index] = p_t_params->p_input_arr1_s32[arr1_index];
        arr1_index++;
        result_index++;
        sem_post(&p_t_params->result_sem); // increase result buffer semaphore
    }
    /*if first array reach its limit, add second array's remaining values to result array*/
    while (arr2_index < p_t_params->input_arr2_len_s32) {
//        printf("arr2_while");
//        fflush(stdout);
        sem_wait(&p_t_params->input_arr2_sem); // wait second array
        p_t_params->p_result_s32[result_index] = p_t_params->p_input_arr2_s32[arr2_index];
        arr2_index++;
        result_index++;
        sem_post(&p_t_params->result_sem); // increase result buffer semaphore
    }
    INT32 sem1, sem2, sem3;
    sem1 = sem2 = sem3 = 0;
    sem_getvalue(&p_t_params->input_arr1_sem, &sem1);
    sem_getvalue(&p_t_params->input_arr2_sem, &sem2);
    sem_getvalue(&p_t_params->result_sem, &sem3);
    printf("sem1:%d  sem2:%ds  result:%d\n", sem1, sem2,sem3);
    fflush(stdout);
    printf("out_while\n");
    fflush(stdout);
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
        fclose(fp);
        return result_arr;
    }
}

void *merge_two_array_to_file(void *params) {
    thread_params_t *p_t_params = (thread_params_t *) params;
    INT16 arr1_index, arr2_index, result_index;
    arr1_index = arr2_index = result_index = 0;

    FILE *fp;
    fp = fopen(p_t_params->output_file_name, "w");

    INT32 sem1, sem2, sem3;
    sem1 = sem2 = sem3 = 0;
    sem_getvalue(&p_t_params->input_arr1_sem, &sem1);
    sem_getvalue(&p_t_params->input_arr2_sem, &sem2);
    sem_getvalue(&p_t_params->result_sem, &sem3);
    printf("sem1:%d  sem2:%ds  result:%d\n", sem1, sem2,sem3);

    /*iterate arrays until someone's lenght*/
    while (arr1_index < p_t_params->input_arr1_len_s32 && arr2_index < p_t_params->input_arr2_len_s32) {
        sem_wait(&p_t_params->input_arr1_sem); // wait first array
        sem_wait(&p_t_params->input_arr2_sem); // wait second array
        printf("heloğ\n");
        fflush(stdout);
        /*if current value of first array less than second*/
        if (p_t_params->p_input_arr1_s32[arr1_index] < p_t_params->p_input_arr2_s32[arr2_index]) {
            fprintf(fp, "%d\n", p_t_params->p_input_arr1_s32[arr1_index++]);
            sem_post(&p_t_params->input_arr2_sem);
            printf("%d\n", p_t_params->p_input_arr1_s32[arr1_index - 1]);
        }
            /*if current value of second array less than first*/
        else {
            fprintf(fp, "%d\n", p_t_params->p_input_arr2_s32[arr2_index++]);
            sem_post(&p_t_params->input_arr1_sem);
            printf("%d\n", p_t_params->p_input_arr2_s32[arr2_index - 1]);
        }

    }
    printf("out_while\n");
    fflush(stdout);
    /*if second array reach its limit, add first array's remaining values to result array*/
    while (arr1_index < p_t_params->input_arr1_len_s32) {
//        printf("arr1_while\n");
//        fflush(stdout);
        sem_wait(&p_t_params->input_arr1_sem); // wait first array
        fprintf(fp, "%d\n", p_t_params->p_input_arr1_s32[arr1_index++]);
    }
    /*if first array reach its limit, add second array's remaining values to result array*/
    while (arr2_index < p_t_params->input_arr2_len_s32) {
//        printf("arr2_while");
//        fflush(stdout);
        sem_wait(&p_t_params->input_arr2_sem); // wait second array
        fprintf(fp, "%d\n", p_t_params->p_input_arr2_s32[arr2_index++]);
    }
    fclose(fp);
}

/*
void merge_two_array(void *params) {
    thread_params_t *t_params = (thread_params_t*) params;
    INT16 arr1_index, arr2_index, result_index;
    arr1_index = arr2_index = result_index = 0;

    while (arr1_index < t_params->input_arr1_len_s32 && arr2_index < t_params->input_arr2_len_s32) {
        if (t_params->p_input_arr1_s32[arr1_index] < t_params->p_input_arr2_s32[arr2_index]) {
            t_params->p_result_s32[result_index] = t_params->p_input_arr1_s32[arr1_index];
            arr1_index++;
            result_index++;
        } else {
            t_params->p_result_s32[result_index] = t_params->p_input_arr1_s32[arr2_index];
            arr2_index++;
            result_index++;
        }
    }

    while (arr1_index < t_params->input_arr1_len_s32) {
        t_params->p_result_s32[result_index] = t_params->p_input_arr1_s32[arr1_index];
        arr1_index++;
        result_index++;
    }

    while (arr2_index < t_params->input_arr2_len_s32) {
        t_params->p_result_s32[result_index] = t_params->p_input_arr2_s32[arr2_index];
        arr2_index++;
        result_index++;
    }
}
*/
