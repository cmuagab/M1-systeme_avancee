#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    
    if(argc != 3) {
        perror("Il faut uniquement un nom de fichier en paramètre.");
        return -1;
    } 

    int src = open(argv[1], O_RDONLY);
    int dest = open(argv[2], O_RDWR | O_CREAT, 0775);

    if(src == -1 || dest == -1) {
        perror("Impossible d'ouvrir le fichier.");
        return -1;
    }

    struct stat sb;
    if(stat(argv[1], &sb) == -1) {
        perror("Erreur de stat.");
        return -1;
    }

    size_t length = sb.st_size;

    char *mapSrc = mmap(0, length, PROT_READ, MAP_PRIVATE, src, 0);

    if(mapSrc == MAP_FAILED) {
        perror("Erreur de lecture");
        return -1;
    }

    if(ftruncate(dest, length) == -1) {
        perror("Erreur de truncate.");
        return -1;
    }


    char *mapDest = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED, dest, 0);
    
    memcpy(mapDest, mapSrc, length);

    if(msync(mapDest, length, MS_SYNC) == -1) {
        perror("Erreur de sync.");
        return -1;
    }

    close(src);
    close(dest);

    return 0;
}
