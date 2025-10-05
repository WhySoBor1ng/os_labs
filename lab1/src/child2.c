#include <unistd.h>
#include <stdlib.h>
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

        char result[256];
        int i = 0, j = 0;
        int space_found = 0;
        while (buffer[i] != '\0')
        {
            if (buffer[i] == ' ')
            {
                if (!space_found)
                {
                    result[j++] = ' ';
                    space_found = 1;
                }
            }
            else
            {
                result[j++] = buffer[i];
                space_found = 0;
            }
            i++;
        }
        result[j] = '\0';

        ssize_t to_write = j;
        ssize_t bytes_written = write(STDOUT_FILENO, result, to_write);
        if (bytes_written != to_write)
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
