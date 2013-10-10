// Implementation
#include "WindowWin32.h"

void cWindowWin32::MoveCenter() {

}

void cWindowWin32::PeekAllMessages() {

}

void cWindowWin32::Close() {

}

void cWindowWin32::setCaptionText(const zsString& str) {

}

bool cWindowWin32::isOpened() const {
	return true;
}

bool cWindowWin32::isFullscreen() const {
	return false;
}

zsString cWindowWin32::getCaptionText() const {
	return zsString();
}

Vec2 cWindowWin32::getCenter() const {
	return Vec2();
}

uint32 cWindowWin32::getClientWidth() const {
	return 0;
}

uint32 cWindowWin32::getClientHeight() const {
	return 0;
}

float cWindowWin32::getClientAspectRatio() const {
	return 1.0f;
}