/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "FrameBufferPlugin.h"

#include <plugin/common/Logs.h>

#include <brayns/common/ActionInterface.h>
#include <brayns/engineapi/Engine.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/Plugin.h>

namespace framebuffer
{
using namespace brayns;

const std::string PLUGIN_VERSION = "0.1.0";
const std::string PLUGIN_API_PREFIX = "fb_";

#define CATCH_STD_EXCEPTION()                  \
    catch (const std::runtime_error &e)        \
    {                                          \
        response.status = false;               \
        response.contents = e.what();          \
        PLUGIN_ERROR << e.what() << std::endl; \
    }

void _addDepthRenderer(brayns::Engine &engine)
{
    PLUGIN_INFO << "Registering 'depth' renderer" << std::endl;
    brayns::PropertyMap properties;
    properties.setProperty({"infinity", 1e6, 0., 1e6, {"Infinity"}});
    engine.addRendererType("depth", properties);
}

void _addAlbedoRenderer(brayns::Engine &engine)
{
    PLUGIN_INFO << "Registering 'albedo' renderer" << std::endl;
    brayns::PropertyMap properties;
    engine.addRendererType("albedo", properties);
}

void _addAmbientOcclusionRenderer(brayns::Engine &engine)
{
    PLUGIN_INFO << "Registering 'ambient occlusion' renderer" << std::endl;
    brayns::PropertyMap properties;
    properties.setProperty({"samplesPerFrame", 16, 1, 256, {"Samples per frame"}});
    properties.setProperty({"rayLength", 1e6, 1e-3, 1e6, {"Ray length"}});
    engine.addRendererType("ambient_occlusion", properties);
}

FrameBufferPlugin::FrameBufferPlugin()
    : ExtensionPlugin()
{
}

void FrameBufferPlugin::init()
{
    auto actionInterface = _api->getActionInterface();
    auto &engine = _api->getEngine();

    if (actionInterface)
    {
        std::string entryPoint = PLUGIN_API_PREFIX + "version";
        PLUGIN_INFO << "Registering '" + entryPoint + "' endpoint" << std::endl;
        actionInterface->registerRequest<Response>(entryPoint, [&]() { return _version(); });
    }

    _addDepthRenderer(engine);
    _addAlbedoRenderer(engine);
    _addAmbientOcclusionRenderer(engine);

    engine.addRendererType("raycast_Ng");
    engine.addRendererType("raycast_Ns");

    _api->getParametersManager().getRenderingParameters().setCurrentRenderer("albedo");
}

Response FrameBufferPlugin::_version() const
{
    Response response;
    response.contents = PLUGIN_VERSION;
    return response;
}

extern "C" ExtensionPlugin *brayns_plugin_create(int argc, char **argv)
{
    PLUGIN_INFO << std::endl;
    PLUGIN_INFO << "_|_|_|    _|                      _|            _|    _|            _|            " << std::endl;
    PLUGIN_INFO << "_|    _|  _|    _|_|_|    _|_|_|  _|  _|        _|    _|    _|_|    _|    _|_|    " << std::endl;
    PLUGIN_INFO << "_|_|_|    _|  _|    _|  _|        _|_|          _|_|_|_|  _|    _|  _|  _|_|_|_|  " << std::endl;
    PLUGIN_INFO << "_|    _|  _|  _|    _|  _|        _|  _|        _|    _|  _|    _|  _|  _|        " << std::endl;
    PLUGIN_INFO << "_|_|_|    _|    _|_|_|    _|_|_|  _|    _|      _|    _|    _|_|    _|    _|_|_|  " << std::endl;
    PLUGIN_INFO << std::endl;
    PLUGIN_INFO << "Initializing Black Hole plug-in (version " << PLUGIN_VERSION << ")" << std::endl;
    PLUGIN_INFO << std::endl;

    return new FrameBufferPlugin();
}

} // namespace framebuffer
