#include "OpenRelTable.h"
#include <cstring>
#include <cstdlib> 

OpenRelTable::OpenRelTable() {

  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
  }

  /************ Setting up Relation Cache entries ************/

  RecBuffer relCatBlock(RELCAT_BLOCK);

  // --- RELCAT's own entry ---
  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

  // --- ATTRCAT's entry ---
  Attribute attrCatRelRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(attrCatRelRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);

  struct RelCacheEntry attrCatRelCacheEntry;
  RelCacheTable::recordToRelCatEntry(attrCatRelRecord, &attrCatRelCacheEntry.relCatEntry);
  attrCatRelCacheEntry.recId.block = RELCAT_BLOCK;
  attrCatRelCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = attrCatRelCacheEntry;

  /************ Setting up Attribute cache entries ************/

  RecBuffer attrCatBlock(ATTRCAT_BLOCK);   // <-- THIS must come before it's used below
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

  // --- RELCAT's attributes (slots 0-5) ---
  AttrCacheEntry* head = nullptr;
  AttrCacheEntry* tail = nullptr;

  for (int i = 0; i < RELCAT_NO_ATTRS; i++) {
      attrCatBlock.getRecord(attrCatRecord, i);

      AttrCacheEntry* attrCacheEntry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
      AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry->attrCatEntry);
      attrCacheEntry->recId.block = ATTRCAT_BLOCK;
      attrCacheEntry->recId.slot = i;
      attrCacheEntry->next = nullptr;

      if (head == nullptr) {
          head = attrCacheEntry;
          tail = attrCacheEntry;
      } else {
          tail->next = attrCacheEntry;
          tail = attrCacheEntry;
      }
  }
  AttrCacheTable::attrCache[RELCAT_RELID] = head;

  // --- ATTRCAT's attributes (slots 6-11) ---
  AttrCacheEntry* head2 = nullptr;
  AttrCacheEntry* tail2 = nullptr;

  for (int i = RELCAT_NO_ATTRS; i < RELCAT_NO_ATTRS + ATTRCAT_NO_ATTRS; i++) {
      attrCatBlock.getRecord(attrCatRecord, i);

      AttrCacheEntry* attrCacheEntry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
      AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry->attrCatEntry);
      attrCacheEntry->recId.block = ATTRCAT_BLOCK;
      attrCacheEntry->recId.slot = i;
      attrCacheEntry->next = nullptr;

      if (head2 == nullptr) {
          head2 = attrCacheEntry;
          tail2 = attrCacheEntry;
      } else {
          tail2->next = attrCacheEntry;
          tail2 = attrCacheEntry;
      }
  }
  AttrCacheTable::attrCache[ATTRCAT_RELID] = head2;
}

OpenRelTable::~OpenRelTable() {
  for (int relId = 0; relId < 2; relId++) {  // RELCAT_RELID and ATTRCAT_RELID
    // free the RelCacheEntry allocated for this relId
    free(RelCacheTable::relCache[relId]);

    // free the linked list of AttrCacheEntry nodes for this relId
    AttrCacheEntry* entry = AttrCacheTable::attrCache[relId];
    while (entry != nullptr) {
      AttrCacheEntry* temp = entry;
      entry = entry->next;
      free(temp);
    }
  }
}