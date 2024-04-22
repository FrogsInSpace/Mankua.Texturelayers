#ifndef _DFDATA_
#define _DFDATA_

// Kaldera - Release

// DCPProt >>>>>>>>>>>>

//This is ID for this particular plugin
//DCPFLICS will identify your product through this ID
static DWORD DCPProt_PID = 205477;
#define ANTICVAR 25481
static int anticvar=ANTICVAR;		// Anti-crack variable to mislead hackers

//Encryption constants
static DWORD DCPProt_xor_var=972481648;
const DWORD DCPProt_file_xor_var=1400377208;

//Your product name
#define DCPProt_PRODUCTNAME _T("Texture Layers 2")

//Dll name, including file extension
#define DCPProt_NAME _T("TexLay.dlm")

// DCPProt <<<<<<<<<<<<


#endif