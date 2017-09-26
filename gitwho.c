#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define GITHUB_GITCONFIG "git config user.email pmikesell@gmail.com"

FILE *runcmd(const char *cmd)
{
	return popen(cmd, "r");
}

char * _strnstr(const char *big, const char *little, size_t len)
{
	int i, j;
	int llen = strlen(little);

	for (i = 0; i <= len - llen; i++) {
		for (j = 0; j < llen; j++) {
			if (big[i+j] != little[j]) {
				if (big[i+j] == '\0')
					return NULL;
				break;
			}
		}
		if (j == llen) {
			return (char*)&big[i];
		}
	}
	return NULL;
}

int run_gitremote(char *buf, int size)
{
	FILE *f = runcmd("git remote -v 2>/dev/null");
	/* some dirs are not git dirs */
	if (f <= 0)
		return 0;
	int tot = 0;
	int cnt = fread(buf, 1, size, f);
	while (cnt > 0 && tot < size) {
		tot += cnt;
		cnt = fread(buf + tot, 1, size - tot, f);
	}
	pclose(f);
	return tot;
}

int gitremote(char *buf, int size)
{
	int ret = run_gitremote(buf, size);
	if (ret <= 0)
		return 0;
	char *po = _strnstr(buf, "origin", ret);
	if (!po)
		return -1;
	/* get to next string */
	while (!isspace(*po)) po++;
	while (isspace(*po)) po++;
	char *pend = po;
	while (!isspace(*pend)) pend++;
	memmove(buf, po, pend - po);
	return pend - po;
}

int remotehost(char *buf, int size)
{
	int ret = gitremote(buf, size);
	if (ret <= 0)
		return ret;
	buf[ret] = 0;

	const char endchrs[] = ":/";
	char *ps = strchr(buf, '@');
	if (ps) {
		ps ++;
	} else if (!strncmp("c:", buf, 2)) {
		ps = buf;
	} else {
		ps = strstr(buf, "://");
		ps += 3;
	}
	memmove(buf, ps, ret - (ps - buf) + 1);
	return strpbrk(buf, endchrs) - buf;
}

int set_github_email()
{
	return system(GITHUB_GITCONFIG);
}

#define BUFSIZE 512

int main(int argc, char *argv[])
{
	int ret = 0;
	char buf[BUFSIZE];
	int cnt = remotehost(buf, BUFSIZE);
	if (cnt <= 0) {
		ret = 0;
		goto exit;
	}
	int isgithub = (int)(_strnstr(buf, "github.com", cnt) != NULL);
	if (isgithub) {
		printf("[github]");
		ret = set_github_email();
	} else {
		printf("[gitolite]");
	}
exit:
	return ret;
}
