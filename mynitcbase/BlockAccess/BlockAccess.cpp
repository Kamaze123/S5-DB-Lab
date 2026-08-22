#include "BlockAccess.h"

#include <cstdlib>
#include <cstring>

RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    // get the previous search index of the relation relId from the relation cache
    // (use RelCacheTable::getSearchIndex() function)
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId, &prevRecId);

    
    // let block and slot denote the record id of the record being currently checked
    int block, slot;

    // if the current search index record is invalid(i.e. both block and slot = -1)
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        RelCatEntry relCatBuf;
        RelCacheTable::getRelCatEntry(relId, &relCatBuf);

        block = relCatBuf.firstBlk;
        slot = 0;
    }
    else
    {
        // (there is a hit from previous search; search should start from
        // the record next to the search index record)

        // block = search index's block
        // slot = search index's slot + 1
        block = prevRecId.block;
        slot = prevRecId.slot + 1;
    }

    while (block != -1)
    {
        RecBuffer recBuffer(block);
        // get the record with id (block, slot) using RecBuffer::getRecord()
        RelCatEntry relCatEntry;
        RelCacheTable::getRelCatEntry(relId, &relCatEntry);

        Attribute record[relCatEntry.numAttrs];
        recBuffer.getRecord(record, slot);
        // get header of the block using RecBuffer::getHeader() function
        HeadInfo head;
        recBuffer.getHeader(&head);
        // get slot map of the block using RecBuffer::getSlotMap() function
        unsigned char slotMap[head.numSlots];
        recBuffer.getSlotMap(slotMap);
        
        if(slot >= head.numSlots)
        {
            block = head.rblock;
            slot = 0;
            continue;  
        }

        if(slotMap[slot] == SLOT_UNOCCUPIED)
        {
            slot++;
            continue;
        }

        // compare record's attribute value to the the given attrVal as below:
        /*
            firstly get the attribute offset for the attrName attribute
            from the attribute cache entry of the relation using
            AttrCacheTable::getAttrCatEntry()
        */
        /* use the attribute offset to get the value of the attribute from
           current record */
        AttrCatEntry attrCatEntry;
        AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatEntry);

        int attrOffset = attrCatEntry.offset;
        Attribute attrVal2 = record[attrOffset];

        int cmpVal = compareAttrs(attrVal2, attrVal, attrCatEntry.attrType);

        /* Next task is to check whether this record satisfies the given condition.
           It is determined based on the output of previous comparison and
           the op value received.
           The following code sets the cond variable if the condition is satisfied.
        */
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            /*
            set the search index in the relation cache as
            the record id of the record that satisfies the given condition
            (use RelCacheTable::setSearchIndex function)
            */
            RecId temp = {block, slot};
            RelCacheTable::setSearchIndex(relId, &temp);
            return RecId{block, slot};
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1, -1};
}
