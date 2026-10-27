#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
*/ 

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

  /* Get File pointers for i/p and o/p files */
   Status open_files(EncodeInfo *encInfo)
   {
     // Src Image file
     encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
     // Do Error handling
     if (encInfo->fptr_src_image == NULL)
       {
    	 perror("fopen");
    	 fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	 return e_failure;
       }
     // Open Secret file
     encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
     // Do Error handling
     if (encInfo->fptr_secret == NULL)
       {
    	  perror("fopen");
    	  fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

          fclose(encInfo->fptr_src_image); 
          encInfo->fptr_src_image = NULL;
    	  return e_failure;
       }
     // Stego Image file
     encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
     // Do Error handling
     if (encInfo->fptr_stego_image == NULL)
       {
    	  perror("fopen");
    	  fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

          fclose(encInfo->fptr_src_image);
          fclose(encInfo->fptr_secret);

          encInfo -> fptr_src_image = NULL;
          encInfo -> fptr_secret = NULL;

    	  return e_failure;
        }
        // No failure return e_success
        return e_success;
   }



    /* Get file size */
    uint get_file_size(FILE *fptr)
    {
      uint size;

      fseek(fptr, 0, SEEK_END);
      size = ftell(fptr);
      rewind(fptr);
      return size;
    }



   /* Check image capaity */
   Status check_capacity(EncodeInfo *encInfo)
   {
      uint image_size;
      uint secret_size;

      image_size = get_image_size_for_bmp(encInfo->fptr_src_image);
      secret_size = get_file_size(encInfo->fptr_secret);

      encInfo->image_capacity = image_size;
      encInfo->size_secret_file = secret_size;

      if(image_size >= secret_size * 8)
      {
         return e_success;
      }
      else
      {
         return e_failure;
      }
   }



    /* Copy BMP image header */
    Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
    {
        char header[54];

        rewind(fptr_src_image);

        if(fread(header, 1, 54, fptr_src_image) != 54)
        {
             return e_failure;
        }
        if(fwrite(header, 1, 54, fptr_dest_image) != 54)
        {
            return e_failure;
        }
        return e_success;
    }



    /* Encode onr=e byte into the LSB of image data */
    Status encode_byte_to_lsb(char data, char *image_buffer)
    {
        //int i;

        for(int i=0; i<8; i++)
        {
            image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
        }
        return e_success;
    }



    /* Encode data into image */
    Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
    {
        char image_buffer[8];
        int i;
        for(int i=0; i<size; i++)
        {
         if (fread(image_buffer, 1, 8, fptr_src_image) != 8)
           {
               return e_failure;
           }

            encode_byte_to_lsb(data[i], image_buffer);

            if(fwrite(image_buffer, 1, 8, fptr_stego_image) != 8)
            {
                return e_failure;
            }
        } 
         return e_success;
    }
        
    

    /* Copy remaining image data */
    Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
    {
        char ch;

        while(fread(&ch, 1, 1, fptr_src) > 0)
        {
            if(fwrite(&ch, 1, 1, fptr_dest) != 1)
            {
                return e_failure;
            }
        }
        return e_success;
    }


    
   /* read and validate */
   Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
   {
     // Check source image extension
     if (strstr(argv[2], ".bmp") == NULL)
     {
        return e_failure;
     }

     // Check secret file extension
     char *extn = strrchr(argv[3], '.');

     if (extn == NULL)
     {
        return e_failure;
     }

     // Store filenames
     encInfo->src_image_fname = argv[2];
     encInfo->secret_fname = argv[3];
     encInfo->stego_image_fname = argv[4];

     // Store secret file extension
     memcpy(encInfo->extn_secret_file, extn, MAX_FILE_SUFFIX);

     return e_success;
   }



    // check_operation_type() 
    OperationType check_operation_type(char *argv[])
    {
      if (argv[1] == NULL)
         return e_unsupported;

      if (strcmp(argv[1], "-e") == 0)
          return e_encode;
      else if (strcmp(argv[1], "-d") == 0)
          return e_decode;
      else
          return e_unsupported;
    }



    /* Encode secret file extension */
    Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
    {
        return encode_data_to_image((char *)file_extn, MAX_FILE_SUFFIX, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    }



    /* Encode secret file size */
    Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
    {
        char size_data[4];
        int i;

        for(int i=0; i<4; i++)
        {
            size_data[i] = (file_size >> (i * 8) & 0xFF);
        }
        return encode_data_to_image(size_data, 4, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    }



    /* Encode secret file data */
     Status encode_secret_file_data(EncodeInfo *encInfo)
     {
        char ch;
        int i;

        rewind(encInfo->fptr_secret);

        for(i=0; i<encInfo->size_secret_file; i++)
        {
            if(fread(&ch, 1, 1, encInfo->fptr_secret) != 1)
            {
                return e_failure;
            }
            
            if(encode_data_to_image(&ch, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
            {
                return e_failure;
            }
        }
        return e_success;
    }



    /* Encode magic string */
    Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
    {
        return encode_data_to_image((char *)magic_string, 2, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    }



    /* perform encoding */
    Status do_encoding(EncodeInfo *encInfo)
    {
        printf("INFO: Opening required files\n");

        if(open_files(encInfo) == e_failure)
        {
             printf("ERROR: Unable to open files\n");
             return e_failure;
        }
           
         printf("INFO: Checking image capacity\n");

        if(check_capacity(encInfo) == e_failure)
        { 
             printf("ERROR: Image does not have enough capacity\n");
             goto cleanup;
        }

        printf("INFO: Copying BMP header\n");

        if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        {
             goto cleanup;
        }

        printf("INFO: Encoding magic string\n"); 

        if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
        {
             goto cleanup;
        }
           
        printf("INFO: Encoding secret file extension\n");

        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
        {
             goto cleanup;
        }

        printf("INFO: Encoding secret file size\n");

        encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

        if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
        {
            goto cleanup;
        }

        printf("INFO: Encoding secret file data\n");

        if(encode_secret_file_data(encInfo) == e_failure)
        {
            goto cleanup;
        }

        printf("INFO: Copying remaining image data\n");

        if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        {
            goto cleanup;
        }
           
        printf("SUCCESS: Encoding completed successfully\n");

        fclose(encInfo->fptr_src_image); 
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image); 

        encInfo->fptr_src_image = NULL;
        encInfo->fptr_secret = NULL;
        encInfo->fptr_stego_image = NULL;
        return e_success;

        cleanup: 
        if(encInfo->fptr_src_image != NULL)
        {
            fclose(encInfo->fptr_src_image);
            encInfo->fptr_src_image = NULL;
        }
        if(encInfo->fptr_secret != NULL)
        {
            fclose(encInfo->fptr_secret); 
            encInfo->fptr_secret = NULL;
        }
        if(encInfo->fptr_stego_image != NULL)
        {
            fclose(encInfo->fptr_stego_image); 
            encInfo->fptr_stego_image = NULL;
        }
        return e_failure; 
    }

