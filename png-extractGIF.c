/**
 * [PROVIDED CODE]: waf's solution
 */

#include <stdio.h>
#include <stdlib.h>
#include "lib/png.h"
#include <string.h>
#include <errno.h>

int png_extractGIF(const char *png_filename, const char *gif_filename) {
  // Open the image file specified in argv[1] for reading:
  PNG *png = PNG_open(png_filename, "r");
  if (!png) { return ERROR_INVALID_FILE; }

  // Open the gif file specified in argv[2] for writing:
  FILE *gif = fopen(gif_filename, "w");

  // Find the uiuc chunk
  PNG_Chunk chunk;
  while (1) {
    // Read chunk and ensure we get a valid result (exit on error):
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      return ERROR_NO_UIUC_CHUNK;
    }

    // Check for the uiuc chunk to exit:
    if ( strcmp(chunk.type, "uiuc") == 0 ) {
      break;
    }

    // Check for IEND to exit
    if ( strcmp(chunk.type, "IEND") == 0 ) {
      PNG_close(png);
      PNG_free_chunk(&chunk);
      return ERROR_NO_UIUC_CHUNK;
    }
    
    PNG_free_chunk(&chunk);
  }

  // Write the gif data into the gif file
  fwrite(chunk.data, chunk.len, 1, gif);

  // Close the files and free the chunk
  fclose(gif);
  PNG_close(png);
  PNG_free_chunk(&chunk);
  return 0;
}


int main(int argc, char *argv[]) {
  // Ensure the correct number of arguments:
  if (argc != 3) {
    printf("Usage: %s <PNG File> <GIF Name>\n", argv[0]);
    return ERROR_INVALID_PARAMS;
  }

  return png_extractGIF(argv[1], argv[2]);
}
