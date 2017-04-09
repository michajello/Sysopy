#ifndef list_header
#define list_header

typedef enum {NAME,SURNAME,EMAIL,PHONE,BIRTHDATE,ADDRESS,ID} key_of_structure;
#define NODE_ATTRIBUTES 6

typedef struct node_list_contact node_list_contact;
typedef struct list_contacts list_contacts;

struct node_list_contact {
  node_list_contact * prev;
  node_list_contact * next;
  int id;
  char ** data;
};

struct list_contacts {
  node_list_contact * head;
  key_of_structure  key ;

  // bool (*add)(list_contacts *, char * , char * , char * , char * ,char * , char * );
  // node_list_contact * (*search_element)(list_contacts *, char * );
  // void (*delete_element)(list_contacts *, char * );
  // // void (*insert_in_order)(list_contacts ** , node_list_contact * );
  // void (*sort_list)(list_contacts ** , key_of_structure ) ;
  // void (*print_list)(list_contacts * );
  // node_list_contact * (*find)(list_contacts * , key_of_structure , char *  );
  int amount;
};
void insert_in_order(list_contacts ** , node_list_contact * );
int comparator (char * , char * );
bool are_equal(char * , char * );
list_contacts * create_list_with_default_key(void);
list_contacts * create_list(key_of_structure );
bool add (list_contacts *, char * , char * , char * , char * ,char * , char * );
node_list_contact * search_element(list_contacts *, char * );
void delete_element (list_contacts *, char * );
void sort_list(list_contacts ** , key_of_structure ) ;
void print_list(list_contacts * );
void delete_list(list_contacts **);
// node_list_contact * find(list_contacts * , key_of_structure , char *  );

#endif
