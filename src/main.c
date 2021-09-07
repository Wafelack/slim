#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int
parse_dur(const char *s)
{
	char final;
	size_t len;
	int mult;
	int res;
	char *cp;

	mult = 1;
	len = strlen(s);
	final = s[len - 1];
	if (!isdigit(final)) {
		switch (final) {
		case 's':
			break;
		case 'm':
			mult = 60;
			break;
		case 'h':
			mult = 3600;
			break;
		case 'd':
			mult = 86400;
			break;
		default:
			fprintf(stderr, "Invalid multiplier: %c.\n", final);
			return -1;
		}
	}
	len = isdigit(final) ? len : len - 1;
	cp = malloc(len);
	if (cp == NULL)
		return -1;
	strncpy(cp, s, len);
	res = atoi(s);
	if (res == 0) {
		free(cp);
		fprintf(stderr, "Invalid duration: %s.\n", s);
		return -1;
	}
	free(cp);
	return res * mult;
}
void
print_bar(int elapsed, int duration)
{
	int remaining;
	int scaled;
	
	scaled = floor((float)elapsed / (float)duration * 50);
	remaining = 50 - scaled;
	printf("\r[");
	for (; scaled >= 0; --scaled)
		putchar('#');
	for (; remaining >= 0; --remaining)
		putchar('-');
	printf("]\t%d/%d (%.0f%%)", elapsed, duration, (float)elapsed / (float)duration * 100);
	fflush(stdout);
}

int
main(int argc, char * const argv[])
{
	int progress = 0, inform = 0, err = 0, duration = -1;
	char c;

	while (c = getopt(argc, argv, ":pi:"), c != -1) {
		switch (c) {
		case 'p':
			progress++;
			break;
		case 'i':
			inform = parse_dur(optarg);
			break;
		case ':':
			fprintf(stderr, "Option '-%c' requires an operand.\n", optopt);
			err++;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option: '-%c'.\n", optopt);
			err++;
			break;
		}
	}

	if (err || argc - optind != 1) {
		fprintf(stderr, "Usage: %s [-p][-i duration]<duration>.\n", argv[0]);
		return EXIT_FAILURE;
	}

	duration = parse_dur(argv[optind]);
	if (duration == -1 || inform == -1)
		return EXIT_FAILURE;

	if (!progress && !inform) {
		return sleep(duration);
	} else if (progress) {
		int elapsed;

		for (elapsed = 0; elapsed <= duration; elapsed++) {
			print_bar(elapsed, duration);
			if (sleep(1) != 0)
				return EXIT_FAILURE;
		}
		puts("");
		return EXIT_SUCCESS;
	} else {
		if (sleep(duration - inform) != 0)
			return EXIT_FAILURE;
		printf("%d seconds remaining...\n", inform);
		return sleep(inform);
	}
}
