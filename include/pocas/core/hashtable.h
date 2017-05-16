#ifndef POCAS_CORE_HASHTABLE_H
#define POCAS_CORE_HASHTABLE_H

#include <stddef.h>

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_KeyValPair);
C_CLASS_DECL(PC_HashKey);

C_CLASS_DECL(PC_HashTable);

enum PC_HashTableSize
{
    PC_HTS_1024,
    PC_HTS_512,
    PC_HTS_256,
    PC_HTS_128,
    PC_HTS_64,
    PC_HTS_32,
    PC_HTS_16,
};
C_ENUM_DECL(PC_HashTableSize);

typedef void (*PC_HashKeyProvider)(PC_HashKey *key, const void *keyObject);
typedef void *(*PC_HashTableEntryCloner)(const void *entry);
typedef void (*PC_HashTableEntryDeleter)(void *entry);

DECLEXPORT const void *PC_KeyValPair_key(const PC_KeyValPair* self);
DECLEXPORT const void *PC_KeyValPair_val(const PC_KeyValPair* self);

DECLEXPORT const void *PC_HashKey_data(const PC_HashKey *self);
DECLEXPORT size_t PC_HashKey_size(const PC_HashKey *self);
DECLEXPORT void PC_HashKey_set(PC_HashKey *self, size_t size, const void *data);

DECLEXPORT PC_HashTable *PC_HashTable_create(PC_HashTableSize size,
        PC_HashKeyProvider keyProvider, PC_HashTableEntryCloner cloner,
        PC_HashTableEntryDeleter deleter);
DECLEXPORT PC_HashTable *PC_HashTable_createStrKey(PC_HashTableSize size,
        PC_HashTableEntryCloner cloner, PC_HashTableEntryDeleter deleter);
DECLEXPORT PC_HashTable *PC_HashTable_createStrKeyVal(PC_HashTableSize size);

DECLEXPORT size_t PC_HashTable_count(const PC_HashTable *self);
DECLEXPORT void PC_HashTable_set(PC_HashTable *self, const void *key, void *val);
DECLEXPORT const void *PC_HashTable_get(const PC_HashTable *self, const void *key);
DECLEXPORT int PC_HashTable_remove(PC_HashTable *self, const void *key);

DECLEXPORT void PC_HashTable_destroy(PC_HashTable *self);

#endif // HASHTABLE_H
