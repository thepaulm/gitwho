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
	char *po = strnstr(buf, "origin", ret);
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

	char endchr = ':';
	char *ps = strchr(buf, '@');
	if (ps) {
		ps ++;
	} else {
		ps = strstr(buf, "://");
		ps += 3;
		endchr = '/';
	}
	memmove(buf, ps, ret - (ps - buf) + 1);
	return strchr(buf, endchr) - buf;
}

int set_github_email()
{
	system(GITHUB_GITCONFIG);
	return 0;
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
	int isgithub = (int)(strnstr(buf, "github.com", cnt) != NULL);
	if (isgithub) {
		printf("[github]");
		ret = set_github_email();
	} else {
		printf("[gitolite]");
	}
exit:
	return ret;
}
