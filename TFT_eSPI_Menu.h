#ifndef TFT_ESPI_MENU_H
#define TFT_ESPI_MENU_H

#include <TFT_Menu.cpp>
#include <TFT_eSPI.h>
	
class  ESPIEditMenu : public TFTEditMenu<TFT_eSPI, uint8_t> {
public:
	ESPIEditMenu(TFT_eSPI *Display, bool EnableTouch = false) : TFTEditMenu<TFT_eSPI, uint8_t>(Display, EnableTouch) {}

protected:
	virtual void setFont(uint8_t font) {
		d->setTextFont(font);
	}
};

class  ESPIItemMenu : public TFTItemMenu<TFT_eSPI, uint8_t> {
public:
	ESPIItemMenu(TFT_eSPI *Display, bool EnableTouch = false)  : TFTItemMenu<TFT_eSPI, uint8_t>(Display, EnableTouch) {}

protected:
	virtual void setFont(uint8_t font) {
		d->setTextFont(font);
	}
};


#endif
