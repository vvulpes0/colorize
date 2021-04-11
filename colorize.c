#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

#include <netpbm/ppm.h>

void usage(void);

int
main(int argc, char* argv[])
{
	FILE* pal_file  = NULL; /* pal:  palettes, one per line     */
	FILE* map_file  = NULL; /* map:  which palette to use where */
	/* base_file is stdin.     base: image of palette indices   */
	pixel** pal;
	pixel* map_row  = NULL;
	pixel* base_row = NULL;
	pixval pal_maxval;
	pixval map_maxval;
	pixval base_maxval;
	int pal_rows;
	int pal_cols;
	int map_cols;
	int map_rows;
	int base_cols;
	int base_rows;
	int map_format;
	int base_format;
	int row_div = 1;
	int col_div = 1;
	int c;
	int i;
	int j;
	pixel p;

	while ((c = getopt(argc, argv, "m:p:h?")) != -1)
	{
		switch (c)
		{
		case 'm':
			if ((map_file = fopen(optarg, "rb")) == NULL)
			{
				fprintf(stderr,
				        "Failed to open map: %s. %s\n",
				        optarg,
				        strerror(errno));
				return 1;
			}
			break;
		case 'p':
			if ((pal_file = fopen(optarg, "rb")) == NULL)
			{
				fprintf(stderr,
				        "Failed to open palettes: %s. %s\n",
				        optarg,
				        strerror(errno));
				return 1;
			}
			break;
		case 'h':
		case '?':
		default:
			usage();
			return !(c == 'h' || c == '?');
			break; /* will never be reached*/
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > 1 || pal_file == NULL)
	{
		usage();
		return 1;
	}
	if (argc && strcmp(argv[0],"-") == 0) {
		--argc;
		++argv;
	}
	if (freopen(argc ? argv[0] : NULL, "rb", stdin) == NULL)
	{
		fprintf(stderr,
		        "Failed to open base%s%s. %s\n",
		        argc ? ": " : "",
		        argc ? argv[0] : "",
		        strerror(errno));
		return 1;
	}

	pm_init(argv[0], 0);
	pal = ppm_readppm(pal_file, &pal_cols, &pal_rows, &pal_maxval);
	ppm_readppminit(stdin, &base_cols, &base_rows,
	                &base_maxval, &base_format);
	if (map_file != NULL)
	{
		ppm_readppminit(map_file, &map_cols, &map_rows,
		                &map_maxval, &map_format);
		col_div = (base_cols + map_cols - 1) / map_cols;
		col_div = col_div < 1 ? 1 : col_div;
		row_div = (base_rows + map_rows - 1) / map_rows;
		row_div = row_div < 1 ? 1 : row_div;
	}

	ppm_writeppminit(stdout, base_cols, base_rows, pal_maxval, 0);
	base_row = malloc(base_cols * sizeof(pixel));
	if (map_file)
	{
		map_row = malloc(map_cols * sizeof(pixel));
	}
	for (i = 0; i < base_rows; ++i)
	{
		ppm_readppmrow(stdin, base_row, base_cols,
		               base_maxval, base_format);
		if (map_file && (i % row_div == 0))
		{
			ppm_readppmrow(map_file, map_row, map_cols,
			               map_maxval, map_format);
		}
		for (j = 0; j < base_cols; ++j)
		{
			c = map_row ? PPM_GETR(map_row[j / col_div]) : 0;
			/* Just use black if out of range */
			PPM_ASSIGN(p, 0, 0, 0);
			if (c < pal_rows && PPM_GETR(base_row[j]) < pal_cols)
			{
				p = pal[c][PPM_GETR(base_row[j])];
			}
			PPM_ASSIGN( base_row[j]
			          , PPM_GETR(p)
			          , PPM_GETG(p)
			          , PPM_GETB(p)
			          );
		}
		ppm_writeppmrow(stdout, base_row, base_cols, pal_maxval, 0);
	}
	if (map_file)
	{
		free(map_row);
	}
	free(base_row);

	return 0;
}

void
usage(void)
{
	fputs("usage: colorize [-h?] -p palettes [-m map] [file]\n", stderr);
	fputs("\th: show this help\n", stderr);
	fputs("\t?: show this help\n", stderr);
	fputs("\tp: PPM of palettes, one per row\n", stderr);
	fputs("\tm: PGM of indices, which palette goes where\n", stderr);
	fputs("replace palette indices in the PGM input\n", stderr);
}
