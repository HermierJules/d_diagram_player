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
int enum_popcount_byte[256];
	problem p = {
		0x00000500000000,
		0x00000000500,
		{ 3,2,5,3,4,1,4,4 },
		{ 1,4,2,7,0,4,4,4 }
	};
board b = 3;
board map_to_board(map m){
	uint64_t pow = 1;
	board b = 0x8000000000000000;
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

void draw_board(problem* p, board b, sfSprite* wall, sfSprite* monster, sfSprite* treasure, sfSprite* empty, sfRenderWindow* window){
	sfVector2f pos;
	pos.x = 96;
	pos.y = 96;
    for(int i = 0; i <= 64; i++)
    {
		
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
		if(i%8 == 0 && i!= 0) {
			pos.y = pos.y + 16*6;
			pos.x = 96;
		}
		else pos.x = pos.x + 16*6;
	}
}
	
	

int main(){
    //16*16 tiles scaled by 6, 9*9 grid
	sfVideoMode mode = {864, 864, 220};
	sfRenderWindow* window;
	sfTexture* texture_wall;
	sfTexture* texture_treasure;
	sfTexture* texture_monster;
	sfTexture* texture_empty;
	sfSprite* wall;
	sfSprite* empty;
	sfSprite* treasure;
	sfSprite* monster;
	sfVector2f scale;
	sfEvent event;
	scale.x = 6;
	scale.y = 6;
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
	//creating the window
	window = sfRenderWindow_create(mode, "dungeons and diagrams", sfResize | sfClose, NULL);
	while(sfRenderWindow_isOpen(window)){
		while(sfRenderWindow_pollEvent(window, &event)){
			/*close window: exit*/
			if(event.type == sfEvtClosed)
				sfRenderWindow_close(window);
		}
		sfRenderWindow_clear(window, sfBlack);
		draw_board(&p, b, wall, monster, treasure, empty, window);
	sfRenderWindow_display(window);
	}
	return 0;
}
