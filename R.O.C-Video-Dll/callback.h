#pragma once

#ifndef CALLBACK_H
#define CALLBACK_H

// Callback called on new frame for unity

typedef void(*NewVideoFrameCallback) (int id, uint8_t* data, int width, int height);

// Callback call on client status change for unity

typedef void(*ClientStatusChangeCallback) (int id, bool status);

#endif