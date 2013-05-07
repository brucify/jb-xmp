//
//  ModifyingXMP.h
//  MyReadingXMP
//
//  Created by Bruce Yinhe on 26/03/2013.
//  Copyright (c) 2013 Bruce Yinhe. All rights reserved.
//

#ifndef __MyReadingXMP__ModifyingXMP__
#define __MyReadingXMP__ModifyingXMP__

#include <iostream>
using namespace std;

#endif /* defined(__MyReadingXMP__ModifyingXMP__) */

void displayPropertyValues(SXMPMeta*);
SXMPMeta createXMPFromRDF();
bool readXMPFile(string*);
void writeRDFToFile(string*, string);


