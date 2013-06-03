// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

/**
 * This is a walk-through based on the Tutorial solution for the Walkthrough 1 in the XMP Programmers Guide.
 */

#include <iostream>
#include <fstream>
#include <string>

#include "Global.h"
#include "ModifyingXMP.h"

using namespace std;

/**
 *	Initializes the toolkit and attempts to open a file for reading metadata.
 *  Once the file is open several properties are read and displayed in the console.
 *  Then reads an XMP file. The XMP object is then dumped to a text file.
 *  Finally embeds the XMP object into the file and the resource file is closed.
 */
int main ( int argc, const char * argv[] )
{

    // OBS hard-coded input path
    string filename = "/Users/bruce/Desktop/Screen Shot 2013-03-28 at 14.30.28.png" /*string( argv[1] )*/;
    
    // init
	if(!SXMPMeta::Initialize())
	{
		cout << "Could not initialize toolkit!";
		return -1;
	}
    
	XMP_OptionBits options = 0;
    #if UNIX_ENV
    options |= kXMPFiles_ServerMode;
    #endif
	
    // Must initialize SXMPFiles before we use it
	if ( ! SXMPFiles::Initialize ( options ) )
	{
		cout << "Could not initialize SXMPFiles.";
		return -1;
	}
	
	try
	{       
        // OBS See manuel for options. Must be OpenForUpdate to edit
        XMP_OptionBits opts = kXMPFiles_OpenForUpdate;

		bool ok;
		SXMPFiles myFile;
		std::string status = "";
        
		// First we try and open the file
		ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
        
		if( ! ok )
		{
			status += "No smart handler available for " + filename + "\n";
			status += "Trying packet scanning.\n";
            
			// Now try using packet scanning
			opts = kXMPFiles_OpenForUpdate | kXMPFiles_OpenUsePacketScanning;
			ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
		}
        
        
		// If the file is open then read the metadata
		if(ok)
		{
			// OBS register custom CC namespace URI
            string ccRegistered;
            SXMPMeta::RegisterNamespace(kXMP_NS_CC, "cc", &ccRegistered);
            cout << "registeted cc ns: = " << ccRegistered  << endl;
            
            // OBS READ begins HERE
            cout << status << endl;
			cout << filename << " is opened successfully" << endl;
            
			// Create the xmp object and get the xmp data, see GetXMP
			SXMPMeta meta;
			myFile.GetXMP(&meta);

            cout << "Before Appending Properties:" << endl;
            displayPropertyValues(&meta);
            
            // printing for debugging purpose, not important
            string xmpBufferOld;
            meta.SerializeToBuffer( &xmpBufferOld, NULL, NULL, "", "", NULL );
            cout << "meta Old length = " << xmpBufferOld.length() << endl;
                        
            // read RDF from .xmp file
            SXMPMeta meta1;
            meta1 = createXMPFromRDF();

            cout << "Things to append:" << endl;
            displayPropertyValues(&meta1);
            
            // append meta1 obj to meta obj
            SXMPUtils::ApplyTemplate(&meta, meta1, kXMPTemplate_AddNewProperties | kXMPTemplate_ReplaceExistingProperties | kXMPTemplate_IncludeInternalProperties);
            
            cout << "After Appending Properties:" << endl;
            displayPropertyValues(&meta);
            
            cout << "exporting updated XMP obj to .xmp file.." << endl;
            string xmpBuffer;
            meta.SerializeToBuffer( &xmpBuffer, NULL, NULL, "", "", NULL );
            cout << "meta length = " << xmpBuffer.length() << endl;

            writeRDFToFile(&xmpBuffer, "XMP_RDF.xmp");

            // OBS READ ends HERE

            // OBS WRITE begings HERE
			    
            // Writing updated XMP back to file, see CanPutXMP
            if( myFile.CanPutXMP( meta ))
            {
                cout << "Putting back to file..." << endl;
                myFile.PutXMP( meta );
            } else {
                cout << "Cannot put back to file." << endl;
                
                // for debugging purpose, not important
                XMP_FileFormat format;
                myFile.GetFileInfo(NULL, NULL, &format);
                
                XMP_OptionBits formatFlags;
                SXMPFiles::GetFormatInfo ( format, &formatFlags );
                
                cout << "formatFlags = " << formatFlags << endl;
                cout << "formatFlags & kXMPFiles_CanInjectXMP = " << (formatFlags & kXMPFiles_CanInjectXMP) << endl;
                cout << "formatFlags & kXMPFiles_CanExpand = " << (formatFlags & kXMPFiles_CanExpand) << endl;
                cout << "&& = " <<  ( (formatFlags & kXMPFiles_CanInjectXMP) && (formatFlags & kXMPFiles_CanExpand) )  << endl;
                
                XMP_PacketInfo packetInfo;
                bool hasXMP = myFile.GetXMP(0, 0, &packetInfo);
                cout << "packetInfo.length = " << packetInfo.length << endl;
                // not important ends
            }
            
			// Close the SXMPFile.  The resource file is already closed if it was
			// opened as read only but this call must still be made.
			myFile.CloseFile();
		}
		else
		{
			cout << "Unable to open " << filename << endl;
		}
	}
	catch(XMP_Error & e)
	{
		cout << "ERROR: " << e.GetErrMsg() << endl;
	}
    
	// Terminate the toolkit
	SXMPFiles::Terminate();
	SXMPMeta::Terminate();
    
	return 0;
}

