#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <SFML/Audio.h>
#include <SFML/Graphics.h>

typedef bool map[64];
typedef uint64_t board;
typedef char constraint[8];
struct problem {
board monsters;
board treasures;
constraint rows;
constraint columns;
};
typedef struct problem problem;

map monsters = {
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 1,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 1,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 1,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 1,
};
map treasures = {
0, 0, 0, 0, 0, 0, 1, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
};
int enum_popcount_byte[256];
problem p = {
		8000800080048000,
		0x0000000000,
		{ 3,2,5,3,4,1,4,4 },
		{ 1,4,2,7,0,4,4,4 }
	};
board b = 5;
board map_to_board(map m){
	uint64_t pow = 1;
	board b = 0x00000000000000;
	for(int i = 0; i < 64; i ++)
	{
		b = b + pow * m[i];
		pow = pow * 2;
	}
	return b;
}

void addbin(int n, int pos, bool m[64]){
	int b = pos;
	while(n != 0){
		if(n%2){
			m[b] = 1;
		}
		b = b +1;
		n = n / 2;
	}
}	
		
void board_to_map(uint64_t b, map m){
	int pos = 0;
	int digit;
	while(b!= 0){
		digit = b % 16;
		b = b/16;
		addbin(digit, pos, m);
		pos = pos + 4;
	}
}

int popcount(uint64_t x){
uint64_t m1 = 0x55; // 0b01010101
uint64_t m2 = 0x33; // 0b00110011
uint64_t m3 = 0x0f; // 0b00001111
x = (x&m1) + ((x&(m1<<1))>>1);
x = (x&m2) + ((x&(m2<<2))>>2);
x = (x&m3) + ((x&(m3<<4))>>4);
return x;	
}

void draw_board(problem* p, board b, sfSprite* wall, sfSprite* monster, sfSprite* treasure, sfSprite* empty, sfFont* font,sfRenderWindow* window){
	char str[96];
	sfVector2f pos;
	sfVector2f postext;
	postext.x = 0;
	postext.y = 0;
	sfText* text;
	text = sfText_create();
	sfText_setColor(text, sfWhite);
	sfText_setFont(text,font);
	sfText_setCharacterSize(text,96);
	sprintf(str," %d%d%d%d%d%d%d%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d",p->rows[0], p->rows[1], p->rows[2], p->rows[3], p->rows[4], p->rows[5], p->rows[6], p->rows[7], p->columns[0], p->columns[1], p->columns[2], p->columns[3], p->columns[4], p->columns[5], p->columns[6], p->columns[7]);
	sfText_setPosition(text,postext);
	sfText_setString(text,str);
	sfRenderWindow_drawText(window,text,NULL);
	pos.x = 0;
	pos.y = 96;
    for(int i = 0; i < 64; i++)
    {
		if(i%8 == 0 && i!= 0) {
			pos.y = pos.y + 16*6;
			pos.x = 96;
		}
		else pos.x = pos.x + 16*6;
			sfSprite_setPosition(empty,pos);
			sfRenderWindow_drawSprite(window, empty, NULL);
        if (((p->monsters >> i) & 1) != 0){
			sfSprite_setPosition(monster, pos);
			sfRenderWindow_drawSprite(window, monster, NULL);
        }
		else if (((p->treasures >> i) & 1) != 0){
			sfSprite_setPosition(treasure, pos);
			sfRenderWindow_drawSprite(window, treasure, NULL);
        }
		else if (((b >> i) & 1) != 0){
			sfSprite_setPosition(wall,pos);
            sfRenderWindow_drawSprite(window, wall, NULL);
		}
	}
	sfText_destroy(text);
}
	

int main(){
p.monsters = map_to_board(monsters);
p.treasures = map_to_board(treasures);	
    //16*16 tiles scaled by 6, 9*9 grid
	sfVideoMode mode = {864, 864, 220};
	sfRenderWindow* window;
	sfTexture* texture_wall;
	sfTexture* texture_treasure;
	sfTexture* texture_monster;
	sfTexture* texture_empty;
	sfSprite* wall;
	sfSprite* empty;
	sfFont* font;
	sfSprite* treasure;
	sfSprite* monster;
	sfVector2f scale;
	sfEvent event;
	scale.x = 6;
	scale.y = 6;
	map walls;
	board_to_map(b, walls);
	//loading the textures
		texture_wall = sfTexture_createFromFile("./ressources/wall.png", NULL);
		texture_treasure = sfTexture_createFromFile("./ressources/treasure.png", NULL);	
	texture_monster = sfTexture_createFromFile("./ressources/monster.png", NULL);
	texture_empty = sfTexture_createFromFile("./ressources/empty.png", NULL);
	//creating the sprites
	wall = sfSprite_create();
	treasure =sfSprite_create();
	empty = sfSprite_create();
	monster =sfSprite_create();
	//setting the texture
	sfSprite_setTexture(wall,texture_wall,sfTrue);
	sfSprite_setTexture(treasure, texture_treasure, sfTrue);
	sfSprite_setTexture(empty, texture_empty, sfTrue);
	sfSprite_setTexture(monster,texture_monster, sfTrue);
	//setting the scale
	sfSprite_setScale(monster,scale);
	sfSprite_setScale(empty,scale);
	sfSprite_setScale(treasure,scale);
	sfSprite_setScale(wall,scale);
	//destroying everything
	//creating the font
	font = sfFont_createFromFile("./ressources/pixel.ttf");
	//creating the window
	window = sfRenderWindow_create(mode, "dungeons and diagrams", sfResize | sfClose, NULL);
	while(sfRenderWindow_isOpen(window)){
		while(sfRenderWindow_pollEvent(window, &event)){
			/*close window: exit*/
			if(event.type == sfEvtClosed)
				sfRenderWindow_close(window);
		}
		sfRenderWindow_clear(window, sfBlack);
		draw_board(&p, b, wall, monster, treasure, empty,font, window);
	sfRenderWindow_display(window);
	}
	return 0;
}
