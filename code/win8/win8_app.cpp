#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>
#include "win8_app.h"


using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace concurrency;


/*
==================
main

==================
*/
[Platform::MTAThread]
int main( Platform::Array<Platform::String^>^ args )
{
	auto q3ApplicationSource = ref new Quake3Win8ApplicationSource();
	CoreApplication::Run(q3ApplicationSource);
	return 0;
}
