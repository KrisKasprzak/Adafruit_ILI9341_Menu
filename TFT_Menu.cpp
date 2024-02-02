#include "TFT_Menu.h"

template<typename T, typename F>
TFTEditMenu<T,F>::TFTEditMenu(T *Display, bool EnableTouch) {
  d = Display;
  enabletouch = EnableTouch;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::init(uint16_t TextColor, uint16_t BackgroundColor,
                    uint16_t HighlightTextColor, uint16_t HighlightColor,
                    uint16_t SelectedTextColor, uint16_t SelectedColor,
                    uint16_t MenuColumn, uint16_t ItemRowHeight, uint16_t MaxRow,
                    const char *TitleText, F ItemFont, F TitleFont) {

  itc = TextColor;
  ibc = BackgroundColor;
  ihtc = HighlightTextColor;
  ihbc = HighlightColor;
  istc = SelectedTextColor;
  isbc = SelectedColor;

  bcolor = TextColor;
  sbcolor = TextColor;
  tbt = 0;        // title bar top
  tbl = 0;        // title bar left
  tbh = 40;       // title bar height
  tbw = d->width(); // title bar width, default to screen width
  tox = 20;       // x pixel offset for title text to manage centering
  toy  = 8;       // y pixel offset for title text to manage height centering
  isx = 10;       // where to start the menu item from left, default indent 10
  mm = 5;
  // isy = tbt + tbh - ItemRowHeight + mm;  // where to start the menu item from top, default below menu bar by 10, note because array is 1 based we start drawing down a row--so back off a row, but pad 10

  isy = tbt + tbh + mm;  // where to start the menu item from top, note because array is 1 based we start drawing down a row--so back off a row, but pad 10



  irh = ItemRowHeight;  // select bar height
  irw = tbw - isx;    // select bar width, default to full width
  col = MenuColumn;
  imr = MaxRow;     // user has to indicate this
  iox = 0;  // pixels to offset text in menu bar
  ioy = 0;  // pixels to offset text in menu bar
  itemf = &ItemFont;     // item font
  titlef = &TitleFont;     // title font
  ttc = TextColor;      // title text color
  tfc = HighlightColor;     // title fill color
  ditc = MENU_C_DKGREY;
  strncpy(ttx, TitleText, MAX_CHAR_LEN);
  strncpy(etx, EXIT_TEXT, MAX_CHAR_LEN);
  currentID = 1;    // id of current highlighted or selected item (1 to ID)
  cr = 1;       // current selected row on the screen (1 to mr-1)
  totalID = 0;    // maximum number of items (where first position is 1)
  sr = 0;       // draw offset for the menu array where first postion is 0
  pr = 1;       // previous selected rown (1 to mr - 1)
  rowselected = false;
  enablestate[0] = true;
  radius = 0;
  thick = 0;
}

template<typename T, typename F>
int TFTEditMenu<T,F>::addNI(const char *ItemText, float Data, float LowLimit, float HighLimit, float Increment,
                    byte DecimalPlaces, const char **ItemMenuText) {

  totalID++;
  strcpy(itemlabel[totalID], ItemText);
  data[totalID] = Data;
  low[totalID] = LowLimit;
  high[totalID] = HighLimit;
  inc[totalID] = Increment;
  dec[totalID] = DecimalPlaces;
  haslist[totalID] = false;
  value[totalID] = Data;

  if (ItemMenuText) {
    haslist[totalID] = true;
    itemtext[totalID] = (char **) ItemMenuText;
  }

  itemBitmap[totalID] = 0;
  bmp_w[totalID] = 0;
  bmp_h[totalID] = 0;
  IconType[totalID] = ICON_NONE;

  enablestate[totalID] = true;
  return (totalID);

}
template<typename T, typename F>
int TFTEditMenu<T,F>::addMono(const char *ItemText, float Data, float LowLimit, float HighLimit, float Increment,
                      byte DecimalPlaces, const char **ItemMenuText,
                      const unsigned char *Bitmap, uint8_t BitmapWidth, uint8_t BitmapHeight) {

  totalID++;
  strcpy(itemlabel[totalID], ItemText);
  data[totalID] = Data;
  low[totalID] = LowLimit;
  high[totalID] = HighLimit;
  inc[totalID] = Increment;
  dec[totalID] = DecimalPlaces;
  haslist[totalID] = false;
  value[totalID] = Data;

  if (ItemMenuText) {
    haslist[totalID] = true;
    itemtext[totalID] = (char **) ItemMenuText;
  }

  itemBitmap[totalID] = (unsigned char*) Bitmap;
  bmp_w[totalID] = BitmapWidth;
  bmp_h[totalID] = BitmapHeight;
  IconType[totalID] = ICON_MONO;

  enablestate[totalID] = true;
  return (totalID);

}

template<typename T, typename F>
int TFTEditMenu<T,F>::add565(const char *ItemText, float Data, float LowLimit, float HighLimit, float Increment,
                     byte DecimalPlaces, const char **ItemMenuText,
                     const uint16_t *Bitmap, uint8_t BitmapWidth, uint8_t BitmapHeight) {

  totalID++;
  strcpy(itemlabel[totalID], ItemText);
  data[totalID] = Data;
  low[totalID] = LowLimit;
  high[totalID] = HighLimit;
  inc[totalID] = Increment;
  dec[totalID] = DecimalPlaces;
  haslist[totalID] = false;
  value[totalID] = Data;

  if (ItemMenuText) {
    haslist[totalID] = true;
    itemtext[totalID] = (char **) ItemMenuText;
  }

  item565Bitmap[totalID] = (uint16_t* ) Bitmap;
  bmp_w[totalID] = BitmapWidth;
  bmp_h[totalID] = BitmapHeight;
  IconType[totalID] = ICON_565;

  enablestate[totalID] = true;
  return (totalID);

}

template<typename T, typename F>
int TFTEditMenu<T,F>::selectRow() {

  if (currentID == 0) {
    // trigger to exit out of the menu
    item = 0;
    return 0;
  }
  // otherwise this is the trigger to enable editing in the row

  rowselected = !rowselected;

  drawRow(currentID);
  item = currentID;
  return currentID;

}


template<typename T, typename F>
void TFTEditMenu<T,F>::drawHeader(bool hl, uint8_t style) {

  setFont(titlef);

  if (enabletouch) {

    if (style == BUTTON_PRESSED) {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, ihbc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
    else {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, tfc);
      d->setTextColor(ttc);
      d->print(ttx);
    }

    if (totalID > imr) {
      // up arrow
      d->fillTriangle( tbl +  25 + 15 , tbt + tbh - 10, // bottom left
                       tbl +  25 - 15 , tbt + tbh - 10, // bottom right
                       tbl +  25    , tbt + 10, //center
                       ttc);
      // down arrow
      d->fillTriangle( tbw -  25 + 15 , tbt + 10, // top left
                       tbw -  25 - 15 , tbt + 10, // top right
                       tbw -  25    , tbt + tbh - 10, //center
                       ttc);
    }

  }
  else {
    if (hl) {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, ihbc);
      d->setTextColor(ihtc);
      d->print(etx);

      d->fillTriangle( tbw - (tbh / 3.0) - 5, tbt + (tbh / 2.0) - 3,
                       tbw - (tbh / 6.0) - 5, tbt + (tbh / 2.0) - (tbh / 3.0),
                       tbw - 5,       tbt + (tbh / 2.0) - 3,
                       ihtc);

      d->fillTriangle( tbw - (tbh / 3.0) - 5, tbt + (tbh / 2.0) + 3,
                       tbw - (tbh / 6.0) - 5, tbt + (tbh / 2.0) + (tbh / 3.0),
                       tbw - 5,       tbt + (tbh / 2.0) + 3,
                       ihtc);
    }

    else {
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, tfc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
  }



}

template<typename T, typename F>
int TFTEditMenu<T,F>::press(int16_t ScreenX, int16_t ScreenY) {

  int bs;


  // check if header pressed
  if (  (ScreenX > tbl) && (ScreenX < (tbw + tbl))  &&  (ScreenY > tbt ) && (ScreenY < (tbt + tbh) )) {

    if (ScreenX < (tbl + 25 + 15)) {
      // up arrow
      MoveDown();
      return -1;
    }

    if (ScreenX > (tbw - 25 - 15)) {
      // down arrow
      MoveUp();
      return -1;
    }

    // above will allow arrow up down but
    // let's not allow menu exit until they are done editing the line item
    if (rowselected) {
      return - 1;
    }
    // not up / down and nothing selected, allow menu exit
    return 0;
  }

  for (i = 1; i <= imr; i++) {
    bs = icox + bmp_w[i + sr] + isx;
    itx = bs + iox;

    if (
      (ScreenX > bs) &&
      (ScreenY > (isy + (irh * (i - 1)))) &&
      (ScreenX < (irw)) &&
      (ScreenY < (isy + irh + (irh * (i - 1))))
    ) {
      if (!enablestate[i + sr]) {
        return -1;
      }
      delay(100);
      if (rowselected) {
        if (i == cr) {
          rowselected = !rowselected;
          item = i + sr;
          cr = i;
          currentID = i + sr;
          //drawRow(currentID);
          return currentID;
        }
      }
      else {
        rowselected = !rowselected;
        currentID = i + sr;
        cr = i;
        //drawRow(currentID);
        return currentID;
      }
    }

  }
  return -1;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::up() {
  cr--;
  currentID--;
  drawItems();
  if (enablestate[currentID] == false) {
    while (enablestate[currentID] == false) {
      cr--;
      currentID--;
      drawItems();
    }
  }
}

template<typename T, typename F>
void TFTEditMenu<T,F>::down() {

  cr++;
  currentID++;
  drawItems();
  if (enablestate[currentID] == false) {
    while (enablestate[currentID] == false) {
      cr++;
      currentID++;
      drawItems();
    }
  }
}


template<typename T, typename F>
void TFTEditMenu<T,F>::MoveUp() {

  if (rowselected) {
    incrementUp();
  }
  else {
    up();
  }

}

template<typename T, typename F>
void TFTEditMenu<T,F>::MoveDown() {

  if (rowselected) {
    incrementDown();
  }
  else {
    down();
  }
}



template<typename T, typename F>
void TFTEditMenu<T,F>::draw() {
  drawHeader(false, 0);
  drawItems();
}

template<typename T, typename F>
void TFTEditMenu<T,F>::drawItems() {

  int  bs;

  if (imr > totalID) {
    imr = totalID;
  }
  redraw = false;
  // determine if we need to pan or just increment the list

  if ((currentID >  totalID) && (sr >= 0) ) {
    // up to top
    cr = 0;
    currentID = 0;
    sr = 0;
    redraw = true;
  }
  else if ( (cr < 0) && (sr == 0) ) {
    //  pan whole menu to bottom
    cr = imr;
    currentID = totalID;
    sr = totalID - imr;
    redraw = true;
  }
  else if ( (cr > imr) && ((sr + cr) > totalID) ) {
    //  pan whole menu to top
    cr = 1;
    currentID = 1;
    sr = 0;
    redraw = true;
  }
  else if ((cr > imr) && (sr >= 0))   {
    // scroll whole list up one by one
    sr++;
    cr = imr;
    redraw = true;
  }
  else if ((cr < 1) && (sr > 0))   {
    // scroll whole list down one by one
    sr--;
    cr = 1;
    redraw = true;
  }

  // determine if we need arrows
  if (cr == imr) {
    moredown = true;
  }
  if (cr == 1) {
    moreup = true;
  }

  if (enablestate[currentID] == false) {
    return;
  }

  // determine if we need to redraw the header and draw
  if ((currentID == 0) && (cr == 0)) {
    drawHeader(true, 0);
    redrawh = true;
  }
  else if ((pr == 0) && (cr > 0)) {
    if (redrawh) {
      redrawh = false;
      drawHeader(false, 0);
    }
  }

  setFont(itemf);

  // now draw the items in the rows
  for (i = 1; i <= imr; i++) {
    // menu bar start
    bs = icox + bmp_w[i + sr] + isx;

    // text start
    itx = bs + iox;

    if (enablestate[i + sr]) {
      temptColor = itc;
    }
    else {
      temptColor = ditc;
    }

    if (redraw) {
      // scroll so blank out every row including icon since row will get scrolled
      d->fillRect(bs, isy - irh + (irh * i), irw - bs, irh, ibc); // back color
    }

    if (i == pr) {
      // maybe just row change so blank previous
      d->fillRect(bs, isy - irh + (irh * pr) , irw - bs, irh, ibc); // back color
    }

    if ((i == cr) && (!enabletouch)) {

      if (radius > 0) {
        d->fillRoundRect(bs,      isy - irh + (irh * i) ,      irw - bs,       irh,        radius,   bcolor);
        d->fillRoundRect(bs + thick,  isy - irh + (irh * i) + thick, irw - bs - (2 * thick),  irh - (2 * thick),  radius,   ihbc);
      }
      else {
        d->fillRect(bs,       isy - irh + (irh * i) ,      irw - bs  ,        irh, bcolor);
        d->fillRect(bs + thick ,  isy - irh + (irh * i) + thick, irw - bs - (2 * thick) , irh - (2 * thick), ihbc);
      }

      if (moreup) {
        d->fillTriangle(irw - 8 - thick,              isy - irh + (irh * i) + irh - (irh / TRIANGLE_H),
                        irw - ((irh / TRIANGLE_W)*.5) - 8 - thick, isy - irh + (irh * i) + (irh / TRIANGLE_H),
                        irw - (irh / TRIANGLE_W) - 8 - thick,    isy - irh + (irh * i) + irh - (irh / TRIANGLE_H), ihtc);
      }
      if (moredown) {
        d->fillTriangle(irw - 8 - thick,                 isy - irh + (irh * i) + (irh / TRIANGLE_H),
                        irw - ((irh / TRIANGLE_W)*.5) - 8 - thick, isy - irh + (irh * i) + irh - (irh / TRIANGLE_H),
                        irw - (irh / TRIANGLE_W) - 8 - thick,    isy - irh + (irh * i) + (irh / TRIANGLE_H),     ihtc);
      }
      temptColor = ihtc;

    }

    // write bitmap

    if (IconType[i + sr] == ICON_MONO) {
      drawMonoBitmap(icox,  icoy + isy - irh + (irh * i), itemBitmap[i + sr], bmp_w[i + sr], bmp_h[i + sr], itc );
    }
    else if (IconType[i + sr] == ICON_565) {
      draw565Bitmap(icox,  icoy + isy - irh + (irh * i), item565Bitmap[i + sr], bmp_w[i + sr], bmp_h[i + sr] );
    }

    // write text
    d->setTextColor(temptColor);
    d->setCursor(itx , isy - irh + (irh * i) + ioy);
    d->print(itemlabel[i + sr]);

    // write new val
    d->setCursor(col , isy - irh + (irh * i) + ioy);
    if (haslist[i + sr]) {
      d->print(itemtext[i + sr][(int) data[i + sr]]);
    }
    else {
      d->print(data[i + sr], dec[i + sr]);
    }

  }

  moreup = false;
  moredown = false;
  pr = cr;

}

template<typename T, typename F>
void TFTEditMenu<T,F>::drawRow(int ID) {

  int  bs;
  int hr = ID - sr;
  bool drawarrow = false;

  uint16_t textcolor, backcolor, sbackcolor;

  moredown = false;
  moreup = false;

  // compute starting place for text
  itx = isx + icox +  bmp_w[ID];
  textcolor = itc;

  if (ID == 0) {
    drawHeader(false, rowselected);
  }

  if (ID <= 0) {
    return;
  }

  bs = icox + bmp_w[hr] + isx;

  if (ID <= sr) {
    // item is off screen
    return;
  }

  if (ID > (sr + imr)) {
    // item is off screen
    return;
  }

  if (enabletouch) {

    if (ID == currentID) {
      if (!rowselected) {
        textcolor = itc;
        backcolor = ibc;
	    sbackcolor = ibc;
      }
      else {
        // case draw row is the selected row
        textcolor = istc;
        backcolor = isbc;
	    sbackcolor = sbcolor;
      }
    }
    else {
      // case draw row is not the selected row
      textcolor = itc;
      backcolor = ibc;
      sbackcolor = ibc;
    }

  }
  else {
    if ((cr + sr) == ID) {

      if (!rowselected) {
        // case 1 draw som is draw row AND highlighted
        textcolor = ihtc;
        backcolor = ihbc;
	sbackcolor = ibc;
        drawarrow = true;

      }
      else if (rowselected) {
        // case 2 = current row is draw row AND highlighted
        textcolor = istc;
        backcolor = isbc;
	sbackcolor = bcolor;
        drawarrow = false;

      }
    }
    else  {
      // current row is not selected
      if (enablestate[ID]) {
        textcolor = itc;
      }
      else {
        textcolor = ditc;
      }
      backcolor = ibc;
      sbackcolor = ibc;
      drawarrow = true;
    }
  }

  if ((cr == 5) && (!rowselected)) {
    moredown = true;
  }
  else if ((cr == 1) && (!rowselected)) {
    moreup = true;
  }
  else if (cr == 0) {
    moredown = true;
    moreup = true;
  }

  if (enabletouch) {
    drawarrow = false;
  }

  if (ID == 0) {
    if (rowselected) {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, ihbc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
    else {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, tfc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
  }
  else {

    if (!enabletouch) {
      // non touch
      if (radius > 0) {
        d->fillRoundRect(bs,      isy - irh + (irh * hr) ,     irw - bs,       irh,        radius,   sbackcolor);
        d->fillRoundRect(bs + thick,  isy - irh + (irh * hr) + thick,  irw - bs - (2 * thick),  irh - (2 * thick),  radius,   backcolor);
      }
      else {
        d->fillRect(bs,       isy - irh + (irh * hr) ,     irw - bs  ,        irh, sbackcolor);
	d->fillRect(bs + thick,  isy - irh + (irh * hr) + thick,  irw - bs - (2 * thick),  irh - (2 * thick),  backcolor);
      }
    }

    else {
      // touch
      if (rowselected) {

        if (radius > 0) {
          d->fillRoundRect(bs,      isy - irh + (irh * hr) ,     irw - bs,       irh,        radius,   sbackcolor);
          d->fillRoundRect(bs + thick,  isy - irh + (irh * hr) + thick,  irw - bs - (2 * thick),  irh - (2 * thick),  radius,   backcolor);
        }
        else {
          d->fillRect(bs,       isy - irh + (irh * hr) ,     irw - bs  ,        irh, sbackcolor);
	  d->fillRect(bs + thick,  isy - irh + (irh * hr) + thick,  irw - bs - (2 * thick),  irh - (2 * thick),  backcolor);
        }

      }
      else {

        d->fillRect(bs, isy - irh + (irh * hr) , irw - bs, irh, ibc); // back color

      }
    }
    if (!enablestate[ID]) {
      textcolor = ditc;
    }
    // write text
    itx = bs + iox;
    setFont(itemf);
    d->setTextColor(textcolor);
    d->setCursor(itx , isy - irh + (irh * hr) + ioy);
    d->print(itemlabel[ID]);

    d->setCursor(col , isy - irh + (irh * (ID - sr)) + ioy);
    if (haslist[ID]) {
      d->print(itemtext[ID][(int) data[ID]]);
    }
    else {
      d->print(data[ID], dec[ID]);
    }

    // write bitmap
    if (IconType[ID] == ICON_MONO) {
      drawMonoBitmap(icox,  icoy + isy - irh + (irh * (ID - sr)), itemBitmap[ID], bmp_w[ID], bmp_h[ID], temptColor );
    }
    else if (IconType[ID] == ICON_565) {
      draw565Bitmap(icox,  icoy + isy - irh + (irh * (ID - sr)), item565Bitmap[ID], bmp_w[ID], bmp_h[ID] );
    }

    if (drawarrow) {
      if (moreup) {
        d->fillTriangle(  irw - 2 - (2 * thick),             isy + (irh * cr) - irh + irh - (irh / TRIANGLE_H),
                          irw - ((irh / TRIANGLE_W)*.5) - 2 - (2 * thick),  isy - irh + (irh * cr) + (irh / TRIANGLE_H),
                          irw - (irh / TRIANGLE_W) - 2 - (2 * thick),   isy - irh + (irh * cr) + irh - (irh / TRIANGLE_H),
                          ihtc);
      }
      else if (moredown) {
        d->fillTriangle(  irw - 2 - (2 * thick),              isy - irh + (irh * cr) + (irh / TRIANGLE_H),
                          irw - ((irh / TRIANGLE_W)*.5) - 2 - (2 * thick),  isy - irh + (irh * cr) + irh - (irh / TRIANGLE_H),
                          irw - (irh / TRIANGLE_W) - 2 - (2 * thick),   isy - irh + (irh * cr) + (irh / TRIANGLE_H),
                          ihtc);
      }
    }

  }
}

template<typename T, typename F>
void TFTEditMenu<T,F>::setIncrementDelay(uint16_t Delay) {

  incdelay = Delay;

}

template<typename T, typename F>
void TFTEditMenu<T,F>::incrementUp() {

  setFont(itemf);
  d->setTextColor(istc);


  if (haslist[currentID]) {

    if ((data[currentID] + inc[currentID]) < high[currentID]) {
      data[currentID] += inc[currentID];
      d->fillRect(col, isy - irh + (irh * cr) + thick, irw - col - (2 * thick), irh - (2 * thick), isbc);
      d->setCursor(col + iox , isy - irh + (irh * cr) + ioy);
      d->print(itemtext[currentID][(int) data[currentID]]);
    }
    else {
      data[currentID] = low[currentID];
      d->fillRect(col, isy - irh + (irh * cr) + thick, irw - col - (2 * thick), irh - (2 * thick), isbc);
      d->setCursor(col, isy - irh + (irh * cr) + ioy);
      d->print(itemtext[currentID][(int) data[currentID]]);
    }

  }
  else {

    data[currentID] += inc[currentID];
    if (data[currentID] > high[currentID]) {
      data[currentID] = low[currentID];
    }
    d->fillRect(col, isy - irh + (irh * cr) + thick, irw - col - (2 * thick), irh - (2 * thick), isbc);
    d->setCursor(col +  iox, isy - irh + (irh * cr) + ioy);
    d->print(data[currentID], dec[currentID]);
  }

  if (IconType[currentID] == ICON_MONO) {
    drawMonoBitmap(icox,  icoy + isy - irh + (irh * cr), itemBitmap[currentID], bmp_w[currentID], bmp_h[currentID], temptColor );
  }
  else if (IconType[currentID] == ICON_565) {
    draw565Bitmap(icox,  icoy + isy - irh + (irh * cr), item565Bitmap[currentID], bmp_w[currentID], bmp_h[currentID] );
  }
  delay(incdelay);
  value[currentID] = data[currentID];
  item = currentID;
}




template<typename T, typename F>
void TFTEditMenu<T,F>::incrementDown() {

  setFont(itemf);
  d->setTextColor(istc);
  if (haslist[currentID]) {
    if ((data[currentID] - inc[currentID]) >= low[currentID]) {
      data[currentID] -= inc[currentID];
      d->fillRect(col , isy - irh + (irh * cr) + thick, irw - col - (2 * thick), irh - (2 * thick), isbc);
      d->setCursor(col , isy - irh + (irh * cr) + ioy);
      d->print(itemtext[currentID][(int) data[currentID]]);
    }
    else {
      data[currentID] = high[currentID] - 1;
      d->fillRect(col , isy - irh + (irh * cr) + thick, irw - col - (2 * thick), irh - (2 * thick), isbc);
      d->setCursor(col, isy - irh + (irh * cr) + ioy);
      d->print(itemtext[currentID][(int) data[currentID]]);
    }
  }
  else {
    data[currentID] -= inc[currentID];
    if (data[currentID] < low[currentID]) {
      data[currentID] = high[currentID];
    }
    d->fillRect(col, isy - irh + (irh * cr) + thick, irw - col - (2 * thick), irh - (2 * thick), isbc);
    d->setCursor(col , isy - irh + (irh * cr) + ioy);
    d->print(data[currentID], dec[currentID]);
  }

  if (IconType[currentID] == ICON_MONO) {
    drawMonoBitmap(icox,  icoy + isy  - irh + (irh * cr), itemBitmap[currentID], bmp_w[currentID], bmp_h[currentID], temptColor );
  }
  else if (IconType[currentID] == ICON_565) {
    draw565Bitmap(icox,  icoy + isy  - irh + (irh * cr), item565Bitmap[currentID], bmp_w[currentID], bmp_h[currentID] );
  }
  delay(incdelay);
  value[currentID] = data[currentID];
  item = currentID;

}

template<typename T, typename F>
void TFTEditMenu<T,F>::setTitleColors( uint16_t TitleTextColor, uint16_t TitleFillColor) {
  ttc = TitleTextColor;
  tfc = TitleFillColor;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::setTitleBarSize(uint16_t TitleTop, uint16_t TitleLeft, uint16_t TitleWith, uint16_t TitleHeight) {
  tbt = TitleTop;
  tbl = TitleLeft;
  tbw = TitleWith;
  tbh = TitleHeight;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::setTitleText( char *TitleText,  char *ExitText) {
  strncpy(ttx, TitleText, MAX_CHAR_LEN);
  strncpy(etx, ExitText, MAX_CHAR_LEN);
}

template<typename T, typename F>
void TFTEditMenu<T,F>::setTitleTextMargins(uint16_t LeftMargin, uint16_t TopMargin) {
  tox = LeftMargin; // pixels to offset text in menu bar
  toy = TopMargin;  // pixels to offset text in menu bar
}


template<typename T, typename F>
void TFTEditMenu<T,F>::setMenuBarMargins(uint16_t LeftMargin, uint16_t Width, uint16_t BorderRadius, uint16_t BorderThickness) {
  isx = LeftMargin;  // pixels to offset text in menu bar
  irw = Width - isx;  // pixels to offset text in menu bar
  radius = BorderRadius;
  thick = BorderThickness;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::setItemColors( uint16_t DisableTextColor, uint16_t HighlightBorderColor, uint16_t SelectBorderColor) {
  ditc = DisableTextColor;
  bcolor = HighlightBorderColor;
  sbcolor = SelectBorderColor;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::SetAllColors(uint16_t TextColor, uint16_t BackgroundColor, 
							uint16_t HighlightTextColor, uint16_t HighlightColor, uint16_t HighlightBorderColor,
							uint16_t SelectedTextColor, uint16_t SelectedColor, uint16_t SelectBorderColor,
							uint16_t DisableTextColor ,	uint16_t TitleTextColor, uint16_t TitleFillColor){

	itc = TextColor;
	ibc = BackgroundColor;
	ihtc = HighlightTextColor;
	ihbc = HighlightColor;
	bcolor = HighlightBorderColor;
	istc = SelectedTextColor;
	isbc = SelectedColor;
	sbcolor = SelectBorderColor;
	ttc = TitleTextColor;
	tfc = TitleFillColor;
	ditc = DisableTextColor;
	
	}

template<typename T, typename F>
void TFTEditMenu<T,F>::setItemTextMargins(uint16_t LeftMargin, uint16_t TopMargin, uint16_t MenuMargin) {
  iox = LeftMargin;  // pixels to offset text in menu bar
  ioy = TopMargin;  // pixels to offset text in menu bar
  mm = MenuMargin;
  isy = tbt + tbh + mm;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::setItemText(int ID, const char *ItemText) {
  strcpy(itemlabel[ID], ItemText);
  drawRow(ID);
}

template<typename T, typename F>
void TFTEditMenu<T,F>::setIconMargins(uint16_t LeftMargin, uint16_t TopMargin) {
  icox = LeftMargin; // pixels to offset text in menu bar
  icoy = TopMargin;  // pixels to offset text in menu bar
}


template<typename T, typename F>
void TFTEditMenu<T,F>::disable(int ID) {
  enablestate[ID] = false;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::enable(int ID) {
  enablestate[ID] = true;
}

template<typename T, typename F>
bool TFTEditMenu<T,F>::getEnableState(int ID) {
  return enablestate[ID];
}

template<typename T, typename F>
void TFTEditMenu<T,F>::SetItemValue(int ID, float ItemValue) {
  value[ID] = ItemValue;
  data[ID] = ItemValue;
}

template<typename T, typename F>
void TFTEditMenu<T,F>::drawMonoBitmap(int16_t x, int16_t y, const unsigned char *bitmap, uint8_t w, uint8_t h, uint16_t color) {

  uint8_t sbyte = 0;
  uint8_t byteWidth = 0;
  int jj, ii;

  byteWidth = (w + 7) / 8;

  for (jj = 0; jj < h; jj++) {
    for (ii = 0; ii < w; ii++) {
      if (ii & 7)  sbyte <<= 1;
      else sbyte   = pgm_read_byte(bitmap + jj * byteWidth + ii / 8);
      if (sbyte & 0x80) d->drawPixel(x + ii, y + jj, color);
    }
  }

}

template<typename T, typename F>
void TFTEditMenu<T,F>::draw565Bitmap(int16_t x, int16_t y, const uint16_t *bitmap, uint8_t w, uint8_t h) {

  uint32_t offset = 0;

  int j, i;

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      d->drawPixel(j + x, i + y, pgm_read_word_near(bitmap + offset));
      offset++;
    }
  }

}

/*

  object type to create a simple selection only menu unlike previous where selecting a line item would allow in-line editing


*/



template<typename T, typename F>
TFTItemMenu<T,F>::TFTItemMenu(T *Display, bool EnableTouch) {
  d = Display;
  enabletouch = EnableTouch;
}

template<typename T, typename F>
void TFTItemMenu<T,F>::init(uint16_t TextColor, uint16_t BackgroundColor,
                    uint16_t HighlightTextColor, uint16_t HighlightColor,
                    uint16_t ItemRowHeight, uint16_t MaxRow,
                    const char *TitleText, F ItemFont, F TitleFont) {

  itc = TextColor;
  ibc = BackgroundColor;
  ihtc = HighlightTextColor;
  ihbc = HighlightColor;
  bcolor = TextColor;
  tbt = 0;      // title bar top
  tbl = 0;      // title bar left
  tbh = 40;     // title bar height
  tbw = d->width(); // title bar width, default to screen width
  tox = 20;     // x pixel offset for title text to offset from left a bit
  toy  = 10;        // y pixel offset for title text to manage height offset from top
  isx = 0;      // where to start the menu item from left, default indent 10
  icox = 0;
  icoy = 0;
  mm = 5;
  isy = tbt + tbh + mm; // where to start the menu item from top, note because array is 1 based we start drawing down a row--so back off a row, but pad 10
  irh = ItemRowHeight;  // select bar height
  irw = tbw - isx;    // select bar width, default to full width
  imr = MaxRow;     // user has to indicate this
  iox = 0;        // pixels to offset text in menu bar
  ioy = 0;        // pixels to offset text in menu bar
  itemf = ItemFont;     // item font
  titlef = TitleFont;   // menu bar font, default to item font
  ttc = TextColor;    // title text color
  tfc = HighlightColor; // title fill color
  ditc = MENU_C_DKGREY;
  strncpy(ttx, TitleText, MAX_CHAR_LEN);
  strncpy(etx, EXIT_TEXT, 6);
  item = 1;
  drawTitleFlag = true; // flag if we draw menu bar again
  currentID = 1;    // id of current highlighted or selected item (1 to ID)
  cr = 1;       // current selected row on the screen (1 to mr-1)
  totalID = 0;      // maximum number of items (where first position is 1)
  sr = 0;       // draw offset for the menu array where first postion is 0
  pr = 1;       // previous selected rown (1 to mr - 1)
  rowselected = false;
  enablestate[0] = true;
  redraw = true;
  radius = 0;
  thick = 0;

}

template<typename T, typename F>
int TFTItemMenu<T,F>::addNI(const char *ItemLabel) {

  totalID++;

  itemBitmap[totalID] = 0;
  bmp_w[totalID] = 0;
  bmp_h[totalID] = 0;
  IconType[totalID] = ICON_NONE;

  enablestate[totalID] = true;
  strncpy(itemlabel[totalID], ItemLabel, MAX_CHAR_LEN);
  return (totalID);

}

template<typename T, typename F>
int TFTItemMenu<T,F>::addMono(const char *ItemLabel, const unsigned char *Bitmap, uint8_t BitmapWidth, uint8_t BitmapHeight) {

  totalID++;

  itemBitmap[totalID] = Bitmap;
  
  bmp_w[totalID] = BitmapWidth;
  bmp_h[totalID] = BitmapHeight;
  IconType[totalID] = ICON_MONO;

  enablestate[totalID] = true;
  strncpy(itemlabel[totalID], ItemLabel, MAX_CHAR_LEN);
  return (totalID);
}

template<typename T, typename F>
int TFTItemMenu<T,F>::add565(const char *ItemLabel, const uint16_t *Bitmap, uint8_t BitmapWidth, uint8_t BitmapHeight) {

  totalID++;

  item565Bitmap[totalID] = Bitmap;
  bmp_w[totalID] = BitmapWidth;
  bmp_h[totalID] = BitmapHeight;
  IconType[totalID] = ICON_565;

  enablestate[totalID] = true;
  strncpy(itemlabel[totalID], ItemLabel, MAX_CHAR_LEN);
    
  return (totalID);
}

template<typename T, typename F>
void TFTItemMenu<T,F>::drawHeader(bool hl, uint8_t style) {

  setFont(titlef);

  if (enabletouch) {

    if (style == BUTTON_PRESSED) {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, ihbc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
    else {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, tfc);
      d->setTextColor(ttc);
      d->print(ttx);
    }

    if (totalID > imr) {
      // up arrow
      d->fillTriangle( tbl +  25 + 15 , tbt + tbh - 10, // bottom left
                       tbl +  25 - 15 , tbt + tbh - 10, // bottom right
                       tbl +  25    , tbt + 10, //center
                       ttc);
      // down arrow
      d->fillTriangle( tbw -  25 + 15 , tbt + 10, // top left
                       tbw -  25 - 15 , tbt + 10, // top right
                       tbw -  25    , tbt + tbh - 10, //center
                       ttc);
    }

  }
  else {
    if (hl) {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, ihbc);
      d->setTextColor(ihtc);
      d->print(etx);

      d->fillTriangle( tbw - (tbh / 3.0) - 5, tbt + (tbh / 2.0) - 3,
                       tbw - (tbh / 6.0) - 5, tbt + (tbh / 2.0) - (tbh / 3.0),
                       tbw - 5,       tbt + (tbh / 2.0) - 3,
                       ihtc);

      d->fillTriangle( tbw - (tbh / 3.0) - 5, tbt + (tbh / 2.0) + 3,
                       tbw - (tbh / 6.0) - 5, tbt + (tbh / 2.0) + (tbh / 3.0),
                       tbw - 5,       tbt + (tbh / 2.0) + 3,
                       ihtc);
    }

    else {
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, tfc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
  }

}



template<typename T, typename F>
int TFTItemMenu<T,F>::press(int16_t ScreenX, int16_t ScreenY) {

  int bs;

  // check if header pressed
  if (  (ScreenX > tbl) && (ScreenX < (tbw + tbl))  &&  (ScreenY > tbt ) && (ScreenY < (tbt + tbh) )) {

    if (ScreenX < (tbl + 25 + 15)) {
      // up arrow
      if (cr == 1) {
        cr = imr;
        //sr++;
      }

      MoveDown();
      return -1;
    }

    if (ScreenX > (tbw - 25 - 15)) {
      // down arrow
      if (cr <= imr) {
        cr = 0;
        //currentID = sr;
      }
      MoveUp();
      return -1;
    }

    return 0;
  }

  for (i = 1; i <= imr; i++) {
    bs = icox + bmp_w[i + sr] + isx;
    itx = bs + iox;

    if (
      (ScreenX > bs) &&
      (ScreenY > (isy + (irh * (i - 1)))) &&
      (ScreenX < (irw)) &&
      (ScreenY < (isy + irh + (irh * (i - 1))))
    ) {
    	if (!enablestate[i + sr]) {
      		return -1;
    	}

      	return i + sr;
    }
  }
  return -1;
}

template<typename T, typename F>
void TFTItemMenu<T,F>::drawRow(int ID, uint8_t style) {

  int  bs;
  int hr = ID - sr;

  if (ID == 0) {
    drawHeader(false, style);
  }

  if ((!enabletouch) || (ID <= 0)) {
    return;
  }

  bs = icox + bmp_w[hr] + isx;

  if (!enablestate[hr]) {
    return;
  }

  if (ID == 0) {
    if (style == BUTTON_PRESSED) {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, ihbc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
    else {
      // draw new menu bar
      d->setCursor(tbl + tox, tbt + toy);
      d->fillRect(tbl, tbt, tbw, tbh, tfc);
      d->setTextColor(ttc);
      d->print(ttx);
    }
  }
  else {


    if (style == BUTTON_PRESSED) {

      if (radius > 0) {
        d->fillRoundRect(bs,      isy - irh + (irh * hr) ,     irw - bs,       irh,        radius,   bcolor);
        d->fillRoundRect(bs + thick,  isy - irh + (irh * hr) + thick,  irw - bs - (2 * thick),  irh - (2 * thick),  radius,   ihbc);
      }
      else {
        d->fillRect(bs,       isy - irh + (irh * hr) ,     irw - bs  ,        irh, bcolor);
        d->fillRect(bs + thick ,  isy - irh + (irh * hr) + thick,  irw - bs - (2 * thick) , irh - (2 * thick), ihbc);
      }

    }
    else {

      d->fillRect(bs, isy - irh + (irh * hr) , irw - bs, irh, ibc); // back color

    }

    // write text
    itx = bs + iox;
    setFont(itemf);
    d->setTextColor(ihtc);
    d->setCursor(itx , isy - irh + (irh * hr) + ioy);
    d->print(itemlabel[ID]);

  }

}


template<typename T, typename F>
void TFTItemMenu<T,F>::draw() {
  drawHeader(false, 0);

  drawItems();
}

template<typename T, typename F>
void TFTItemMenu<T,F>::drawItems() {

  int  bs;

  if (imr > totalID) {
    imr = totalID;
  }
  redraw = false;
  // determine if we need to pan or just increment the list

  if ((currentID >  totalID) && (sr >= 0) ) {
    // up to top
    cr = 0;
    currentID = 0;
    sr = 0;
    redraw = true;
  }
  else if ( (cr < 0) && (sr == 0) ) {
    //  pan whole menu to bottom
    cr = imr;
    currentID = totalID;
    sr = totalID - imr;
    redraw = true;
  }
  else if ( (cr > imr) && ((sr + cr) > totalID) ) {
    //  pan whole menu to top
    cr = 1;
    currentID = 1;
    sr = 0;
    redraw = true;
  }
  else if ((cr > imr) && (sr >= 0))   {
    // scroll whole list up one by one
    sr++;
    cr = imr;
    redraw = true;
  }
  else if ((cr < 1) && (sr > 0))   {
    // scroll whole list down one by one
    sr--;
    cr = 1;
    redraw = true;
  }

  // determine if we need arrows
  if (cr == imr) {
    moredown = true;
  }
  if (cr == 1) {
    moreup = true;
  }

  if (enablestate[currentID] == false) {
    return;
  }

  // determine if we need to redraw the header and draw
  if ((currentID == 0) && (cr == 0)) {
    drawHeader(true, 0);
  }
  else if ((pr == 0) && (cr > 0)) {
    drawHeader(false, 0);
  }

  setFont(itemf);

  // now draw the items in the rows
  for (i = 1; i <= imr; i++) {
    // menu bar start
    bs = icox + bmp_w[i + sr] + isx;

    // text start
    itx = bs + iox;

    if (enablestate[i + sr]) {
      temptColor = itc;
    }
    else {
      temptColor = ditc;
    }

    if (redraw) {
      // scroll so blank out every row including icon since row will get scrolled
      d->fillRect(bs, isy - irh + (irh * i), irw - bs, irh, ibc); // back color
    }

    if (i == pr) {
      // maybe just row change so blank previous
      d->fillRect(bs, isy - irh + (irh * pr) , irw - bs, irh, ibc); // back color
    }

    if ((i == cr) && (!enabletouch)) {

      if (radius > 0) {
        d->fillRoundRect(bs,      isy - irh + (irh * i) ,      irw - bs,       irh,        radius,   bcolor);
        d->fillRoundRect(bs + thick,  isy - irh + (irh * i) + thick, irw - bs - (2 * thick),  irh - (2 * thick),  radius,   ihbc);
      }
      else {
        d->fillRect(bs,       isy - irh + (irh * i) ,      irw - bs  ,        irh, bcolor);
        d->fillRect(bs + thick ,  isy - irh + (irh * i) + thick, irw - bs - (2 * thick) , irh - (2 * thick), ihbc);
      }


      if (moreup) {
        d->fillTriangle(irw - 8 - thick,              isy - irh + (irh * i) + irh - (irh / TRIANGLE_H),
                        irw - ((irh / TRIANGLE_W)*.5) - 8 - thick, isy - irh + (irh * i) + (irh / TRIANGLE_H),
                        irw - (irh / TRIANGLE_W) - 8 - thick,    isy - irh + (irh * i) + irh - (irh / TRIANGLE_H), ihtc);
      }
      if (moredown) {
        d->fillTriangle(irw - 8 - thick,                 isy - irh + (irh * i) + (irh / TRIANGLE_H),
                        irw - ((irh / TRIANGLE_W)*.5) - 8 - thick, isy - irh + (irh * i) + irh - (irh / TRIANGLE_H),
                        irw - (irh / TRIANGLE_W) - 8 - thick,    isy - irh + (irh * i) + (irh / TRIANGLE_H),     ihtc);
      }
      temptColor = ihtc;

    }

    // write bitmap

    if (IconType[i + sr] == ICON_MONO) {
      drawMonoBitmap(icox,  icoy + isy - irh + (irh * i), itemBitmap[i + sr], bmp_w[i + sr], bmp_h[i + sr], itc );
    }
    else if (IconType[i + sr] == ICON_565) {
      draw565Bitmap(icox,  icoy + isy - irh + (irh * i), item565Bitmap[i + sr], bmp_w[i + sr], bmp_h[i + sr] );
    }

    // write text
    d->setTextColor(temptColor);
    d->setCursor(itx , isy - irh + (irh * i) + ioy);
    d->print(itemlabel[i + sr]);
  }

  moreup = false;
  moredown = false;
  pr = cr;

}

template<typename T, typename F>
void TFTItemMenu<T,F>::MoveUp() {

  cr--;

  currentID--;

  drawItems();

  if (enablestate[currentID] == false) {
    while (enablestate[currentID] == false) {
      cr--;
      currentID--;
      drawItems();
    }
  }
}

template<typename T, typename F>
void TFTItemMenu<T,F>::MoveDown() {

  cr++;
  currentID++;
  drawItems();
  if (enablestate[currentID] == false) {
    while (enablestate[currentID] == false) {
      cr++;
      currentID++;
      drawItems();
    }
  }

}

template<typename T, typename F>
int TFTItemMenu<T,F>::selectRow() {
  if (currentID == 0) {
    cr = 0;
    sr = 0;
  }
  item = currentID;
  return currentID;
}

template<typename T, typename F>
void TFTItemMenu<T,F>::setTitleColors( uint16_t TitleTextColor, uint16_t TitleFillColor) {
  ttc = TitleTextColor;
  tfc = TitleFillColor;

}

template<typename T, typename F>
void TFTItemMenu<T,F>::setTitleBarSize( uint16_t TitleTop, uint16_t TitleLeft, uint16_t TitleWith, uint16_t TitleHeight) {

  tbt = TitleTop;
  tbl = TitleLeft;
  tbw = TitleWith;
  tbh = TitleHeight;

}

template<typename T, typename F>
void TFTItemMenu<T,F>::setTitleText(char *TitleText, char *ExitText) {
  strncpy(ttx, TitleText, MAX_CHAR_LEN);
  strncpy(etx, ExitText, MAX_CHAR_LEN);
}

template<typename T, typename F>
void TFTItemMenu<T,F>::setTitleTextMargins(uint16_t LeftMargin, uint16_t TopMargin) {
  tox = LeftMargin;   // pixels to offset text in menu bar
  toy = TopMargin;   // pixels to offset text in menu bar
}

template<typename T, typename F>
void TFTItemMenu<T,F>::setMenuBarMargins(uint16_t LeftMargin, uint16_t Width, byte BorderRadius, byte BorderThickness) {
  isx = LeftMargin;  // pixels to offset text in menu bar
  irw = Width - isx;  // pixels to offset text in menu bar
  radius = BorderRadius;
  thick = BorderThickness;

}

template<typename T, typename F>
void TFTItemMenu<T,F>::setItemColors(uint16_t DisableTextColor, uint16_t BorderColor) {
  ditc = DisableTextColor;
  bcolor = BorderColor;
}


template<typename T, typename F>
void TFTItemMenu<T,F>::SetAllColors(uint16_t TextColor, uint16_t BackgroundColor, uint16_t HighlightTextColor, uint16_t HighlightColor, 
		uint16_t HighLightBorderColor, uint16_t DisableTextColor, uint16_t TitleTextColor, uint16_t TitleFillColor) {

	itc = TextColor;
	ibc = BackgroundColor;
	ihtc = HighlightTextColor;
	ihbc = HighlightColor;
	bcolor = HighLightBorderColor;
	ttc = TitleTextColor;
	tfc = TitleFillColor;
	ditc = DisableTextColor;
	
	}

template<typename T, typename F>
void TFTItemMenu<T,F>::setItemTextMargins(uint16_t LeftMargin, uint16_t TopMargin, uint16_t MenuMargin) {
  iox = LeftMargin;  // pixels to offset text in menu bar
  ioy = TopMargin;  // pixels to offset text in menu bar
  mm = MenuMargin;
  isy = tbt + tbh + mm;
}

template<typename T, typename F>
void TFTItemMenu<T,F>::setIconMargins(uint16_t IconOffsetX, uint16_t IconOffsetY) {
  icox = IconOffsetX; // pixels to offset text in menu bar
  icoy = IconOffsetY;  // pixels to offset text in menu bar
}

template<typename T, typename F>
void TFTItemMenu<T,F>::disable(int ID) {
  enablestate[ID] = false;
}

template<typename T, typename F>
void TFTItemMenu<T,F>::enable(int ID) {
  enablestate[ID] = true;
}

template<typename T, typename F>
bool TFTItemMenu<T,F>::getEnableState(int ID) {
  return enablestate[ID];
}


template<typename T, typename F>
void TFTItemMenu<T,F>::drawMonoBitmap(int16_t x, int16_t y, const unsigned char *bitmap, uint8_t w, uint8_t h, uint16_t color) {

  uint8_t sbyte = 0;
  uint8_t byteWidth = 0;
  int jj, ii;

  byteWidth = (w + 7) / 8;

  for (jj = 0; jj < h; jj++) {
    for (ii = 0; ii < w; ii++) {
      if (ii & 7)  sbyte <<= 1;
      else sbyte   = pgm_read_byte(bitmap + jj * byteWidth + ii / 8);
      if (sbyte & 0x80) d->drawPixel(x + ii, y + jj, color);
    }
  }

}

template<typename T, typename F>
void TFTItemMenu<T,F>::draw565Bitmap(int16_t x, int16_t y, const uint16_t *bitmap, uint8_t w, uint8_t h) {

  uint32_t offset = 0;

  int j, i;

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      d->drawPixel(j + x, i + y, pgm_read_word_near(bitmap + offset));
      offset++;
    }
  }

}

/////////////////////////////////
// end of this menu library
/////////////////////////////////
