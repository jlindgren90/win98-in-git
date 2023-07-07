#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <utime.h>
#include <glib.h>

#define MAXLINE 4096

static char line[MAXLINE];
static char path[MAXLINE];
static char path2[MAXLINE];

int main(void)
{
    FILE *in = fopen("meta.sh", "r");
    if (!in)
    {
        fprintf(stderr, "Cannot open meta.sh\n");
        return EXIT_FAILURE;
    }

    int unchanged = 0;
    int updated = 0;
    int errors = 0;

    while (fgets(line, MAXLINE, in))
    {
        int mode;
        long ts, tsfrac;
        struct stat st;

        if (sscanf(line, "mkdir -p \"%[^\"]\"", path) == 1)
        {
            if (g_file_test(path, G_FILE_TEST_IS_DIR))
            {
                unchanged++;
            }
            else if (g_mkdir_with_parents(path, 0777) < 0)
            {
                fprintf(stderr, "Cannot create folder: %s\n", path);
                errors++;
            }
            else
            {
                fprintf(stderr, "Created folder: %s\n", path);
                updated++;
            }
        }
        else if (sscanf(line, "chmod %o \"%[^\"]\" && touch -d@%ld.%ld \"%[^\"]\"",
                        &mode, path, &ts, &tsfrac, path2) == 5)
        {
            if (strcmp(path, path2) != 0)
            {
                fprintf(stderr, "Invalid line (path mismatch): %s\n", line);
                errors++;
            }
            else if (lstat(path, &st) < 0)
            {
                fprintf(stderr, "Not found: %s\n", path);
                errors++;
            }
            else if (!S_ISREG(st.st_mode))
            {
                fprintf(stderr, "Not a regular file: %s\n", path);
                errors++;
            }
            else
            {
                if ((st.st_mode & 0777) == mode)
                {
                    unchanged++;
                }
                else if (chmod(path, mode) < 0)
                {
                    fprintf(stderr, "Cannot set mode (%o -> %o): %s\n",
                            st.st_mode & 0777, mode, path);
                    errors++;
                }
                else
                {
                    fprintf(stderr, "Set mode (%o -> %o): %s\n",
                            st.st_mode & 0777, mode, path);
                    updated++;
                }

                if (st.st_mtime == ts)
                {
                    unchanged++;
                }
                else if (utime(path, &(struct utimbuf){ts, ts}) < 0)
                {
                    fprintf(stderr, "Cannot set timestamp: %s\n", path);
                    errors++;
                }
                else
                {
                    fprintf(stderr, "Set timestamp: %s\n", path);
                    updated++;
                }
            }
        }
        else
        {
            fprintf(stderr, "Invalid line: %s\n", line);
            continue;
        }

        fprintf(stderr, "%d unchanged, %d updated\r", unchanged, updated);
        fflush(stderr);
    }

    fprintf(stderr, "\n");
    fclose(in);
    return 0;
}
