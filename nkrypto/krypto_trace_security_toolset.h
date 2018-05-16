#ifndef KRYPTO_TRACE_SECURITY_TOOLSET_H
#define KRYPTO_TRACE_SECURITY_TOOLSET_H

extern "C" {

/********************************************************************
***
* \brief 	A function to generate an unique id based on the given serial number
* \param sn : 	Pointer to the given serial number
* \param uid : 	Pointer to the output unique id
* \return 	0: OK; -1: Failed
********************************************************************/
int generateUid(const char *sn, char *uid);

/********************************************************************
* \brief 			A function to sign the information
* \param sn : 			Pointer to the given serial number
* \param info : 		Pointer to the given information
* \param signature_base64: 	Pointer to the output signature in base64 format
* \return 			0: OK; -1: Failed
********************************************************************/
int ecdsaSign(const char *sn, const char *info, char *signature_base64);

/********************************************************************
***
* \brief 			A function to verify a recoverable signature
* \param signature_base64 : 	Pointer to the given signature in base64 format
* \param info : 		Pointer to the given information
* \param uid : 			Pointer to the given unique id
* \return 			0: OK; -1: Failed
********************************************************************/
int verifySignature(const char * signature_base64, const char * info, const char *uid);



/********************************************************************
**
* \brief 		A function to get the information of ndcode_reader_security_toolset.dll
* \param message: 	Pointer to a message memory to store version information
* \param message_len: 	Length of the message memory
* \return 		0: OK; -1: Failed
**
********************************************************************/
//int get_version(char* message, int message_len);

}
#endif // KRYPTO_TRACE_SECURITY_TOOLSET_H
