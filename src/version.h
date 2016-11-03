#ifndef VERSION_H
#define VERSION_H

//Change Log:
//"0.0.0.1"
//Project Started
//"0.0.0.2"
//Save And Load Mail Server
//"0.0.0.3"
//Save And Load Email
//"0.0.0.4"
//NSD Payslip sender Option Added
//"0.0.0.5"
//Adding Versioning info
//"0.0.0.6"
//Change Payslip file section separator from '-' to '#' 'exe : 2.1.85
//"0.0.0.7"
//output messages has been reviewed
//"0.0.0.8"
//Create Buttons On Form For User Convenience.
//"0.0.0.9"
//Add Icon To The Buttons
//"0.0.0.10"
//load and save server config file will lokk up in the application path instead of user home folder
//"0.0.0.11"
//changed buttons position
//clears the attachments list when opening an eml file.
//eml version 1.1 : includes payslip check box position status
//"0.0.0.12"
//Accourding to the navaco Error : ESMTP (Afr@net mail server) required Date in the header
//"0.0.1.0"
//reprogrammed the whole sequence of smtp flowchart. Codes moved from ready read to send mail method of Smtp Class.
//"0.0.1.1"
//// 1 - change the name of the attached file if in payslip to "PaySlip.pdf" in othe hand remove whitespaces from file name
////     because some email clients doesn't recognize fiel names with whitespaces
// 2 - make subject editable in payslip
// 3 - set subject to "Payslip" in Payslip mode (was "NSD Payslip")
// 4 - Add Name and version of the app and the company url at the end of messages



#define NSDUNIQUEVER                0,0,1,1
#define NSDUNIQUEVER_STR           "0.0.1.1\0"

#define VER_FILEVERSION             NSDUNIQUEVER
#define VER_FILEVERSION_STR         NSDUNIQUEVER_STR

#define VER_PRODUCTVERSION          NSDUNIQUEVER
#define VER_PRODUCTVERSION_STR      NSDUNIQUEVER_STR

#define VER_COMPANYNAME_STR         "NSD Corp"
#define VER_FILEDESCRIPTION_STR     "NSD Mail Sender"
#define VER_INTERNALNAME_STR        "NSD Mail Sender"
#define VER_LEGALCOPYRIGHT_STR      "Copyright © 2015 NSD Corp"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "NSDMailSender.exe"
#define VER_PRODUCTNAME_STR         "NSD Mail Sender"

#define VER_COMPANYDOMAIN_STR       "NSD-Corp.com"

#endif // VERSION_H
