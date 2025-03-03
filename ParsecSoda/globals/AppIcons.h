#pragma once

#include "../Texture.h"

typedef ID3D11ShaderResourceView* Icon;

class AppIcons
{
public:
	static void init(ID3D11Device* pd3dDevice);

	static ID3D11ShaderResourceView* play;
	static ID3D11ShaderResourceView* stop;

	static ID3D11ShaderResourceView* yes;
	static ID3D11ShaderResourceView* no;

	static ID3D11ShaderResourceView* send;
	static ID3D11ShaderResourceView* back;

	static ID3D11ShaderResourceView* lock;
	static ID3D11ShaderResourceView* unlock;

	static ID3D11ShaderResourceView* padOn;
	static ID3D11ShaderResourceView* padOff;
	static ID3D11ShaderResourceView* plug;

	static ID3D11ShaderResourceView* micOn;
	static ID3D11ShaderResourceView* micOff;
	static ID3D11ShaderResourceView* speakersOn;
	static ID3D11ShaderResourceView* speakersOff;

	static ID3D11ShaderResourceView* refresh;
	static ID3D11ShaderResourceView* sort;
	static ID3D11ShaderResourceView* submit;

	static ID3D11ShaderResourceView* chat;
	static ID3D11ShaderResourceView* users;
	static ID3D11ShaderResourceView* log;
	static ID3D11ShaderResourceView* logoff;

	static ID3D11ShaderResourceView* userOn;
	static ID3D11ShaderResourceView* userOff;
	static ID3D11ShaderResourceView* block;

	static ID3D11ShaderResourceView* logo;

private:
	static Texture _play;
	static Texture _stop;

	static Texture _yes;
	static Texture _no;
	
	static Texture _send;
	static Texture _back;

	static Texture _padOn;
	static Texture _padOff;
	static Texture _plug;

	static Texture _lock;
	static Texture _unlock;
	
	static Texture _micOn;
	static Texture _micOff;
	static Texture _speakersOn;
	static Texture _speakersOff;
	
	static Texture _refresh;
	static Texture _sort;
	static Texture _submit;

	static Texture _chat;
	static Texture _users;
	static Texture _log;
	static Texture _logoff;

	static Texture _userOn;
	static Texture _userOff;
	static Texture _block;

	static Texture _logo;
};

