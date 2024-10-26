/* В adding_loop(просто цикл) в котором вызывается: 
 * add_element (rdi - число из массива дата, rsi - ссылка на предыдущий элемен ) добавляются элементы в связанный список через malloc.аким образом создается связный список в памяти, созданный задом наперед, последний элемент не имеет ссылку на другой элемент, в отличие от первого.
 * m(rdi - ссылка на первый элемент в куче, rsi - ссылка на функцию print_int) печатает все значения из связного списка, разделяя пробелами 
 * print_int() вызывает printf("%ld ", long int )
 * puts() - просто пустую строку вставляет
 * f() поиск четного числа в структуре, начиная с первого элемента
 * p()
 * */
#include <stdio.h>
#include <stdlib.h>

// global var
long int data[] = {4, 8, 15, 16, 23, 42};
long int data_lenght = sizeof(data) / 8;

// func declaration
void* add_element(long int data_element, void* p_to_prev);
void like_m(void **pp_data); // also pointer to print_int (call printf inside)
void* like_f(void **pp_data);
void Clear(void *p_data);

int main(void)
{
	long int data_iter = data_lenght - 1;
	void* p_data = NULL;
	
	do
	{
		p_data = add_element(data[data_iter], p_data);
		
		//printf("data_el[%ld] = %ld\n", data[data_iter], data_iter);
	}while (--data_iter >= 0);
#if 0
	void ** pp_data = &p_data;
	printf("**pp_data = %ld\n", *(long int*)(*pp_data));
#endif
	like_m(&p_data);

	puts(&(const char){0});
	
	void *p_even_data = like_f(&p_data);
	like_m(&p_even_data);
	puts(&(const char){0});

	Clear(p_data);
	Clear(p_even_data);
	return 0;
}

void* add_element(long int data_element, void* p_to_prev)
{

	long int* heap_data = malloc(16);
	*heap_data = data_element;
	
	void ** pp_to_ref =(void **)( heap_data + 1);
	*pp_to_ref = p_to_prev;

	return heap_data;
}


void like_m(void **pp_data)
{
	while(*pp_data != NULL)
	{
		//long int * ptr_inside_struct = p_data;
		printf("%ld ",*((long int*)(*pp_data))); // print num in pointer
		fflush(0); // to out immediately clear stdin ? not out 
		pp_data = (void**)((long int*)*pp_data +1); // try to shift pointer to 8 byte
	}
}

void* like_f(void **pp_data)
{
	void *p_to_even = NULL;
	while(*pp_data != NULL)
	{
		// even sign
		if( *(long int*)(*pp_data) % 2 == 1)
		{
			p_to_even = add_element(*(long int*)(*pp_data), p_to_even);
			//printf("%ld ",*(long int*)(*p_data));
		}
		pp_data = (void**)((long int*)*pp_data + 1);
	}

	return p_to_even;
}
void Clear(void *p_data)
{
	while(p_data != NULL)
	{
		void *p_cleared = p_data;
		p_data = *(void**)((long int*)p_data + 1);
		free(p_cleared);
	}
}

