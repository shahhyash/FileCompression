#include "fileIO.h"

#include <unistd.h>

/*
 *    Reads nbyte bytes from the filedescriptor filedes and stores them in buf.
 *    Returns 1 if successful. Otherwise returns a value according to the error.
 */
int better_read(int filedes, char * buf, size_t nbyte, char * file, int line)
{
        ssize_t ret = 1;
        int bytes_read = 0;
        while (nbyte-bytes_read > 0 && ret != 0)
        {
                ret = read(filedes, &buf[bytes_read], nbyte-bytes_read);
                if (ret > 0)
                {
                        bytes_read += ret;
                }
                else if (ret < 0)
                {
                        fprintf(stderr, "[better_read] Error. FILE: %s. Line: %d.\n", file, line);
                        return ret;
                }
                else if (ret == 0 && nbyte-bytes_read != 0)
                {
                        fprintf(stderr, "[better_read] End of file before all bytes read. FILE: %s. Line: %d.\n", file, line);
                        return ret;
                }
        }
        return 1;
}

/*
 *    Writes nbyte bytes from the buf and stores them in file with filedescriptor filedes.
 *    Returns 1 if successful. Otherwise returns a value according to the error.
 */
int better_write(int filedes, char * buf, size_t nbyte, char * file, int line)
{
        ssize_t ret = 1;
        int bytes_written = 0;
        while (nbyte-bytes_written > 0 && ret != 0)
        {
                ret = write(filedes, &buf[bytes_written], nbyte-bytes_written);
                if (ret > 0)
                {
                        bytes_written += ret;
                }
                else if (ret < 0)
                {
                        fprintf(stderr, "[better_write] Error. FILE: %s. Line: %d.\n", file, line);
                        return ret;
                }
                else if (ret == 0 && nbyte-bytes_written != 0)
                {
                        fprintf(stderr, "[better_write] End of file before all bytes read. FILE: %s. Line: %d.\n", file, line);
                        return ret;
                }
        }
        return 1;
}
