//
//  Global.h
//  MyReadingXMP
//
//  Created by Bruce Yinhe on 26/03/2013.
//  Copyright (c) 2013 Bruce Yinhe. All rights reserved.
//

#ifndef MyReadingXMP_Global_h
#define MyReadingXMP_Global_h

#include <string>

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string

// Must be defined to give access to XMPFiles
#define XMP_INCLUDE_XMPFILES 1

// Ensure XMP templates are instantiated
#include "public/include/XMP.incl_cpp"

// Provide access to the API
#include "public/include/XMP.hpp"

#define kXMP_NS_CC        "http://creativecommons.org/ns#"


#endif
