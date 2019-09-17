/*! @file main.c
 * LineSorter
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <memory.h>
#include <time.h>

#define GREEN "\033[32;1m"

#ifndef NMEASURE
#define MEASURE(x)														\
	{																	\
		clock_t start = clock();										\
		x;																\
		clock_t stop  = clock();								    	\
		printf(GREEN "## MEASURE (%s): %lg ms\033[0m\n", #x,			\
		       (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC);		\
	}
#else
#define MEASURE(x) { x; }
#endif

#ifndef NDEBUG
#define ASSERT(x) 																			\
	if (!(x)) {																				\
		printf("\033[31;1m## ASSERT FAILED (%s), file: %s, func: %s, line: %d\033[0m\n", 	\
				#x, __FILE__, __PRETTY_FUNCTION__, __LINE__);								\
		exit(4);																			\
	}
#else
#define ASSERT(x) ;
#endif

/*! Функция гарантированного выделения памяти
 * Будет вызван exit, если выделить память не удалось
 * @param [in] size Количество необходимой памяти в байтах
 * @return Указатель на начало выделенной памяти
 */
void *xmalloc(size_t size);

typedef struct stack stack;

//! Структура стэка
struct stack {
	size_t elsize; ///< Размер одного элемента в байтах
	void *bottom;  ///< Указатель на начало памяти стэка
	void *top;     ///< Указатель на верхний элемент стэка
	size_t size;   ///< Размер стэка в байтах
};

#define INITIAL_STACK_SIZE (1024) ///< Начальный размер стэка в элементах
#define STACK_CHUNK_SIZE   (1024) ///< Размер блока для расширения в элементах

/*! Функция инициализации стэка
 * @param [in] elsize Размер элемента стэка в байтах
 * @return Указатель на структуру инициализированного стэка
 */
stack *stack_init(size_t elsize);

/*! Функция добавления в стэк
 * @param [in] stackp Указатель на структуру стэка
 * @param [in] datap Указатель на добавляемый элемент
 */
void stack_push(stack *stackp, void *datap);

/*! Функция взятия из стэка
 * @param [in] stackp Указатель на структуру стэка
 * @param [out] datap Указатель на возвращаемый элемент
 */
void stack_pop(stack *stackp, void *datap);

/*! Функция деинициализации стэка
 * @param [in] stackp Указатель на структуру стэка
 */
void stack_deinit(stack *stackp);

/*! Функция выяснения, пустой ли стэк
 * @param [in] stackp Указатель на структуру стэка
 * @return 1 если стэк пуст, 0 иначе
 */
char stack_is_empty(stack *stackp);

/*! Функция обмена значениями двух элементов
 * @param [in] a Указатель на первый элемент
 * @param [in] b Указатель на второй элемент
 * @param [in] t Указатель на буфер
 * @param [in] s Размер элементов в байтах
 */
void swap(void *a, void *b, void *buf, size_t size);

/*! Функция сортировки (Быстрая сортировка)
 * @param [in] arr Указатель на массив
 * @param [in] n Количество элементов
 * @param [in] size Размер одного элемента
 * @param [in] cmp Компаратор
 */
void q_sort(void *arr, size_t n, size_t size,
	    int (*cmp)(const void *, const void *));

/*! Функция измерения длины нулл-терминированной строки \
 * с учётом нулл-символа
 * @param [in] str Нулл-турминированная строка
 * @return Длина строки
 */
size_t len(const char *str);

typedef struct string string;

//! Структура строки
struct string {
	char *str; ///< Указатель на начало строки
	size_t len; ///< Длина строки с учётом нулл-символа
};

#define TOLOWER_OFFSET \
	('A' - 'a') ///< Сдвиг между заглавными и строчными буквами

/*! Функция проверки символа на то, является ли он буквой
 * @param [in] c Символ
 * @return 1 если c - буква, 0 - иначе
 */
int is_alphac(const char c);

/*! Функция проверки символа на то, является ли он заглваной буквой
 * @param [in] c Символ
 * @return 1 если c - заглавная буква, 0 - иначе
 */
int is_capitalc(const char c);

/*! Функция перевода символа в строчный нижний регистр
 * @param [in] c Символ
 * @return c в нижнем регистре, если c - заглавная буква, c - иначе
 */
char to_lowerc(const char c);

/*! Функция сравнения двух строк в лексиграфическом порядке с учётом
 * только букв строк, приведённых в нижний регистр
 * @param [in] str1 Первая строка
 * @param [in] str2 Вторая строка
 * @return 1 если первая строка больше второй, 0 если они равны и \
 * -1 если первая меньше второй
 */
int str_compare(const char *str1, const char *str2);

/*! Функция сравнения двух строк в лексиграфическом порядке с учётом
 * только букв строк, приведённых в нижний регистр
 * @param [in] str1 Первая строка
 * @param [in] str2 Вторая строка
 * @return 1 если первая строка больше второй, 0 если они равны и \
 * -1 если первая меньше второй
 */
int string_compare(const string *str1, const string *str2);

/*! Функция сравнения инверсий двух строк в лексиграфическом порядке с
 * учётом только букв строк, приведённых в нижний регистр
 * @param [in] str1 Первая строка
 * @param [in] str2 Вторая строка
 * @return 1 если инверсия первой строки больше инверсии второй, \
 * 0 если они равны и -1 если инверсия первой меньше инверсии второй
 */
int str_rev_compare(const char *str1, const char *str2);

/*! Функция сравнения инверсий двух строк в лексиграфическом порядке с
 * учётом только букв строк, приведённых в нижний регистр
 * @param [in] str1 Первая строка
 * @param [in] str2 Вторая строка
 * @return 1 если инверсия первой строки больше инверсии второй, \
 * 0 если они равны и -1 если инверсия первой меньше инверсии второй
 */
int string_rev_compare(const string *str1, const string *str2);

/*! Функция обмена значениями указателей на строки
 * @param [in] str1 Указатель для обмена
 * @param [in] str2 Указатель для обмена
 */
void swap_str(char **str1, char **str2);

/*! Функция обмена значениями строк
 * @param [in] str1 Указатель на строку для обмена
 * @param [in] str2 Указатель на строку для обмена
 */
void swap_string(string *str1, string *str2);

/*! Функция чтения файла в строку
 * @param [in] filename Путь к файлу
 * @return Нулл-терминированная строка с содержанием файла
 */
char *read_file(const char *filename);

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
char **gen_strs(const char *str, size_t numpoints, char stopc);

/*! Функция сортировки массива строк (Сортировка пузырьком)
 * @param [in] array Массив нулл-терминированных строк
 * @param [in] size Размер массива
 * @param [in] comp Компаратор
 */
void bubble_sort_str(char **array, size_t size,
		     int (*comp)(const char *, const char *));

/*! Функция сортировки массива строк (Быстрая сортировка)
 * @param [in] array Массив нулл-терминированных строк
 * @param [in] size Размер массива
 * @param [in] comp Компаратор
 */
void quick_sort_str(char **array, size_t size,
		    int (*comp)(const char *, const char *));

/*! Функция сортировки массива строк (Сортировка пузырьком)
 * @param [in] array Массив нулл-терминированных строк
 * @param [in] size Размер массива
 * @param [in] comp Компаратор
 */
void bubble_sort_string(string *array, size_t size,
			int (*comp)(const string *, const string *));

/*! Функция сортировки массива строк (Быстрая сортировка)
 * @param [in] array Массив нулл-терминированных строк
 * @param [in] size Размер массива
 * @param [in] comp Компаратор
 */
void quick_sort_string(string *array, size_t size,
		       int (*comp)(const string *, const string *));

/*! Функция записи массива строк в файл
 * @param [in] filename Путь к файлу
 * @param [in] lines Массив строк
 * @param [in] linenum Размер массива строк
 */
void write_file_str(const char *filename, char **lines, size_t linenum);

/*! Функция записи массива строк в файл
 * @param [in] filename Путь к файлу
 * @param [in] lines Массив строк
 * @param [in] linenum Размер массива строк
 */
void write_file_strings(const char *filename, string *lines, size_t linenum);

/*! Функция очистки буффера ввода
 * @note Честно скопипащено с SO. fseek и fflush не заработали на моей машине
 */
void clear_input();

/*! Функция гарантированного открытия файла
 * Будет вызван exit, если открыть файл не удалось
 * @param [in] fname Путь до файла
 * @param [in] mod Опции открытия файла
 * @return Указатель на открытый файл
 */
FILE *xfopen(const char *fname, const char *mod);

/*! Функция выяснения размера открытого файла
 * @param [in] file Указатель на файл
 * @return Размер файла
 */
size_t file_size(FILE *file);

/*! Функция создания массива оканчивающихся
 * на данный символ подстрок
 * @param [in] str Исходная строка
 * @param [in] numpoints Количество подстрок
 * @param [in] stopc Символ окончания подстроки
 * @return Массив подстрок
 *
 * @note UB, если количество stopc в str меньше, чем numpoints
 */
string *gen_strings(const char *str, size_t numpoints, char stopc);

/*! Совместимый с stdlib компаратор-обёртка над @ref str_compare */
int c_str_compare(const void *p1, const void *p2)
{
	return str_compare(*(char *const *)p1, *(char *const *)p2);
}

/*! Совместимый с stdlib компаратор-обёртка над @ref string_compare */
int c_string_compare(const void *p1, const void *p2)
{
	return string_compare((const string *)p1, (const string *)p2);
}

/*! Совместимый с stdlib компаратор-обёртка над @ref str_rev_compare */
int c_str_rev_compare(const void *p1, const void *p2)
{
	return str_rev_compare(*(char *const *)p1, *(char *const *)p2);
}

/*! Совместимый с stdlib компаратор-обёртка над @ref string_rev_compare */
int c_string_rev_compare(const void *p1, const void *p2)
{
	return string_rev_compare((const string *)p1, (const string *)p2);
}

/*! Функция для вывода usage
 * @param [in] name Название программы
 */
void print_usage(const char *name);

/*! Функция сортировки строк файла и вывода результата в другой файл
 * @param [in] infile Путь до входного файла
 * @param [in] outfile Путь до выходного файла
 * @param [in] reverse 0 - прямая сортировка, иначе - обратная
 */
void sort_file_lines(const char *infile, const char *outfile, char reverse);

#define Q_SORT_TEST_ITERATIONS (10) ///< Количество итераций теста q_sort
#define Q_SORT_TEST_MAX_SIZE                                                   \
	(1000) ///< Максимальная длина массива для теста q_sort
#define Q_SORT_TEST_MIN_SIZE                                                   \
	(100) ///< Минимальная длина массива для теста q_sort
#define Q_SORT_TEST_MAX_INT                                                    \
	(1000) ///< Максимальный размер элемента для теста q_sort

/*! Совместимый с stdlib компаратор для int */
int c_int_cmp(const void *a, const void *b);

#ifdef TEST
/*! Функция тестирования @ref q_sort */
void test_q_sort();

/*! Фунция тестирования @ref str_compare */
void test_compare();

/*! Функция тестирования @ref str_rev_compare */
void test_rev_compare();
#endif

int main(int argc, char *argv[])
{
#ifndef TEST
	if (argc != 4) {
		print_usage(argv[0]);
		return 0;
	}

	sort_file_lines(argv[1], argv[2], to_lowerc(argv[3][0]) == 'r');
#else
	test_q_sort();
	test_compare();
	test_rev_compare();
#endif
}

#ifdef TEST
void test_q_sort()
{
	srand(time(NULL));

	for (int i = 0; i < Q_SORT_TEST_ITERATIONS; ++i) {
		int size =
			rand() % (Q_SORT_TEST_MAX_SIZE - Q_SORT_TEST_MIN_SIZE) +
			Q_SORT_TEST_MIN_SIZE;
		int *arr = xmalloc(sizeof(int) * size);
		for (int j = 0; j < size; ++j)
			arr[j] = rand();

		q_sort(arr, size, sizeof(int), c_int_cmp);

		for (int j = 0; j < size - 1; ++j)
			if (arr[j] > arr[j + 1]) {
				printf("# q_sort test failed on array:\n");
				for (int k = 0; k < size; ++k)
					printf("%d ", arr[k]);
				exit(3);
			}
	}

	printf("# q_sort test passed\n");
}

void test_compare()
{
	ASSERT(str_compare("BNKaYE c!Vr-bfc ", "kzw'SotrS.eZBcAFdnBV") == -1);
	ASSERT(str_compare(".MdAkj", "mGHj'xlew") == -1);
	ASSERT(str_compare("OYxEjAvq", "TaYGDRo") == -1);
	ASSERT(str_compare("nCNecPH-ahjU!psIOFB", "siNXcQWqqFAdynriZFVL,") ==
	       -1);
	ASSERT(str_compare("QuhJKBQOyI", "R'-qn ytAkfwccMygA bD") == -1);
	ASSERT(str_compare("!dlMF??kT!kOvjFT", "?!V ba-EnYYMgw") == -1);
	ASSERT(str_compare("l.te,uzntGrT!K", "y Z,e") == -1);
	ASSERT(str_compare("?HSIkRCvtyG?", "pXNzszbMb!") == -1);
	ASSERT(str_compare("DPG?uGltIp", "vHR-xYGWnKck,?yXP") == -1);
	ASSERT(str_compare("rTIJhY.wB?D", "rznIzLptfIVD-JPwOhcRv'r") == -1);
	ASSERT(str_compare("OrcEyqUnEaCfb.T", "wZp?UV!ipbGYnLZLbBu") == -1);
	ASSERT(str_compare("aRbDKd'ipWo ", "GyYTiKKPnztpuVS' O-fol'") == -1);
	ASSERT(str_compare("MrCbhXym", "zFAib") == -1);
	ASSERT(str_compare("JxZVpbVdeKmgica", "ZlkOHAmXuV") == -1);
	ASSERT(str_compare("kBdN.qZYTWH?'pLFycDai", "nyZEgoeYZ..FpKIpOp") ==
	       -1);
	ASSERT(str_compare("Qg?PLJ,GdWCeyHEe.", "?RVv-R!yCJ?zhC") == -1);
	ASSERT(str_compare("VQlYm L.CN!I", "xtQcO") == -1);
	ASSERT(str_compare("IBJJJRGeLE.zc", "nS.,SHXm") == -1);
	ASSERT(str_compare("tGgHTlKEVi", "wFjsnsy.AFbXJ-") == -1);
	ASSERT(str_compare("NrvpWJO!nrtpqWRmpLqwok",
			   "SEs'AD.ivCgVwHYPFqJVgKxC") == -1);
	ASSERT(str_compare("?ApXdmeeD?", "W.tb-cyenQZoCdLnQHVBdBTm") == -1);
	ASSERT(str_compare("IFOuIn", "SCbdY.Anik!Vncnm!,!wyc t") == -1);
	ASSERT(str_compare("CfDcJkgaIZtsahEdo!DAbAOc",
			   "HKXHCclAFeeQzxA?sLomqHJ") == -1);
	ASSERT(str_compare("oswjAnHYMMaSXpfVhlEeAU", "z!EQy,xekEfvBBXPQJWAn") ==
	       -1);
	ASSERT(str_compare("Mit?rfmBfCsK,w", "NcaVOT") == -1);
	ASSERT(str_compare("IUtv-MzTSbh.kc?vd-R l-R", "xX?ug") == -1);
	ASSERT(str_compare("ipiyNbq", "'q,FAulJJKBmEFZDKalQCm") == -1);
	ASSERT(str_compare("AxtHhYZjPF'dBl-", "UeBeOICfcrAVNet") == -1);
	ASSERT(str_compare("E'nCYa", "tdBtnFvn") == -1);
	ASSERT(str_compare("gltMsvfg uyygeKJ!dPOvF",
			   "XjVhqsFjG!DBBBdtthggxRj") == -1);
	ASSERT(str_compare("lxM?JAC", " mt GKzQJ!jU.UG") == -1);
	ASSERT(str_compare("qhoRPjB AJ'bzV", "QV.mRAV'bXj") == -1);
	ASSERT(str_compare("eFY,qUS,zbDpRUm", "hZPqGUL!VzJ!ZqG,rxTwmi") == -1);
	ASSERT(str_compare("nktoWnfioYhauh.wM' kW'H",
			   "ycMX-rhrYIiFyY.jLEY-ONM") == -1);
	ASSERT(str_compare("ahXINknIgnFzDFYznfeXy",
			   "ZG.f?kAqpXamIVjNk-pJOIc") == -1);
	ASSERT(str_compare("jA!a?", "stDtmpcI") == -1);
	ASSERT(str_compare("bL-DAnIuRt", "LtK-LMjc") == -1);
	ASSERT(str_compare("baTulqzFIBoeDiGjYzvwWip", "hi sbxa,CxE,D") == -1);
	ASSERT(str_compare("bioPfXJ rl", "vTbBHIp.Uxj?TaZ-KKDf!un") == -1);
	ASSERT(str_compare("D!ESyXqPa,lCisxW", "UvrXoeTMGC-XfyUMrwIpCS") == -1);
	ASSERT(str_compare("k.EKZuQ!XCEhtvgqQnSYei",
			   "srbRu.dVjOHBlY'ZTJS rJm") == -1);
	ASSERT(str_compare("?BmjucGycg?FGMPb-bj?wx-a",
			   " pcOBABNYzoHH.CisVX.LQ") == -1);
	ASSERT(str_compare("NASM?AuqZYOFoETKVJ", "QOGlHMiILMqyA-JgXKN.LG") ==
	       -1);
	ASSERT(str_compare("ionhd ", "oRMBuFj?JfSXKeZdm") == -1);
	ASSERT(str_compare("kcLPWGhtZEJ.hYZZm", "sWdMqBHAVSL?fVl") == -1);
	ASSERT(str_compare(",ilpstD,!,RCK", "'qjd'jrD?MxJ.SY") == -1);
	ASSERT(str_compare("punWxFPrSIwoQLoTwxQvWp", "zbRekoSImXK,ltI") == -1);
	ASSERT(str_compare("hUob !", "YRr?wmnaZVGMu.OFvcWjGU,") == -1);
	ASSERT(str_compare("Mqio-k ?", "WBmr'KWX eQCxU") == -1);
	ASSERT(str_compare("EMDBRJ?EZQ.mqJwAKL", "piEJ!uZ'K") == -1);
	ASSERT(str_compare("pnMet", "ZPXXoA") == -1);
	ASSERT(str_compare("ekCr UVdKk ZWLHhzpmKyYMN", "iDHzj cvcpQxZ") == -1);
	ASSERT(str_compare("Th,unTsVXxgt!GH", "WU lgH,hE'zRAHyTdxnb") == -1);
	ASSERT(str_compare("EBjg,Xmsn!DiUCcT", "W!heZLmvrixuXTezxlOly") == -1);
	ASSERT(str_compare("OepHDwiaUaRO'gtlcjJYrfIW", "SMe?euYAoLlCZT") == -1);
	ASSERT(str_compare("GY!ANe dlHLQ", " l,AhqSka") == -1);
	ASSERT(str_compare("FY-NxXiYlqL", "WFqDGDAmCURNg.!TSh") == -1);
	ASSERT(str_compare("'PjNqrePcrx.XmAWok,?,WM", "pmNKraCnuWG?UCt") == -1);
	ASSERT(str_compare("iIs!rM", "QspndrMpdkq'ma") == -1);
	ASSERT(str_compare("cxOv.QQwKS", "D-AHXi!qYatbzx") == -1);
	ASSERT(str_compare("csDfyqIKvQg,U!", "Ys!o!Vh'BXa,hO") == -1);
	ASSERT(str_compare("!KjG!mGYporWePmIN", "WXVkeLi,qypOW") == -1);
	ASSERT(str_compare("dOgaq Zfs.GfUaMnDD", "k'LrrDrdMSFagMLUuL") == -1);
	ASSERT(str_compare("eCBJub,eCU", "GyERdPfzCp") == -1);
	ASSERT(str_compare("d,!'GmZKgrizoGpl 'tw?e", "ZxYI,U") == -1);
	ASSERT(str_compare("Fnfc'mqRQ Fu?WWXGNvh", "gIAQ uT-oPeMqhfqLfT") ==
	       -1);
	ASSERT(str_compare("gvgZBwW", "LnJ ezRy") == -1);
	ASSERT(str_compare("qHAeIHcXHiqdbk", "YZPkFALnyEniM!dQ joAvJlL") == -1);
	ASSERT(str_compare("TdECdHuLnPayl", "wVvygV") == -1);
	ASSERT(str_compare("MzmM. eZDw", "tbldoUbsUXDF-TXmfzLKmG") == -1);
	ASSERT(str_compare("eBYQmkRBrIc.'IlC!iMxv", "JmcRO!ujMDlJaBdYziO?U") ==
	       -1);
	ASSERT(str_compare("aNLfeGImWrN?HVi!ZNcUys", "VST'jYIVEKFf?H!OsmDr") ==
	       -1);
	ASSERT(str_compare("'DbZIVlDqdxOdS", "uZQOOuMYM,stb,sBliU") == -1);
	ASSERT(str_compare("oCLoHe,X", "rUyQ bfrbwNUhJA J") == -1);
	ASSERT(str_compare("l.Y,SdGWcJMheoyY", " ',wwqbYHYot.xyjkGa-!M") == -1);
	ASSERT(str_compare("DvDGPKfDLsFZdoc,F", "IeS!s,h?og-qZlJo?ECZZWO") ==
	       -1);
	ASSERT(str_compare("npzZitngZ", "vmFDQC?c.JKF'") == -1);
	ASSERT(str_compare("KP,ha'WDpXEk-", ".PGxwnrUkq!gKJ!KEiYoyC") == -1);
	ASSERT(str_compare("hwhuIAu", "YdXMkj-,S .MJ") == -1);
	ASSERT(str_compare("dKHtTc!", "eItp,htAPgndOXEeFexR?") == -1);
	ASSERT(str_compare("HiwEhABkJ", "ncMlG''kgxR.se Mt") == -1);
	ASSERT(str_compare("Nztfb wHqy", "TdEBAYw") == -1);
	ASSERT(str_compare("NgjfAvSDwsVZuf,zA?mvw", "T?fqXBYCpsJWU?t DKw,") ==
	       -1);
	ASSERT(str_compare("q,VnO", "ZB?Cn!") == -1);
	ASSERT(str_compare("L'qApjxA.cHMqR", "?!uM!wGQPYxSzZP") == -1);
	ASSERT(str_compare("O!zj!JOmYDxuhG'", "xmqECAolAFVeiNZR") == -1);
	ASSERT(str_compare("VHXUckoqSz", "zvzLCzfA'm SpMF") == -1);
	ASSERT(str_compare("mqLHdkXpaTrGPe,pvlDP",
			   "qVAW.wiaZ?ujwN!kdLTWxIo'") == -1);
	ASSERT(str_compare("prxVDxHzMOmtLhy-YuNTWr", "z!TKPDKjL,zyXrmEvv") ==
	       -1);
	ASSERT(str_compare("Q.oY.FHKgfNDsvQN", "ysoNpYq'D") == -1);
	ASSERT(str_compare("dyZEORkofBfQ", "WgdoPUq-xHqhH!jTaJZQCU-") == -1);
	ASSERT(str_compare("-pSUq!WQXS,v", "WUyjrY!n,kGKw-") == -1);
	ASSERT(str_compare("jirhxGVxJxXR L'T", "kyXcA") == -1);
	ASSERT(str_compare("oOzhLbtLMFyi.E", "s TUzzjj!!MMsNw Tp") == -1);
	ASSERT(str_compare("mvkIipkjxaVYgzyIFm,SbBRO", "ZTSjLhiaHnK,Dya ") ==
	       -1);
	ASSERT(str_compare("''UzAFzJZwjHjn", "WigmXbFAhlgt") == -1);
	ASSERT(str_compare("qq'qwEx", "SdFmeLH") == -1);
	ASSERT(str_compare("sskRHt,cHfK'cb,", "vW,QEQXsDTAHzvRmSZJoYXu?") ==
	       -1);
	ASSERT(str_compare("GmLobsFkDpKR", "nvwyoxWJBaEUvtxEw.'Zv") == -1);
	ASSERT(str_compare("UKJTIF", "ySb'nIdDMUuYmheU!peKAtP") == -1);
	ASSERT(str_compare("tHbSsgMmby-!jKOLiK aqCd",
			   "Di UXuo-h-l.uAVYy-Trh") == 1);
	ASSERT(str_compare("wmmtGm'.or'GBeBLcV", "og-LoT,rw?vzJbLoEmSnvXUl") ==
	       1);
	ASSERT(str_compare("RFctcLjfz", "qDObdjr?I-CvOJzbjQFBCCbY") == 1);
	ASSERT(str_compare("?!LakV.MdafhsN", "h f,HUker?M-LzT CW") == 1);
	ASSERT(str_compare("WQFNxPDxV-P-CdbIYWCR", "dSCsumSN?PRRipxWZ MD ") ==
	       1);
	ASSERT(str_compare("XF!nhRdLJBU!", "qePNqhnRvO") == 1);
	ASSERT(str_compare("!t-!xvDnMqG", "HDYUfHi-xQ") == 1);
	ASSERT(str_compare("Vp'.!pdpwwk!OaUFAcQgaS", "vfiaSFten?snBfSiQ") == 1);
	ASSERT(str_compare("nGjdAh,LHOyN", "k?JkHzinhnC HaAJrYRWjB") == 1);
	ASSERT(str_compare("yDSMfCJOVmMTVqqtECrgr", "L?ujTSUccRTtdQkqp?.D") ==
	       1);
	ASSERT(str_compare("JkbWeM?LmXeSf", "Gha,wlOyYR") == 1);
	ASSERT(str_compare("iwjf.", "b'gXnkDmuyfoXQ.uANiqTkYq") == 1);
	ASSERT(str_compare("vIqClCAnz'A'", "nwWnvyyO-Hvy!fFWdKTO") == 1);
	ASSERT(str_compare("vPltqJqGJRH", "s!oxrXhVpcX") == 1);
	ASSERT(str_compare("Qu?Yjme!Tu", "BVOkIrGmoexZHYvApDMh") == 1);
	ASSERT(str_compare("cyApPs", "bZ jVr?lIkZZNaCa") == 1);
	ASSERT(str_compare("rjyiPQB?YTasZS!V!X", ".-pTd-joapltDmxKAqgQ") == 1);
	ASSERT(str_compare("wseXVqACNIX", "qttqndaIO") == 1);
	ASSERT(str_compare("tFwGF", "jdHsf.fyX") == 1);
	ASSERT(str_compare("Wwlh.", "KZZ,wqj!n'.iZIo") == 1);
	ASSERT(str_compare("WbPt.Nqb.WEjXctSd!JdgH",
			   "GgOVqowceUVHSHSHaOfJNyp-") == 1);
	ASSERT(str_compare(".RSsslTBsTrk'pBL'YY", "IfZoYEqAS br?.GdptJrz") ==
	       1);
	ASSERT(str_compare("fzKOGXYP.ydreCa.fT'JI", " A ksDNk") == 1);
	ASSERT(str_compare("TJRanjHtg", "Cjk,oEX") == 1);
	ASSERT(str_compare("wWJcPGeTJ?dyf", ".ArcI-ALnFNAMeoiP") == 1);
	ASSERT(str_compare("tfDK GlZHYM nEe", "Bu!W'Q") == 1);
	ASSERT(str_compare("pnbNplkXTnQOTvSe", "eVCyGOZUrUCo''P") == 1);
	ASSERT(str_compare("VrCfu C", ",VilJqTZ.FW lkeO") == 1);
	ASSERT(str_compare("ZxxcCvBUsBMR ArzxYws", "J.VWBf.xvU") == 1);
	ASSERT(str_compare("yLTMSbbWAkM ,V", "dNuKFBfyZUvs'xVFF") == 1);
	ASSERT(str_compare("wGrfnSQRybkT", "VnNRoECsXB") == 1);
	ASSERT(str_compare("xJBUokk", "p nQWr!wdmzChCDV'sAs!Y-") == 1);
	ASSERT(str_compare("STEZCZ-N", "aQzkExO,KvfYyeU") == 1);
	ASSERT(str_compare("ZGftn-ZTeMsrJlkaQDfq", "KyCtAGFOi-F") == 1);
	ASSERT(str_compare("?wy.XP-FMevabA", "Hi-HNqIqHlFjQPqzhecn dy") == 1);
	ASSERT(str_compare("?qzsWq.O-OTqfTHYge!,uv", "goCDsqKp") == 1);
	ASSERT(str_compare("LXSgMiRMLe'Duyk", "brO catjGMTVkQImhniRx") == 1);
	ASSERT(str_compare("jqafDjvq", "GmOFNLCimKHhw") == 1);
	ASSERT(str_compare("TEcS!H?ee?jzFI", "HFldhertBhTD?BFaX") == 1);
	ASSERT(str_compare("o.G,BC'NtjOH!hqgO", "elD?IfPTizL!OWaUpt") == 1);
	ASSERT(str_compare("ylym.CWdMJX-wTMD", "CYfqN !QTp") == 1);
	ASSERT(str_compare("H'tyL ?TZkEnxgr", "EDAzMJxtFbh") == 1);
	ASSERT(str_compare("XrteV,-fktIWA!fSDaStV!Ne", "LaH ulE") == 1);
	ASSERT(str_compare("U-hTNUV ujPoO", "gCV'MD,XLJ?TXiVhqhM") == 1);
	ASSERT(str_compare("vNih,", "kURQsClqCgJXw") == 1);
	ASSERT(str_compare("rexa'FhsYhN uegqxNINeQ", "filY,yAQF") == 1);
	ASSERT(str_compare("t nLgGxRYzPIDF!Ik.ACuC",
			   "iSHjzo'rHweuBjXaZvOmwk") == 1);
	ASSERT(str_compare("XxmadSa", "u.cVmnXK,bYxkR,irc") == 1);
	ASSERT(str_compare("ST?qMUfJWS", "aK!bQDX,pojdVml ?HoEq") == 1);
	ASSERT(str_compare("wB-OoH", "eB.BCxkQi") == 1);
	ASSERT(str_compare("zYMp!go q", "xWwaOuqIGdjCefSBI xG") == 1);
	ASSERT(str_compare("qX'fVgmChZzx',QwEd", "fVqjeS") == 1);
	ASSERT(str_compare("LXSjiO?UbIswEzT", "FgjgaydamQnPL-XHLDz") == 1);
	ASSERT(str_compare("zO'RVCwnDUNv!ijHD,x?", "U'eoXhVTr LQSHqQI") == 1);
	ASSERT(str_compare("V.gxI.OwTsu", "aKnQrQzkETyQbOr-") == 1);
	ASSERT(str_compare("XEJmYSsMmouaxLUUe?j", "TTaXIPaexM") == 1);
	ASSERT(str_compare("gucSbtUFth", "EewAmnxl,OVaivWii? ?qkim") == 1);
	ASSERT(str_compare(".hWqqb", "Atru'vcd") == 1);
	ASSERT(str_compare("l.ZVx eAd?LxX", "-cFg.KlkmQxYvJ,FG.!IDN") == 1);
	ASSERT(str_compare(".O -nzZbZRJDfw'DUxs op", "DVeHQZUmE") == 1);
	ASSERT(str_compare("WRENh.ALPj", "!GjHfS.fBu'") == 1);
	ASSERT(str_compare("Sp!sDTZK", "ehd,T") == 1);
	ASSERT(str_compare("zLNkTd KB'wKu!QzFPcN", "h.G  'h.") == 1);
	ASSERT(str_compare("UBgbdLPhIRjDfv", "GKHcxtjFw!") == 1);
	ASSERT(str_compare("!!!IWTbJ!FYDqpojVQJI", "aR-OSjgDg") == 1);
	ASSERT(str_compare("V'?jTg-zg?f?", "homWTl sEAKAjd!bS GWqSK") == 1);
	ASSERT(str_compare("KDO'NDhUgaI.xrogfCEmMj", "AHA.fKJGOWbgb") == 1);
	ASSERT(str_compare("!rmTmoU-dvyM.IqW", "luaAHmeLcZWdw") == 1);
	ASSERT(str_compare("reaSYvaQOAPT", "IpPwYHh") == 1);
	ASSERT(str_compare("HTbfjFQCxZtWN.C!", "G'dRht-'sXx") == 1);
	ASSERT(str_compare("XyheWhMg", "S!FW'Trr,IrtrKsC!SHoyCoM") == 1);
	ASSERT(str_compare("tiVyPCTb", "DgVADJYSJETkRW kAIt") == 1);
	ASSERT(str_compare("in,hkr-y", "DlV.s") == 1);
	ASSERT(str_compare(",?kHQCrqjic?GTRTS", " E?jYMQaahovGKx") == 1);
	ASSERT(str_compare("uf,gfgcDjupBAMVyVaGHAM'M", "qvACCz,,U.") == 1);
	ASSERT(str_compare("sIRYFJvjOs", "mnmiaeru!iIP-c-") == 1);
	ASSERT(str_compare("UNUveuMONF?sgdHJD.?L'", "tKrHzerlqQNN g!aaU") == 1);
	ASSERT(str_compare("TR'zGpnyjAm?LLqCypV", "lCqczjXISSX") == 1);
	ASSERT(str_compare("Wzf.RufSs,K'F.JFRzzUQU", "?.nyYfGlcdSOpsLvV'jU") ==
	       1);
	ASSERT(str_compare("RFFqrtURG", "IWuJ zWlGGqx,jykyuS") == 1);
	ASSERT(str_compare("JdXQj,T.WdQ-v", "f-HZkWTc") == 1);
	ASSERT(str_compare("SwZCsnTgsJyStrp?YmEQ", "MHmSWXy!fwb") == 1);
	ASSERT(str_compare("uHrdrlUeQ,Cs", "EcFuxWvfj'm!l.SRKfr") == 1);
	ASSERT(str_compare("uueEFtqI'sic-fJvP", "qKPTH,bx") == 1);
	ASSERT(str_compare("qflsJuSbhtcsV-icoDPdEhCO",
			   "P-LRKiyLcnqoOJABX!wF,dw") == 1);
	ASSERT(str_compare("BJD.zLZRDHZkrRq", "AVTCwX?zCtB.khP") == 1);
	ASSERT(str_compare("UFQyqsTJjAsTuk?WHky", "rj-X?mVamGI!k") == 1);
	ASSERT(str_compare("vvG,ywOjPHII", "pz-lvsffoDrvyC?iazP") == 1);
	ASSERT(str_compare("xTCcuoeMXFyuObIZHX!n,", "cIXvG?Hc") == 1);
	ASSERT(str_compare("Hpsozxgsi,jkU", "FNlmDxNAHnb") == 1);
	ASSERT(str_compare("xevTD", "vWMpFXeUSDA") == 1);
	ASSERT(str_compare("mwubAXxJOJfYwPx fmr", "kyBCfmWyMgIqLFaojeU") == 1);
	ASSERT(str_compare("'m.jQTLFIhs", "JAIyWEAaDXwEY PSedN") == 1);
	ASSERT(str_compare("xFUXakWKIJWaxADBl.Ey", "-wG,Svq,fJuOvDcUwVcOP") ==
	       1);
	ASSERT(str_compare("r!mOVViHPvg", "!osUXXlBgUHpV") == 1);
	ASSERT(str_compare("!bUBbZsQ!rprJ!", "BraNxOisZa") == 1);
	ASSERT(str_compare("EpGqKDWkU-rVvyMYoDI", "eHK?iltvphZw") == 1);
	ASSERT(str_compare("pWz'Z", "igfRiCHKheXNK'XjaT") == 1);
	ASSERT(str_compare("ruG'npa'UuHFcqfEC'", "ji!r!") == 1);
	ASSERT(str_compare("V kzMUv!fbQQSv CpFpp.H?",
			   ".etNauIfqAPzEoXYmg!M'c") == 1);
	ASSERT(str_compare("-FE", "fE") == 0);
	ASSERT(str_compare("P E", "PE") == 0);
	ASSERT(str_compare("E-T", "eT") == 0);
	ASSERT(str_compare("XO", "Xo") == 0);
	ASSERT(str_compare("h-", "h' ") == 0);
	ASSERT(str_compare("o ", ",O-'") == 0);
	ASSERT(str_compare("'?r", ".R") == 0);
	ASSERT(str_compare("?o", "?O") == 0);
	ASSERT(str_compare("fM!", "FM-") == 0);
	ASSERT(str_compare("?.?", ",'") == 0);
	ASSERT(str_compare(",wO", "wO") == 0);
	ASSERT(str_compare("-R", " R") == 0);
	ASSERT(str_compare(",B", "-B") == 0);
	ASSERT(str_compare("',", "!-") == 0);
	ASSERT(str_compare("Bu", "B U") == 0);
	ASSERT(str_compare("?,", "  ") == 0);
	ASSERT(str_compare(" ,!", "!,") == 0);
	ASSERT(str_compare("E!", "E ") == 0);
	ASSERT(str_compare("B'", "b-") == 0);
	ASSERT(str_compare("V-", "v,'") == 0);
	ASSERT(str_compare("Nn", ",nn") == 0);
	ASSERT(str_compare("!M", ".M") == 0);
	ASSERT(str_compare("h,C", "hC") == 0);
	ASSERT(str_compare("md", "MD") == 0);
	ASSERT(str_compare("Ju", "ju") == 0);
	ASSERT(str_compare(" B", ".B") == 0);
	ASSERT(str_compare("Vd", "VD") == 0);
	ASSERT(str_compare("ql", "Ql") == 0);
	ASSERT(str_compare("'xn", "Xn") == 0);
	ASSERT(str_compare("?!", " ,") == 0);
	ASSERT(str_compare("D-", "-d'") == 0);
	ASSERT(str_compare(".o", ".O") == 0);
	ASSERT(str_compare("!R", "'r") == 0);
	ASSERT(str_compare("qW", "Qw") == 0);
	ASSERT(str_compare("Z ? ", "z!") == 0);
	ASSERT(str_compare("Y, ", "Y-") == 0);
	ASSERT(str_compare("!mt", "m.T") == 0);
	ASSERT(str_compare("gTc", "gTc") == 0);
	ASSERT(str_compare("-c", "?c") == 0);
	ASSERT(str_compare("!o", "?-O") == 0);
	ASSERT(str_compare("f?'?", "-f-") == 0);
	ASSERT(str_compare("rk", "rK") == 0);
	ASSERT(str_compare("!FY", "FY") == 0);
	ASSERT(str_compare("D-", "D!") == 0);
	ASSERT(str_compare("nh", "NH") == 0);
	ASSERT(str_compare("'a", ",a") == 0);
	ASSERT(str_compare("be", "B?e") == 0);
	ASSERT(str_compare("TX", "?Tx") == 0);
	ASSERT(str_compare("rB", "rb") == 0);
	ASSERT(str_compare("pg", "PG") == 0);
	ASSERT(str_compare("?f?Y", "FY") == 0);
	ASSERT(str_compare("uv", "UV") == 0);
	ASSERT(str_compare("O?", ",o'") == 0);
	ASSERT(str_compare("D'", "D!") == 0);
	ASSERT(str_compare("Bo", "bo") == 0);
	ASSERT(str_compare("Ip", "ip") == 0);
	ASSERT(str_compare("  ", "'!") == 0);
	ASSERT(str_compare("Lr", "l r") == 0);
	ASSERT(str_compare(".-", ". ") == 0);
	ASSERT(str_compare(",a", "!A") == 0);
	ASSERT(str_compare("e,", "E-") == 0);
	ASSERT(str_compare(".!", "--") == 0);
	ASSERT(str_compare("K,y", "Ky") == 0);
	ASSERT(str_compare(". ", "??") == 0);
	ASSERT(str_compare("dY", "Dy") == 0);
	ASSERT(str_compare("D!!", "d,") == 0);
	ASSERT(str_compare("M'", "M-") == 0);
	ASSERT(str_compare("kZ", "KZ") == 0);
	ASSERT(str_compare("M?", "m!") == 0);
	ASSERT(str_compare(" H", "?h") == 0);
	ASSERT(str_compare(".eF", "eF") == 0);
	ASSERT(str_compare("-,r", " r") == 0);
	ASSERT(str_compare("Ei", "eI") == 0);
	ASSERT(str_compare("jr", "jr") == 0);
	ASSERT(str_compare(" ?", "..") == 0);
	ASSERT(str_compare("U.", "U ") == 0);
	ASSERT(str_compare("zK", "zK") == 0);
	ASSERT(str_compare("??", ",!") == 0);
	ASSERT(str_compare(",W", "?w") == 0);
	ASSERT(str_compare("ky", "ky") == 0);
	ASSERT(str_compare("?!", ",!") == 0);
	ASSERT(str_compare("?-", "!.") == 0);
	ASSERT(str_compare("a-", "A.-") == 0);
	ASSERT(str_compare(",aP", "AP") == 0);
	ASSERT(str_compare("RC", "Rc") == 0);
	ASSERT(str_compare("!tK", "TK") == 0);
	ASSERT(str_compare("Ft", "fT") == 0);
	ASSERT(str_compare("KZ", "!Kz") == 0);
	ASSERT(str_compare("bz", "b?z") == 0);
	ASSERT(str_compare("BG", "bg") == 0);
	ASSERT(str_compare("'P", "?-p") == 0);
	ASSERT(str_compare("Zt", "Zt") == 0);
	ASSERT(str_compare("fi", "F'I") == 0);
	ASSERT(str_compare("?C", ",c") == 0);
	ASSERT(str_compare("?K,", "k'") == 0);
	ASSERT(str_compare("d'Z", "Dz") == 0);
	ASSERT(str_compare(".Gg", "gG") == 0);
	ASSERT(str_compare("P?", "P!") == 0);
	ASSERT(str_compare("gM", "gM") == 0);
	ASSERT(str_compare("HO", "Ho") == 0);

	printf("# str_compare test passed\n");
}

void test_rev_compare()
{
	ASSERT(str_rev_compare("fsHLgtz R,gZOF.XaChH", "EixBvO,tkJaY!Mx") ==
	       -1);
	ASSERT(str_rev_compare("cDGCP", "GqYYRXFEAfmxBQRyaHeIx.kZ") == -1);
	ASSERT(str_rev_compare("iSw fHqVkRP?!jhbA", "p,uhtMcHf") == -1);
	ASSERT(str_rev_compare(" kns-RadjguIKHYZ XfP'", "iXhhDS!''u") == -1);
	ASSERT(str_rev_compare("ThdqmRjGPNEkQhxfEwfG", "MI'Ts!YstYR") == -1);
	ASSERT(str_rev_compare("ADTASy,q PJczFR,aIt'FB!", "-iU,h,YLPahdVx") ==
	       -1);
	ASSERT(str_rev_compare("hRZGq", "NJhP'ZUy") == -1);
	ASSERT(str_rev_compare("oNUX,", "PZoJz") == -1);
	ASSERT(str_rev_compare("mektkXBrXLZexZC", ".'EAtbg-l,LQS") == -1);
	ASSERT(str_rev_compare("enY?JTHgoNtLn?DrdnARe H",
			       "HHGfvDgGHFjDQXcG dBK") == -1);
	ASSERT(str_rev_compare("WVr'mS!EzKgt!", "SKdvm'JobTRoIZelY,Bx?") == -1);
	ASSERT(str_rev_compare("IasvTRHtdcvfBVgyMNKCb",
			       "MHwr!kktVqHrspOLNJwdy") == -1);
	ASSERT(str_rev_compare("-UdNbq", "DN'x.tv") == -1);
	ASSERT(str_rev_compare("TrmfiGvZmkEW", "TOZGIz?nX!iD!YY") == -1);
	ASSERT(str_rev_compare("ZbnB.HwI", "oXqQzYw!kHYGFqUHssFJXJAU") == -1);
	ASSERT(str_rev_compare("QtCWH?lHWJkEGFgkx,c.FI", "oxj-i!Uct R'W") ==
	       -1);
	ASSERT(str_rev_compare("B!nUj?l'd", "Pxu?lNOThM") == -1);
	ASSERT(str_rev_compare("gzCGvczlB'", "XM!fh-MhfZ") == -1);
	ASSERT(str_rev_compare("KTRyx!MTVRG", "Yr.W?") == -1);
	ASSERT(str_rev_compare("yc!TSeCrPRkq.w", "Obu'c ThUhX") == -1);
	ASSERT(str_rev_compare("gIoIDTPfdH", "whkOIvED?MB?k.WU") == -1);
	ASSERT(str_rev_compare("Ky'amg TqSi!e", "wlWybI,!GEyQDVoKCHY?") == -1);
	ASSERT(str_rev_compare("DklyIAe-vIg", "s.uuFH,FwIm.sLTxuX") == -1);
	ASSERT(str_rev_compare("dzGmh.Q, iIJkaJd", "ktvj?'VLkq") == -1);
	ASSERT(str_rev_compare("vmGQ-kBjbiSHeZdiGtIAvc", "SI?zpSrNdRk") == -1);
	ASSERT(str_rev_compare("p'yms'e-", "?khwc?ssK") == -1);
	ASSERT(str_rev_compare("XrcusPSh", ".inY?PALe.zU") == -1);
	ASSERT(str_rev_compare("lKsQ-s-'MJrfVg.TR", "NJ!vKS EF'.kMLPPqiY") ==
	       -1);
	ASSERT(str_rev_compare("tq'Fjeh", "nMBBDJd'p") == -1);
	ASSERT(str_rev_compare("nNEcwBouVXWVYhB!uAiF", "PLH'tEJBhM!FXQy") ==
	       -1);
	ASSERT(str_rev_compare("p?tNlSrxhByHSIieiIRFs", "mae'lsQPQpV") == -1);
	ASSERT(str_rev_compare("AS'Kn-TbzURL!J", "PrTA?C'F'sazJcws") == -1);
	ASSERT(str_rev_compare(".'ck-'ZlIhiYeCOU",
			       "nPnmjxGQGbMZOMWWM.BXVPyv") == -1);
	ASSERT(str_rev_compare("Yo'W,Lizc", "FlJbUUFaKGL,") == -1);
	ASSERT(str_rev_compare("jYszjlQrN-vmrlQyDiwYAo", "RBgeEWlTHDu? ") ==
	       -1);
	ASSERT(str_rev_compare("mTix!U", "WjtS-hDT.XyJyihARNv") == -1);
	ASSERT(str_rev_compare("OD?KAkMy", "wRp.Uicom,xJZ") == -1);
	ASSERT(str_rev_compare("YqEq.ICQo", "Yb'dLMcX") == -1);
	ASSERT(str_rev_compare("FrDyhp P'L.Yj?yxBJfj-", "LGNwRXvjd,qhSyT") ==
	       -1);
	ASSERT(str_rev_compare("QTiJG.JZPkSZfXBpsPgaoEc",
			       "YGKhddOvJuTmNbfStOz,CuK?") == -1);
	ASSERT(str_rev_compare("dEYG,rvSvO,!", "wHoYi qnAtqw!CiZ") == -1);
	ASSERT(str_rev_compare("peeLX'Cf", "ShMAf,lcRRxR-eS i") == -1);
	ASSERT(str_rev_compare("gRGCJXBWDB", "xMEPLp.J") == -1);
	ASSERT(str_rev_compare("SVmyJbhYXgCGygUPsHvg", "Oqlkec'lbHkyOX") == -1);
	ASSERT(str_rev_compare("!leIirTkaBb", "APhbWHeOP-fv.GqDiJXdLp") == -1);
	ASSERT(str_rev_compare("xc,HYCyd", "bxG'iJTJSKpOEgfP") == -1);
	ASSERT(str_rev_compare("CZlufFYsco YxQ'wviK,BmHt", "ysvlNbW") == -1);
	ASSERT(str_rev_compare("EhyZuqk", "AQ AwLyq?SwPfNpKaEvD W") == -1);
	ASSERT(str_rev_compare("'GTXNTtYwip", "eMlu.'oRprD!fixMaV") == -1);
	ASSERT(str_rev_compare("VryP!PFTGMK", "XKe,S") == -1);
	ASSERT(str_rev_compare("ihyBfvwOlsWFZj", "XM.ZYAxkEttka! jtuY") == -1);
	ASSERT(str_rev_compare("ZPyx,jMDuomeyhDXP'zKqA",
			       "!rFgiBXcBU?-?'h.OQ") == -1);
	ASSERT(str_rev_compare("Usxse'scLd", "rXzXeQS.M") == -1);
	ASSERT(str_rev_compare("!RkaupZpBnKq", "-XYH.gxnuS") == -1);
	ASSERT(str_rev_compare("rQxoM!zNuC'PyB!.hc", "ModomZmc") == -1);
	ASSERT(str_rev_compare("'-HrQuhloOagQa", ",At.IzlmWT") == -1);
	ASSERT(str_rev_compare("ksPxVL,y'VRQCzcXTDhrCUi", "b.t.T") == -1);
	ASSERT(str_rev_compare("wFqBKR' PvoTXQ", "m?gCqmP.or!!vD-pfq,kBS") ==
	       -1);
	ASSERT(str_rev_compare(" quRxsQjpOxaH", "BBJIJVi.CW'SxmJ") == -1);
	ASSERT(str_rev_compare("Cie!HQpoJbXiSEeWjPg",
			       "pCswhiDPSQCKg'ZfSdQB ,As") == -1);
	ASSERT(str_rev_compare("frQVkF", "GRG'GnAvNnuyn") == -1);
	ASSERT(str_rev_compare("ud,'jCSWxMLWvSZe", "zzJhti'Y!cS") == -1);
	ASSERT(str_rev_compare(" ViG??bY,zwMTvTateHf-Ck", "NkFRiiz.aoq'") ==
	       -1);
	ASSERT(str_rev_compare("zZ'DXCaplnkxuOx M", "sjULbKtj kx-T") == -1);
	ASSERT(str_rev_compare("oLINXctpikJatwHOWIojHA", "FkbhlpiTajhLsuah") ==
	       -1);
	ASSERT(str_rev_compare("EhJFotmD", "GSPblDWvgc.Xxi") == -1);
	ASSERT(str_rev_compare("'bO!FEQdCp.'kvO!thcQ,",
			       ",,fhXtA!La!pL?.kWau") == -1);
	ASSERT(str_rev_compare("e!IH'S?cy-h dPBkjC", "eQNhsCfRzbFqOH!oW,Znh") ==
	       -1);
	ASSERT(str_rev_compare("P,AsK!WAw!YjpzRJJ?QJYfGF",
			       "GOTa,mOIxhVogxVzlxP") == -1);
	ASSERT(str_rev_compare("azQiXpKtBWLE", "!-WEIy.,-wdhN") == -1);
	ASSERT(str_rev_compare("qKGfTcUmkHq'sHZAB", "gSPygOTTWaNsyNd.URb,e.") ==
	       -1);
	ASSERT(str_rev_compare(".hqFwHRhzumSLqpjgq", "Ex'LkElHQmOZ") == -1);
	ASSERT(str_rev_compare("g.V crPojQzq'Nrlg",
			       "ZgqYb-AgoQ Aub-rf,EjOHUy") == -1);
	ASSERT(str_rev_compare("eGCFXCxirNB", "Q.'VAjjZKWaPTSFYIpijdC") == -1);
	ASSERT(str_rev_compare("P'xXr", "sHRoSzL FPJiZvHbif?jnkU") == -1);
	ASSERT(str_rev_compare("flDHqXh, sXvhlPIfiVNevl", "DUMSJuxKz") == -1);
	ASSERT(str_rev_compare("yJfnSjAEDMNgmtml", "'EGgskuByjaVP") == -1);
	ASSERT(str_rev_compare("GAUag.XXYJDBdJj", "ODLh?'SrBJzqP !-LOY-.n") ==
	       -1);
	ASSERT(str_rev_compare("YnGOAUJv-bYC", "gBrSoDALg-yNkpXN") == -1);
	ASSERT(str_rev_compare("txZLZstzhyHOqpVkNBQ", "Al.tSlJT") == -1);
	ASSERT(str_rev_compare("zXEKAfICv?rWDJYVYB", "KQxxVknQ") == -1);
	ASSERT(str_rev_compare("! PcBtaQvClLZc", "jntCkGHw") == -1);
	ASSERT(str_rev_compare("vqzNQ rEeD-MCpva?G Jr", "zdsiR'BPi'esj.Jzqs") ==
	       -1);
	ASSERT(str_rev_compare("o'LfpPI,iUCsbgTbHiC,hSYp",
			       "cxNIWXXPJY.?iYNztFOE,y'") == -1);
	ASSERT(str_rev_compare("H,FF ", "FxjW!M!dSBXfzVGYQNhweso") == -1);
	ASSERT(str_rev_compare("KTFeZG", "DzQqRJdN") == -1);
	ASSERT(str_rev_compare("YD,CA", "iRG'JEUBJuIfNF Yb") == -1);
	ASSERT(str_rev_compare("ruvMKFTBQli F'TdExHg", "cmPOR") == -1);
	ASSERT(str_rev_compare("?vbNko", "ebhE-nUc'Mo") == -1);
	ASSERT(str_rev_compare("p'AhKJDrzRhVTSjc'RWbVq", "bfULfm.igu") == -1);
	ASSERT(str_rev_compare("vMSw e", "Og-y?jQ.Z!xO KMP") == -1);
	ASSERT(str_rev_compare("QKQKHYqBhO,yrfTYbBc", "idqCasAg?") == -1);
	ASSERT(str_rev_compare("zabZ,pk,qSU!XFhk", "dDiSNbS") == -1);
	ASSERT(str_rev_compare(".h.B Uaf''iplyfqSsX", "lWpOaxdwLNKbCtjDiV.x") ==
	       -1);
	ASSERT(str_rev_compare("W sAdsgErAqdsaUnjkzqLste", "Bh-XP") == -1);
	ASSERT(str_rev_compare("loYICd?Fq?tdzCc", "WKJGIzBuUCn,") == -1);
	ASSERT(str_rev_compare(" A?idT nYKRUCH?Ai", "nxBp,attX'UGOsKhd-W") ==
	       -1);
	ASSERT(str_rev_compare("eQNoGiGA!edVNnI", "D,HxffVHnQKEM'S") == -1);
	ASSERT(str_rev_compare("pFs,NMT.-CJBdxUui.nxJuVf",
			       ",ka-' !yDp RyObtv") == -1);
	ASSERT(str_rev_compare("OTPLSRprjxMIggKfg",
			       "Jszmnv'OwbJWIZuovMN'y!j") == -1);
	ASSERT(str_rev_compare("KujqSeCrahhbfp?yaTDK",
			       "d?ObgJmAkElyDOykG?n'fF") == 1);
	ASSERT(str_rev_compare("VxPtNgCidQ", "YTpwsTROGofZm") == 1);
	ASSERT(str_rev_compare("-ogkhCJpd yhHAL ", "jJ,QoSnxDAA!WRjvH") == 1);
	ASSERT(str_rev_compare("YhkJ.N", "Qn'P.guq o-hXfTLZwOmm") == 1);
	ASSERT(str_rev_compare(" BHtyB KrIKbJTuj", "wOjIeVRMp!Oa,") == 1);
	ASSERT(str_rev_compare("FUVzv.", "AlXzs,yHhVLTTaEveqkpd-zO") == 1);
	ASSERT(str_rev_compare("i.WOXt.SEoPINNQPmwB,bM!",
			       "GyhGE,LUwDtPRoIiAHJ") == 1);
	ASSERT(str_rev_compare("DaOuBfW", "WdNQKNzjtlvyM MfU") == 1);
	ASSERT(str_rev_compare("e,r-GpzV", "JPaJf?gnPlsp!LgqRnEG") == 1);
	ASSERT(str_rev_compare("lYKmTEOBvcRUtKTboaZzn", "KoDKH") == 1);
	ASSERT(str_rev_compare("XqtAS,MVxsGk CEWhyV'x", ".gXltN,FoYQ") == 1);
	ASSERT(str_rev_compare("vr-bemGDa'ZIOH", "C.aDReZBYG") == 1);
	ASSERT(str_rev_compare("lvcEeVPwdHQ?nH,shE?imsZ'",
			       "kAB.cJPLpwkhB.kiDKtt") == 1);
	ASSERT(str_rev_compare("rfCxxWpRYDYNEiiny", "DsuWD,fEU") == 1);
	ASSERT(str_rev_compare("IJ,oY.IdY v'OI-j", "w.YlrBvmYULclbW'uc") == 1);
	ASSERT(str_rev_compare("zH -LmNnrXGtKjWNu", "BjSvwrLnOZPw,lnGczRXa") ==
	       1);
	ASSERT(str_rev_compare("wD V-QGwx?JX NYWm",
			       "krQjmN?XgQSmmUTtPxquyyQe") == 1);
	ASSERT(str_rev_compare("yKVIfsALY", "iFE!- Axnf-") == 1);
	ASSERT(str_rev_compare("zS nJmWPJ", "UbPaSNCvv-e-?") == 1);
	ASSERT(str_rev_compare("IcWNkwKADJuvdIe aQPc", "oTfT!dA.pOMC") == 1);
	ASSERT(str_rev_compare("DkmW yayYWJS", "Q.ouljSYNFCEU!rRZr") == 1);
	ASSERT(str_rev_compare("iIwPa ", "JFVXSkjugGDmlCtDDpqSmA") == 1);
	ASSERT(str_rev_compare(",u.-Zt!RC", "NxToAAQchMKiqvZgQEFtQqeA") == 1);
	ASSERT(str_rev_compare("u-TivPazRt", "G,sD-LejlpVVJiSK sCcK ") == 1);
	ASSERT(str_rev_compare("aKNmrxRVzLYgI", "OO.CWZYPEhDFiqb") == 1);
	ASSERT(str_rev_compare("JctSXdexwcaU", "GKshAqKWPb'-uP") == 1);
	ASSERT(str_rev_compare("rkTBDF", "WRMTB?") == 1);
	ASSERT(str_rev_compare("IUVZ.RBG?Ro?ngUTnGC TTDr",
			       "qOI-YR aiM'qvBLbb.") == 1);
	ASSERT(str_rev_compare(" sjDwqzkSKeDesYTbCPz", "QzUMmdjEHOuLXQnCW") ==
	       1);
	ASSERT(str_rev_compare("kDsk-'AJ", "tIbHHn.A'pkb.RDtQoR-,X?h") == 1);
	ASSERT(str_rev_compare("IRpadlKFtTt?iwsEnt ipRhJ", "npImnoD!SVf,") ==
	       1);
	ASSERT(str_rev_compare("bishQZ", "Lw?,.XCpZ!!wWI") == 1);
	ASSERT(str_rev_compare("gNYlkbp'HDUWKy", "AgQl!OAq'ojfPByr") == 1);
	ASSERT(str_rev_compare("ku,vfvXsTUPrENktK TZcdle", "?QxGTB") == 1);
	ASSERT(str_rev_compare("ZGMzPNK-F'QBGx", "V.hai'G.RVYwUbTA") == 1);
	ASSERT(str_rev_compare("'.GZDw.QA,PF'WWhhKxq", "NGMgVvbgeiE!qfCFl") ==
	       1);
	ASSERT(str_rev_compare("-DZPOli,I-zSNqTpV?Ui FFw", "K.VbjIDsdENC") ==
	       1);
	ASSERT(str_rev_compare("cSlCYni!w", "SRJEboDYis") == 1);
	ASSERT(str_rev_compare("yNhdcgAaZV", "PWdWqGOeHkYEZ GRTCGa!Zwf") == 1);
	ASSERT(str_rev_compare("YFAcIdVZG.Qz-d", "ATGWIv?tgHzd") == 1);
	ASSERT(str_rev_compare("pShyXM", "KbZYB.HPM") == 1);
	ASSERT(str_rev_compare("gZ?iciBwU.xG!n", "BPG WAX-vbL") == 1);
	ASSERT(str_rev_compare("nspLR-HAj.YvQnJiQowi.yDR",
			       "MI'zvzRXODYySkH-af") == 1);
	ASSERT(str_rev_compare("WmJyXnt?JmJEOOZ", "NUayYkCY!YNwIK-ssn,xsbRf") ==
	       1);
	ASSERT(str_rev_compare("'DITmRtFOF!vh'!aeU,?", "CpQJTSuq") == 1);
	ASSERT(str_rev_compare("aTIlpHI!NcZe", "!FKxst,KzXzRFA") == 1);
	ASSERT(str_rev_compare("yvnh?rSaLdpPI JPDQw-T", "-D,ZenQJPnCe.rt'Gg") ==
	       1);
	ASSERT(str_rev_compare("WcYWosaOdyasKt!", "BUEAd.n") == 1);
	ASSERT(str_rev_compare("YbXfSYaa?ZSy!CLg,-QH", "rEAA?f.,DdKa") == 1);
	ASSERT(str_rev_compare("s, vAYOaH.VF", "'DA!rzo'O!BiHHzDlZzIF") == 1);
	ASSERT(str_rev_compare("LAfGhAmz!fDXZBz-k", "YjG.mvLFA DUq,vAii?CI") ==
	       1);
	ASSERT(str_rev_compare("rydyDbHYhoNOn.TFOr p-",
			       " '!wYOb?eZyo!aNm'vL") == 1);
	ASSERT(str_rev_compare("zIi,fH!j-?zM-x", "bIKnFRRMYKJwGuXBzubT") == 1);
	ASSERT(str_rev_compare("yCBdTY?.X", "odvgyi.cKwSbR") == 1);
	ASSERT(str_rev_compare("cLiajiaRwWMltizI onl'bjk", "UytSBH,hTyTXf") ==
	       1);
	ASSERT(str_rev_compare("gdfAxnw!bpvZpBuQv", "sOtujrmNoqSm") == 1);
	ASSERT(str_rev_compare("Fezuk,sWgg,yQ", ",X.KBALuusMUOwj") == 1);
	ASSERT(str_rev_compare("yyeEdnvW'GZ", "BBdVSyAYXcJ.UOTdPXS") == 1);
	ASSERT(str_rev_compare("Lsz'kKsDyeot", "pDABplkuNsyVGO,d-ZSF!") == 1);
	ASSERT(str_rev_compare("zLM B,s,i,Twr,z", "gLOzSj-PUBvaOxssFqiJB") ==
	       1);
	ASSERT(str_rev_compare("Vf.bhqkOgifDLIS", "ciYKcuoIkMSmFEYScLOLtPiD") ==
	       1);
	ASSERT(str_rev_compare("inJVFtpn", "pwLMhBggtd") == 1);
	ASSERT(str_rev_compare("yBFafzzUkfUozcFp'", "PTZNkvvF") == 1);
	ASSERT(str_rev_compare("mi yd.BQufZKT", "rjJbHbASri") == 1);
	ASSERT(str_rev_compare("!nHXbPXxyelv.pbLyzWH", "DuQseex!e") == 1);
	ASSERT(str_rev_compare("NEycKibtpTzkT,dv?JwmlZ", "hnnh--mL,xk Jysaf") ==
	       1);
	ASSERT(str_rev_compare("aadeIL'XRRqPmR", "eGUeAVIODeGa'RHN-.yo") == 1);
	ASSERT(str_rev_compare("Mu ?g Gh-hJxBNx!y'Fmksx",
			       "knc,.lyqss dRe?L?dEv") == 1);
	ASSERT(str_rev_compare("Duwy,vYRM!TKTtRUv", "FubdE,hwXrcFoilg") == 1);
	ASSERT(str_rev_compare("tKUU'fAXLFdykH ", "WOUC-lUuEQiHJ.sr!-FO-.d") ==
	       1);
	ASSERT(str_rev_compare("f,aau edyTMyv!CirlefQ?p", "Wcqej!") == 1);
	ASSERT(str_rev_compare("!BBc-V.huXeL?xqOJV",
			       "XYybw?GsHiuRltbpRbebJfgV") == 1);
	ASSERT(str_rev_compare("aeB?NHU", "qyOeBBKe'kxtZII") == 1);
	ASSERT(str_rev_compare("wM,hZ", "'Y'!c..rT?dy ") == 1);
	ASSERT(str_rev_compare("jYJ!FFGLQJPfkveksKnmW", "VBundAfTdLNCWVUo") ==
	       1);
	ASSERT(str_rev_compare("VzTeRO-fFxSHOGdHjDE", "?EOxdTTbDE") == 1);
	ASSERT(str_rev_compare("k?FbTzbK CmGva.oW", "PAnyjVcNbVRMQOf") == 1);
	ASSERT(str_rev_compare("H,SyJA.ovzbz", "EN'pzyRoTxPxBUXEZgCYomG") == 1);
	ASSERT(str_rev_compare("SoR!YVTvLK", "Z,l!mJIZIuA") == 1);
	ASSERT(str_rev_compare("N!ErEa YMiwBD,nyKfNSYVrg",
			       "Q.MIIrOnwiURcYygJa") == 1);
	ASSERT(str_rev_compare("FQ!I.UnkKnCbO-WQ", "NfNwNPMpYCDMmg-yG.J") == 1);
	ASSERT(str_rev_compare("uTzLvRGa.jZ te", "hA.eXbodbVTXGY.ed A") == 1);
	ASSERT(str_rev_compare(" yIf B !!y", "kPexQQED,cfGrZDpeHZSOVK") == 1);
	ASSERT(str_rev_compare("JrpLy", "hrLcbsM'-") == 1);
	ASSERT(str_rev_compare("oYn!gStqbJP", "jTCvf-AewMpB") == 1);
	ASSERT(str_rev_compare("pJl,-HcPSCZaRL aPUV", "BjkgnnDYXRNd") == 1);
	ASSERT(str_rev_compare("PSIc fqC gcCfYbXmfi", "XYPdgirV zUS-a") == 1);
	ASSERT(str_rev_compare("RBShUUI'QuPJ'NhMDEzXP.", "FWqSiQglnKfe ZD") ==
	       1);
	ASSERT(str_rev_compare("aJc.FkdP", "'H Kc.WB?p") == 1);
	ASSERT(str_rev_compare("nnMdS jbS.pMGPLn", "?eq?CqGp?d !vwcNmk") == 1);
	ASSERT(str_rev_compare("jzO-kCX!bAvnJvv", "TQiFWxKmXL") == 1);
	ASSERT(str_rev_compare("SBzTwGIhPegZ'I,POLIJN", "juNjpE") == 1);
	ASSERT(str_rev_compare("ITxENNIPEVs", "Yo!YH'ixGZrPuf") == 1);
	ASSERT(str_rev_compare(".D qgygHUGSuX", "hajvgHe,GFVa") == 1);
	ASSERT(str_rev_compare("d,FmXvnXSIybaXyjSxY!J",
			       "BGBKmRAP'bCBofGTHYdxSPTE") == 1);
	ASSERT(str_rev_compare("uebtVk", "VJwgfSNpuJrlwC.J") == 1);
	ASSERT(str_rev_compare("Q?jyTKPscSqV.", "S kTvxlrU") == 1);
	ASSERT(str_rev_compare("iei'k'vT", "mNxhJZK") == 1);
	ASSERT(str_rev_compare("by'-yRy gZwJ .,", "yq,vqk!RdvLqpUNd") == 1);
	ASSERT(str_rev_compare("oMaSwp?MhK-", "oSydAWf,RXu.Uae") == 1);
	ASSERT(str_rev_compare("p.", "p,") == 0);
	ASSERT(str_rev_compare("S ", "S,") == 0);
	ASSERT(str_rev_compare(".c", "?C") == 0);
	ASSERT(str_rev_compare(",O", "?O") == 0);
	ASSERT(str_rev_compare("LR.Y", "lry") == 0);
	ASSERT(str_rev_compare("  ", "''") == 0);
	ASSERT(str_rev_compare("'f", " f") == 0);
	ASSERT(str_rev_compare("?U", "..U") == 0);
	ASSERT(str_rev_compare("g?", "g-") == 0);
	ASSERT(str_rev_compare("Q,H", "QH") == 0);
	ASSERT(str_rev_compare("s-", "s!") == 0);
	ASSERT(str_rev_compare("M'", "m ") == 0);
	ASSERT(str_rev_compare("dP", "dP") == 0);
	ASSERT(str_rev_compare("!?", "!.") == 0);
	ASSERT(str_rev_compare("uj", "Uj") == 0);
	ASSERT(str_rev_compare("x!", "x,,") == 0);
	ASSERT(str_rev_compare("O,", "O'") == 0);
	ASSERT(str_rev_compare("ii-", "I !I?") == 0);
	ASSERT(str_rev_compare("-!", " ,") == 0);
	ASSERT(str_rev_compare("kr", "KR") == 0);
	ASSERT(str_rev_compare("KC", "kc") == 0);
	ASSERT(str_rev_compare("uQ-", "uQ") == 0);
	ASSERT(str_rev_compare("D ", "D-") == 0);
	ASSERT(str_rev_compare("ez", "Ez,") == 0);
	ASSERT(str_rev_compare("E.", "E.") == 0);
	ASSERT(str_rev_compare(",X", " x") == 0);
	ASSERT(str_rev_compare("QZ'", "Qz") == 0);
	ASSERT(str_rev_compare("G!", "g'") == 0);
	ASSERT(str_rev_compare("Wh", "Wh") == 0);
	ASSERT(str_rev_compare("k-", "k-") == 0);
	ASSERT(str_rev_compare("Hb", "hb") == 0);
	ASSERT(str_rev_compare("u!", "u!") == 0);
	ASSERT(str_rev_compare("qM", "qM") == 0);
	ASSERT(str_rev_compare("am", "AM") == 0);
	ASSERT(str_rev_compare("vgJ", "vgJ") == 0);
	ASSERT(str_rev_compare(",D", " D") == 0);
	ASSERT(str_rev_compare("f ", "f!") == 0);
	ASSERT(str_rev_compare("!!", ",?") == 0);
	ASSERT(str_rev_compare("a-", "a'-") == 0);
	ASSERT(str_rev_compare("NW", "Nw") == 0);
	ASSERT(str_rev_compare("n.", "n-") == 0);
	ASSERT(str_rev_compare("Zw", "zw") == 0);
	ASSERT(str_rev_compare(".?", "- ") == 0);
	ASSERT(str_rev_compare("qt", "qt") == 0);
	ASSERT(str_rev_compare("Z,z", "zz") == 0);
	ASSERT(str_rev_compare("B.", "b ") == 0);
	ASSERT(str_rev_compare("?K-", " k") == 0);
	ASSERT(str_rev_compare("sO", "So") == 0);
	ASSERT(str_rev_compare("aH", "a-H") == 0);
	ASSERT(str_rev_compare("JP?", "j,P") == 0);
	ASSERT(str_rev_compare(".r", "?r") == 0);
	ASSERT(str_rev_compare("ft", "FT") == 0);
	ASSERT(str_rev_compare("yo", "Y'O") == 0);
	ASSERT(str_rev_compare("vU", "VU?") == 0);
	ASSERT(str_rev_compare("-l", ",-l") == 0);
	ASSERT(str_rev_compare("KJ", "Kj") == 0);
	ASSERT(str_rev_compare("B,H", "B'h") == 0);
	ASSERT(str_rev_compare("T'", "T-.") == 0);
	ASSERT(str_rev_compare("' ", " .") == 0);
	ASSERT(str_rev_compare("Kz", "KZ") == 0);
	ASSERT(str_rev_compare("sB", "sB") == 0);
	ASSERT(str_rev_compare("?k-", ",K") == 0);
	ASSERT(str_rev_compare("gn", "gN") == 0);
	ASSERT(str_rev_compare("D.z", "Dz") == 0);
	ASSERT(str_rev_compare("iI-.", "ii'?") == 0);
	ASSERT(str_rev_compare("--b", "?b") == 0);
	ASSERT(str_rev_compare("'U", "-u") == 0);
	ASSERT(str_rev_compare("tbe", "tBe") == 0);
	ASSERT(str_rev_compare("h'", "H.") == 0);
	ASSERT(str_rev_compare("zi", "ZI") == 0);
	ASSERT(str_rev_compare("'W", "-w") == 0);
	ASSERT(str_rev_compare("Pz", "pz") == 0);
	ASSERT(str_rev_compare("fv", "F v") == 0);
	ASSERT(str_rev_compare(" n", ",n") == 0);
	ASSERT(str_rev_compare("Bi", "BI") == 0);
	ASSERT(str_rev_compare("fK", "FK") == 0);
	ASSERT(str_rev_compare("c'", "C?") == 0);
	ASSERT(str_rev_compare("B!!", "b.") == 0);
	ASSERT(str_rev_compare("rW", "RW") == 0);
	ASSERT(str_rev_compare(" y", "!y") == 0);
	ASSERT(str_rev_compare("'m,", "-M") == 0);
	ASSERT(str_rev_compare("?M.", "!M") == 0);
	ASSERT(str_rev_compare("al", "Al") == 0);
	ASSERT(str_rev_compare("DR", "dr") == 0);
	ASSERT(str_rev_compare("Qm!", "qm") == 0);
	ASSERT(str_rev_compare(" a", ".A") == 0);
	ASSERT(str_rev_compare("x-", "x'") == 0);
	ASSERT(str_rev_compare("wZ", "wZ") == 0);
	ASSERT(str_rev_compare("d.", "D,") == 0);
	ASSERT(str_rev_compare(",,'", "?,") == 0);
	ASSERT(str_rev_compare("OC", "O!C") == 0);
	ASSERT(str_rev_compare("Fc", "fC") == 0);
	ASSERT(str_rev_compare("L?", "l!") == 0);
	ASSERT(str_rev_compare("Mc", "mc") == 0);
	ASSERT(str_rev_compare("TR", "tr") == 0);
	ASSERT(str_rev_compare("Z-", "Z,") == 0);
	ASSERT(str_rev_compare("ex", "EX") == 0);
	ASSERT(str_rev_compare("!J", "?j") == 0);
	ASSERT(str_rev_compare("TG", "TG") == 0);
	ASSERT(str_rev_compare("QG", "qG") == 0);

	printf("# str_rev_compare test passed\n");
}

int c_int_cmp(const void *a, const void *b)
{
	return (*(const int *)a) - (*(const int *)b);
}
#endif

void sort_file_lines(const char *infile, const char *outfile, char reverse)
{
	ASSERT(infile != NULL);
	ASSERT(outfile != NULL);

	printf("# Reading file...\n");

	char *file;
	MEASURE(file = read_file(infile))

	printf("# Done\n# Processing...\n");

	size_t linenum;
	MEASURE(linenum = replace(file, '\n', '\0'))

	string *lines;
	MEASURE(lines = gen_strings(file, linenum, '\0'))

	MEASURE(q_sort(lines, linenum, sizeof(string),
		       reverse ? c_string_rev_compare : c_string_compare));

	printf("# Done\n# Writing file...\n");

	MEASURE(write_file_strings(outfile, lines, linenum))
	printf("# Done\n# Exiting...\n");

	free(file);
	free(lines);
}

void print_usage(const char *name)
{
	printf("# LineSorter v2 by InversionSpaces\n");
	printf("# Sorts lines of INPUT in STRAIGHT|REVERSE order and writes it to OUPUT\n");
	printf("# Usage: %s INPUT OUTPUT STRAIGHT|REVERSE\n", name);
}

size_t len(const char *str)
{
	ASSERT(str != NULL);

	char *tmp = (char *)str;
	while (*tmp)
		++tmp;

	return (size_t)(tmp - str) + 1;
}

int is_alphac(const char c)
{
	return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

int is_capitalc(const char c)
{
	if ('A' <= c && c <= 'Z')
		return 1;
	return 0;
}

char to_lowerc(const char c)
{
	return (is_capitalc(c)) ? (c - TOLOWER_OFFSET) : c;
	
}

int str_compare(const char *str1, const char *str2)
{
	ASSERT(str1 != NULL);
	ASSERT(str2 != NULL);

	char *s1 = (char *)str1;
	char *s2 = (char *)str2;
	while (*s1 && *s2) {
		while (*s1 && !is_alphac(*s1))
			++s1;
		while (*s2 && !is_alphac(*s2))
			++s2;

		if (!*s1 && !*s2)
			return 0;
		if (!*s1)
			return -1;
		if (!*s2)
			return 1;

		char lc1 = to_lowerc(*s1);
		char lc2 = to_lowerc(*s2);

		if (lc1 < lc2)
			return -1;
		if (lc1 > lc2)
			return 1;

		++s1;
		++s2;
	}

	if (!*s1 && !*s2)
		return 0;
	if (!*s1)
		return -1;
	if (!*s2)
		return 1;

	ASSERT(0); ///< Недостижимый код
	return 0;
}

int string_compare(const string *str1, const string *str2)
{
	ASSERT(str1 != NULL);
	ASSERT(str2 != NULL);

	return str_compare(str1->str, str2->str);
}

/* Внутренняя функция сравнения инверсий строк */
int __str_rev_compare(const char *str1, size_t len1, const char *str2,
		      size_t len2)
{
	ASSERT(str1 != NULL);
	ASSERT(str2 != NULL);

	const char *s1 = str1 + len1 - 2;
	const char *s2 = str2 + len2 - 2;
	while (s1 >= str1 && s2 >= str2) {
		while (s1 >= str1 && !is_alphac(*s1))
			--s1;
		while (s2 >= str2 && !is_alphac(*s2))
			--s2;

		if (s1 + 1 == str1 && s2 + 1 == str2)
			return 0;
		if (s1 + 1 == str1)
			return -1;
		if (s2 + 1 == str2)
			return 1;

		char lc1 = to_lowerc(*s1);
		char lc2 = to_lowerc(*s2);

		if (lc1 < lc2)
			return -1;
		if (lc1 > lc2)
			return 1;

		--s1;
		--s2;
	}

	if (s1 + 1 == str1 && s2 + 1 == str2)
		return 0;
	if (s1 + 1 == str1)
		return -1;
	if (s2 + 1 == str2)
		return 1;

	ASSERT(0); ///< Недостижимый код
	return 0;
}

int str_rev_compare(const char *str1, const char *str2)
{
	ASSERT(str1 != NULL);
	ASSERT(str2 != NULL);

	return __str_rev_compare(str1, len(str1), str2, len(str2));
}

int string_rev_compare(const string *str1, const string *str2)
{
	ASSERT(str1 != NULL);
	ASSERT(str2 != NULL);

	return __str_rev_compare(str1->str, str1->len, str2->str, str2->len);
}

void swap_str(char **str1, char **str2)
{
	ASSERT(str1 != NULL);
	ASSERT(*str1 != NULL);
	ASSERT(str2 != NULL);
	ASSERT(*str2 != NULL);

	if (str1 == str2)
		return;

	char *tmp = *str1;
	*str1 = *str2;
	*str2 = tmp;
}

void swap_string(string *str1, string *str2)
{
	ASSERT(str1 != NULL);
	ASSERT(str2 != NULL);

	if (str1 == str2)
		return;

	string tmp = *str1;
	*str1 = *str2;
	*str2 = tmp;
}

char *read_file(const char *filename)
{
	ASSERT(filename != NULL);

	FILE *fp = xfopen(filename, "rb");
	size_t size = file_size(fp);

	char *retval = (char *)xmalloc(size + 1);

	size_t readed = fread(retval, 1, size, fp);
	if (readed != size) {
		printf("# ERROR: Failed to read file: %s. \
					Exiting...\n", filename);
		fclose(fp);
		exit(2);
	}
	retval[size] = 0;

	fclose(fp);

	return retval;
}

size_t replace(char *str, char from, char to)
{
	ASSERT(str != NULL);

	size_t count = 0;
	for (; *str; ++str) {
		if (*str == from) {
			*str = to;
			++count;
		}
	}

	return count;
}

char **gen_strs(const char *str, size_t numpoints, char stopc)
{
	ASSERT(str != NULL);

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

string *gen_strings(const char *str, size_t numpoints, char stopc)
{
	ASSERT(str != NULL);

	string *retval = (string *)xmalloc(numpoints * sizeof(string));

	size_t pos = 0;
	for (size_t i = 0; i < numpoints; ++i) {
		retval[i].str = (char *)str + pos;

		size_t spos = pos;
		while (str[pos] != stopc)
			++pos;
		retval[i].len = pos - spos + 1;

		++pos;
	}

	return retval;
}

void bubble_sort_str(char **array, size_t size,
		     int (*comp)(const char *, const char *))
{
	ASSERT(array != NULL);

	char found = 0;
	for (size_t n = 0; n < size; ++n) {
		found = 0;
		for (char **i = array; i - size + 1 < array; ++i)
			if (comp(*i, *(i + 1)) > 0) {
				swap_str(i, i + 1);
				found = 1;
			}
		if (!found)
			return;
	}
}

void bubble_sort_string(string *array, size_t size,
			int (*comp)(const string *, const string *))
{
	ASSERT(array != NULL);

	char found = 0;
	for (size_t n = 0; n < size; ++n) {
		found = 0;
		for (string *i = array; i - size + 1 < array; ++i)
			if (comp(i, i + 1) > 0) {
				swap_string(i, i + 1);
				found = 1;
			}
		if (!found)
			return;
	}
}

void quick_sort_str(char **array, size_t size,
		    int (*comp)(const char *, const char *))
{
	ASSERT(array != NULL);

	if (size < 2)
		return;

	if (size < 10) {
		bubble_sort_str(array, size, comp);
		return;
	}

	char **l = array;
	char **r = array + size - 1;
	char **p = array + size / 2;

	while (l < r) {
		while (comp(*l, *p) < 0 && l < p)
			++l;
		while (comp(*r, *p) >= 0 && p < r)
			--r;
		if (l == r)
			break;

		swap_str(l, r);

		if (l == p)
			p = r;
		else if (r == p)
			p = l;
	}

	quick_sort_str(array, (size_t)(p - array), 					comp);
	quick_sort_str(p + 1, (size_t)((array + size) - (p + 1)), 	comp);
}

void quick_sort_string(string *array, size_t size,
		       int (*comp)(const string *, const string *))
{
	ASSERT(array != NULL);

	if (size < 2)
		return;

	if (size < 10) {
		bubble_sort_string(array, size, comp);
		return;
	}

	string *l = array;
	string *r = array + size - 1;
	string *p = array + size / 2;

	while (l < r) {
		while (comp(l, p) < 0 && l < p)
			++l;
		while (comp(r, p) >= 0 && p < r)
			--r;
		if (l == r)
			break;

		swap_string(l, r);

		if (l == p)
			p = r;
		else if (r == p)
			p = l;
	}

	quick_sort_string(array, (size_t)(p - array), comp);
	quick_sort_string(p + 1, (size_t)((array + size) - (p + 1)), comp);
}

void write_file_str(const char *filename, char **lines, size_t linenum)
{
	ASSERT(filename != NULL);
	ASSERT(lines != NULL);

	FILE *fp = xfopen(filename, "wt");

	for (size_t i = 0; i < linenum; ++i) {
		ASSERT(lines[i] != NULL);
		fprintf(fp, "%s\n", lines[i]);
	}

	fclose(fp);
}

void write_file_strings(const char *filename, string *lines, size_t linenum)
{
	ASSERT(filename != NULL);
	ASSERT(lines != NULL);

	FILE *fp = xfopen(filename, "wt");

	for (size_t i = 0; i < linenum; ++i) {
		ASSERT(lines[i].str != NULL);
		fprintf(fp, "%s\n", lines[i].str);
	}

	fclose(fp);
}

void clear_input()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {
	}
}

FILE *xfopen(const char *fname, const char *mod)
{
	FILE *retval = fopen(fname, mod);
	if (retval == NULL) {
		printf("# ERROR: Failed to open file: %s. Exiting...\n", fname);
		exit(1);
	}

	return retval;
}

void *xmalloc(size_t size)
{
	void *retval = malloc(size);
	if (retval == NULL) {
		printf("# ERROR: Failed to allocate memory. Exiting...\n");
		exit(2);
	}

	return retval;
}

size_t file_size(FILE *file)
{
	ASSERT(file != NULL);

	size_t retval = 0;
	
#ifdef __unix__
	struct stat st;
	int err = fstat(fileno(file), &st);
	if (err != 0) {
#endif
		fseek(file, 0L, SEEK_END);
		retval = ftell(file);
		fseek(file, 0L, SEEK_SET);
#ifdef __unix__
	}
	retval = st.st_size;
#endif

	return retval;
}

stack *stack_init(size_t elsize)
{
	stack *retval = (stack *)xmalloc(sizeof(stack));
	retval->elsize = elsize;
	retval->size = INITIAL_STACK_SIZE * elsize;
	retval->bottom = xmalloc(retval->size);
	retval->top = retval->bottom;
	return retval;
}

void stack_push(stack *stackp, void *datap)
{
	ASSERT(stackp != NULL);
	ASSERT(datap != NULL);

	if (stackp->size == (size_t)(stackp->top - stackp->bottom)) {
		size_t nsize = stackp->size + STACK_CHUNK_SIZE * stackp->elsize;
		stackp->bottom = realloc(stackp->bottom, nsize);
		stackp->top = stackp->bottom + stackp->size;
		stackp->size = nsize;
	}
	memcpy(stackp->top, datap, stackp->elsize);
	stackp->top += stackp->elsize;
}

void stack_pop(stack *stackp, void *datap)
{
	ASSERT(stackp != NULL);
	ASSERT(datap != NULL);

	stackp->top -= stackp->elsize;
	memcpy(datap, stackp->top, stackp->elsize);
}

void stack_deinit(stack *stackp)
{
	ASSERT(stackp != NULL);

	free(stackp->bottom);
	free(stackp);
}

char stack_is_empty(stack *stackp)
{
	ASSERT(stackp != NULL);

	return (stackp->top == stackp->bottom);
}

void swap(void *a, void *b, void *buf, size_t size)
{
	ASSERT(a != NULL);
	ASSERT(b != NULL);
	ASSERT(buf != NULL);

	memcpy(buf, a, size);
	memcpy(a, b, size);
	memcpy(b, buf, size);
}

void q_sort(void *arr, size_t n, size_t size,
	    int (*cmp)(const void *, const void *))
{
	ASSERT(arr != NULL);

	stack *st = stack_init(sizeof(void *));
	void *t = xmalloc(size);

	void *l = arr;
	void *r = arr + size * (n - 1);

	void *ls = NULL;
	void *rs = NULL;
	void *p = NULL;

	stack_push(st, &r);
	stack_push(st, &l);

	while (!stack_is_empty(st)) {
		stack_pop(st, &l);
		stack_pop(st, &r);

		size_t tsize = (r - l) / size + 1;

		if (tsize < 2)
			continue;

		ls = l;
		rs = r;
		p = l + (tsize / 2) * size;

		while (l < r) {
			while (cmp(p, l) > 0 && l < p)
				l += size;
			while (cmp(p, r) <= 0 && r > p)
				r -= size;

			if (l == r)
				break;

			swap(l, r, t, size);

			if (p == l)
				p = r;
			else if (p == r)
				p = l;
		}

		if (p > ls && p - size > ls) {
			p -= size;
			stack_push(st, &p);
			p += size; // Для следующего if
			stack_push(st, &ls);
		}
		
		if (p < rs && p + size < rs) {
			stack_push(st, &rs);
			p += size;
			stack_push(st, &p);
			//p -= size; // Для симметрии
		}
	}

	stack_deinit(st);
	free(t);
}
