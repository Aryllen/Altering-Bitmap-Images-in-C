/*
 * Nicole Kauer
 * Winter 2018
 *
 * Code tested on 32-bit Linux Mint "Cinnamon" per class requirements.
 * 
 * Code issues: 
 * This code does not allow for the user to put in the wrong size for the image.
 * I also did not do any checking for user input other than checking that the
 * file opens based off the file name given.  It is assumed the user knows what
 * they are doing.
 *
 * Extra credit attempt:
 * Outputs a fifth version in grayscale.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HEADER_SIZE 54          // Bitmap header size
#define PIXEL_COL 3             // Number of columns/pixel
#define MAX_VAL 255             // Max pixel value
#define MID_VAL 128             // Middle pixel value
#define MIN_VAL 0               // Min pixel value
#define CONTRAST_RATIO 2.9695   // Contrast ratio
#define GRAY_RED 0.2126         // Grayscale coefficient for red
#define GRAY_GREEN 0.7152       // Grayscale coefficient for green
#define GRAY_BLUE 0.0722        // Grayscale coefficient for blue
#define FILE_EXT ".bmp"         // Extension to add to filename
#define OUT_FILE_1 "copy1.bmp"  // Output file for inverted color image
#define OUT_FILE_2 "copy2.bmp"  // Output file for increased contrast image
#define OUT_FILE_3 "copy3.bmp"  // Output file for mirrored and flipped image
#define OUT_FILE_4 "copy4.bmp"  // Output file for scaled and repeated image
#define OUT_FILE_5 "copy5.bmp"  // Output file for grayscale image
#define TRUE 1                  // Boolean true
#define FALSE 0                 // Boolean false

void invertColor(unsigned char*, unsigned char*, int, int, unsigned char*);
void increaseContrast(unsigned char*, unsigned char*,int, int, unsigned char*); 
void mirrorFlip(unsigned char*, unsigned char*, int, int, unsigned char*);
void scaleRepeat(unsigned char*, unsigned char*, int, int, unsigned char*);
void grayscale(unsigned char*, unsigned char*, int, int, unsigned char*);
void runOutScanf();

/** 
 * Driver.
 */
int main(void) {

    unsigned char *myOriginal, *myAltered; // Arrays to hold images
    unsigned char myHeader[HEADER_SIZE];   // Bitmap header
    int myHeight;           // Image height
    int myWidth;            // Image width
    int isGoodInfo = FALSE; // Flag to stop asking for user info

    /* Get file data */

    do {
        // Get user info; filename restricted to header size because arbitrary
        char myFile[HEADER_SIZE];    

        // Dialog
        printf("Enter the filename: ");
        fgets(myFile, HEADER_SIZE, stdin);

        // Get rid of newline character
        int i = 0;
        while (i < strlen(myFile)) {
            if (myFile[i] == '\n') {
                myFile[i] = myFile[i + 1];
                while (i < strlen(myFile)) {
                    myFile[i] = myFile[i + 1];
                    i++;
                }
            }
            i++;
        }

        // Append the file extension for bitmap files
        strcat(myFile, FILE_EXT);

        printf("Enter height and width (in pixels): ");
        scanf("%d%d", &myHeight, &myWidth);
        runOutScanf();    // Clear scanf

        // Use info to make image array from file

        // Pointer for image file
        FILE *imageFile; 

        // Handle error if file not found
        if ((imageFile = fopen(myFile, "rb")) == NULL) {
            printf("\nThe file could not be opened. Please try again.\n");
            isGoodInfo = FALSE; // Still false; repeat
        } else {
            isGoodInfo = TRUE;  // Don't repeat  

            // Allocate memory for original image array
            myOriginal = (unsigned char*) 
                malloc(myHeight * myWidth * PIXEL_COL * sizeof(char));
            
            // Read file into original image array; close file when done
            fread(myHeader, 1, HEADER_SIZE, imageFile);
            fread(myOriginal, 1, myHeight * myWidth * PIXEL_COL, imageFile);
            fclose(imageFile); 
        }

    } while (isGoodInfo == FALSE);

    // Allocation memory for altered image array
    myAltered = (unsigned char*) 
        malloc(myHeight * myWidth * PIXEL_COL * sizeof(char));

    /* Alter image and print to file */

    // Invert color of image
    invertColor(myOriginal, myAltered, myHeight, myWidth, myHeader);

    // Increase image contrast
    increaseContrast(myOriginal, myAltered, myHeight, myWidth, myHeader);

    // Mirror and flip image
    mirrorFlip(myOriginal, myAltered, myHeight, myWidth, myHeader);

    // Grayscale image
    grayscale(myOriginal, myAltered, myHeight, myWidth, myHeader);

    // Scale and repeat image
    scaleRepeat(myOriginal, myAltered, myHeight, myWidth, myHeader);

    // Print final message when done
    printf("\nDone. Check the generated images.\n");

    free(myOriginal);
    free(myAltered);
}

/*******************************************************************************
 * Image altering methods begin.
 ******************************************************************************/

/**
 * Inverts image colors.
 * @param theImage address to the image array
 * @param theAltered address to store the altered version of image in
 */
void invertColor(unsigned char *theImage, unsigned char *theAltered, 
                    int theHeight, int theWidth, unsigned char *theHeader) { 

    int rowWidth = theWidth * PIXEL_COL; // To avoid constantly calculating this
    int row = 0; 
    int col = 0;
    short temp;  // Use a short so doesn't wrap if outside char range

    for (row = 0; row < theHeight; row++) {
        for (col = 0; col < rowWidth; col++) {
            
            // Calculate new color value
            temp = MAX_VAL - theImage[row * rowWidth + col];
            
            // Bounds check; store correct value
            if (temp < MIN_VAL) {
                theAltered[row * rowWidth + col] = MIN_VAL; 
            } else if (temp > MAX_VAL) {
                theAltered[row * rowWidth + col] = MAX_VAL;
            } else {
                theAltered[row * rowWidth + col] = (char) temp;
            }
        }
    }

    // Pointer to output file
    FILE *outputFile;

    // Create file and write array to it; give error if problem making file
    if ((outputFile = fopen(OUT_FILE_1, "wb")) == NULL) {
        printf("\nThe inverted color file could not be created.\n");
    } else {
        fwrite(theHeader, sizeof(char), HEADER_SIZE, outputFile);
        fwrite(theAltered, sizeof(char), theHeight * rowWidth, outputFile); 
        fclose(outputFile);   
    }
}

/**
 * Increases image contrast.

 * @param theImage address to the image array
 * @param theAltered address to store the altered version of image in
 * @param theHeight image height
 * @param theWidth image width
 * @param theHeader address to the header for the bitmap file
 */
void increaseContrast(unsigned char *theImage, unsigned char *theAltered, 
                    int theHeight, int theWidth, unsigned char *theHeader) {

    int rowWidth = theWidth * PIXEL_COL; // To avoid constantly calculating this
    int row = 0;
    int col = 0;
    short temp;

    for (row = 0; row < theHeight; row++) {
        for (col = 0; col < rowWidth; col++) {

            // Calculate and store value temporarily
            temp = (CONTRAST_RATIO * 
                ((short) theImage[row * rowWidth + col] - MID_VAL)) + MID_VAL; 

            // Range check  and store correct value
            if (temp < MIN_VAL) {
                theAltered[row * rowWidth + col] = MIN_VAL;  
            } else if (temp > MAX_VAL) {
                theAltered[row * rowWidth + col] = MAX_VAL;  
            } else {
                theAltered[row * rowWidth + col] = (unsigned char) temp;     
            }
        }
    }

    // Pointer to output file
    FILE *outputFile;

    // Create file and write array to it; give error if problem making file
    if ((outputFile = fopen(OUT_FILE_2, "wb")) == NULL) {
        printf("\nThe increased contrast file could not be created.\n");
    } else {
        fwrite(theHeader, sizeof(char), HEADER_SIZE, outputFile);
        fwrite(theAltered, sizeof(char), theHeight * rowWidth, outputFile); 
        fclose(outputFile);   
    }
}

/**
 * Mirrors image vertically and flips image horizontally.
 *
 * @param theImage address to the image array
 * @param theAltered address to store the altered version of image in
 * @param theHeight image height
 * @param theWidth image width
 * @param theHeader address to the header for the bitmap file
 */
void mirrorFlip(unsigned char *theImage, unsigned char *theAltered, 
                    int theHeight, int theWidth, unsigned char *theHeader) {

    int rowWidth = theWidth * PIXEL_COL; // To avoid constantly calculating this
    int row = 0;
    int col = 1;

    for (row = 0; row < theHeight; row++) {
        for (col = 1; col <= (theWidth / 2) * PIXEL_COL; col += PIXEL_COL) {
            // Blue
            theAltered[(theHeight - row - 1) * rowWidth + (col - 1)] 
                = theImage[row * rowWidth + (col - 1)];
            theAltered[(theHeight - row - 1) * rowWidth + (rowWidth - 2 - col)] 
                = theImage[row * rowWidth + (col - 1)]; 

            // Green
            theAltered[(theHeight - row - 1) * rowWidth + col] 
                = theImage[row * rowWidth + col];
            theAltered[(theHeight - row - 1) * rowWidth + (rowWidth - 1 - col)] 
                = theImage[row * rowWidth + col]; 

            // Red
            theAltered[(theHeight - row - 1) * rowWidth + (col + 1)] 
                = theImage[row * rowWidth + (col + 1)];
            theAltered[(theHeight - row - 1) * rowWidth + (rowWidth - col)] 
                = theImage[row * rowWidth + (col + 1)]; 
        }
    }

    // For odd widths, copy the middle row twice
    if ((theWidth / 2 ) % 2 != 0) {
        for (row = 0; row < theHeight; row++) {
            theAltered[row * rowWidth + ((theWidth / 2) * PIXEL_COL + 1)] 
                = theAltered[row * rowWidth + ((theWidth / 2) * PIXEL_COL)];
        }
    }

    // Pointer to output file
    FILE *outputFile;

    // Create file and write array to it; give error if problem making file
    if ((outputFile = fopen(OUT_FILE_3, "wb")) == NULL) {
        printf("\nThe mirrored and flipped file could not be created.\n");
    } else {
        fwrite(theHeader, sizeof(char), HEADER_SIZE, outputFile);
        fwrite(theAltered, sizeof(char), theHeight * rowWidth, outputFile); 
        fclose(outputFile);   
    }
}

/**
 * Scales down and repeats image, with different colors represented.  This will
 * alter the original image array. Create this last so theImage array does
 * not need to be returned to the original image state.
 *
 * @param theImage address to the image array
 * @param theAltered address to store the altered version of image in
 * @param theHeight image height
 * @param theWidth image width
 * @param theHeader address to the header for the bitmap file
 */
void scaleRepeat(unsigned char *theImage, unsigned char *theAltered, 
                    int theHeight, int theWidth, unsigned char *theHeader) {

    int rowWidth = theWidth * PIXEL_COL; // To avoid constantly calculating this
    int row = 0;
    int col = 1;

    // Scale image down in place
    for (row = 0; row < theHeight; row++) {
      for (col = 1; col < rowWidth; col += PIXEL_COL) {
        if (col % 2 == 0) {
            // Blue
            theImage[(row / 2) * rowWidth + (((col - 1) / 2 ) - 1)] 
                = theImage[row * rowWidth + (col - 1)]; 

            // Green
            theImage[(row / 2) * rowWidth + ((col / 2) - 1)] 
                = theImage[row * rowWidth + col]; 

            // Red
            theImage[(row / 2) * rowWidth + (col / 2)] 
                = theImage[row * rowWidth + (col + 1)];
        } else {
            // Blue
            theImage[(row / 2) * rowWidth + ((col - 1) / 2)] 
                = theImage[row * rowWidth + (col - 1)]; 

            // Green
            theImage[(row / 2) * rowWidth + ((col / 2) + 1)] 
                = theImage[row * rowWidth + col]; 

            // Red
            theImage[(row / 2) * rowWidth + (((col + 1) / 2) + 1)] 
                = theImage[row * rowWidth + (col + 1)];
        }        
      }  
    }

    // Restart index variables
    row = 0;
    col = 0;

    // Repeat by copying scaled down image to the altered array with the correct
    // colors from each pixel going to the correct section of the array.
    for (row = 0; row < theHeight / 2; row++) {
      for (col = 0; col < rowWidth / 2; col++) {

        // Red portion of pixel goes to upper left
        // Red portion of pixel in upper right, lower left --> 0
        if (col % PIXEL_COL == 2) {
          theAltered[(row + (theHeight / 2)) * rowWidth + col] 
                = theImage[row * rowWidth + col];         
          theAltered[row * rowWidth + col] = MIN_VAL;                                  
          theAltered[(row + (theHeight / 2)) * rowWidth + (col + (rowWidth / 2))]     
                = MIN_VAL; 
        } 

        // Green portion of pixel goes to upper right
        // Green portion of pixel in upper and lower left --> 0
        if (col % PIXEL_COL == 1) {
          theAltered[(row + (theHeight / 2)) * rowWidth + (col + (rowWidth / 2))] 
                = theImage[row * rowWidth + col];                           
          theAltered[row * rowWidth + col] = MIN_VAL;                          
          theAltered[(row + (theHeight / 2)) * rowWidth + col] = MIN_VAL; 
        }

        // Blue portion of pixel goes to lower left
        // Blue portion of pixel in upper left and right --> 0
        if (col % PIXEL_COL == 0) {
          theAltered[row * rowWidth + col] = theImage[row * rowWidth + col];                     
          theAltered[(row + (theHeight / 2)) * rowWidth + (col + (rowWidth / 2))] 
                = MIN_VAL;
          theAltered[(row + (theHeight / 2)) * rowWidth + col] = MIN_VAL; 
        }

        // All portions of pixel go to lower right
        theAltered[row * rowWidth + (col + ((rowWidth) / 2))] 
                = theImage[row * rowWidth + col]; 
      }  
    }

    // Pointer to output file
    FILE *outputFile;

    // Create file and write array to it; give error if problem making file
    if ((outputFile = fopen(OUT_FILE_4, "wb")) == NULL) {
        printf("\nThe scaled and repeated file could not be created.\n");
    } else {
        fwrite(theHeader, sizeof(char), HEADER_SIZE, outputFile);
        fwrite(theAltered, sizeof(char), theHeight * rowWidth, outputFile); 
        fclose(outputFile);   
    }  
}

/**
 * Changes image to grayscale. Code based on Method 2 psuedocode with ITU-R 
 * recommendation found at:
 * www.tannerhelland.com/3643/grayscale-image-algorithm-vb6/
 *
 * @param theImage address to the image array
 * @param theAltered address to store the altered version of image in
 * @param theHeight image height
 * @param theWidth image width
 * @param theHeader address to the header for the bitmap file
 */
void grayscale(unsigned char *theImage, unsigned char *theAltered, 
                    int theHeight, int theWidth, unsigned char *theHeader) { 

    int rowWidth = theWidth * PIXEL_COL; // To avoid constantly calculating this
    int row = 0; 
    int col = 0;
    short temp;

    for (row = 0; row < theHeight; row++) {
        for (col = 0; col < rowWidth; col += PIXEL_COL) {

            // Calculate gray value
            temp = GRAY_BLUE * theImage[row * rowWidth + col] 
                    + GRAY_GREEN * theImage[row * rowWidth + (col + 1)] 
                    + GRAY_RED * theImage[row * rowWidth + (col + 2)];

            // Bounds check
            if (temp < MIN_VAL) {
                temp = MIN_VAL;
            } else if (temp > MAX_VAL) {
                temp = MAX_VAL;
            } 

            // Store gray value in all three RGB spots for the pixel
            theAltered[row * rowWidth + col] = (unsigned char) temp;
            theAltered[row * rowWidth + (col + 1)] = (unsigned char) temp;
            theAltered[row * rowWidth + (col + 2)] = (unsigned char) temp;
        }
    }

    // Pointer to output file
    FILE *outputFile;

    // Create file and write array to it; give error if problem making file
    if ((outputFile = fopen(OUT_FILE_5, "wb")) == NULL) {
        printf("\nThe scaled and repeated file could not be created.\n");
    } else {
        fwrite(theHeader, sizeof(char), HEADER_SIZE, outputFile);
        fwrite(theAltered, sizeof(char), theHeight * rowWidth, outputFile); 
        fclose(outputFile);   
    }  
}

/*******************************************************************************
 * Image altering methods end.
 ******************************************************************************/
/*******************************************************************************
 * Miscellaneous helper methods begin.
 ******************************************************************************/

/**
 * Runs out scanf buffer.
 */
void runOutScanf() {

    char garbage;

    do {
        scanf("%c", &garbage);
    } while (garbage != '\n');
}

/*******************************************************************************
 * Miscellaneous helper methods end.
 ******************************************************************************/