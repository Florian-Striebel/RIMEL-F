#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../src/crc64.h"
#include "../src/sha1.h"
#define TABLE_SIZE (1<<24)
int Table[TABLE_SIZE];
uint64_t crc64Hash(char *key, size_t len) {
return crc64(0,(unsigned char*)key,len);
}
uint64_t sha1Hash(char *key, size_t len) {
SHA1_CTX ctx;
unsigned char hash[20];
SHA1Init(&ctx);
SHA1Update(&ctx,(unsigned char*)key,len);
SHA1Final(hash,&ctx);
uint64_t hash64;
memcpy(&hash64,hash,sizeof(hash64));
return hash64;
}
unsigned long testHashingFunction(uint64_t (*hash)(char *, size_t)) {
unsigned long collisions = 0;
memset(Table,0,sizeof(Table));
char *prefixes[] = {"object", "message", "user", NULL};
for (int i = 0; prefixes[i] != NULL; i++) {
for (int j = 0; j < TABLE_SIZE/2; j++) {
char keyname[128];
size_t keylen = snprintf(keyname,sizeof(keyname),"%s:%d",
prefixes[i],j);
uint64_t bucket = hash(keyname,keylen) % TABLE_SIZE;
if (Table[bucket]) {
collisions++;
} else {
Table[bucket] = 1;
}
}
}
return collisions;
}
int main(void) {
printf("SHA1 : %lu\n", testHashingFunction(sha1Hash));
printf("CRC64: %lu\n", testHashingFunction(crc64Hash));
return 0;
}
