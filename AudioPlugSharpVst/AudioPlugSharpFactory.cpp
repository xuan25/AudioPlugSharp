#include "AudioPlugSharpFactory.h"
#include "AssemblyResolver.h"

using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace AudioPlugSharp;

#include <msclr/gcroot.h>

using namespace msclr;

gcroot<AudioPlugSharp::IAudioPlugin^> plugin;

AudioPlugSharpFactory::AudioPlugSharpFactory()
	: CPluginFactory(PFactoryInfo())
{
	System::String^ assemblyName = Path::GetFileNameWithoutExtension(Assembly::GetExecutingAssembly()->Location);

	// Our plugin should be our name but without the 'Vst' at the end
	assemblyName = assemblyName->Substring(0, assemblyName->Length - 3);

	Logger::Log("Plugin assembly name: " + assemblyName);

	Assembly^ pluginAssembly = AssemblyResolver::LoadAssembly(assemblyName);

	if (pluginAssembly == nullptr)
		return;

	plugin = nullptr;

	try
	{
		plugin = safe_cast<IAudioPlugin^>(AssemblyResolver::GetObjectByInterface(pluginAssembly, IAudioPlugin::typeid));
	}
	catch (Exception^ ex)
	{
		Logger::Log("Failed to cast pluginInfo: " + ex->ToString());

		return;
	}

	char* companyChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->Company);
	char* websiteChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->Website);
	char* contactChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->Contact);
	char* pluginNameChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->PluginName);
	char* pluginCategoryChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->PluginCategory);
	char* pluginVersionChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->PluginVerstion);
	char* processorGuidChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->ProcessorGuid);
	char* controllerGuidChars = (char*)(void*)Marshal::StringToHGlobalAnsi(plugin->ControllerGuid);

	AudioPlugSharpProcessor::AudioPlugSharpProcessorUID.fromString(processorGuidChars);
	AudioPlugSharpController::AudioPlugSharpControllerUID.fromString(controllerGuidChars);

	factoryInfo = PFactoryInfo(companyChars, websiteChars, contactChars, Vst::kDefaultFactoryFlags);

	static const PClassInfo2 componentClass
	(
		AudioPlugSharpProcessor::AudioPlugSharpProcessorUID,
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		pluginNameChars,
		0,
		pluginCategoryChars,
		companyChars,
		pluginVersionChars,
		kVstVersionString
	);

	registerClass(&componentClass, AudioPlugSharpProcessor::createInstance);

	static const PClassInfo2 controllerClass
	(
		AudioPlugSharpController::AudioPlugSharpControllerUID,
		PClassInfo::kManyInstances,
		kVstComponentControllerClass,
		pluginNameChars,
		0,
		"",
		companyChars,
		pluginVersionChars,
		kVstVersionString
	);

	Marshal::FreeHGlobal((IntPtr)companyChars);
	Marshal::FreeHGlobal((IntPtr)websiteChars);
	Marshal::FreeHGlobal((IntPtr)contactChars);
	Marshal::FreeHGlobal((IntPtr)pluginNameChars);
	Marshal::FreeHGlobal((IntPtr)pluginCategoryChars);
	Marshal::FreeHGlobal((IntPtr)pluginVersionChars);
	Marshal::FreeHGlobal((IntPtr)processorGuidChars);
	Marshal::FreeHGlobal((IntPtr)controllerGuidChars);

	registerClass(&controllerClass, AudioPlugSharpController::createInstance);
}
