# Image Steganography Using C

## Project Description

Image Steganography is a C programming project that hides secret text data inside a BMP image and retrieves the hidden data using the Least Significant Bit (LSB) technique.

## Objectives

* Hide secret text inside an image.
* Extract hidden information from an encoded image.
* Learn file handling and bitwise operations in C.
* Implement encoding and decoding techniques.

## Technologies Used

* Programming Language: C
* Compiler: GCC
* Image Format: BMP
* Technique: Least Significant Bit (LSB)

## Project Files

* `main.c` - Main program
* `encode.c` - Encoding implementation
* `encode.h` - Encoding header file
* `decode.c` - Decoding implementation
* `decode.h` - Decoding header file
* `common.h` - Common definitions
* `types.h` - Status and operation types
* `beautiful.bmp` - Original image
* `secret.txt` - Text file containing secret data

## Compilation

```bash
gcc main.c encode.c decode.c -o stego
```

## How to Run

### Encoding

```bash
./stego -e beautiful.bmp secret.txt stego_img.bmp
```

### Decoding

```bash
./stego -d stego_img.bmp decoded_secret.txt
```

### Verify the Output

```bash
cmp secret.txt decoded_secret.txt
```

If the command produces no output, the original and decoded files are identical.

## Concepts Learned

* C programming
* File handling
* Structures and pointers
* Bitwise operations
* Command-line arguments
* LSB encoding and decoding

## Conclusion

This project demonstrates how to hide and retrieve secret text data inside a BMP image using C programming and the LSB steganography technique.

## Author

A.Srinath
