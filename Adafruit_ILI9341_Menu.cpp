/*
  The MIT License (MIT)

  library writen by Kris Kasprzak

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  On a personal note, if you develop an application or product using this library
  and make millions of dollars, I'm happy for you!

  rev   date      author        change
  1.0   1/2022      kasprzak      initial code
  2.0   1/2022      kasprzak      added touch support
  3.0   1/2022      kasprzak      fixed font issue with MEGA
*/

#include <Adafruit_ILI9341_Menu.h>  

EditMenu::EditMenu(Adafruit_ILI9341 *Display, bool EnableTouch) : TFTEditMenu<Adafruit_ILI9341, const GFXfont*>(Display, EnableTouch) {
}

/*

  object type to create a simple selection only menu unlike previous where selecting a line item would allow in-line editing


*/



ItemMenu::ItemMenu(Adafruit_ILI9341 *Display, bool EnableTouch) : TFTItemMenu<Adafruit_ILI9341, const GFXfont*>(Display, EnableTouch) {
}
