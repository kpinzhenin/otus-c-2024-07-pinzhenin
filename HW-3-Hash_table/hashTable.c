#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// struct of tables entry
struct string_hash
{
	int key_count;
	struct string_hash *next_hash;
	char key[];
};

void string_hash_add_entry(struct string_hash *ptr_table, char *key, size_t len);

struct string_hash* string_hash_create_entry(char *key, size_t len);

void string_hash_print(struct string_hash *ptr_table);

void string_hash_clear(struct string_hash * ptr_to_next_element);

int main(int argc, char *argv[])
{

	FILE *fp_in = fopen(argv[1],"r");
	
	if (fp_in == NULL)
	{
		printf("Error. Open file %s", argv[1]);
		exit(1);
	}
	
	struct string_hash *ptr_table = NULL;
	
	char input_word[256]; // max lenght of word - 256 symbols
	
	while ( (fscanf(fp_in, "%s", input_word  )) > 0)
	{
		size_t readed_word_len = strlen(input_word) + 1; // +1 for '\0' terminator
								 //
		// for empty hash table create first entry
		if (ptr_table == NULL)
			ptr_table = string_hash_create_entry(input_word, readed_word_len);
		else
			string_hash_add_entry(ptr_table, input_word, readed_word_len);
	}
	string_hash_print(ptr_table);
	string_hash_clear(ptr_table);
	fclose(fp_in);
	return 0;
}

void string_hash_add_entry(struct string_hash *ptr_table, char *key, size_t len)
{
	while(ptr_table != NULL)	
	{
		// match key entry
		if(strcmp(ptr_table->key, key) == 0)
		{
			ptr_table->key_count++;
			return;
		}
		// mean - last entry (then not contain match key)
		else if(ptr_table->next_hash == NULL)
		{
			// add new entry
			struct string_hash *ptr_string_hash = string_hash_create_entry(key, len); 
			if (ptr_string_hash != NULL)
				ptr_table->next_hash = ptr_string_hash;
			return;
		}		
		// move to next entry
		ptr_table = ptr_table->next_hash;
	}	
}

struct string_hash* string_hash_create_entry(char *key, size_t len)
{
	// assign memory for new entry
	struct string_hash *new_entry = (struct string_hash *)malloc(sizeof(struct string_hash) + len);
	if (new_entry == NULL)			
		printf("NULL was returned to %s with len %ld \n", key, len);
	else
	{
		strcpy(new_entry->key, key);
		new_entry->key_count = 1;
	}
	return new_entry;
}

void string_hash_print(struct string_hash *ptr_table)
{
	while(ptr_table != NULL)
	{
		printf("<%s> : %d \n", ptr_table->key, ptr_table->key_count);
		ptr_table = ptr_table->next_hash;		
	}

}

void string_hash_clear(struct string_hash * ptr_to_next_element)
{
	while(ptr_to_next_element != NULL)
	{
		// save ptr to next element from deleting element
		struct string_hash *tmp = ptr_to_next_element->next_hash;
		free((void*)ptr_to_next_element);
		ptr_to_next_element = tmp;
	}
}
