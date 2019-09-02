/*! @file main.c
 * LineSorter 
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

/// Функция обмена значениями указателей на строки
/// @param [in] str1 Указатель для обмена
/// @param [in] str2 Указатель для обмена
void swapStrings(char **str1, char **str2) {
	if(str1 == str2) return;
	
	char *tmp = *str1;
	*str1 = *str2;
	*str2 = tmp;
}


#define STR_VEC_INTIAL_SIZE (32) ///< Начальный размер вектора строк в элементах
#define STR_VEC_RESIZE_CHUNK (32) ///< Добавляемый при необходимости размер в элементах


/// @details Структура вектора строк
typedef struct {
	char** array; ///< Внутренний динамический массив
	size_t size; ///< Количество хранимых элементов
	size_t realsize; ///< Колличество эллементов, для которых аалоцированна память
} StrVector;


/*! Функция инциализации вектора строк
 * @return Указатель на инициализированный вектор строк
 */
StrVector* strVecInit() {
	StrVector* retval = (StrVector*)malloc(sizeof(StrVector));
	
	if(retval == NULL) abort();
	
	retval->size = 0;
	retval->realsize = STR_VEC_INTIAL_SIZE;
	retval->array = (char**)malloc(STR_VEC_INTIAL_SIZE * sizeof(char*));
}

/*! Функция расширения аллоцированной памяти вектора строк
 * @param [in] vec Указатель на вектор строк
 * @param [in] size Количество элементов, память для которых необходимо аллоцировать
 */
void strVecReserve(StrVector* vec, size_t size) {
	char** newarray = (char**)realloc(vec->array, size * sizeof(char*));
	
	if(newarray == NULL) abort();
	
	vec->array = newarray;
	vec->realsize = size;
}


/*! Функция добавления элемента в вектор строк
 * @param [in] vec Указатель на вектор строк
 * @param [in] str Нулл-терминированная строка для добавления
 * @param [in] len Длина str без нулл-байта
 */
void strVecAppend(StrVector* vec, const char* str, size_t len) {
	if(vec->realsize == vec->size) 
		strVecReserve(vec, vec->size + STR_VEC_RESIZE_CHUNK);
	
	vec->array[vec->size] = (char*)malloc((len + 1) * sizeof(char));
	strcpy(vec->array[vec->size], str);
	vec->size += 1;
}

/*! Функция получения элемента вектора строк
 * @param [in] vec Указатель на вектор строк
 * @param [in] i Индекс элемента
 * @return Константная ссылка на элемент
 */
const char* strVecGet(StrVector* vec, size_t i) {
	return vec->array[i];
}

/*! Функция сортировки вектора строк в лексиграфическом порядке
 * @param [in] vec Указатель на вектор строк
 */
void strVecSort(StrVector* vec) {
	for(int n = 0; n < vec->size; n++) 
		for(int i = 0; i < vec->size - 1; i++)
			if(strcmp(vec->array[i], vec->array[i + 1]) > 0)
				swapStrings(&vec->array[i], &vec->array[i + 1]);
}

/*! Функция деинициализации вектора строк
 * @param [in] vec Указатель на вектор строк
 */
void strVecDeinit(StrVector* vec) {
	for(int i = 0; i < vec->size; i++)
		free(vec->array[i]);
	free(vec->array);
}

#define INITIAL_BUFFER_SIZE (256) ///< Начальный размер в символах буфера считывания строки из файла

/*! Функция считывания файла в вектор строк
 * @param [in] filename Путь к файлу
 * @param [in,out] vec Указатель на инициализированный вектор строк
 */
void fileToStrVec(const char* filename, StrVector* vec) {
	printf("# Trying to read file...\n");
	
	FILE *fp = fopen(filename, "rt");
	
	if(fp == NULL) {
		printf("# Could not open file. Aborting...\n");
		exit(1);
	}
	
	size_t buffersize = INITIAL_BUFFER_SIZE;
	char* strbuffer = (char*)malloc(INITIAL_BUFFER_SIZE * sizeof(char));
	
	if(strbuffer == NULL) {
		printf("# Could not allocate buffer. Aborting...\n");
		fclose(fp);
		exit(1);
	}
	
	while(!feof(fp)) {
		ssize_t charsread = getline(&strbuffer, &buffersize, fp);
		if(charsread > 0)
			strVecAppend(vec, strbuffer, charsread);
	}
	
	fclose(fp);
	free(strbuffer);
	
	printf("# Done\n");
}

/*! Функция вывода содержимого вектора строк в файл
 * @param [in] filename Путь до файла
 * @param [in] vec Указатель на инициализированный вектор строк
 */
void strVecToFile(const char* filename, StrVector* vec) {
	printf("# Trying to write file...\n");
	
	FILE *fp = fopen(filename, "wt");
	
	if(fp == NULL) {
		printf("# Could not open file. Aborting...\n");
		exit(1);
	}
	
	for(int i = 0; i < vec->size; i++)
		if(fprintf(fp, "%s", strVecGet(vec, i)) < 0) {
			printf("# Could not write to file. Aborting...\n");
			exit(1);
		}
		
	fclose(fp);
	
	printf("# Done\n");
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("# Sorting lines in INPUT_FILE and write them in OUTPUT_FILE\n");
		printf("# Usage: %s INPUT_FILE OUTPUT_FILE\n", argv[0]);
		return 1;
	}
	
	StrVector* vec = strVecInit();
	
	fileToStrVec(argv[1], vec);
	strVecSort(vec);
	strVecToFile(argv[2], vec);
	
	strVecDeinit(vec);
}
