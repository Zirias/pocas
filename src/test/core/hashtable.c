#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/hashtable.h>
#include <pocas/test/test.h>

TESTCLASS(pocascore::HashTable);

static HashTable *stringTable1;

TESTINIT()
{
    stringTable1 = HashTable_createStrKeyVal(HashTableSize_16);
}

TESTDONE()
{
    HashTable_destroy(stringTable1);
}

TESTMETHOD(set_64_items)
{
    char key[6], val[6];

    for (unsigned i = 64; i; --i)
    {
        sprintf(key, "key%u", i);
        sprintf(val, "val%u", i);
        HashTable_set(stringTable1, key, val);
    }

    Test_assertIntEqual(64, HashTable_count(stringTable1), "wrong item count");
    Test_assertStrEqual("val1", HashTable_get(stringTable1, "key1"), "wrong hashtable entry");
    Test_assertStrEqual("val7", HashTable_get(stringTable1, "key7"), "wrong hashtable entry");
    Test_assertStrEqual("val23", HashTable_get(stringTable1, "key23"), "wrong hashtable entry");
    Test_assertRefEqual(0, HashTable_get(stringTable1, "key67"), "found invalid entry");

    Test_pass();
}
