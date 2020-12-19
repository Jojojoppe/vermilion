#include <vermilion/instance.hpp>

Vermilion::Core::Instance::Instance(int * hintType, int * hintValue){
	// Parse hints
	int platform_render = parseHintType_RENDER_PLATFORM(hintType, hintValue);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "RENDER_PLATFORM: %s", Vermilion::Core::RenderPlatformString[platform_render].c_str());
	int platform_window = parseHintType_WINDOW_PLATFORM(hintType, hintValue);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "WINDOW_PLATFORM: %s", Vermilion::Core::WindowPlatformString[platform_window].c_str());
	
}

Vermilion::Core::Instance::~Instance(){

}

int Vermilion::Core::Instance::parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue){
	int API = Vermilion::Core::renderPlatform[0];

	if(!hintType){
		return API;
	}

	while(*hintType){
	
		if((*hintType)==Vermilion::Core::HintType::HINT_TYPE_RENDER_PLATFORM){
			// Check if hint can be applied
			const int * rp = Vermilion::Core::renderPlatform;
			while(*rp){
				if(*rp == *hintType){
					// hint can be applied
					API = *rp;
					return API;
				}
				rp++;
			}
			// hint cannot be applied, choose first in possible values
			API = rp[0];
			return API;
		}

		hintType++;
		hintValue++;
	}
	return API;
}

int Vermilion::Core::Instance::parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue){
	int WINDOW = Vermilion::Core::windowPlatform[0];

	if(!hintType){
		return WINDOW;
	}

	while(*hintType){
	
		if((*hintType)==Vermilion::Core::HintType::HINT_TYPE_WINDOW_PLATFORM){
			// Check if hint can be applied
			const int * wp = Vermilion::Core::windowPlatform;
			while(*wp){
				if(*wp == *hintType){
					// hint can be applied
					WINDOW = *wp;
					return WINDOW;
				}
				wp++;
			}
			// hint cannot be applied, choose first in possible values
			WINDOW = wp[0];
			return WINDOW;
		}

		hintType++;
		hintValue++;
	}
	return WINDOW;
}

