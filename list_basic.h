#define NOERR 0
#define NULLINITSIZE 1
#define ALLOCFAILURE 2
#define REALLOCFAILURE 3

struct list;

int list_init(struct list* list, size_t initial_size, size_t elementsize);
int list_get(struct list* list, void* element, int
 index);
int list_set(struct list* list, void* element, int
 index);
