#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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
	float	width;
	float	height;
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

int	check_zone(t_zone *zone)
{
	return (zone->width > 0 && zone->width <= 300
			&& zone->height > 0 && zone->height <= 300);
}

char	*get_zone(FILE *file, t_zone *zone)
{
	int scan;

	scan = fscanf(file, "%d %d %c\n", &zone->width, &zone->height, &zone->back);
	if (scan != 3)
		return (0);
	if (!check_zone(zone))
		return (0);

	char *data;
	int len;
	len = zone->width * zone->height;
	if ( !(data = calloc(len + 1, sizeof(char))) )
		return (0);
	while (len--)
		data[len] = zone->back;
	return (data);
}

int	check_shape(t_shape *sh)
{
	return (sh->width > 0 && sh->height > 0
			&& (sh->type == 'r' || sh->type == 'R'));
}

int	in_rectangle(float x, float y, t_shape *sh)
{
	float gap = 1.00000000;

	if (x < sh->x || x > (sh->x + sh->width)
		|| y < sh->y || y > (sh->y + sh->height))
		return (0);
	/* 여기를 통과하면 x >= sh->x && x <= (sh->x + sh->width)
		y >= sh->y && y <= (sh->y + sh->height) 의 조건을 충족하는 x,y값이다 */

	// 여기는 테두리를 재는 곳이다
	if ( x - sh->x < gap || (sh->x + sh->width) - x < gap 
		|| y - sh->y < gap || (sh->y + sh->height) - y < gap )
		return (2);
	return (1);
}

void	draw_shape(char *data, t_shape *sh, t_zone *zone)
{
	int	x = 0;
	int	y = 0;
	int ret = 0;

	while (y < zone->height)
	{
		x = 0;
		while(x < zone->width)
		{
			ret = in_rectangle( (float)x, (float)y, sh);
			//printf("x:%d y:%d ret:%d\n", x, y, ret);
			if ( (sh->type == 'r' && ret == 2) || (sh->type == 'R' && ret) )
			{
				//printf("x:%d y:%d\n", x, y);
				data[y * zone->width + x] = sh->text;
			}
			x++;
		}
		y++;
	}
}

int	draw_shapes(FILE *file, char *data, t_zone *zone)
{
	t_shape sh;
	int		scan;

	while (1)
	{
		scan = fscanf(file, "%c %f %f %f %f %c\n", 
						&sh.type, &sh.x, &sh.y, &sh.width, &sh.height, &sh.text);
		if (scan != 6 && scan != -1)
			return (0);
		if (scan == -1)
			break ;
		if (!check_shape(&sh))
			return (0);
		draw_shape(data, &sh, zone);
	}
	return (1);
}

void	print_data(char *data, t_zone *zone)
{
	int	i = 0;

	while (i++ < zone->height)
	{
		write(1, data, zone->width);
		write(1, "\n", 1);
		data += zone->width;
	}	
}


int main(int argc, char **argv)
{
	t_zone	zone;
	FILE	*file;
	char	*data;
	char	msg1[] = "Error: argument\n";
	char	msg2[] = "Error: Operation file corrupted\n";


	if (argc != 2)
		return (clear(0, 0, msg1));
	
	if ( !(file = fopen(argv[1], "r")) )
		return ( clear(0,0, msg2) );
	
	if ( !(data = get_zone(file, &zone)) )
		return ( clear(file, 0, msg2) );
	
	if (!draw_shapes(file, data, &zone))	
		return ( clear(file, data, msg2) );
	
	print_data(data, &zone);
	clear(file, data, 0);
	//system("leaks my | grep leaked");
	return (0);
}
