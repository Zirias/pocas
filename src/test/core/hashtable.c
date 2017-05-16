#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/hashtable.h>
#include <pocas/test/test.h>

PT_TESTCLASS(pocascore::HashTable);

static PC_HashTable *stringTable1;

PT_TESTINIT()
{
    stringTable1 = PC_HashTable_createStrKeyVal(PC_HTS_16);
}

PT_TESTDONE()
{
    PC_HashTable_destroy(stringTable1);
}

PT_TESTMETHOD(set_64_items)
{
    char key[6], val[6];

    for (unsigned i = 64; i; --i)
    {
        sprintf(key, "key%u", i);
        sprintf(val, "val%u", i);
        PC_HashTable_set(stringTable1, key, val);
    }

    PT_Test_assertIntEqual(64, PC_HashTable_count(stringTable1), "wrong item count");
    PT_Test_assertStrEqual("val1", PC_HashTable_get(stringTable1, "key1"), "wrong hashtable entry");
    PT_Test_assertStrEqual("val7", PC_HashTable_get(stringTable1, "key7"), "wrong hashtable entry");
    PT_Test_assertStrEqual("val23", PC_HashTable_get(stringTable1, "key23"), "wrong hashtable entry");
    PT_Test_assertRefEqual(0, PC_HashTable_get(stringTable1, "key67"), "found invalid entry");

    PT_Test_pass();
}

PT_TESTMETHOD(set_64_items_and_remove_two)
{
    char key[6], val[6];

    for (unsigned i = 64; i; --i)
    {
        sprintf(key, "key%u", i);
        sprintf(val, "val%u", i);
        PC_HashTable_set(stringTable1, key, val);
    }

    PC_HashTable_remove(stringTable1, "key23");
    PC_HashTable_remove(stringTable1, "key42");

    PT_Test_assertIntEqual(62, PC_HashTable_count(stringTable1), "wrong item count");
    PT_Test_assertStrEqual("val1", PC_HashTable_get(stringTable1, "key1"), "wrong hashtable entry");
    PT_Test_assertStrEqual("val7", PC_HashTable_get(stringTable1, "key7"), "wrong hashtable entry");
    PT_Test_assertStrEqual("val51", PC_HashTable_get(stringTable1, "key51"), "wrong hashtable entry");
    PT_Test_assertRefEqual(0, PC_HashTable_get(stringTable1, "key23"), "found invalid entry");
    PT_Test_assertRefEqual(0, PC_HashTable_get(stringTable1, "key42"), "found invalid entry");

    PT_Test_pass();
}
