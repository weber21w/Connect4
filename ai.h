#ifndef AI_H
#define AI_H
uint8_t sim[BOARD_WIDTH_EXT*BOARD_HEIGHT_EXT];//AI simulation buffer
uint8_t weights[BOARD_WIDTH_EXT];
uint8_t levels[BOARD_WIDTH_EXT];
uint8_t ai_target = 0;
void db(){TriggerFx(0,120,1);}

void AIDrawSim(){
	uint16_t goff;
	for(uint8_t i=0;i<board_height;i++){
		goff = i*BOARD_WIDTH_EXT;
		for(uint8_t j=0;j<board_width;j++){
			PrintByte(5+(j*5),3+(i*4),sim[goff++],1);
		}
	}
}

uint8_t AIFindWinningMove(uint8_t color, uint8_t height){//calculates if there is an immediate winning move for a given color, with a specified height of the last dropped piece(returns column to drop in if so)

	if(height < board_height)
		height = board_height;
	uint16_t loff = 0;
	uint8_t t0,t1,t2,t3,count;
	for(uint8_t i=0;i<board_height;i++){//check horizontally for any immediate threats
		loff = i*BOARD_WIDTH_EXT;
		for(uint8_t j=0;j<board_width-3;j++){
//	PrintChar(5+(j*5),1+(i*4),'8');	
	//	WaitVsync(20);
			t0 = sim[loff++];
			t1 = sim[loff++];
			t2 = sim[loff++];
			t3 = sim[loff++];
			count = 0;
			if(t0 == color)
				count++;
			if(t1 == color)
				count++;
			if(t2 == color)
				count++;
			if(t3 == color)
				count++;
			if(count != 3)//at least 3 threats in this section?
				continue;
			count = 0;
			if(!t0)
				count++;
			if(!t1)
				count++;
			if(!t2)
				count++;
			if(!t3)
				count++;
			if(count != 1)//less or more than 1 blank?
				continue;
			//otherwise we found an immediate threat, no need to think further
//PrintByte(2,1,t0,1);PrintByte(2,2,t1,1);PrintByte(2,3,t2,1);PrintByte(2,4,t3,1);PrintByte(2,6,count,1);PrintByte(2,8,loff,1);
			if(!t0){PrintByte(3,0,j+0,1);
				return j+0;
			}else if(!t1){PrintByte(3,0,j+1,1);
				return j+1;
			}else if(!t2){PrintByte(3,0,j+2,1);
				return j+2;
			}else{//PrintByte(3,0,j+3,1);PrintByte(3,1,i,1);
				return j+3;
			}
		}
	}
return 255;
	for(uint8_t i=0;i<board_width;i++){//check vertially for any immediate threats
		if(board[i])
			continue;
		loff = i+((levels[i]-1)*BOARD_WIDTH_EXT);
		for(uint8_t j=0;j<board_height-2;j++){
			uint8_t t0 = sim[loff]; loff += BOARD_WIDTH_EXT;
			uint8_t t1 = sim[loff]; loff += BOARD_WIDTH_EXT;
			uint8_t t2 = sim[loff]; loff += BOARD_WIDTH_EXT;
			uint8_t count = 0;
//		PrintChar((i*5)+4,(j*4)+1,'1');
	//	WaitVsync(20);
			if(t0 && t0 == color)
				count++;
			if(t1 && t1 == color)
				count++;
			if(t2 && t2 == color)
				count++;
			if(count != 3)//no single space that could be filled in this section?
				continue;
PrintByte(3,0,2,1);
			//otherwise we found an immediate threat, no need to think further
		//	db();while(1);
			return i;
		}
	}

	for(uint8_t i=0;i<board_width-3;i++){//check for diagonals
		for(uint8_t j=3;j<board_height;j++){//bottom left to top right
//		PrintChar((i*5)+4,(j*4)+1,'2');
//		WaitVsync(20);
			loff = (j*BOARD_WIDTH_EXT)+i;
			uint8_t t0 = sim[loff]; loff -= (BOARD_WIDTH_EXT-1);
			uint8_t t1 = sim[loff]; loff -= (BOARD_WIDTH_EXT-1);
			uint8_t t2 = sim[loff]; loff -= (BOARD_WIDTH_EXT-1);
			uint8_t t3 = sim[loff]; loff -= (BOARD_WIDTH_EXT-1);
			uint8_t count = 0;
			if(t0 && t0 == color)
				count++;
			if(t1 && t1 == color)
				count++;
			if(t2 && t2 == color)
				count++;
			if(t3 && t3 == color)
				count++;
			if(count != 3)//no single space that could be filled in this section?
				continue;
PrintByte(3,0,3,1);
			//otherwise we found an immediate threat, no need to think further
			if(!t0)
				return i+0;
			else if(!t1)
				return i+1;
			else if(!t2)
				return i+2;
			else
				return i+3;
		}
		
		for(uint8_t j=0;j<board_height-3;j++){//top left to bottom right
//			PrintChar((i*5)+4,(j*4)+1,'3');
			loff = (j*BOARD_WIDTH_EXT)+i;
			uint8_t t0 = sim[loff]; loff += (BOARD_WIDTH_EXT+1);
			uint8_t t1 = sim[loff]; loff += (BOARD_WIDTH_EXT+1);
			uint8_t t2 = sim[loff]; loff += (BOARD_WIDTH_EXT+1);
			uint8_t t3 = sim[loff]; loff += (BOARD_WIDTH_EXT+1);
			uint8_t count = 0;
			if(t0 && t0 == color)
				count++;
			if(t1 && t1 == color)
				count++;
			if(t2 && t2 == color)
				count++;
			if(t3 && t3 == color)
				count++;
			if(count != 3)//no single space that could be filled in this section?
				continue;
PrintByte(3,0,4,1);
			//otherwise we found an immediate threat, no need to think further
			if(!t0)
				return i+0;
			else if(!t1)
				return i+1;
			else if(!t2)
				return i+2;
			else
				return i+3;
		}
	}
	return 255;
}


uint8_t AIThink(){//this is called with the asssumption the move to be played is the AI player(allowing easy AI vs AI)
	uint8_t i,j,k,color;
	uint16_t l;
	uint16_t loff;
	uint8_t mag,best;
	uint8_t opponent;
	color = 0;
	for(uint16_t c=0;c<BOARD_WIDTH_EXT*BOARD_HEIGHT_EXT;c++){//determine which color is to play next(and assume we are that)
		if(board[c])
			color++;
		sim[c] = board[c];//copy the board to the AI buffer also...
	}

	color &= 1;//if an odd number, the color to play is yellow, otherwise red
//color != color;//HACK
	color += 1;//in the actual board data, 0 is empty, 1 is red, 2 is yellow
	if(color == 2)
		opponent = 1;
	else
		opponent = 2;

	i = AIFindWinningMove(color, 0);//first, see if there is an immediate win option
	if(i != 255){//no need to continue if so
	db();
		return i;
	}
//WaitVsync(1);
	for(i=0;i<board_width;i++){//get the height of each column based on previously dropped pieces
		for(j=0;j<board_height;j++){
			if(board[(j*BOARD_WIDTH_EXT)+i])
				break;
		}
		levels[i] = j;
		weights[i] = (10*j);//all else equal, move to columns with more free spaces(buy more time for the opponent to make an error...)
	}

	for(i=0;i<board_width;i++){//try every valid move, and see if it leads to immediate defeat
		if(sim[i])//can't play this column
			continue;
		loff = ((levels[i]-1)*BOARD_WIDTH_EXT)+i;
		sim[loff] = color;
		levels[i]--;
		uint8_t m = AIFindWinningMove(opponent, 0);//see if there is a winning move for the opponent after this simulated move
WaitVsync(1);
		levels[i]++;
		sim[loff] = 0;
		if(m < 255){
			weights[i] = 0;
	//	PrintByte(20,20,m,1);db();while(1);
		}
	}

AI_CHOOSE_MOVE:

	mag = 0;
	best = 3;
	for(uint8_t i=0;i<board_width;i++){
		if(weights[i] > mag){//current best move?
			mag = weights[i];
			best = i;
		}
	}
	if(mag == 0){//no move was good? we must be forced to play a losing move...
		for(uint8_t i=0;i<board_width;i++){//just select the first legal move then...
			if(!board[i]){
				best = i;
				break;
			}
		}
	}
//	db();
//PrintByte(5,25,best,1);
	return best;
}
#endif //AI_H
