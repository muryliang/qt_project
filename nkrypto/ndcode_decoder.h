#ifndef NDCODE_DECODER_H
#define NDCODE_DECODER_H

extern "C" {

/********************************************************************
* Error Code Definition:
*
* 0:	DECODE_SUCCESSFULLY
* -1	INCORRECT_VERSION_ID		Incorrect version_id provided. Currently only support 2 version_id: 0 and 1
* -2	INCORRECT_LAYER_NUMBER		Incorrect layers_number provided. Currently only support 5 layers in version 0, and only support 5, 6 and 7 layers in version 1.
* -3	INCORRECT_MASK_ID		Incorrect mask_id provided. This parameter is only needed in the encoder.
* -4	MESSAGE_FORMAT_DISMATCH		Incorrect massage format provided. This parameter is only needed in the encoder.
* -5	HL2M_TYPE_DISMATCH		Incorrect type and value of bits array provided. It should be 0 or 1 in type integer.
* -6	HL2M_SIZE_DISMATCH		Incorrect size of bits array provided. It depends on the layer of the code.
* -7	FLIP_BITS_DISMATCH		Incorrect value of flip bits provided.
* -8	MESSAGE_SIZE_DISMATCH		The size of the message memory is not large enough to contain the output.
* -9	REEDSOLO_DECODE_ERROR		Failed to be decoded using reed solomon.
* -10	FPE_ENCRYPT_ERROR		Failed to be encrypt using format preserving encryption.
* -11	FPE_DECRYPT_ERROR		Failed to be decrypt using format preserving decryption.
* -12	HMAC_CHECKING_ERROR		Failed to pass the hash mac checking.
*
*********************************************************************/

/********************************************************************
* \brief 				Set the decoding parametres of this NDcode Decoder
* \param version_id : 			The version of NDcode pattern used to decode the bits_array
*					There are two version available (0 & 1).
*
* \param layers_number: 		Number of layers.
*					There is only one layer number (5) available in version 0.
*					There are three layer numbers (5, 6, 7) available in version 1.
*
* \param reserved: 			Used to be the decrypted_flag. Now it is fixed internally, do not use anymore.
*
* \param spiral_to_layered_flag: 	A boolean value (0/1) to determine whether need to convert the input bits array into layered pattern or not.
*
* \return 				0: OK; Otherwise: ERROR CODE
********************************************************************/
int set_decoder_paras(int version_id, int layers_number, int reserved, int spiral_to_layered_flag);

/********************************************************************
* \brief 			A decoding function to decode a bits_array into a serial number in hexadecimal string
* \param bits_array : 		Pointer to the input array of the binary value
* \param bits_array_size : 	Size of to the input array of the binary value
* \param sn : 			Pointer to the output serial number in hexidecimal string format
* \param sn_len : 		Length of the output serial number in hexidecimal string format
* \param mode_id : 		The output security mode ID. It is only needed in version 1 decoding.
* \return :			0: OK; Otherwise: ERROR CODE
********************************************************************/
int decoder(int *bits_array, size_t bits_array_size, char *sn, int sn_len, uint8_t *mode_id);

/********************************************************************
* \brief: 		A function to get the version of ndcode_decoder.dll
* \param message: 	Pointer to a message memory to store version information
* \param message_len:	Length of the message memory
* \return: 		0: OK; Otherwise:ERROR CODE
********************************************************************/
//int get_version(char*message, int message_len);
}

#endif // NDCODE_DECODER_H
