/* A.Srinath
   Date:25/10/2026
   Description : Developed an Image Steganography project using C programming to hide secret text files inside BMP images and 
                 retrieve the hidden information without visibly changing the image.Implemented Least Significant Bit (LSB)
                 encoding and decoding techniques to embed secret data into image pixel data.
*/

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    OperationType operation;

    // Step 1: Check operation type
    operation = check_operation_type(argv);

    if (operation == e_encode)
    {
       // Step 2: Validate command-line arguments
        if (argc != 5)
        {
            printf("Usage: %s -e <source.bmp> <secret.txt> <output.bmp>\n",
                   argv[0]);
            return 1;
        }

        if (read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            printf("ERROR: Invalid input arguments\n");
            return 1;
        }

        // Step 3: Perform encoding
        if (do_encoding(&encInfo) == e_failure)
        {
            printf("ERROR: Encoding failed\n");
            return 1;
        }

        printf("SUCCESS: Encoding completed!!\n");
    }


    
    // decode part
    else if (operation == e_decode)
    {
        DecodeInfo decInfo;

        if(argc != 4)
        {
            printf("Usage: %s -d <stego_image.bmp> <output_file>\n",argv[0]);
            return 1;
        }
        decInfo.stego_image_fname = argv[2];
        decInfo.output_fname = argv[3];

        if(do_decoding(&decInfo) == e_failure)
        {
            printf("ERROR: Decoding failed\n");
            return 1;
        }
           printf("SUCCESS: Decoding completed!!\n");
    }
    return 0;
}


