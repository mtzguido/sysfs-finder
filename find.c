#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define ROOT	"/sys/class/ubi"
#define ATTR	"name"

int check_dir(char *name, char *attr, char *val)
{
	struct stat st;
	char filename[NAME_MAX+1];
	char buf[100]; /* limit ok? */
	int fd, t;

	sprintf(filename, "%s/%s", ROOT, name);
	stat(filename, &st);

	/* Ignore non-directories */
	if (!S_ISDIR(st.st_mode))
		return 0;

	strcat(filename, "/");
	strcat(filename, attr);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		/* Just ignore if the file didn't exist */
		if (errno != ENOENT)
			perror("open");

		return 0;
	}

	t = read(fd, buf, sizeof buf);
	close(fd);
	if (t < 0) {
		perror("read");
		return 0;
	}

	buf[t] = 0;
	if (buf[t-1] == '\n')
		buf[t-1] = 0;

	return strcmp(buf, val) == 0;
}

int main(int argc, char **argv)
{
	DIR *root;
	struct dirent *de;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <name>\n", argv[0]);
		fprintf(stderr, "       finds all ubi volumes with name equal to <name>\n");
		return 1;
	}

	root = opendir(ROOT);
	if (!root) {
		perror("opendir");
		return 1;
	}

	while (errno = 0, de = readdir(root), de) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;

		if (check_dir(de->d_name, ATTR, argv[1])) {
			printf("%s\n", de->d_name);

			/* break; ? */
		}
	}

	if (errno) {
		perror("readdir");
		closedir(root);
		return 1;
	}

	closedir(root);

	return 0;
}
