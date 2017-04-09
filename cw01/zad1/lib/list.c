#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// typedef enum {NAME,SURNAME,EMAIL,PHONE,BIRTHDATE,ADDRESS,UNDEFINED} key_of_structure;
//contacts are searched by key, key also is used during sort
#define NODE_ATTRIBUTES 6



list_contacts * create_list (key_of_structure  keyList){
  key_of_structure key = keyList;


  list_contacts * newList = (list_contacts* ) malloc( sizeof(list_contacts) );
  newList->head=NULL;
  newList->key=key;
  return newList;
}

list_contacts * create_list_with_default_key() {
  key_of_structure  key =  SURNAME;
  return create_list(key);
}

bool add(list_contacts *list, char * name, char * surname, char * email, char * phone,char * birthdate, char * address ){

  if(list == NULL) {
    printf("List does not exist" );
    return false;
  }
  node_list_contact * newNode = (node_list_contact*) malloc(sizeof(node_list_contact));
  newNode->data=(char**)malloc(sizeof(char*) * NODE_ATTRIBUTES);
  newNode->data[NAME]=name;
  newNode->data[SURNAME]=surname;
  newNode->data[EMAIL]=email;
  newNode->data[PHONE]=phone;
  newNode->data[ADDRESS]=address;
  newNode->data[BIRTHDATE]=birthdate;

  newNode->next=newNode->prev=NULL;
  if(list->head == NULL){
    list->head=newNode;
    return true;
  }

  newNode->next=list->head;
  list->head->prev=newNode;
  list->head=newNode;
  return true;
}

bool are_equal(char * arg1, char * arg2){
  return strcmp(arg1, arg2 ) == 0;
}

int comparator (char * arg1, char * arg2){
  return strcmp(arg1, arg2);
}


node_list_contact * search_element(list_contacts *list, char * contactKey) {
  if(list == NULL ) {
    printf("List does not exist.\n" );
    return NULL;
  }
  if (list->head == NULL){
    printf("List is empty, contact not found.\n");
    return NULL;
  }

  // printf("%s\n",contactKey );
  node_list_contact * node = list->head;

  while (node != NULL && !are_equal(node->data[list->key],contactKey)) {
    // printf("%s %s\n", node->data[list->key],contactKey );
    node=node->next;
  }

  return node;
}

node_list_contact * find(list_contacts * list, key_of_structure key, char * arg ){
  if(list == NULL ) {
    printf("List does not exist.\n" );
    return NULL;
  }
  if (list->head == NULL){
    printf("List is empty, contact not found.\n");
    return NULL;
  }
  node_list_contact * node = list->head;
  while (node != NULL) {
    if (are_equal(node->data[key],arg)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}


void delete_element(list_contacts *list, char * arg) {
  if(list == NULL ) {
    printf("List does not exist.\n" );
    return;
  }
  if (list->head == NULL) {
    printf("List is empty, cannot delete contact.\n");
    return;
  }

  node_list_contact * nodeToDelete = search_element(list, arg);
  if(nodeToDelete == NULL){
    printf("There is no such contact, \"%s\" cannot be deleted.  \n", arg);
    return;
  }

    printf("%s %s %s %s %s %s was removed \n", nodeToDelete->data[NAME],nodeToDelete->data[SURNAME],nodeToDelete->data[EMAIL],nodeToDelete->data[PHONE],nodeToDelete->data[BIRTHDATE],nodeToDelete->data[ADDRESS]);
  free(nodeToDelete->data);

  if (nodeToDelete->next == NULL && nodeToDelete->prev==NULL) {
    free(nodeToDelete);
    list->head=NULL;
    return;
  }

  if(nodeToDelete->next != NULL && nodeToDelete->prev != NULL) {
    nodeToDelete->next->prev=nodeToDelete->prev;
    nodeToDelete->prev->next=nodeToDelete->next;
    free(nodeToDelete);
    return;
  }

  if(nodeToDelete->next == NULL){
    nodeToDelete->prev->next = NULL;
    free(nodeToDelete);
  }
  else {
    list->head=nodeToDelete->next;
    nodeToDelete->next->prev = NULL;
    free(nodeToDelete);
  }
}

void insert_in_order(list_contacts ** sortedList, node_list_contact * nodeToInsert) {
  // print_list(*sortedList);
  if ((*sortedList)->head == NULL) {
    (*sortedList)->head = nodeToInsert;
    nodeToInsert->prev = nodeToInsert->next = NULL;
    return;
  }

  if (comparator(nodeToInsert->data[(*sortedList)->key], (*sortedList)->head->data[(*sortedList)->key]) < 0) {
    // printf("%s\n",nodeToInsert->data[(*sortedList)->key] );
    nodeToInsert->next = (*sortedList)->head;
    nodeToInsert->prev = NULL;
    (*sortedList)->head->prev=nodeToInsert;
    (*sortedList)->head=nodeToInsert;
    return;
  }

  node_list_contact * tmpNode = (*sortedList)->head;
  while (tmpNode->next != NULL && comparator( nodeToInsert->data[(*sortedList)->key],tmpNode->data[(*sortedList)->key]) >= 0) {
    // printf("%d %s %s\n", comparator( nodeToInsert->data[(*sortedList)->key],tmpNode->data[(*sortedList)->key]) ,nodeToInsert->data[(*sortedList)->key], tmpNode->data[(*sortedList)->key] );
    tmpNode=tmpNode->next;
  }

  //when tmpNode is tail
  if (tmpNode->next == NULL) {
      if (comparator( nodeToInsert->data[(*sortedList)->key],tmpNode->data[(*sortedList)->key]) > 0){
        nodeToInsert->prev=tmpNode;
        nodeToInsert->next=NULL;
        tmpNode->next=nodeToInsert;
        return;
    } else {
      nodeToInsert->next=tmpNode;
      tmpNode->prev->next=nodeToInsert;
      nodeToInsert->prev=tmpNode->prev;
      tmpNode->prev=nodeToInsert;
      return;
    }
  }
  //insert into middle of list
  tmpNode->prev->next=nodeToInsert;
  nodeToInsert->next=tmpNode;
  nodeToInsert->prev=tmpNode->prev;
  tmpNode->prev=nodeToInsert;

}


//** is reference to pointer
void sort_list(list_contacts ** list, key_of_structure key) {
  if(*(list) == NULL ) {
    printf("List does not exist.\n" );
    return;
  }
  if ((*list)->head == NULL) {
    printf("List is empty, cannot sort contacts.\n");
    return;
  }

  list_contacts * sortedList = create_list(key);
  node_list_contact * iterator = (*list)->head;
  node_list_contact * tmpNode;

  while (iterator->next!=NULL) {
    iterator->next->prev = NULL;
    tmpNode=iterator;
    iterator=iterator->next;
    tmpNode->next=NULL;
    insert_in_order(&sortedList,tmpNode);
  }
  insert_in_order(&sortedList,iterator);
  free(*list);
  (*list)=sortedList;


}

void print_list(list_contacts *list) {
  if(list->head == NULL) {
    printf("List is empty\n" );
    return;
  }
  node_list_contact *node = list->head;
  // node_list_contact * tmpNode= list->head;
  // printf("Kluczem jest %d\n", list->key );
  while (node != NULL) {
    printf("%s %s %s %s %s %s \n", node->data[NAME],node->data[SURNAME],node->data[EMAIL],node->data[PHONE],node->data[BIRTHDATE],node->data[ADDRESS]);
    // tmpNode = node;
    node=node->next;
  }

  //
  // printf("Powrót w drugą stronę\n" );
  // while (tmpNode != NULL) {
  //   printf("%s %s %s %s %s %s \n", tmpNode->data[NAME],tmpNode->data[SURNAME],tmpNode->data[EMAIL],tmpNode->data[PHONE],tmpNode->data[ADDRESS],tmpNode->data[BIRTHDATE]);
  //   tmpNode=tmpNode->prev;
  // }
  // // // printf("%s\n",list->head->data[list->key] );
  // printf("\n" );
}
void delete_list(list_contacts ** list) {
  if (*(list) == NULL) {
    printf("Cannot remove list. List does not exist\n" );
    return;
  }
  if ((*list)->head == NULL) {
    free(*(list));
    printf("List was removed\n" );
    return;
  }
  node_list_contact * iterator = (*list)->head;
  node_list_contact * tmp;
  while (iterator != NULL) {
    tmp=iterator;
    iterator=iterator->next;
    free(tmp->data);
    free(tmp);
  }
  free(*list);
}
