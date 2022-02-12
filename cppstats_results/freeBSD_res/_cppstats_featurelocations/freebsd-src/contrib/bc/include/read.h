


































#if !defined(BC_READ_H)
#define BC_READ_H

#include <stdlib.h>

#include <status.h>
#include <vector.h>






#define BC_READ_BIN_CHAR(c) (!(c))






BcStatus bc_read_line(BcVec *vec, const char *prompt);






char* bc_read_file(const char *path);








BcStatus bc_read_chars(BcVec *vec, const char *prompt);







bool bc_read_buf(BcVec *vec, char *buf, size_t *buf_len);

#endif
