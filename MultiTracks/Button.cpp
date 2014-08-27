#include "Button.h"

namespace ww
{

Button::Button(const char* text) :
Widget("BUTTON", text, BS_DEFPUSHBUTTON)
{

}

void Button::OnCommand(int code)
{
	switch(code)
	{
	case BN_CLICKED:
		SignalClicked.emit();
		OnClicked();
		break;
	}
}

}