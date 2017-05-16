#include <stdlib.h>
#include <string.h>

#include <pocas/core/string.h>
#include <pocas/core/hashtable.h>

typedef struct PC_HashTableEntry PC_HashTableEntry;
typedef struct PC_HashTableSizeParams PC_HashTableSizeParams;

struct PC_HashKey
{
    size_t size;
    void *data;
};

struct PC_HashTableEntry
{
    PC_HashKey key;
    const void *keyObject;
    void *valObject;
    PC_HashTableEntry* next;
};

struct PC_HashTableSizeParams
{
    const size_t buckets;
    const size_t hashmask;
};

struct PC_KeyValPair
{
    const void *key;
    const void *val;
};

struct PC_HashTable
{
    const PC_HashTableSizeParams *sizeParams;
    PC_HashKeyProvider keyProvider;
    PC_HashTableEntryCloner entryCloner;
    PC_HashTableEntryDeleter entryDeleter;
    size_t count;
    PC_HashTableEntry* buckets[];
};

static const PC_HashTableSizeParams sizes[] = {
    { 1024, 0x3ff },
    { 512, 0x1ff },
    { 256, 0xff },
    { 128, 0x7f },
    { 64, 0x3f },
    { 32, 0x1f },
    { 16, 0xf }
};

static size_t hash(const PC_HashKey *key, size_t hashmask)
{
    size_t h = 5381;

    for (size_t i = 0; i < key->size; ++i)
    {
        h += (h << 5) + ((const unsigned char *)key->data)[i];
    }

    return h & hashmask;
}

static int keysAreEqual(const PC_HashKey *lhs, const PC_HashKey *rhs)
{
    if (lhs->size != rhs->size) return 0;
    return !memcmp(lhs->data, rhs->data, lhs->size);
}

static void stringKeyProvider(PC_HashKey *key, const void *string)
{
    PC_HashKey_set(key, strlen(string), string);
}

static PC_HashTableEntry *PC_HashTableEntry_create(const PC_HashKey *key,
        const void *keyObject, void *valObject)
{
    PC_HashTableEntry *self = malloc(sizeof(PC_HashTableEntry));
    self->key.size = key->size;
    self->key.data = key->data;
    self->keyObject = keyObject;
    self->valObject = valObject;
    self->next = 0;
    return self;
}

SOEXPORT void PC_HashKey_set(PC_HashKey *self, size_t size, const void *data)
{
    free(self->data);
    self->data = malloc(size);
    memcpy(self->data, data, size);
    self->size = size;
}

SOEXPORT PC_HashTable *PC_HashTable_create(PC_HashTableSize size,
        PC_HashKeyProvider keyProvider, PC_HashTableEntryCloner cloner,
        PC_HashTableEntryDeleter deleter)
{
    const PC_HashTableSizeParams *sizeParams = sizes + size;
    PC_HashTable *self = calloc(1, sizeof(PC_HashTable)
            + sizeParams->buckets * sizeof(PC_HashTableEntry *));
    self->sizeParams = sizeParams;
    self->keyProvider = keyProvider;
    self->entryCloner = cloner;
    self->entryDeleter = deleter;
    self->count = 0;
    return self;
}

SOEXPORT PC_HashTable *PC_HashTable_createStrKey(PC_HashTableSize size,
        PC_HashTableEntryCloner cloner, PC_HashTableEntryDeleter deleter)
{
    return PC_HashTable_create(size, stringKeyProvider, cloner, deleter);
}

SOEXPORT PC_HashTable *PC_HashTable_createStrKeyVal(PC_HashTableSize size)
{
    return PC_HashTable_createStrKey(size,
            (PC_HashTableEntryCloner)PC_String_copy, free);
}

SOEXPORT size_t PC_HashTable_count(const PC_HashTable *self)
{
    return self->count;
}

SOEXPORT void PC_HashTable_set(PC_HashTable *self, const void *key, void *val)
{
    PC_HashKey hkey;
    hkey.data = 0;
    self->keyProvider(&hkey, key);
    size_t bucket = hash(&hkey, self->sizeParams->hashmask);

    PC_HashTableEntry *entry = self->buckets[bucket];
    PC_HashTableEntry *prev = 0;
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
        entry = PC_HashTableEntry_create(&hkey, key,
                self->entryCloner ? self->entryCloner(val) : val);
        if (prev) prev->next = entry;
        else self->buckets[bucket] = entry;
        ++self->count;
    }
}

SOEXPORT const void *PC_HashTable_get(const PC_HashTable *self, const void *key)
{
    PC_HashKey hkey;
    hkey.data = 0;
    self->keyProvider(&hkey, key);
    size_t bucket = hash(&hkey, self->sizeParams->hashmask);
    const void *result = 0;

    PC_HashTableEntry *entry = self->buckets[bucket];
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

SOEXPORT int PC_HashTable_remove(PC_HashTable *self, const void *key)
{
    PC_HashKey hkey;
    hkey.data = 0;
    self->keyProvider(&hkey, key);
    size_t bucket = hash(&hkey, self->sizeParams->hashmask);

    PC_HashTableEntry *entry = self->buckets[bucket];
    PC_HashTableEntry *prev = 0;
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

SOEXPORT void PC_HashTable_destroy(PC_HashTable *self)
{
    if (!self) return;

    for (size_t i = 0; i < self->sizeParams->buckets; ++i)
    {
        PC_HashTableEntry *entry = self->buckets[i];
        while (entry)
        {
            PC_HashTableEntry *next = entry->next;
            if (self->entryDeleter) self->entryDeleter(entry->valObject);
            free(entry->key.data);
            free(entry);
            entry = next;
        }
    }
    free(self);
}
