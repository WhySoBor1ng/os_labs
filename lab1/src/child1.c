#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc != 1 && argv)
    {
        const char msg[] = "Error: invalid argument count\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        return 1;
    }

    char buffer[256];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0';

        for (ssize_t i = 0; i < bytes_read; ++i)
        {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }

        ssize_t bytes_written = write(STDOUT_FILENO, buffer, bytes_read);
        if (bytes_written != bytes_read)
        {
            const char msg[] = "Error: unable to write to pipe\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            return 1;
        }
    }

    if (bytes_read < 0)
    {
        const char msg[] = "Error: unable to read from pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        return 1;
    }

    return 0;
}
