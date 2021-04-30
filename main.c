/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <LCD1602.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);

#define DS_pin GPIO_PIN_8
#define OE_pin GPIO_PIN_9
#define CLK_pin GPIO_PIN_2
#define LATCH_pin GPIO_PIN_1
#define LED_port GPIOB
enum color{red, green};

uint8_t gnd[8][8] ={{0,1,1,1,1,1,1,1},
									 	{1,0,1,1,1,1,1,1},
										{1,1,0,1,1,1,1,1},
										{1,1,1,0,1,1,1,1},
										{1,1,1,1,0,1,1,1},
										{1,1,1,1,1,0,1,1},
										{1,1,1,1,1,1,0,1},
										{1,1,1,1,1,1,1,0}};			// GND


/*uint8_t LED[8][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};	*/	//	RED  GREEN RED GREEN RED ......

uint8_t LED[8][16] = {{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
											{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
											{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
											{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
											{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
											{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
											{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
											{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}};		//	RED  GREEN RED GREEN RED ......

void setLEDsOff(){
	for(int i = 0; i<8; i++){
		for(int j = 0; j<16; j++){
			LED[i][j] = 0;
		}
	}
}

void showLEDs(void){
	LED_port->BRR = OE_pin;
	for(int i = 0; i<8; i++){
		for(int j = 0; j<24; j++){
			if(j<8){
				if(gnd[i][7-j]==1){
					LED_port->BSRR = DS_pin;
				} else {
					LED_port->BRR = DS_pin;
				}
			} else {
				if(LED[i][23-j]){
					LED_port->BSRR = DS_pin;
				} else {
					LED_port->BRR = DS_pin;
				}
			}
			LED_port->BSRR = CLK_pin;
			LED_port->BRR = CLK_pin;
		}
		LED_port->BSRR = LATCH_pin;
		LED_port->BRR = LATCH_pin;
		HAL_Delay(1);
	}
}

/**** BOARD *******************************************************************************/
#define pawnW 1
#define pawnB 101
#define rookW 2
#define rookB 102
#define knightW 3
#define knightB 103
#define bishopW 4
#define bishopB 104
#define queenW 5
#define queenB 105
#define kingW 6
#define kingB 106
#define empty 0
#define b 1
#define w 0
uint32_t lifted = 0;
uint32_t lifted_row = 0;
uint32_t lifted_col = 0;
uint8_t turn = w;
uint8_t game = 0;
enum game{play, draw, white, black};

uint8_t board[8][8] =  {{rookB,knightB,bishopB,queenB,kingB,bishopB,knightB,rookB},
												{pawnB,pawnB,pawnB,pawnB,pawnB,pawnB,pawnB,pawnB},
												{empty,empty,empty,empty,empty,empty,empty,empty},
												{empty,empty,empty,empty,empty,empty,empty,empty},
												{empty,empty,empty,empty,empty,empty,empty,empty},
												{empty,empty,empty,empty,empty,empty,empty,empty},
												{pawnW,pawnW,pawnW,pawnW,pawnW,pawnW,pawnW,pawnW},
												{rookW,knightW,bishopW,queenW,kingW,bishopW,knightW,rookW}};

uint8_t guessBoard[8][8] =  {{rookB,knightB,bishopB,queenB,kingB,bishopB,knightB,rookB},
															{pawnB,pawnB,pawnB,pawnB,pawnB,pawnB,pawnB,pawnB},
															{empty,empty,empty,empty,empty,empty,empty,empty},
															{empty,empty,empty,empty,empty,empty,empty,empty},
															{empty,empty,empty,empty,empty,empty,empty,empty},
															{empty,empty,empty,empty,empty,empty,empty,empty},
															{pawnW,pawnW,pawnW,pawnW,pawnW,pawnW,pawnW,pawnW},
															{rookW,knightW,bishopW,queenW,kingW,bishopW,knightW,rookW}};


/** Check legal moves **********************************/
uint8_t row_low = 0, row_high = 7;
uint8_t col_low = 0, col_high = 7;
uint8_t king[2][2] = {{7,4}, // king white
											{0,4}}; // king black
int8_t knight_logic[2][8] = {{-1,-2,-2,-1,1,2,2,1},{-2,-1,1,2,-2,-1,1,2}};
uint8_t check = 0;
uint8_t printed = 0;

int checkForCheck(){
	int king_row = king[turn][0];
	int king_col = king[turn][1];
	// check by Rook or Queen
	int i = king_row;
	for(int j = king_col + 1; j<8; j++){
		if(j>7){
			break;
		}
		if((guessBoard[i][j]==(((1-turn)*100)+rookW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}
	for(int j = king_col - 1; j>=0; j--){
		if((guessBoard[i][j]==(((1-turn)*100)+rookW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}
	int j = king_col;
	for(int i = king_row - 1; i>=0; i--){
		if(i<0){
			break;
		}
		if((guessBoard[i][j]==(((1-turn)*100)+rookW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}
	for(int i = king_row + 1; i<8; i++){
		if((guessBoard[i][j]==(((1-turn)*100)+rookW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}

	// check by Bishop or Queen
	j = 0;
	for(int i = king_row - 1; i>=0; i--){
		j = (king_col+1)+(king_row-1-i);
		if (j>7){
			break;
		}
		if((guessBoard[i][j]==(((1-turn)*100)+bishopW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}
	for(int i = king_row - 1; i>=0; i--){
		j = (king_col-1)+(king_row-1-i);
		if (j<0){
			break;
		}
		if((guessBoard[i][j]==(((1-turn)*100)+bishopW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}
	for(int i = king_row + 1; i<8; i++){
		j = (king_col-1)+(king_row+1-i);
		if (j<0){
			break;
		}
		if((guessBoard[i][j]==(((1-turn)*100)+bishopW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}
	for(int i = king_row + 1; i<8; i++){
		j = (king_col+1)+(i-(king_row+1));
		if (j>7){
			break;
		}
		if((guessBoard[i][j]==(((1-turn)*100)+bishopW))||
		(guessBoard[i][j]==(((1-turn)*100)+queenW))
		){
			return 1;
		} else if(((guessBoard[i][j]-(turn*100))>0)&&((guessBoard[i][j]-(turn*100))<100)){
			break;
		}
	}

	// check by Knight
	for(int k = 0; k<8; k++){
		i = king_row + knight_logic[0][k];
		j = king_col + knight_logic[1][k];
		if((i<0)||(i>7)||(j<0)||(j>7)){
			continue;
		}
		if(guessBoard[i][j]==(((1-turn)*100)+knightW)){
			return 1;
		}
	}

	//check by Pawn
	i = king_row + (turn?1:-1);
	j = king_col - 1;
	if((i>=0)&&(i<8)&&(j>=0)&&(j<8)){
		if(guessBoard[i][j]==(((1-turn)*100)+pawnW)){
			return 1;
		}
	}
	j = king_col + 1;
	if((i>=0)&&(i<8)&&(j>=0)&&(j<8)){
		if(guessBoard[i][j]==(((1-turn)*100)+pawnW)){
			return 1;
		}
	}
	return 0;
}


int setupLEDs(int i, int j){
	if(board[i][j]==empty){
		guessBoard[i][j] = lifted;
		if(!checkForCheck()){
			LED[i][(2*j)+1] = 1; // green at (i,j)
		}
		guessBoard[i][j] = board[i][j];
	} else {
		if (((turn == w) && (board[i][j]>100))||((turn == b) && (board[i][j] < 100))){
			guessBoard[i][j] = lifted;
			if(!checkForCheck()){
				LED[i][2*j] = 1; // red at (i, j)
			}
			guessBoard[i][j] = board[i][j];
			return 2;
		}
		return 0;
	}
	return 1;
}
int checkPawn(){
	int k = 0;
	int i = (lifted_row-1)+(2*turn);
	i>7?i=7:i;
	i<0?i=0:i;
	for(int j=(lifted_col-1); j<=(lifted_col+1); j++){
		if((j<0)||(j>7)){
			continue;
		}
		if((board[i][j]==empty)&&(j==lifted_col)){
			guessBoard[i][j] = lifted;
			if(!checkForCheck()){
				LED[i][(2*j)+1] = 1; // green at (i,j)
				k++;
			}
			guessBoard[i][j] = board[i][j];
			if((i==1)||(i==6)){											// for first time move
				if(board[(i-1)+(2*turn)][j]==empty){
					guessBoard[i][j] = lifted;
					if(!checkForCheck()){
						LED[(i-1)+(2*turn)][(2*j)+1] = 1; // led green at (i-1)+(2*turn), j
						k++;
					}
					guessBoard[i][j] = board[i][j];
				}
			}
		} else if (((j==lifted_col-1)||(j==lifted_col+1))&&((board[i][j]-(100*turn))!=empty)&&((board[i][j]-(100*turn))<100)){
			guessBoard[i][j] = lifted;
			if(!checkForCheck()){
				LED[i][2*j] = 1; // red at (i, j)
				k++;
			}
			guessBoard[i][j] = board[i][j];
		}
	}
	return k;
}

int checkRook(){
	int k = 0;
	int m = 0;
	int i = lifted_row;
	for(int j = lifted_col + 1; j<8; j++){
		if(j>7){
			break;
		}
		m = setupLEDs(i,j);
		if(m==0){ 	// handle a case for same piece
			break;
		} else {
			k++;
			if(m==2){
				break;
			}
		}
	}
	for(int j = lifted_col - 1; j>=0; j--){
		m = setupLEDs(i,j);
		if(m==0){
			break;
		} else {
			k++;
			if(m==2){break;}
		}
	}
	int j = lifted_col;
	for(int i = lifted_row - 1; i>=0; i--){
		if(i<0){
			break;
		}
		m = setupLEDs(i,j);
		if(m==0){
			break;
		} else {
			k++;
			if(m==2){break;}
		}
	}
	for(int i = lifted_row + 1; i<8; i++){
		m = setupLEDs(i,j);
		if(m==0){
			break;
		} else {
			k++;
			if(m==2){break;}
		}
	}
	return k;
}

int checkBishop(){
	int k = 0;
	int j = 0;
	int m = 0;
	for(int i = lifted_row - 1; i>=0; i--){
		j = (lifted_col+1)+(lifted_row-1-i);
		if ((j>7)||(i<0)||(j<0)){
			break;
		}
		m = setupLEDs(i,j);
		if(m==0){
			break;
		}else{
			k++;
			if(m==2){break;}
		}
	}
	for(int i = lifted_row - 1; i>=0; i--){
		j = (lifted_col-1)+(lifted_row-1-i);
		if ((j<0)||(i<0)||(j>7)){
			break;
		}
		m = setupLEDs(i,j);
		if(m==0){
			break;
		}else{
			k++;
			if(m==2){break;}
		}
	}
	for(int i = lifted_row + 1; i<8; i++){
		j = (lifted_col-1)+(lifted_row+1-i);
		if ((j<0)||(i>7)||(j>7)){
			break;
		}
		m = setupLEDs(i,j);
		if(m==0){
			break;
		}else{
			k++;
			if(m==2){break;}
		}
	}
	for(int i = lifted_row + 1; i<8; i++){
		j = (lifted_col+1)+(i-(lifted_row+1));
		if ((j<0)||(i>7)||(j>7)){
			break;
		}
		m = setupLEDs(i,j);
		if(m==0){
			break;
		}else{
			k++;
			if(m==2){break;}
		}
	}
	return k;
}


int checkKing(){
	int m = 0;
	int k = 0;
	lifted_row == 7 ? (row_high = 7): (row_high = (lifted_row+1));
	lifted_row == 0 ? (row_low = 0): (row_low = (lifted_row-1));
	lifted_col == 7 ? (col_high = 7): (col_high = (lifted_col+1));
	lifted_col == 0 ? (col_low = 0): (col_low = (lifted_col-1));
	for(int i = row_low; i <= row_high; i++){
		for(int j = col_low; j <= col_high; j++){
			if((i==lifted_row)&&(j==lifted_col)){
				continue;
			}
			m = setupLEDs(i,j);
			if(m==0){
				break;
			}else{
				k++;
				if(m==2){break;}
			}
		}
	}
	return k;
}

int checkKnight(){
	int m = 0;
	int i = 0;
	int j = 0;
	for(int k = 0; k<8; k++){
		i = lifted_row + knight_logic[0][k];
		j = lifted_col + knight_logic[1][k];
		if((i<0)||(i>7)||(j<0)||(j>7)){
			continue;
		}
		if(setupLEDs(i,j)){
			m++;
		}
	}
	return m;
}
int checkLegalMoves(){
	if((lifted==pawnW)||(lifted==pawnB)){									// pawn
		return checkPawn();
	} else if ((lifted == rookB)||(lifted==rookW)){				// rook
		return checkRook();
	} else if ((lifted == knightB)||(lifted==knightW)){		// knight
		return checkKnight();
	} else if ((lifted == bishopB)||(lifted==bishopW)){		// bishop
		return checkBishop();
	} else if ((lifted == kingB)||(lifted==kingW)){				// king
		return checkKing();
	} else if ((lifted == queenB)||(lifted==queenW)){			// queen
		if(!checkBishop()){
			if(!checkRook()){
				return 0;
			}
		}
	} return 0;
}

uint8_t checkGame(){
	if(checkForCheck()){
		if(!check){printed = 0;}
		check = 1;
	} else {
		if(check){printed = 0;}
		check = 0;
	}
	int k = 0;
	for(int i = 0; i<8; i++){
		for(int j = 0; j<8; j++){
			if(((board[i][j] - (turn * 100))>0)&&((board[i][j] - (turn * 100))<100)){
				lifted = board[i][j];
				lifted_row = i;
				lifted_col = j;
			} else {
				continue;
			}
			if(checkLegalMoves()){
				k++;
				break;
			}
		}
		if (k>0){
			break;
		}
	}
	lifted=0;
	setLEDsOff();
	if(check){
		LED[king[turn][0]][2*king[turn][1]] = 1;	// red on king; king in CHECK condition
	}
	if(check && !k){
		return white+(1-turn);
	} else if(!check && !k){
		return draw;
	} else if(k){
		return play;
	}
	return play;
}


/* Sensor Section *********************************************/
#define DEMUX_port GPIOA
uint32_t DEMUX_pins[4] = {GPIO_PIN_11, GPIO_PIN_10, GPIO_PIN_9, GPIO_PIN_8}; // ABCE

#define sensor_port GPIOB
uint32_t sensor_pins[8] = {GPIO_PIN_7,GPIO_PIN_6,GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_3,GPIO_PIN_15,GPIO_PIN_14,GPIO_PIN_13}; // 0 to 7
void readSensors(void){
	for(uint8_t j = 0; j<8; j++){ // activating column
		DEMUX_port->BRR = DEMUX_pins[3];
		for(uint8_t k = 0; k<3; k++){
			if(j&(1<<k)){
				DEMUX_port->BSRR = DEMUX_pins[k];
			} else {
				DEMUX_port->BRR = DEMUX_pins[k];
			}
		}
		DEMUX_port->BSRR = DEMUX_pins[3];
		HAL_Delay(1);
		for(uint8_t i = 0; i<8; i++){  // reading the row
			if((sensor_port->IDR & sensor_pins[i])&&(board[i][j]!= empty)){ // when a piece is lifted
				if(lifted==0){
					lifted = board[i][j];
					lifted_row = i;
					lifted_col = j;
					board[i][j] = empty;
					guessBoard[i][j] = empty;
					LED[i][(2*j)+1] = 1; // green at (i,j)
				} else {
					board[i][j] = empty;
					guessBoard[i][j] = empty;
				}
			}
			else if((!(sensor_port->IDR & sensor_pins[i]))&&(board[i][j] == empty)){ // when piece is placed back to some position
				if((lifted == kingB)||(lifted==kingW)){
					king[turn][0] = lifted_row;
					king[turn][1] = lifted_col;
				}
				if((i!=lifted_row)&&(j!=lifted_col)){
					turn = 1 - turn;
				}
				board[i][j] = lifted;
				guessBoard[i][j] = lifted;
				game = checkGame();
				lifted = 0;
				lifted_row = 0;
				lifted_col = 0;
			}
			if(lifted){
				checkLegalMoves();
			}
		}
	}
	DEMUX_port->BRR = DEMUX_pins[3];
}

/*** LCD *********************************************/
#define lcd_port GPIOC
uint32_t lcd[8] = {GPIO_PIN_1,GPIO_PIN_0,GPIO_PIN_15,GPIO_PIN_14,GPIO_PIN_13,GPIO_PIN_12,GPIO_PIN_11,GPIO_PIN_10};
uint32_t lcdRS = GPIO_PIN_3;
uint32_t lcdEN = GPIO_PIN_2;
enum lcd_mode{cmd, data};
void lcd_do(int cmd, int m){
	if(m){ // data mode
		lcd_port->BSRR = lcdRS;
	} else { // command mode
		lcd_port->BRR = lcdRS;
	}
	lcd_port->BSRR = lcdEN;
	for(int i = 0; i<8; i++){
		if(cmd & (1<<i)){
			lcd_port->BSRR = lcd[i];
		} else {
			lcd_port->BRR = lcd[i];
		}
	}
	HAL_Delay(1);
	lcd_port->BRR = lcdEN;
}

void lcd_begin(){
	HAL_Delay(20);
	lcd_do(0x02, 0);
	lcd_do(0x38, 0);
	lcd_do(0x0c, 0);
	lcd_do(0x06, 0);
	lcd_do(0x01, 0);
}

void lcd_print(char str[]){
	for(uint32_t i = 0; str[i]!='\0'; i++){
		lcd_do(str[i],1);
	}
}

void lcd_setCursor(int x, int y){
	if ( y == 1 ) {
		lcd_do(0xc0 + x, 0);
	} else if ( y ==0 ){
		lcd_do(0x80 + x, 0);
	}
}


int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  GPIOA->MODER = 0x28555555;
  GPIOC->MODER = 0x55555555;
  GPIOB->MODER = 0x01550015;
  GPIOB->PUPDR = 0x55005540; // pull ups
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  /* USER CODE END 2 */
    lcd_init();

    setLEDsOff();
    printed = 0;
    game = play;
    lcd_setCursor(0,0);
    lcd_print("White Move");
    //HAL_Delay(1000);
  //  lcd_clear();
  //  lcd_clear();
  //  lcd_put_cur(0, 0);
  //  lcd_send_string("Hello ");
  //  lcd_send_string("World ");
  //
  //  lcd_put_cur(1, 0);
  //  lcd_send_string("Sebastian");
  //  HAL_Delay(1000);
  //  lcd_clear();
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
			    readSensors();
	  	  		showLEDs();
	  	  		if(check){
	  	  			if(!printed){
	  	  				lcd_clear();
	  	  				lcd_put_cur(0, 0);
	  	  				lcd_send_string("Check");
	  	  				printed = 1;
	  	  			}
	  	  		}
	  	  		if(game!=play){
	  	  		    lcd_clear();
	  	  		    lcd_put_cur(0, 0);
	  	  			lcd_send_string("Check");
	  	  			HAL_Delay(0);
	  	  			while(1);
	  	  		}
	  		    lcd_clear();
	  	  		lcd_put_cur(0, 0);
	  	  		lcd_send_string("Reti");
	  	  		lcd_put_cur(1, 0);
	  	  		lcd_send_string("Opening");
	  	  		HAL_Delay(100);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 48-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA2 PA3 PA4
                           PA5 PA6 PA7 PA8
                           PA9 PA10 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 PB13 PB14
                           PB15 PB3 PB4 PB5
                           PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
