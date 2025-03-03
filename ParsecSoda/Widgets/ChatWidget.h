#pragma once

#include "ToggleIconButtonWidget.h"
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Hosting.h"

class ChatWidget
{
public:
	ChatWidget(Hosting& hosting);
	bool render();

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);

	#define LOG_BUFFER_LEN 16383
	#define SEND_BUFFER_LEN 1023

private:
	bool isDirty();
	void sendMessage();
	bool setSendBuffer(const char* value);

	// Dependency injection
	Hosting& _hosting;

	// Attributes
	string _logBuffer;
	char _sendBuffer[SEND_BUFFER_LEN];
	vector<string>& _chatLog;
	size_t _messageCount;
};

