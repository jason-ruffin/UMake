#include <stddef.h>

struct list_struct;
typedef struct list_struct list_t;
typedef void* list_iterator_t;

list_t* list_new();
void list_free(list_t* list);

void list_append(list_t* list, void* item);
size_t list_length(list_t* list);

list_iterator_t list_first(list_t* list);
list_iterator_t list_last(list_t* list);

void* iterator_item(list_iterator_t* iterator);
void iterator_next(list_iterator_t* iterator);
