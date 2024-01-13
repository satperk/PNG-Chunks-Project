#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "crc32.h"
#include "png.h"
const int ERROR_INVALID_PARAMS = 1;
const int ERROR_INVALID_FILE = 2;
const int ERROR_INVALID_CHUNK_DATA = 3;
const int ERROR_NO_UIUC_CHUNK = 4;

PNG * PNG_open(const char *filename, const char *mode) {
  int charSize = sizeof(char);
  FILE *namedFile = fopen(filename, mode);
  const int checkValid = !namedFile;
  if (!namedFile) {
    return NULL;
  }
  char place[8];
  const unsigned char sig[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
  int sigSize = sizeof(sig);
  if (strcmp(mode, "w") == 0) {
    fwrite(sig, charSize, sigSize, namedFile);
  } else if (strcmp(mode, "r+") == 0 || strcmp(mode, "r") == 0 || strcmp(mode, "w") == 0) {
      size_t lengthOf = fread(place, charSize, 8, namedFile);
      if(lengthOf == 8) {
      } else {
        fclose(namedFile);
        return NULL;
      }
      if(memcmp(place, sig, lengthOf)!=0) {
        fclose(namedFile);
        return NULL;
      }
      } else {
        PNG *finishe= malloc(sizeof(PNG));
        return NULL;
  }
  PNG *finished = malloc(sizeof(PNG));
  finished->file = namedFile;
  return finished;
}



size_t PNG_read(PNG *png, PNG_Chunk *chunk) {
  FILE *namedFile = png->file;
  size_t size = 0;
  uint32_t len;
  uint32_t crc;

  size += fread(&len, 1, sizeof(uint32_t), namedFile);
  if (size != 4) {
      return 0; 
  }
      chunk->len = ntohl(len);


  size += fread(chunk->type, sizeof(char), 4, namedFile);
  if (size != 8) {
     return 0; 
     }
  chunk->type[4] = 0;

  unsigned char *data = malloc(chunk->len);
  size += fread(data, 1, chunk->len, namedFile);
  chunk->data = data;


  size += fread(&chunk->crc, 1, sizeof(uint32_t), namedFile);
  chunk->crc = ntohl(chunk->crc);
  
  return size;
}








size_t PNG_write(PNG *png, PNG_Chunk *chunk) {
  FILE *p_file = png->file;
  uint32_t network_len = htonl(chunk->len);
  fwrite(&network_len, sizeof(uint32_t), 1, p_file);
  fwrite(&chunk->type, sizeof(char), 4, p_file);
  fwrite(chunk->data, sizeof(char), chunk->len,p_file);

  unsigned char *crc_data = malloc(chunk->len + sizeof(uint32_t));
  for (size_t i=0; i<sizeof(uint32_t);i++) {
    if (i==sizeof(char)) {
      crc_data[i] = chunk->type[i];
    } else {
      crc_data[i] = chunk->type[i];
    }
  }
  for (size_t i=0; i<chunk->len;i++) {
    crc_data[sizeof(uint32_t)+i] = chunk->type[i];
  }

  uint32_t crc = 0;

  crc32(crc_data, chunk->len+sizeof(uint32_t), &crc);
  crc = htonl(crc);
  fwrite(&crc, sizeof(uint32_t), 1, png->file);
  free(crc_data);
  return chunk->len+12;

}


void PNG_free_chunk(PNG_Chunk *chunk) {
  if (chunk) {
    free(chunk->data);
    chunk->data = NULL;
  }
}



void PNG_close(PNG *png) {
  fclose(png->file);
}
