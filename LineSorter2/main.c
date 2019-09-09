/*! @file main.c
 * LineSorter
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

/*! Функция измерения длины нулл-терминированной строки с учётом нулл-символа
 * @param [in] str Нулл-турминированная строка
 * @return Длина строки
 */
size_t len(const char *str);

#define TOLOWER_OFFSET \
	('A' - 'a') ///< Сдвиг между заглавными и строчными буквами

/*! Функция проверки символа на то, является ли он буквой
 * @param [in] c Символ
 * @return 1 если c - буква, 0 - иначе
 */
int isalphac(const char c);

/*! Функция проверки символа на то, является ли он заглваной буквой
 * @param [in] c Символ
 * @return 1 если c - заглавная буква, 0 - иначе
 */
int iscapitalc(const char c);

/*! Функция перевода символа в строчный нижний регистр
 * @param [in] c Символ
 * @return c в нижнем регистре, если c - заглавная буква, c - иначе
 */
char tolowerc(const char c);

/*! Функция сравнения двух строк в лексиграфическом порядке с учётом
 * только букв строк, приведённых в нижний регистр
 * @param [in] str1 Первая строка
 * @param [in] str2 Вторая строка
 * @return 1 если первая строка больше второй, 0 если они равны и \
 * -1 если первая меньше второй
 */
int strcompare(const char *str1, const char *str2);

/*! Функция сравнения инверсий двух строк в лексиграфическом порядке с
 * учётом только букв строк, приведённых в нижний регистр
 * @param [in] str1 Первая строка
 * @param [in] str2 Вторая строка
 * @return 1 если инверсия первой строки больше инверсии второй, \
 * 0 если они равны и -1 если инверсия первой меньше инверсии второй
 */
int strrevcompare(const char *str1, const char *str2);

/*! Функция обмена значениями указателей на строки
 * @param [in] str1 Указатель для обмена
 * @param [in] str2 Указатель для обмена
 */
void swap(char **str1, char **str2);

/*! Функция чтения файла в строку
 * @param [in] filename Путь к файлу
 * @return Нулл-терминированная строка с содержанием файла
 */
char *readfile(const char *filename);

/*! Функция замены символа в строке
 * @param [in] str Строка
 * @param [in] from Заменяемый символ
 * @param [in] to Заменяющий символ
 * @return Количество заменённых символов
 */
size_t replace(char *str, char from, char to);

/*! Функция создания массива указателей на оканчивающиеся
 * на данный символ подстроки
 * @param [in] str Исходная строка
 * @param [in] numpoints Количество подстрок
 * @param [in] stopc Символ окончания подстроки
 * @return Массив указателей на построки
 *
 * @note UB, если количество stopc в str меньше, чем numpoints
 */
char **genpointers(const char *str, size_t numpoints, char stopc);

/*! Функция сортировки массива строк (Быстрая сортировка)
 * @param [in] array Массив нулл-терминированных строк
 * @param [in] size Размер массива
 * @param [in] comp Компаратор
 */
void bubblesort(char **array, size_t size,
		int (*comp)(const char *, const char *));

/*! Функция сортировки массива строк (Быстрая сортировка)
 * @param [in] array Массив нулл-терминированных строк
 * @param [in] size Размер массива
 * @param [in] comp Компаратор
 */
void quicksort(char **array, size_t size,
	       int (*comp)(const char *, const char *));

/*! Функция записи массива строк в файл
 * @param [in] filename Путь к файлу
 * @param [in] lines Массив строк
 * @param [in] linenum Размер массива строк
 */
void writefile(const char *filename, char **lines, size_t linenum);

/*! Функция очистки буффера ввода
 * @note Честно скопипащено с SO. fseek и fflush не заработали на моей машине
 */
void clearinput();

/*! Функция гарантированного открытия файла
 * Будет вызван exit, если открыть файл не удалось
 * @param [in] fname Путь до файла
 * @param [in] mod Опции открытия файла
 * @return Указатель на открытый файл
 */
FILE* xfopen(const char* fname, const char* mod);

/*! Функция гарантированного выделения памяти
 * Будет вызван exit, если открыть  файл не удалось
 * @param [in] size Колличество необходимой памяти в байтах
 * @return Указатель на начало выделенной памяти
 */
void* xmalloc(size_t size);

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("# LineSorter v2 by InversionSpaces\n");
		printf("# Sorts lines of INPUT and writes it to OUPUT\n");
		printf("# Usage: %s INPUT OUTPUT\n", argv[0]);
		return 1;
	}

	printf("# Reading file...\n");
	char *file = readfile(argv[1]);

	printf("# Done\n# Processing...\n");
	size_t linenum = replace(file, '\n', '\0');
	char **lines = genpointers(file, linenum, '\0');

	printf("# Reverse or straight sorting? [r/s]\n");

	char choice;
	scanf("%c", &choice);
	clearinput();
	choice = tolowerc(choice);

	int (*comp)(const char *, const char *);
	if (choice == 's')
		comp = strcompare;
	else if (choice == 'r')
		comp = strrevcompare;
	else {
		printf("# Error unknown choice. Aborting...\n");
		return 1;
	}

	printf("# Do you have too much time? [y/n]\n");
	scanf("%c", &choice);
	clearinput();
	choice = tolowerc(choice);
	if (choice == 'y')
		bubblesort(lines, linenum, comp);
	else if (choice == 'n')
		quicksort(lines, linenum, comp);
	else {
		printf("# Error unknown choice. Aborting...\n");
		return 1;
	}

	printf("# Done\n# Writing file...\n");
	writefile(argv[2], lines, linenum);
	printf("# Done\n# Exiting...\n");

	free(file);
	free(lines);
}

size_t len(const char *str)
{
	assert(str != NULL);

	char *tmp = (char *)str;
	while (*tmp)
		++tmp;

	return (size_t)(tmp - str);
}

int isalphac(const char c)
{
	return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

int iscapitalc(const char c)
{
	if ('A' <= c && c <= 'Z')
		return 1;
	return 0;
}

char tolowerc(const char c)
{
	if (iscapitalc(c))
		return (c - TOLOWER_OFFSET);
	return c;
}

int strcompare(const char *str1, const char *str2)
{
	assert(str1 != NULL);
	assert(str2 != NULL);

	size_t i = 0, j = 0;
	while (str1[i] && str2[j]) {
		while (str1[i] && !isalphac(str1[i]))
			i++;
		while (str2[j] && !isalphac(str2[j]))
			j++;

		if (!str1[i] && !str2[j])
			return 0;
		if (!str1[i])
			return -1;
		if (!str2[j])
			return 1;

		char lc1 = tolowerc(str1[i]);
		char lc2 = tolowerc(str2[j]);

		if (lc1 < lc2)
			return -1;
		if (lc1 > lc2)
			return 1;

		i++;
		j++;
	}

	if (!str1[i] && !str2[j])
		return 0;
	if (!str1[i])
		return -1;
	if (!str2[j])
		return 1;

	assert(0); ///< Недостижимый код
	return 0;
}

int strrevcompare(const char *str1, const char *str2)
{
	assert(str1 != NULL);
	assert(str2 != NULL);

	ssize_t i = len(str1) - 1, j = len(str2) - 1;
	while (i >= 0 && j >= 0) {
		while (i >= 0 && !isalphac(str1[i]))
			i--;
		while (j >= 0 && !isalphac(str2[j]))
			j--;

		if (i == -1 && j == -1)
			return 0;
		if (i == -1)
			return -1;
		if (j == -1)
			return 1;

		char lc1 = tolowerc(str1[i]);
		char lc2 = tolowerc(str2[j]);

		if (lc1 < lc2)
			return -1;
		if (lc1 > lc2)
			return 1;

		i--;
		j--;
	}

	if (i == -1 && j == -1)
		return 0;
	if (i == -1)
		return -1;
	if (j == -1)
		return 1;

	assert(0); ///< Недостижимый код
	return 0;
}

void swap(char **str1, char **str2)
{
	assert(str1 != NULL);
	assert(*str1 != NULL);
	assert(str2 != NULL);
	assert(*str2 != NULL);

	if (str1 == str2)
		return;

	char *tmp = *str1;
	*str1 = *str2;
	*str2 = tmp;
}

char *readfile(const char *filename)
{
	assert(filename != NULL);

	FILE *fp = xfopen(filename, "rb");

#ifdef __unix__
	struct stat st;
	int err = fstat(fileno(fp), &st);
	if (err != 0) {
		printf("# ERROR: Failed to get file stats. Exiting...\n");
		exit(1);
	}
	size_t size = st.st_size;
#else
	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
#endif

	char *retval = (char *)xmalloc(size + 1);

	size_t readed = fread(retval, 1, size, fp);
	if (readed != size) {
		printf("# ERROR: Failed to read file. Exiting...\n");
		fclose(fp);
		exit(2);
	}
	retval[size] = 0;

	fclose(fp);

	return retval;
}

size_t replace(char *str, char from, char to)
{
	assert(str != NULL);

	size_t count = 0;
	for (size_t i = 0; str[i]; ++i) {
		if (str[i] == from) {
			str[i] = to;
			count++;
		}
	}

	return count;
}

char **genpointers(const char *str, size_t numpoints, char stopc)
{
	assert(str != NULL);

	char **retval = (char **)xmalloc(numpoints * sizeof(char *));

	size_t pos = 0;
	for (size_t i = 0; i < numpoints; ++i) {
		retval[i] = (char *)str + pos;
		while (str[pos] != stopc)
			pos++;
		pos++;
	}

	return retval;
}

void bubblesort(char **array, size_t size,
		int (*comp)(const char *, const char *))
{
	assert(array != NULL);

	char found = 0;
	for (size_t n = 0; n < size; ++n) {
		found = 0;
		for (size_t i = 0; i < size - 1; ++i)
			if (comp(array[i], array[i + 1]) > 0) {
				swap(&array[i], &array[i + 1]);
				found = 1;
			}
		if (!found)
			return;
	}
}

void quicksort(char **array, size_t size,
	       int (*comp)(const char *, const char *))
{
	assert(array != NULL);

	if (size < 2)
		return;
		
	if (size < 10) {
		bubblesort(array, size, comp);
		return;
	}

	size_t l = 0, r = size - 1, p = (l + r) / 2;
	while (l < r) {
		while (comp(array[l], array[p]) < 0 && l < p)
			l++;
		while (comp(array[r], array[p]) >= 0 && p < r)
			r--;
		if (l == r)
			break;

		swap(&array[l], &array[r]);

		if (l == p)
			p = r;
		else if (r == p)
			p = l;
	}

	quicksort(array, p, comp);
	quicksort(array + p + 1, size - p - 1, comp);
}

void writefile(const char *filename, char **lines, size_t linenum)
{
	assert(filename != NULL);
	assert(lines != NULL);

	FILE *fp = xfopen(filename, "wb");

	for (size_t i = 0; i < linenum; ++i) {
		assert(lines[i] != NULL);
		fprintf(fp, "%s\n", lines[i]);
	}

	fclose(fp);
}

void clearinput()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {
	}
}

FILE* xfopen(const char* fname, const char* mod) {
	FILE* retval = fopen(fname, mod);
	if (retval == NULL) {
		printf("# ERROR: Failed to open file: %s. Exiting...\n", fname);
		exit(1);
	}
	
	return retval;
}

void* xmalloc(size_t size) {
	void* retval = malloc(size);
	if (retval == NULL) {
		printf("# ERROR: Failed to allocate memory. Exiting...\n");
		exit(2);
	}
	
	return retval;
}
