#include "main.h"

BmpImg * openBmpImg(char * path){
    FILE * fb = fopen(path, "r");
    if(fb == NULL){
        perror("Open Image Faild! \n");
        return NULL;
    }

    BmpImg * image = malloc(sizeof(BmpImg));

    fseek(fb, 0x12, 0);
    fread(image, 12, 1, fb);

    // 计算填充字节大小
    int filler;
    if(((image->width * (image->depth/8)))%4 == 0 ){
        filler = 0;
    }
    else{
        filler = 4 - ( (image->width * (image->depth/8)) % 4 );
    }
    image->fillerSize = filler;

    image->p = malloc(image->height * image->width * (image->depth/8));

    int readSize = 0;

    int i;
    for(i=0; i<image->height; i++){
        fseek(fb, 0x36 + i * ((image->width) * (image->depth/8) + image->fillerSize), 0);
        readSize += fread(image->p + (image->width * image->depth/8 * i ), image->depth/3, image->width, fb);
    }

    printf("%d has be Read\n", readSize);

    fclose(fb);
    return image;
}