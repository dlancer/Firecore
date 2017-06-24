#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "textProg.h"
#include "common.h"

//---------------------------------------------------------------------------//
// to make cool unfold animation
uint8_t titleRowRPosX = PIC_TITLE_R_BASE_X;
uint8_t titleRowLPosX = PIC_TITLE_L_BASE_X;

//uint8_t maxStars = MAX_STARS;
stars_t stars[MAX_STARS];
bool startState = true;
//---------------------------------------------------------------------------//
void printTextSlow(const uint8_t *text)
{
  for(uint16_t count=0; count < strlen_P(text); count++) {
    tftPrintChar(pgm_read_byte(text + count));
#if ADD_SOUND
    if(soundEnable) toneBuzz(500, 1);
#endif
  }
}

void drawTextWindow(const uint8_t *text, const uint8_t *btnText)
{
  tftSetTextSize(1);
  tftSetTextColor(COLOR_WHITE);
  tftSetCursor(TEXT_WINDOW_X, TEXT_WINDOW_Y);
  tftFillRect(0, 88,  159, 40, INDIGO_COLOR);  // Frame 0
  tftDrawRect(2, 90, 155, 35, COLOR_WHITE);    // Frame 1
  
  // set F_CPU to 500 kHz by ps = 64 (0x06)
  setMainFreq(0x06);  // It is so epic retro !!!!
  tftPrint_P(text);
  //printTextSlow(text);
  setMainFreq(0x00);
  
  tftSetCursor(TEXT_OK_X, TEXT_OK_Y);
  tftPrint_P(btnText);
}

void drawStars(void)
{
  int16_t tmpPosX, tmpPosY;
  
  // Arrrrrghh!!!!
  // How to make it faster?!?!
  
  // draw stars and blow your mind, if still not
  for(uint8_t count=0; count < MAX_STARS; count++) {
    tmpPosX = stars[count].pos.x;
    tmpPosY = stars[count].pos.y;
    
    tftDrawPixel(tmpPosX, tmpPosY, currentBackGroundColor); // clear previous star
    
    // now move them
    if((tmpPosX -= STAR_STEP) < 0) {
      tmpPosX = TFT_W;
      stars[count].pos.y = RN % (TFT_H-4);
      stars[count].color = RN % 80;
    }
    
    tftDrawPixel(tmpPosX, tmpPosY, getPicWord(nesPalette_ext, stars[count].color));
    
    // now apply back what we change
    stars[count].pos.x = tmpPosX;
  }
}
// --------------------------------------------------------------- //

void rocketEpxlosion(rocket_t *pRocket)
{
  // Please, don`t say anything about this...
  // generate delay about 20 ms

  pRocket->onUse = false;
  uint16_t posX = pRocket->pos.x;
  uint16_t posY = pRocket->pos.y;
  uint8_t i;

  for(i = 0; i < 5; i++) { // base formation
    tftFillCircle(posX, posY, i*2, COLOR_WHITE);
    tftDrawCircle(posX, posY, i*2, COLOR_ORANGE);
#if ADD_SOUND
    if(soundEnable) toneBuzz(200*i, 2);
#endif
  }

  for(i = 5; i > 0; i--) { // something ?
    tftDrawCircle(posX, posY, i*2, COLOR_YELLOW);
#if ADD_SOUND
    if(soundEnable)  toneBuzz(100*i, 2);
#endif
  }

  for(i = 0; i < 7; i++) { // remove smoke
    tftFillCircle(posX, posY, i*2, currentBackGroundColor);
#if ADD_SOUND
    if(soundEnable) toneBuzz(50*i, 2);
#endif
  }
}
// --------------------------------------------------------------- //

void drawShip(void)
{
  const uint8_t *pic = (shipState ? shipBaseHi : shipBaseLow);
  uint16_t picSize = (shipState ? SHIP_BASE_HI_PIC_SIZE : SHIP_BASE_LOW_PIC_SIZE);
  
  shipState = !shipState;
  movePicture(&ship.posBase, &ship.posNew, SHIP_PIC_W, SHIP_PIC_H);
  
  drawBMP_RLE_P(ship.posBase.x, ship.posBase.y, 
                     SHIP_PIC_W, SHIP_PIC_H, pic, picSize);
}

void drawPlayerRockets(void)
{
  for(uint8_t count =0; count < MAX_PEW_PEW; count++) {
    if(playeRockets[count].onUse) {
      // remove previous rocket image
      tftFillRect(playeRockets[count].pos.x, playeRockets[count].pos.y, 
                                     ROCKET_W, ROCKET_H, currentBackGroundColor);
      
      if((playeRockets[count].pos.x += PLAYER_ROCKET_SPEED) < TFT_W) {
        
        drawBMP_RLE_P(playeRockets[count].pos.x, playeRockets[count].pos.y,
                                  ROCKET_W, ROCKET_H, rocketPic, ROCKET_PIC_SIZE);
      } else {
        playeRockets[count].pos.x = ship.posBase.x + ROCKET_OFFSET_X;
        playeRockets[count].onUse = false;
      }
    }
  }
}
// --------------------------------------------------------------- //

void drawInVaders(void)
{
  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(alien[count].alive) { // ALIIIVEE! IT`S ALIIVEEE!

      const uint8_t *pic = (alien[count].state ? alienShipHi : alienShipLow);
      uint16_t picSize = (alien[count].state ? ALIEN_SHIP_HI_PIC_SIZE : ALIEN_SHIP_LOW_PIC_SIZE);
      
      alien[count].state = !alien[count].state;
      movePicture(&alien[count].posBase, &alien[count].posNew, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
      
      drawBMP_RLE_P(alien[count].posBase.x, alien[count].posBase.y,
                       ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H, pic, picSize);
    }    
  }
}
// --------------------------------------------------------------- //

void drawStart(void)
{
  tftSetCursor(60, 100);
  
  if(startState) {
    tftSetTextColor(COLOR_WHITE);
  } else {
    tftSetTextColor(currentBackGroundColor);
  }
  startState = !startState;
  tftPrint_P(pressAtext);
}

void drawTitleText(void)
{
  drawBMP_RLE_P(TITLE_PIC_POS_X, TITLE_PIC_POS_Y,
                  TEXT_TITLE_HI_W, TEXT_TITLE_HI_H, titleTextPic, TEXT_TITLE_HI_SIZE);
}

// make unfold animation
void drawRows(void)
{
  titleRowLPosX -= PIC_ROWS_STEP;
  titleRowRPosX += PIC_ROWS_STEP;
  
  if((titleRowLPosX <= PIC_ROW_L_POS_X ) || ( titleRowRPosX >= PIC_ROW_R_POS_X )) {
    
    titleRowLPosX = PIC_TITLE_L_BASE_X;
    titleRowRPosX = PIC_TITLE_R_BASE_X;
    
    deleteAllTasks();
    addTitleTasks();
    drawTitleText();
  } else {
    drawBMP_RLE_P(titleRowLPosX, PIC_ROW_L_POS_Y,
                    PIC_TITLE_ROW_WH, PIC_TITLE_ROW_WH,
                      rowsLeftPic, ROWS_L_DATA_PIC_SIZE);
    
    drawBMP_RLE_P(titleRowRPosX, PIC_ROW_R_POS_Y,
                    PIC_TITLE_ROW_WH, PIC_TITLE_ROW_WH,
                      rowsRightPic, ROWS_R_DATA_PIC_SIZE);
  }
}
// --------------------------------------------------------------- //

// hardware scrolling; blocks everything
void screenSliderEffect(uint16_t color)
{
  tftDrawFastVLine(TFT_W, 0, TFT_H, color);
  for (int16_t i = TFT_W; i >= 0; i--) {
    tftScrollSmooth(1, i, 4);
    tftDrawFastVLine(TFT_W-i-1, 0, TFT_H, color);
  }
  tftDrawFastVLine(0, 0, TFT_H, color);
}
// --------------------------------------------------------------- //

void drawBMP_RLE_P(int16_t x, int16_t y, uint8_t w, uint8_t h,
                              const uint8_t *pPic, int16_t sizePic)
{
  // This is used
  // when need maximum pic compression,
  // and you can waste some CPU resources for it;
  // It use very simple RLE compression;
  // Also draw background color;
  
  uint16_t repeatColor;
  uint8_t tmpInd, repeatTimes;
  
  tftSetAddrWindow(x, y, x+w-1, y+h-1); // -1 == convert to display addr size
  
  while(sizePic--) {  // compressed pic size!
    // get color index or repeat times
    tmpInd = getPicByte(pPic);
    
    if(~tmpInd & 0x80) { // is it color index?
      repeatTimes = 1;
    } else {   // nope, just repeat color
      repeatTimes = tmpInd - 0x80;
      // get previous color index to repeat
      tmpInd = getPicByte(pPic-1);
    }
    
    // get color from colorTable by tmpInd color index
    repeatColor = getPicWord(nesPalette_ext, tmpInd);
/*
    if(repeatColor == replaceColor) {
      repeatColor = currentBackGroundColor;
    }
*/  
    do {
      --repeatTimes;
      pushColorFast(repeatColor);
    } while(repeatTimes);
    
    ++pPic;
  }
}
