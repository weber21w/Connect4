/*
 *  Dr.Mario port for Uzebox ver 1.1
 *  Copyright (C) 2009  Codecrank, Mapes
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/patches.inc"
#include "data/title.inc"
#include "data/font.inc"
#include "data/tiles.inc"
#include "data/frames.inc"
//#include "uzenet.h"

extern void Uzenet_Update();
extern volatile u16 uzenet_state;
extern volatile u8 uzenet_error;

uint16_t pads[2],oldPads[2];

#define BOARD_WIDTH_STD		7//size of original game board...
#define BOARD_HEIGHT_STD	6

#define BOARD_WIDTH_EXT		8//for whatever reason, the existing ports use a larger board...
#define BOARD_HEIGHT_EXT	7

uint8_t board[BOARD_WIDTH_EXT*BOARD_HEIGHT_EXT];
uint8_t board_width,board_height;
uint8_t wins[2];
uint8_t net_role;
uint8_t frame;

#define NET_ROLE_DISABLED	0
#define NET_ROLE_PLAYER1	1
#define NET_ROLE_PLAYER2	2
#define NET_ROLE_SPECTATOR	3

#include "ai.h"

char pname[2][13+1];
uint8_t chat_column,chat_row;
uint8_t mode, turn, winner, move;


void Intro(){
//return;
	SetTileTable(tile_data);
	ClearVram();
	char ypos[10],ydelay[10];
	for(uint8_t i=0;i<10;i++){
		ydelay[i] = GetPrngNumber(0)%60;
		ypos[i] = 0;
	}
	
	for(uint8_t i=0;i<90;i++){
		WaitVsync(1);
		ClearVram();
		Print(15,13,PSTR("U Z E B O X"));
		for(uint8_t j=0;j<10;j++){
			if(ydelay[j])
				ydelay[j]--;
			else if(ypos[j] < SCREEN_TILES_V-3)
				ypos[j] ++;
			DrawMap(j*4,ypos[j],pgm_read_word(&piece_frames[2+(j&1)]));
		}
	}
	for(uint8_t i=0;i<10;i++)
		DrawMap(i*4,SCREEN_TILES_V-3,pgm_read_word(&piece_frames[5+(i&1)]));
	for(uint8_t i=SCREEN_TILES_V-4;i>=0;i--){
		for(uint8_t j=0;j<SCREEN_TILES_H;j++)
			SetTile(j,i,1);
		for(uint8_t j=0;j<SCREEN_TILES_H;j++){
			for(uint8_t k=11;k<18;k++)
			SetTile(j,k,0);
		}
		//if(i <= 13){
			Print(15,13,PSTR("U Z E B O X"));

		if(i <= 17){
			uint8_t detected = 0;
			if(uzenet_state & 1){
				Print(11,15,PSTR("+ESP8266 DETECTED+"));
				detected++;
			}
			if(uzenet_state & 4){
				Print(11,15+detected,PSTR("+SPI RAM DETECTED+"));
				detected++;
			}
		}
		if(i == 0)
			break;
		WaitVsync(1);
	}
	WaitVsync(60);
	FadeOut(4,true);
	FadeIn(3,false);
}

void VsyncRoutine(){
	frame++;
	for(uint8_t p=0;p<2;p++){
		oldPads[p] = pads[p];
		pads[p] = ReadJoypad(p);
	}
	Uzenet_Update();
}

uint8_t UzenetScreen(){
	SetTileTable(title_data);
	ClearVram();
	DrawMap(0,0,title_map);

	if(!(uzenet_state & 4) || !(uzenet_state & 1) || !(uzenet_state & 8)){
		if(!(uzenet_state & 4)){
			Print(6,9,PSTR("SPI RAM NOT DETECTED"));
		}
		if(!(uzenet_state & 1)){
			Print(6,10,PSTR("ESP8266 NOT DETECTED"));
		}
		if(!(uzenet_state & 8)){
			Print(6,11,PSTR("UZEPASS NOT DETECTED"));
		}

		Print(6,13,PSTR("ONLINE PLAY DISABLED"));
		Print(6,15,PSTR("VERIFY UZENET MODULE"));
		Print(6,16,PSTR("AND RUN SETUP UTILITY"));
		Print(5,19,PSTR("PRESS START TO CONTINUE"));
		uzenet_error |= 128;//don't keep trying
		TriggerFx(6,0xff,1);
	}

//	if(1)
//		Print(0,20,PSTR(" Uzenet Module Detected"));
	Print(0,21,PSTR(" Connecting to Uzenet..."));
	while(1){
		WaitVsync(1);
	}
	return 1;
}

uint8_t TitleScreen(){
	SetTileTable(title_data);
	ClearVram();
//	for(uint8_t i=0;i<SCREEN_TILES_H;i++){SetTile(i,SCREEN_TILES_V,0);SetTile(i,SCREEN_TILES_V+1,0);} //hmm...
	DrawMap(0,0,title_map);
	Print(6,21,PSTR("                  "));
	Print(6,22,PSTR("  1 Player Game   "));
	Print(6,23,PSTR("  2 Player Game   "));
	Print(6,24,PSTR("  Uzenet          "));
	if(!(uzenet_state & 1))
		Print(14,24,PSTR("-DISABLED"));
	Print(6,25,PSTR("                  "));
	uint8_t cursor = 0;
	while(1){
		WaitVsync(1);
		GetPrngNumber(0);
//pads[0] = BTN_START;
		if(pads[0] & BTN_START && !(oldPads[0] & BTN_START))
			break;
		if(pads[0] & BTN_UP && !(oldPads[0] & BTN_UP) && cursor)
			cursor--;
		if(pads[0] & BTN_DOWN  && !(oldPads[0] & BTN_DOWN) && cursor < 2)
			cursor++;
		if(!(uzenet_state & 1) && cursor == 2)
			cursor = 1;

		for(uint8_t i=22;i<25;i++)
			PrintChar(6,i,' ');
		PrintChar(6,22+cursor,'>');
	}
	TriggerFx(12,255,1);
	return cursor;
}

uint8_t PrintByteVertical(uint8_t x, uint8_t y, uint8_t b){

	for(uint8_t i=0;i<2;i++){
		uint8_t c=b%10;
		if(b>0 || i==0){
			SetFont(x,y--,c+'0'-32);
		}else{
				SetFont(x,y--,'0'-32);

		}
		b=b/10;
	}
}

uint8_t PrintRamVertical(uint8_t x, uint8_t y, unsigned char *string){
	uint8_t i=0;

	while(1){
		char c = string[i++];		
		if(c!=0){
			c=((c&127)-32);			
			SetFont(x,y++,c);
		}else
			return y;
	}
	
}

uint8_t PrintRamHorizontal(uint8_t x, uint8_t y, unsigned char *string){
	uint8_t i=0;

	while(1){
		char c = string[i++];		
		if(c!=0){
			c=((c&127)-32);			
			SetFont(x++,y,c);
		}else
			return x;
	}
	
}

uint8_t PrintVertical(uint8_t x, uint8_t y, const char *string){
	uint8_t i=0;

	while(1){
		char c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			c=((c&127)-32);			
			SetFont(x,y++,c);
		}else{
			return y;
		}
	}
	
}

void DrawWins(){
	PrintRamVertical(1,3,pname[0]);
	PrintVertical(1,17,PSTR("WINS"));
	PrintByteVertical(1,23,wins[0]);
	PrintRamVertical(SCREEN_TILES_H-2,3,pname[1]);
	PrintVertical(SCREEN_TILES_H-2,17,PSTR("WINS"));
	PrintByteVertical(SCREEN_TILES_H-2,23,wins[1]);
}

void DrawBoard(){
	SetTileTable(tile_data);
	ClearVram();
	if(board_width == BOARD_WIDTH_EXT)
		DrawMap(1,0,board_map_ext);
	else
		DrawMap(1,0,board_map_std);

	DrawWins();
}


uint8_t JoinGame(){
	return 0;
}

uint8_t HostGame(){
	return 0;
}

void ChatPrint(const char *string){
	uint8_t i=0;

	while(1){
		char c = pgm_read_byte(&(string[i++]));		
		if(c != 0){
			c=((c&127)-32);			
			SetFont(chat_column++,chat_row,c);
		}else{
			break;
		}
	}
	
}


void ClearBoard(){
	for(int16_t i=0;i<BOARD_WIDTH_EXT*BOARD_HEIGHT_EXT;i++)
		board[i] = 0;
}

uint8_t GetBoard(uint8_t column, uint8_t row){
	return board[(row*BOARD_WIDTH_EXT)+column];
}

void RedrawPosition(uint8_t column, uint8_t row){
	uint8_t color = GetBoard(column,row);
//	PrintInt(10,10,color,1);
	if(board_width == BOARD_WIDTH_EXT){//extended size(compressed spacing)
	
	//	DrawMap(2+(column*4),0+(row*3),pgm_read_word(&piece_frames[3+color]));
	}else{//standard size board(full spacing)
	
		DrawMap(3+(column*5),1+(row*4),pgm_read_word(&piece_frames[4+color]));
	}
}

void SetBoard(uint8_t column, uint8_t row, uint8_t p){
	board[(row*BOARD_WIDTH_EXT)+column] = p;
	RedrawPosition(column,row);
}

uint8_t DropPiece(uint8_t p, uint8_t column){

	if(column >= BOARD_WIDTH_EXT)
		column = BOARD_WIDTH_EXT-1;

  	uint8_t row = 0;
  	p++;//0 is blank, 1 is P1, 2 is P2

	while(!GetBoard(column,row)){//animate dropping piece until we hit something below
		WaitVsync(2);

	  	if(row == 0){
	  //		TriggerFx(1,255,1);
			
		}else
	  		SetBoard(column,row-1,0);//clear previous position(this will automatically redraw)

		SetBoard(column,row++,p);
		if(row == board_height)
			break;
	}
	if(row == 0){//failed on first move?
//		TriggerFx(4,255,1);
		return 254;//try again..
	}
	row--;
//	TriggerFx(2,255,1);


	//Check for winning:
	uint8_t run = 0;
	for(uint8_t x=0;x<board_width;x++){//check horizontal
		if(GetBoard(x,row) != p)
			run = 0;
		else if(++run > 3)
			return p-1;
	}

	run = 0;
	for(uint8_t y=0;y<board_height;y++){//check vertical
		if(GetBoard(column,y) != p)
			run = 0;
		else if(++run > 3)
			return p-1;
	}

	uint8_t t1 = column;
	uint8_t t2 = row;
	while(t1 && t2){//get top left
		t1--;
		t2--;
	}

	run = 0;
	while(1){//scan from top left to bottom right

		if(GetBoard(t1++,t2++) != p)
			run = 0;
		else if(++run > 3)
			return p-1;
		if(t1 == board_width || t2 == board_height)
			break;
	}

	while(column < board_width-1 && row){//get top right
		column++;
		row--;
	}

	run = 0;
	while(1){//scan from top right to bottom left

		if(GetBoard(column--,row++) != p)
			run = 0;
		else if(++run > 3)
			return p-1;
		if(column == board_width || row == board_height)
			break;
	}
  
	return 255;//no victory, no error
}

const char player_str[] PROGMEM = "Player1";

int main(){
	SetUserPostVsyncCallback(&VsyncRoutine);
	GetPrngNumber(0xACE1);
	SetFontTable(font_data);
	InitMusicPlayer(patches);
	SetMasterVolume(224);
	Intro();

  	uint8_t cursor=0;//the horizontal position above the board where a piece will drop from, controlled by the active player
PROGRAM_TOP:
	chat_column = chat_row = 0;
	mode = TitleScreen();
	if(mode == 2){
		net_role == UzenetScreen();
		if(!net_role)
			goto PROGRAM_TOP;
		net_role = mode;


		board_width = BOARD_WIDTH_EXT;//be compatible with existing clients using non-standard board size
		board_height = BOARD_HEIGHT_EXT;
	}else{//local games use the standard board size(nicer graphics)
		board_width = BOARD_WIDTH_STD;
		board_height = BOARD_HEIGHT_STD;
		net_role = NET_ROLE_DISABLED;
		for(uint8_t i=0;i<8;i++){
			if(i == 7)
				pname[0][i] = 0;
			else
				pname[0][i] = pgm_read_byte(&player_str[i]);
			pname[1][i] = pname[0][i];
		}
		pname[1][6] = '2';
	}
	wins[0] = wins[1] = 0;
GAME_TOP:
	//play the game
	ClearBoard();
	turn = 0;
	move = 0;
	cursor = 3;
	winner = 255;
	ai_target = 255;
	DrawBoard();//this will also draw player names
MOVE_TOP:
	WaitVsync(1);
	if(move >= board_width*board_height){//draw game(handles standard or extended size)
		winner = 2;
		goto GAME_END;
	}
	for(uint8_t i=0;i<board_width;i++)
		RedrawPosition(i,0);
	
	if(turn){
		if(frame&0b1000){

			if(!board[cursor]){
				SetTile(4+(cursor*5),1,49);
				SetTile(5+(cursor*5),1,50);
			}else{
				SetTile(4+(cursor*5),1,53);
				SetTile(4+(cursor*5),1,54);
			}
		}
		
	}else{
		if(frame&0b1000){

			if(!board[cursor]){
				SetTile(4+(cursor*5),1,47);
				SetTile(5+(cursor*5),1,48);
			}else{
				SetTile(4+(cursor*5),1,51);
				SetTile(5+(cursor*5),1,52);
			}
		}
	}
if(turn){
	if(ai_target < 255){
	//	PrintByte(30,25,ai_target,1);
		if(cursor > ai_target)
			cursor--;
		else if(cursor < ai_target)
			cursor++;
		else{
			pads[0] = BTN_B;oldPads[0] = 0;
			ai_target = 255;	
		}
	}else{
		ai_target = AIThink();
	//	if(board[ai_target])
	//		ai_target = 3;
	//		db();
//		AIDrawSim();
	//	PrintByte(30,25,ai_target,1);

//		for(uint8_t l=0;l<board_width;l++)
//			PrintByte(5+(l*5),24,weights[l],1);
//		goto MOVE_TOP;
	//	continue;
	}	
}
	if(cursor && (pads[0] & BTN_LEFT) && !(oldPads[0] & BTN_LEFT))
		cursor--;
	if(cursor < board_width-1 && (pads[0] & BTN_RIGHT) && !(oldPads[0] & BTN_RIGHT))
		cursor++;

	if(((!(pads[0] & BTN_B)) || (oldPads[0] & BTN_B)))// || (!(pads[turn] & BTN_Y) || (oldPads[turn] & BTN_Y)))
		goto MOVE_TOP;

	winner = DropPiece(turn,cursor);

//PrintByte(20,21,move,1);	
	if(winner == 254)
		goto MOVE_TOP;//invalid move, try again
	if(winner < 3)
		goto GAME_END;
//MOVE_END:
	turn = !turn;//otherwise, next player's turn
	Print(3,25,turn?PSTR("P2's Turn"):PSTR("P1's Turn"));
	move++;
	ai_target = 255;	
	goto MOVE_TOP;

GAME_END:
	if(winner == 2){//draw
		Print(10,25,PSTR("Draw Game"));
	}else{
		wins[winner]++;
		PrintRam(10,25,pname[winner]);
		uint8_t x;
		for(x=0;x<13;x++){
			if(pname[winner][x] == '\0')
				break;
		}
		Print(11+x,25,PSTR("Wins!"));
	}

	while(1){//wait for player input to continue or quit
		WaitVsync(1);
		if(1){//joypad
			Print(3,25,PSTR("[start] to play again, (B) to quit"));
		}else{//keyboard
			Print(3,25,PSTR("[Enter] to play again, [Esc] to quit"));		
		}
		for(uint8_t i=0;i<2;i++){
			if(pads[i] & BTN_START && !(oldPads[i] & BTN_START)){
				FadeOut(2,true);
				FadeIn(3,false);
				goto GAME_TOP;
			}
		}
	}
	goto PROGRAM_TOP;
}
