#ifndef POCAS_CORE_HASHTABLE_H
#define POCAS_CORE_HASHTABLE_H

#include <stddef.h>

#include <pocas/core/decl.h>

typedef struct KeyValPair KeyValPair;
typedef struct HashKey HashKey;
typedef struct HashTable HashTable;
typedef enum HashTableSize
{
    HashTableSize_1024,
    HashTableSize_512,
    HashTableSize_256,
    HashTableSize_128,
    HashTableSize_64,
    HashTableSize_32,
    HashTableSize_16,
} HashTableSize;

typedef void (*HashKeyProvider)(HashKey *key, const void *keyObject);
typedef void *(*HashTableEntryCloner)(const void *entry);
typedef void (*HashTableEntryDeleter)(void *entry);

DECLEXPORT const void *KeyValPair_key(const KeyValPair* self);
DECLEXPORT const void *KeyValPair_val(const KeyValPair* self);

DECLEXPORT const void *HashKey_data(const HashKey *self);
DECLEXPORT size_t HashKey_size(const HashKey *self);
DECLEXPORT void HashKey_set(HashKey *self, size_t size, const void *data);

DECLEXPORT HashTable *HashTable_create(HashTableSize size,
        HashKeyProvider keyProvider, HashTableEntryCloner cloner,
        HashTableEntryDeleter deleter);
DECLEXPORT HashTable *HashTable_createStrKey(HashTableSize size,
        HashTableEntryCloner cloner, HashTableEntryDeleter deleter);
DECLEXPORT HashTable *HashTable_createStrKeyVal(HashTableSize size);

DECLEXPORT size_t HashTable_count(const HashTable *self);
DECLEXPORT void HashTable_set(HashTable *self, const void *key, void *val);
DECLEXPORT const void *HashTable_get(const HashTable *self, const void *key);
DECLEXPORT int HashTable_remove(HashTable *self, const void *key);

DECLEXPORT void HashTable_destroy(HashTable *self);

#endif // HASHTABLE_H
