#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define FILE_SIZE 100000000 // 100MB
#define LINE_LENGTH 1000
//#include <openssl/sha.h>

#define SHARED_MEM_NAME "/my_shared_memory"
#define SHARED_MEM_SIZE 100000000

int client() {
    printf("this is client...\n");
    int fd;
    char *shared_mem;

    // Open the text file to be shared
    FILE *file = fopen("random.txt", "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Create and open a shared memory file
    fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory file
    if (ftruncate(fd, SHARED_MEM_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory file into memory
    shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("check\n");

    // Copy the contents of the text file to the shared memory
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i=0;
    while ((read = getline(&line, &len, file)) != -1) {
        strcat(shared_mem, line);
        printf("%d\n",i);
        i++;
    }

    // Cleanup
    /*/fclose(file);
    munmap(shared_mem, SHARED_MEM_SIZE);
    close(fd);
    shm_unlink(SHARED_MEM_NAME);
    /*/
    return 0;
}

int server() {
    printf("this is server...\n");
    int fd;
    char *shared_mem;

    // Open the shared memory file
    fd = shm_open(SHARED_MEM_NAME, O_RDONLY, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory file into memory
    shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Print the contents of the shared memory
    printf("%s", shared_mem);

    // Cleanup
    munmap(shared_mem, SHARED_MEM_SIZE);
    close(fd);

    return 0;
}

int create_random_file(char* filename) {
    // Open file for writing
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Seed random number generator
    srand(time(NULL));

    // Write random data to file
    for (int i = 0; i < FILE_SIZE / LINE_LENGTH; i++) {
        // Generate random line
        char line[LINE_LENGTH + 1];
        for (int j = 0; j < LINE_LENGTH; j++) {
            line[j] = 'a' + rand() % 26; // generate random lowercase letter
        }
        line[LINE_LENGTH] = '\n'; // add newline character

        // Write line to file
        fwrite(line, 1, LINE_LENGTH + 1, fp);
    }

    // Close file
    fclose(fp);

    return 0;
}
int file_checksum(){
        FILE *file;
    unsigned int checksum = 0;
    int c;

    // Open the text file to be checked
    file = fopen("random.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    // Calculate the checksum of the file
    while ((c = fgetc(file)) != EOF) {
        checksum += c;
    }

    // Print the checksum
    printf("Checksum: %u\n", checksum);

    // Close the file
    fclose(file);

    return 0;
}

int hash(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return 1;
    }

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    unsigned char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        SHA256_Update(&ctx, buffer, bytes_read);
    }

    unsigned char checksum[SHA256_DIGEST_LENGTH];
    SHA256_Final(checksum, &ctx);

    fclose(file);

    printf("SHA-256 checksum of file %s:\n", filename);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", checksum[i]);
    }
    printf("\n");

    return 0;
}

int main(int argc, char* argv[]){
    create_random_file("random.txt");
    //file_checksum();
    /*/
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }/*/
    /*/
        if(argc == 1){
            server();
    }
        else{
            client();
    }/*/
}
