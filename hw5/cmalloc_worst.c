#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

typedef struct Node node;

struct Node {
  int isUsed;
  node *prev, *next;
  int size;
};

node *head = NULL, *tail = NULL;

void *more(int size) {
  void *p = sbrk(size + sizeof(node));
  node *sp = (node*)p;
  if (head == NULL)
    head = p;
  sp->prev = tail;
  sp->next = NULL;
  sp->size = size;
  sp->isUsed = 0;
  if (tail)
    tail->next = sp;
  return tail = sp;
}

void *cmalloc(int size) {
  node *sp = NULL, *cp;
  for (cp = head; cp; cp = cp->next)
    if (!cp->isUsed && cp->size > size && (sp == NULL || cp->size < sp->size))
      sp = cp;
  if (!sp) sp = more(size);
  if (sp->size > size + sizeof(node)) {
    node *p = (node*)((void*)sp + size);
    p->prev = sp;
    p->next = sp->next;
    p->size = sp->size - size - sizeof(node);
    p->isUsed = 0;
    if (sp->next) sp->next->prev = p;
    sp->next = p;
    sp->size = size;
    sp->isUsed = 1;
  } else {
    sp->isUsed = 1;
  }
  return (void*)(sp + 1);
}

node *merge(node* a, node* b) {
  //a->next == b
  a->next = b->next;
  a->size = a->size + b->size + sizeof(node);
  return a;
}

void free(void *p) {
  node *sp = (node*)p - 1;
  sp->isUsed = 0;
  if (sp->prev && sp->prev->isUsed == 0)
    sp = merge(sp->prev, sp);
  if (sp->next && sp->next->isUsed == 0)
    sp = merge(sp, sp->next);
}

int main() {
  void *a = cmalloc(128);
  printf("%p\n", a);
  void *b = cmalloc(256);
  printf("%p\n", b);
  void *c = cmalloc(512);
  printf("%p\n", c);
  void *d = cmalloc(1024);
  printf("%p\n", d);
  free(a);
}
