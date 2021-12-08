#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

typedef struct	s_zone
{
	int		width;
	int		height;
	char	back;
}	t_zone;

typedef struct	s_shape
{
	char	type;
	float	x;
	float	y;
	float	radius;
	char	text;
}	t_shape;

int	clear(FILE *file, char *data, char *msg)
{
	if (file)
		fclose(file);
	if (data)
		free(data);
	int i = 0;
	if (msg)
	{
		while (msg[i])
			i++;
		write(1, msg, i);
	}
	return (1);
}

char	*get_zone(FILE *file, t_zone *zone)
{
	int		scan;
	char	*data;
	int		i;

	scan = fscanf(file, "%d %d %c\n", &zone->width, &zone->height, &zone->back);
	if (scan != 3)
		return (0);
	if (!(zone->width > 0 && zone->width <= 300
		&& zone->height > 0 && zone->height <= 300))
		return (0);
	data = (char *)calloc(zone->width * zone->height, sizeof(char));
	if (!data)
		return (0);
	i = 0;
	while (i < zone->width * zone->height)
		data[i++] = zone->back;
	return (data);
}

int	in_circle(float x, float y, t_shape *sh)
{
	float	distance;

	// 좌표평면에서 두 점사이의 거리 구하기
	distance = sqrtf(powf(x - sh->x, 2.) + powf(y - sh->y, 2.));
	if (distance <= sh->radius)
	{
		if ((sh->radius - distance) < 1.00000000)
			return (2);
		return (1);
	}
	return (0);
}

void	draw_shape(t_zone *zone, t_shape *sh, char *data)
{
	int	x = 0;
	int	y = 0;
	int	ret;

	while (y < zone->height)
	{
		x = 0;
		while (x < zone->width)
		{
			ret = in_circle((float)x, (float)y, sh);
			//printf("x:%d y:%d ret:%d\n", x, y, ret);
			if ((sh->type == 'c' && ret == 2) 
				|| (sh->type == 'C' && ret != 0))
				{
					//printf("x:%d y:%d ret:%d\n", x, y, ret);
					data[y * zone->width + x] = sh->text;
				}
			x++;
		}
		y++;
	}
}

int	draw_shapes(FILE *file, t_zone *zone, char *data)
{
	t_shape	sh;
	int		scan;

	while (1)
	{
		scan = fscanf(file, "%c %f %f %f %c\n", 
						&sh.type, &sh.x, &sh.y, &sh.radius, &sh.text);
		if (scan != 5 && scan != -1)
			return (0);
		if (scan == -1)
			break ;
		if (sh.radius <= 0.00000000 || (sh.type != 'c' && sh.type != 'C'))
			return (0);
		//printf("%c %f %f %f %c\n", sh.type, sh.x, sh.y, sh.radius, sh.text);
		draw_shape(zone, &sh, data);
	}
	return (1);
}

void	print_data(t_zone *zone, char *data)
{
	int	i = 0;
	while (i++ < zone->height)
	{
		write(1, data, zone->width);
		write(1, "\n", 1);
		data += zone->width;
	}
}

int	main(int argc, char **argv)
{
	FILE	*file;
	char	*data;
	t_zone	zone;
	char	msg1[] = "Error: argument\n";
	char	msg2[] = "Error: Operation file corrupted\n";

	if (argc != 2)
		return (clear(0, 0, msg1));

	if ( !(file = fopen(argv[1], "r")) )
		return (clear(0, 0, msg2));

	if ( !(data = get_zone(file, &zone)) )
		return (clear(file, 0, msg2));
	
	if ( !(draw_shapes(file, &zone, data)) )
		return (clear(file, data, msg2));
	
	print_data(&zone, data);
	clear(file, data, 0);
	return(0);
}

