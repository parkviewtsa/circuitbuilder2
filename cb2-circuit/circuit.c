/*
 * init.c
 */

#include "circuit.h"

cbHeader* circuit_init (char* path)
{
    cbComponent* table = malloc(TABLE_CAP_BASE*sizeof(cbComponent));
    cbHeader* header = malloc(sizeof(cbHeader));
    if (path[0] != 0) {  // Null as first character = empty string
        loadCircuitFile(path, &header, table);
    } else {
        header->table_cap = TABLE_CAP_BASE;
        header->comp_count = 0;
        header->table = table;
    }
    return header;
}

void add_circuit_component (cbComponent comp, cbComponent* table)
{

}
