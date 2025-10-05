#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc != 1 && argv)
    {
        return 1;
    }

    int parent_to_child1[2];
    if (pipe(parent_to_child1) == -1)
    {
        const char error_msg[] = "Error: unable to create parent_to_child1 pipe\n";
        write(STDERR_FILENO, error_msg, sizeof(error_msg));
        exit(EXIT_FAILURE);
    }

    int child1_to_child2[2];
    if (pipe(child1_to_child2) == -1)
    {
        const char error_msg[] = "Error: unable to create child1_to_child2 pipe\n";
        write(STDERR_FILENO, error_msg, sizeof(error_msg));
        exit(EXIT_FAILURE);
    }

    int child2_to_parent[2];
    if (pipe(child2_to_parent) == -1)
    {
        const char error_msg[] = "Error: unable to create child2_to_parent pipe\n";
        write(STDERR_FILENO, error_msg, sizeof(error_msg));
        exit(EXIT_FAILURE);
    }

    pid_t child1_id = fork();
    if (child1_id == -1)
    {
        const char error_msg[] = "Error: unable to create child1\n";
        write(STDERR_FILENO, error_msg, sizeof(error_msg));
        exit(EXIT_FAILURE);
    }
    if (child1_id == 0)
    {
        close(parent_to_child1[1]);
        dup2(parent_to_child1[0], STDIN_FILENO);
        close(parent_to_child1[0]);

        close(child1_to_child2[0]);
        dup2(child1_to_child2[1], STDOUT_FILENO);
        close(child1_to_child2[1]);

        close(child2_to_parent[0]);
        close(child2_to_parent[1]);

        execl("./child1", "child1", NULL);

        const char error_msg[] = "Error: execl child1 failed\n";
        write(STDERR_FILENO, error_msg, sizeof(error_msg));
        exit(EXIT_FAILURE);
    }

    pid_t child2_id = fork();
    if (child2_id == -1)
    {
        const char error_msg[] = "Error: unable to create child2\n";
        write(STDERR_FILENO, error_msg, sizeof(error_msg));
        exit(EXIT_FAILURE);
    }
    if (child2_id == 0)
    {
        close(child1_to_child2[1]);
        dup2(child1_to_child2[0], STDIN_FILENO);
        close(child1_to_child2[0]);

        close(child2_to_parent[0]);
        dup2(child2_to_parent[1], STDOUT_FILENO);
        close(child2_to_parent[1]);

        close(parent_to_child1[0]);
        close(parent_to_child1[1]);

        execl("./child2", "child2", NULL);
        const char error_msg[] = "Error: execl child2 failed\n";

        write(STDERR_FILENO, error_msg, sizeof(error_msg));
        exit(EXIT_FAILURE);
    }

    close(parent_to_child1[0]);
    close(child1_to_child2[0]);
    close(child1_to_child2[1]);
    close(child2_to_parent[1]);

    const char msg[] = "Input text:\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    char buffer[128];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0)
    {
        write(parent_to_child1[1], buffer, bytes_read);

        ssize_t result_read = read(child2_to_parent[0], buffer, sizeof(buffer) - 1);
        if (result_read > 0)
        {
            buffer[result_read] = '\0';
            write(STDOUT_FILENO, "Processed string: ", 18);
            write(STDOUT_FILENO, buffer, result_read);
        }
    }

    close(parent_to_child1[1]);
    close(child2_to_parent[0]);

    int status;
    waitpid(child1_id, &status, 0);
    waitpid(child2_id, &status, 0);

    return 0;
}