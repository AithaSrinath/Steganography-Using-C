#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Open files required for decoding */
Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open stego image\n");
        return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");

    if (decInfo->fptr_output == NULL)
    {
        printf("ERROR: Unable to create output file\n");
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
        return e_failure;
    }

    return e_success;
}



/* Decode one byte from 8 image bytes */
Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    int i;

    *data = 0;

    for (i = 0; i < 8; i++)
    {
        *data = (*data << 1) | (image_buffer[i] & 1);
    }

    return e_success;
}



/* Decode data from image */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image)
{
    char image_buffer[8];
    int i;

    for (i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, fptr_stego_image) != 8)
        {
            return e_failure;
        }

        if (decode_byte_from_lsb(image_buffer, &data[i]) == e_failure)
        {
            return e_failure;
        }
    }

    return e_success;
}



/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3] = {0};

    if (decode_data_from_image(magic_string, 2, decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    if (magic_string[0] == MAGIC_STRING[0] && magic_string[1] == MAGIC_STRING[1])
    {
        return e_success;
    }

    printf("ERROR: Magic string does not match\n");
    return e_failure;
}



/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    if (decode_data_from_image(decInfo->extn_secret_file, 4, decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    decInfo->extn_secret_file[4] = '\0';

    return e_success;
}



/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char size_data[4];
    int i;
    long file_size = 0;

    if (decode_data_from_image(size_data, 4, decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    for (i = 0; i < 4; i++)
    {
        file_size |= ((unsigned char)size_data[i]) << (i * 8);
    }

    decInfo->size_secret_file = file_size;

    return e_success;
}



/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    long i;

    for (i = 0; i < decInfo->size_secret_file; i++)
    {
        if (decode_data_from_image(&ch, 1, decInfo->fptr_stego_image) == e_failure)
        {
            return e_failure;
        }

        if (fwrite(&ch, 1, 1, decInfo->fptr_output) != 1)
        {
            return e_failure;
        }
    }

    return e_success;
}



/* Perform decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    /* Initialize file pointers */ 
    decInfo->fptr_stego_image = NULL;
    decInfo->fptr_output = NULL;

    if (open_decode_files(decInfo) == e_failure)
    {
        return e_failure;
    }

    /* Skip the 54-byte BMP header */
    if(fseek(decInfo->fptr_stego_image, 54, SEEK_SET) != 0)
    { 
        goto cleanup;
    }
    if(decode_magic_string(decInfo) == e_failure) 
    { 
        goto cleanup;
    }
    if(decode_secret_file_extn(decInfo) == e_failure)
    { 
        goto cleanup;
    }
    if(decode_secret_file_size(decInfo) == e_failure)
    { 
        goto cleanup;
    }
    if(decode_secret_file_data(decInfo) == e_failure)
    { 
        goto cleanup; 
    }

    printf("SUCCESS: Decoding completed successfully\n");

    /* Close files after successful decoding */
    fclose(decInfo->fptr_stego_image);
    decInfo->fptr_stego_image = NULL; 

    fclose(decInfo->fptr_output);
    decInfo->fptr_output = NULL;

    return e_success;

    cleanup:
    /* Close files if decoding fails */ 
    if(decInfo->fptr_stego_image != NULL)
    {
        fclose(decInfo->fptr_stego_image); 
        decInfo->fptr_stego_image = NULL;
    }
    if(decInfo->fptr_output != NULL)
    {
        fclose(decInfo->fptr_output); 
        decInfo->fptr_output = NULL;
    }
    return e_failure;
}








 
