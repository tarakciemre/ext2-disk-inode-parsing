#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

#define BLOCK_SIZE 4096

struct superblock {
    unsigned int s_inodes_count;
    unsigned int s_blocks_count;
    unsigned int s_r_blocks_count;
    unsigned int s_free_blocks_count;
    unsigned int s_free_inodes_count;
    unsigned int s_first_data_block;
    unsigned int s_log_block_size;
    unsigned int s_log_frag_size;
    unsigned int s_blocks_per_group;
    unsigned int s_frags_per_group;
    unsigned int s_inodes_per_group;
};

char buffer[BLOCK_SIZE];
int fd;

struct inode {
    unsigned short i_mode; 
    unsigned short i_uid; 
    unsigned int i_size; 
    unsigned int i_atime; 
    unsigned int i_ctime; 
    unsigned int i_mtime; 
    unsigned int i_dtime; 
    unsigned short i_gid; 
    unsigned short i_links_count; 
    unsigned int i_blocks; 
    unsigned int i_flags; 
    unsigned int i_osd1; 
    unsigned int i_block[15]; 
};

struct dir_entry {
    unsigned int inode;       
    unsigned short rec_len;   
    unsigned char name_len;   
    unsigned char file_type;  
    char name[255];           
};

int bg_inode_table;

void parse_superblock(struct superblock* sb) {
    memcpy(&sb->s_inodes_count, buffer + 0 + 1024, sizeof(int));
    memcpy(&sb->s_blocks_count, buffer + 4 + 1024, sizeof(int));
    memcpy(&sb->s_r_blocks_count, buffer + 8 + 1024, sizeof(int));
    memcpy(&sb->s_free_blocks_count, buffer + 12 + 1024, sizeof(int));
    memcpy(&sb->s_free_inodes_count, buffer + 16 + 1024, sizeof(int));
    memcpy(&sb->s_first_data_block, buffer + 20 + 1024, sizeof(int));
    memcpy(&sb->s_log_block_size, buffer + 24 + 1024, sizeof(int));
    memcpy(&sb->s_log_frag_size, buffer + 28 + 1024, sizeof(int));
    memcpy(&sb->s_blocks_per_group, buffer + 32 + 1024, sizeof(int));
    memcpy(&sb->s_frags_per_group, buffer + 36 + 1024, sizeof(int));
    memcpy(&sb->s_inodes_per_group, buffer + 40 + 1024, sizeof(int));

    printf("Superblock Information:\n");
    printf("s_inodes_count:  \t%u \n", sb->s_inodes_count);
    printf("s_blocks_count:  \t%u \n", sb->s_blocks_count);
    printf("s_r_blocks_count: \t%u \n", sb->s_r_blocks_count);
    printf("s_free_blocks_count: \t%u \n", sb->s_free_blocks_count);
    printf("s_free_inodes_count: \t%u \n", sb->s_free_inodes_count);
    printf("s_first_data_block: \t%u \n", sb->s_first_data_block);
    printf("s_log_block_size: \t%u \n", sb->s_log_block_size);
    printf("s_log_frag_size: \t%u \n", sb->s_log_frag_size);
    printf("s_blocks_per_group: \t%u \n", sb->s_blocks_per_group);
    printf("s_frags_per_group: \t%u \n", sb->s_frags_per_group);
    printf("s_inodes_per_group: \t%u \n", sb->s_inodes_per_group);

};

void parse_inode(struct inode* in, int inode_num, int print_mode) {
    // parse inode information
    memcpy(&in->i_mode, buffer + (inode_num * 128), sizeof(short));
    memcpy(&in->i_uid, buffer + 2 + (inode_num * 128), sizeof(short));
    memcpy(&in->i_size, buffer + 4 + (inode_num * 128), sizeof(int));
    memcpy(&in->i_atime, buffer + 8 + (inode_num * 128), sizeof(int));
    memcpy(&in->i_ctime, buffer + 12 + (inode_num * 128), sizeof(int));
    memcpy(&in->i_mtime, buffer + 16 + (inode_num * 128), sizeof(int));
    memcpy(&in->i_dtime, buffer + 20 + (inode_num * 128), sizeof(int));
    memcpy(&in->i_gid, buffer + 24 + (inode_num * 128), sizeof(short));
    memcpy(&in->i_links_count, buffer + 26 + (inode_num * 128), sizeof(short));
    memcpy(&in->i_blocks, buffer + 28 + (inode_num * 128), sizeof(int));
    memcpy(&in->i_block, buffer + 40 + (inode_num * 128), sizeof(int) * 15);

    if (print_mode == 1) {
        printf("\ni_mode: \t%hu \n", in->i_mode);
        printf("i_uid:  \t%hu \n", in->i_uid);
        printf("i_size: \t%u \n", in->i_size);
        printf("i_atime: \t%u \n", in->i_atime);
        printf("i_ctime: \t%u \n", in->i_ctime);
        printf("i_mtime: \t%u \n", in->i_mtime);
        printf("i_dtime: \t%u \n", in->i_dtime);
        printf("i_gid:  \t%hu \n", in->i_gid);
        printf("i_links_count: \t%hu \n", in->i_links_count);
        printf("i_blocks: \t%u \n", in->i_blocks);
    } 

    /*
    for (int i = 0; i < 15; i++) {
        //printf("i_block[%d]: %d \n", i, in->i_block[i]);
    }
    */
};

void parse_block_descriptor () {
    memcpy(&bg_inode_table, buffer + 8, sizeof(int));
    printf("bg_inode_table: %d \n", bg_inode_table);
};

void read_block(int block_num) {
    lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    read(fd, buffer, BLOCK_SIZE);
}

void parse_directory_entry( struct dir_entry* de, int offset, int print_mode) {
    // parse directory entry information
    memcpy(&de->inode, buffer + offset + 0, sizeof(int));
    memcpy(&de->rec_len, buffer + offset + 4, sizeof(short));
    memcpy(&de->name_len, buffer + offset + 6, sizeof(char));
    memcpy(&de->file_type, buffer + offset + 7, sizeof(char));
    memcpy(&de->name, buffer + offset + 8, sizeof(char) * 255);
    if(print_mode == 1){
        printf("inode: %d rec_len: %d name_len: %d file_type: %d name: %s \n", de->inode, de->rec_len, de->name_len, de->file_type, de->name);
    }
    else if(print_mode == 0){
        printf("%s \n", de->name);
    }
}

void print_buffer () {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%c", buffer[i]);
    }
}

void access_inode (int inode_num, int print_mode) {
    struct inode my_inode;
    read_block(bg_inode_table);
    parse_inode(&my_inode, inode_num, print_mode);
}


int main(int argc, char **argv) {

    char file_name[255];
    strcpy(file_name, argv[1]);
    // OPEN DISK
    fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        perror("Error opening ext2 filesystem");
        return 1;
    }

    // READ SUPERBLOCK
    read_block(0);
    struct superblock sb;
    printf("\n--------------------\n");
    parse_superblock(&sb);

    // READ BLOCK DESCRIPTOR
    read_block(1);
    parse_block_descriptor();

    // READ INODE TABLE 
    read_block(bg_inode_table);
    struct inode in;
    parse_inode(&in, 1, 0);

    // READ DIRECTORY ENTRY
    read_block(in.i_block[0]);
    struct dir_entry de;
    int DIRECTORY_ENTRY_SIZE = 12;

    struct dir_entry* dir_block = malloc(BLOCK_SIZE);
    read(fd, dir_block, BLOCK_SIZE);

    int k = 0;
    // iterate through all directory entries in this block
    printf("--------------------\n");
    printf("\nFile names: \n");
    while (k < BLOCK_SIZE ) {
        read_block(in.i_block[0]);
        parse_directory_entry(&de, k, 0);
        k += de.rec_len;
    }
    k = 24;
    printf("\n--------------------\n");
    printf("\nContents of the inodes of directory entries: \n");
    while (k < BLOCK_SIZE ) {
        printf("\n");
        read_block(in.i_block[0]);
        parse_directory_entry(&de, k, 1);
        k += de.rec_len;
        printf("accessing inode: %d \n", de.inode);
        access_inode(de.inode, 1);
    }

    free(dir_block);
    close(fd);
    return 0;
}
