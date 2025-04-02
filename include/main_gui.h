// NOT A PROJECT TO BE TAKEN SERIOUSLY
// GUI: NoesisGUI (https://www.noesisengine.com/)

#pragma once

#include "Noesis/NoesisPCH.h"
#include "Noesis/NsCore/Noesis.h"
#include "Noesis/NsGui/IntegrationAPI.h"
#include "Noesis/NsGui/XamlProvider.h"

#include "Noesis/NsApp/ApplicationLauncher.h"
#include "Noesis/NsApp/Application.h"
#include "Noesis/NsApp/ApplicationLauncherApi.h"
#include "Noesis/NsApp/EmbeddedXamlProvider.h"
#include "Noesis/NsApp/ProvidersApi.h"

namespace context {
    class App final: public NoesisApp::Application
    {
        NS_IMPLEMENT_INLINE_REFLECTION_(App, NoesisApp::Application, "RssReader.App")
    };
} // namespace context