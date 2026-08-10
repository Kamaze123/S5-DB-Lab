#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include<bits/stdc++.h>
#include<sstream>

// int main(int argc, char *argv[]) {
//   /* Initialize the Run Copy of Disk */
//   Disk disk_run;
//   // StaticBuffer buffer;
//   // OpenRelTable cache;
  
//   //return FrontendInterface::handleFrontend(argc, argv);
// }

int main(int argc, char *argv[]) {
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;

  
  for(int i = 0; i < 2; i++){
    //get the relation catalog entry using RelCacheTable::getRelCatEntry()
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(i, &relCatEntry);
    printf("Relation: %s\n", relCatEntry.relName);

    for(int j = 0; j < relCatEntry.numAttrs; j++){
      AttrCatEntry attrCatEntry;
      AttrCacheTable::getAttrCatEntry(i, j, &attrCatEntry);

      printf("  %s: %s\n", attrCatEntry.attrName, attrCatEntry.attrType == NUMBER ? "NUM" : "STR");
    }
  }

  return 0;
}