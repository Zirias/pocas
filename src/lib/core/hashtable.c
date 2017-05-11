#include <stdlib.h>
#include <string.h>

#include <pocas/core/string.h>
#include <pocas/core/hashtable.h>

typedef struct HashTableEntry HashTableEntry;
typedef struct HashTableSizeParams HashTableSizeParams;

struct HashKey
{
    size_t size;
    void *data;
};

struct HashTableEntry
{
    HashKey key;
    const void *keyObject;
    void *valObject;
    HashTableEntry* next;
};

struct HashTableSizeParams
{
    const size_t buckets;
    const size_t hashmask;
};

struct KeyValPair
{
    const void *key;
    const void *val;
};

struct HashTable
{
    const HashTableSizeParams *sizeParams;
    HashKeyProvider keyProvider;
    HashTableEntryCloner entryCloner;
    HashTableEntryDeleter entryDeleter;
    size_t count;
    HashTableEntry* buckets[];
};

static const HashTableSizeParams sizes[] = {
    { 1024, 0x3ff },
    { 512, 0x1ff },
    { 256, 0xff },
    { 128, 0x7f },
    { 64, 0x3f },
    { 32, 0x1f },
    { 16, 0xf }
};

static size_t hash(const HashKey *key, size_t hashmask)
{
    size_t h = 5381;

    for (size_t i = 0; i < key->size; ++i)
    {
        h += (h << 5) + ((const unsigned char *)key->data)[i];
    }

    return h & hashmask;
}

static int keysAreEqual(const HashKey *lhs, const HashKey *rhs)
{
    if (lhs->size != rhs->size) return 0;
    return !memcmp(lhs->data, rhs->data, lhs->size);
}

static void stringKeyProvider(HashKey *key, const void *string)
{
    HashKey_set(key, strlen(string), string);
}

static HashTableEntry *HashTableEntry_create(const HashKey *key,
        const void *keyObject, void *valObject)
{
    HashTableEntry *self = malloc(sizeof(HashTableEntry));
    self->key.size = key->size;
    self->key.data = key->data;
    self->keyObject = keyObject;
    self->valObject = valObject;
    self->next = 0;
    return self;
}

SOEXPORT void HashKey_set(HashKey *self, size_t size, const void *data)
{
    free(self->data);
    self->data = malloc(size);
    memcpy(self->data, data, size);
    self->size = size;
}

SOEXPORT HashTable *HashTable_create(HashTableSize size,
        HashKeyProvider keyProvider, HashTableEntryCloner cloner,
        HashTableEntryDeleter deleter)
{
    const HashTableSizeParams *sizeParams = sizes + size;
    HashTable *self = calloc(1, sizeof(HashTable)
            + sizeParams->buckets * sizeof(HashTableEntry *));
    self->sizeParams = sizeParams;
    self->keyProvider = keyProvider;
    self->entryCloner = cloner;
    self->entryDeleter = deleter;
    self->count = 0;
    return self;
}

SOEXPORT HashTable *HashTable_createStrKey(HashTableSize size,
        HashTableEntryCloner cloner, HashTableEntryDeleter deleter)
{
    return HashTable_create(size, stringKeyProvider, cloner, deleter);
}

SOEXPORT HashTable *HashTable_createStrKeyVal(HashTableSize size)
{
    return HashTable_createStrKey(size,
            (HashTableEntryCloner)String_copy, free);
}

SOEXPORT size_t HashTable_count(const HashTable *self)
{
    return self->count;
}

SOEXPORT void HashTable_set(HashTable *self, const void *key, void *val)
{
    HashKey hkey;
    hkey.data = 0;
    self->keyProvider(&hkey, key);
    size_t bucket = hash(&hkey, self->sizeParams->hashmask);

    HashTableEntry *entry = self->buckets[bucket];
    HashTableEntry *prev = 0;
    while (entry)
    {
        prev = entry;
        if (keysAreEqual(&hkey, &entry->key)) break;
        entry = entry->next;
    }

    if (entry)
    {
        entry->keyObject = key;
        if (self->entryDeleter) self->entryDeleter(entry->valObject);
        entry->valObject = self->entryCloner ? self->entryCloner(val) : val;
        free(hkey.data);
    }
    else
    {
        entry = HashTableEntry_create(&hkey, key,
                self->entryCloner ? self->entryCloner(val) : val);
        if (prev) prev->next = entry;
        else self->buckets[bucket] = entry;
        ++self->count;
    }
}

SOEXPORT const void *HashTable_get(const HashTable *self, const void *key)
{
    HashKey hkey;
    hkey.data = 0;
    self->keyProvider(&hkey, key);
    size_t bucket = hash(&hkey, self->sizeParams->hashmask);
    const void *result = 0;

    HashTableEntry *entry = self->buckets[bucket];
    while (entry)
    {
        if (keysAreEqual(&hkey, &entry->key))
        {
            result = entry->valObject;
            break;
        }
        entry = entry->next;
    }

    free(hkey.data);
    return result;
}

SOEXPORT int HashTable_remove(HashTable *self, const void *key)
{
    HashKey hkey;
    hkey.data = 0;
    self->keyProvider(&hkey, key);
    size_t bucket = hash(&hkey, self->sizeParams->hashmask);

    HashTableEntry *entry = self->buckets[bucket];
    HashTableEntry *prev = 0;
    while (entry)
    {
        if (keysAreEqual(&hkey, &entry->key))
        {
            if (prev) prev->next = entry->next;
            else self->buckets[bucket] = entry->next;
            if (self->entryDeleter) self->entryDeleter(entry->valObject);
            free(entry->key.data);
            free(entry);
            --self->count;
            free(hkey.data);
            return 1;
        }
        prev = entry;
        entry = prev->next;
    }

    free(hkey.data);
    return 0;
}

SOEXPORT void HashTable_destroy(HashTable *self)
{
    if (!self) return;

    for (size_t i = 0; i < self->sizeParams->buckets; ++i)
    {
        HashTableEntry *entry = self->buckets[i];
        while (entry)
        {
            HashTableEntry *next = entry->next;
            if (self->entryDeleter) self->entryDeleter(entry->valObject);
            free(entry->key.data);
            free(entry);
            entry = next;
        }
    }
    free(self);
}
