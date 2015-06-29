/*
 * circuit.c
 */

#include <string.h>

#include "circuit.h"
#include "error.h"

int circuit_init (cbHeader* header, char* path)
{
  while (!(cbComponent** table = malloc(TABLE_CAP_BASE*sizeof(cbComponent*))) {
    crash(CB_ERR_MALLOC, FATAL);
  }
  if (path[0]) {  // Null as first character = empty string
    while (cb_LoadFile(path, &header, table)) crash(CB_ERR_RFILE, FATAL);
  } else {
    header->table_cap = TABLE_CAP_BASE;
    header->comp_count = 0;
    header->table = table;
    // Initialise all the pointers in the table to null
    for (int i = 0; i < header->table_cap; i++) {
      header->table[i] = NULL;
    }
  }
  return 0;
}

cbComponent* cb_CreateComponent (int type)
{
  int len = strlen(SYMBOL_DIR) + 8;
  char path[len+1];
  snprintf(path, len, "%s%02x%s", SYMBOL_DIR, type, ".cbip");

  cbComponent* ret = malloc(sizeof(cbComponent));
  *ret = {
    type,
    NULL,
    NULL,
    crCreateItem(filename)
  };
  return ret;
}

void cb_AddComponent (cbComponent* comp, cbHeader* header)
{
  // Stick the component in the first empty slot in the table
  for (int i = 0; i < header->table_cap; i++) {
    if (!header->table[i]) {
      header->table[i] = comp;
      header->comp_count++;
      return;
    }
  }
  // Will never run if an empty slot is found
  // Expand the table, insert the component and initialise the rest of the new
  // slots to null.
  header->table = (cbComponent**) realloc(header->table,
      (header->table_cap+TABLE_EXP_SIZE)*sizeof(cbComponent*));
  header->table[header->table_cap+1] = comp;
  header->comp_count++
  for (i = 2; i <= TABLE_EXP_SIZE; i++) {
    header->table[header->table_cap+i] = NULL;
  }
  header->table_cap += TABLE_EXP_SIZE;
}

void cb_DeleteComponent (cbComponent* comp, cbHeader* header)
{
  if ((int i = cb_FindComponent(comp, header)) != -1) {
    header->table[i] = NULL;
    free(comp);
    header->comp_count--;
  } else {
    crash(CB_ERR_DNE, NOT_FATAL)
  }
}

cb_Close (cbHeader* header)
{
  for (int i = 0; i < header->table_cap; i++) {
    free(header->table[i]);
  }
  free(header->table);
  free(header);
}

// Given a pointer, finds the index of that pointer in the array.
// Returns -1 if not found
int cb_FindComponent (cbComponent* comp, const cbHeader* header)
{
  for (int i = 0; i < header->table_cap; i++;) {
    if (header->table[i] == comp) return i;
  }
  return -1;
}

void cb_ConnectComponents (cbComponent* from, cbComponent* to)
{
  from->to = to;
  to->from = from;
}

cbComponent* cb_GetComponentByDrawItem (crItem* ditem, cbHeader* header)
{
  for (int i = 0; i < header->table_cap; i++;) {
    if (header->table[i]->draw_item == ditem) return header->table[i];
  }
  return NULL;
}
