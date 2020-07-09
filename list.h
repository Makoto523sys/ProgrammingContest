#include <stdio.h>
#include <stdlib.h>

struct node {
  int value;
  struct node* prev;
  struct node* next;
};

struct list {
  struct node* head;
  struct node* tail;
};


inline void initialize(struct list* p) {
  p->head = (struct node*)malloc(sizeof(struct node));
  p->tail = (struct node*)malloc(sizeof(struct node));
  p->head->prev = 0;
  p->head->next = p->tail;
  p->tail->prev = p->head;
  p->tail->next = 0;
}

inline void insertBefore(struct node* p, int v) {
  struct node* newNode = (struct node*)malloc(sizeof(struct node));
  newNode->value = v;
  newNode->next = p;
  p->prev->next = newNode;
  newNode->prev = p->prev;
  p->prev = newNode;
}

inline void insertAfter(struct node* p, int v) {
  struct node* newNode = (struct node*)malloc(sizeof(struct node));
  newNode->value = v;
  newNode->prev = p;
  p->next->prev = newNode;
  newNode->next = p->next;
  p->next = newNode;
}

inline void erase(struct node* p) {
  p->prev->next = p->next;
  p->next->prev = p->prev;
  free(p);
}

inline void printNumbers(struct list* p) {
  for (struct node* i = p->head->next; i->next != 0; i = i->next)
    printf("%d ", i->value);
  printf("\n");
}

inline void printString(struct list* p) {
  for (struct node* i = p->head->next; i->next != 0; i = i->next)
    printf("%c", i->value);
  printf("\n");
}
